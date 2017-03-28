/*
 * Copyright (C) 2010 Kim Woelders
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
/*
 * Based on hack by raster - ecore_x/xlib/ecore_x.c.
 */
#include <dlfcn.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include "util.h"

typedef             Status(RF) (Display * dpy, void *rep, int extra,
				Bool discard);

/* find the real Xlib and the real X function */
static void        *
GetFunc(const char *name)
{
   void               *lib_xlib;
   void               *func;

   lib_xlib = dlopen("libX11.so", RTLD_GLOBAL | RTLD_LAZY);

   func = dlsym(lib_xlib, name);

   return func;
}

extern Status       _XReply(Display * dpy, void *rep, int extra, Bool discard);

__EXPORT__          Status
_XReply(Display * dpy, void *rep, int extra, Bool discard)
{
   static RF          *func = NULL;

   char                s[1024], *p, *name;
   void               *bt[128];
   int                 i, n, l;
   char              **sym;

   if (!func)
      func = (RF *) GetFunc("_XReply");

   l = 0;
   l += snprintf(s + l, sizeof(s) - l, "RT: ");
   n = backtrace(bt, 128);
   if (n <= 0)
      goto done;

   sym = backtrace_symbols(bt, n);
   if (!sym)
      goto done;

   for (i = 1; i < n; i++)
     {
#if 1
	name = strchr(sym[i], '(');
	if (name)
	  {
	     name++;
	     p = strchr(name, '+');
	     if (!p)
		p = strchr(name, ')');
	     if (p)
		*p = '\0';
	  }
	if (!name || *name == '\0')
	   name = (char *)"?";
	l += snprintf(s + l, sizeof(s) - l, name);
#else
	l += snprintf(s + l, sizeof(s) - l, sym[i]);
#endif

	if (i < n - 1)
	   l += snprintf(s + l, sizeof(s) - l, " < ");
     }
   free(sym);

 done:
   printf("%s\n", s);

   return func(dpy, rep, extra, discard);
}
