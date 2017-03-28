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
#include "E.h"
#include "aclass.h"
#include "backgrounds.h"
#include "borders.h"
#include "buttons.h"
#include "conf.h"
#include "cursors.h"
#include "file.h"
#include "iclass.h"
#include "menus.h"
#include "progress.h"
#include "session.h"
#include "tclass.h"
#include "tooltips.h"
#include "windowmatch.h"

#define MAX_E_CFG_VERSION 2	/* Max. supported configuration version */

static char        *ConfigFileFind(const char *name, const char *themepath,
				   int pp);

void
SkipTillEnd(FILE * fs)
{
   char                s[FILEPATH_LEN_MAX];
   int                 i1, i2;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = i2 = 0;
	sscanf(s, "%i %i", &i1, &i2);
	if (i1 == CONFIG_CLOSE)
	   return;
	if (i2 == CONFIG_OPEN)
	   SkipTillEnd(fs);
     }
}

#define LINE_BUFFER_SIZE 1024
/*
 * This function will get a single line from the file
 * The string will be null terminated.
 * Size must be >= 2.
 */
char               *
GetLine(char *s, int size, FILE * f)
{
   static char        *buffer = NULL;
   static const char  *bufptr = NULL;
   char               *so, ch, quote, escape;
   const char         *si;
   size_t              nr;

   if (!buffer)
     {
	buffer = EMALLOC(char, LINE_BUFFER_SIZE);

	if (!buffer)
	   return NULL;
	buffer[LINE_BUFFER_SIZE - 1] = '\0';
     }

   si = bufptr;
   so = s;
   quote = '\0';
   escape = '\0';
   for (;;)
     {
	/* Get a line from the input file */
	if (!si)
	  {
	     nr = fread(buffer, 1, LINE_BUFFER_SIZE - 1, f);
	     if (nr == 0)
		break;
	     buffer[nr] = '\0';
	     si = buffer;
	  }

	/* Split on ';' or '\n', handle quoting */
	ch = *si++;
	switch (ch)
	  {
	  case '\0':
	     si = NULL;
	     break;
	  case ';':		/* Line separator */
	     if (escape || quote)
		goto case_char;
	     /* FALLTHROUGH */
	  case '\n':
	     if (so == s)	/* Skip empty lines */
		break;
	     *so = '\0';	/* Terminate and return */
	     goto done;
	  case '\r':		/* Ignore */
	     break;
	  case '\\':		/* Escape */
	     if (escape)
		goto case_char;
	     escape = ch;
	     break;
	  case '"':		/* Quoting */
/*	       case '\'': */
	     if (escape)
		goto case_char;
	     if (quote == '\0')
		quote = ch;
	     else if (quote == ch)
		quote = '\0';
	     else
		goto case_char;
	     break;
	  case ' ':		/* Whitespace */
	  case '\t':
	     if (so == s)	/* Skip leading whitespace */
		break;
	   case_char:		/* Normal character */
	  default:
	     *so++ = ch;
	     escape = '\0';
	     if (--size > 1)
		break;
	     *so = '\0';
	     goto done;
	  }
     }

 done:
   bufptr = si;
   if (!si)
     {
	/* EOF */
	Efree(buffer);
	buffer = NULL;
	if (so == s)
	   return NULL;
     }

   /* Strip trailing whitespace */
   si = so;
   for (; so > s; so--)
     {
	ch = so[-1];
	if (ch != ' ' && ch != '\t')
	   break;
     }
   if (so != si)
      *so = '\0';

   if (EDebug(EDBUG_TYPE_CONFIG) > 1)
      Eprintf("GetLine %s\n", s);

   return s;
}

int
ConfigParseline1(char *str, char *s2, char **p2, char **p3)
{
   int                 i1, len1, len2, fields;

   i1 = CONFIG_INVALID;
   len1 = len2 = 0;
   s2[0] = '\0';
   fields = sscanf(str, "%i %n%4000s %n", &i1, &len1, s2, &len2);
   if (p2)
      *p2 = (len1) ? str + len1 : NULL;
   if (p3)
      *p3 = (len2) ? str + len2 : NULL;

   if (fields <= 0)
     {
	i1 = CONFIG_INVALID;
     }
   else if (i1 == CONFIG_CLOSE || i1 == CONFIG_NEXT)
     {
	if (fields != 1)
	  {
	     Alert(_("CONFIG: ignoring extra data in \"%s\""), str);
	  }
     }
   else if (i1 != CONFIG_INVALID)
     {
	if (fields != 2)
	  {
	     i1 = CONFIG_INVALID;
	     Alert(_("CONFIG: missing required data in \"%s\""), str);
	  }
     }

   return i1;
}

void
ConfigParseError(const char *where, const char *line)
{
   Alert(_("Warning: unable to determine what to do with\n"
	   "the following text in the middle of current %s definition:\n"
	   "%s\nWill ignore and continue...\n"), where, line);
}

void
ConfigAlertLoad(const char *txt)
{
   Alert(_("Warning:  Configuration error in %s block.\n"
	   "Outcome is likely not good.\n"), txt);
}

static int
ConfigFilePreparse(const char *src, const char *dst, const char *themepath)
{
   char                execline[FILEPATH_LEN_MAX];
   const char         *variant;

   if (EDebug(EDBUG_TYPE_CONFIG))
      Eprintf("ConfigFilePreparse %s -> %s\n", src, dst);

   /* When themepath is NULL it shouldn't be used, but this is consistent
    * with old behavior */
   if (!themepath)
      themepath = Mode.theme.path;
   variant = (Mode.theme.variant) ? Mode.theme.variant : "";

   Esnprintf(execline, sizeof(execline), "%s/epp " "-P " "-nostdinc " "-undef "
	     "-include %s/config/definitions " "-I%s " "-I%s/config "
	     "-D ENLIGHTENMENT_VERSION=%s " "-D ENLIGHTENMENT_ROOT=%s "
	     "-D ENLIGHTENMENT_BIN=%s "
	     "-D ENLIGHTENMENT_THEME=%s " "-D VARIANT=%s "
	     "-D ECONFDIR=%s " "-D ECACHEDIR=%s "
	     "-D SCREEN_RESOLUTION_%ix%i=1 "
	     "-D SCREEN_WIDTH_%i=1 " "-D SCREEN_HEIGHT_%i=1 "
	     "-D SCREEN_DEPTH_%i=1 "
	     "%s %s",
	     EDirBin(), EDirRoot(), themepath, EDirRoot(),
	     e_wm_version, EDirRoot(), EDirBin(), themepath, variant,
	     EDirUser(), EDirUserCache(),
	     WinGetW(VROOT), WinGetH(VROOT), WinGetW(VROOT), WinGetH(VROOT),
	     WinGetDepth(VROOT), src, dst);
   Esystem(execline);

   return exists(dst) ? 0 : 1;
}

/* Split the process of finding the file from the process of loading it */
int
ConfigFileRead(FILE * fs)
{
   int                 err;
   int                 i1, i2, fields;
   char                s[FILEPATH_LEN_MAX];
   int                 e_cfg_ver = 0;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = i2 = CONFIG_INVALID;
	fields = sscanf(s, "%i %i", &i1, &i2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_VERSION)
	  {
	     if (fields == 2)
		e_cfg_ver = i2;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
	       {
		  Alert(_("CONFIG: ignoring extra data in \"%s\""), s);
	       }
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\""), s);
		  i1 = CONFIG_INVALID;
	       }
	  }

	if (i2 == CONFIG_OPEN)
	  {
	     if (e_cfg_ver > MAX_E_CFG_VERSION)
	       {
		  AlertX(_("Theme versioning ERROR"),
			 _("Restart with Defaults"), " ",
			 _("Abort and Exit"),
			 _("ERROR:\n" "\n"
			   "The configuration for the theme you are running is\n"
			   "incompatible. It's config revision is %i.\n"
			   "It needs to be marked as being revision <= %i\n"
			   "\n"
			   "Please contact the theme author or maintainer and\n"
			   "inform them that in order for their theme to function\n"
			   "with this version of Enlightenment, they have to\n"
			   "update it to the current settings, and then match\n"
			   "the revision number.\n" "\n"
			   "If the theme revision is higher than Enlightenment's\n"
			   "it may be that you haven't upgraded Enlightenment for\n"
			   "a while and this theme takes advantages of new\n"
			   "features in Enlightenment in new versions.\n"),
			 e_cfg_ver, MAX_E_CFG_VERSION);
		  SessionExit(EEXIT_THEME, "DEFAULT");
	       }
	     else
	       {
		  switch (i1)
		    {
		    case CONFIG_CLOSE:
		       goto done;

		    case CONFIG_CURSOR:
		       err = ECursorConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Cursor");
		       break;
		    case CONFIG_IMAGECLASS:
		       err = ImageclassConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Image class");
		       break;
		    case CONFIG_TOOLTIP:
		       err = TooltipConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Tooltip");
		       break;
		    case CONFIG_TEXT:
		       err = TextclassConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Text class");
		       break;
		    case MENU_STYLE:
		       err = MenuStyleConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Menu style");
		       break;
		    case CONFIG_MENU:
		       err = MenuConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Menu");
		       break;
		    case CONFIG_BORDER:
		       err = BorderConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Border");
		       break;
		    case CONFIG_BUTTON:
		       err = ButtonsConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Button");
		       break;
		    case CONFIG_DESKTOP:
		       err = BackgroundsConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Background");
		       break;
		    case CONFIG_WINDOWMATCH:
		       err = WindowMatchConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Window match");
		       break;
		    case CONFIG_COLORMOD:
		       break;
		    case CONFIG_ACTIONCLASS:
		       err = AclassConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Action class");
		       break;
		    case CONFIG_SLIDEOUT:
		       err = SlideoutsConfigLoad(fs);
		       if (err)
			  ConfigAlertLoad("Slideout");
		       break;
		    default:
		       break;
		    }
	       }
	  }
     }

 done:
   return 0;
}

static char        *
FindFilePath(const char *name, const char *path)
{
   char                s[FILEPATH_LEN_MAX];
   int                 len;

#if 0
   Eprintf("%s: %s (%s)\n", __func__, name, path);
#endif
   if (path)
     {
	len = Esnprintf(s, sizeof(s), "%s/%s", path, name);
	name = s;
     }
   else
     {
	len = strlen(name);
     }
   if (len <= 0)
      return NULL;

   if (canread(name))
      return Estrdup(name);
   else
      return NULL;
}

/* *INDENT-OFF* */
static const struct {
   const char         *where, *subdir;
} fprm[] = {
   { "utE",  "config" },
   { "ute",   NULL    },
   { "UuE",  "menus"  },
   { "UuE",  "icons"  }
};
/* *INDENT-ON* */

char               *
FindFile(const char *file, const char *themepath, int type)
{
   const char         *w, *f, *path;
   char                s[FILEPATH_LEN_MAX];
   char               *p;

   /* if absolute path - and file exists - return it */
   if (isabspath(file))
     {
	p = FindFilePath(file, NULL);
	/* Absolute path - no need to look elsewhere */
	goto done;
     }

   p = NULL;
   for (w = fprm[type].where; *w; w++)
     {
	f = file;
	if (*w <= 'Z')
	  {
	     /* Look in subdir */
	     Esnprintf(s, sizeof(s), "%s/%s", fprm[type].subdir, file);
	     f = s;
	  }

	switch (*w & 0xdf)
	  {
	  default:
	     continue;
	  case 'U':		/* User config */
	     path = EDirUser();
	     break;
	  case 'E':		/* e16 config */
	     path = EDirRoot();
	     break;
	  case 'T':		/* Theme */
	     path = themepath;
	     if (!path)
		continue;
	     break;
	  }
	p = FindFilePath(f, path);
	if (p)
	   break;
     }

 done:
#if 0
   Eprintf("%s %d: %s (%s): %s\n", __func__, type, file, themepath, p);
#endif
   return p;
}

char               *
ThemeFileFind(const char *file, int type)
{
   return FindFile(file, Mode.theme.path, type);
}

static char        *
ConfigFileFind(const char *name, const char *themepath, int pp)
{
   char                s[FILEPATH_LEN_MAX];
   char               *fullname, *file, *ppfile;
   int                 i, err;

   fullname = FindFile(name, themepath, FILE_TYPE_CONFIG);
   if (!fullname)
      return NULL;

   /* Quit if not preparsing */
   if (!pp)
      return fullname;

   /* The file exists. Now check the preparsed one. */
   file = Estrdup(fullname);
   for (i = 0; file[i]; i++)
      if (file[i] == '/')
	 file[i] = '.';

   if (Mode.theme.variant)
      file = Estrdupcat2(file, "_", Mode.theme.variant);
   Esnprintf(s, sizeof(s), "%s/cached/cfg/%s.preparsed", EDirUserCache(), file);

   ppfile = Estrdup(s);
   if (exists(s) && moddate(s) > moddate(fullname))
      goto done;

   /* No preparesd file or source is newer. Do preparsing. */
   err = ConfigFilePreparse(fullname, ppfile, themepath);
   if (err)
     {
	Efree(ppfile);
	ppfile = NULL;
     }

 done:
   Efree(fullname);
   Efree(file);
   return ppfile;
}

int
ConfigFileLoad(const char *name, const char *themepath,
	       int (*parse) (FILE * fs), int preparse)
{
   int                 err = -1;
   char               *file;
   FILE               *fs;

   if (EDebug(EDBUG_TYPE_CONFIG))
      Eprintf("ConfigFileLoad %s\n", name);

   file = ConfigFileFind(name, themepath, preparse);
   if (!file)
      goto done;

   fs = fopen(file, "r");
   Efree(file);
   if (!fs)
      goto done;

   err = parse(fs);

   fclose(fs);

 done:
   return err;
}

int
ThemeConfigLoad(void)
{
   static const char  *const config_files[] = {
      "init.cfg",
      "cursors.cfg",
      "textclasses.cfg",
      "imageclasses.cfg",
      "desktops.cfg",
      "actionclasses.cfg",
      "buttons.cfg",
      "slideouts.cfg",
      "borders.cfg",
      "windowmatches.cfg",
      "tooltips.cfg",
      "menustyles.cfg",
   };
   char                s[FILEPATH_LEN_MAX];
   Progressbar        *p = NULL;
   int                 i;

   /* Font mappings */
   FontConfigLoad();

   Esnprintf(s, sizeof(s), "%s/", Mode.theme.path);

   for (i = 0; i < (int)(sizeof(config_files) / sizeof(char *)); i++)

     {
	if (!Mode.wm.restart && Conf.startup.animate)
	  {
	     if (i == 2)
		StartupWindowsCreate();

	     if ((i > 1) && (!p))
	       {
		  p = ProgressbarCreate(_("Enlightenment Starting..."), 400,
					16);
		  if (p)
		     ProgressbarShow(p);
	       }
	  }

	ConfigFileLoad(config_files[i], Mode.theme.path, ConfigFileRead, 1);

	if (p)
	   ProgressbarSet(p, (i * 100) /
			  (int)(sizeof(config_files) / sizeof(char *)));

	/* Hack - We are not running in the event loop here */
	EobjsRepaint();
     }

   if (p)
      ProgressbarDestroy(p);

   /* Font mappings no longer needed */
   FontConfigUnload();

   return 0;
}
