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
#include "shadowfb.h"
#include "servermd.h"
#include "xgi.h"


void XGIPointerMoved(SCRN_ARG_TYPE arg, int x, int y)
{
    SCRN_INFO_PTR(arg);
    XGIPtr          pXGI = XGIPTR(pScrn);
    int             newX, newY;

    if(pXGI->rotate == 1)
    {
        newX = pScrn->pScreen->height - y - 1;
        newY = x;
    }
    else
    {
        newX = y;
        newY = pScrn->pScreen->width - x - 1;
    }

    (*pXGI->PointerMoved)(arg, newX, newY);
}
void XGIRefreshArea(ScrnInfoPtr pScrn, int num, BoxPtr pbox)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int     width, height, bpp, fbPitch;
    CARD8   *src, *dst;

    bpp = pScrn->bitsPerPixel >> 3;
    fbPitch = BitmapBytePad(pScrn->displayWidth * pScrn->bitsPerPixel);

    while(num--)
    {
        width = (pbox->x2 - pbox->x1) * bpp;
        height = pbox->y2 - pbox->y1;
        src = pXGI->pShadow + (pbox->y1 * pXGI->shadowPitch) + (pbox->x1 * bpp);
        dst = pXGI->fbBase + (pbox->y1 * fbPitch) + (pbox->x1 * bpp);

        while(height--)
        {
            memcpy(dst, src, width);
            dst += fbPitch;
            src += pXGI->shadowPitch;
        }

        pbox++;
    }
}

void XGIShadowUpdate (ScreenPtr pScreen, shadowBufPtr pBuf)
{
    RegionPtr       pRegion = &pBuf->damage;
    ScrnInfoPtr     pScrn;
    pScrn = xf86ScreenToScrn(pScreen);

    (XGIPTR(pScrn))->RefreshArea (pScrn, REGION_NUM_RECTS(pRegion),
                                  REGION_RECTS(pRegion));
}

void XGIRefreshArea8(ScrnInfoPtr pScrn, int num, BoxPtr pbox)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int     count, width, height, y1, y2, dstPitch, srcPitch;
    CARD8   *dstPtr, *srcPtr, *src;
    CARD32 *dst;

    dstPitch = pScrn->displayWidth;
    srcPitch = -pXGI->rotate * pXGI->shadowPitch;

    while(num--)
    {
        width = pbox->x2 - pbox->x1;
        y1 = pbox->y1 & ~3;
        y2 = (pbox->y2 + 3) & ~3;
        height = (y2 - y1) >> 2;  /* in dwords */

        if(pXGI->rotate == 1)
        {
            dstPtr = pXGI->fbBase + (pbox->x1 * dstPitch) + pScrn->virtualX - y2;
            srcPtr = pXGI->pShadow + ((1 - y2) * srcPitch) + pbox->x1;
        }
        else
        {
            dstPtr = pXGI->fbBase + ((pScrn->virtualY - pbox->x2) * dstPitch) + y1;
            srcPtr = pXGI->pShadow + (y1 * srcPitch) + pbox->x2 - 1;
        }

        while(width--)
        {
            src = srcPtr;
            dst = (CARD32*)dstPtr;
            count = height;
            while(count--)
            {
                *(dst++) = src[0]
                           | (src[srcPitch] << 8)
                           | (src[srcPitch * 2] << 16)
                           | (src[srcPitch * 3] << 24);
                src += srcPitch * 4;
            }
            srcPtr += pXGI->rotate;
            dstPtr += dstPitch;
        }

        pbox++;
    }
}


void XGIRefreshArea16(ScrnInfoPtr pScrn, int num, BoxPtr pbox)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int     count, width, height, y1, y2, dstPitch, srcPitch;
    CARD16  *dstPtr, *srcPtr, *src;
    CARD32  *dst;

    dstPitch = pScrn->displayWidth;
    srcPitch = -pXGI->rotate * pXGI->shadowPitch >> 1;

    while(num--)
    {
        width = pbox->x2 - pbox->x1;
        y1 = pbox->y1 & ~1;
        y2 = (pbox->y2 + 1) & ~1;
        height = (y2 - y1) >> 1;  /* in dwords */

        if(pXGI->rotate == 1)
        {
            dstPtr = (CARD16*)pXGI->fbBase + (pbox->x1 * dstPitch) + pScrn->virtualX - y2;
            srcPtr = (CARD16*)pXGI->pShadow + ((1 - y2) * srcPitch) + pbox->x1;
        }
        else
        {
            dstPtr = (CARD16*)pXGI->fbBase + ((pScrn->virtualY - pbox->x2) * dstPitch) + y1;
            srcPtr = (CARD16*)pXGI->pShadow+ (y1 * srcPitch) + pbox->x2 - 1;
        }

        while(width--)
        {
            src = srcPtr;
            dst = (CARD32*)dstPtr;
            count = height;
            while(count--)
            {
                *(dst++) = src[0] | (src[srcPitch] << 16);
                src += srcPitch * 2;
            }
            srcPtr += pXGI->rotate;
            dstPtr += dstPitch;
        }

      pbox++;
    }
}


/* this one could be faster */
void XGIRefreshArea24(ScrnInfoPtr pScrn, int num, BoxPtr pbox)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int     count, width, height, y1, y2, dstPitch, srcPitch;
    CARD8   *dstPtr, *srcPtr, *src;
    CARD32  *dst;

    dstPitch = BitmapBytePad(pScrn->displayWidth * 24);
    srcPitch = -pXGI->rotate * pXGI->shadowPitch;

    while(num--)
    {
        width = pbox->x2 - pbox->x1;
        y1 = pbox->y1 & ~3;
        y2 = (pbox->y2 + 3) & ~3;
        height = (y2 - y1) >> 2;  /* blocks of 3 dwords */

        if(pXGI->rotate == 1)
        {
            dstPtr = pXGI->fbBase + (pbox->x1 * dstPitch) + ((pScrn->virtualX - y2) * 3);
            srcPtr = pXGI->pShadow + ((1 - y2) * srcPitch) + (pbox->x1 * 3);
        }
        else
        {
            dstPtr = pXGI->fbBase + ((pScrn->virtualY - pbox->x2) * dstPitch) + (y1 * 3);
            srcPtr = pXGI->pShadow + (y1 * srcPitch) + (pbox->x2 * 3) - 3;
        }

        while(width--)
        {
            src = srcPtr;
            dst = (CARD32*)dstPtr;
            count = height;
            while(count--)
            {
                dst[0] = src[0]
                         | (src[1] << 8)
                         | (src[2] << 16)
                         | (src[srcPitch] << 24);
                dst[1] = src[srcPitch + 1]
                         | (src[srcPitch + 2] << 8)
                         | (src[srcPitch * 2] << 16)
                         | (src[(srcPitch * 2) + 1] << 24);
                dst[2] = src[(srcPitch * 2) + 2]
                         | (src[srcPitch * 3] << 8)
                         | (src[(srcPitch * 3) + 1] << 16)
                         | (src[(srcPitch * 3) + 2] << 24);
                dst += 3;
                src += srcPitch * 4;
            }
            srcPtr += pXGI->rotate * 3;
            dstPtr += dstPitch;
        }
        pbox++;
    }
}

void XGIRefreshArea32(ScrnInfoPtr pScrn, int num, BoxPtr pbox)
{
    XGIPtr  pXGI = XGIPTR(pScrn);
    int     count, width, height, dstPitch, srcPitch;
    CARD32  *dstPtr, *srcPtr, *src, *dst;

    dstPitch = pScrn->displayWidth;
    srcPitch = -pXGI->rotate * pXGI->shadowPitch >> 2;

    while(num--)
    {
        width = pbox->x2 - pbox->x1;
        height = pbox->y2 - pbox->y1;

        if(pXGI->rotate == 1)
        {
            dstPtr = (CARD32*)pXGI->fbBase + (pbox->x1 * dstPitch) + pScrn->virtualX - pbox->y2;
            srcPtr = (CARD32*)pXGI->pShadow + ((1 - pbox->y2) * srcPitch) + pbox->x1;
        }
        else
        {
            dstPtr = (CARD32*)pXGI->fbBase + ((pScrn->virtualY - pbox->x2) * dstPitch) + pbox->y1;
            srcPtr = (CARD32*)pXGI->pShadow + (pbox->y1 * srcPitch) + pbox->x2 - 1;
        }

        while(width--)
        {
            src = srcPtr;
            dst = dstPtr;
            count = height;
            while(count--)
            {
                *(dst++) = *src;
                src += srcPitch;
            }
            srcPtr += pXGI->rotate;
            dstPtr += dstPitch;
        }
      pbox++;
    }
}


