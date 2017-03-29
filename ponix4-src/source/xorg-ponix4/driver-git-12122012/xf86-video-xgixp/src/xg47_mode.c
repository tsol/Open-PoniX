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

#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_bios.h"
#include "xg47_mode.h"

/* Jong 09/15/2006; support dual view; default is single mode */
int		g_DualViewMode=0;


void XG47LoadPalette(ScrnInfoPtr pScrn, int numColors, int *indicies,
                     LOCO *colors, VisualPtr pVisual)
{
    vgaHWPtr pVgaHW = VGAHWPTR(pScrn);
    XGIPtr   pXGI = XGIPTR(pScrn);
    int      i, index;

    for(i = 0; i < numColors; i++)
    {
        index = indicies[i];
        OUTB(0x3C6, 0xFF);
        DACDelay(pVgaHW);
        OUTB(0x3c8, index);
        DACDelay(pVgaHW);
        OUTB(0x3c9, colors[index].red);
        DACDelay(pVgaHW);
        OUTB(0x3c9, colors[index].green);
        DACDelay(pVgaHW);
        OUTB(0x3c9, colors[index].blue);
        DACDelay(pVgaHW);
    }
}

void XG47SetOverscan(ScrnInfoPtr pScrn, int overscan)
{
    vgaHWPtr pVgaHW = VGAHWPTR(pScrn);

    if (overscan < 0 || overscan > 255)
        return;

    pVgaHW->enablePalette(pVgaHW);
    pVgaHW->writeAttr(pVgaHW, OVERSCAN, overscan);
    pVgaHW->disablePalette(pVgaHW);
}

unsigned int XG47DDCRead(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    /* Define SDA as input */
    OUT3X5B(0x37, 0x04);

    /* Wait until vertical retrace is in progress. */
    while (INB(0x3DA) & 0x08);
    while (!(INB(0x3DA) & 0x08));

    /* Get the result */ 
   return IN3X5B(0x37) & 0x01;
}


/**
 * Validate a mode passed from the core X server.
 * 
 * \sa xf86ValidateModes, xf86InitialCheckModeForDriver
 */
int XG47ValidMode(ScrnInfoPtr pScrn, DisplayModePtr dispMode)
{
    XGIPtr pXGI = XGIPTR(pScrn);

    /* judge the mode on CRT, DVI */
    if ((pXGI->displayDevice & ST_DISP_CRT) 
        || (IN3CFB(0x5B) & ST_DISP_CRT)
        || (pXGI->displayDevice & ST_DISP_DVI)
        || (IN3CFB(0x5B) & ST_DISP_DVI)) {
        if (dispMode->HDisplay == 1400) {
            return MODE_PANEL;
        }
    }

    return MODE_OK;
}
