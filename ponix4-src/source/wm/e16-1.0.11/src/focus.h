/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2010 Kim Woelders
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
#ifndef _FOCUS_H_
#define _FOCUS_H_

#include <X11/Xlib.h>

/* focus.c */
#define FOCUS_NOP         0
#define FOCUS_INIT        1
#define FOCUS_SET         2
#define FOCUS_NONE        3
#define FOCUS_ENTER       4
#define FOCUS_LEAVE       5
#define FOCUS_EWIN_NEW    6
#define FOCUS_EWIN_UNMAP  7
#define FOCUS_DESK_ENTER  8
#define FOCUS_DESK_LEAVE  9
#define FOCUS_NEXT       10
#define FOCUS_PREV       11
#define FOCUS_CLICK      12

void                FocusEnable(int on);
void                FocusToEWin(EWin * ewin, int why);
void                FocusHandleEnter(EWin * ewin, XEvent * ev);
void                FocusHandleLeave(EWin * ewin, XEvent * ev);
void                FocusHandleChange(EWin * ewin, XEvent * ev);
void                FocusHandleClick(EWin * ewin, Win win);
void                FocusNewDeskBegin(void);
void                FocusNewDesk(void);
void                FocusScreen(int scr);

void                ClickGrabsUpdate(void);

/* warp.c */
void                WarpFocus(int delta);

#endif /* _FOCUS_H_ */
