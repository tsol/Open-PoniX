/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "config.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>

#if HAVE_STRDUP
#define USE_LIBC_STRDUP  1	/* Use libc strdup if present */
#endif

typedef struct {
   Window              win;
   char               *msg;
} Client;

void                CommsInit(void);
Window              CommsSetup(Window win);
Window              CommsFindCommsWindow(void);
void                CommsSend(Client * c, const char *s);
char               *CommsGet(Client * c, XEvent * ev);
Client             *ClientCreate(Window win);
void                ClientDestroy(Client * c);

#define Ecalloc     calloc
#define Emalloc     malloc
#define Erealloc    realloc
#if HAVE_FREE_NULL_BUG
#define Efree(p)    if (p) free(p)
#else
#define Efree       free
#endif

#define EMALLOC(type, num) (type*)Emalloc((num)*sizeof(type))
#define EREALLOC(type, ptr, num) (type*)Erealloc(ptr, (num)*sizeof(type))

#if USE_LIBC_STRDUP
#define Estrdup(s) ((s) ? strdup(s) : NULL)
#else
char               *Estrdup(const char *s);
#endif

extern Display     *disp;
