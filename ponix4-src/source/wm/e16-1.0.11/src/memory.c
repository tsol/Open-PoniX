/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2005-2008 Kim Woelders
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
#include "E.h"
#include <ctype.h>

char               *
Estrtrim(char *s)
{
   int                 l;

   while (*s == ' ')
      s++;
   if (!*s)
      return s;

   l = strlen(s);
   while (isspace(s[l - 1]))
      l--;
   s[l] = '\0';

   return s;
}

char               *
Estrdup(const char *s)
{
#if USE_LIBC_STRDUP
   if (s)
      return strdup(s);
   return NULL;
#else
   int                 sz;

   if (!s)
      return NULL;
   sz = strlen(s);
   ss = EMALLOC(char, sz + 1);
   strncpy(ss, s, sz + 1);
   return ss;
#endif
}

char               *
Estrndup(const char *s, size_t n)
{
#if USE_LIBC_STRNDUP
   if (s)
      return strndup(s, n);
   return NULL;
#else
   char               *ss;

   if (!s)
      return NULL;
   ss = EMALLOC(char, n + 1);
   strncpy(ss, s, n);
   ss[n] = '\0';
   return ss;
#endif
}

char               *
Estrdupcat2(char *ss, const char *s1, const char *s2)
{
   char               *s;
   int                 len, l1, l2;

   if (!ss)
      return Estrdup(s2);

   len = strlen(ss);
   l1 = (s1) ? strlen(s1) : 0;
   l2 = (s2) ? strlen(s2) : 0;

   s = EREALLOC(char, ss, len + l1 + l2 + 1);
   if (!s)
      return NULL;
   if (s1 && l1)
      memcpy(s + len, s1, l1);
   if (s2 && l2)
      memcpy(s + len + l1, s2, l2);
   s[len + l1 + l2] = '\0';

   return s;
}

#if 0				/* Unused */
char              **
StrlistDup(char **lst, int num)
{
   char              **ss;
   int                 i;

   if (!lst || num <= 0)
      return NULL;

   ss = EMALLOC(char *, num + 1);
   for (i = 0; i < num; i++)
      ss[i] = Estrdup(lst[i]);
   ss[i] = NULL;

   return ss;
}
#endif

void
StrlistFree(char **lst, int num)
{
   if (!lst)
      return;
   while (num--)
      Efree(lst[num]);
   Efree(lst);
}

#if 0				/* FIXME - Remove? */
char               *
StrlistJoin(char **lst, int num)
{
   int                 i, size;
   char               *s;

   if (!lst || num <= 0)
      return NULL;

   s = NULL;

   size = strlen(lst[0]) + 1;
   s = EMALLOC(char, size);
   strcpy(s, lst[0]);
   for (i = 1; i < num; i++)
     {
	size += strlen(lst[i]) + 1;
	s = EREALLOC(char, s, size);

	strcat(s, " ");
	strcat(s, lst[i]);
     }

   return s;
}
#endif

char               *
StrlistEncodeEscaped(char *buf, int len, char **lst, int num)
{
   int                 i, j, ch;
   char               *s, *p;

   if (!lst || num <= 0)
      return NULL;

   j = 0;
   s = buf;
   p = lst[0];
   for (i = 0; i < len - 2; i++)
     {
	if (!p)			/* A string list should not contain NULL items */
	   break;

	ch = *p++;
	switch (ch)
	  {
	  default:
	     *s++ = ch;
	     break;
	  case '\0':
	     if (++j >= num)
		goto done;
	     p = lst[j];
	     if (!p || !p[0])
		goto done;
	     *s++ = ' ';
	     break;
	  case ' ':
	     *s++ = '\\';
	     *s++ = ' ';
	     i++;
	     break;
	  }
     }

 done:
   *s = '\0';
   return buf;
}

char              **
StrlistDecodeEscaped(const char *str, int *pnum)
{
   int                 num, len;
   const char         *s, *p;
   char              **lst;

   if (!str)
      return NULL;

   lst = NULL;
   num = 0;
   s = str;
   for (;;)
     {
	while (*s == ' ')
	   s++;
	if (*s == '\0')
	   break;

	lst = EREALLOC(char *, lst, num + 1);

	lst[num] = NULL;
	len = 0;

	for (;;)
	  {
	     p = strchr(s, ' ');
	     if (!p)
		p = s + strlen(s);

	     lst[num] = EREALLOC(char, lst[num], len + p - s + 1);

	     memcpy(lst[num] + len, s, p - s);
	     len += p - s;
	     lst[num][len] = '\0';

	     s = p;
	     if (p[-1] == '\\')
	       {
		  if (*p)
		     lst[num][len - 1] = ' ';
		  else
		     break;
	       }
	     else
	       {
		  break;
	       }
	     while (*s == ' ')
		s++;
	     if (*s == '\0')
		break;
	  }
	num++;
     }

   /* Append NULL item */
   lst = EREALLOC(char *, lst, num + 1);

   lst[num] = NULL;

   *pnum = num;
   return lst;
}

char              **
StrlistFromString(const char *str, int delim, int *num)
{
   const char         *s, *p;
   char              **lst;
   int                 n, len;

   lst = NULL;
   n = 0;
   for (s = str; s; s = p)
     {
	p = strchr(s, delim);
	if (p)
	  {
	     len = p - s;
	     p++;
	  }
	else
	  {
	     len = strlen(s);
	  }
	if (len <= 0)
	   continue;

	lst = EREALLOC(char *, lst, n + 2);

	lst[n++] = Estrndup(s, len);
     }

   if (lst)
      lst[n] = NULL;
   *num = n;
   return lst;
}

void
Esetenv(const char *name, const char *value)
{
   if (value)
     {
#if HAVE_SETENV
	setenv(name, value, 1);
#else
	char                buf[FILEPATH_LEN_MAX];

	Esnprintf(buf, FILEPATH_LEN_MAX, "%s=%s", name, value);
	putenv(Estrdup(buf));
#endif
     }
   else
     {
#if HAVE_UNSETENV
	unsetenv(name);
#else
	if (getenv(name))
	   putenv((char *)name);
#endif
     }
}
