/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2012 Kim Woelders
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
#ifndef _UTIL_H_
#define _UTIL_H_

#include "config.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT2PTR(i) ((void*)(long)(i))
#define PTR2INT(p) ((int)(long)(p))

/* Inspired by Xfuncproto.h */
#if defined(__GNUC__) && ((__GNUC__ * 100 + __GNUC_MINOR__) >= 303)
#define __EXPORT__      __attribute__((visibility("default")))
#elif defined(__SUNPRO_C) && (__SUNPRO_C >= 0x550)
#define __EXPORT__      __global
#else /* not gcc >= 3.3 and not Sun Studio >= 8 */
#define __EXPORT__
#endif

#if HAVE___ATTRIBUTE__
#define __PRINTF_N__(no)  __attribute__((__format__(__printf__, (no), (no)+1)))
#define __NORETURN__      __attribute__((noreturn))
#else
#define __PRINTF_N__(no)
#define __NORETURN__
#endif
#define __PRINTF__   __PRINTF_N__(1)
#define __PRINTF_2__ __PRINTF_N__(2)
#define __PRINTF_5__ __PRINTF_N__(5)

#if HAVE_STRDUP
#define USE_LIBC_STRDUP  1	/* Use libc strdup if present */
#endif
#if HAVE_STRNDUP
#define USE_LIBC_STRNDUP 1	/* Use libc strndup if present */
#endif

#ifdef HAVE_STRCASECMP
#define Estrcasecmp(s1, s2) strcasecmp(s1, s2)
#else
int                 Estrcasecmp(const char *s1, const char *s2);
#endif
#ifdef HAVE_STRCASESTR
#define Estrcasestr(haystack, needle) strcasestr(haystack, needle)
#else
const char         *Estrcasestr(const char *haystack, const char *needle);
#endif

/* memory.c */
#define Ecalloc     calloc
#define Emalloc     malloc
#define Erealloc    realloc
#if HAVE_FREE_NULL_BUG
#define Efree(p)    if (p) free(p)
#else
#define Efree       free
#endif

#define ECALLOC(type, num) (type*)Ecalloc(num, sizeof(type))
#define EMALLOC(type, num) (type*)Emalloc((num)*sizeof(type))
#define EREALLOC(type, ptr, num) (type*)Erealloc(ptr, (num)*sizeof(type))

#define _EFREE(p)    do { if (p) { Efree(p); p = NULL; } } while (0)
#define _EFDUP(p, s) do { if (p) Efree(p); p = Estrdup(s); } while (0)

#define STRCPY(dst, src) do { src[sizeof(dst)-1] = '\0'; strcpy(dst, src); } while(0)

char               *Estrtrim(char *s);

char               *Estrdup(const char *s);
char               *Estrndup(const char *s, size_t n);
char               *Estrdupcat2(char *ss, const char *s1, const char *s2);

char              **StrlistDup(char **lst, int num);
__EXPORT__ void     StrlistFree(char **lst, int num);
char               *StrlistJoin(char **lst, int num);
char               *StrlistEncodeEscaped(char *buf, int len, char **lst,
					 int num);
char              **StrlistDecodeEscaped(const char *str, int *pnum);
__EXPORT__ char   **StrlistFromString(const char *str, int delim, int *num);

void                Esetenv(const char *name, const char *value);

/* misc.c */
__EXPORT__ void __PRINTF__ Eprintf(const char *fmt, ...);

#ifdef HAVE_SNPRINTF
#define Evsnprintf vsnprintf
#define Esnprintf snprintf
#else /* HAVE_SNPRINTF */
int                 Evsnprintf(char *str, size_t count, const char *fmt,
			       va_list args);

#ifdef HAVE_STDARG_H
int                 Esnprintf(char *str, size_t count, const char *fmt, ...);

#else
int                 Esnprintf(va_alist);
#endif
#endif /* HAVE_SNPRINTF */

#if USE_MODULES
/* Dynamic loading */
const void         *ModLoadSym(const char *lib, const char *sym,
			       const char *name);
#endif

unsigned int        GetTimeMs(void);
unsigned int        GetTimeUs(void);

#endif /* _UTIL_H_ */
