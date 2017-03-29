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

#ifndef _XG47_MODE_H_
#define _XG47_MODE_H_

extern void XG47LoadPalette(ScrnInfoPtr pScrn, int numColors, int *indicies,
                            LOCO *colors, VisualPtr pVisual);
extern void XG47SetOverscan(ScrnInfoPtr pScrn, int overscan);
extern unsigned int XG47DDCRead(ScrnInfoPtr pScrn);
extern int XG47ValidMode(ScrnInfoPtr pScrn, DisplayModePtr dispMode);

extern void xg47_mode_save(ScrnInfoPtr pScrn, vgaRegPtr pVgaReg, XGIRegPtr pXGIReg);
extern void xg47_mode_restore(ScrnInfoPtr pScrn, vgaRegPtr pVgaReg, XGIRegPtr pXGIReg);
extern Bool XG47_NativeModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);

extern void SetColorDAC(XGIPtr pXGI, unsigned color_depth, XGIRegPtr regs);
extern void SetModeCRTC1(XGIPtr pXGI, DisplayModePtr disp_mode, XGIRegPtr regs);

#endif
