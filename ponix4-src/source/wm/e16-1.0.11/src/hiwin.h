/*
 * Copyright (C) 2005-2008 Kim Woelders
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
#ifndef _HIWIN_H_
#define _HIWIN_H_

#include <X11/Xlib.h>

typedef struct _hiwin Hiwin;

Hiwin              *HiwinCreate(void);
void                HiwinSetGeom(Hiwin * phi, int x, int y, int w, int h);
void                HiwinInit(Hiwin * phi, EWin * ewin);
void                HiwinSetCallback(Hiwin * phi,
				     void (*func) (Win win, XEvent * ev,
						   void *data), void *data);
void                HiwinGetXY(Hiwin * phi, int *x, int *y);
void                HiwinMove(Hiwin * phi, int x, int y);
EWin               *HiwinGetEwin(Hiwin * phi, int check);
void                HiwinShow(Hiwin * phi, EWin * ewin, int zoom, int confine);
void                HiwinHide(Hiwin * phi);

#endif /* _HIWIN_H_ */
