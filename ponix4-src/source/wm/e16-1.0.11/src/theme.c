/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "emodule.h"
#include "file.h"
#include "util.h"
#include "session.h"

/* Update Mode.theme.paths (theme path list) */
static void
ThemePathsUpdate(void)
{
   char                paths[4096];

   Esnprintf(paths, sizeof(paths), "%s/themes:%s/themes:%s", EDirUser(),
	     EDirRoot(), (Conf.theme.extra_path) ? Conf.theme.extra_path : "");
   _EFDUP(Mode.theme.paths, paths);
}

/* Check if this is a theme dir */
static const char  *
ThemeCheckPath(const char *path)
{
   static const char  *const theme_files[] = {
      "init.cfg",
#if 0
      "epplets/epplets.cfg",
#endif
      NULL
   };
   const char         *tf;
   int                 i;
   char                s[4096];

   for (i = 0; (tf = theme_files[i]); i++)
     {
	Esnprintf(s, sizeof(s), "%s/%s", path, tf);
	if (!isfile(s))
	   return NULL;
     }

   return path;
}

char               *
ThemePathName(const char *path)
{
   const char         *p;
   char               *s;

   if (!path)
      return NULL;
   p = strrchr(path, '/');
   if (!p)
      return Estrdup(path);	/* Name only */
   if (strcmp(p + 1, "e16"))
      return Estrdup(p + 1);	/* Regular path */

   /* <path>/<themename>/e16 */
   s = strdup(path);
   s[p - path] = '\0';
   p = strrchr(s, '/');
   if (!p)
      return Estrdup(path);	/* Should not happen */
   p++;
   memmove(s, p, strlen(p) + 1);
   return s;
}

static void
append_merge_dir(char *dir, char ***list, int *count)
{
   char                ss[FILEPATH_LEN_MAX], s1[FILEPATH_LEN_MAX];
   char              **str = NULL, *s;
   int                 i, num;

   str = E_ls(dir, &num);
   if (!str)
      return;

   for (i = 0; i < num; i++)
     {
	if (!strcmp(str[i], "DEFAULT"))
	   continue;

	Esnprintf(ss, sizeof(ss), "%s/%s", dir, str[i]);

	if (isdir(ss))
	  {
	     if (ThemeCheckPath(ss))
		goto got_one;
	     Esnprintf(ss, sizeof(ss), "%s/%s/e16", dir, str[i]);
	     if (ThemeCheckPath(ss))
		goto got_one;
	     continue;
	  }
	else if (isfile(ss))
	  {
	     s = strstr(str[i], ".etheme");
	     if (!s)
		continue;
	     Esnprintf(s1, sizeof(s1), "%s/themes/%s", EDirUser(), str[i]);
	     s = strstr(s1, ".etheme");
	     if (!s)
		continue;
	     *s = '\0';
	     if (isdir(s1))
		continue;
	  }
	else
	  {
	     continue;
	  }

      got_one:
	(*count)++;
	*list = EREALLOC(char *, *list, *count);

	(*list)[(*count) - 1] = Estrdup(ss);
     }
   StrlistFree(str, num);
}

char              **
ThemesList(int *number)
{
   char              **lst, **list;
   int                 i, num, count;

   ThemePathsUpdate();
   lst = StrlistFromString(Mode.theme.paths, ':', &num);

   count = 0;
   list = NULL;
   for (i = 0; i < num; i++)
      append_merge_dir(lst[i], &list, &count);

   StrlistFree(lst, num);

   *number = count;
   return list;
}

static const char  *
ThemeGetPath(const char *path, char *buf, unsigned int len)
{
   const char         *s;
   char                s1[FILEPATH_LEN_MAX];
   int                 l;

   /* We only attempt to dereference a DEFAULT link */
   s = strstr(path, "/DEFAULT");
   if (!s)
      return path;

   l = readlink(path, s1, sizeof(s1) - 1);
   if (l < 0)
      return path;
   s1[l] = '\0';

   if (isabspath(s1))
     {
	Esnprintf(buf, len, "%s", s1);
	return buf;
     }

   Esnprintf(buf, len, "%s", path);	/* Copy path */
   l = s + 1 - path;
   Esnprintf(buf + l, len - l, "%s", s1);	/* Substitute link */

   return buf;
}

static char        *
ThemeExtract(const char *path)
{
   char                s[FILEPATH_LEN_MAX];
   char                th[FILEPATH_LEN_MAX];
   FILE               *f;
   unsigned char       buf[262];
   size_t              ret;
   char               *name;

   /* its a directory - just use it "as is" */
   if (isdir(path))
     {
	path = ThemeGetPath(path, s, sizeof(s));
	goto done;
     }

   if (!isfile(path))
      return NULL;

   /* its a file - check its type */
   f = fopen(path, "r");
   if (!f)
      return NULL;
   ret = fread(buf, 1, sizeof(buf), f);
   memset(buf + ret, 0, sizeof(buf) - ret);
   fclose(f);

   name = fileof(path);
   Esnprintf(th, sizeof(th), "%s/themes/%s", EDirUser(), name);
   Efree(name);

   /* check magic numbers */
   if ((buf[0] == 31) && (buf[1] == 139))
     {
	/* gzipped tarball */
	Esnprintf(s, sizeof(s),
		  "gzip -d -c < %s | (cd %s ; tar -xf -)", path, th);
     }
   else if ((buf[257] == 'u') && (buf[258] == 's') &&
	    (buf[259] == 't') && (buf[260] == 'a') && (buf[261] == 'r'))
     {
	/* vanilla tarball */
	Esnprintf(s, sizeof(s), "(cd %s ; tar -xf %s)", th, path);
     }
   else
      return NULL;

   E_md(th);
   path = th;

   /* exec the untar if tarred */
   Esystem(s);

 done:
   if (ThemeCheckPath(path))
      return Estrdup(path);

   /* failed */
   return NULL;
}

char               *
ThemeFind(const char *theme)
{
   static const char  *const default_themes[] = {
      "DEFAULT", "winter", "BrushedMetal-Tigert", "ShinyMetal", NULL
   };
   char                tdir[4096], *path;
   char              **lst;
   int                 i, j, num;

   ThemePathsUpdate();

   path = NULL;

   if (!theme || !theme[0])
     {
	theme = NULL;
     }
   else if (!strcmp(theme, "-"))	/* Use fallbacks */
     {
	return NULL;
     }
   else if (isabspath(theme))
     {
	path = ThemeExtract(theme);
	if (path)
	   return path;
	theme = NULL;
     }

   lst = StrlistFromString(Mode.theme.paths, ':', &num);

   i = 0;
   do
     {
	if (!theme)
	   goto next;
	for (j = 0; j < num; j++)
	  {
	     Esnprintf(tdir, sizeof(tdir), "%s/%s", lst[j], theme);
	     path = ThemeExtract(tdir);
	     if (path)
		goto done;

	     Esnprintf(tdir, sizeof(tdir), "%s/%s/e16", lst[j], theme);
	     path = ThemeExtract(tdir);
	     if (path)
		goto done;
	  }
      next:
	theme = default_themes[i++];
     }
   while (theme);

 done:
   StrlistFree(lst, num);

   if (path)
      return path;

   /* No theme found yet, just find any theme */
   lst = ThemesList(&num);
   if (!lst)
      return NULL;
   path = Estrdup(lst[0]);
   StrlistFree(lst, num);

   return path;
}

void
ThemePathFind(void)
{
   char               *name, *path, *s;

   /*
    * Conf.theme.name is read from the configuration.
    * Mode.theme.path may be assigned on the command line.
    */
   name = (Mode.theme.path) ? Mode.theme.path : Conf.theme.name;
   s = (name) ? strchr(name, '=') : NULL;
   if (s)
     {
	*s = 0;
	Efree(Mode.theme.variant);
	Mode.theme.variant = Estrdup(s + 1);
     }
   path = ThemeFind(name);

   if (!path && (!name || strcmp(name, "-")))
     {
	Alert(_("No themes were found in the default directories:\n"
		" %s\n"
		"Proceeding from here is mostly pointless.\n"),
	      Mode.theme.paths);
     }

   Efree(Conf.theme.name);
   Conf.theme.name = ThemePathName(path);

   Efree(Mode.theme.path);
   Mode.theme.path = (path) ? path : Estrdup("-");
}

#if ENABLE_DIALOGS
#include "dialog.h"
#include "settings.h"
/*
 * Configuration dialog
 */
static char         tmp_use_theme_font;
static char         tmp_use_alt_font;

static void
_DlgThemeConfigure(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val >= 2)
      return;
   if (Conf.theme.use_theme_font_cfg == tmp_use_theme_font &&
       Conf.theme.use_alt_font_cfg == tmp_use_alt_font)
      return;

   DialogOK(_("Message"), _("Changes will take effect after restart"));

   Conf.theme.use_theme_font_cfg = tmp_use_theme_font;
   Conf.theme.use_alt_font_cfg = tmp_use_alt_font;
   autosave();
}

static void
_DlgThemeFill(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;
   char                buf[1024];

   tmp_use_theme_font = Conf.theme.use_theme_font_cfg;
   tmp_use_alt_font = Conf.theme.use_alt_font_cfg;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Use theme font configuration"));
   DialogItemCheckButtonSetPtr(di, &tmp_use_theme_font);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   Esnprintf(buf, sizeof(buf), _("Use alternate font configuration (%s)"),
	     Conf.theme.font_cfg ? Conf.theme.font_cfg : _("Not set"));
   DialogItemSetText(di, buf);
   DialogItemCheckButtonSetPtr(di, &tmp_use_alt_font);
}

const DialogDef     DlgTheme = {
   "CONFIGURE_AUDIO",
   N_("Theme"),
   N_("Theme Settings"),
   SOUND_SETTINGS_MISCELLANEOUS,
   "pix/miscellaneous.png",
   N_("Enlightenment Theme\n" "Settings Dialog"),
   _DlgThemeFill,
   DLG_OAC, _DlgThemeConfigure,
};
#endif /* ENABLE_DIALOGS */

/*
 * Theme module
 */

static void
ThemesIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	char               *path;

	IpcPrintf("Name: %s\n", (Conf.theme.name) ? Conf.theme.name : "-");
	IpcPrintf("Full: %s\n", Mode.theme.path);
	path = ThemeFind(NULL);
	IpcPrintf("Default: %s\n", path);
	Efree(path);
	IpcPrintf("Path: %s\n", Mode.theme.paths);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	char              **lst;
	int                 i, num;

	lst = ThemesList(&num);
	if (!lst)
	   return;
	for (i = 0; i < num; i++)
	   IpcPrintf("%s\n", lst[i]);
	StrlistFree(lst, num);
     }
   else if (!strcmp(cmd, "use"))
     {
	/* FIXME - ThemeCheckIfValid(s) */
	SessionExit(EEXIT_THEME, prm);
     }
}

static const IpcItem ThemeIpcArray[] = {
   {
    ThemesIpc,
    "theme", "th",
    "Theme commands",
    "  theme             Show current theme\n"
    "  theme list        Show all themes\n"
    "  theme use <name>  Switch to theme <name>\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(ThemeIpcArray)/sizeof(IpcItem))

static const CfgItem ThemeCfgItems[] = {
   CFG_ITEM_STR(Conf.theme, name),
   CFG_ITEM_STR(Conf.theme, extra_path),
   CFG_ITEM_BOOL(Conf.theme, use_theme_font_cfg, 0),
   CFG_ITEM_BOOL(Conf.theme, use_alt_font_cfg, 0),
   CFG_ITEM_STR(Conf.theme, font_cfg),
};
#define N_CFG_ITEMS (sizeof(ThemeCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModTheme;

const EModule       ModTheme = {
   "theme", "th",
   NULL,
   {N_IPC_FUNCS, ThemeIpcArray},
   {N_CFG_ITEMS, ThemeCfgItems}
};
