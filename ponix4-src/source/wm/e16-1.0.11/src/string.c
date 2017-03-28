/*
 * Copyright (C) 2008 Kim Woelders
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
#include "util.h"
#include <ctype.h>

#ifndef HAVE_STRCASECMP
int
Estrcasecmp(const char *s1, const char *s2)
{
   char                ch1, ch2;

   for (;;)
     {
	ch1 = toupper(*s1++);
	ch2 = toupper(*s2++);
	if (ch1 == '\0' || ch1 != ch2)
	   break;
     }

   return ch1 - ch2;
}
#endif

#ifndef HAVE_STRCASESTR
const char         *
Estrcasestr(const char *haystack, const char *needle)
{
   const char         *s1, *s2;
   char                ch1, ch2;

   for (;; haystack++)
     {
	s1 = haystack;
	s2 = needle;
	if (*s1 == '\0')
	   break;
	for (;;)
	  {
	     ch1 = toupper(*s1++);
	     ch2 = toupper(*s2++);
	     if (ch2 == '\0')
		return haystack;
	     if (ch1 == '\0' || ch1 != ch2)
		break;
	  }
     }
   return NULL;
}
#endif
