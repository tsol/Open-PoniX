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

/*
 * This is a first cut at a non-accelerated version to work with the
 * new server design (DHD).
 */                     

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

#include "xf86DDC.h"

#include "mga_dac3026.h"

static void MGA3026RamdacInit(ScrnInfoPtr);
static Bool MGA3026_i2cInit(ScrnInfoPtr pScrn);

/*
 * implementation
 */

static void
MGA3026LoadCursorImage(
    ScrnInfoPtr pScrn, 
    unsigned char *src
)
{
    MGAPtr pMga = MGAPTR(pScrn);
    int i = 1024;
       
    outTi3026(TVP3026_CURSOR_CTL, 0xf3, 0x00); /* reset A9,A8 */
    /* reset cursor RAM load address A7..A0 */
    outTi3026dreg(TVP3026_WADR_PAL, 0x00); 

    while(i--) {
	while (INREG8(0x1FDA) & 0x01);
	while (!(INREG8(0x1FDA) & 0x01));
        outTi3026dreg(TVP3026_CUR_RAM, *(src++));    
    }
}


static void 
MGA3026ShowCursor(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* Enable cursor - X11 mode */
    outTi3026(TVP3026_CURSOR_CTL, 0x6c, 0x13);
}

static void
MGA3026HideCursor(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* Disable cursor */
    outTi3026(TVP3026_CURSOR_CTL, 0xfc, 0x00);
}

static void
MGA3026SetCursorPosition(
   ScrnInfoPtr pScrn, 
   int x, int y
)
{
    MGAPtr pMga = MGAPTR(pScrn);
    x += 64;
    y += 64;

    /* Output position - "only" 12 bits of location documented */
   
    outTi3026dreg(TVP3026_CUR_XLOW, x & 0xFF);
    outTi3026dreg(TVP3026_CUR_XHI, (x >> 8) & 0x0F);
    outTi3026dreg(TVP3026_CUR_YLOW, y & 0xFF);
    outTi3026dreg(TVP3026_CUR_YHI, (y >> 8) & 0x0F);
}

static void
MGA3026SetCursorColors(
   ScrnInfoPtr pScrn, 
   int bg, int fg
)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* The TI 3026 cursor is always 8 bits so shift 8, not 10 */

    /* Background color */
    outTi3026dreg(TVP3026_CUR_COL_ADDR, 1);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (bg & 0x00FF0000) >> 16);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (bg & 0x0000FF00) >> 8);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (bg & 0x000000FF));

    /* Foreground color */
    outTi3026dreg(TVP3026_CUR_COL_ADDR, 2);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (fg & 0x00FF0000) >> 16);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (fg & 0x0000FF00) >> 8);
    outTi3026dreg(TVP3026_CUR_COL_DATA, (fg & 0x000000FF));
}

static Bool 
MGA3026UseHWCursor(ScreenPtr pScrn, CursorPtr pCurs)
{
    if( XF86SCRNINFO(pScrn)->currentMode->Flags & V_DBLSCAN )
    	return FALSE;
    return TRUE;
}

static const int DDC_SDA_MASK = 1 << 2;
static const int DDC_SCL_MASK = 1 << 4;

static unsigned int
MGA3026_ddc1Read(ScrnInfoPtr pScrn)
{
  MGAPtr pMga = MGAPTR(pScrn);

  /* Define the SDA as an input */
  outTi3026(TVP3026_GEN_IO_CTL, 0xfb, 0);

  /* wait for Vsync */
  while( INREG( MGAREG_Status ) & 0x08 );
  while( ! (INREG( MGAREG_Status ) & 0x08) );

  /* Get the result */
  return (inTi3026(TVP3026_GEN_IO_DATA) & DDC_SDA_MASK) >> 2 ;
}

static void
MGA3026_I2CGetBits(I2CBusPtr b, int *clock, int *data) 
{
  ScrnInfoPtr pScrn = xf86Screens[b->scrnIndex];
  MGAPtr pMga = MGAPTR(pScrn);
  unsigned char val;

  /* Get the result. */
  val = inTi3026(TVP3026_GEN_IO_DATA); 
  *clock = (val & DDC_SCL_MASK) != 0;
  *data  = (val & DDC_SDA_MASK) != 0;

#ifdef DEBUG
	 ErrorF("MGA3026_I2CGetBits(%p,...) val=0x%x, returns clock %d, data %d\n", b, val, *clock, *data);
#endif
}

/*
 * ATTENTION! - the DATA and CLOCK lines need to be tri-stated when
 * high. Therefore turn off output driver for the line to set line
 * to high. High signal is maintained by a 15k Ohm pll-up resistor.
 */
static void
MGA3026_I2CPutBits(I2CBusPtr b, int clock, int data)
{
  ScrnInfoPtr pScrn = xf86Screens[b->scrnIndex];
  MGAPtr pMga = MGAPTR(pScrn);
  unsigned char val,drv;

  /* Write the values */
  val = (clock ? DDC_SCL_MASK : 0) | (data ? DDC_SDA_MASK : 0);
  drv = ((!clock) ? DDC_SCL_MASK : 0) | ((!data) ? DDC_SDA_MASK : 0);
  /* Define the SDA (Data) and SCL (clock) as outputs */
  outTi3026(TVP3026_GEN_IO_CTL, ~(DDC_SDA_MASK | DDC_SCL_MASK), drv);
  outTi3026(TVP3026_GEN_IO_DATA, ~(DDC_SDA_MASK | DDC_SCL_MASK), val); 

#ifdef DEBUG
  ErrorF("MGA3026_I2CPutBits(%p, %d, %d) val=0x%x\n", b, clock, data, val);
#endif

}


static Bool
MGA3026_i2cInit(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    I2CBusPtr I2CPtr;

    I2CPtr = xf86CreateI2CBusRec();
    if(!I2CPtr) return FALSE;

    I2CPtr->BusName    = "DDC";
    I2CPtr->scrnIndex  = pScrn->scrnIndex;
    I2CPtr->I2CPutBits = MGA3026_I2CPutBits;
    I2CPtr->I2CGetBits = MGA3026_I2CGetBits;

    /* I2CPutByte is timing out, experimenting with AcknTimeout 
     * default is 2CPtr->AcknTimeout = 5; 
     */
    /* I2CPtr->AcknTimeout = 10; */

    if (!xf86I2CBusInit(I2CPtr)) {
        xf86DestroyI2CBusRec(I2CPtr, TRUE, TRUE);
	return FALSE;
    } else {
        pMga->DDC_Bus1 = I2CPtr;
        return TRUE;
    }
}

static void
MGA3026RamdacInit(ScrnInfoPtr pScrn)
{
    MGAPtr pMga;
    MGARamdacPtr MGAdac;

    pMga = MGAPTR(pScrn);
    MGAdac = &pMga->Dac;

    MGAdac->isHwCursor		= TRUE;
    MGAdac->CursorMaxWidth	= 64;
    MGAdac->CursorMaxHeight	= 64;
    MGAdac->SetCursorColors	= MGA3026SetCursorColors;
    MGAdac->SetCursorPosition	= MGA3026SetCursorPosition;
    MGAdac->LoadCursorImage	= MGA3026LoadCursorImage;
    MGAdac->HideCursor		= MGA3026HideCursor;
    MGAdac->ShowCursor		= MGA3026ShowCursor;
    MGAdac->UseHWCursor		= MGA3026UseHWCursor;
    MGAdac->CursorFlags		= 
#if X_BYTE_ORDER == X_LITTLE_ENDIAN
				HARDWARE_CURSOR_BIT_ORDER_MSBFIRST |
#endif
				HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
				HARDWARE_CURSOR_SOURCE_MASK_NOT_INTERLEAVED;

    MGAdac->LoadPalette 	= MGA3026LoadPalette;
    MGAdac->RestorePalette	= MGA3026RestorePalette;
    
    MGAdac->maxPixelClock = pMga->bios.pixel.max_freq;
    MGAdac->ClockFrom = X_PROBED;

    MGAdac->MemoryClock = pMga->bios.mem_clock;
    MGAdac->MemClkFrom = X_PROBED;
    MGAdac->SetMemClk = TRUE;


    /* safety check */
    if ( (MGAdac->MemoryClock < 40000) ||
         (MGAdac->MemoryClock > 70000) )
	MGAdac->MemoryClock = 50000; 

    /*
     * Should initialise a sane default when the probed value is
     * obviously garbage.
     */
     
    /* Check if interleaving can be used and set the rounding value */
    if (pScrn->videoRam > 2048)
        pMga->Interleave = TRUE;
    else {
        pMga->Interleave = FALSE;
        pMga->BppShifts[0]++;
        pMga->BppShifts[1]++;
        pMga->BppShifts[2]++;
        pMga->BppShifts[3]++;
    }

    pMga->Roundings[0] = 128 >> pMga->BppShifts[0];
    pMga->Roundings[1] = 128 >> pMga->BppShifts[1];
    pMga->Roundings[2] = 128 >> pMga->BppShifts[2];
    pMga->Roundings[3] = 128 >> pMga->BppShifts[3];

    /* Set Fast bitblt flag */
    pMga->HasFBitBlt = pMga->bios.fast_bitblt;
}

void MGA3026LoadPalette(
    ScrnInfoPtr pScrn, 
    int numColors, 
    int *indices,
    LOCO *colors,
    VisualPtr pVisual
){
    MGAPtr pMga = MGAPTR(pScrn);
    int i, index;

    if (pMga->Overlay8Plus24 && (pVisual->nplanes != 8))
	return;

    if (pVisual->nplanes == 16) {
	for(i = 0; i < numColors; i++) {
	    index = indices[i];
            outTi3026dreg(MGA1064_WADR_PAL, index << 2);
            outTi3026dreg(MGA1064_COL_PAL, colors[index >> 1].red);
            outTi3026dreg(MGA1064_COL_PAL, colors[index].green);
            outTi3026dreg(MGA1064_COL_PAL, colors[index >> 1].blue);

	/* we have to write 2 indices since the pixel X on the
	   TVP3026 has green colors at different locations from
	   the red and blue colors */
	    if(index <= 31) {
		outTi3026dreg(MGA1064_WADR_PAL, index << 3);
		outTi3026dreg(MGA1064_COL_PAL, colors[index].red);
		outTi3026dreg(MGA1064_COL_PAL, colors[(index << 1) + 1].green);
		outTi3026dreg(MGA1064_COL_PAL, colors[index].blue);
	    }
	}
    } else {
	int shift = (pVisual->nplanes == 15) ? 3 : 0;

	for(i = 0; i < numColors; i++) {
	    index = indices[i];
            outTi3026dreg(MGA1064_WADR_PAL, index << shift);
            outTi3026dreg(MGA1064_COL_PAL, colors[index].red);
            outTi3026dreg(MGA1064_COL_PAL, colors[index].green);
            outTi3026dreg(MGA1064_COL_PAL, colors[index].blue);
	}
    }
}

void
MGA3026SavePalette(ScrnInfoPtr pScrn, unsigned char* pntr)
{
    MGAPtr pMga = MGAPTR(pScrn);
    int i = 768;

    outTi3026dreg(TVP3026_RADR_PAL, 0x00);
    while(i--)
        *(pntr++) = inTi3026dreg(TVP3026_COL_PAL);
}

void
MGA3026RestorePalette(ScrnInfoPtr pScrn, unsigned char* pntr)
{
    MGAPtr pMga = MGAPTR(pScrn);
    int i = 768;

    outTi3026dreg(TVP3026_WADR_PAL, 0x00);
    while(i--) 
        outTi3026dreg(TVP3026_COL_PAL, *(pntr++));
}


void MGA2064SetupFuncs(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    
    pMga->PreInit = MGA3026RamdacInit;
    pMga->ddc1Read = MGA3026_ddc1Read;
    /* vgaHWddc1SetSpeed will only work if the card is in VGA mode */
    pMga->DDC1SetSpeed = vgaHWddc1SetSpeedWeak();
    pMga->i2cInit = MGA3026_i2cInit;
}
