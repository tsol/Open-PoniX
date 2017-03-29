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

#ifdef HAVE_XAA_H
#include "xaalocal.h"
#endif

#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_driver.h"

#include "dgaproc.h"

static Bool XGIDGAOpenFramebuffer(ScrnInfoPtr, char **, unsigned char **,
                                int *, int *, int *);
static Bool XGIDGASetMode(ScrnInfoPtr, DGAModePtr);
static void XGIDGASetViewport(ScrnInfoPtr, int, int, int);
static int  XGIDGAGetViewport(ScrnInfoPtr);
#ifdef HAVE_XAA_H
static void XGIDGASync(ScrnInfoPtr);
static void XGIDGAFillRect(ScrnInfoPtr, int, int, int, int, unsigned long);
static void XGIDGABlitRect(ScrnInfoPtr, int, int, int, int, int, int);
#if 0
static void XGIDGABlitTransRect(ScrnInfoPtr, int, int, int, int, int, int,
                              unsigned long);
#endif
#endif

static DGAFunctionRec XGIDGAFuncs = {
    XGIDGAOpenFramebuffer,
    NULL,
    XGIDGASetMode,
    XGIDGASetViewport,
    XGIDGAGetViewport,
#ifdef HAVE_XAA_H
    XGIDGASync,
    XGIDGAFillRect,
    XGIDGABlitRect,
#endif
};

Bool XGIDGAInit(ScreenPtr pScreen)
{
    ScrnInfoPtr     pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr          pXGI = XGIPTR(pScrn);
    DGAModePtr      pDgaModes = NULL, pNewDgaModes = NULL, pCurrentMode;
    DisplayModePtr  pMode, pFirstMode;
    int             bpp = pScrn->bitsPerPixel >> 3;
    int             num = 0;
    Bool            oneMore;

    pMode = pFirstMode = pScrn->modes;

    while(pMode)
    {
        if(0 /*pScrn->displayWidth != pMode->HDisplay*/)
        {
            pNewDgaModes = realloc(pDgaModes, (num + 2) * sizeof(DGAModeRec));
            oneMore = TRUE;
        }
        else
        {
            pNewDgaModes = realloc(pDgaModes, (num + 1) * sizeof(DGAModeRec));
            oneMore = FALSE;
        }

        if(!pNewDgaModes)
        {
            free(pDgaModes);
            return FALSE;
        }
        pDgaModes = pNewDgaModes;

SECOND_PASS:

        pCurrentMode = pDgaModes + num;
        num++;

        pCurrentMode->mode = pMode;
        pCurrentMode->flags = DGA_CONCURRENT_ACCESS | DGA_PIXMAP_AVAILABLE;
        pCurrentMode->flags |= DGA_FILL_RECT | DGA_BLIT_RECT;
        if(pMode->Flags & V_DBLSCAN)
            pCurrentMode->flags |= DGA_DOUBLESCAN;
        if(pMode->Flags & V_INTERLACE)
            pCurrentMode->flags |= DGA_INTERLACED;
        pCurrentMode->byteOrder = pScrn->imageByteOrder;
        pCurrentMode->depth = pScrn->depth;
        pCurrentMode->bitsPerPixel = pScrn->bitsPerPixel;
        pCurrentMode->red_mask = pScrn->mask.red;
        pCurrentMode->green_mask = pScrn->mask.green;
        pCurrentMode->blue_mask = pScrn->mask.blue;
        pCurrentMode->visualClass = (bpp == 1) ? PseudoColor : TrueColor;
        pCurrentMode->viewportWidth = pMode->HDisplay;
        pCurrentMode->viewportHeight = pMode->VDisplay;
        pCurrentMode->xViewportStep = 1;
        pCurrentMode->yViewportStep = 1;
        pCurrentMode->viewportFlags = DGA_FLIP_RETRACE;
        pCurrentMode->offset = 0;
        pCurrentMode->address = pXGI->fbBase;

        if(oneMore)
        {
            /* first one is narrow width */
            pCurrentMode->bytesPerScanline = ((pMode->HDisplay * bpp) + 3) & ~3L;
            pCurrentMode->imageWidth = pMode->HDisplay;
            pCurrentMode->imageHeight =  pMode->VDisplay;
            pCurrentMode->pixmapWidth = pCurrentMode->imageWidth;
            pCurrentMode->pixmapHeight = pCurrentMode->imageHeight;
            pCurrentMode->maxViewportX = pCurrentMode->imageWidth - pCurrentMode->viewportWidth;
            /* this might need to get clamped to some maximum */
            pCurrentMode->maxViewportY = pCurrentMode->imageHeight - pCurrentMode->viewportHeight;
            oneMore = FALSE;
            goto SECOND_PASS;
        }
        else
        {
            pCurrentMode->bytesPerScanline = ((pScrn->displayWidth * bpp) + 3) & ~3L;
            pCurrentMode->imageWidth = pScrn->displayWidth;
            pCurrentMode->imageHeight =  pMode->VDisplay;
            pCurrentMode->pixmapWidth = pCurrentMode->imageWidth;
            pCurrentMode->pixmapHeight = pCurrentMode->imageHeight;
            pCurrentMode->maxViewportX = pCurrentMode->imageWidth - pCurrentMode->viewportWidth;
            /* this might need to get clamped to some maximum */
            pCurrentMode->maxViewportY = pCurrentMode->imageHeight - pCurrentMode->viewportHeight;
        }

        pMode = pMode->next;
        if(pMode == pFirstMode)
            break;
    }

    pXGI->numDgaModes = num;
    pXGI->pDgaModes = pDgaModes;

    return DGAInit(pScreen, &XGIDGAFuncs, pDgaModes, num);
}

static Bool XGIDGASetMode(ScrnInfoPtr pScrn, DGAModePtr pMode)
{
    static int  oldDisplayWidth[MAXSCREENS];
    int         index = pScrn->pScreen->myNum;
    XGIPtr      pXGI = XGIPTR(pScrn);

    if (!pMode)
    {
        /* restore the original mode */
        /* put the ScreenParameters back */
        pScrn->displayWidth = oldDisplayWidth[index];

        XGISwitchMode(SWITCH_MODE_ARGS(pScrn, pScrn->currentMode));
        pXGI->isDgaActive = FALSE;
   }
   else
   {
        if (!pXGI->isDgaActive)
        {
            /* save the old parameters */
            oldDisplayWidth[index] = pScrn->displayWidth;
            /*fprintf(stderr,"pXGI->isDgaActive is FALSE makin it TRUE\n");*/
            pXGI->isDgaActive = TRUE;
        }

        pScrn->displayWidth = pMode->bytesPerScanline /
                              (pMode->bitsPerPixel >> 3);

        XGISwitchMode(SWITCH_MODE_ARGS(pScrn, pMode->mode));
   }

   return TRUE;
}

static int XGIDGAGetViewport(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    return pXGI->dgaViewportStatus;
}

static void XGIDGASetViewport(ScrnInfoPtr pScrn,
                            int x, int y,
                            int flags)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    XGIAdjustFrame(ADJUST_FRAME_ARGS(pScrn, x, y));
    pXGI->dgaViewportStatus = 0;  /* TRIDENTAdjustFrame loops until finished */
}

#ifdef HAVE_XAA_H
static void XGIDGAFillRect(ScrnInfoPtr pScrn,
                         int x, int y,
                         int w, int h,
                         unsigned long color)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    if(pXGI->pXaaInfo)
    {
        (*pXGI->pXaaInfo->SetupForSolidFill)(pScrn, color, GXcopy, ~0);
        (*pXGI->pXaaInfo->SubsequentSolidFillRect)(pScrn, x, y, w, h);
        SET_SYNC_FLAG(pXGI->pXaaInfo);
    }
}

static void XGIDGASync(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    if(pXGI->pXaaInfo)
    {
        (*pXGI->pXaaInfo->Sync)(pScrn);
    }
}

static void XGIDGABlitRect(ScrnInfoPtr pScrn,
                         int srcx, int srcy,
                         int w, int h,
                         int dstx, int dsty)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    if(pXGI->pXaaInfo)
    {
        int xdir = ((srcx < dstx) && (srcy == dsty)) ? -1 : 1;
        int ydir = (srcy < dsty) ? -1 : 1;

        (*pXGI->pXaaInfo->SetupForScreenToScreenCopy)(
                    pScrn, xdir, ydir, GXcopy, ~0, -1);
        (*pXGI->pXaaInfo->SubsequentScreenToScreenCopy)(
                   pScrn, srcx, srcy, dstx, dsty, w, h);
        SET_SYNC_FLAG(pXGI->pXaaInfo);
    }
}

#if 0
static void
XGIDGABlitTransRect(ScrnInfoPtr pScrn,
                  int srcx, int srcy,
                  int w, int h,
                  int dstx, int dsty,
                  unsigned long color)
{
}
#endif
#endif
static Bool
XGIDGAOpenFramebuffer(ScrnInfoPtr pScrn,
                    char **name,
                    unsigned char **mem,
                    int *size,
                    int *offset,
                    int *flags)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    *name = NULL;             /* no special device */
    *mem = (unsigned char*) pScrn->memPhysBase;
    *size = pXGI->fbSize;
    *offset = 0;
    *flags = DGA_NEED_ROOT;

    return TRUE;
}


