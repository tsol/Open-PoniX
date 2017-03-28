/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
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
#ifndef _LANG_H_
#define _LANG_H_

#include <stdlib.h>

#include "config.h"

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(String) gettext(String)
#ifdef gettext_noop
#define N_(String) gettext_noop(String)
#else
#define N_(String) (String)
#endif
#else
#define _(String) (String)
#define N_(String) (String)
#endif

/* lang.c */
void                LangInit(void);
void                LangExit(void);
void                LangExport(void);

char               *EstrUtf82Int(const char *str, int len);
const char         *EstrInt2Enc(const char *str, int want_utf8);
void                EstrInt2EncFree(const char *str, int want_utf8);

int                 EwcOpen(int utf8);
void                EwcClose(void);
int                 EwcStrToWcs(const char *str, int len, wchar_t * wcs,
				int wcl);
int                 EwcWcsToStr(const wchar_t * wcs, int wcl, char *str,
				int len);

#endif /* _LANG_H_ */
