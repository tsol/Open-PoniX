/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2008-2011 Kim Woelders
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dox.h"

int
exists(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return (1);
}

void
freestrlist(char **l, int num)
{
   if (!l)
      return;
   while (num--)
      Efree(l[num]);
   Efree(l);
}

void
word(char *s, int num, char *wd)
{
   int                 cnt, i;
   char               *start, *finish, *ss;

   if (!s)
      return;
   if (!wd)
      return;
   if (num <= 0)
     {
	*wd = 0;
	return;
     }
   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;

   while (s[i])
     {
	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t')))
	  {
	     finish = &s[i];
	     break;
	  }
	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if (i == 0)
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	     else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	  }
	i++;
     }
   if (cnt == num)
     {
	if ((start) && (finish))
	  {
	     for (ss = start; ss < finish; ss++)
		*wd++ = *ss;
	  }
	else if (start)
	  {
	     for (ss = start; *ss != 0; ss++)
		*wd++ = *ss;
	  }
	*wd = 0;
     }
}
