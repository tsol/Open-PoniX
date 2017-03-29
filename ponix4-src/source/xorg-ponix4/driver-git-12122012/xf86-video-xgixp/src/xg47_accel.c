/***************************************************************************
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.			   *
 *									   *
 * All Rights Reserved.							   *
 *									   *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the	   *
 * "Software"), to deal in the Software without restriction, including	   *
 * without limitation on the rights to use, copy, modify, merge,	   *
 * publish, distribute, sublicense, and/or sell copies of the Software,	   *
 * and to permit persons to whom the Software is furnished to do so,	   *
 * subject to the following conditions:					   *
 *									   *
 * The above copyright notice and this permission notice (including the	   *
 * next paragraph) shall be included in all copies or substantial	   *
 * portions of the Software.						   *
 *									   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF	   *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND		   *
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR			   *
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER	   *
 * DEALINGS IN THE SOFTWARE.						   *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"

#include "xf86PciInfo.h"
#include "xf86Pci.h"

#include "miline.h"

#include "xaarop.h"
#ifdef HAVE_XAA_H
#include "xaalocal.h"
#endif
#include "xgi.h"
#include "xgi_regs.h"
#include "xg47_regs.h"
#include "xgi_driver.h"
#include "xg47_cmdlist.h"
#include "xg47_accel.h"
#include "xgi_debug.h"
#include "xgi_misc.h"

/**
 * After the begin command, there are 4 DWORDs for every 3 commands.
 */
#define ACCEL_CMD_SIZE(num_cmds)  (4 * (((num_cmds) + 2) / 3))
#define CMDBUF_SIZE      0x2000


/* Jong 09/16/2006; support dual view */
extern int		g_DualViewMode;

typedef struct XG47_ACCEL_INFO
{
    unsigned long engineCmd;
    unsigned long engineFlag;

    int clp_x1, clp_y1, clp_x2, clp_y2;
    Bool clp_enable;

    int rop;

    unsigned int planemask;
    int color_depth;
    int trans_color;
    CARD32 pitch;
    int fg_color;
    int bg_color;
}XG47_accel_info;

static XG47_accel_info  accel_info;


/* inner functions */
static Bool XG47InitCmdList();
static void ResetClip();
static void SetClip(XGIPtr, CARD32 left, CARD32 top, CARD32 right, CARD32 bottom);

static inline void SetColorDepth(int color);
static inline void SetROP3(int rop);
static inline void SetFGColor(XGIPtr, CARD32 color);
static inline void SetPatFGColor(XGIPtr, CARD32 color);
static inline void SetPatBKColor(XGIPtr, CARD32 color);
static inline void SetDstColorKey(XGIPtr, CARD32 color);
static inline void SetDstBasePitch(XGIPtr, CARD32 pitch, CARD32 base);
static inline void SetSrcBasePitch(XGIPtr, CARD32 pitch, CARD32 base);
static inline void SetDstXY(XGIPtr, CARD32 x, CARD32 y, CARD32 addr);
static inline void SetSrcXY(XGIPtr, CARD32 x, CARD32 y, CARD32 addr);
static inline void SetDstSize(XGIPtr, CARD32 w, CARD32 h);
static inline void SetDrawingCommand(XGIPtr, CARD32 cmd, CARD32 flag);

/* XAA functions */
/* Sync */
static void XG47Sync(ScrnInfoPtr pScrn);

/* Clipper */
static void XG47SetClippingRectangle(ScrnInfoPtr pScrn,
                                     int left, int top, int right, int bottom);
static void XG47DisableClipping(ScrnInfoPtr pScrn);

/* Blits */
static void XG47SetupForScreenToScreenCopy(ScrnInfoPtr pScrn,
                                           int xdir, int ydir,
                                           int rop,
                                           unsigned int planemask,
                                           int trans_color);

static void XG47SubsequentScreenToScreenCopy(ScrnInfoPtr pScrn,
                                         	 int xsrc, int ysrc,
                                         	 int xdst, int ydst,
                                             int w, int h);
/* Solid fills */
static void XG47SetupForSolidFill(ScrnInfoPtr pScrn,
                                  int color,
                                  int rop,
                                  unsigned int planemask);

static void XG47SubsequentSolidFillRect(ScrnInfoPtr pScrn,
                                        int x, int y, int w, int h);

/* Screen to screen color expansion - HW not Support */
/*
static void XG47SetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                      int fg, int bg,
                                                      int rop,
                                                      unsigned int planemask);

static void XG47SubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                        int x, int y,
                                                        int w, int h,
                                                        int srcx, int srcy,
                                                        int skipleft);
*/

/* CPU to Screen Color expansion - similiar to XAAImageWrite() */
/*
static void XG47SetupForCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                   int fg, int bg,
                                                   int rop,
                                                   unsigned int planemask);

static void XG47SubsequentCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                     int x, int y, int w, int h,
                                                     int skipleft);
*/

/* 8x8 mono pattern fills */

static void XG47SetupForMono8x8PatternFill(ScrnInfoPtr pScrn,
                                           int patx, int paty,
                                           int fg, int bg,
                                           int rop,
                                           unsigned int planemask);

static void XG47SubsequentMono8x8PatternFillRect(ScrnInfoPtr pScrn,
                                                 int patx, int paty,
                                                 int x, int y, int w, int h);

/* 8x8 color pattern fills */
static void XG47SubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn,
                                                  int patx, int paty,
                                                  int x, int y, int w, int h);

static void XG47SetupForColor8x8PatternFill(ScrnInfoPtr pScrn,
                                            int patx, int paty,
                                            int rop,
                                            unsigned int planemask,
                                            int transparency_color);


/* Solid lines */
static void XG47SetupForSolidLine(ScrnInfoPtr pScrn,
                                  int color,
                                  int rop,
                                  unsigned int planemask);

/*
static void XG47SubsequentSolidTwoPointLine(ScrnInfoPtr pScrn,
                                            int xa, int ya, int xb, int yb,
                                            int flags);
*/
static void XG47SubsequentSolidBresenhamLine(ScrnInfoPtr pScrn,
                                             int x, int y, int absmaj, int absmin,
                                             int err, int len, int octant);

static void XG47SubsequentSolidHorVertLine(ScrnInfoPtr pScrn,
                                           int x, int y, int len, int dir);
/*
static void XG47SetupForDashedLine(ScrnInfoPtr pScrn,
                                   int fg, int bg,
                                   int rop,
                                   unsigned int planemask,
                                   int length,
                                   unsigned char *pattern);

static void XG47SubsequentDashedTwoPointLine(ScrnInfoPtr pScrn,
                                             int xa, int ya, int xb, int yb,
                                             int flags, int phase);

static void XG47SubsequentDashedBresenhamLine(ScrnInfoPtr pScrn,
                                              int x, int y,
                                              int absmaj, int absmin,
                                              int err, int len,
                                              int flags, int phase);
*/

#ifdef XGIDUALVIEW
static void XGIRestoreAccelState(ScrnInfoPtr pScrn);
#endif

Bool XG47AccelInit(ScreenPtr pScreen)
{
#ifdef HAVE_XAA_H
    XAAInfoRecPtr   pXaaInfo;
    ScrnInfoPtr     pScrn = xf86ScreenToScrn(pScreen);

    XGIPtr pXGI = XGIPTR(pScrn);

    PDEBUG(ErrorF("*-*Jong-XG47AccelInit()\n"));	
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47AccelInit\n");

    XGIDebug(DBG_CMDLIST, "[DBG] XG47AccelInit() Enable cmdlist\n");

    if (pXGI->noAccel)
        return FALSE;

    pXGI->pXaaInfo = pXaaInfo = XAACreateInfoRec();
    if (!pXaaInfo) return FALSE;

    pXaaInfo->Flags             = PIXMAP_CACHE          |
                                  OFFSCREEN_PIXMAPS     |
                                  LINEAR_FRAMEBUFFER;

    pXaaInfo->Sync = XG47Sync;


    pXaaInfo->SetClippingRectangle = XG47SetClippingRectangle;
    pXaaInfo->DisableClipping      = XG47DisableClipping;


    if (ENABLE_HW_SRC2SRC)
    {
		PDEBUG(ErrorF("*-*Jong-[INFO]Enable HW Screen to Screen Copy\n"));
        XGIDebug(DBG_INFO, "[INFO]Enable HW Screen to Screen Copy\n");

        pXaaInfo->ScreenToScreenCopyFlags      = NO_PLANEMASK                  |
                                                 NO_TRANSPARENCY; 

        pXaaInfo->SetupForScreenToScreenCopy   = XG47SetupForScreenToScreenCopy;
        pXaaInfo->SubsequentScreenToScreenCopy = XG47SubsequentScreenToScreenCopy;
    }

    if (ENABLE_HW_SOLIDFILL)
    {
		PDEBUG(ErrorF("*-*Jong-[INFO]Enable HW SolidFill\n"));
        XGIDebug(DBG_INFO, "[INFO]Enable HW SolidFill\n");
        pXaaInfo->SolidFillFlags            = NO_PLANEMASK | NO_TRANSPARENCY;
        pXaaInfo->SetupForSolidFill         = XG47SetupForSolidFill;
        pXaaInfo->SubsequentSolidFillRect   = XG47SubsequentSolidFillRect;
    }

    if (ENABLE_HW_SOLIDLINE)
    {
		PDEBUG(ErrorF("*-*Jong-[INFO]Enable HW SolidLine\n"));
        XGIDebug(DBG_INFO, "[INFO]Enable HW SolidLine\n");
        pXaaInfo->SolidLineFlags             = NO_PLANEMASK | NO_TRANSPARENCY;
        pXaaInfo->SetupForSolidLine          = XG47SetupForSolidLine;
        pXaaInfo->SubsequentSolidHorVertLine = XG47SubsequentSolidHorVertLine;
        pXaaInfo->SubsequentSolidBresenhamLine = XG47SubsequentSolidBresenhamLine;
    }

    if (ENABLE_HW_8X8PATTERN)
    {
		PDEBUG(ErrorF("*-*Jong-[INFO]Enable HW Color 8x8 color pattern fill\n"));
        XGIDebug(DBG_INFO, "[INFO]Enable HW Color 8x8 color pattern fill\n");
        pXaaInfo->Color8x8PatternFillFlags = HARDWARE_PATTERN_SCREEN_ORIGIN |
                                             BIT_ORDER_IN_BYTE_MSBFIRST;
        pXaaInfo->SetupForColor8x8PatternFill = XG47SetupForColor8x8PatternFill;
        pXaaInfo->SubsequentColor8x8PatternFillRect =
                                          XG47SubsequentColor8x8PatternFillRect;
    }

    if (ENABLE_HW_8X8MONOPAT)
    {
		PDEBUG(ErrorF("*-*Jong-[INFO]Enable HW Color 8x8 Mono pattern fill\n"));
        XGIDebug(DBG_INFO, "[INFO]Enable HW Color 8x8 Mono pattern fill\n");
        pXaaInfo->Mono8x8PatternFillFlags =  NO_PLANEMASK | NO_TRANSPARENCY |
                                            BIT_ORDER_IN_BYTE_MSBFIRST |
                                            HARDWARE_PATTERN_SCREEN_ORIGIN |
                                            HARDWARE_PATTERN_PROGRAMMED_BITS;

        pXaaInfo->SetupForMono8x8PatternFill = XG47SetupForMono8x8PatternFill;
        pXaaInfo->SubsequentMono8x8PatternFillRect = XG47SubsequentMono8x8PatternFillRect;
    }

    if (ENABLE_HW_IMAGEWRITE)
    {
    }

    pXaaInfo->ImageWriteBase  = pXGI->IOBase + 0x10000;
    pXaaInfo->ImageWriteRange = 0x8000;

/* Jong 09/16/2006; RestoreAccelState() is required if PCI entity has IS_SHARED_ACCEL */
/* Otherwise; it will cause failed of XAAInit() */
#ifdef XGIDUALVIEW
	      if(g_DualViewMode) 
		  {
			pXaaInfo->RestoreAccelState = XGIRestoreAccelState;
	      }
#endif

    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-Before XG47InitCmdList\n");
    if (XG47InitCmdList(pScrn) == FALSE)
    {
    	return FALSE;
    }
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After XG47InitCmdList\n");

    /* We always use patten bank0 since HW can not handle patten bank swapping corretly! */
    accel_info.engineFlag   = FER_EN_PATTERN_FLIP;
    /* ENINT; 32bpp */
    accel_info.engineCmd    = 0x8200;

    accel_info.clp_x1 = 0;
    accel_info.clp_y1 = 0;
    accel_info.clp_x2 = pScrn->virtualX;    /* Virtual width */
    accel_info.clp_y2 = pScrn->virtualY;

    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-Before XAAInit()\n");
    return(XAAInit(pScreen, pXaaInfo));
#else
    return FALSE;
#endif
}

#ifdef HAVE_XAA_H
/*
Function:
    1. Wait for GE
    2. Reset GE
*/
static void XG47Sync(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47Sync\n");

    if (accel_info.clp_enable)
    {
        XG47DisableClipping(pScrn);
    }

    XG47WaitForIdle(pXGI);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47Sync\n");
}

void XG47WaitForIdle(XGIPtr pXGI)
{
    int	idleCount = 0 ;
    unsigned int busyCount = 0;
    unsigned long reg;

    int enablewait = 1;

    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47WaitForIdle\n");
    if (enablewait == 1)
    {
        while (idleCount < 5)
        {
            reg = INB(0x2800);
            if (0 == (reg & IDLE_MASK))
            {
                idleCount++ ;
                busyCount = 0;
            }
            else
            {
                idleCount = 0 ;
                busyCount++;
                if (busyCount >= 5000000)
                {
                    /* vGE_Reset(); */
                    busyCount = 0;
                }
            }
        }
    }

    XGIDebug(DBG_WAIT_FOR_IDLE, "[DEBUG] [2800]=%08x\n", INDW(2800));
    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47WaitForIdle\n");
}


static void XG47SetClippingRectangle(ScrnInfoPtr pScrn,
                                     int left, int top, int right, int bottom)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetClippingRectangle\n");
    accel_info.clp_x1 = left;
    accel_info.clp_y1 = top;
    accel_info.clp_x2 = right;
    accel_info.clp_y2 = bottom;
    accel_info.clp_enable = TRUE;
    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetClippingRectangle\n");
}

static void XG47DisableClipping(ScrnInfoPtr pScrn)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47DisableClipping\n");

    ResetClip();

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47DisableClipping\n");
}

static void XG47SetupForScreenToScreenCopy(ScrnInfoPtr pScrn,
                                           int xdir, int ydir,
                                           int rop,
                                           unsigned int planemask,
                                           int trans_color)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetupForScreenToScreenCopy\n");

    accel_info.color_depth = pScrn->bitsPerPixel >> 3;
    accel_info.pitch = pScrn->displayWidth * accel_info.color_depth;
    accel_info.rop = rop;
    accel_info.planemask = planemask;
    accel_info.trans_color = trans_color;

    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) 
	| ((XAAGetCopyROP(rop) & 0xff) << 24);

    SetColorDepth(accel_info.color_depth);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetupForScreenToScreenCopy\n");
}

static void XG47SubsequentScreenToScreenCopy(ScrnInfoPtr pScrn,
                                             int x1, int y1,
                                             int x2, int y2,
                                             int w, int h)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    const unsigned size = ACCEL_CMD_SIZE(1+1+2+1+1+1+2);


    XGIDebug(DBG_FUNCTION, "[DBG-Jong-05292006] Enter XG47SubsequentScreenToScreenCopy\n");

    xg47_BeginCmdList(pXGI->cmdList, size);

    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);
    SetSrcBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetSrcXY(pXGI, x1, y1, 0);
    SetDstXY(pXGI, x2, y2, 0);
    SetDstSize(pXGI, w, h);

    SetDrawingCommand(pXGI, FER_CMD_BITBLT, FER_SOURCE_IN_VRAM);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentScreenToScreenCopy\n");
}

void XG47SetupForSolidFill(ScrnInfoPtr pScrn,
                           int color,
                           int rop,
                           unsigned int planemask)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetupForSolidFill\n");

    accel_info.color_depth = pScrn->bitsPerPixel >> 3;
    accel_info.pitch = pScrn->displayWidth * accel_info.color_depth;
    accel_info.rop = rop;
    accel_info.planemask = planemask;
    accel_info.fg_color = color;

    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) |
	((XAAGetPatternROP(rop) & 0xff) << 24);

    SetColorDepth(accel_info.color_depth);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetupForSolidFill\n");
}


void XG47SubsequentSolidFillRect(ScrnInfoPtr pScrn,
                                 int x, int y, int w, int h)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    const unsigned size = ACCEL_CMD_SIZE(1+1+2+1+1+2);


    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SubsequentSolidFillRect(%d,%d,%d,%d)\n", x, y, w, h);

    xg47_BeginCmdList(pXGI->cmdList, size);

    SetPatFGColor(pXGI, accel_info.fg_color);
    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetDstXY(pXGI, x, y, 0);
    SetDstSize(pXGI, w, h);
    SetDrawingCommand(pXGI, FER_CMD_BITBLT, EBP_SOLID_BRUSH);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentSolidFillRect");
}

/*
static void XG47SetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                      int fg, int bg,
                                                      int rop,
                                                      unsigned int planemask)
{

}

static void XG47SubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                        int x, int y,
                                                        int w, int h,
                                                        int srcx, int srcy,
                                                        int skipleft)
{

}

*/

static void XG47SetupForMono8x8PatternFill(ScrnInfoPtr pScrn,
                                           int patx, int paty,
                                           int fg, int bg,
                                           int rop,
                                           unsigned int planemask)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetupForMono8x8PatternFill\n");

    accel_info.color_depth = pScrn->bitsPerPixel >> 3;
    accel_info.pitch = pScrn->displayWidth * accel_info.color_depth;
    accel_info.rop = rop;
    accel_info.planemask = planemask;
    accel_info.fg_color = fg;
    accel_info.bg_color = bg;

    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) |
	((XAAGetPatternROP(rop) & 0xff) << 24);

    SetColorDepth(accel_info.color_depth);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetupForMono8x8PatternFill\n");
}

static void XG47SubsequentMono8x8PatternFillRect(ScrnInfoPtr pScrn,
                                                 int patx, int paty,
                                                 int x, int y, int w, int h)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    const unsigned size = ACCEL_CMD_SIZE(6+1+1+1+1+2+1+1+2);


    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SubsequentMono8x8PatternFillRect(%d,%d,%d,%d)\n", x, y, w, h);

    xg47_BeginCmdList(pXGI->cmdList, size);

    xg47_SendGECommand(pXGI->cmdList, ENG_DRAWINGFLAG, FER_EN_PATTERN_FLIP);
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN, patx);
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN1, paty);
    xg47_SendGECommand(pXGI->cmdList, ENG_DRAWINGFLAG, FER_EN_PATTERN_FLIP | FER_PATTERN_BANK1);
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN, patx);
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN1, paty);

    SetPatFGColor(pXGI, accel_info.fg_color);
    SetPatBKColor(pXGI, accel_info.bg_color);

    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);
    SetSrcBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetDstXY(pXGI, x, y, 0);
    SetDstSize(pXGI, w, h);

    SetDrawingCommand(pXGI, FER_CMD_BITBLT, FER_MONO_PATTERN_DPRO | 0x2);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentMono8x8PatternFillRect\n");
}



static void XG47SetupForColor8x8PatternFill(ScrnInfoPtr pScrn,
                                            int patx, int paty,
                                            int rop,
                                            unsigned int planemask,
                                            int transparency_color)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    unsigned long pattern_offset;
    unsigned long* ptr;
    int i;
    const unsigned size = ACCEL_CMD_SIZE((pScrn->bitsPerPixel * 2) * 2);

    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetupForColor8x8PatternFill\n");

    accel_info.color_depth = pScrn->bitsPerPixel >> 3;
    accel_info.pitch = pScrn->displayWidth * accel_info.color_depth;
    accel_info.rop = rop;
    accel_info.planemask = planemask;
    accel_info.trans_color = transparency_color;

    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) |
	((XAAGetPatternROP(rop) & 0xff) << 24);

    SetColorDepth(accel_info.color_depth);

    xg47_BeginCmdList(pXGI->cmdList, size);

    pattern_offset = ((paty * pScrn->displayWidth * pScrn->bitsPerPixel / 8) +
                      (patx * pScrn->bitsPerPixel / 8));

    ptr = (unsigned long *)(pXGI->fbBase + pattern_offset);
    for(i = 0; i < pScrn->bitsPerPixel*2; i ++, ptr++ )
    {
        xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN +((i & ~0x20)<<2), *ptr);
    }

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetupForColor8x8PatternFill\n");
}

static void XG47SubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn,
                                                  int patx, int paty,
                                                  int x, int y, int w, int h)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    uint32_t trans_draw = 0;
    const unsigned size = ACCEL_CMD_SIZE(1+1+1+2+1+1+2);


    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SubsequentColor8x8PatternFillRect(%d,%d,%d,%d)\n", x, y, w, h);

    xg47_BeginCmdList(pXGI->cmdList, size);

    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);
    SetSrcBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.trans_color != -1) {
        SetDstColorKey(pXGI, accel_info.trans_color & 0xffffff);
        trans_draw = EBP_TRANSPARENT_MODE;
    }

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetDstXY(pXGI, x, y, 0);
    SetDstSize(pXGI, w, h);

    SetDrawingCommand(pXGI, FER_CMD_BITBLT, trans_draw);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentColor8x8PatternFillRect\n");
}

static void XG47SetupForSolidLine(ScrnInfoPtr pScrn,
                                  int color,
                                  int rop,
                                  unsigned int planemask)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SetupForSolidLine\n");

    accel_info.color_depth = pScrn->bitsPerPixel >> 3;
    accel_info.pitch = pScrn->displayWidth * accel_info.color_depth;
    accel_info.rop = rop;
    accel_info.planemask = planemask;
    accel_info.fg_color = color;
    accel_info.bg_color = 0xffffffff;
 
    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) |
	((XAAGetPatternROP(rop) & 0xff) << 24);

    SetColorDepth(accel_info.color_depth);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SetupForSolidLine\n");
}

static void XG47SubsequentSolidHorVertLine(ScrnInfoPtr pScrn,
                                           int x, int y, int len, int dir)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    const unsigned size = ACCEL_CMD_SIZE(1+1+1+2+1+1+2);

    XGIDebug(DBG_FUNCTION, "[DBG] Enter XG47SubsequentSolidHorVertLine(%d,%d,%d,%d)\n", x, y, len, dir);

    xg47_BeginCmdList(pXGI->cmdList, size);

    SetPatFGColor(pXGI, accel_info.fg_color);
    SetPatBKColor(pXGI, accel_info.bg_color);
    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetDstXY(pXGI, x, y, 0);
    if (dir == DEGREES_0)
        SetDstSize(pXGI, len, 1);
    else
        SetDstSize(pXGI, 1, len);

    SetDrawingCommand(pXGI, FER_CMD_BITBLT,  EBP_SOLID_BRUSH);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentSolidHorVertLine\n");
}

static void XG47SubsequentSolidBresenhamLine(ScrnInfoPtr pScrn,
                                             int x, int y, int absmaj, int absmin,
                                             int err, int len, int octant)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int direction = 0;
    int axial, diagonal, error;
    const unsigned size = ACCEL_CMD_SIZE(1+1+1+2+1+1+1+2);


    XGIDebug(DBG_FUNCTION,
        "[DBG] Enter XG47SubsequentSolidBresenhamLine(%d,%d,%d,%d,%d,%d,%d)\n",
        x, y, absmaj, absmin, err, len, octant);

    xg47_BeginCmdList(pXGI->cmdList, size);

    /* convert bresenham line interface style */
    axial = (absmin>>1)&0xFFF;
    diagonal = ((absmin - absmaj)>>1)&0xFFF;
    error = absmin -(absmaj>>1);
    if(error &0x01) {
        if(octant & YDECREASING)
            error++;
        else
            error--;
    }

    /* set direction */
    if(octant & YMAJOR) direction |= EBP_Y_MAJOR;
    if(octant & XDECREASING) direction |= EBP_OCT_X_DEC;
    if(octant & YDECREASING) direction |= EBP_OCT_Y_DEC;

    SetPatFGColor(pXGI, accel_info.fg_color);
    SetPatBKColor(pXGI, accel_info.bg_color);

    SetDstBasePitch(pXGI, accel_info.pitch, pScrn->fbOffset);

    if (accel_info.clp_enable) {
        SetClip(pXGI, accel_info.clp_x1, accel_info.clp_y1, accel_info.clp_x2, accel_info.clp_y2);
    }

    SetSrcXY(pXGI, diagonal, axial, 0);
    SetDstXY(pXGI, x, y, 0);
    SetDstSize(pXGI, error, len);

    SetDrawingCommand(pXGI, FER_CMD_LINEDRAW, EBP_MONO_SOURCE | direction);

    xg47_EndCmdList(pXGI->cmdList);

    XGIDebug(DBG_FUNCTION, "[DBG] Leave XG47SubsequentSolidBresenhamLine\n");
}

/*
static void XG47SubsequentSolidTwoPointLine(ScrnInfoPtr pScrn,
                                            int xa, int ya, int xb, int yb,
                                            int flags)
{
}
*/

static void SetColorDepth(int color)
{
    /* 2125[0,1] = screen color depth */

    /* color   1   2   3   4
     * mode    0   1   3   2
     *
     * Then left shift by 8 to get into position.
     */
    const unsigned mode = (color == 3) ? 3 : (color >> 1);
    accel_info.engineCmd = (accel_info.engineCmd & ~0x300) | (mode << 8);
}



static void SetROP3(int rop)
{
    accel_info.engineCmd = (accel_info.engineCmd & 0xffffff) | 
	((XAAGetCopyROP(rop) & 0xff) << 24);
}

static void SetFGColor(XGIPtr pXGI, CARD32 color)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetFGColor()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_FGCOLOR, color);
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetFGColor()\n");
}

static void SetPatFGColor(XGIPtr pXGI, CARD32 color)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetPatFGColor()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN_FG, color);
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetPatFGColor()\n");
}

static void SetPatBKColor(XGIPtr pXGI, CARD32 color)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetPatBKColor()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_PATTERN_BG, color);
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetPatBKColor()\n");
}

static void SetDstColorKey(XGIPtr pXGI, CARD32 color)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetDstColorKey()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_DEST_COLORKEY, color);
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetDstColorKey()\n");
}

static void SetDstBasePitch(XGIPtr pXGI, CARD32 pitch, CARD32 base)
{
    /* copy to onscreen, HW start address is zero */
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetDstBasePitch()\n");

	/* Jong 11/08/2006; seems have a bug for base=64MB=0x4000000 */
	/* base >> 4 = 0x400000; 0x400000 & 0x3fffff = 0x0 */
	/* Thus, base address must be less than 64MB=0x4000000 */
    xg47_SendGECommand(pXGI->cmdList, ENG_DST_BASE, ((pitch & 0x3ff0) << 18) | ((base >> 4) & 0x3fffff));

#ifdef XGI_DUMP_DUALVIEW
	ErrorF("Jong-Debug-SetDstBasePitch-((base >> 4) & 0x7fffff)=0x%x-\n", ((base >> 4) & 0x7fffff));
#endif

    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetDstBasePitch()\n");
}

static void SetSrcBasePitch(XGIPtr pXGI, CARD32 pitch, CARD32 base)
{
    /* copy from onscreen, HW start address is zero */
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetSrcBasePitch()\n");

	/* Jong 11/08/2006; seems have a bug for base=64MB=0x4000000 */
	/* base >> 4 = 0x400000; 0x400000 & 0x3fffff = 0x0 */
	/* Thus, base address must be less than 64MB=0x4000000 */
    xg47_SendGECommand(pXGI->cmdList, ENG_SRC_BASE, ((pitch & 0x3ff0) << 18) | ((base >> 4) & 0x3fffff)); 

    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetSrcBasePitch()\n");
}

static void SetDstXY(XGIPtr pXGI, CARD32 x, CARD32 y, CARD32 addr)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetDstXY()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_DESTXY, (((x & 0x1fff)<<16) | (y & 0x1fff) | (addr & 0xe0000000) | ((addr & 0x1c000000) >> 13)));
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetDstXY()\n");
}

static void SetSrcXY(XGIPtr pXGI, CARD32 x, CARD32 y, CARD32 addr)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetSrcXY()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_SRCXY, (((x & 0xfff)<<16) | (y & 0xfff)   | (addr & 0xe0000000) | ((addr & 0x1c000000) >> 13)));
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetSrcXY()\n");
}

static void SetDstSize(XGIPtr pXGI, CARD32 w, CARD32 h)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetDstSize()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_DIMENSION, (((w & 0xfff)<<16) | (h & 0xfff)));
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetDstSize()\n");
}

static void SetDrawingCommand(XGIPtr pXGI, CARD32 cmd, CARD32 flag)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetDrawingCommand()\n");
    xg47_SendGECommand(pXGI->cmdList, ENG_DRAWINGFLAG, (accel_info.engineFlag | flag));
    xg47_SendGECommand(pXGI->cmdList, ENG_COMMAND, (accel_info.engineCmd | cmd));
    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetDrawingCommand()\n");
}

static void SetClip(XGIPtr pXGI, CARD32 left, CARD32 top, CARD32 right, CARD32 bottom)
{
    XGIDebug(DBG_FUNCTION, "[DBG] Enter SetClip()\n");

    accel_info.engineCmd |= FER_CLIP_ON;
    xg47_SendGECommand(pXGI->cmdList, ENG_CLIPSTARTXY, (((left & 0xfff)<<16) |
                                   (top & 0xfff)));
    xg47_SendGECommand(pXGI->cmdList, ENG_CLIPENDXY, (((right & 0xfff)<<16) |
                                 ((bottom>0xfff?0xfff:bottom) & 0xfff)));

    XGIDebug(DBG_FUNCTION, "[DBG] Leave SetClip()\n");
}

static void ResetClip(void)
{
    accel_info.clp_enable = FALSE;
    accel_info.engineCmd &= ~FER_CLIP_ON;
}

static Bool XG47InitCmdList(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    XGIDebug(DBG_FUNCTION, "[DBG]Enter XG47InitCmdList() - XGI_CMDLIST_ENABLE(1)\n");

    pXGI->cmdList = xg47_Initialize(pScrn, CMDBUF_SIZE, pXGI->drm_fd);
    if (pXGI->cmdList == NULL) {
        XAADestroyInfoRec(pXGI->pXaaInfo);
        pXGI->pXaaInfo = NULL;
    }

    return (pXGI->cmdList != NULL);
}

#endif

void XG47AccelExit(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn   = xf86ScreenToScrn(pScreen);
    XGIPtr      pXGI = XGIPTR(pScrn);

    XGIDebug(DBG_FUNCTION, "Enter XG47AccelExit\n");

    XG47WaitForIdle(pXGI);

    xg47_Cleanup(pScrn, pXGI->cmdList);
    pXGI->cmdList = NULL;

    XGIDebug(DBG_FUNCTION, "Leave XG47AccelExit\n");
}


#ifdef XGIDUALVIEW
static void
XGIRestoreAccelState(ScrnInfoPtr pScrn)
{
	XGIPtr pXGI = XGIPTR(pScrn);
	XG47WaitForIdle(pXGI);
}
#endif
