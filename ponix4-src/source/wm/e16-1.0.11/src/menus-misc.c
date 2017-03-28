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
#include "backgrounds.h"
#include "borders.h"
#include "desktops.h"
#include "ewins.h"
#include "file.h"
#include "groups.h"
#include "iclass.h"
#include "menus.h"
#include "parse.h"
#include "progress.h"
#include <errno.h>
#include <sys/stat.h>

static char         menu_scan_recursive = 0;

static Menu        *MenuCreateFromDirectory(const char *name, Menu * parent,
					    MenuStyle * ms, const char *dir);
static int          _ext_is_imagetype(const char *ext);

static MenuItem    *
MenuItemCreateFromBackground(const char *bgid, const char *file)
{
   MenuItem           *mi;
   Background         *bg;
   ImageClass         *ic;
   char                thumb[1024], buf[1024];

   bg = BrackgroundCreateFromImage(bgid, file, thumb, sizeof(thumb));
   if (!bg)
      return NULL;

   ic = ImageclassCreateSimple("`", thumb);

   Esnprintf(buf, sizeof(buf), "bg use %s", bgid);

   mi = MenuItemCreate(NULL, ic, buf, NULL);

   return mi;
}

static const char  *
_dircache_filename(char *buf, unsigned int len, struct stat *st)
{
   static const char   chmap[] =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   int                 aa, bb, cc;

   aa = (int)st->st_ino;
   bb = filedev_map((int)st->st_dev);
   cc = (st->st_mtime > st->st_ctime) ? st->st_mtime : st->st_ctime;
   Esnprintf(buf, len, ".%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
	     chmap[(aa >> 0) & 0x3f], chmap[(aa >> 6) & 0x3f],
	     chmap[(aa >> 12) & 0x3f], chmap[(aa >> 18) & 0x3f],
	     chmap[(aa >> 24) & 0x3f], chmap[(aa >> 28) & 0x3f],
	     chmap[(bb >> 0) & 0x3f], chmap[(bb >> 6) & 0x3f],
	     chmap[(bb >> 12) & 0x3f], chmap[(bb >> 18) & 0x3f],
	     chmap[(bb >> 24) & 0x3f], chmap[(bb >> 28) & 0x3f],
	     chmap[(cc >> 0) & 0x3f], chmap[(cc >> 6) & 0x3f],
	     chmap[(cc >> 12) & 0x3f], chmap[(cc >> 18) & 0x3f],
	     chmap[(cc >> 24) & 0x3f], chmap[(cc >> 28) & 0x3f]);

   return buf;
}

static int
MenuLoadFromDirectory(Menu * m)
{
   Progressbar        *p = NULL;
   Menu               *mm;
   int                 i, num, len;
   const char         *dir;
   char              **list, s[4096], ss[4096], cs[4096];
   const char         *ext;
   MenuItem           *mi;
   struct stat         st;
   FILE               *f;
   time_t              lastmod;

   dir = MenuGetData(m);
   lastmod = moddate(dir);
   if (!menu_scan_recursive && lastmod <= MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, lastmod);

   MenuEmpty(m, 0);

   if (stat(dir, &st) < 0)
      return 1;

   Esnprintf(cs, sizeof(cs), "%s/cached/img/%s.dir",
	     EDirUserCache(), _dircache_filename(ss, sizeof(ss), &st));

   if (Mode.backgrounds.force_scan)
      goto skip_dir_cache;

   if (exists(cs))
     {
	/* cached dir listing - use it */

	f = fopen(cs, "r");
	if (!f)
	   return 1;
	while (fgets(s, sizeof(s), f))
	  {
	     char                s2[4096];

	     s[strlen(s) - 1] = 0;
	     len = 0;
	     sscanf(s, "%1000s %1000s %n", ss, s2, &len);
	     if (!strcmp(ss, "BG"))
	       {
		  Esnprintf(ss, sizeof(ss), "%s/%s", dir, s + len);
		  mi = MenuItemCreateFromBackground(s2, ss);
		  MenuAddItem(m, mi);
	       }
	     else if (!strcmp(ss, "EXE"))
	       {
		  Esnprintf(ss, sizeof(ss), "exec %s/%s", dir, s2);
		  mi = MenuItemCreate(NULL, NULL, ss, NULL);
		  MenuAddItem(m, mi);
	       }
	     else if (!strcmp(ss, "DIR"))
	       {
		  Esnprintf(ss, sizeof(ss), "%s/%s", dir, s2);
		  mm = MenuCreateFromDirectory(ss, m, NULL, ss);
		  mi = MenuItemCreate(s2, NULL, NULL, mm);
		  MenuAddItem(m, mi);
	       }
	  }
	fclose(f);
	return 1;
     }

 skip_dir_cache:
   Esnprintf(s, sizeof(s), "Scanning %s", dir);

   p = ProgressbarCreate(s, 600, 16);
   if (p)
      ProgressbarShow(p);

   f = fopen(cs, "w");

   list = E_ls(dir, &num);
   for (i = 0; i < num; i++)
     {
	if (p)
	   ProgressbarSet(p, (i * 100) / num);
	Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
	/* skip "dot" files and dirs - senisble */
	if ((*(list[i]) == '.') || (stat(ss, &st) < 0))
	   continue;

	ext = fileext(ss);
	if (S_ISDIR(st.st_mode))
	  {
	     /* Submenu */
	     mm = MenuCreateFromDirectory(ss, m, NULL, ss);
	     mi = MenuItemCreate(list[i], NULL, NULL, mm);
	     MenuAddItem(m, mi);
	     if (f)
		fprintf(f, "DIR %s\n", list[i]);
	  }
#if 0
	else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
	  {
	     /* Executable */
	     /* that's it - people are stupid and have executable images and just */
	     /* don't get it - so I'm disablign this to save people from their own */
	     /* stupidity */
	     mi = MenuItemCreate(list[i], NULL, ss, NULL);
	     MenuAddItem(m, mi);
	     if (f)
		fprintf(f, "EXE %s\n", list[i]);
	  }
#endif
	else if (_ext_is_imagetype(ext))
	  {
	     /* Background */
	     char                s3[512];

	     _dircache_filename(s3, sizeof(s3), &st);

	     mi = MenuItemCreateFromBackground(s3, ss);
	     if (mi)
	       {
		  MenuAddItem(m, mi);

		  if (f)
		     fprintf(f, "BG %s %s\n", s3, list[i]);
	       }
	  }
     }
   if (f)
      fclose(f);
   if (p)
      ProgressbarDestroy(p);
   if (list)
      StrlistFree(list, num);

   return 1;
}

static Menu        *
MenuCreateFromDirectory(const char *name, Menu * parent, MenuStyle * ms,
			const char *dir)
{
   static int          calls = 0;
   Menu               *m;

   if (calls > 32)
      return NULL;
   calls++;

   m = MenuCreate(name, NULL, parent, ms);
   MenuSetData(m, Estrdup(dir));
   MenuSetLoader(m, MenuLoadFromDirectory);

   if (menu_scan_recursive)
      MenuLoadFromDirectory(m);

   calls--;

   return m;
}

void
ScanBackgroundMenu(void)
{
   menu_scan_recursive = 1;
   MenuLoad(MenuFind("backgrounds", NULL));
   Mode.backgrounds.force_scan = 0;
   menu_scan_recursive = 0;
}

static void
FillFlatFileMenu(Menu * m, const char *file)
{
   FILE               *f;
   char                first = 1;
   char                s[4096];
   unsigned int        len;

   f = fopen(file, "r");
   if (!f)
     {
	Eprintf("Unable to open menu file %s -- %s\n", file, strerror(errno));
	return;
     }

   MenuSetIconSize(m, 0);	/* Scale to default */

   while (fgets(s, 4096, f))
     {
	if (s[0] == '#')
	   continue;
	len = strlen(s);
	while (len && (s[len - 1] == '\n' || s[len - 1] == '\r'))
	   len--;
	if (len == 0)
	   continue;
	s[len] = '\0';

	if (first)
	  {
	     char               *title, *style, *alias;

	     first = 0;

	     title = style = alias = NULL;
	     parse(s, "%S%S%S", &title, &style, &alias);

	     if (title)
		MenuSetTitle(m, title);
	     if (!style)
		style = (char *)"ROOT";
	     MenuSetStyle(m, MenuStyleFind(style));
	     if (alias)
		MenuSetAlias(m, alias);
	  }
	else
	  {
	     char               *txt, *icon, *act, *params;
	     char                wd[4096];
	     MenuItem           *mi;
	     ImageClass         *icc = NULL;
	     Menu               *mm;

	     txt = icon = act = params = NULL;
	     wd[0] = '\0';
	     parse(s, "%S%T%S%S", &txt, &icon, &act, &params);

	     if (icon)
		icon = FindFile(icon, NULL, FILE_TYPE_ICON);
	     if (icon)
	       {
		  Esnprintf(wd, sizeof(wd), "__FM.%s", icon);
		  icc = ImageclassFind(wd, 0);
		  if (!icc)
		     icc = ImageclassCreateSimple(wd, icon);
		  Efree(icon);
	       }
	     if ((act) && (!strcmp(act, "exec")) && (params))
	       {
		  sscanf(params, "%4000s", wd);
		  if (path_canexec(wd))
		    {
		       Esnprintf(wd, sizeof(wd), "exec %s", params);
		       mi = MenuItemCreate(txt, icc, wd, NULL);
		       MenuAddItem(m, mi);
		    }
	       }
	     else if ((act) && (!strcmp(act, "menu")) && (params))
	       {
		  mm = MenuFind(params, NULL);
		  if (mm)
		    {
		       mi = MenuItemCreate(txt, icc, NULL, mm);
		       MenuAddItem(m, mi);
		    }
	       }
	     else if (act)
	       {
		  mi = MenuItemCreate(txt, icc, act, NULL);
		  MenuAddItem(m, mi);
	       }
	  }
     }
   fclose(f);
}

static int
MenuLoadFromFlatFile(Menu * m)
{
   const char         *ff;
   time_t              lastmod;

   ff = MenuGetData(m);
   lastmod = moddate(ff);
   if (lastmod <= MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, lastmod);

   MenuEmpty(m, 0);
   FillFlatFileMenu(m, ff);

   return 1;
}

static Menu        *
MenuCreateFromFlatFile(const char *name, Menu * parent, MenuStyle * ms,
		       const char *file)
{
   Menu               *m = NULL;
   char               *ff;
   static int          calls = 0;

   if (calls > 32)
      return NULL;
   calls++;

   if (!file)
      file = name;

   ff = FindFile(file, NULL, FILE_TYPE_MENU);
   if (!ff)
      goto done;

   m = MenuCreate(file, NULL, parent, ms);
   if (name != file)
      MenuSetAlias(m, name);
   MenuSetData(m, ff);
   MenuSetLoader(m, MenuLoadFromFlatFile);

 done:
   calls--;

   return m;
}

static Menu        *
MenuCreateFromBackgrounds(const char *name, MenuStyle * ms)
{
   Menu               *m;
   char                s[FILEPATH_LEN_MAX];

   Esnprintf(s, sizeof(s), "%s/backgrounds", EDirUser());

   m = MenuCreateFromDirectory(name, NULL, ms, s);
   if (!m)
      return NULL;

   MenuSetTitle(m, _("Backgrounds"));
   MenuSetInternal(m);

   return m;
}

static int
MenuLoadFromThemes(Menu * m)
{
   char              **lst;
   int                 i, num;
   char                ss[4096], *s;
   MenuItem           *mi;

   if (MenuGetTimestamp(m))
      return 0;
   MenuSetTimestamp(m, 1);

   lst = ThemesList(&num);
   for (i = 0; i < num; i++)
     {
	s = ThemePathName(lst[i]);
	Esnprintf(ss, sizeof(ss), "theme use %s", s);
	mi = MenuItemCreate(s, NULL, ss, NULL);
	MenuAddItem(m, mi);
	Efree(s);
     }
   if (lst)
      StrlistFree(lst, i);

   return 1;
}

static Menu        *
MenuCreateFromThemes(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Themes"));
   MenuSetInternal(m);
   MenuSetLoader(m, MenuLoadFromThemes);

   return m;
}

static int
BorderNameCompare(const void *b1, const void *b2)
{
   if (b1 && b2)
      return strcmp(BorderGetName((const Border *)b1),
		    BorderGetName((const Border *)b2));

   return 0;
}

static Menu        *
MenuCreateFromBorders(const char *name, MenuStyle * ms)
{
   char                s[128];
   Menu               *m;
   Border            **lst;
   int                 i, num;
   MenuItem           *mi;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Border"));

   lst = BordersGetList(&num);
   if (!lst)
      return m;

   Quicksort((void **)lst, 0, num - 1, BorderNameCompare);
   for (i = 0; i < num; i++)
     {
	/* if its not internal (ie doesnt start with _ ) */
	if (lst[i]->name[0] != '_')
	  {
	     Esnprintf(s, sizeof(s), "wop * bo %s", lst[i]->name);
	     mi = MenuItemCreate(lst[i]->name, NULL, s, NULL);
	     MenuAddItem(m, mi);
	  }
     }
   Efree(lst);

   return m;
}

static int
MenuCheckShowEwinDesk(EWin * ewin, void *prm)
{
   if (!EwinGetTitle(ewin) || ewin->props.skip_winlist)
      return 0;
   return !prm || EoGetDesk(ewin) == prm;
}

static void
MenuLoadFromEwins(Menu * m, int (*f) (EWin * ewin, void *prm), void *prm)
{
   EWin               *const *lst;
   int                 i, num;
   char                s[256];
   MenuItem           *mi;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (!f(lst[i], prm))
	   continue;

	Esnprintf(s, sizeof(s), "wop %#lx focus", EwinGetClientXwin(lst[i]));
	mi = MenuItemCreate(EwinGetTitle(lst[i]), NULL, s, NULL);
	MenuAddItem(m, mi);
     }
}

static int
MenuLoadFromAllEwins(Menu * m)
{
   MenuEmpty(m, 0);
   MenuLoadFromEwins(m, MenuCheckShowEwinDesk, NULL);
   return 1;
}

static Menu        *
MenuCreateFromAllEWins(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Window List"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromAllEwins);

   return m;
}

static int
MenuLoadFromDesktops(Menu * m)
{
   Menu               *mm;
   unsigned int        i;
   char                s[256];
   MenuItem           *mi;

   MenuEmpty(m, 0);

   for (i = 0; i < DesksGetNumber(); i++)
     {
	mm = MenuCreate("__SUBMENUDESK_E", NULL, m, NULL);
	Esnprintf(s, sizeof(s), "desk goto %i", i);
	mi = MenuItemCreate(_("Go to this Desktop"), NULL, s, NULL);
	MenuAddItem(mm, mi);
	MenuLoadFromEwins(mm, MenuCheckShowEwinDesk, DeskGet(i));

	Esnprintf(s, sizeof(s), _("Desktop %i"), i);
	mi = MenuItemCreate(s, NULL, NULL, mm);
	MenuAddItem(m, mi);
     }

   return 1;
}

static Menu        *
MenuCreateFromDesktops(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Desks"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromDesktops);

   return m;
}

static int
MenuLoadFromGroups(Menu * m)
{
   Menu               *mm;
   Group             **lst;
   int                 i, j, num;
   char                s[256];
   MenuItem           *mi;

   MenuEmpty(m, 0);

   lst = GroupsGetList(&num);
   if (!lst)
      return 1;

   for (i = 0; i < num; i++)
     {
	mm = MenuCreate("__SUBMENUGROUP_E", NULL, m, NULL);

	Esnprintf(s, sizeof(s), "gop %li showhide",
		  EwinGetClientXwin(lst[i]->members[0]));
	mi = MenuItemCreate(_("Show/Hide this group"), NULL, s, NULL);

	Esnprintf(s, sizeof(s), "wop %#lx ic",
		  EwinGetClientXwin(lst[i]->members[0]));
	MenuAddItem(mm, mi);
	mi = MenuItemCreate(_("Iconify this group"), NULL, s, NULL);
	MenuAddItem(mm, mi);

	for (j = 0; j < lst[i]->num_members; j++)
	  {
	     Esnprintf(s, sizeof(s), "wop %#lx focus",
		       EwinGetClientXwin(lst[i]->members[j]));
	     mi = MenuItemCreate(EwinGetTitle(lst[i]->members[j]), NULL,
				 s, NULL);
	     MenuAddItem(mm, mi);
	  }
	Esnprintf(s, sizeof(s), _("Group %i"), i);
	mi = MenuItemCreate(s, NULL, NULL, mm);
	MenuAddItem(m, mi);
     }
   Efree(lst);

   return 1;
}

static Menu        *
MenuCreateFromGroups(const char *name, MenuStyle * ms)
{
   Menu               *m;

   m = MenuCreate(name, NULL, NULL, ms);
   MenuSetTitle(m, _("Groups"));
   MenuSetInternal(m);
   MenuSetDynamic(m);
   MenuSetLoader(m, MenuLoadFromGroups);

   return m;
}

#if 0				/* Not finished */
Menu               *
MenuCreateMoveToDesktop(char *name, Menu * parent, MenuStyle * ms)
{
   Menu               *m;
   int                 i;
   char                s1[256], s2[256];

   MenuItem           *mi;

   m = MenuCreate(name, NULL, parent, ms);

   for (i = 0; i < Mode.numdesktops; i++)
     {
	Esnprintf(s1, sizeof(s1), _("Desktop %i"), i);
	Esnprintf(s2, sizeof(s2), "%i", i);
	mi = MenuItemCreate(s1, NULL, s2, NULL);
	MenuAddItem(m, mi);
     }

   return m;
}
#endif

Menu               *
MenusCreateInternal(const char *type, const char *name, const char *style,
		    const char *prm)
{
   Menu               *m;
   MenuStyle          *ms;

   m = NULL;
   ms = NULL;
   if (style)
      ms = MenuStyleFind(style);

   if (!type)
     {
	if (!strstr(name, ".menu"))
	   type = name;
     }

   if (!type || !strcmp(type, "file"))
     {
	m = MenuCreateFromFlatFile(name, NULL, ms, prm);
     }
   else if (!strcmp(type, "dirscan"))
     {
	SoundPlay(SOUND_SCANNING);
	m = MenuCreateFromDirectory(name, NULL, ms, prm);
     }
   else if (!strcmp(type, "backgrounds"))
     {
	m = MenuCreateFromBackgrounds(name, ms);
     }
   else if (!strcmp(type, "borders"))
     {
	m = MenuCreateFromBorders(name, ms);
     }
   else if (!strcmp(type, "themes"))
     {
	m = MenuCreateFromThemes(name, ms);
     }
   else if (!strcmp(type, "windowlist"))
     {
	m = MenuCreateFromAllEWins(name, ms);
     }
   else if (!strcmp(type, "deskmenu"))
     {
	m = MenuCreateFromDesktops(name, ms);
     }
   else if (!strcmp(type, "groupmenu"))
     {
	m = MenuCreateFromGroups(name, ms);
     }

   return m;
}

static int
_ext_is_imagetype(const char *ext)
{
   static const char  *const exts[] = {
      "jpg", "jpeg", "gif", "png", "tif", "tiff",
      "xpm", "ppm", "pgm", "pnm", "bmp", NULL
   };
   int                 i;

   for (i = 0; exts[i]; i++)
      if (!Estrcasecmp(exts[i], ext))
	 return 1;

   return 0;
}
