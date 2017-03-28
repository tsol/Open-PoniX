/*
 * Copyright (C) 2004-2010 Kim Woelders
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
#include "e16-ecore_list.h"
#include "parse.h"

typedef struct {
   char               *name;
   char               *font;
} FontAlias;

static Ecore_List  *font_list = NULL;

static void
_FontAliasDestroy(void *data)
{
   FontAlias          *fa = (FontAlias *) data;

   if (!fa)
      return;
   Efree(fa->name);
   Efree(fa->font);

   Efree(fa);
}

static FontAlias   *
FontAliasCreate(const char *name, const char *font)
{
   FontAlias          *fa;

   fa = EMALLOC(FontAlias, 1);
   if (!fa)
      return NULL;

   fa->name = Estrdup(name);
   fa->font = Estrdup(font);

   if (!font_list)
     {
	font_list = ecore_list_new();
	ecore_list_free_cb_set(font_list, _FontAliasDestroy);
     }
   ecore_list_prepend(font_list, fa);

   return fa;
}

static int
_FontMatchName(const void *data, const void *match)
{
   return strcmp(((const FontAlias *)data)->name, (const char *)match);
}

const char         *
FontLookup(const char *name)
{
   FontAlias          *fa;

   fa = (FontAlias *) ecore_list_find(font_list, _FontMatchName, name);

   return (fa) ? fa->font : NULL;
}

/*
 * Configuration load
 */

static int
_FontConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX], *ss, *name, *font;
   int                 len;

   for (;;)
     {
	ss = fgets(s, sizeof(s), fs);
	if (!ss)
	   break;

	len = strcspn(s, "#\r\n");
	if (len <= 0)
	   continue;

	name = font = NULL;
	parse(s, "%S%S", &name, &font);
	if (!name || !font)
	   continue;

	if (strncmp(name, "font-", 5))
	   continue;
	FontAliasCreate(name, font);
     }

   return err;
}

static int
_FontConfigLoad1(const char *cfg, int look_in_theme_too)
{
   const char         *path;

   path = (look_in_theme_too) ? Mode.theme.path : NULL;

   return ConfigFileLoad(cfg, path, _FontConfigLoad, 0);
}

void
FontConfigLoad(void)
{
   /* First check explicitly specified configuration (not in theme dir) */
   if (Conf.theme.use_alt_font_cfg && Conf.theme.font_cfg)
     {
	if (!_FontConfigLoad1(Conf.theme.font_cfg, 0))
	   return;
     }

   /* If using theme font is specified look for that */
   if (Conf.theme.use_theme_font_cfg)
     {
	if (!_FontConfigLoad1("fonts.theme.cfg", 1))
	   return;
     }

   /* Look in user config dir (not in theme dir) */
   if (!_FontConfigLoad1("fonts.cfg", 0))
      return;

#if USE_PANGO
   if (!_FontConfigLoad1("fonts.pango.cfg", 1))
      return;
#endif
#if USE_XFT
   if (!_FontConfigLoad1("fonts.xft.cfg", 1))
      return;
#endif
   _FontConfigLoad1("fonts.cfg", 1);
}

void
FontConfigUnload(void)
{
   ecore_list_destroy(font_list);
   font_list = NULL;
}
