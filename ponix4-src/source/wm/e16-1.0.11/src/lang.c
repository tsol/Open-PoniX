/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2011 Kim Woelders
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
#include "emodule.h"
#include "lang.h"
#include <X11/Xlib.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#if HAVE_LANGINFO_CODESET
#include <langinfo.h>
#endif

#ifndef ENABLE_NLS
#define bindtextdomain(pkg,locale)
#define textdomain(pkg)
#define bind_textdomain_codeset(pkg,enc)
#endif

#if HAVE_ICONV

#include <iconv.h>
#define BAD_CD ((iconv_t)-1)

#ifndef ICONV_CONST		/* Not sure this is necessary */
#define ICONV_CONST
#endif

static iconv_t      iconv_cd_int2utf8 = BAD_CD;
static iconv_t      iconv_cd_utf82int = BAD_CD;
static iconv_t      iconv_cd_int2loc = BAD_CD;
static iconv_t      iconv_cd_loc2int = BAD_CD;

static char        *
Eiconv(iconv_t icd, const char *txt, size_t len)
{
   char                buf[4096];
   ICONV_CONST char   *pi;
   char               *po;
   size_t              ni, no;

   pi = (ICONV_CONST char *)txt;
   po = buf;
   ni = (len > 0) ? len : strlen(txt);
   if (icd == BAD_CD)
      return Estrndup(txt, ni);
   no = sizeof(buf);
   iconv(icd, &pi, &ni, &po, &no);

   po = Estrndup(buf, sizeof(buf) - no);

   return po;
}

static              iconv_t
EiconvOpen(const char *to, const char *from)
{
   iconv_t             icd;

   icd = iconv_open(to, from);
   if (icd == BAD_CD)
      Eprintf("*** WARNING - Missing conversion %s->%s\n", from, to);

   return icd;
}

#endif

#if 0				/* Unused */
/* Convert locale to internal format (alloc always) */
char               *
EstrLoc2Int(const char *str, int len)
{
   if (str == NULL)
      return NULL;

#if HAVE_ICONV
   if (iconv_cd_loc2int != BAD_CD)
      return Eiconv(iconv_cd_loc2int, str, len);
#endif

   if (len <= 0)
      len = strlen(str);
   return Estrndup(str, len);
}
#endif

/* Convert UTF-8 to internal format (alloc always) */
char               *
EstrUtf82Int(const char *str, int len)
{
   if (!str)
      return NULL;

#if HAVE_ICONV
   if (iconv_cd_utf82int != BAD_CD)
      return Eiconv(iconv_cd_utf82int, str, len);
#endif

   if (len <= 0)
      len = strlen(str);
   return Estrndup(str, len);
}

/* Convert internal to required (alloc only if necessary) */
const char         *
EstrInt2Enc(const char *str, int want_utf8)
{
#if HAVE_ICONV
   if (Mode.locale.utf8_int == want_utf8)
      return str;

   if (!str)
      return NULL;

   if (want_utf8)
      return Eiconv(iconv_cd_int2utf8, str, strlen(str));

   return Eiconv(iconv_cd_int2loc, str, strlen(str));
#else
   want_utf8 = 0;
   return str;
#endif
}

/* Free string returned by EstrInt2Enc() */
void
EstrInt2EncFree(const char *str, int want_utf8)
{
#if HAVE_ICONV
   if (Mode.locale.utf8_int == want_utf8)
      return;

   Efree((char *)str);
#else
   str = NULL;
   want_utf8 = 0;
#endif
}

/*
 * Stuff to do mb/utf8 <-> wc conversions.
 */
#if HAVE_ICONV
static iconv_t      iconv_cd_str2wcs = BAD_CD;
static iconv_t      iconv_cd_wcs2str = BAD_CD;
#endif

int
EwcOpen(int utf8)
{
#if HAVE_ICONV
   const char         *enc;

   if (utf8)
      enc = "UTF-8";
   else
      enc = nl_langinfo(CODESET);

#if SIZEOF_WCHAR_T == 4
   iconv_cd_str2wcs = EiconvOpen("UCS-4", enc);
   iconv_cd_wcs2str = EiconvOpen(enc, "UCS-4");
#else
   iconv_cd_str2wcs = EiconvOpen("WCHAR_T", enc);
   iconv_cd_wcs2str = EiconvOpen(enc, "WCHAR_T");
#endif

   if (iconv_cd_str2wcs != BAD_CD && iconv_cd_wcs2str != BAD_CD)
      return 0;

   EwcClose();
   return -1;
#else
   /* NB! This case will not work properly if needed MB encoding is utf8
    * but locale isn't */
   utf8 = 0;
   return 0;
#endif
}

void
EwcClose(void)
{
#if HAVE_ICONV
   if (iconv_cd_str2wcs != BAD_CD)
      iconv_close(iconv_cd_str2wcs);
   iconv_cd_str2wcs = BAD_CD;
   if (iconv_cd_wcs2str != BAD_CD)
      iconv_close(iconv_cd_wcs2str);
   iconv_cd_wcs2str = BAD_CD;
#endif
}

int
EwcStrToWcs(const char *str, int len, wchar_t * wcs, int wcl)
{
#if HAVE_ICONV
   ICONV_CONST char   *pi;
   char               *po;
   size_t              ni, no, rc;
   char                buf[4096];

   pi = (ICONV_CONST char *)str;
   ni = len;

   if (!wcs)
     {
	no = 4096;
	po = buf;
	rc = iconv(iconv_cd_str2wcs, &pi, &ni, &po, &no);
	if (rc == (size_t) (-1) || no == 0)
	   return -1;
	wcl = (4096 - no) / sizeof(wchar_t);
	return wcl;
     }

   po = (char *)wcs;
   no = wcl * sizeof(wchar_t);
   rc = iconv(iconv_cd_str2wcs, &pi, &ni, &po, &no);
   if (rc == (size_t) (-1))
      return 0;
   return wcl - no / sizeof(wchar_t);
#else
   if (!wcs)
      return mbstowcs(NULL, str, 0);

   mbstowcs(wcs, str, wcl);
   wcs[wcl] = (wchar_t) '\0';

   len = 0;
   return wcl;
#endif
}

int
EwcWcsToStr(const wchar_t * wcs, int wcl, char *str, int len)
{
#if HAVE_ICONV
   ICONV_CONST char   *pi;
   size_t              ni, no, rc;

   pi = (ICONV_CONST char *)wcs;
   ni = wcl * sizeof(wchar_t);
   no = len;
   rc = iconv(iconv_cd_wcs2str, &pi, &ni, &str, &no);
   if (rc == (size_t) (-1))
      return 0;
   return len - no;
#else
   int                 i, j, n;

   j = 0;
   for (i = 0; i < wcl; i++)
     {
	if (j + (int)MB_CUR_MAX > len)
	   break;
	n = wctomb(str + j, wcs[i]);
	if (n > 0)
	   j += n;
     }
   str[j] = '\0';
   return j;
#endif
}

/*
 * Setup
 */

static struct {
   char               *internal;
   char               *exported;
} Conf_locale =
{
NULL, NULL};

static struct {
   char                init;
   char               *env_language;
   char               *env_lc_all;
   char               *env_lc_messages;
   char               *env_lang;
} locale_data;

static void
LangEnvironmentSetup(const char *locale)
{
   /* Precedence:  LANGUAGE, LC_ALL, LC_MESSAGES, LANG */
   if (locale)
     {
	/* Set requested */
	Esetenv("LANGUAGE", locale);
	Esetenv("LC_ALL", locale);
	Esetenv("LANG", locale);
     }
   else
     {
	/* Restore saved */
	Esetenv("LANGUAGE", locale_data.env_language);
	Esetenv("LC_ALL", locale_data.env_lc_all);
	Esetenv("LC_MESSAGES", locale_data.env_lc_messages);
	Esetenv("LANG", locale_data.env_lang);
     }
}

static void
LangEnvironmentSave(void)
{
   if (locale_data.init)
      return;
   locale_data.init = 1;

   locale_data.env_language = Estrdup(getenv("LANGUAGE"));
   locale_data.env_lc_all = Estrdup(getenv("LC_ALL"));
   locale_data.env_lc_messages = Estrdup(getenv("LC_MESSAGES"));
   locale_data.env_lang = Estrdup(getenv("LANG"));
}

void
LangExport(void)
{
   if (Conf_locale.exported)
      LangEnvironmentSetup(Conf_locale.exported);
   else if (Conf_locale.internal)
      LangEnvironmentSetup(NULL);
}

void
LangInit(void)
{
   const char         *enc_loc, *enc_int;

   if (!locale_data.init)
      LangEnvironmentSave();

   LangEnvironmentSetup(Conf_locale.internal);

   setlocale(LC_ALL, "");	/* Set up things according to env vars */

   bindtextdomain(PACKAGE, LOCALEDIR);
   textdomain(PACKAGE);

   if (!XSupportsLocale())
      setlocale(LC_ALL, "C");
   XSetLocaleModifiers("");

   /* I dont want any internationalisation of my numeric input & output */
   setlocale(LC_NUMERIC, "C");

   /* Get the environment character encoding */
#if HAVE_LANGINFO_CODESET
   enc_loc = nl_langinfo(CODESET);
#else
   enc_loc = "ISO-8859-1";
#endif

   /* Debug - possibility to set desired internal representation */
   enc_int = getenv("E_CHARSET");
   if (enc_int)
      bind_textdomain_codeset(PACKAGE, enc_int);
   else
      enc_int = enc_loc;

   Mode.locale.lang = setlocale(LC_MESSAGES, NULL);
   if (EDebug(EDBUG_TYPE_VERBOSE))
     {
	Eprintf("Locale: %s\n", setlocale(LC_ALL, NULL));
	Eprintf("Character encoding: locale=%s internal=%s MB_CUR_MAX=%zu\n",
		enc_loc, enc_int, MB_CUR_MAX);
     }

   if (!Estrcasecmp(enc_loc, "utf8") || !Estrcasecmp(enc_loc, "utf-8"))
      Mode.locale.utf8_loc = 1;
   if (!Estrcasecmp(enc_int, "utf8") || !Estrcasecmp(enc_int, "utf-8"))
      Mode.locale.utf8_int = 1;

#if HAVE_ICONV
   if (Mode.locale.utf8_int && Mode.locale.utf8_loc)
      return;
   if (Mode.locale.utf8_int)
     {
	iconv_cd_loc2int = EiconvOpen("UTF-8", enc_loc);
	iconv_cd_int2loc = EiconvOpen(enc_loc, "UTF-8");
     }
   else
     {
	iconv_cd_utf82int = EiconvOpen(enc_loc, "UTF-8");
	iconv_cd_int2utf8 = EiconvOpen("UTF-8", enc_loc);
     }
#endif
}

void
LangExit(void)
{
#if HAVE_ICONV
   if (iconv_cd_int2utf8 != BAD_CD)
      iconv_close(iconv_cd_int2utf8);
   if (iconv_cd_utf82int != BAD_CD)
      iconv_close(iconv_cd_utf82int);
   if (iconv_cd_int2loc != BAD_CD)
      iconv_close(iconv_cd_int2loc);
   if (iconv_cd_loc2int != BAD_CD)
      iconv_close(iconv_cd_loc2int);
   iconv_cd_int2utf8 = iconv_cd_utf82int = BAD_CD;
   iconv_cd_int2loc = iconv_cd_loc2int = BAD_CD;
#endif

   LangEnvironmentSetup(NULL);
}

static void
LangCfgChange(void *item __UNUSED__, const char *locale)
{
   if (*locale == '\0')
      locale = NULL;
   LangExit();
   _EFDUP(Conf_locale.internal, locale);
   LangInit();
}

static const CfgItem LocaleCfgItems[] = {
   CFG_FUNC_STR(Conf_locale, internal, LangCfgChange),
   CFG_ITEM_STR(Conf_locale, exported),
};
#define N_CFG_ITEMS (sizeof(LocaleCfgItems)/sizeof(CfgItem))

extern const EModule ModLocale;

const EModule       ModLocale = {
   "locale", NULL,
   NULL,
   {0, NULL},
   {N_CFG_ITEMS, LocaleCfgItems}
};
