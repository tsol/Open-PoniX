/*
 * MGA-1064, MGA-G100, MGA-G200, MGA-G400, MGA-G550 RAMDAC driver
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/drivers/mga/mga_dacG.c,v 1.54tsi Exp $ */

/*
 * This is a first cut at a non-accelerated version to work with the
 * new server design (DHD).
 */                     

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "colormapst.h"

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

#include <stdlib.h>
#include <unistd.h>

/*
 * implementation
 */

static void MGAGRamdacInit(ScrnInfoPtr);
static Bool MGAG_i2cInit(ScrnInfoPtr pScrn);

/*
 * MGAGLoadPalette
 */

static void
MGAPaletteLoadCallback(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    MGAPaletteInfo *pal = pMga->palinfo;
    int i;

    while (!(INREG8(0x1FDA) & 0x08)); 

    for(i = 0; i < 256; i++) {
	if(pal->update) {
	    outMGAdreg(MGA1064_WADR_PAL, i);
            outMGAdreg(MGA1064_COL_PAL, pal->red);
            outMGAdreg(MGA1064_COL_PAL, pal->green);
            outMGAdreg(MGA1064_COL_PAL, pal->blue);
	    pal->update = FALSE;
	}
	pal++;
    }
    pMga->PaletteLoadCallback = NULL;
}

void MGAGLoadPalette(
    ScrnInfoPtr pScrn, 
    int numColors, 
    int *indices,
    LOCO *colors,
    VisualPtr pVisual
){
    MGAPtr pMga = MGAPTR(pScrn);

    if((pMga->Overlay8Plus24) && (pVisual->nplanes != 8)) 
	return;

     if(pMga->Chipset == PCI_CHIP_MGAG400 || pMga->Chipset == PCI_CHIP_MGAG550){ 
	 /* load them at the retrace in the block handler instead to 
	    work around some problems with static on the screen */
	while(numColors--) {
	    pMga->palinfo[*indices].update = TRUE;
	    pMga->palinfo[*indices].red   = colors[*indices].red;
	    pMga->palinfo[*indices].green = colors[*indices].green;
	    pMga->palinfo[*indices].blue  = colors[*indices].blue;
	    indices++;
	}
	pMga->PaletteLoadCallback = MGAPaletteLoadCallback;
	return;
    } else {
	while(numColors--) {
            outMGAdreg(MGA1064_WADR_PAL, *indices);
            outMGAdreg(MGA1064_COL_PAL, colors[*indices].red);
            outMGAdreg(MGA1064_COL_PAL, colors[*indices].green);
            outMGAdreg(MGA1064_COL_PAL, colors[*indices].blue);
	    indices++;
	}
    }
}

/*
 * MGAGRestorePalette
 */

void
MGAGRestorePalette(ScrnInfoPtr pScrn, unsigned char* pntr)
{
    MGAPtr pMga = MGAPTR(pScrn);
    int i = 768;

    outMGAdreg(MGA1064_WADR_PAL, 0x00);
    while(i--)
	outMGAdreg(MGA1064_COL_PAL, *(pntr++));
}

/*
 * MGAGSavePalette
 */
void
MGAGSavePalette(ScrnInfoPtr pScrn, unsigned char* pntr)
{
    MGAPtr pMga = MGAPTR(pScrn);
    int i = 768;

    outMGAdreg(MGA1064_RADR_PAL, 0x00);
    while(i--) 
	*(pntr++) = inMGAdreg(MGA1064_COL_PAL);
}

/****
 ***  HW Cursor
 */
static void
MGAGLoadCursorImage(ScrnInfoPtr pScrn, unsigned char *src)
{
    MGAPtr pMga = MGAPTR(pScrn);
    CARD32 *dst = (CARD32*)(pMga->FbBase + pMga->FbCursorOffset);
    int i = 128;
    
    /* swap bytes in each line */
    while( i-- ) {
        *dst++ = (src[4] << 24) | (src[5] << 16) | (src[6] << 8) | src[7];
        *dst++ = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
        src += 8;
    }
}

static void 
MGAGShowCursor(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* Enable cursor - X-Windows mode */
    outMGAdac(MGA1064_CURSOR_CTL, 0x03);
}

static void 
MGAGShowCursorG100(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* Enable cursor - X-Windows mode */
    outMGAdac(MGA1064_CURSOR_CTL, 0x01);
}

static void
MGAGHideCursor(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    /* Disable cursor */
    outMGAdac(MGA1064_CURSOR_CTL, 0x00);
}

static void
MGAGSetCursorPosition(ScrnInfoPtr pScrn, int x, int y)
{
    MGAPtr pMga = MGAPTR(pScrn);
    x += 64;
    y += 64;

    /* cursor update must never occurs during a retrace period (pp 4-160) */
    while( INREG( MGAREG_Status ) & 0x08 );
    
    /* Output position - "only" 12 bits of location documented */
    OUTREG8( RAMDAC_OFFSET + MGA1064_CUR_XLOW, (x & 0xFF));
    OUTREG8( RAMDAC_OFFSET + MGA1064_CUR_XHI, (x & 0xF00) >> 8);
    OUTREG8( RAMDAC_OFFSET + MGA1064_CUR_YLOW, (y & 0xFF));
    OUTREG8( RAMDAC_OFFSET + MGA1064_CUR_YHI, (y & 0xF00) >> 8);
}


static void
MGAGSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg)
{
    MGAPtr pMga = MGAPTR(pScrn);

    /* Background color */
    outMGAdac(MGA1064_CURSOR_COL0_RED,   (bg & 0x00FF0000) >> 16);
    outMGAdac(MGA1064_CURSOR_COL0_GREEN, (bg & 0x0000FF00) >> 8);
    outMGAdac(MGA1064_CURSOR_COL0_BLUE,  (bg & 0x000000FF));

    /* Foreground color */
    outMGAdac(MGA1064_CURSOR_COL1_RED,   (fg & 0x00FF0000) >> 16);
    outMGAdac(MGA1064_CURSOR_COL1_GREEN, (fg & 0x0000FF00) >> 8);
    outMGAdac(MGA1064_CURSOR_COL1_BLUE,  (fg & 0x000000FF));
}

static void
MGAGSetCursorColorsG100(ScrnInfoPtr pScrn, int bg, int fg)
{
    MGAPtr pMga = MGAPTR(pScrn);

    /* Background color */
    outMGAdac(MGA1064_CURSOR_COL1_RED,   (bg & 0x00FF0000) >> 16);
    outMGAdac(MGA1064_CURSOR_COL1_GREEN, (bg & 0x0000FF00) >> 8);
    outMGAdac(MGA1064_CURSOR_COL1_BLUE,  (bg & 0x000000FF));

    /* Foreground color */
    outMGAdac(MGA1064_CURSOR_COL2_RED,   (fg & 0x00FF0000) >> 16);
    outMGAdac(MGA1064_CURSOR_COL2_GREEN, (fg & 0x0000FF00) >> 8);
    outMGAdac(MGA1064_CURSOR_COL2_BLUE,  (fg & 0x000000FF));
}

static Bool 
MGAGUseHWCursor(ScreenPtr pScrn, CursorPtr pCurs)
{
    MGAPtr pMga = MGAPTR(xf86Screens[pScrn->myNum]);
   /* This needs to detect if its on the second dac */
    if( XF86SCRNINFO(pScrn)->currentMode->Flags & V_DBLSCAN )
    	return FALSE;
    if( pMga->SecondCrtc == TRUE )
     	return FALSE;
    return TRUE;
}


/*
 * According to mga-1064g.pdf pp215-216 (4-179 & 4-180) the low bits of
 * XGENIODATA and XGENIOCTL are connected to the 4 DDC pins, but don't say
 * which VGA line is connected to each DDC pin, so I've had to guess.
 *
 * DDC1 support only requires DDC_SDA_MASK,
 * DDC2 support requires DDC_SDA_MASK and DDC_SCL_MASK
 *
 * If we want DDC on second head (P2) then we must use DDC2 protocol (I2C)
 *
 * Be careful, DDC1 and DDC2 refer to protocols, DDC_P1 and DDC_P2 refer to
 * DDC data coming in on which videoport on the card 
 */
static const int DDC_P1_SDA_MASK = (1 << 1);
static const int DDC_P1_SCL_MASK = (1 << 3);
static const int DDC_P2_SDA_MASK = (1 << 0);
static const int DDC_P2_SCL_MASK = (1 << 2);

static unsigned int
MGAG_ddc1Read(ScrnInfoPtr pScrn)
{
  MGAPtr pMga = MGAPTR(pScrn);
  unsigned char val;
  
  /* Define the SDA as an input */
  outMGAdacmsk(MGA1064_GEN_IO_CTL, ~(DDC_P1_SCL_MASK | DDC_P1_SDA_MASK), 0);

  /* wait for Vsync */
  if (pMga->is_G200SE) {
    usleep(4);
  } else {
    while( INREG( MGAREG_Status ) & 0x08 );
    while( ! (INREG( MGAREG_Status ) & 0x08) );
  }

  /* Get the result */
  val = (inMGAdac(MGA1064_GEN_IO_DATA) & DDC_P1_SDA_MASK);
  return val;
}

static void
MGAG_I2CGetBits(I2CBusPtr b, int *clock, int *data, int my_scl_mask, int my_sda_mask)
{
  ScrnInfoPtr pScrn = xf86Screens[b->scrnIndex];
  MGAPtr pMga = MGAPTR(pScrn);
  unsigned char val;
  
   /* Get the result. */
   val = inMGAdac(MGA1064_GEN_IO_DATA);
   
   *clock = (val & my_scl_mask) != 0;
   *data  = (val & my_sda_mask) != 0;
#ifdef DEBUG
  ErrorF("MGAG_I2CGetBits(%p,...) val=0x%x, returns clock %d, data %d\n", b, val, *clock, *data);
#endif
}

/*
 * ATTENTION! - the DATA and CLOCK lines need to be tri-stated when
 * high. Therefore turn off output driver for the line to set line
 * to high. High signal is maintained by a 15k Ohm pull-up resistor.
 */
static void
MGAG_I2CPutBits(I2CBusPtr b, int clock, int data, int my_scl_mask, int my_sda_mask)
{
  ScrnInfoPtr pScrn = xf86Screens[b->scrnIndex]; 
  MGAPtr pMga = MGAPTR(pScrn);
  unsigned char drv, val;

  val = (clock ? my_scl_mask : 0) | (data ? my_sda_mask : 0);
  drv = ((!clock) ? my_scl_mask : 0) | ((!data) ? my_sda_mask : 0);

  /* Write the values */
  outMGAdacmsk(MGA1064_GEN_IO_CTL, ~(my_scl_mask | my_sda_mask) , drv);
  outMGAdacmsk(MGA1064_GEN_IO_DATA, ~(my_scl_mask | my_sda_mask) , val);
#ifdef DEBUG
  ErrorF("MGAG_I2CPutBits(%p, %d, %d) val=0x%x\n", b, clock, data, val);
#endif
}

/* FIXME, can we use some neater way besides these silly stubs? */

static void
MGAG_DDC_P1_I2CPutBits(I2CBusPtr b, int clock, int data)
{
	MGAG_I2CPutBits(b, clock, data, DDC_P1_SCL_MASK, DDC_P1_SDA_MASK);
}
static void
MGAG_DDC_P2_I2CPutBits(I2CBusPtr b, int clock, int data)
{
	MGAG_I2CPutBits(b, clock, data, DDC_P2_SCL_MASK, DDC_P2_SDA_MASK);
}

static void
MGAG_DDC_P1_I2CGetBits(I2CBusPtr b, int *clock, int *data)
{
	MGAG_I2CGetBits(b, clock, data, DDC_P1_SCL_MASK, DDC_P1_SDA_MASK);
}
static void
MGAG_DDC_P2_I2CGetBits(I2CBusPtr b, int *clock, int *data)
{
	MGAG_I2CGetBits(b, clock, data, DDC_P2_SCL_MASK, DDC_P2_SDA_MASK);
}

Bool
MGAG_i2cInit(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    I2CBusPtr I2CPtr;

    I2CPtr = xf86CreateI2CBusRec();
    if(!I2CPtr) return FALSE;

    pMga->DDC_Bus1 = I2CPtr;

    I2CPtr->BusName    = "DDC P1";
    I2CPtr->scrnIndex  = pScrn->scrnIndex;
    I2CPtr->I2CPutBits = MGAG_DDC_P1_I2CPutBits;
    I2CPtr->I2CGetBits = MGAG_DDC_P1_I2CGetBits;
    I2CPtr->AcknTimeout = 5;

    if (!xf86I2CBusInit(I2CPtr)) {
        xf86DestroyI2CBusRec(pMga->DDC_Bus1, TRUE, TRUE);
        pMga->DDC_Bus1 = NULL;
        return FALSE;
    }

    /* We have a dual head setup on G-series, set up DDC #2. */
    I2CPtr = xf86CreateI2CBusRec();
    if(!I2CPtr) return FALSE;

    pMga->DDC_Bus2 = I2CPtr;

    I2CPtr->BusName    = "DDC P2";
    I2CPtr->scrnIndex  = pScrn->scrnIndex;
    I2CPtr->I2CPutBits = MGAG_DDC_P2_I2CPutBits;
    I2CPtr->I2CGetBits = MGAG_DDC_P2_I2CGetBits;
    I2CPtr->AcknTimeout = 5;

    if (!xf86I2CBusInit(I2CPtr)) {
        xf86DestroyI2CBusRec(pMga->DDC_Bus2, TRUE, TRUE);
        pMga->DDC_Bus2 = NULL;
    }

    return TRUE;
}


/*
 * MGAGRamdacInit
 * Handle broken G100 special.
 */
static void
MGAGRamdacInit(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);
    MGARamdacPtr MGAdac = &pMga->Dac;

    MGAdac->isHwCursor             = TRUE;
    MGAdac->CursorOffscreenMemSize = 1024;
    MGAdac->CursorMaxWidth         = 64;
    MGAdac->CursorMaxHeight        = 64;
    MGAdac->SetCursorPosition      = MGAGSetCursorPosition;
    MGAdac->LoadCursorImage        = MGAGLoadCursorImage;
    MGAdac->HideCursor             = MGAGHideCursor;
    if ((pMga->Chipset == PCI_CHIP_MGAG100) 
	|| (pMga->Chipset == PCI_CHIP_MGAG100)) {
      MGAdac->SetCursorColors        = MGAGSetCursorColorsG100;
      MGAdac->ShowCursor             = MGAGShowCursorG100;
    } else {
      MGAdac->SetCursorColors        = MGAGSetCursorColors;
      MGAdac->ShowCursor             = MGAGShowCursor;
    }
    MGAdac->UseHWCursor            = MGAGUseHWCursor;
    MGAdac->CursorFlags            =
#if X_BYTE_ORDER == X_LITTLE_ENDIAN
    				HARDWARE_CURSOR_BIT_ORDER_MSBFIRST |
#endif
    				HARDWARE_CURSOR_SOURCE_MASK_INTERLEAVE_64 |
    				HARDWARE_CURSOR_TRUECOLOR_AT_8BPP;

    MGAdac->LoadPalette 	   = MGAGLoadPalette;
    MGAdac->RestorePalette	   = MGAGRestorePalette;


    MGAdac->maxPixelClock = pMga->bios.pixel.max_freq;
    MGAdac->ClockFrom = X_PROBED;

    /* Disable interleaving and set the rounding value */
    pMga->Interleave = FALSE;

    pMga->Roundings[0] = 64;
    pMga->Roundings[1] = 32;
    pMga->Roundings[2] = 64;
    pMga->Roundings[3] = 32;

    /* Clear Fast bitblt flag */
    pMga->HasFBitBlt = FALSE;
}

void MGAGSetupFuncs(ScrnInfoPtr pScrn)
{
    MGAPtr pMga = MGAPTR(pScrn);

    pMga->PreInit = MGAGRamdacInit;
    pMga->ddc1Read = MGAG_ddc1Read;
    /* vgaHWddc1SetSpeed will only work if the card is in VGA mode */
    pMga->DDC1SetSpeed = vgaHWddc1SetSpeedWeak();
    pMga->i2cInit = MGAG_i2cInit;
}

