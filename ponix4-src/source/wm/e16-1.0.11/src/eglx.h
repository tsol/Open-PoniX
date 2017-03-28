/*
 * Copyright (C) 2007-2010 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _EGLX_H_
#define _EGLX_H_

#include "eimage.h"
#include "xwin.h"

typedef struct _glhook ETexture;

struct _glhook {
   unsigned int        texture;
   unsigned short      target;
   unsigned char       type;
   unsigned int        glxpmap;
};

int                 EGlInit(void);
void                EGlExit(void);

Visual             *EGlGetVisual(void);
unsigned int        EGlGetDepth(void);

void                EGlWindowConnect(Window xwin);
void                EGlWindowDisconnect(void);

ETexture           *EGlTextureFromImage(EImage * im, int mode);
ETexture           *EGlTextureFromDrawable(Drawable draw, int mode);
void                EGlTextureDestroy(ETexture * et);
void                EGlTextureInvalidate(ETexture * et);

#endif /* _EGLX_H_ */
