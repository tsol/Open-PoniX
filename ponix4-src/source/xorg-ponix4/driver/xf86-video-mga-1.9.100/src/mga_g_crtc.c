/*
 * Copyright 2007 Tilman Sauerbeck
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software")
 * to deal in the software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * them Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTIBILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Tilman Sauerbeck <tilman@code-monkey.de>
 *
 * Sources:
 *    xf86-video-intel, mga_dacG.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* All drivers should typically include these */
#include "xf86.h"
#include "xf86_OSproc.h"

/* Drivers for PCI hardware need this */
#include "xf86PciInfo.h"

/* Drivers that need to access the PCI config space directly need this */
#include "xf86Pci.h"

#include "mga_reg.h"
#include "mga.h"
#include "mga_macros.h"
#include "mga_dacG.h"

/*
 * Only change bits shown in this mask.  Ideally reserved bits should be
 * zeroed here.  Also, don't change the vgaioen bit here since it is
 * controlled elsewhere.
 *
 * XXX These settings need to be checked.
 */
#define OPTION1_MASK	0xFFFFFEFF
#define OPTION2_MASK	0xFFFFFFFF
#define OPTION3_MASK	0xFFFFFFFF

#define OPTION1_MASK_PRIMARY	0xFFFC0FF

typedef struct {
    unsigned char ExtVga[6];
    unsigned char DacRegs[0x50];
    CARD32 Option;
    CARD32 Option2;
    CARD32 Option3;
    Bool PIXPLLCSaved;
    long clock;
} MgaCrtcStateRec, *MgaCrtcStatePtr;

typedef struct {
    MgaCrtcStateRec saved_state;
} MgaCrtcDataRec, *MgaCrtcDataPtr;

static void crtc_dpms(xf86CrtcPtr crtc, int mode);
static void crtc_save(xf86CrtcPtr crtc);
static void crtc_restore(xf86CrtcPtr crtc);
static Bool crtc_lock(xf86CrtcPtr crtc);
static void crtc_unlock(xf86CrtcPtr crtc);
static Bool crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
                            DisplayModePtr adjusted_mode);
static void crtc_prepare(xf86CrtcPtr crtc);
static void crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
                          DisplayModePtr adjusted_mode, int x, int y);
static void crtc_commit(xf86CrtcPtr crtc);
static void crtc_destroy(xf86CrtcPtr crtc);

static const xf86CrtcFuncsRec crtc_funcs = {
    .dpms = crtc_dpms,
    .save = crtc_save,
    .restore = crtc_restore,
    .lock = crtc_lock,
    .unlock = crtc_unlock,
    .mode_fixup = crtc_mode_fixup,
    .prepare = crtc_prepare,
    .mode_set = crtc_mode_set,
    .commit = crtc_commit,
    .destroy = crtc_destroy
};

static void
crtc_dpms(xf86CrtcPtr crtc, int mode)
{
}

static void
MGAG200SEComputePLLParam(long lFo, int *M, int *N, int *P)
{
    unsigned int ulComputedFo;
    unsigned int ulFDelta;
    unsigned int ulFPermitedDelta;
    unsigned int ulFTmpDelta;
    unsigned int ulVCOMax, ulVCOMin;
    unsigned int ulTestP;
    unsigned int ulTestM;
    unsigned int ulTestN;
    unsigned int ulPLLFreqRef;

    ulVCOMax        = 320000;
    ulVCOMin        = 160000;
    ulPLLFreqRef    = 25000;

    ulFDelta = 0xFFFFFFFF;
    /* Permited delta is 0.5% as VESA Specification */
    ulFPermitedDelta = lFo * 5 / 1000;  

    /* Then we need to minimize the M while staying within 0.5% */
    for (ulTestP = 8; ulTestP > 0; ulTestP >>= 1) {
	if ((lFo * ulTestP) > ulVCOMax) continue;
	if ((lFo * ulTestP) < ulVCOMin) continue;

	for (ulTestN = 17; ulTestN <= 256; ulTestN++) {
	    for (ulTestM = 1; ulTestM <= 32; ulTestM++) {
		ulComputedFo = (ulPLLFreqRef * ulTestN) / (ulTestM * ulTestP);
		if (ulComputedFo > lFo)
		    ulFTmpDelta = ulComputedFo - lFo;
		else
		    ulFTmpDelta = lFo - ulComputedFo;

		if (ulFTmpDelta < ulFDelta) {
		    ulFDelta = ulFTmpDelta;
		    *M = ulTestM - 1;
		    *N = ulTestN - 1;
		    *P = ulTestP - 1;
		}
	    }
	}
    }
}


/**
 * Calculate the PLL settings (m, n, p, s).
 *
 * For more information, refer to the Matrox "MGA1064SG Developer
 * Specification" (document 10524-MS-0100).  chapter 5.7.8. "PLLs Clocks
 * Generators"
 *
 * \param f_out   Desired clock frequency, measured in kHz.
 * \param best_m  Value of PLL 'm' register.
 * \param best_n  Value of PLL 'n' register.
 * \param p       Value of PLL 'p' register.
 * \param s       Value of PLL 's' filter register (pix pll clock only).
 */

static void
MGAGCalcClock(xf86CrtcPtr crtc, long f_out,
              int *best_m, int *best_n, int *p, int *s)
{
	MGAPtr pMga = MGAPTR(crtc->scrn);
	int m, n;
	double f_vco;
	double m_err, calc_f;
	const double ref_freq = (double) pMga->bios.pll_ref_freq;
	int feed_div_min, feed_div_max;
	int in_div_min, in_div_max;
	int post_div_max;
	
	switch( pMga->Chipset )
	{
	case PCI_CHIP_MGA1064:
		feed_div_min = 100;
		feed_div_max = 127;
		in_div_min   = 1;
		in_div_max   = 31;
		post_div_max = 7;
		break;
	case PCI_CHIP_MGAG400:
	case PCI_CHIP_MGAG550:
		feed_div_min = 7;
		feed_div_max = 127;
		in_div_min   = 1;
		in_div_max   = 31;
		post_div_max = 7;
		break;
	case PCI_CHIP_MGAG200_SE_A_PCI:
	case PCI_CHIP_MGAG200_SE_B_PCI:
	case PCI_CHIP_MGAG100:
	case PCI_CHIP_MGAG100_PCI:
	case PCI_CHIP_MGAG200:
	case PCI_CHIP_MGAG200_PCI:
	default:
		feed_div_min = 7;
		feed_div_max = 127;
		in_div_min   = 1;
		in_div_max   = 6;
		post_div_max = 7;
		break;
	}

	/* Make sure that f_min <= f_out */
	if ( f_out < ( pMga->bios.pixel.min_freq / 8))
		f_out = pMga->bios.pixel.min_freq / 8;

	/*
	 * f_pll = f_vco / (p+1)
	 * Choose p so that 
	 * pMga->bios.pixel.min_freq <= f_vco <= pMga->bios.pixel.max_freq
	 * we don't have to bother checking for this maximum limit.
	 */
	f_vco = ( double ) f_out;
	for ( *p = 0; *p <= post_div_max && f_vco < pMga->bios.pixel.min_freq;
		*p = *p * 2 + 1, f_vco *= 2.0);

	/* Initial amount of error for frequency maximum */
	m_err = f_out;

	/* Search for the different values of ( m ) */
	for ( m = in_div_min ; m <= in_div_max ; m++ )
	{
		/* see values of ( n ) which we can't use */
		for ( n = feed_div_min; n <= feed_div_max; n++ )
		{ 
			calc_f = ref_freq * (n + 1) / (m + 1) ;

			/*
			 * Pick the closest frequency.
			 */
			if ( abs(calc_f - f_vco) < m_err ) {
				m_err = abs(calc_f - f_vco);
				*best_m = m;
				*best_n = n;
			}
		}
	}
	
	/* Now all the calculations can be completed */
	f_vco = ref_freq * (*best_n + 1) / (*best_m + 1);

	/* Adjustments for filtering pll feed back */
	if ( (50000.0 <= f_vco)
	&& (f_vco < 100000.0) )
		*s = 0;	
	if ( (100000.0 <= f_vco)
	&& (f_vco < 140000.0) )
		*s = 1;	
	if ( (140000.0 <= f_vco)
	&& (f_vco < 180000.0) )
		*s = 2;	
	if ( (180000.0 <= f_vco) )
		*s = 3;	

#ifdef DEBUG
	xf86DrvMsg(pScrn->scrnIndex, X_INFO,
		   "f_out_requ =%ld f_pll_real=%.1f f_vco=%.1f n=0x%x m=0x%x p=0x%x s=0x%x\n",
		   f_out, (f_vco / (*p + 1)), f_vco, *best_n, *best_m, *p, *s );
#endif
}

/*
 * MGAGSetPCLK - Set the pixel (PCLK) clock.
 */
static void
MGAGSetPCLK(xf86CrtcPtr crtc, MgaCrtcStatePtr state, long f_out)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);

    /* Pixel clock values */
    int m, n, p, s;

    if (MGAISGx50(pMga)) {
        state->clock = f_out;
        return;
    }

    if (pMga->is_G200SE) {
        MGAG200SEComputePLLParam(f_out, &m, &n, &p);

        state->DacRegs[MGA1064_PIX_PLLC_M] = m;
        state->DacRegs[MGA1064_PIX_PLLC_N] = n;
        state->DacRegs[MGA1064_PIX_PLLC_P] = p;
    } else {
        /* Do the calculations for m, n, p and s */
        MGAGCalcClock(crtc, f_out, &m, &n, &p, &s );

        /* Values for the pixel clock PLL registers */
        state->DacRegs[MGA1064_PIX_PLLC_M] = m & 0x1F;
        state->DacRegs[MGA1064_PIX_PLLC_N] = n & 0x7F;
        state->DacRegs[MGA1064_PIX_PLLC_P] = (p & 0x07) |
                                             ((s & 0x03) << 3);
    }
}

static void
state_set(xf86CrtcPtr crtc, MgaCrtcStatePtr state,
          DisplayModePtr mode, int x, int y)
{
    /*
     * initial values of the DAC registers
     */
    const static unsigned char initDAC[] = {
        /* 0x00: */	   0,    0,    0,    0,    0,    0, 0x00,    0,
        /* 0x08: */	   0,    0,    0,    0,    0,    0,    0,    0,
        /* 0x10: */	   0,    0,    0,    0,    0,    0,    0,    0,
        /* 0x18: */	0x00,    0, 0xC9, 0xFF, 0xBF, 0x20, 0x1F, 0x20,
        /* 0x20: */	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* 0x28: */	0x00, 0x00, 0x00, 0x00,    0,    0,    0, 0x40,
        /* 0x30: */	0x00, 0xB0, 0x00, 0xC2, 0x34, 0x14, 0x02, 0x83,
        /* 0x38: */	0x00, 0x93, 0x00, 0x77, 0x00, 0x00, 0x00, 0x3A,
        /* 0x40: */	   0,    0,    0,    0,    0,    0,    0,    0,
        /* 0x48: */	   0,    0,    0,    0,    0,    0,    0,    0
    };

    int i;
    int hd, hs, he, ht, vd, vs, ve, vt, wd;
    int BppShift;
    MGAPtr pMga = MGAPTR(crtc->scrn);
    vgaRegPtr vga = &VGAHWPTR(crtc->scrn)->ModeReg;
    unsigned int startadd = (y * crtc->scrn->virtualX) + x;

    BppShift = pMga->BppShifts[(crtc->scrn->bitsPerPixel >> 3) - 1];

    for (i = 0; i < sizeof(state->DacRegs); i++)
        state->DacRegs[i] = initDAC[i];

    switch (pMga->Chipset) {
    case PCI_CHIP_MGA1064:
        state->DacRegs[MGA1064_SYS_PLL_M] = 0x04;
        state->DacRegs[MGA1064_SYS_PLL_N] = 0x44;
        state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
        state->Option = 0x5F094F21;
        state->Option2 = 0x00000000;
        break;
    case PCI_CHIP_MGAG100:
    case PCI_CHIP_MGAG100_PCI:
        state->DacRegs[MGA1064_VREF_CTL] = 0x03;

        if (pMga->HasSDRAM) {
            if (pMga->OverclockMem) {
                /* 220 Mhz */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x06;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x38;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
            } else {
                /* 203 Mhz */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x01;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x0E;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
            }

            state->Option = 0x404991a9;
        } else {
            if (pMga->OverclockMem) {
                /* 143 Mhz */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x06;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x24;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x10;
            } else {
                /* 124 Mhz */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x04;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x16;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x08;
            }

            state->Option = 0x4049d121;
        }

        state->Option2 = 0x0000007;
        break;
    case PCI_CHIP_MGAG400:
    case PCI_CHIP_MGAG550:
        if (MGAISGx50(pMga))
            break;

        if (pMga->Dac.maxPixelClock == 360000) { /* G400 MAX */
            if (pMga->OverclockMem) {
                /* 150/200 */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x05;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x42;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
                state->Option3 = 0x019B8419;
                state->Option = 0x50574120;
            } else {
                /* 125/166 */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x02;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x1B;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
                state->Option3 = 0x019B8419;
                state->Option = 0x5053C120;
            }
        } else {
            if (pMga->OverclockMem) {
                /* 125/166 */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x02;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x1B;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x18;
                state->Option3 = 0x019B8419;
                state->Option = 0x5053C120;
            } else {
                /* 110/166 */
                state->DacRegs[MGA1064_SYS_PLL_M] = 0x13;
                state->DacRegs[MGA1064_SYS_PLL_N] = 0x7A;
                state->DacRegs[MGA1064_SYS_PLL_P] = 0x08;
                state->Option3 = 0x0190a421;
                state->Option = 0x50044120;
            }
        }

        if (pMga->HasSDRAM)
            state->Option &= ~(1 << 14);

        state->Option2 = 0x01003000;
        break;
    case PCI_CHIP_MGAG200_SE_A_PCI:
    case PCI_CHIP_MGAG200_SE_B_PCI:
        state->DacRegs[MGA1064_VREF_CTL] = 0x03;
        state->DacRegs[MGA1064_PIX_CLK_CTL] =
            MGA1064_PIX_CLK_CTL_SEL_PLL;

        state->DacRegs[MGA1064_MISC_CTL] =
            MGA1064_MISC_CTL_DAC_EN |
            MGA1064_MISC_CTL_VGA8 |
            MGA1064_MISC_CTL_DAC_RAM_CS;

        if (pMga->HasSDRAM)
            state->Option = 0x40049120;

        state->Option2 = 0x00008000;
        break;
    case PCI_CHIP_MGAG200:
    case PCI_CHIP_MGAG200_PCI:
    default:
        if (pMga->OverclockMem) {
            /* 143 Mhz */
            state->DacRegs[MGA1064_SYS_PLL_M] = 0x06;
            state->DacRegs[MGA1064_SYS_PLL_N] = 0x24;
            state->DacRegs[MGA1064_SYS_PLL_P] = 0x10;
        } else {
            /* 124 Mhz */
            state->DacRegs[MGA1064_SYS_PLL_M] = 0x04;
            state->DacRegs[MGA1064_SYS_PLL_N] = 0x2D;
            state->DacRegs[MGA1064_SYS_PLL_P] = 0x19;
        }

        state->Option2 = 0x00008000;

        if (pMga->HasSDRAM)
            state->Option = 0x40499121;
        else
            state->Option = 0x4049cd21;

        break;
    }

    /* must always have the pci retries on but rely on
       polling to keep them from occuring */
    state->Option &= ~0x20000000;

    switch (crtc->scrn->bitsPerPixel) {
    case 8:
        state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_8bits;
        startadd /= 8;
        break;
    case 16:
        state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_16bits;

        if ((crtc->scrn->weight.red == 5) && (crtc->scrn->weight.green == 5)
            && (crtc->scrn->weight.blue == 5)) {
            state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_15bits;
        }

        startadd /= 4;
        break;
    case 24:
        state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_24bits;
        startadd /= 8;
        break;
    case 32:
        if (pMga->Overlay8Plus24) {
            state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_32bits;
            state->DacRegs[MGA1064_COL_KEY_MSK_LSB] = 0xFF;
            state->DacRegs[MGA1064_COL_KEY_LSB] = pMga->colorKey;
        } else
            state->DacRegs[MGA1064_MUL_CTL] = MGA1064_MUL_CTL_32_24bits;

        startadd /= 2;
        break;
    default:
        FatalError("MGA: unsupported depth\n");
    }

    /* we only have 20 bits to store the start address */
    startadd &= 0xfffff;

    /*
     * This will initialize all of the generic VGA registers.
     */
    if (!vgaHWInit(crtc->scrn, mode)) {
        ErrorF("oh noes, vgahwinit failed\n");
        return;
    }

    /*
     * Here all of the MGA registers get filled in.
     */
    hd = (mode->HDisplay >> 3) - 1;
    hs = (mode->HSyncStart >> 3) - 1;
    he = (mode->HSyncEnd >> 3) - 1;
    ht = (mode->HTotal >> 3) - 1;
    vd = mode->VDisplay - 1;
    vs = mode->VSyncStart - 1;
    ve = mode->VSyncEnd - 1;
    vt = mode->VTotal - 2;

    /* HTOTAL & 0x7 equal to 0x6 in 8bpp or 0x4 in 24bpp causes strange
     * vertical stripes
     */
    if ((ht & 0x07) == 0x06 || (ht & 0x07) == 0x04)
        ht++;

    if (crtc->scrn->bitsPerPixel == 24)
        wd = (crtc->scrn->displayWidth * 3) >> (4 - BppShift);
    else
        wd = crtc->scrn->displayWidth >> (4 - BppShift);

    state->ExtVga[0] = 0;
    state->ExtVga[5] = 0;

    if (mode->Flags & V_INTERLACE) {
        state->ExtVga[0] = 0x80;
        state->ExtVga[5] = (hs + he - ht) >> 1;
        wd <<= 1;
        vt &= 0xFFFE;
    }

    state->ExtVga[0] |= (wd & 0x300) >> 4;
    state->ExtVga[0] |= (startadd >> 16) & 0x0f;

    state->ExtVga[1] = (((ht - 4) & 0x100) >> 8) |
                       ((hd & 0x100) >> 7) |
                       ((hs & 0x100) >> 6) |
                       (ht & 0x40);
    state->ExtVga[2] = ((vt & 0xc00) >> 10) |
                       ((vd & 0x400) >> 8) |
                       ((vd & 0xc00) >> 7) |
                       ((vs & 0xc00) >> 5) |
                       ((vd & 0x400) >> 3); /* linecomp */

    if (crtc->scrn->bitsPerPixel == 24)
        state->ExtVga[3] = (((1 << BppShift) * 3) - 1) | 0x80;
    else
        state->ExtVga[3] = ((1 << BppShift) - 1) | 0x80;

    state->ExtVga[4] = 0;

    vga->CRTC[0] = ht - 4;
    vga->CRTC[1] = hd;
    vga->CRTC[2] = hd;
    vga->CRTC[3] = (ht & 0x1F) | 0x80;
    vga->CRTC[4] = hs;
    vga->CRTC[5] = ((ht & 0x20) << 2) | (he & 0x1F);
    vga->CRTC[6] = vt & 0xFF;
    vga->CRTC[7] = ((vt & 0x100) >> 8 ) |
                   ((vd & 0x100) >> 7 ) |
                   ((vs & 0x100) >> 6 ) |
                   ((vd & 0x100) >> 5 ) |
                   ((vd & 0x100) >> 4 ) | /* linecomp */
                   ((vt & 0x200) >> 4 ) |
                   ((vd & 0x200) >> 3 ) |
                   ((vs & 0x200) >> 2 );
    vga->CRTC[9] = ((vd & 0x200) >> 4) |
                   ((vd & 0x200) >> 3); /* linecomp */

    vga->CRTC[12] = (startadd & 0xff00) >> 8;
    vga->CRTC[13] = startadd & 0xff;

    vga->CRTC[16] = vs & 0xFF;
    vga->CRTC[17] = (ve & 0x0F) | 0x20;
    vga->CRTC[18] = vd & 0xFF;
    vga->CRTC[19] = wd & 0xFF;
    vga->CRTC[21] = vd & 0xFF;
    vga->CRTC[22] = (vt + 1) & 0xFF;
    vga->CRTC[24] = vd & 0xFF; /* linecomp */

    state->DacRegs[MGA1064_CURSOR_BASE_ADR_LOW] = pMga->FbCursorOffset >> 10;
    state->DacRegs[MGA1064_CURSOR_BASE_ADR_HI] = pMga->FbCursorOffset >> 18;

    if (pMga->SyncOnGreen) {
        state->DacRegs[MGA1064_GEN_CTL] &=
            ~MGA1064_GEN_CTL_SYNC_ON_GREEN_DIS;

        state->ExtVga[3] |= 0x40;
    }

    /* select external clock */
    vga->MiscOutReg |= 0x0C;

    if (mode->Flags & V_DBLSCAN)
        vga->CRTC[9] |= 0x80;

    if (MGAISGx50(pMga)) {
        OUTREG(MGAREG_ZORG, 0);
    }

    MGAGSetPCLK(crtc, state, mode->Clock);

    /* This disables the VGA memory aperture */
    vga->MiscOutReg &= ~0x02;

#if X_BYTE_ORDER == X_BIG_ENDIAN
    /* Disable byte-swapping for big-endian architectures - the XFree
       driver seems to like a little-endian framebuffer -ReneR */
    /* state->Option |= 0x80000000; */
    state->Option &= ~0x80000000;
#endif
}

/*
 * This function restores a video mode. It basically writes out all of
 * the registers that have previously been saved in the MgaCrtcStateRec
 * data structure.
 */
static void
state_restore(xf86CrtcPtr crtc, MgaCrtcStatePtr state,
              vgaRegPtr vga, int vga_flags)
{
    ScrnInfoPtr scrn = crtc->scrn;
    MGAPtr pMga = MGAPTR(scrn);
    CARD32 optionMask;
    int i;

    /*
     * Pixel Clock needs to be restored regardless if we use
     * HALLib or not. HALlib doesn't do a good job restoring
     * VESA modes. MATROX: hint, hint.
     *
     * FIXME: This seems weird. Verify.
     */
    if (MGAISGx50(pMga) && state->clock) {
        MGAG450SetPLLFreq(scrn, MGA_PIXEL_PLL, state->clock);
        state->PIXPLLCSaved = FALSE;
    }

    /* Do not set the memory config for primary cards as it
       should be correct already. Only on little endian architectures
       since we need to modify the byteswap bit. -ReneR */
#if X_BYTE_ORDER == X_BIG_ENDIAN
    optionMask = OPTION1_MASK;
#else
    optionMask = (pMga->Primary) ? OPTION1_MASK_PRIMARY : OPTION1_MASK;
#endif

    /*
     * Code is needed to get things back to bank zero.
     */

    /* restore DAC registers
     * according to the docs we shouldn't write to reserved regs
     */
    for (i = 0; i < sizeof(state->DacRegs); i++) {
        if ((i <= 0x03) ||
                (i == 0x07) ||
                (i == 0x0b) ||
                (i == 0x0f) ||
                ((i >= 0x13) && (i <= 0x17)) ||
                (i == 0x1b) ||
                (i == 0x1c) ||
                ((i >= 0x1f) && (i <= 0x29)) ||
                ((i >= 0x30) && (i <= 0x37)) ||
                (MGAISGx50(pMga) && !state->PIXPLLCSaved &&
                 ((i == 0x2c) || (i == 0x2d) || (i == 0x2e) ||
                  (i == 0x4c) || (i == 0x4d) || (i == 0x4e))))
            continue;
        if (pMga->is_G200SE
                && ((i == 0x2C) || (i == 0x2D) || (i == 0x2E)))
            continue;
        outMGAdac(i, state->DacRegs[i]);
    }

    if (!MGAISGx50(pMga)) {
        /* restore pci_option register */
#ifdef XSERVER_LIBPCIACCESS
        pci_device_cfg_write_bits(pMga->PciInfo, optionMask,
                                  state->Option, PCI_OPTION_REG);

        if (pMga->Chipset != PCI_CHIP_MGA1064)
            pci_device_cfg_write_bits(pMga->PciInfo, OPTION2_MASK,
                                      state->Option2, PCI_MGA_OPTION2);

        if (pMga->Chipset == PCI_CHIP_MGAG400 || pMga->Chipset == PCI_CHIP_MGAG550)
            pci_device_cfg_write_bits(pMga->PciInfo, OPTION3_MASK,
                                      state->Option3, PCI_MGA_OPTION3);
#else
        pciSetBitsLong(pMga->PciTag, PCI_OPTION_REG, optionMask,
                       state->Option);

        if (pMga->Chipset != PCI_CHIP_MGA1064)
            pciSetBitsLong(pMga->PciTag, PCI_MGA_OPTION2, OPTION2_MASK,
                           state->Option2);

        if (pMga->Chipset == PCI_CHIP_MGAG400 || pMga->Chipset == PCI_CHIP_MGAG550)
            pciSetBitsLong(pMga->PciTag, PCI_MGA_OPTION3, OPTION3_MASK,
                           state->Option3);
#endif
    }

    /* restore CRTCEXT regs */
    for (i = 0; i < 6; i++)
        OUTREG16(MGAREG_CRTCEXT_INDEX, (state->ExtVga[i] << 8) | i);

    /* This handles restoring the generic VGA registers. */
    if (pMga->is_G200SE) {
        MGAG200SERestoreMode(scrn, vga);

        if (vga_flags & VGA_SR_FONTS)
            MGAG200SERestoreFonts(scrn, vga);
    } else
        vgaHWRestore(scrn, vga, vga_flags & ~VGA_SR_CMAP);

    if (vga_flags & VGA_SR_CMAP)
        MGAGRestorePalette(scrn, vga->DAC);

    /*
     * this is needed to properly restore start address
     */
    OUTREG16(MGAREG_CRTCEXT_INDEX, (state->ExtVga[0] << 8) | 0);

#ifdef DEBUG
    ErrorF("Setting DAC:");
    for (i = 0; i < sizeof(state->DacRegs); i++) {
#if 1
        if(!(i%16)) ErrorF("\n%02X: ",i);
        ErrorF("%02X ", mgaReg->DacRegs[i]);
#else
        if(!(i%8)) ErrorF("\n%02X: ",i);
        ErrorF("0x%02X, ", mgaReg->DacRegs[i]);
#endif
    }
    ErrorF("\nOPTION  = %08lX\n", mgaReg->Option);
    ErrorF("OPTION2 = %08lX\n", mgaReg->Option2);
    ErrorF("CRTCEXT:");
    for (i=0; i<6; i++) ErrorF(" %02X", state->ExtVga[i]);
    ErrorF("\n");
#endif
}

static void
state_save(xf86CrtcPtr crtc, MgaCrtcStatePtr state, int vga_flags)
{
    ScrnInfoPtr scrn = crtc->scrn;
    MGAPtr pMga = MGAPTR(scrn);
    vgaRegPtr vga = &VGAHWPTR(scrn)->SavedReg;
    int i;

    if (MGAISGx50(pMga))
        state->clock = MGAG450SavePLLFreq(scrn, MGA_PIXEL_PLL);

    /*
     * Code is needed to get back to bank zero.
     */
    OUTREG16(MGAREG_CRTCEXT_INDEX, 0x0004);

    /*
     * This function will handle creating the data structure and filling
     * in the generic VGA portion.
     */
    if (pMga->is_G200SE) {
        MGAG200SESaveMode(scrn, vga);

        if (vga_flags & VGA_SR_FONTS)
            MGAG200SESaveFonts(scrn, vga);
    } else
        vgaHWSave(scrn, vga, vga_flags);

    MGAGSavePalette(scrn, vga->DAC);

    /*
     * The port I/O code necessary to read in the extended registers.
     */
    for (i = 0; i < sizeof(state->DacRegs); i++)
        state->DacRegs[i] = inMGAdac(i);

    state->PIXPLLCSaved = TRUE;


#ifdef XSERVER_LIBPCIACCESS
    pci_device_cfg_read_u32(pMga->PciInfo, &state->Option,
                            PCI_OPTION_REG);
    pci_device_cfg_read_u32(pMga->PciInfo, &state->Option2,
                            PCI_MGA_OPTION2);

    if (pMga->Chipset == PCI_CHIP_MGAG400 || pMga->Chipset == PCI_CHIP_MGAG550)
        pci_device_cfg_read_u32(pMga->PciInfo, &state->Option3,
                                PCI_MGA_OPTION3);
#else
    state->Option = pciReadLong(pMga->PciTag, PCI_OPTION_REG);
    state->Option2 = pciReadLong(pMga->PciTag, PCI_MGA_OPTION2);

    if (pMga->Chipset == PCI_CHIP_MGAG400 || pMga->Chipset == PCI_CHIP_MGAG550)
        state->Option3 = pciReadLong(pMga->PciTag, PCI_MGA_OPTION3);
#endif

    for (i = 0; i < 6; i++) {
        OUTREG8(MGAREG_CRTCEXT_INDEX, i);
        state->ExtVga[i] = INREG8(MGAREG_CRTCEXT_DATA);
    }

#ifdef DEBUG
    ErrorF("Saved values:\nDAC:");
    for (i = 0; i < sizeof(state->DacRegs); i++) {
#if 1
        if(!(i%16)) ErrorF("\n%02X: ",i);
        ErrorF("%02X ", state->DacRegs[i]);
#else
        if(!(i%8)) ErrorF("\n%02X: ",i);
        ErrorF("0x%02X, ", state->DacRegs[i]);
#endif
    }

    ErrorF("\nOPTION  = %08lX\n:", state->Option);
    ErrorF("OPTION2 = %08lX\nCRTCEXT:", state->Option2);

    for (i=0; i<6; i++) ErrorF(" %02X", state->ExtVga[i]);
        ErrorF("\n");
#endif
}

static void
crtc_save(xf86CrtcPtr crtc)
{
    MgaCrtcDataPtr data = MGACRTCDATAPTR(crtc);
    MGAPtr pMga = MGAPTR(crtc->scrn);
    int vga_flags = VGA_SR_MODE;

    if (pMga->Primary)
        vga_flags |= VGA_SR_FONTS;

    state_save(crtc, &data->saved_state, vga_flags);
}

static void
crtc_restore(xf86CrtcPtr crtc)
{
    MgaCrtcDataPtr data = MGACRTCDATAPTR(crtc);
    MGAPtr pMga = MGAPTR(crtc->scrn);
    vgaHWPtr vga = VGAHWPTR(crtc->scrn);
    int vga_flags = VGA_SR_MODE | VGA_SR_CMAP;

    if (pMga->Primary)
        vga_flags |= VGA_SR_FONTS;

    state_restore(crtc, &data->saved_state, &vga->SavedReg, vga_flags);
}

static Bool
crtc_lock(xf86CrtcPtr crtc)
{
#ifdef XF86DRI
    return MGADRILock(crtc->scrn);
#else
    return FALSE;
#endif
}

static void
crtc_unlock(xf86CrtcPtr crtc)
{
#ifdef XF86DRI
    MGADRIUnlock(crtc->scrn);
#endif
}

static Bool
crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
                DisplayModePtr adjusted_mode)
{
    return TRUE;
}

static void
crtc_prepare(xf86CrtcPtr crtc)
{
    crtc->funcs->dpms(crtc, DPMSModeOff);
}

static void
crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
              DisplayModePtr adjust_mode, int x, int y)
{
    MgaCrtcStateRec state;
    vgaHWPtr vga = VGAHWPTR(crtc->scrn);

    memset(&state, 0, sizeof (state));

    state_set(crtc, &state, mode, x, y);
    state_restore(crtc, &state, &vga->ModeReg, VGA_SR_MODE);
}

static void
crtc_commit(xf86CrtcPtr crtc)
{
    crtc->funcs->dpms(crtc, DPMSModeOn);
}

static void
crtc_destroy(xf86CrtcPtr crtc)
{
    if (crtc->driver_private)
        xfree (crtc->driver_private);
}

Bool
MgaGCrtc1Init(ScrnInfoPtr scrn)
{
    xf86CrtcPtr crtc;
    MgaCrtcDataPtr data;

    data = xnfcalloc (sizeof (MgaCrtcDataRec), 1);
    if (!data)
        return FALSE;

    crtc = xf86CrtcCreate (scrn, &crtc_funcs);
    if (!crtc) {
        xfree(data);
        return FALSE;
    }

    crtc->driver_private = data;

    return TRUE;
}
