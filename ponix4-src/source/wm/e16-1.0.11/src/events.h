/*
 * Copyright (C) 2006-2012 Kim Woelders
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
#ifndef _EVENTS_H_
#define _EVENTS_H_

#include <X11/Xlib.h>

/* Re-mapped X-events */
#define EX_EVENT_CREATE_GONE             64
#define EX_EVENT_UNMAP_GONE              65
#define EX_EVENT_MAP_GONE                66
#define EX_EVENT_MAPREQUEST_GONE         67
#define EX_EVENT_REPARENT_GONE           68

#define EX_EVENT_SHAPE_NOTIFY            71
#define EX_EVENT_SAVER_NOTIFY            72
#define EX_EVENT_SCREEN_CHANGE_NOTIFY    73
#define EX_EVENT_DAMAGE_NOTIFY           74
#define EX_EVENT_GLX_FLIP                75

void                EventsInit(void);
void                EventsMain(void);
void                EventShow(const XEvent * ev);
void                EventShowError(const XErrorEvent * ev);

typedef struct _EventFdDesc EventFdDesc;
typedef void        (EventFdHandler) (void);
EventFdDesc        *EventFdRegister(int fd, EventFdHandler * handler);
void                EventFdUnregister(EventFdDesc * efd);

int                 EventsUpdateXY(int *px, int *py);

#endif /* _EVENTS_H_ */
