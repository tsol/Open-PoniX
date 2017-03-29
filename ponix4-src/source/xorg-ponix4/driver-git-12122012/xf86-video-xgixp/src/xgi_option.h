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

#ifndef _XGI_OPTION_H
#define _XGI_OPTION_H

#include "xf86str.h"

typedef enum {
    OPTION_SW_CURSOR,
    OPTION_NOACCEL,
    OPTION_SHADOW_FB,
    OPTION_ROTATE,
    OPTION_VIDEO_KEY,
    OPTION_NOMMIO,
    OPTION_MMIO_ONLY,
    OPTION_DISPLAY,
    OPTION_DAC_8BIT,
    OPTION_SETMCLK,
    OPTION_NEED_SHADOW,
    OPTION_NEED_STRETCH,
    OPTION_MUX_THRESHOLD,
    OPTION_GAMMA_BRIGHT,
#ifdef __DEBUG_FUNC__
    OPTION_SOLID_LINE,
    OPTION_SOLID_FILL,
    OPTION_SRC_TO_SRC,
    OPTION_8X8_PATTERN,
    OPTION_8X8_MONOPAT,
    OPTION_IMAGE_WRITE,
#endif
    OPTION_NULL
} XGIOpts;

extern const OptionInfoRec   XGIOptions[];
extern const unsigned long   XGIOptionSize;
extern const OptionInfoRec * XGIAvailableOptions(int chipid, int busid);
extern Bool XGIProcessOptions(ScrnInfoPtr pScrn);

#endif /* ___XGI_OPTION_H___ */


