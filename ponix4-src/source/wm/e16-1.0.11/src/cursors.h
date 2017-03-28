/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2009 Kim Woelders
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
#ifndef _CURSORS_H_
#define _CURSORS_H_

#include "etypes.h"

/* cursors.c */
#define ECSR_ROOT           0
#define ECSR_GRAB           1
#define ECSR_PGRAB          2
#define ECSR_ACT_MOVE       3
#define ECSR_ACT_RESIZE     4
#define ECSR_ACT_RESIZE_H   5
#define ECSR_ACT_RESIZE_V   6
#define ECSR_ACT_RESIZE_TL  7
#define ECSR_ACT_RESIZE_TR  8
#define ECSR_ACT_RESIZE_BL  9
#define ECSR_ACT_RESIZE_BR 10
#define ECSR_COUNT         11

ECursor            *ECursorAlloc(const char *name);
void                ECursorFree(ECursor * ec);
void                ECursorApply(ECursor * ec, Win win);
Cursor              ECsrGet(int which);
void                ECsrApply(int which, Window win);

int                 ECursorConfigLoad(FILE * fs);

#endif /* _CURSORS_H_ */
