/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2008 Kim Woelders
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
#ifndef _GRABS_H_
#define _GRABS_H_

#include "xtypes.h"

int                 GrabKeyboardSet(Win win);
int                 GrabKeyboardRelease(void);
int                 GrabPointerSet(Win win, unsigned int csr, int confine);
void                GrabPointerRelease(void);
void                GrabButtonSet(unsigned int button, unsigned int modifiers,
				  Win win, unsigned int event_mask,
				  unsigned int csr, int confine);
void                GrabButtonRelease(unsigned int button,
				      unsigned int modifiers, Win win);
void                GrabKeySet(unsigned int key, unsigned int modifiers,
			       Win win);
void                GrabKeyRelease(unsigned int key, unsigned int modifiers,
				   Win win);

#endif /* _GRABS_H_ */
