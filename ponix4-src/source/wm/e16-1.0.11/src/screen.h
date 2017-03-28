/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2011 Kim Woelders
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
#ifndef _SCREEN_H_
#define _SCREEN_H_

/* screen.c */
void                ScreenInit(void);
void                ScreenAdd(int type, int head, int x, int y, unsigned int w,
			      unsigned int h);
void                ScreenSplit(unsigned int nx, unsigned int ny);
void                ScreenShowInfo(const char *prm);
int                 ScreenGetGeometry(int x, int y, int *px, int *py,
				      int *pw, int *ph);
void                ScreenGetGeometryByHead(int head, int *px, int *py,
					    int *pw, int *ph);
int                 ScreenGetAvailableArea(int x, int y, int *px, int *py,
					   int *pw, int *ph, int ignore_struts);
int                 ScreenGetGeometryByPointer(int *px, int *py,
					       int *pw, int *ph);
int                 ScreenGetAvailableAreaByPointer(int *px, int *py,
						    int *pw, int *ph,
						    int ignore_struts);

#endif /* _SCREEN_H_ */
