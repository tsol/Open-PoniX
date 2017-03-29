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


#ifndef _XG47_CURSOR_H_
#define _XG47_CURSOR_H_

#define  ST_CURSORTYPE_MASK    0x000F
#define  ST_MONO_CURSOR        0x0001    /* mono cursor */
#define  ST_COLOR_CURSOR       0x0002    /* color cursor */
#define  ST_ALPHA_CURSOR       0x0004    /* alpha cursor */

#define  ST_CURSORSIZE_MASK    0x00F0
#define  ST_32X32_CURSOR       0x0020    /* 32x32 */
#define  ST_64X64_CURSOR       0x0040    /* 64x64 */
#define  ST_128X128_CURSOR     0x0080    /* 128x128 */

#define  ST_CURSOR_VISIBLE     0x0100    /* cursor is visible */

#define  ST_CURSORDIS_MASK     0xC000
#define  ST_CURSOR_ON_W2       0x8000    /* cursor on W2 */
#define  ST_CURSOR_ON_CRTC     0x4000    /* cursor on CRTC */

#define  CSOR_FMT_MASK         0x3000
#define  CSOR_FMT_A8R8B8G8     0x2000
#define  CSOR_FMT_A8R3B3G2     0x1000

#define  MAX_64X64_CURSOR_SIZE    0x4000
#define  MAX_128X128_CURSOR_SIZE  0x10000

#define  MAX_HW_HEADS               8    /* max of hardware externsion */

/*
 *-----------------------------------------------------------------------------
 * Cusor control registers
 *-----------------------------------------------------------------------------
 */
#define AGI_CURSOR_HLOW             0x40
#define AGI_CURSOR_HHIGH            0x41
#define AGI_CURSOR_VLOW             0x42
#define AGI_CURSOR_VHIGH            0x43
#define AGI_CURSOR_PATADRR_LOW      0x44
#define AGI_CURSOR_PATADRR_HIGH     0x45
#define AGI_CURSOR_HPRESET          0x46
#define AGI_CURSOR_VPRESET          0x47
#define AGI_CURSOR_FGCOLOR          0x48
#define AGI_CURSOR_BKCOLOR          0x4C
#define AGI_CURSOR_CONTROL          0x50

extern Bool XG47HWCursorInit(ScreenPtr pScreen);
extern void XG47HWCursorCleanup(ScreenPtr pScreen);

#endif
