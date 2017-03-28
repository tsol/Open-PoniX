/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2005-2009 Kim Woelders
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
#include "borders.h"
#include "conf.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "ewins.h"
#include "ewin-ops.h"
#include "windowmatch.h"

typedef struct _windowmatch WindowMatch;

struct _windowmatch {
   char               *name;
   /* Match criteria */
   char                match;
   char                op;
   char                prop;
   char                qual;
   char               *value;
   int                 width_min, width_max;
   int                 height_min, height_max;
   /* Match actions */
   char               *args;
   Border             *border;
};

#define MATCH_TYPE_TITLE        1
#define MATCH_TYPE_WM_NAME      2
#define MATCH_TYPE_WM_CLASS     3
#define MATCH_TYPE_SIZE         4
#define MATCH_TYPE_SIZE_H       5
#define MATCH_TYPE_SIZE_V       6
#define MATCH_TYPE_PROP         7

#define MATCH_PROP_TRANSIENT    1
#define MATCH_PROP_SHAPED       2
#define MATCH_PROP_FIXEDSIZE    3
#define MATCH_PROP_FIXEDSIZE_H  4
#define MATCH_PROP_FIXEDSIZE_V  5

#define MATCH_OP_BORDER         1
#define MATCH_OP_ICON           2
#define MATCH_OP_WINOP          3

static int          WindowMatchEobjOpsParse(EObj * eo, const char *ops);

static Ecore_List  *wm_list = NULL;

static const char  *const MatchType[] = {
   NULL, "Title", "Name", "Class", "Size", "Width", "Height", "Prop", NULL
};

static const char  *const MatchProp[] = {
   NULL, "Transient", "Shaped", "FixedSize", "FixedWidth", "FixedHeight", NULL
};

static const char  *const MatchOp[] = {
   NULL, "Border", "Icon", "Winop", NULL
};

static int
MatchFind(const char *const *list, const char *str)
{
   int                 i;

   for (i = 1; list[i]; i++)
      if (!strcmp(str, list[i]))
	 return i;

   return 0;
}

static WindowMatch *
WindowMatchCreate(const char *name)
{
   WindowMatch        *b;

   b = ECALLOC(WindowMatch, 1);
   if (!b)
      return NULL;

   if (!wm_list)
      wm_list = ecore_list_new();
   ecore_list_prepend(wm_list, b);

   b->name = Estrdup(name);
   b->width_max = 99999;
   b->height_max = 99999;

   return b;
}

static void
WindowMatchDestroy(WindowMatch * wm)
{
   if (!wm)
      return;

   ecore_list_node_remove(wm_list, wm);

   Efree(wm->name);
   Efree(wm->value);
   Efree(wm->args);

   Efree(wm);
}

int
WindowMatchConfigLoad(FILE * fs)
{
   int                 err = 0;
   WindowMatch        *wm = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_VERSION:
	  case CONFIG_WINDOWMATCH:
	     err = -1;
	     break;

	  case CONFIG_CLASSNAME:
	     wm = WindowMatchCreate(s2);
	     break;

	  case CONFIG_CLOSE:
	     if (wm)
	       {
		  if (!wm->match || !wm->op)
		     WindowMatchDestroy(wm);
		  wm = NULL;
		  err = 0;
	       }
	     goto done;

	  case WINDOWMATCH_MATCHTITLE:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_TITLE;
	     wm->value = Estrdup(s2);
	     break;
	  case WINDOWMATCH_MATCHNAME:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_WM_NAME;
	     wm->value = Estrdup(s2);
	     break;
	  case WINDOWMATCH_MATCHCLASS:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_WM_CLASS;
	     wm->value = Estrdup(s2);
	     break;

	  case WINDOWMATCH_WIDTH:
	     if (!wm)
		break;
	     if (wm->match == MATCH_TYPE_SIZE_V)
		wm->match = MATCH_TYPE_SIZE;
	     else
		wm->match = MATCH_TYPE_SIZE_H;
	     sscanf(s, "%*s %u %u", &(wm->width_min), &(wm->width_max));
	     break;
	  case WINDOWMATCH_HEIGHT:
	     if (!wm)
		break;
	     if (wm->match == MATCH_TYPE_SIZE_H)
		wm->match = MATCH_TYPE_SIZE;
	     else
		wm->match = MATCH_TYPE_SIZE_V;
	     sscanf(s, "%*s %u %u", &(wm->height_min), &(wm->height_max));
	     break;

	  case WINDOWMATCH_TRANSIENT:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     wm->prop = MATCH_PROP_TRANSIENT;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_SHAPED:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     wm->prop = MATCH_PROP_SHAPED;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_H:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     if (wm->prop == MATCH_PROP_FIXEDSIZE_V)
		wm->prop = MATCH_PROP_FIXEDSIZE;
	     else
		wm->prop = MATCH_PROP_FIXEDSIZE_H;
	     wm->qual = !atoi(s2);
	     break;
	  case WINDOWMATCH_NO_RESIZE_V:
	     if (!wm)
		break;
	     wm->match = MATCH_TYPE_PROP;
	     if (wm->prop == MATCH_PROP_FIXEDSIZE_H)
		wm->prop = MATCH_PROP_FIXEDSIZE;
	     else
		wm->prop = MATCH_PROP_FIXEDSIZE_V;
	     wm->qual = !atoi(s2);
	     break;

	  case CONFIG_BORDER:
	  case WINDOWMATCH_USEBORDER:
	     if (!wm)
		break;
	     wm->border = BorderFind(s2);
	     if (!wm->border)
		break;
	     wm->op = MATCH_OP_BORDER;
	     break;

	  case WINDOWMATCH_ICON:
	  case CONFIG_ICONBOX:
#if 0				/* This has not been active since at least 0.16.5 */
	     if (!wm)
		break;
	     wm->icon = ImageclassFind(s2, 0);
	     if (!wm->icon)
		break;
	     wm->op = MATCH_OP_ICON;
	     wm->icon->ref_count++;
#endif
	     break;

	  case WINDOWMATCH_DESKTOP:
	  case CONFIG_DESKTOP:
#if 0				/* This has not been active since at least 0.16.5 */
	     wm->desk = atoi(s2);
#endif
	     break;

	  case WINDOWMATCH_MAKESTICKY:
	     if (!wm)
		break;
	     wm->args = Estrdupcat2(wm->args, ":", "stick");
	     break;

	  default:
	     ConfigParseError("WindowMatch", s);
	     break;
	  }
     }

 done:
   return err;
}

static WindowMatch *
WindowMatchDecode(const char *line)
{
   char                match[32], value[1024], op[32];
   const char         *args;
   WindowMatch        *wm = NULL;
   int                 err, num, w1, w2, h1, h2;

   match[0] = value[0] = op[0] = '\0';
   num = sscanf(line, "%32s %1024s %32s %n", match, value, op, &w1);
   if (num < 3)
      return NULL;
   args = line + w1;
   if (*args == '\0')
      return NULL;

   err = 0;

   num = MatchFind(MatchType, match);
   if (num <= 0)
     {
	Eprintf("WindowMatchDecode: Error (%s): %s\n", match, line);
	err = 1;
	goto done;
     }

   wm = WindowMatchCreate(NULL);
   if (!wm)
      return NULL;

   wm->match = num;

   switch (wm->match)
     {
     case MATCH_TYPE_TITLE:
     case MATCH_TYPE_WM_NAME:
     case MATCH_TYPE_WM_CLASS:
	wm->value = Estrdup(value);
	break;

     case MATCH_TYPE_SIZE:
	num = sscanf(value, "%u-%ux%u-%u", &w1, &w2, &h1, &h2);
	if (num < 4)
	   goto case_error;
	wm->width_min = w1;
	wm->width_max = w2;
	wm->height_min = h1;
	wm->height_max = h2;
	break;
     case MATCH_TYPE_SIZE_H:
	num = sscanf(value, "%u-%u", &w1, &w2);
	if (num < 2)
	   goto case_error;
	wm->width_min = w1;
	wm->width_max = w2;
	break;
     case MATCH_TYPE_SIZE_V:
	num = sscanf(value, "%u-%u", &h1, &h2);
	if (num < 2)
	   goto case_error;
	wm->height_min = h1;
	wm->height_max = h2;
	break;

     case MATCH_TYPE_PROP:
	num = 0;
	if (*value == '!')
	  {
	     wm->qual = 1;
	     num = 1;
	  }
	wm->prop = MatchFind(MatchProp, value + num);
	if (wm->prop <= 0)
	   goto case_error;
	break;

      case_error:
	Eprintf("WindowMatchDecode: Error (%s): %s\n", value, line);
	err = 1;
	goto done;
     }

   wm->op = MatchFind(MatchOp, op);
   if (wm->op <= 0)
     {
	Eprintf("WindowMatchDecode: Error (%s): %s\n", op, line);
	err = 1;
	goto done;
     }

   switch (wm->op)
     {
     case MATCH_OP_BORDER:
	wm->border = BorderFind(args);
	if (!wm->border)
	  {
	     err = 1;
	     goto done;
	  }
	break;

     case MATCH_OP_ICON:
	/* FIXME - Check if exists */
	wm->args = Estrdup(args);
	break;

     case MATCH_OP_WINOP:
	if (WindowMatchEobjOpsParse(NULL, args))
	  {
	     Eprintf("WindowMatchDecode: Error (%s): %s\n", args, line);
	     err = 1;
	     goto done;
	  }
	wm->args = Estrdup(args);
	break;
     }

 done:
   if (err)
     {
	if (wm)
	   WindowMatchDestroy(wm);
     }
   else
     {
	ecore_list_append(wm_list, ecore_list_node_remove(wm_list, wm));
     }
   return wm;
}

static char        *
WindowMatchEncode(WindowMatch * wm, char *buf, int len)
{
   char                s[1024];
   const char         *qual, *value, *args;

   qual = " ";

   switch (wm->match)
     {
     default:
	value = wm->value;
	break;

     case MATCH_TYPE_SIZE:
	value = s;
	sprintf(s, "%u-%ux%u-%u", wm->width_min, wm->width_max,
		wm->height_min, wm->height_max);
	break;
     case MATCH_TYPE_SIZE_H:
	value = s;
	sprintf(s, "%u-%u", wm->width_min, wm->width_max);
	break;
     case MATCH_TYPE_SIZE_V:
	value = s;
	sprintf(s, "%u-%u", wm->height_min, wm->height_max);
	break;

     case MATCH_TYPE_PROP:
	qual = (wm->qual) ? "!" : " ";
	value = MatchProp[(int)wm->prop];
	break;
     }

   switch (wm->op)
     {
     default:
	args = wm->args;
	break;

     case MATCH_OP_BORDER:
	args = BorderGetName(wm->border);
	break;
     }

   Esnprintf(buf, len, "%-8s %s%-16s %s %s", MatchType[(int)wm->match],
	     qual, value, MatchOp[(int)wm->op], args);

   return buf;
}

static int
WindowMatchConfigLoad2(FILE * fs)
{
   char                s[FILEPATH_LEN_MAX], *ss;
   int                 len;

   for (;;)
     {
	ss = fgets(s, sizeof(s), fs);
	if (!ss)
	   break;

	len = strcspn(s, "#\r\n");
	if (len <= 0)
	   continue;
	s[len] = '\0';

	WindowMatchDecode(s);
     }

   return 0;
}

static int
WindowMatchEwinTest(const WindowMatch * wm, const EWin * ewin)
{
   int                 match;

   match = 0;

   switch (wm->match)
     {
     case MATCH_TYPE_TITLE:
	return matchregexp(wm->value, EwinGetIcccmName(ewin));

     case MATCH_TYPE_WM_NAME:
	return matchregexp(wm->value, EwinGetIcccmCName(ewin));

     case MATCH_TYPE_WM_CLASS:
	return matchregexp(wm->value, EwinGetIcccmClass(ewin));

     case MATCH_TYPE_SIZE:
	match = (ewin->client.w >= wm->width_min &&
		 ewin->client.w <= wm->width_max &&
		 ewin->client.h >= wm->height_min &&
		 ewin->client.h <= wm->height_max);
	break;
     case MATCH_TYPE_SIZE_H:
	match = (ewin->client.w >= wm->width_min &&
		 ewin->client.w <= wm->width_max);
	break;
     case MATCH_TYPE_SIZE_V:
	match = (ewin->client.h >= wm->height_min &&
		 ewin->client.h <= wm->height_max);
	break;

     case MATCH_TYPE_PROP:
	switch (wm->prop)
	  {
	  case MATCH_PROP_TRANSIENT:
	     match = EwinIsTransient(ewin);
	     break;

	  case MATCH_PROP_SHAPED:
	     match = ewin->state.shaped;
	     break;

	  case MATCH_PROP_FIXEDSIZE:
	     match = ewin->props.no_resize_h && ewin->props.no_resize_v;
	     break;
	  case MATCH_PROP_FIXEDSIZE_H:
	     match = ewin->props.no_resize_h;
	     break;
	  case MATCH_PROP_FIXEDSIZE_V:
	     match = ewin->props.no_resize_v;
	     break;
	  }
     }

   if (wm->qual)
      match = !match;
   return match;
}

#if USE_COMPOSITE
static int
WindowMatchEobjTest(const WindowMatch * wm, const EObj * eo)
{
   int                 match;

   match = 0;

   switch (wm->match)
     {
     case MATCH_TYPE_TITLE:
	return matchregexp(wm->value, EobjGetName(eo));

     case MATCH_TYPE_WM_NAME:
	return matchregexp(wm->value, EobjGetCName(eo));

     case MATCH_TYPE_WM_CLASS:
	return matchregexp(wm->value, EobjGetClass(eo));
     }

   if (wm->qual)
      match = !match;
   return match;
}
#endif

typedef struct {
   int                 type;
   const EWin         *ewin;
} wmatch_type_data;

static int
WindowMatchTypeMatch(const void *data, const void *match)
{
   const WindowMatch  *wm = (WindowMatch *) data;
   const wmatch_type_data *wmtd = (wmatch_type_data *) match;

   return !(wm->op == wmtd->type && WindowMatchEwinTest(wm, wmtd->ewin));
}

static WindowMatch *
WindowMatchType(const EWin * ewin, int type)
{
   wmatch_type_data    wmtd;

   wmtd.type = type;
   wmtd.ewin = ewin;

   return (WindowMatch *) ecore_list_find(wm_list, WindowMatchTypeMatch, &wmtd);
}

Border             *
WindowMatchEwinBorder(const EWin * ewin)
{
   WindowMatch        *wm;

   wm = WindowMatchType(ewin, MATCH_OP_BORDER);
#if 0
   Eprintf("WindowMatchEwinBorder %s %s\n", EwinGetTitle(ewin),
	   (wm) ? BorderGetName(wm->border) : "???");
#endif
   if (wm)
      return wm->border;
   return NULL;
}

const char         *
WindowMatchEwinIcon(const EWin * ewin)
{
   WindowMatch        *wm;

   wm = WindowMatchType(ewin, MATCH_OP_ICON);
#if 0
   Eprintf("WindowMatchEwinIcon %s %s\n", EwinGetTitle(ewin),
	   (wm) ? wm->args : "???");
#endif
   if (wm)
      return wm->args;
   return NULL;
}

static int
GetBoolean(const char *value)
{
   /* We set off if "0" or "off", otherwise on */
   if (!value)
      return 1;
   else if (!strcmp(value, "0") || !strcmp(value, "off"))
      return 0;
   return 1;
}

#define WINOP_SET_BOOL(item, val) item = GetBoolean(val)

static void
WindowMatchEwinOpsAction(EWin * ewin, int op, const char *args)
{
   int                 a, b;

   /* NB! This must only be used when a new client is being adopted */

   switch (op)
     {
     default:
	/* We should not get here */
	return;

     case EWIN_OP_TITLE:
	Efree(EwinGetIcccmName(ewin));
	EwinGetIcccmName(ewin) = Estrdup(args);
	break;

     case EWIN_OP_ICONIFY:
	WINOP_SET_BOOL(ewin->icccm.start_iconified, args);
	break;

     case EWIN_OP_SHADE:
	WINOP_SET_BOOL(ewin->state.shaded, args);
	break;

     case EWIN_OP_STICK:
	WINOP_SET_BOOL(ewin->o.sticky, args);
	break;

     case EWIN_OP_DESK:
	EoSetDesk(ewin, DeskGetValid(atoi(args)));
	break;

#if 0				/* Causes crash */
     case EWIN_OP_AREA:
	a = b = 0;
	if (sscanf(args, "%u %u", &a, &b) < 2)
	   break;
	EwinMoveToArea(ewin, a, b);	/* FIXME - We should not move here */
	break;
#endif

     case EWIN_OP_MOVE:
	a = ewin->client.x;
	b = ewin->client.y;
	if (sscanf(args, "%i %i", &a, &b) < 2)
	   break;
	ewin->client.x = a;
	ewin->client.y = b;
	ewin->state.placed = 1;
	break;

     case EWIN_OP_SIZE:
	a = ewin->client.w;
	b = ewin->client.h;
	if (sscanf(args, "%u %u", &a, &b) < 2)
	   break;
	ewin->client.w = a;
	ewin->client.h = b;
	ewin->state.maximized_horz = ewin->state.maximized_vert = 0;
	break;

     case EWIN_OP_FULLSCREEN:
	WINOP_SET_BOOL(ewin->state.fullscreen, args);
	break;

     case EWIN_OP_LAYER:
	EoSetLayer(ewin, atoi(args));
	break;

     case EWIN_OP_OPACITY:
	a = atoi(args);
	ewin->ewmh.opacity = OpacityFromPercent(OpacityFix(a, 0));
	break;

     case EWIN_OP_FOCUSED_OPACITY:
	a = atoi(args);
	ewin->props.focused_opacity = OpacityFromPercent(OpacityFix(a, 0));
	break;

     case EWIN_OP_SKIP_LISTS:
	WINOP_SET_BOOL(ewin->props.skip_winlist, args);
	ewin->props.skip_focuslist = ewin->props.skip_ext_task =
	   ewin->props.skip_winlist;
	break;

     case EWIN_OP_FOCUS_CLICK:
	WINOP_SET_BOOL(ewin->props.focusclick, args);
	break;

     case EWIN_OP_NEVER_USE_AREA:
	WINOP_SET_BOOL(ewin->props.never_use_area, args);
	break;

     case EWIN_OP_NO_BUTTON_GRABS:
	WINOP_SET_BOOL(ewin->props.no_button_grabs, args);
	break;

     case EWIN_OP_AUTOSHADE:
	WINOP_SET_BOOL(ewin->props.autoshade, args);
	break;

     case EWIN_OP_INH_APP_FOCUS:
	WINOP_SET_BOOL(EwinInhGetApp(ewin, focus), args);
	break;

     case EWIN_OP_INH_APP_MOVE:
	WINOP_SET_BOOL(EwinInhGetApp(ewin, move), args);
	break;

     case EWIN_OP_INH_APP_SIZE:
	WINOP_SET_BOOL(EwinInhGetApp(ewin, size), args);
	break;

     case EWIN_OP_INH_USER_CLOSE:
	WINOP_SET_BOOL(EwinInhGetUser(ewin, close), args);
	break;

     case EWIN_OP_INH_USER_MOVE:
	WINOP_SET_BOOL(EwinInhGetUser(ewin, move), args);
	break;

     case EWIN_OP_INH_USER_SIZE:
	WINOP_SET_BOOL(EwinInhGetUser(ewin, size), args);
	break;

     case EWIN_OP_INH_WM_FOCUS:
	WINOP_SET_BOOL(EwinInhGetWM(ewin, focus), args);
	break;

#if USE_COMPOSITE
     case EWIN_OP_FADE:
	WINOP_SET_BOOL(ewin->o.fade, args);
	break;

     case EWIN_OP_SHADOW:
	WINOP_SET_BOOL(ewin->o.shadow, args);
	break;

     case EWIN_OP_NO_REDIRECT:
	WINOP_SET_BOOL(ewin->o.noredir, args);
	break;

     case EWIN_OP_NO_ARGB:
	WINOP_SET_BOOL(ewin->props.no_argb, args);
	break;
#endif
     }
}

#if USE_COMPOSITE
static void
WindowMatchEobjOpsAction(EObj * eo, int op, const char *args)
{
   int                 a;

   switch (op)
     {
     default:
	/* We should not get here */
	return;

     case EWIN_OP_OPACITY:
	a = atoi(args);
	eo->opacity = OpacityFromPercent(OpacityFix(a, 100));
	break;

     case EWIN_OP_FADE:
	WINOP_SET_BOOL(eo->fade, args);
	break;

     case EWIN_OP_SHADOW:
	WINOP_SET_BOOL(eo->shadow, args);
	break;

     case EWIN_OP_NO_REDIRECT:
	WINOP_SET_BOOL(eo->noredir, args);
	break;
     }
}
#endif

static int
WindowMatchEobjOpsParse(EObj * eo, const char *ops)
{
   int                 err, len;
   const WinOp        *wop;
   char               *ops2, *s, *p, op[32];

   if (!ops)
      return -1;

   /* Parse ':' separated operations list, e.g. "layer 3:desk 1: shade" */
   p = ops2 = Estrdup(ops);

   err = 0;
   for (; p; p = s)
     {
	/* Break at ':' */
	s = strchr(p, ':');
	if (s)
	   *s++ = '\0';

	len = 0;
	sscanf(p, "%31s %n", op, &len);
	if (len <= 0)
	   break;
	p += len;

	wop = EwinOpFind(op);
	if (!wop || !wop->ok_match)
	  {
	     err = -1;
	     break;
	  }

	/* If eo is NULL, we are validating the configuration */
	if (!eo)
	   continue;
#if USE_COMPOSITE
	if (eo->type == EOBJ_TYPE_EWIN)
	   WindowMatchEwinOpsAction((EWin *) eo, wop->op, p);
	else
	   WindowMatchEobjOpsAction(eo, wop->op, p);
#else
	WindowMatchEwinOpsAction((EWin *) eo, wop->op, p);
#endif
     }

   Efree(ops2);

   return err;
}

static void
_WindowMatchEwinFunc(void *_wm, void *_ew)
{
   const WindowMatch  *wm = (WindowMatch *) _wm;
   EWin               *ew = (EWin *) _ew;

   if (wm->op != MATCH_OP_WINOP || !WindowMatchEwinTest(wm, ew))
      return;

   /* Match found - do the ops */
   WindowMatchEobjOpsParse(EoObj(ew), wm->args);
}

void
WindowMatchEwinOps(EWin * ew)
{
   ecore_list_for_each(wm_list, _WindowMatchEwinFunc, ew);
}

#if USE_COMPOSITE
static void
_WindowMatchEobjFunc(void *_wm, void *_eo)
{
   const WindowMatch  *wm = (WindowMatch *) _wm;
   EObj               *eo = (EObj *) _eo;

   if (wm->op != MATCH_OP_WINOP || !WindowMatchEobjTest(wm, eo))
      return;

   /* Match found - do the ops */
   WindowMatchEobjOpsParse(eo, wm->args);
}

void
WindowMatchEobjOps(EObj * eo)
{
   ecore_list_for_each(wm_list, _WindowMatchEobjFunc, eo);
}
#endif

/*
 * Winmatch module
 */

static void
WindowMatchSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
#if 0				/* Done as part of theme loading */
	ConfigFileLoad("windowmatches.cfg", Mode.theme.path,
		       WindowMatchConfigLoad, 1);
#endif
	ConfigFileLoad("matches.cfg", NULL, WindowMatchConfigLoad2, 0);
#if 0
	WindowMatchConfigLoadUser();
#endif
#if 0
	IcondefChecker(0, NULL);
#endif
	break;
     }
}

static void
WindowMatchIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096], buf[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
     }
   else if (!strncmp(cmd, "list", 2))
     {
	WindowMatch        *wm;

	ECORE_LIST_FOR_EACH(wm_list, wm)
	   IpcPrintf("%s\n", WindowMatchEncode(wm, buf, sizeof(buf)));
     }
}

static const IpcItem WindowMatchIpcArray[] = {
   {
    WindowMatchIpc,
    "wmatch", "wma",
    "Window match functions",
    "  wmatch list               List window matches\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(WindowMatchIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModWindowMatch;

const EModule       ModWindowMatch = {
   "winmatch", NULL,
   WindowMatchSighan,
   {N_IPC_FUNCS, WindowMatchIpcArray}
   ,
   {0, NULL}
};
