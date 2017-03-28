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
#include "borders.h"
#include "buttons.h"
#include "cursors.h"
#include "desktops.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "grabs.h"
#include "slide.h"
#include "xwin.h"

#define SLIDEOUT_EVENT_MASK \
  (KeyPressMask | KeyReleaseMask | \
   ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask | \
   PointerMotionMask)

typedef struct {
   EObj                o;
   char               *name;
   char                direction;
   int                 num_objs;
   EObj              **objs;
   unsigned int        ref_count;
   EWin               *context_ewin;
} Slideout;

static void         SlideoutCalcSize(Slideout * s);

static Ecore_List  *slideout_list = NULL;

static struct {
   Slideout           *active;
} Mode_slideouts =
{
NULL};

static void         SlideoutHandleEvent(Win win, XEvent * ev, void *prm);

static Slideout    *
SlideoutCreate(const char *name, char dir)
{
   Slideout           *s;

   s = ECALLOC(Slideout, 1);
   if (!s)
      return NULL;

   s->name = Estrdup(name);
   s->direction = dir;

   EoInit(s, EOBJ_TYPE_MISC, None, -10, -10, 1, 1, 1, name);
   EoSetShadow(s, 1);
   ESelectInput(EoGetWin(s), SLIDEOUT_EVENT_MASK);
   EventCallbackRegister(EoGetWin(s), SlideoutHandleEvent, s);

   return s;
}

static void
SlideoutShow(Slideout * s, EWin * ewin, Win win)
{
   int                 x, y, i, xx, yy, sw, sh;
   char                pdir;
   XSetWindowAttributes att;
   int                 w, h;
   Desk               *dsk;

   /* Don't ever show more than one slideout */
   if (Mode_slideouts.active)
      return;

   SlideoutCalcSize(s);
   EGetGeometry(win, NULL, NULL, NULL, &w, &h, NULL, NULL);
   ETranslateCoordinates(win, VROOT, 0, 0, &x, &y, NULL);

   sw = EoGetW(s);
   sh = EoGetH(s);
   xx = 0;
   yy = 0;
   switch (s->direction)
     {
     case 2:
	xx = x + ((w - sw) >> 1);
	yy = y - sh;
	if ((yy < 0) && (sh < WinGetH(VROOT)))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 3:
	xx = x + ((w - sw) >> 1);
	yy = y + h;
	if (((yy + sh) > WinGetH(VROOT)) && (sh < WinGetH(VROOT)))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 0:
	xx = x - sw;
	yy = y + ((h - sh) >> 1);
	if ((xx < 0) && (sw < WinGetW(VROOT)))
	  {
	     pdir = s->direction;
	     s->direction = 1;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     case 1:
	xx = x + w;
	yy = y + ((h - sh) >> 1);
	if (((xx + sw) > WinGetW(VROOT)) && (sw < WinGetW(VROOT)))
	  {
	     pdir = s->direction;
	     s->direction = 0;
	     SlideoutShow(s, ewin, win);
	     s->direction = pdir;
	     return;
	  }
	break;
     default:
	break;
     }

   if (ewin)
     {
	/* If the slideout is associated with an ewin,
	 * put it on the same virtual desktop. */
	dsk = EoGetDesk(ewin);
	if (BorderWinpartIndex(ewin, win) >= 0 &&
	    !EoIsFloating(ewin) /* && !ewin->sticky */ )
	  {
	     xx -= EoGetX(dsk);
	     yy -= EoGetY(dsk);
	  }
	EoSetLayer(s, EoGetLayer(ewin));
     }
   else
     {
	dsk = DeskGet(0);
	EoSetLayer(s, 10);
	EoSetFloating(s, 1);
     }
   EoReparent(s, EoObj(dsk), xx, yy);

   switch (s->direction)
     {
     case 0:
	att.win_gravity = SouthEastGravity;
	EChangeWindowAttributes(EoGetWin(s), CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_objs; i++)
	   EChangeWindowAttributes(EobjGetWin(s->objs[i]), CWWinGravity, &att);
	EoMoveResize(s, xx, yy, 1, 1);
	ESync(ESYNC_SLIDEOUT);
	EoMap(s, 2);
	EobjSlideSizeTo(EoObj(s), xx + sw, yy, xx, yy, 1, sh, sw, sh,
			Conf.shading.speed);
	break;
     case 1:
	att.win_gravity = NorthWestGravity;
	EChangeWindowAttributes(EoGetWin(s), CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_objs; i++)
	   EChangeWindowAttributes(EobjGetWin(s->objs[i]), CWWinGravity, &att);
	EoMoveResize(s, xx, yy, 1, 1);
	ESync(ESYNC_SLIDEOUT);
	EoMap(s, 2);
	EobjSlideSizeTo(EoObj(s), xx, yy, xx, yy, 1, sh, sw, sh,
			Conf.shading.speed);
	break;
     case 2:
	att.win_gravity = SouthEastGravity;
	EChangeWindowAttributes(EoGetWin(s), CWWinGravity, &att);
	att.win_gravity = NorthWestGravity;
	for (i = 0; i < s->num_objs; i++)
	   EChangeWindowAttributes(EobjGetWin(s->objs[i]), CWWinGravity, &att);
	EoMoveResize(s, xx, yy, 1, 1);
	ESync(ESYNC_SLIDEOUT);
	EoMap(s, 2);
	EobjSlideSizeTo(EoObj(s), xx, yy + sh, xx, yy, sw, 1, sw, sh,
			Conf.shading.speed);
	break;
     case 3:
	att.win_gravity = NorthWestGravity;
	EChangeWindowAttributes(EoGetWin(s), CWWinGravity, &att);
	att.win_gravity = SouthEastGravity;
	for (i = 0; i < s->num_objs; i++)
	   EChangeWindowAttributes(EobjGetWin(s->objs[i]), CWWinGravity, &att);
	EoMoveResize(s, xx, yy, 1, 1);
	ESync(ESYNC_SLIDEOUT);
	EoMap(s, 2);
	EobjSlideSizeTo(EoObj(s), xx, yy, xx, yy, sw, 1, sw, sh,
			Conf.shading.speed);
	break;
     default:
	break;
     }
   s->ref_count++;
   s->context_ewin = ewin;

   GrabPointerSet(EoGetWin(s), ECSR_ROOT, 0);

   Mode_slideouts.active = s;
}

static void
SlideoutHide(Slideout * s)
{
   if (!s)
      return;

   GrabPointerRelease();
   EoUnmap(s);
   s->context_ewin = NULL;
   s->ref_count--;
   Mode_slideouts.active = NULL;
}

static void
SlideoutCalcSize(Slideout * s)
{
   int                 i, x, y;
   int                 sw, sh, bw, bh;

   if (!s)
      return;

   sw = 0;
   sh = 0;
   x = 0;
   y = 0;
   for (i = 0; i < s->num_objs; i++)
     {
	bw = EobjGetW(s->objs[i]);
	bh = EobjGetH(s->objs[i]);

	switch (s->direction)
	  {
	  case 2:
	  case 3:
	     if (bw > sw)
		sw = bw;
	     sh += bh;
	     break;
	  case 0:
	  case 1:
	     if (bh > sh)
		sh = bh;
	     sw += bw;
	     break;
	  default:
	     break;
	  }
     }

   EoResize(s, sw, sh);

   for (i = 0; i < s->num_objs; i++)
     {
	bw = EobjGetW(s->objs[i]);
	bh = EobjGetH(s->objs[i]);

	switch (s->direction)
	  {
	  case 2:
	     y += bh;
	     EMoveWindow(EobjGetWin(s->objs[i]), (sw - bw) >> 1, sh - y);
	     break;
	  case 3:
	     EMoveWindow(EobjGetWin(s->objs[i]), (sw - bw) >> 1, y);
	     y += bh;
	     break;
	  case 0:
	     x += bw;
	     EMoveWindow(EobjGetWin(s->objs[i]), sw - x, (sh - bh) >> 1);
	     break;
	  case 1:
	     EMoveWindow(EobjGetWin(s->objs[i]), x, (sh - bh) >> 1);
	     x += bw;
	     break;
	  default:
	     break;
	  }
     }
   EShapePropagate(EoGetWin(s));
}

static void
SlideoutButtonCallback(EObj * seo, XEvent * ev, ActionClass * ac)
{
   Slideout           *s = (Slideout *) seo;
   EWin               *ewin = s->context_ewin;

   if (ev->type == ButtonRelease)
      SlideoutHide(s);

   if (ac)
      ActionclassEvent(ac, ev, ewin);
}

static void
SlideoutAddButton(Slideout * s, Button * b)
{
   EObj               *eob = (EObj *) b;

   if (!s || !b)
      return;

   s->num_objs++;
   s->objs = EREALLOC(EObj *, s->objs, s->num_objs);
   s->objs[s->num_objs - 1] = eob;
   ButtonSwallowInto(b, EoObj(s));
   ButtonSetCallback(b, SlideoutButtonCallback, EoObj(s));
   SlideoutCalcSize(s);
}

#if 0
static void
SlideoutRemoveButton(Slideout * s, Button * b)
{
   s = NULL;
   b = NULL;
}
#endif

static void
SlideoutHandleEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Slideout           *s = (Slideout *) prm;

   switch (ev->type)
     {
     case KeyPress:
     case KeyRelease:
	SlideoutHide(s);
	break;
     case ButtonPress:
	break;
     case ButtonRelease:
	SlideoutHide(s);
	break;
     case EnterNotify:
	if (ev->xcrossing.mode != NotifyGrab)
	   GrabPointerRelease();
	break;
     case LeaveNotify:
	if (ev->xcrossing.mode != NotifyUngrab)
	   GrabPointerSet(EoGetWin(s), ECSR_ROOT, 0);
	break;
     }
}

static void
SlideoutsHide(void)
{
   if (Mode_slideouts.active)
      SlideoutHide(Mode_slideouts.active);
}

/*
 * Configuration load/save
 */
#include "conf.h"

int
SlideoutsConfigLoad(FILE * fs)
{
   int                 err = 0;
   Slideout           *slideout = 0;
   int                 i1;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char                name[FILEPATH_LEN_MAX];

   if (!slideout_list)
      slideout_list = ecore_list_new();

   name[0] = '\0';

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     if (slideout)
		ecore_list_prepend(slideout_list, slideout);
	     goto done;
	  case CONFIG_CLASSNAME:
	     strcpy(name, s2);
	     break;
	  case SLIDEOUT_DIRECTION:
	     slideout = SlideoutCreate(name, (char)atoi(s2));
	     break;
	  case CONFIG_BUTTON:
	     SlideoutAddButton(slideout, ButtonFind(s2));
	     break;
	  default:
	     ConfigParseError("Slideout", s);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

/*
 * Slideouts Module
 */

static void
SlideoutsSighan(int sig, void *prm)
{
   switch (sig)
     {
     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	SlideoutsHide();
	break;

     case ESIGNAL_EWIN_UNMAP:
	if (Mode_slideouts.active
	    && Mode_slideouts.active->context_ewin == (EWin *) prm)
	   SlideoutsHide();
	break;
     }
}

static int
_SlideoutMatchName(const void *data, const void *match)
{
   return strcmp(((const Slideout *)data)->name, (const char *)match);
}

static Slideout    *
SlideoutFind(const char *name)
{
   return (Slideout *) ecore_list_find(slideout_list, _SlideoutMatchName, name);
}

static void
IPC_Slideout(const char *params)
{
   Slideout           *s;

   if (!params)
      return;

   s = SlideoutFind(params);
   if (!s)
      return;

   SoundPlay(SOUND_SLIDEOUT_SHOW);
   SlideoutShow(s, GetContextEwin(), Mode.context_win);
}

static const IpcItem SlideoutsIpcArray[] = {
   {
    IPC_Slideout, "slideout", NULL, "Show slideout", NULL},
};
#define N_IPC_FUNCS (sizeof(SlideoutsIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModSlideouts;

const EModule       ModSlideouts = {
   "slideouts", "slideout",
   SlideoutsSighan,
   {N_IPC_FUNCS, SlideoutsIpcArray},
   {0, NULL}
};
