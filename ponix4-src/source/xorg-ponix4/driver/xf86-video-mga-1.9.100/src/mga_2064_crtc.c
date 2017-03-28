/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/mga/mga_dac3026.c,v 1.58tsi Exp $ */
/*
 * Copyright 1994 by Robin Cutshaw <robin@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Robin Cutshaw not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Robin Cutshaw makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ROBIN CUTSHAW DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ROBIN CUTSHAW BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * Modified for TVP3026 by Harald Koenig <koenig@tat.physik.uni-tuebingen.de>
 *
 * Modified for MGA Millennium by Xavier Ducoin <xavier@rd.lectra.fr>
 *
 * Doug Merritt <doug@netcom.com>
 * 24bpp: fixed high res stripe glitches, clock glitches on all res
 *
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
#include "mga_dac3026.h"

/*
 * Only change these bits in the Option register.  Make sure that the
 * vgaioen bit is never in this mask because it is controlled elsewhere
 */
#define OPTION_MASK 0xFFEFFEFF	/* ~(eepromwt | vgaioen) */


#define DACREGSIZE 21
#define PALETTE_SIZE (256 * 3)

typedef struct {
    unsigned char ExtVga[6];
    unsigned char DacClk[6];
    unsigned char DacRegs[DACREGSIZE];
    CARD32 Option;
} MgaCrtcStateRec, *MgaCrtcStatePtr;

typedef struct {
    MgaCrtcStateRec saved_state;
} MgaCrtcDataRec, *MgaCrtcDataPtr;

static void crtc_dpms(xf86CrtcPtr crtc, int mode);
static void crtc_save(xf86CrtcPtr crtc);
static void crtc_restore(xf86CrtcPtr crtc);
static void crtc_prepare(xf86CrtcPtr crtc);
static void crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
                          DisplayModePtr adjusted_mode, int x, int y);
static void crtc_commit(xf86CrtcPtr crtc);
static void crtc_destroy(xf86CrtcPtr crtc);

static const xf86CrtcFuncsRec crtc_funcs = {
    .dpms = crtc_dpms,
    .save = crtc_save,
    .restore = crtc_restore,
    .prepare = crtc_prepare,
    .mode_set = crtc_mode_set,
    .commit = crtc_commit,
    .destroy = crtc_destroy
};

/*
 * indexes to ti3026 registers (the order is important)
 */
const static unsigned char MGADACregs[DACREGSIZE] = {
	0x0F, 0x18, 0x19, 0x1A, 0x1C,   0x1D, 0x1E, 0x2A, 0x2B, 0x30,
	0x31, 0x32, 0x33, 0x34, 0x35,   0x36, 0x37, 0x38, 0x39, 0x3A,
	0x06
};

/* note: to fix a cursor hw glitch, register 0x37 (blue color key) needs
   to be set to magic numbers, even though they are "never" used because
   blue keying disabled in 0x38.

   Matrox sez:

   ...The more precise statement of the software workaround is to insure
   that bits 7-5 of register 0x37 (Blue Color Key High) and bits 7-5 of
   register 0x38 (HZOOM)are the same...
*/

/* also note: the values of the MUX control register 0x19 (index [2]) can be
   found in table 2-17 of the 3026 manual. If interlace is set, the values
   listed here are incremented by one.
*/

/*
 * initial values of ti3026 registers
 */
const static unsigned char MGADACbpp8[DACREGSIZE] = {
	0x06, 0x80, 0x4b, 0x25, 0x00,   0x00, 0x0C, 0x00, 0x1E, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0x00, 0x00,    0, 0x00,
	0x00
};
const static unsigned char MGADACbpp16[DACREGSIZE] = {
	0x07, 0x45, 0x53, 0x15, 0x00,   0x00, 0x2C, 0x00, 0x1E, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0x00, 0x10,    0, 0x00,
	0x00
};
/*
 * [0] value was 0x07, but changed to 0x06 by Doug Merrit to fix high res
 * stripe glitches and clock glitches at 24bpp.
 */
/* [0] value is now set inside of MGA3026Init, based on the silicon revision
   It is still set to 7 or 6 based on the revision, though, since setting to
   8 as in the documentation makes (some?) revB chips get the colors wrong...
   maybe BGR instead of RGB? This only applies to 24bpp, since it is the only
   one documented as depending on revision.
 */

const static unsigned char MGADACbpp24[DACREGSIZE] = {
	0x06, 0x56, 0x5b, 0x25, 0x00,   0x00, 0x2C, 0x00, 0x1E, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0x00, 0x10,    0, 0x00,
	0x00
};
const static unsigned char MGADACbpp32[DACREGSIZE] = {
	0x07, 0x46, 0x5b, 0x05, 0x00,   0x00, 0x2C, 0x00, 0x1E, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0x00, 0x10,    0, 0x00,
	0x00
};

/* on at least some 2064Ws, the PSEL line flips at 4MB or so, so PSEL keying
   has to be off in register 0x1e -> bit4 clear */

const static unsigned char MGADACbpp8plus24[DACREGSIZE] = {
	0x07, 0x06, 0x5b, 0x05, 0x00,   0x00, 0x2C, 0x00, 0x1E, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,   0xFF, 0x00, 0x01, 0x00, 0x00,
	0x00
};

/*
 * MGATi3026CalcClock - Calculate the PLL settings (m, n, p).
 *
 * DESCRIPTION
 *   For more information, refer to the Texas Instruments
 *   "TVP3026 Data Manual" (document SLAS098B).
 *     Section 2.4 "PLL Clock Generators"
 *     Appendix A "Frequency Synthesis PLL Register Settings"
 *     Appendix B "PLL Programming Examples"
 *
 * PARAMETERS
 *   f_out		IN	Desired clock frequency.
 *   f_max		IN	Maximum allowed clock frequency.
 *   m			OUT	Value of PLL 'm' register.
 *   n			OUT	Value of PLL 'n' register.
 *   p			OUT	Value of PLL 'p' register.
 *
 * HISTORY
 *   January 11, 1997 - [aem] Andrew E. Mileski
 *   Split off from MGATi3026SetClock.
 */

/* The following values are in kHz */
#define TI_MIN_VCO_FREQ  110000
#define TI_MAX_VCO_FREQ  220000
#define TI_MAX_MCLK_FREQ 100000
#define TI_REF_FREQ      14318.18

static double
MGATi3026CalcClock (long f_out, long f_max, int *m, int *n, int *p)
{
	int best_m = 0, best_n = 0;
	double f_pll, f_vco;
	double m_err, inc_m, calc_m;

	/* Make sure that f_min <= f_out <= f_max */
	if ( f_out < ( TI_MIN_VCO_FREQ / 8 ))
		f_out = TI_MIN_VCO_FREQ / 8;
	if ( f_out > f_max )
		f_out = f_max;

	/*
	 * f_pll = f_vco / 2 ^ p
	 * Choose p so that TI_MIN_VCO_FREQ <= f_vco <= TI_MAX_VCO_FREQ
	 * Note that since TI_MAX_VCO_FREQ = 2 * TI_MIN_VCO_FREQ
	 * we don't have to bother checking for this maximum limit.
	 */
	f_vco = ( double ) f_out;
	for ( *p = 0; *p < 3 && f_vco < TI_MIN_VCO_FREQ; ( *p )++ )
		f_vco *= 2.0;

	/*
	 * We avoid doing multiplications by ( 65 - n ),
	 * and add an increment instead - this keeps any error small.
	 */
	inc_m = f_vco / ( TI_REF_FREQ * 8.0 );

	/* Initial value of calc_m for the loop */
	calc_m = inc_m + inc_m + inc_m;

	/* Initial amount of error for an integer - impossibly large */
	m_err = 2.0;

	/* Search for the closest INTEGER value of ( 65 - m ) */
	for ( *n = 3; *n <= 25; ( *n )++, calc_m += inc_m ) {

		/* Ignore values of ( 65 - m ) which we can't use */
		if ( calc_m < 3.0 || calc_m > 64.0 )
			continue;

		/*
		 * Pick the closest INTEGER (has smallest fractional part).
		 * The optimizer should clean this up for us.
		 */
		if (( calc_m - ( int ) calc_m ) < m_err ) {
			m_err = calc_m - ( int ) calc_m;
			best_m = ( int ) calc_m;
			best_n = *n;
		}
	}

	/* 65 - ( 65 - x ) = x */
	*m = 65 - best_m;
	*n = 65 - best_n;

	/* Now all the calculations can be completed */
	f_vco = 8.0 * TI_REF_FREQ * best_m / best_n;
	f_pll = f_vco / ( 1 << *p );

#ifdef DEBUG
	ErrorF( "f_out=%ld f_pll=%.1f f_vco=%.1f n=%d m=%d p=%d\n",
		f_out, f_pll, f_vco, *n, *m, *p );
#endif

	return f_pll;
}

/*
 * MGATi3026SetMCLK - Set the memory clock (MCLK) PLL.
 *
 * HISTORY
 *   January 11, 1997 - [aem] Andrew E. Mileski
 *   Written and tested.
 */
static void
MGATi3026SetMCLK( ScrnInfoPtr scrn, long f_out )
{
	int mclk_m, mclk_n, mclk_p;
	int pclk_m, pclk_n, pclk_p;
	int mclk_ctl;
	MGAPtr pMga = MGAPTR(scrn);

	MGATi3026CalcClock(f_out, TI_MAX_MCLK_FREQ, &mclk_m, &mclk_n, &mclk_p);

	/* Save PCLK settings */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfc );
	pclk_n = inTi3026( TVP3026_PIX_CLK_DATA );
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfd );
	pclk_m = inTi3026( TVP3026_PIX_CLK_DATA );
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfe );
	pclk_p = inTi3026( TVP3026_PIX_CLK_DATA );

	/* Stop PCLK (PLLEN = 0, PCLKEN = 0) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfe );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, 0x00 );

	/* Set PCLK to the new MCLK frequency (PLLEN = 1, PCLKEN = 0 ) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfc );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, ( mclk_n & 0x3f ) | 0xc0 );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, mclk_m & 0x3f );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, ( mclk_p & 0x03 ) | 0xb0 );

	/* Wait for PCLK PLL to lock on frequency */
	while (( inTi3026( TVP3026_PIX_CLK_DATA ) & 0x40 ) == 0 ) {
		;
	}

	/* Output PCLK on MCLK pin */
	mclk_ctl = inTi3026( TVP3026_MCLK_CTL );
	outTi3026( TVP3026_MCLK_CTL, 0, mclk_ctl & 0xe7 );
	outTi3026( TVP3026_MCLK_CTL, 0, ( mclk_ctl & 0xe7 ) | 0x08 );

	/* Stop MCLK (PLLEN = 0 ) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfb );
	outTi3026( TVP3026_MEM_CLK_DATA, 0, 0x00 );

	/* Set MCLK to the new frequency (PLLEN = 1) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xf3 );
	outTi3026( TVP3026_MEM_CLK_DATA, 0, ( mclk_n & 0x3f ) | 0xc0 );
	outTi3026( TVP3026_MEM_CLK_DATA, 0, mclk_m & 0x3f );
	outTi3026( TVP3026_MEM_CLK_DATA, 0, ( mclk_p & 0x03 ) | 0xb0 );

	/* Wait for MCLK PLL to lock on frequency */
	while (( inTi3026( TVP3026_MEM_CLK_DATA ) & 0x40 ) == 0 ) {
		;
	}

	/* Output MCLK PLL on MCLK pin */
	outTi3026( TVP3026_MCLK_CTL, 0, ( mclk_ctl & 0xe7 ) | 0x10 );
	outTi3026( TVP3026_MCLK_CTL, 0, ( mclk_ctl & 0xe7 ) | 0x18 );

	/* Stop PCLK (PLLEN = 0, PCLKEN = 0 ) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfe );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, 0x00 );

	/* Restore PCLK (PLLEN = ?, PCLKEN = ?) */
	outTi3026( TVP3026_PLL_ADDR, 0, 0xfc );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, pclk_n );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, pclk_m );
	outTi3026( TVP3026_PIX_CLK_DATA, 0, pclk_p );

	/* Wait for PCLK PLL to lock on frequency */
	while (( inTi3026( TVP3026_PIX_CLK_DATA ) & 0x40 ) == 0 ) {
		;
	}
}

/*
 * state_set_pclk - Set the pixel (PCLK) and loop (LCLK) clocks.
 *
 * PARAMETERS
 *   f_pll			IN	Pixel clock PLL frequencly in kHz.
 *   bpp			IN	Bytes per pixel.
 *
 * HISTORY
 *   January 11, 1997 - [aem] Andrew E. Mileski
 *   Split to simplify code for MCLK (=GCLK) setting.
 *
 *   December 14, 1996 - [aem] Andrew E. Mileski
 *   Fixed loop clock to be based on the calculated, not requested,
 *   pixel clock. Added f_max = maximum f_vco frequency.
 *
 *   October 19, 1996 - [aem] Andrew E. Mileski
 *   Commented the loop clock code (wow, I understand everything now),
 *   and simplified it a bit. This should really be two separate functions.
 *
 *   October 1, 1996 - [aem] Andrew E. Mileski
 *   Optimized the m & n picking algorithm. Added maxClock detection.
 *   Low speed pixel clock fix (per the docs). Documented what I understand.
 *
 *   ?????, ??, ???? - [???] ????????????
 *   Based on the TVP3026 code in the S3 driver.
 */

static void
state_set_pclk(ScrnInfoPtr scrn, MgaCrtcStatePtr state, long f_out, int bpp)
{
    /* Pixel clock values */
    int m, n, p;

    /* Loop clock values */
    int lm, ln, lp, lq;
    double z;

    /* The actual frequency output by the clock */
    double f_pll;

    long f_max = TI_MAX_VCO_FREQ;

    MGAPtr pMga = MGAPTR(scrn);

    /* Get the maximum pixel clock frequency */
    if (pMga->MaxClock > TI_MAX_VCO_FREQ)
        f_max = pMga->MaxClock;

    /* Do the calculations for m, n, and p */
    f_pll = MGATi3026CalcClock( f_out, f_max, & m, & n, & p );

    /* Values for the pixel clock PLL registers */
    state->DacClk[ 0 ] = ( n & 0x3f ) | 0xc0;
    state->DacClk[ 1 ] = ( m & 0x3f );
    state->DacClk[ 2 ] = ( p & 0x03 ) | 0xb0;

	/*
	 * Now that the pixel clock PLL is setup,
	 * the loop clock PLL must be setup.
	 */

	/*
	 * First we figure out lm, ln, and z.
	 * Things are different in packed pixel mode (24bpp) though.
	 */
	 if (scrn->bitsPerPixel == 24) {

		/* ln:lm = ln:3 */
		lm = 65 - 3;

		/* Check for interleaved mode */
		if ( bpp == 2 )
			/* ln:lm = 4:3 */
			ln = 65 - 4;
		else
			/* ln:lm = 8:3 */
			ln = 65 - 8;

		/* Note: this is actually 100 * z for more precision */
		z = ( 11000 * ( 65 - ln )) / (( f_pll / 1000 ) * ( 65 - lm ));
	}
	else {
		/* ln:lm = ln:4 */
		lm = 65 - 4;

		/* Note: bpp = bytes per pixel */
		ln = 65 - 4 * ( 64 / 8 ) / bpp;

		/* Note: this is actually 100 * z for more precision */
		z = (( 11000 / 4 ) * ( 65 - ln )) / ( f_pll / 1000 );
	}

	/*
	 * Now we choose dividers lp and lq so that the VCO frequency
	 * is within the operating range of 110 MHz to 220 MHz.
	 */

	/* Assume no lq divider */
	lq = 0;

	/* Note: z is actually 100 * z for more precision */
	if ( z <= 200.0 )
		lp = 0;
	else if ( z <= 400.0 )
		lp = 1;
	else if ( z <= 800.0 )
		lp = 2;
	else if ( z <= 1600.0 )
		lp = 3;
	else {
		lp = 3;
		lq = ( int )( z / 1600.0 );
	}

        /* Values for the loop clock PLL registers */
        if (scrn->bitsPerPixel == 24 ) {
            /* Packed pixel mode values */
            state->DacClk[ 3 ] = ( ln & 0x3f ) | 0x80;
            state->DacClk[ 4 ] = ( lm & 0x3f ) | 0x80;
            state->DacClk[ 5 ] = ( lp & 0x03 ) | 0xf8;
        } else {
            /* Non-packed pixel mode values */
            state->DacClk[ 3 ] = ( ln & 0x3f ) | 0xc0;
            state->DacClk[ 4 ] = ( lm & 0x3f );
            state->DacClk[ 5 ] = ( lp & 0x03 ) | 0xf0;
        }
        state->DacRegs[ 18 ] = lq | 0x38;

#ifdef DEBUG
	ErrorF( "bpp=%d z=%.1f ln=%d lm=%d lp=%d lq=%d\n",
		bpp, z, ln, lm, lp, lq );
#endif
}

static void
state_set(xf86CrtcPtr crtc, MgaCrtcStatePtr state, DisplayModePtr mode)
{
    ScrnInfoPtr scrn = crtc->scrn;
    int hd, hs, he, ht, vd, vs, ve, vt, wd;
    int i, BppShift, index_1d = 0;
    const unsigned char* initDAC;
    MGAPtr pMga = MGAPTR(scrn);
    MGARamdacPtr MGAdac = &pMga->Dac;
    vgaRegPtr pVga = &VGAHWPTR(scrn)->ModeReg;

    BppShift = pMga->BppShifts[(scrn->bitsPerPixel >> 3) - 1];

    switch (scrn->bitsPerPixel) {
    case 8:
        initDAC = MGADACbpp8;
        break;
    case 16:
        initDAC = MGADACbpp16;
        break;
    case 24:
        initDAC = MGADACbpp24;
        break;
    case 32:
        if (pMga->Overlay8Plus24)
            initDAC = MGADACbpp8plus24;
        else
            initDAC = MGADACbpp32;

        break;
    default:
        FatalError("MGA: unsupported bits per pixel\n");
    }

    for (i = 0; i < DACREGSIZE; i++) {
        state->DacRegs[i] = initDAC[i];

        if (MGADACregs[i] == 0x1D)
            index_1d = i;
    }

    if ((scrn->bitsPerPixel == 32) && pMga->Overlay8Plus24) {
        state->DacRegs[9] = pMga->colorKey;
        state->DacRegs[10] = pMga->colorKey;
    }

    if ((scrn->bitsPerPixel == 16) && (scrn->weight.red == 5) &&
        (scrn->weight.green == 5) && (scrn->weight.blue == 5))
        state->DacRegs[1] &= ~0x01;

    if (pMga->Interleave)
        state->DacRegs[2] += 1;

    if (scrn->bitsPerPixel == 24) {
        int silicon_rev;

        /* we need to set DacRegs[0] differently based on the silicon
         * revision of the 3026 RAMDAC, as per page 2-14 of tvp3026.pdf.
         * If we have rev A silicon, we want 0x07; rev B silicon wants
         * 0x06.
         */
        silicon_rev = inTi3026(TVP3026_SILICON_REV);

#ifdef DEBUG
        ErrorF("TVP3026 revision 0x%x (rev %s)\n",
               silicon_rev, (silicon_rev <= 0x20) ? "A" : "B");
#endif

        if (silicon_rev <= 0x20)
            state->DacRegs[0] = 0x07; /* rev A */
        else
            state->DacRegs[0] = 0x06; /* rev B */
    }

    /*
     * This will initialize all of the generic VGA registers.
     */
    if (!vgaHWInit(scrn, mode))
        return;

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

    if (scrn->bitsPerPixel == 24)
        wd = (scrn->displayWidth * 3) >> (4 - BppShift);
    else
        wd = scrn->displayWidth >> (4 - BppShift);

    state->ExtVga[0] = 0;
    state->ExtVga[5] = 0;

    if (mode->Flags & V_INTERLACE) {
        state->ExtVga[0] = 0x80;
        state->ExtVga[5] = (hs + he - ht) >> 1;

        wd <<= 1;
        vt &= 0xFFFE;

        /* enable interlaced cursor */
        state->DacRegs[20] |= 0x20;
    }

    state->ExtVga[0] |= (wd & 0x300) >> 4;
    state->ExtVga[1] = (((ht - 4) & 0x100) >> 8) |
                       ((hd & 0x100) >> 7) |
                       ((hs & 0x100) >> 6) |
                       (ht & 0x40);
    state->ExtVga[2] = ((vt & 0xc00) >> 10) |
                       ((vd & 0x400) >> 8) |
                       ((vd & 0xc00) >> 7) |
                       ((vs & 0xc00) >> 5);

    if (scrn->bitsPerPixel == 24)
        state->ExtVga[3] = (((1 << BppShift) * 3) - 1) | 0x80;
    else
        state->ExtVga[3] = ((1 << BppShift) - 1) | 0x80;

    /* Set viddelay (CRTCEXT3 Bits 3-4). */
    state->ExtVga[3] |= (scrn->videoRam == 8192 ? 0x10
                        : scrn->videoRam == 2048 ? 0x08 : 0x00);

    state->ExtVga[4] = 0;

    pVga->CRTC[0] = ht - 4;
    pVga->CRTC[1] = hd;
    pVga->CRTC[2] = hd;
    pVga->CRTC[3] = (ht & 0x1F) | 0x80;
    pVga->CRTC[4] = hs;
    pVga->CRTC[5] = ((ht & 0x20) << 2) | (he & 0x1F);
    pVga->CRTC[6] = vt & 0xFF;
    pVga->CRTC[7] = ((vt & 0x100) >> 8 ) |
                    ((vd & 0x100) >> 7 ) |
                    ((vs & 0x100) >> 6 ) |
                    ((vd & 0x100) >> 5 ) |
                    0x10 |
                    ((vt & 0x200) >> 4 ) |
                    ((vd & 0x200) >> 3 ) |
                    ((vs & 0x200) >> 2 );
    pVga->CRTC[9] = ((vd & 0x200) >> 4) | 0x40;
    pVga->CRTC[16] = vs & 0xFF;
    pVga->CRTC[17] = (ve & 0x0F) | 0x20;
    pVga->CRTC[18] = vd & 0xFF;
    pVga->CRTC[19] = wd & 0xFF;
    pVga->CRTC[21] = vd & 0xFF;
    pVga->CRTC[22] = (vt + 1) & 0xFF;

    if (mode->Flags & V_DBLSCAN)
        pVga->CRTC[9] |= 0x80;

    /* Per DDK vid.c line 75, sync polarity should be controlled
     * via the TVP3026 RAMDAC register 1D and so MISC Output Register
     * should always have bits 6 and 7 set.
     */

    pVga->MiscOutReg |= 0xC0;

    if ((mode->Flags & (V_PHSYNC | V_NHSYNC)) &&
        (mode->Flags & (V_PVSYNC | V_NVSYNC))) {
        if (mode->Flags & V_PHSYNC)
            state->DacRegs[index_1d] |= 0x01;
        if (mode->Flags & V_PVSYNC)
            state->DacRegs[index_1d] |= 0x02;
    } else {
        int VDisplay = mode->VDisplay;

        if (mode->Flags & V_DBLSCAN)
            VDisplay *= 2;
        if (VDisplay < 400)
            state->DacRegs[index_1d] |= 0x01; /* +hsync -vsync */
        else if (VDisplay < 480)
            state->DacRegs[index_1d] |= 0x02; /* -hsync +vsync */
        else if (VDisplay < 768)
            state->DacRegs[index_1d] |= 0x00; /* -hsync -vsync */
        else
            state->DacRegs[index_1d] |= 0x03; /* +hsync +vsync */
    }

    if (pMga->SyncOnGreen)
        state->DacRegs[index_1d] |= 0x20;

    state->Option = 0x402C0100;  /* fine for 2064 and 2164 */

    if (pMga->Interleave)
        state->Option |= 0x1000;
    else
        state->Option &= ~0x1000;

    /* must always have the pci retries on but rely on
     * polling to keep them from occuring
     */
    state->Option &= ~0x20000000;

    pVga->MiscOutReg |= 0x0C;

    /* XXX Need to check the first argument */
    state_set_pclk(scrn, state, mode->Clock, 1 << BppShift);

    /* FIXME
     * this one writes registers rather than writing to the
     * mgaReg->ModeReg and letting Restore write to the hardware
     * but that's no big deal since we will Restore right after
     * this function.
     */

    MGATi3026SetMCLK(scrn, MGAdac->MemoryClock);

#ifdef DEBUG
    ErrorF("%6ld: %02X %02X %02X	%02X %02X %02X	%08lX\n",
           mode->Clock, state->DacClk[0], state->DacClk[1],
           state->DacClk[2], state->DacClk[3], state->DacClk[4],
           state->DacClk[5], state->Option);

    for (i = 0; i < sizeof(MGADACregs); i++)
        ErrorF("%02X ", state->DacRegs[i]);

    for (i = 0; i < 6; i++)
        ErrorF(" %02X", state->ExtVga[i]);

    ErrorF("\n");
#endif

    /* This disables the VGA memory aperture */
    pVga->MiscOutReg &= ~0x02;
}

/*
 * This function restores a video mode. It basically writes out all of
 * the registers that have previously been saved in the vgaMGARec data
 * structure.
 */
static void
state_restore(xf86CrtcPtr crtc, MgaCrtcStatePtr state,
              vgaRegPtr vga, int vga_flags)
{
    ScrnInfoPtr scrn = crtc->scrn;
    MGAPtr pMga = MGAPTR(scrn);
    int i;

    /*
     * Code is needed to get things back to bank zero.
     */
    for (i = 0; i < 6; i++)
        OUTREG16(0x1FDE, (state->ExtVga[i] << 8) | i);

#ifdef XSERVER_LIBPCIACCESS
    pci_device_cfg_write_bits(pMga->PciInfo, OPTION_MASK,
                              state->Option, PCI_OPTION_REG);
#else
    pciSetBitsLong(pMga->PciTag, PCI_OPTION_REG, OPTION_MASK,
                   state->Option);
#endif

    /* select pixel clock PLL as clock source */
    outTi3026(TVP3026_CLK_SEL, 0, state->DacRegs[3]);

    /* set loop and pixel clock PLL PLLEN bits to 0 */
    outTi3026(TVP3026_PLL_ADDR, 0, 0x2A);
    outTi3026(TVP3026_LOAD_CLK_DATA, 0, 0);
    outTi3026(TVP3026_PIX_CLK_DATA, 0, 0);

    /*
     * This function handles restoring the generic VGA registers.
     */
    vgaHWRestore(scrn, vga, vga_flags);

    MGA3026RestorePalette(scrn, vga->DAC);

    /*
     * Code to restore SVGA registers that have been saved/modified
     * goes here.
     */

    /* program pixel clock PLL */
    outTi3026(TVP3026_PLL_ADDR, 0, 0x00);

    for (i = 0; i < 3; i++)
        outTi3026(TVP3026_PIX_CLK_DATA, 0, state->DacClk[i]);

    if (vga->MiscOutReg & 0x08) {
        /* poll until pixel clock PLL LOCK bit is set */
        outTi3026(TVP3026_PLL_ADDR, 0, 0x3F);
        while (!(inTi3026(TVP3026_PIX_CLK_DATA) & 0x40));
    }

    /* set Q divider for loop clock PLL */
    outTi3026(TVP3026_MCLK_CTL, 0, state->DacRegs[18]);

    /* program loop PLL */
    outTi3026(TVP3026_PLL_ADDR, 0, 0x00);

    for (i = 3; i < 6; i++)
        outTi3026(TVP3026_LOAD_CLK_DATA, 0, state->DacClk[i]);

    if ((vga->MiscOutReg & 0x08) && ((state->DacClk[3] & 0xC0) == 0xC0)) {
        /* poll until loop PLL LOCK bit is set */
        outTi3026(TVP3026_PLL_ADDR, 0, 0x3F);
        while (!(inTi3026(TVP3026_LOAD_CLK_DATA) & 0x40));
    }

    /*
     * restore other DAC registers
     */
    for (i = 0; i < DACREGSIZE; i++)
        outTi3026(MGADACregs[i], 0, state->DacRegs[i]);

#ifdef DEBUG
    ErrorF("PCI retry (0-enabled / 1-disabled): %d\n",
            !!(state->Option & 0x20000000));
#endif
}

static void
state_save(xf86CrtcPtr crtc, MgaCrtcStatePtr state, int vga_flags)
{
    ScrnInfoPtr scrn = crtc->scrn;
    MGAPtr pMga = MGAPTR(scrn);
    vgaRegPtr vga = &VGAHWPTR(scrn)->SavedReg;
    int i;

    /*
     * Code is needed to get back to bank zero.
     */
    OUTREG16(MGAREG_CRTCEXT_INDEX, 0x0004);

    /*
     * This function will handle creating the data structure and filling
     * in the generic VGA portion.
     */
    vgaHWSave(scrn, vga, vga_flags);
    MGA3026SavePalette(scrn, vga->DAC);

    /*
     * The port I/O code necessary to read in the extended registers
     * into the fields of the vgaMGARec structure.
     */
    for (i = 0; i < 6; i++) {
        OUTREG8(0x1FDE, i);
        state->ExtVga[i] = INREG8(0x1FDF);
    }

    outTi3026(TVP3026_PLL_ADDR, 0, 0x00);

    for (i = 0; i < 3; i++) {
        state->DacClk[i] = inTi3026(TVP3026_PIX_CLK_DATA);
        outTi3026(TVP3026_PIX_CLK_DATA, 0, state->DacClk[i]);
    }

    outTi3026(TVP3026_PLL_ADDR, 0, 0x00);

    for (i = 3; i < 6; i++) {
        state->DacClk[i] = inTi3026(TVP3026_LOAD_CLK_DATA);
        outTi3026(TVP3026_LOAD_CLK_DATA, 0, state->DacClk[i]);
    }

    for (i = 0; i < DACREGSIZE; i++)
        state->DacRegs[i] = inTi3026(MGADACregs[i]);

#ifdef XSERVER_LIBPCIACCESS
    pci_device_cfg_read_u32(pMga->PciInfo, &state->Option, PCI_OPTION_REG);
#else
    state->Option = pciReadLong(pMga->PciTag, PCI_OPTION_REG);
#endif

#ifdef DEBUG
    ErrorF("read: %02X %02X %02X	%02X %02X %02X	%08lX\n",
           state->DacClk[0], state->DacClk[1],
           state->DacClk[2], state->DacClk[3],
           state->DacClk[4], state->DacClk[5],
           state->Option);

    for (i = 0; i < DACREGSIZE; i++)
        ErrorF("%02X ", state->DacRegs[i]);

    for (i = 0; i < 6; i++)
        ErrorF(" %02X", state->ExtVga[i]);

    ErrorF("\n");
#endif
}

static void
crtc_dpms(xf86CrtcPtr crtc, int mode)
{
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
    int vga_flags = VGA_SR_MODE;

    if (pMga->Primary)
        vga_flags |= VGA_SR_FONTS;

    state_restore(crtc, &data->saved_state, &vga->SavedReg, vga_flags);
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

    state_set(crtc, &state, mode);
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
Mga2064CrtcInit(ScrnInfoPtr scrn)
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
