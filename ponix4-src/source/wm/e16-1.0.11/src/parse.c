/*
 * Copyright (C) 2007 Kim Woelders
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
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

int
parse(char *buf, const char *fmt, ...)
{
   int                 nitems;
   char                chi, chf, chq;
   char               *p, **ps;
   va_list             args;

   va_start(args, fmt);

   for (nitems = 0;;)
     {
	chf = *fmt++;
	if (chf != '%')
	   break;
	chf = *fmt++;
	if (chf == '\0')
	   break;
	/* Strip leading whitespace */
	while (isspace(*buf))
	   buf++;
	if (!*buf)
	   break;
	switch (chf)
	  {
	  case 'S':		/* Return pointer to string */
	  case 'T':		/* As S, convert "NULL" to NULL pointer */
	     chi = *buf;
	     chq = (chi == '\'' || chi == '"') ? chi : '\0';
	     if (chq)
	       {
		  /* Token is quoted */
		  buf++;
		  for (p = buf;; p++)
		    {
		       p = strchr(p, chq);
		       if (p)
			 {
			    if (p[1] && !isspace(p[1]))
			       continue;
			    *p++ = '\0';	/* Terminate at quote */
			 }
		       else
			 {
			    p = buf + strlen(buf);	/* Missing end quote */
			 }
		       break;
		    }
	       }
	     else
	       {
		  /* Token is unquoted */
		  p = buf + 1;
		  while (*p && !isspace(*p))
		     p++;
		  if (*p)
		     *p++ = '\0';
	       }
	     ps = va_arg(args, char **);

	     if (chf == 'T' && (!buf[0] || !strcmp(buf, "NULL")))
		*ps = NULL;
	     else
		*ps = buf;
	     nitems++;
	     buf = p;
	  }
     }

   va_end(args);

   return nitems;
}
