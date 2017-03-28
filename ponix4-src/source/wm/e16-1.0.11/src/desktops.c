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
#include "buttons.h"
#include "desktops.h"
#include "dialog.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"
#include "iclass.h"
#include "screen.h"
#include "settings.h"
#include "slide.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <time.h>
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#define DESK_EVENT_MASK1 \
  (ButtonPressMask | ButtonReleaseMask)
#define DESK_EVENT_MASK2 \
  (EnterWindowMask | LeaveWindowMask | \
   SubstructureNotifyMask | SubstructureRedirectMask | PropertyChangeMask)

#define DESK_EVENT_MASK (DESK_EVENT_MASK1 | DESK_EVENT_MASK2)

#define ENLIGHTENMENT_CONF_NUM_DESKTOPS 32

typedef struct {
   Desk               *current;
   Desk               *previous;
   Desk               *desk[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   unsigned int        order[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   int                 drag_x0, drag_y0;
   unsigned int        prev_num;
} Desktops;

static void         DeskRaise(unsigned int num);
static void         DeskLower(unsigned int num);
static void         DeskHandleEvents(Win win, XEvent * ev, void *prm);
static void         DeskButtonCallback(EObj * eo, XEvent * ev,
				       ActionClass * ac);

/* The desktops */
static Desktops     desks;

#define _DeskGet(d) (desks.desk[d])

static void
DeskControlsCreate(Desk * dsk)
{
   char                s[512];
   const char         *ic1, *ic2, *ic3;

#if ENABLE_DESKRAY
   const char         *ic4;
#endif
   char                ac1[64], ac2[64], ac3[64];
   Button             *b;
   ActionClass        *ac;
   Action             *a;
   int                 x[3], y[3], w[3], h[3], m, n, o;
   const char         *t;

   if (Conf.desks.dragdir < 0 || Conf.desks.dragdir > 3)
      Conf.desks.dragdir = 2;
   if (Conf.desks.dragbar_ordering < 0 || Conf.desks.dragbar_ordering > 5)
      Conf.desks.dragbar_ordering = 1;
   if (Conf.desks.dragbar_width < 0)
      Conf.desks.dragbar_width = 0;
   else if (Conf.desks.dragbar_width > 64)
      Conf.desks.dragbar_width = 64;
   if (Conf.desks.dragbar_length < 0)
      Conf.desks.dragbar_length = 0;
   else if (Conf.desks.dragbar_length > WinGetW(VROOT))
      Conf.desks.dragbar_length = WinGetW(VROOT);

   Esnprintf(ac1, sizeof(ac1), "DRAGBAR_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac1))
     {
	ac = ActionclassCreate(ac1, 0);
	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 1, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	Esnprintf(s, sizeof(s), "desk drag %i", dsk->num);
	ActionAddTo(a, s);

	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 3, 0, NULL, NULL);
	ActionclassAddAction(ac, a);
	ActionAddTo(a, "menus show deskmenu");

	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 2, 0, NULL, NULL);
	ActionclassAddAction(ac, a);
	ActionAddTo(a, "menus show windowlist");

	if (dsk->num > 0)
	  {
	     t = _("Hold down the mouse button and drag\n"
		   "the mouse to be able to drag the desktop\n"
		   "back and forth.\n"
		   "Click right mouse button for a list of all\n"
		   "Desktops and their applications.\n"
		   "Click middle mouse button for a list of all\n"
		   "applications currently running.\n");
	     ActionclassSetTooltipString(ac, t);
	  }
	else
	  {
	     t = _("This is the Root desktop.\n"
		   "You cannot drag the root desktop around.\n"
		   "Click right mouse button for a list of all\n"
		   "Desktops and their applications.\n"
		   "Click middle mouse button for a list of all\n"
		   "applications currently running.\n");
	     ActionclassSetTooltipString(ac, t);
	  }
     }

   Esnprintf(ac2, sizeof(ac2), "RAISEBUTTON_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac2))
     {
	ac = ActionclassCreate(ac2, 0);
	a = ActionCreate(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	ActionAddTo(a, "desk next");
	t = _("Click here to raise this desktop\nto the top.");
	ActionclassSetTooltipString(ac, t);
     }

   Esnprintf(ac3, sizeof(ac3), "LOWERBUTTON_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac3))
     {
	ac = ActionclassCreate(ac3, 0);
	a = ActionCreate(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	ActionAddTo(a, "desk prev");
	t = _("Click here to lower this desktop\nto the bottom.");
	ActionclassSetTooltipString(ac, t);
     }

   if (Conf.desks.dragdir < 2)
     {
	ic1 = "DESKTOP_DRAGBUTTON_VERT";
	ic2 = "DESKTOP_RAISEBUTTON_VERT";
	ic3 = "DESKTOP_LOWERBUTTON_VERT";
#if ENABLE_DESKRAY
	ic4 = "DESKTOP_DESKRAY_VERT";
#endif
     }
   else
     {
	ic1 = "DESKTOP_DRAGBUTTON_HORIZ";
	ic2 = "DESKTOP_RAISEBUTTON_HORIZ";
	ic3 = "DESKTOP_LOWERBUTTON_HORIZ";
#if ENABLE_DESKRAY
	ic4 = "DESKTOP_DESKRAY_HORIZ";
#endif
     }

   switch (Conf.desks.dragbar_ordering)
     {
     case 0:
	m = 0;
	n = 1;
	o = 2;
	break;
     case 1:
	m = 0;
	n = 2;
	o = 1;
	break;
     case 2:
	m = 2;
	n = 0;
	o = 1;
	break;
     case 3:
	m = 1;
	n = 0;
	o = 2;
	break;
     case 4:
	m = 1;
	n = 2;
	o = 0;
	break;
     case 5:
	m = 2;
	n = 1;
	o = 0;
	break;
     default:
	m = 0;
	n = 1;
	o = 2;
	break;
     }

   switch (Conf.desks.dragdir)
     {
     default:
     case 0:
	w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   h[2] = WinGetH(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   h[2] = Conf.desks.dragbar_length;
	x[0] = x[1] = x[2] = 0;
	y[m] = 0;
	y[n] = y[m] + h[m];
	y[o] = y[n] + h[n];
	break;
     case 1:
	w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   h[2] = WinGetH(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   h[2] = Conf.desks.dragbar_length;
	x[0] = x[1] = x[2] = WinGetW(VROOT) - Conf.desks.dragbar_width;
	y[m] = 0;
	y[n] = y[m] + h[m];
	y[o] = y[n] + h[n];
	break;
     case 2:
	h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   w[2] = WinGetW(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   w[2] = Conf.desks.dragbar_length;
	y[0] = y[1] = y[2] = 0;
	x[m] = 0;
	x[n] = x[m] + w[m];
	x[o] = x[n] + w[n];
	break;
     case 3:
	h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   w[2] = WinGetW(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   w[2] = Conf.desks.dragbar_length;
	y[0] = y[1] = y[2] = WinGetH(VROOT) - Conf.desks.dragbar_width;
	x[m] = 0;
	x[n] = x[m] + w[m];
	x[o] = x[n] + w[n];
	break;
     }

   b = NULL;

   if (Conf.desks.dragbar_width > 0)
     {
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic2, ac2, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[0], 0,
			 y[0], 0, 0, w[0], 0, h[0], 0, dsk->num, 0);
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic3, ac3, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[1], 0,
			 y[1], 0, 0, w[1], 0, h[1], 0, dsk->num, 0);
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic1, ac1, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[2], 0,
			 y[2], 0, 0, w[2], 0, h[2], 0, dsk->num, 0);
	ButtonSetCallback(b, DeskButtonCallback, EoObj(dsk));
     }

#if ENABLE_DESKRAY
   if (dsk->num > 0)
     {
	if (Conf.desks.dragdir == 0)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else if (Conf.desks.dragdir == 1)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
			      99999, 0, 0,
			      EoGetX(dsk) + WinGetW(VROOT) -
			      Conf.desks.dragbar_width, 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else if (Conf.desks.dragdir == 2)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0,
			      EoGetY(dsk) + WinGetH(VROOT) -
			      Conf.desks.dragbar_width, 0, 0, 0, 0, 0, 1, 0, 1);
	  }
     }
#endif

   dsk->tag = b;
}

static void
DeskControlsDestroy(Desk * dsk, int id)
{
   ButtonsForeach(id, dsk, ButtonDestroy);
}

static void
DeskControlsShow(Desk * dsk, int id)
{
   ButtonsForeach(id, dsk, ButtonShow);
}

static void
DeskConfigure(Desk * dsk)
{
   Background         *bg;

   DeskControlsCreate(dsk);
   DeskControlsShow(dsk, 1);

   bg = BackgroundGetForDesk(dsk->num);
   DeskBackgroundSet(dsk, bg);

   dsk->bg.pixel = 0xffffffff;

   if (dsk->num > 0)
     {
	EoMove(dsk, WinGetW(VROOT), 0);
	EoMap(dsk, 0);
     }

   ModulesSignal(ESIGNAL_DESK_ADDED, dsk);
}

static Desk        *
DeskCreate(int desk, int configure)
{
#if USE_COMPOSITE
   EObj               *eo;
#endif
   Desk               *dsk;
   Win                 win;
   char                buf[64];

   if (desk < 0 || desk >= ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      return NULL;

   dsk = ECALLOC(Desk, 1);
   if (!dsk)
      return dsk;

   desks.desk[desk] = dsk;
   dsk->num = desk;
   desks.order[desk] = desk;

   win = (desk == 0) ? VROOT : NULL;

   Esnprintf(buf, sizeof(buf), "Desk-%d", desk);
   EoSetNoRedirect(dsk, 1);
   EoInit(dsk, EOBJ_TYPE_DESK, win,
	  0, 0, WinGetW(VROOT), WinGetH(VROOT), 0, buf);
   EventCallbackRegister(EoGetWin(dsk), DeskHandleEvents, dsk);
   dsk->bg.o = EoObj(dsk);
   if (desk == 0)
     {
	desks.current = dsk;
#if !USE_BG_WIN_ON_ALL_DESKS	/* TBD - Use per virtual root bg window? */
#if USE_COMPOSITE
	/* Add background window */
	eo = EobjWindowCreate(EOBJ_TYPE_ROOT_BG,
			      0, 0, WinGetW(VROOT), WinGetH(VROOT),
			      0, "Root-bg");
	eo->floating = 0;
	EobjSetLayer(eo, 0);
	EventCallbackRegister(EobjGetWin(eo), DeskHandleEvents, dsk);
	dsk->bg.o_bg = eo;
	ESelectInput(EobjGetWin(eo), EnterWindowMask);
#endif
#endif
     }
   else
     {
	EoSetFloating(dsk, 1);
	EoSetLayer(dsk, 0);
	/* Set the _XROOT... atoms so apps will find them even before the bg is set */
	HintsSetRootInfo(EoGetWin(dsk), None, 0);
     }

#if USE_BG_WIN_ON_ALL_DESKS	/* TBD - Use per virtual root bg window? */
#if USE_COMPOSITE
   /* Add background window */
   Esnprintf(buf, sizeof(buf), "Desk-bg-%d", desk);
   eo = EobjWindowCreate(EOBJ_TYPE_MISC,
			 0, 0, WinGetW(VROOT), WinGetH(VROOT), 0, buf);
   eo->floating = 0;
   EobjReparent(eo, EoObj(dsk), 0, 0);
   EobjSetLayer(eo, 0);
   dsk->bg.o_bg = eo;
   EventCallbackRegister(EobjGetWin(eo), DeskHandleEvents, dsk);
#endif
#endif

   HintsSetRootHints(EoGetWin(dsk));

   if (configure)
      DeskConfigure(dsk);

   /* Root window: Don't include ButtonPressMask as it may cause the event
    * selection to fail for the other events too.
    * The ButtonPress/ReleaseMask events are selected in SetupX(). */
   if (desk == 0)
      ESelectInputChange(EoGetWin(dsk), DESK_EVENT_MASK2, 0);
   else
      ESelectInput(EoGetWin(dsk), DESK_EVENT_MASK);

   return dsk;
}

static void
DeskDestroy(Desk * dsk)
{
   ModulesSignal(ESIGNAL_DESK_REMOVED, dsk);

#if USE_COMPOSITE
   if (dsk->bg.o_bg)
     {
	EventCallbackUnregister(EobjGetWin(dsk->bg.o_bg), DeskHandleEvents,
				dsk);
	EobjWindowDestroy(dsk->bg.o_bg);
     }
#endif
   EventCallbackUnregister(EoGetWin(dsk), DeskHandleEvents, dsk);

   DeskControlsDestroy(dsk, 1);
   DeskControlsDestroy(dsk, 2);

   if (dsk->bg.bg)
      BackgroundDecRefcount(dsk->bg.bg);

   EoFini(dsk);

   desks.desk[dsk->num] = NULL;
   Efree(dsk);
}

EObj               *
DeskGetBackgroundObj(const Desk * dsk)
{
   return (dsk) ? dsk->bg.o : NULL;
}

Pixmap
DeskGetBackgroundPixmap(const Desk * dsk)
{
   if (!dsk)
      return None;
   return dsk->bg.pmap;
}

Background         *
DeskBackgroundGet(const Desk * dsk)
{
   return (dsk) ? dsk->bg.bg : NULL;
}

static void
DeskBackgroundConfigure(Desk * dsk)
{
   Win                 win;
   Pixmap              pmap = dsk->bg.pmap;
   unsigned int        pixel = dsk->bg.pixel;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf
	 ("DeskBackgroundConfigure %d v=%d %#lx/%#lx: ext=%d pmap=%#lx/%#lx pixel=%#x/%#x\n",
	  dsk->num, dsk->viewable, EoGetXwin(dsk), EobjGetXwin(dsk->bg.o),
	  BackgroundIsNone(dsk->bg.bg), pmap, dsk->bg.pmap_set, pixel,
	  dsk->bg.pixel);

#if USE_COMPOSITE
   if (dsk->bg.o_bg)
     {
	if (ECompMgrIsActive())
	  {
	     dsk->bg.o = dsk->bg.o_bg;
	     EobjMap(dsk->bg.o_bg, 0);
	  }
	else
	  {
	     dsk->bg.o = EoObj(dsk);
	     EobjUnmap(dsk->bg.o_bg);
	  }
     }
#endif

   win = EobjGetWin(dsk->bg.o);

   if (dsk->viewable || !dsk->bg.bg)
     {
#if USE_COMPOSITE && !USE_BG_WIN_ON_ALL_DESKS
	if (ECompMgrIsActive())
	  {
	     ECompMgrDeskConfigure(dsk);
	     ESetWindowBackgroundPixmap(win, None);
	  }
	else
#endif
	  {
	     if (pmap != None)
	       {
		  ESetWindowBackgroundPixmap(win, pmap);
		  if (dsk->num == 0 && win != VROOT)
		     ESetWindowBackgroundPixmap(VROOT, pmap);
	       }
	     else
	       {
		  ESetWindowBackground(win, pixel);
		  if (dsk->num == 0 && win != VROOT)
		     ESetWindowBackground(VROOT, pixel);
	       }
	     EClearWindow(win);
	  }

	HintsSetRootInfo(EoGetWin(dsk), pmap, pixel);
     }
   else if (dsk->bg.bg)
     {
	if (!Conf.hints.set_xroot_info_on_root_window)
	   HintsSetRootInfo(EoGetWin(dsk), None, 0);

	ESetWindowBackgroundPixmap(win, None);
	if (dsk->num == 0 && win != VROOT)
	   ESetWindowBackgroundPixmap(VROOT, None);
     }
}

static void
DeskBackgroundRefresh(Desk * dsk, int why)
{
   Background         *bg = dsk->bg.bg;
   Pixmap              pmap = dsk->bg.pmap;
   unsigned int        pixel = dsk->bg.pixel;
   int                 changed = 0;
   int                 reconfigure = 0;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskBackgroundRefresh %d v=%d why=%d pmap=%#lx pixel=%#x\n",
	      dsk->num, dsk->viewable, why, pmap, pixel);

   switch (why)
     {
     case DESK_BG_REFRESH:
	if (bg && dsk->viewable)
	   BackgroundTouch(bg);
	break;

     case DESK_BG_RECONFIGURE_ALL:
	reconfigure = 1;
	break;

     case DESK_BG_TIMEOUT:
	if (bg && dsk->viewable)
	   BackgroundTouch(bg);
	return;

     case DESK_BG_FREE:
	if (!bg || dsk->viewable)
	   return;
	break;
     }

   if (bg)
     {
	if (dsk->viewable)
	  {
	     if (BackgroundGetSeqNo(bg) == dsk->bg.seq_no)
		goto done;

	     pmap = BackgroundGetPixmap(bg);
	     pixel = 0;

	     if (pmap == None)
		BackgroundRealize(bg, EoGetWin(dsk), None,
				  EoGetW(dsk), EoGetH(dsk), 1, &pmap, &pixel);
	     if (pmap != None)
		BackgroundPixmapSet(bg, pmap);

	     dsk->bg.seq_no = BackgroundGetSeqNo(bg);
	     changed = 1;
	  }
	else
	  {
	     if (dsk->bg.pmap == None)
		return;

	     pmap = None;
	     pixel = 0;
	     dsk->bg.seq_no = 0;
	  }
     }
   else
     {
	pmap = (Mode.root.ext_pmap_valid) ? Mode.root.ext_pmap : None;
	pixel = 0;
	changed = pmap != dsk->bg.pmap_set;
     }

 done:
   dsk->bg.pmap = pmap;
   if (reconfigure || pmap != dsk->bg.pmap_set || pixel != dsk->bg.pixel)
     {
	dsk->bg.pixel = pixel;
	DeskBackgroundConfigure(dsk);
	dsk->bg.pmap_set = pmap;
     }

   if (bg && dsk->viewable)
      if (Conf.hints.set_xroot_info_on_root_window)
	 HintsSetRootInfo(VROOT, pmap, pixel);

   if (changed)
      ModulesSignal(ESIGNAL_BACKGROUND_CHANGE, dsk);
}

void
DeskBackgroundSet(Desk * dsk, Background * bg)
{
   if (!dsk)
      return;

   BackgroundSetForDesk(bg, dsk->num);
   if (bg && BackgroundIsNone(bg))
      bg = NULL;

   if (dsk->bg.bg != bg)
     {
	if (dsk->bg.bg)
	   BackgroundDecRefcount(dsk->bg.bg);
	if (bg)
	   BackgroundIncRefcount(bg);
     }

   dsk->bg.bg = bg;

   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
}

void
DesksBackgroundRefresh(Background * bg, int why)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	if (!dsk)		/* May happen during init */
	   continue;
	if (bg && dsk->bg.bg != bg)
	   continue;
	DeskBackgroundRefresh(dsk, why);
     }
}

static void
DeskResize(int desk, int w, int h)
{
   Desk               *dsk;
   int                 x;

   dsk = _DeskGet(desk);

   if (dsk->num != 0)
     {
	x = (dsk->viewable) ? EoGetX(dsk) : WinGetW(VROOT);
	EoMoveResize(dsk, x, 0, w, h);
     }
#if USE_COMPOSITE
   if (dsk->bg.o_bg)
      EobjMoveResize(dsk->bg.o_bg, 0, 0, w, h);
#endif
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   DeskControlsDestroy(dsk, 1);
   DeskControlsCreate(dsk);
   DeskControlsShow(dsk, 1);
}

Desk               *
DeskGet(unsigned int desk)
{
   if (desk >= Conf.desks.num)
      return NULL;

   return _DeskGet(desk);
}

Desk               *
DeskGetValid(unsigned int desk)
{
   if (desk >= Conf.desks.num)
      desk = 0;

   return _DeskGet(desk);
}

Desk               *
DeskGetRelative(Desk * dsk, int inc)
{
   unsigned int        desk, num;

   num = Conf.desks.num;
   desk = (dsk) ? dsk->num : 0;
   inc %= (int)num;
   desk += num + inc;
   desk %= num;

   return _DeskGet(desk);
}

void
DeskGetArea(const Desk * dsk, int *ax, int *ay)
{
   if (!dsk)
     {
	*ax = *ay = 0;
	return;
     }
   *ax = dsk->current_area_x;
   *ay = dsk->current_area_y;
}

void
DeskSetArea(Desk * dsk, int ax, int ay)
{
   if (!dsk)
      return;

   dsk->current_area_x = ax;
   dsk->current_area_y = ay;
}

void
DeskSetDirtyStack(Desk * dsk, EObj * eo)
{
   dsk->stack.dirty++;
   dsk->stack.latest = eo;
   if (EobjGetType(eo) == EOBJ_TYPE_EWIN)
      dsk->stack.update_client_list = 1;
   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskSetDirtyStack %d (%d): %s\n", dsk->num, dsk->stack.dirty,
	      EobjGetName(eo));
}

void
DeskCurrentGetArea(int *ax, int *ay)
{
   DeskGetArea(desks.current, ax, ay);
}

static void
DeskCurrentSetArea(int ax, int ay)
{
   DeskSetArea(desks.current, ax, ay);
}

unsigned int
DesksGetNumber(void)
{
   return Conf.desks.num;
}

Desk               *
DesksGetCurrent(void)
{
   return desks.current;
}

unsigned int
DesksGetCurrentNum(void)
{
   return desks.current->num;
}

void
DesksSetCurrent(Desk * dsk)
{
   if (!dsk)
      return;
   desks.current = dsk;
}

static void
DesksResize(int w, int h)
{
   unsigned int        i;

   BackgroundsInvalidate(0);

   for (i = 0; i < Conf.desks.num; i++)
      DeskResize(i, w, h);

   EdgeWindowsShow();

   ModulesSignal(ESIGNAL_DESK_RESIZE, NULL);
}

static void
ChangeNumberOfDesktops(unsigned int quantity)
{
   unsigned int        i;
   int                 j, num;
   EWin               *const *lst;

   if (quantity >= ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      quantity = ENLIGHTENMENT_CONF_NUM_DESKTOPS;

   if (quantity <= 0 || quantity == Conf.desks.num)
      return;

   for (i = quantity; i < Conf.desks.num; i++)
      DeskLower(i);

   if (quantity > Conf.desks.num)
     {
	while (Conf.desks.num < quantity)
	  {
	     Conf.desks.num++;
	     DeskCreate(Conf.desks.num - 1, 1);
	  }
     }
   else if (quantity < Conf.desks.num)
     {
	lst = EwinListGetAll(&num);
	for (j = 0; j < num; j++)
	  {
	     if (EoGetDeskNum(lst[j]) >= quantity)
		EwinMoveToDesktop(lst[j], _DeskGet(0));
	  }

	while (Conf.desks.num > quantity)
	  {
	     DeskDestroy(_DeskGet(Conf.desks.num - 1));
	     Conf.desks.num--;
	  }
     }

   if (DesksGetCurrentNum() >= Conf.desks.num)
      DeskGotoNum(Conf.desks.num - 1);

   HintsSetDesktopConfig();

   autosave();
}

static void
DesksControlsCreate(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsCreate(_DeskGet(i));
}

static void
DesksControlsDestroy(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsDestroy(_DeskGet(i), 1);
}

static void
DesksControlsShow(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsShow(_DeskGet(i), 1);
}

static void
DesksControlsRefresh(void)
{
   DesksControlsDestroy();
   DesksControlsCreate();
   DesksControlsShow();

   autosave();
}

#if 0				/* Unused */
static void
DeskShowTabs(void)
{
   ButtonsForeach(2, NULL, ButtonShow);
}

static void
DeskHideTabs(void)
{
   ButtonsForeach(2, NULL, ButtonHide);
}
#endif

static void
DeskButtonShowDefault(Button * b)
{
   if (!ButtonDoShowDefault(b))
      return;
   ButtonShow(b);
}

static void
DeskShowButtons(void)
{
   ButtonsForeach(0, NULL, DeskButtonShowDefault);
}

static void
MoveToDeskTop(Desk * dsk)
{
   int                 i, j;

   EoRaise(dsk);

   j = -1;
   i = 0;
   while (j < 0 && i < (int)Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   j = i;
	i++;
     }
   if (j < 0)
      return;
   if (j > 0)
     {
	for (i = j - 1; i >= 0; i--)
	   desks.order[i + 1] = desks.order[i];
	desks.order[0] = dsk->num;
     }
}

static void
MoveToDeskBottom(Desk * dsk)
{
   int                 i, j;

   EoLower(dsk);

   j = -1;
   i = 0;
   while (j < 0 && i < (int)Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   j = i;
	i++;
     }
   if (j < 0)
      return;
   if (j < (int)Conf.desks.num - 1)
     {
	for (i = j; i < (int)Conf.desks.num - 1; i++)
	   desks.order[i] = desks.order[i + 1];
	desks.order[Conf.desks.num - 1] = dsk->num;
     }
}

Desk               *
DesktopAt(int x, int y)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(desks.order[i]);
	if (x >= EoGetX(dsk) && x < (EoGetX(dsk) + WinGetW(VROOT)) &&
	    y >= EoGetY(dsk) && y < (EoGetY(dsk) + WinGetH(VROOT)))
	   return _DeskGet(desks.order[i]);
     }
   return _DeskGet(0);
}

static void
DesksStackingCheck(void)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	if (i && !dsk->viewable)
	   continue;
	if (!dsk->stack.dirty)
	   continue;
	DeskRestack(dsk);
     }
}

static void
_DesksIdler(void *data __UNUSED__)
{
   DesksStackingCheck();
}

static void
DeskMove(Desk * dsk, int x, int y)
{
   Desk               *dd;
   unsigned int        i;
   int                 n, v, dx, dy;

   if (dsk->num <= 0)
      return;

   n = -1;
   i = 0;
   while (n < 0 && i < Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   n = i;
	i++;
     }
   if (n < 0)			/* Should not be possible */
      return;

   dx = x - EoGetX(dsk);
   dy = y - EoGetY(dsk);

   if (x == 0 && y == 0)
     {
	/* Desks below are obscured - touch and set unviewable */
	for (i = n + 1; i < Conf.desks.num; i++)
	  {
	     dd = _DeskGet(desks.order[i]);
	     if (dd->viewable)
		BackgroundTouch(dd->bg.bg);
	     dd->viewable = 0;
	  }
     }
   else
     {
	v = dsk->viewable;

	for (i = n + 1; i < Conf.desks.num; i++)
	  {
	     dd = _DeskGet(desks.order[i]);
	     if (!dd->viewable && v)
	       {
		  dd->viewable = 1;
		  DeskBackgroundRefresh(_DeskGet(desks.order[i]),
					DESK_BG_REFRESH);
	       }
	     else if (dd->viewable && !v)
	       {
		  BackgroundTouch(dd->bg.bg);
		  dd->viewable = 0;
	       }

	     if (EoGetX(dd) == 0 && EoGetY(dd) == 0)
		v = 0;
	  }
     }

   EoMove(dsk, x, y);

   if (dsk->tag)
      ButtonMoveRelative(dsk->tag, dx, dy);

#if 0				/* FIXME - Remove? */
   EWin               *const *lst;

   lst = EwinListGetAll(&n);
   for (i = 0; i < (unsigned int)n; i++)
      if (EoGetDesk(lst[i]) == dsk)
	 ICCCM_Configure(lst[i]);
#endif
}

static void
DeskHide(unsigned int desk)
{
   Desk               *dsk;

   if (desk <= 0 || desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   if (dsk->viewable)
      BackgroundTouch(dsk->bg.bg);
   dsk->viewable = 0;
   EoMove(dsk, WinGetW(VROOT), 0);
}

static void
DeskEnter(Desk * dsk)
{
   int                 i;

   EGrabServer();

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   MoveToDeskTop(dsk);

   desks.prev_num = desks.current->num;
   desks.previous = desks.current = dsk;

   if (dsk->num == 0)
     {
	for (i = Conf.desks.num - 1; i > 0; i--)
	   DeskHide(desks.order[i]);
     }

   EwinsMoveStickyToDesk(dsk);
   ButtonsMoveStickyToDesk(dsk);
   DesksStackingCheck();
   EdgeWindowsShow();

   EUngrabServer();
}

void
DeskGotoNum(unsigned int desk)
{
   Desk               *dsk;

   if (Conf.desks.desks_wraparound)
      desk %= Conf.desks.num;
   if (desk >= Conf.desks.num || desk == desks.current->num)
      return;

   dsk = _DeskGet(desk);
   DeskGoto(dsk);
}

static void
DeskSwitchStart(void)
{
   FocusNewDeskBegin();
}

static void
DeskSwitchDone(void)
{
   HintsSetCurrentDesktop();
   FocusNewDesk();
}

void
DeskGoto(Desk * dsk)
{
   if (!dsk || dsk == desks.previous)
      return;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskGoto %d\n", dsk->num);

   ModulesSignal(ESIGNAL_DESK_SWITCH_START, NULL);

   MoveResizeSuspend();
   DeskSwitchStart();

   if (dsk->num > 0)
     {
	if (Conf.desks.slidein)
	  {
	     EObj               *eo = &dsk->o;

	     if (!dsk->viewable)
	       {
		  int                 x, y;

		  switch (Conf.desks.dragdir)
		    {
		    default:
		    case 0:
		       x = WinGetW(VROOT);
		       y = 0;
		       break;
		    case 1:
		       x = -WinGetW(VROOT);
		       y = 0;
		       break;
		    case 2:
		       x = 0;
		       y = WinGetH(VROOT);
		       break;
		    case 3:
		       x = 0;
		       y = -WinGetH(VROOT);
		       break;
		    }
		  DeskMove(dsk, x, y);
		  DeskEnter(dsk);
		  EobjsSlideBy(&eo, 1, -x, -y, Conf.desks.slidespeed);
	       }
	     else
	       {
		  EobjsSlideBy(&eo, 1, -EoGetX(dsk), -EoGetY(dsk),
			       Conf.desks.slidespeed);
		  DeskEnter(dsk);
	       }
	  }
	else
	  {
	     DeskEnter(dsk);
	  }
	DeskMove(dsk, 0, 0);
     }
   else
     {
	DeskEnter(dsk);
     }

   DeskSwitchDone();
   MoveResizeResume();

   ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskGoto %d done\n", dsk->num);
}

static void
UncoverDesktop(unsigned int desk)
{
   Desk               *dsk;

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
}

static void
DeskRaise(unsigned int desk)
{
   Desk               *dsk;

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskRaise(%d) current=%d\n", desk, desks.current->num);

   DeskSwitchStart();
   DeskEnter(dsk);
   DeskSwitchDone();

   ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);

   ESync(ESYNC_DESKS);
}

static void
DeskLower(unsigned int desk)
{
   Desk               *dsk;

   if ((desk <= 0) || (desk >= Conf.desks.num))
      return;

   dsk = _DeskGet(desk);

   DeskSwitchStart();
   MoveToDeskBottom(dsk);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskLower(%d) %d -> %d\n", desk, desks.current->num,
	      desks.order[0]);

   desks.previous = desks.current = DeskGet(desks.order[0]);

   EGrabServer();

   UncoverDesktop(desks.order[0]);
   DeskHide(desk);

   EwinsMoveStickyToDesk(desks.current);
   ButtonsMoveStickyToDesk(desks.current);
   DesksStackingCheck();
   DeskSwitchDone();
   if (Mode.mode == MODE_NONE)
      ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);

   EUngrabServer();
   ESync(ESYNC_DESKS);
}

#if 0				/* Unused */
void
DeskShow(int desk)
{
   Desk               *dsk;
   int                 i;

   if (desk < 0 || desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   MoveToDeskTop(desk);

   if (desk == 0)
     {
	for (i = Conf.desks.num - 1; i > 0; i--)
	   DeskHide(desks.order[i]);
     }
}
#endif

static void
DeskRestackSimple(Desk * dsk)
{
   EObj               *const *lst, *eo;
   int                 i, num;
   XWindowChanges      xwc;
   unsigned int        value_mask;

   eo = dsk->stack.latest;

   lst = EobjListStackGetForDesk(&num, dsk);
   if (num < 2)
      return;

   for (i = 0; i < num; i++)
      if (lst[i] == eo)
	 break;
   if (i >= num)
      return;

   eo->stacked = 1;

   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskRestackSimple %#lx %s\n", EobjGetXwin(eo), EobjGetName(eo));

   if (i < num - 1)
     {
	xwc.stack_mode = Above;
	xwc.sibling = EobjGetXwin(lst[i + 1]);
     }
   else
     {
	xwc.stack_mode = Below;
	xwc.sibling = EobjGetXwin(lst[i - 1]);
     }
   value_mask = CWSibling | CWStackMode;
   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskRestackSimple %#10lx %s %#10lx\n", EobjGetXwin(eo),
	      (xwc.stack_mode == Above) ? "Above" : "Below", xwc.sibling);
   XConfigureWindow(disp, EobjGetXwin(eo), value_mask, &xwc);
}

#define _APPEND_TO_WIN_LIST(win) \
  { \
     wl = EREALLOC(Window, wl, ++tot); \
     wl[tot - 1] = win; \
  }
void
DeskRestack(Desk * dsk)
{
   Window             *wl;
   int                 i, num, tot;
   EObj               *const *lst, *eo;

   if (!dsk->stack.dirty)
      return;

   /* Special case if only one window needs restacking */
   if (dsk->stack.dirty == 1)
     {
	DeskRestackSimple(dsk);
	goto done;
     }

   /* Build the window stack, top to bottom */
   tot = 0;
   wl = NULL;
   lst = EobjListStackGetForDesk(&num, dsk);

   /* Normal objects */
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	_APPEND_TO_WIN_LIST(EobjGetXwin(eo));
	eo->stacked = 1;
     }

   if (EDebug(EDBUG_TYPE_STACKING))
     {
	Eprintf("DeskRestack %d (%d):\n", dsk->num, dsk->stack.dirty);
	for (i = 0; i < tot; i++)
	   Eprintf(" win=%#10lx parent=%#10lx\n", wl[i],
		   EXWindowGetParent(wl[i]));
     }

   EXRestackWindows(wl, tot);

   Efree(wl);

 done:
   if (dsk->stack.update_client_list)
     {
	dsk->stack.update_client_list = 0;
	HintsSetClientStacking();
	ClickGrabsUpdate();
     }
   dsk->stack.dirty = 0;
   dsk->stack.latest = NULL;
}

void
DeskGotoByEwin(EWin * ewin)
{
   if (EoIsSticky(ewin) || EoIsFloating(ewin))
      return;

   DeskGoto(EoGetDesk(ewin));
   DeskCurrentGotoArea(ewin->area_x, ewin->area_y);
}

/*
 * Areas
 */

void
DesksFixArea(int *ax, int *ay)
{
   if (*ax < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = Conf.desks.areas_nx - 1;
	else
	   *ax = 0;
     }
   else if (*ax >= Conf.desks.areas_nx)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = 0;
	else
	   *ax = Conf.desks.areas_nx - 1;
     }

   if (*ay < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ay = Conf.desks.areas_ny - 1;
	else
	   *ay = 0;
     }
   else if (*ay >= Conf.desks.areas_ny)
     {
	if (Conf.desks.areas_wraparound)
	   *ay = 0;
	else
	   *ay = Conf.desks.areas_ny - 1;
     }
}

static int
AreaXYToLinear(int ax, int ay)
{
   DesksFixArea(&ax, &ay);
   return (ay * Conf.desks.areas_nx) + ax;
}

static void
AreaLinearToXY(int a, int *ax, int *ay)
{
   if (a < 0)
      a = 0;
   else if (a >= (Conf.desks.areas_nx * Conf.desks.areas_ny))
      a = (Conf.desks.areas_nx * Conf.desks.areas_ny) - 1;
   *ay = a / Conf.desks.areas_nx;
   *ax = a - (*ay * Conf.desks.areas_nx);
}

static void
SetAreaSize(int aw, int ah)
{
   if (aw < 1)
      aw = 1;
   if (ah < 1)
      ah = 1;
   Conf.desks.areas_nx = aw;
   Conf.desks.areas_ny = ah;
   HintsSetViewportConfig();
   EdgeWindowsShow();
   ModulesSignal(ESIGNAL_AREA_CONFIGURED, NULL);
}

void
DesksGetAreaSize(int *aw, int *ah)
{
   *aw = Conf.desks.areas_nx;
   *ah = Conf.desks.areas_ny;
}

static void
SetNewAreaSize(int ax, int ay)
{

   int                 a, b, i, num;
   EWin               *const *lst;

   if (ax <= 0)
      return;
   if (ay <= 0)
      return;

   DesksGetAreaSize(&a, &b);
   if ((a == ax) && (b == ay))
      return;

   SetAreaSize(ax, ay);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (!EoIsSticky(lst[i]))
	  {
	     if (lst[i]->area_x >= ax)
		EwinMoveToArea(lst[i], ax - 1, lst[i]->area_x);
	     if (lst[i]->area_y >= ay)
		EwinMoveToArea(lst[i], lst[i]->area_x, ay - 1);
	  }
     }

   DeskCurrentGetArea(&a, &b);
   if (a >= ax)
     {
	DeskCurrentGotoArea(ax - 1, b);
	DeskCurrentGetArea(&a, &b);
     }
   if (b >= ay)
      DeskCurrentGotoArea(a, ay - 1);

   autosave();
}

static void
SetCurrentLinearArea(int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   DeskCurrentGotoArea(ax, ay);
}

static int
GetCurrentLinearArea(void)
{
   int                 ax, ay;

   DeskCurrentGetArea(&ax, &ay);

   return AreaXYToLinear(ax, ay);
}

static void
MoveCurrentLinearAreaBy(int a)
{
   SetCurrentLinearArea(GetCurrentLinearArea() + a);
}

/* Return 1 to disable area switch */
static int
_DeskAreaSwitchCheckEwins(void)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListGetForDesk(&num, desks.current);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];

	/* Disable if there are non-sticky shading windows */
	if (EoIsSticky(ewin) || ewin->state.iconified)
	   continue;
	if (ewin->state.shading)
	   return 1;
     }

   return 0;
}

static void
DeskAreaSwitchStart(void)
{
   FocusNewDeskBegin();
}

static void
DeskAreaSwitchDone(void)
{
   HintsSetDesktopViewport();
   FocusNewDesk();
}

void
DeskCurrentGotoArea(int ax, int ay)
{
   EWin               *const *lst, *ewin;
   int                 i, num, dx, dy, pax, pay;

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
      return;

   if (_DeskAreaSwitchCheckEwins())
      return;

   DesksFixArea(&ax, &ay);
   DeskCurrentGetArea(&pax, &pay);

   if (ax == pax && ay == pay)
      return;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("%s %d,%d\n", __func__, ax, ay);

   ModulesSignal(ESIGNAL_AREA_SWITCH_START, NULL);

   dx = WinGetW(VROOT) * (ax - pax);
   dy = WinGetH(VROOT) * (ay - pay);

   if (dx < 0)
      SoundPlay(SOUND_MOVE_AREA_LEFT);
   else if (dx > 0)
      SoundPlay(SOUND_MOVE_AREA_RIGHT);
   else if (dy < 0)
      SoundPlay(SOUND_MOVE_AREA_UP);
   else if (dy > 0)
      SoundPlay(SOUND_MOVE_AREA_DOWN);

   MoveResizeSuspend();

   DeskAreaSwitchStart();

   /* set the current area up in out data structs */
   DeskCurrentSetArea(ax, ay);

   /* move all the windows around */
   lst = EwinListGetAll(&num);
   if (Conf.desks.slidein)
     {
	int                 wnum = 0;
	EObj              **wl = NULL;

	/* create the list of windwos to move */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (EoIsSticky(ewin) || ewin->state.iconified)
		continue;
	     if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
		continue;

	     if (EoIsFloating(ewin) && Conf.movres.mode_move == MR_OPAQUE)
		continue;

	     wnum++;
	     wl = EREALLOC(EObj *, wl, wnum);
	     wl[wnum - 1] = &ewin->o;
	  }

	/* slide them */
	if (wl)
	  {
	     EobjsSlideBy(wl, wnum, -dx, -dy, Conf.desks.slidespeed);
	     Efree(wl);
	     EobjsRepaint();
	  }
     }

   /* move all windows to their final positions */
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (EwinIsTransientChild(ewin))
	   continue;
	if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
	   continue;

	if (EoIsSticky(ewin) ||
	    (EoIsFloating(ewin) && Conf.movres.mode_move == MR_OPAQUE) ||
	    (!ewin->state.iconified && Conf.desks.slidein))
	   EwinMove(ewin, EoGetX(ewin), EoGetY(ewin), MRF_NOCHECK_ONSCREEN);
	else
	   EwinMove(ewin, EoGetX(ewin) - dx, EoGetY(ewin) - dy,
		    MRF_NOCHECK_ONSCREEN);
     }

   if (!Conf.desks.slidein)
      EobjsRepaint();

   MoveResizeResume();

   /* re-focus on a new ewin on that new desktop area */
   DeskAreaSwitchDone();

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("%s done\n", __func__);

   ModulesSignal(ESIGNAL_AREA_SWITCH_DONE, DesksGetCurrent());

   /* update which "edge flip resistance" detector windows are visible */
   EdgeWindowsShow();
}

void
DeskCurrentMoveAreaBy(int dx, int dy)
{
   int                 ax, ay;

   DeskCurrentGetArea(&ax, &ay);
   DeskCurrentGotoArea(ax + dx, ay + dy);
}

/*
 * Actions, events
 */
static char         sentpress = 0;

static void
ButtonProxySendEvent(XEvent * ev)
{
   if (Mode.button_proxy_win)
      EXSendEvent(Mode.button_proxy_win, SubstructureNotifyMask, ev);
}

static void
DeskDragStart(int desk)
{
   Desk               *dsk;

   dsk = DeskGet(desk);
   if (!dsk)
      return;

   desks.drag_x0 = Mode.events.cx - EoGetX(dsk);
   desks.drag_y0 = Mode.events.cy - EoGetY(dsk);

   Mode.mode = MODE_DESKDRAG;
}

static void
DeskDragEnd(Desk * dsk __UNUSED__)
{
   Mode.mode = MODE_NONE;
}

static void
DeskDragMotion(Desk * dsk)
{
   int                 x, y;

   x = Mode.events.mx - desks.drag_x0;
   y = Mode.events.my - desks.drag_y0;

   switch (Conf.desks.dragdir)
     {
     case 0:
	if (x < 0)
	   x = 0;
	y = 0;
	break;
     case 1:
	if (x > 0)
	   x = 0;
	y = 0;
	break;
     case 2:
	x = 0;
	if (y < 0)
	   y = 0;
	break;
     case 3:
	x = 0;
	if (y > 0)
	   y = 0;
	break;
     default:
	break;
     }
   DeskMove(dsk, x, y);
}

static void
DeskButtonCallback(EObj * eo, XEvent * ev, ActionClass * ac)
{
   Desk               *dsk;

   if (Mode.mode != MODE_DESKDRAG)
     {
	if (ac)
	   ActionclassEvent(ac, ev, NULL);
	return;
     }

   dsk = (Desk *) eo;
   switch (ev->type)
     {
     case ButtonRelease:
	DeskDragEnd(dsk);
	break;
     case MotionNotify:
	DeskDragMotion(dsk);
	break;
     }
}

static int
DeskCheckAction(Desk * dsk __UNUSED__, XEvent * ev)
{
   ActionClass        *ac;

   ac = ActionclassFind("DESKBINDINGS");
   if (!ac)
      return 0;

   return ActionclassEvent(ac, ev, NULL);
}

static void
DeskEventButtonPress(Desk * dsk, XEvent * ev)
{
   /* Don't handle desk bindings while doing stuff */
   if (Mode.mode)
      return;

   GrabPointerRelease();

   if (!DeskCheckAction(dsk, ev))
      ButtonProxySendEvent(ev);
}

static void
DeskEventButtonRelease(Desk * dsk, XEvent * ev)
{
   /* Don't handle desk bindings while doing stuff */
   if (Mode.mode)
      return;

   if (sentpress)
     {
	/* We never get here? */
	sentpress = 0;
	ButtonProxySendEvent(ev);
     }

   DeskCheckAction(dsk, ev);
}

static void
DeskRootResize(int root, int w, int h)
{
   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskRootResize %d %dx%d\n", root, w, h);

   if (root && (VROOT != RROOT))
     {
	WinGetW(RROOT) = w;
	WinGetH(RROOT) = h;
     }

   /* Quit if no change */
   if (w == WinGetW(VROOT) && h == WinGetH(VROOT))
      return;

   EWindowSync(VROOT);

   /* Quit if size is not final */
   if (w != WinGetW(VROOT) || h != WinGetH(VROOT))
      return;

   ScreenInit();
   DesksResize(w, h);

   HintsSetDesktopConfig();

   Mode.screen.w_old = WinGetW(VROOT);
   Mode.screen.h_old = WinGetH(VROOT);
}

static ActionClass *
DeskGetAclass(void *data __UNUSED__)
{
   return ActionclassFind("DESKBINDINGS");
}

static void
DeskPropertyChange(Desk * dsk, XEvent * ev)
{
   Pixmap              pmap;

   if (ev->xproperty.atom == E_XROOTPMAP_ID)
     {
	/* Possible race here? */
	pmap = HintsGetRootPixmap(EoGetWin(dsk));
	if (EDebug(EDBUG_TYPE_DESKS))
	   Eprintf("DeskPropertyChange win=%#lx _XROOTPMAP_ID=%#lx\n",
		   ev->xany.window, pmap);
	if (ev->xany.window != WinGetXwin(VROOT))
	   return;
	if (pmap == dsk->bg.pmap)
	   return;
	if (pmap == Mode.root.ext_pmap)
	   return;
	Mode.root.ext_pmap = pmap;
	Mode.root.ext_pmap_valid = EDrawableCheck(pmap, 0);
	DesksBackgroundRefresh(NULL, DESK_BG_REFRESH);
     }
   else if (ev->xproperty.atom == E_XROOTCOLOR_PIXEL)
     {
	if (EDebug(EDBUG_TYPE_DESKS))
	   Eprintf("DeskPropertyChange win=%#lx _XROOTCOLOR_PIXEL\n",
		   ev->xany.window);
	if (ev->xany.window != WinGetXwin(VROOT))
	   return;
     }
}

static void
DeskHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Desk               *dsk = (Desk *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	DeskEventButtonPress(dsk, ev);
	break;
     case ButtonRelease:
	DeskEventButtonRelease(dsk, ev);
	break;

     case EnterNotify:
	FocusHandleEnter(NULL, ev);
	break;
     case LeaveNotify:
	FocusHandleLeave(NULL, ev);
	break;

     case MotionNotify:
	/* Motion over desk buttons doesn't go here - We probably don't care much. */
	DesksSetCurrent(DesktopAt(Mode.events.mx, Mode.events.my));
	TooltipsSetPending(1, DeskGetAclass, dsk);
	break;

     case ConfigureNotify:
	if (ev->xconfigure.window != WinGetXwin(VROOT))
	   break;
	if (Mode.wm.window)	/* This test should not be necessary but... */
	  {
	     Mode.wm.win_x = ev->xconfigure.x;
	     Mode.wm.win_y = ev->xconfigure.y;
	  }
	DeskRootResize(0, ev->xconfigure.width, ev->xconfigure.height);
	break;

     case PropertyNotify:
	if (ev->xany.window == WinGetXwin(VROOT))
	   DeskPropertyChange(dsk, ev);
	break;

#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	{
	   XRRScreenChangeNotifyEvent *rrev = (XRRScreenChangeNotifyEvent *) ev;

	   XRRUpdateConfiguration(ev);
	   Mode.screen.rotation = rrev->rotation;
	   DeskRootResize(1, rrev->width, rrev->height);
	}
	break;
#endif
     }
}

/* Settings */

static void
DeskDragdirSet(const char *params)
{
   Desk               *dsk;
   unsigned int        i;
   int                 pd;

   pd = Conf.desks.dragdir;

   if (params && params[0])
      Conf.desks.dragdir = atoi(params);
   else
     {
	Conf.desks.dragdir++;
	if (Conf.desks.dragdir > 3)
	   Conf.desks.dragdir = 0;
     }

   if (pd == Conf.desks.dragdir)
      return;

   for (i = 1; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	EoMove(dsk, (dsk->viewable) ? 0 : WinGetW(VROOT), 0);
     }
   DesksControlsRefresh();
}

static void
DeskDragbarOrderSet(const char *params)
{
   int                 pd;

   pd = Conf.desks.dragbar_ordering;

   if (params && params[0])
      Conf.desks.dragbar_ordering = atoi(params);
   else
     {
	Conf.desks.dragbar_ordering++;
	if (Conf.desks.dragbar_ordering > 5)
	   Conf.desks.dragbar_ordering = 0;
     }

   if (pd == Conf.desks.dragbar_ordering)
      return;

   DesksControlsRefresh();
}

#if 0				/* FIXME */

static int
doDragbarWidthSet(EWin * edummy, const char *params)
{
   int                 pd;
   Button             *b;

   pd = Conf.desks.dragbar_width;
   if (params)
      Conf.desks.dragbar_width = atoi(params);

   if (pd != Conf.desks.dragbar_width)
     {
	DesksControlsRefresh();
     }
   return 0;
}

static int
doDragbarLengthSet(EWin * edummy, const char *params)
{
   int                 pd;
   Button             *b;

   pd = Conf.desks.dragbar_length;
   if (params)
      Conf.desks.dragbar_length = atoi(params);

   if (pd != Conf.desks.dragbar_length)
     {
	DesksControlsRefresh();
     }
   return 0;
}
#endif

#if ENABLE_DESKRAY
static int
doDeskray(EWin * edummy, const char *params)
{
   if (params)
     {
	if (!atoi(params))
	  {
	     DeskHideTabs();
	     Conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     Conf.deskmode = MODE_DESKRAY;
	     DeskShowTabs();
	  }
     }
   else
     {
	if (Conf.deskmode == MODE_DESKRAY)
	  {
	     DeskHideTabs();
	     Conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     Conf.deskmode = MODE_DESKRAY;
	     DeskShowTabs();
	  }
     }
   return 0;
}
#endif /* ENABLE_DESKRAY */

static void
DesksInit(void)
{
   unsigned int        i;

   memset(&desks, 0, sizeof(desks));

   Mode.screen.w_old = WinGetW(VROOT);
   Mode.screen.h_old = WinGetH(VROOT);

   Mode.backgrounds.mini_w = WinGetW(VROOT) / 12;
   Mode.backgrounds.mini_h = WinGetH(VROOT) / 12;

   /* Backward compatibility hack */
   if (Conf.desks.edge_flip_resistance <= 0)
      Conf.desks.edge_flip_mode = EDGE_FLIP_OFF;

   desks.previous = NULL;

   for (i = 0; i < Conf.desks.num; i++)
      DeskCreate(i, 0);

   SetAreaSize(Conf.desks.areas_nx, Conf.desks.areas_ny);

   /* Retreive stuff from last time we were loaded if we're restarting */
   EHintsGetDeskInfo();

   HintsSetDesktopConfig();
   HintsSetDesktopViewport();
}

static void
DesksConfigure(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskConfigure(_DeskGet(i));

   UncoverDesktop(0);
}

/*
 * Desktops Module
 */

static void
DesksSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	DesksInit();
	break;

     case ESIGNAL_CONFIGURE:
	DesksConfigure();
	break;

     case ESIGNAL_START:
	/* Draw all the buttons that belong on the desktop */
	DeskShowButtons();
	IdlerAdd(_DesksIdler, NULL);
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Dialogs
 */
typedef struct {
   int                 desktops;
   int                 prev_desktops;
   DItem              *desk_text;
   char                desktop_slide;
   int                 desktop_slide_speed;
   char                desktop_wraparound;
   char                dragbar;
   int                 dragdir;
   char                initialised;
   Win                 wins[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
} DeskDlgData;

static void
CB_ConfigureDesktops(Dialog * d, int val, void *data __UNUSED__)
{
   DeskDlgData        *dd = DLG_DATA_GET(d, DeskDlgData);

   if (val >= 2)
      return;

   ChangeNumberOfDesktops(dd->desktops);
   Conf.desks.slidein = dd->desktop_slide;
   Conf.desks.slidespeed = dd->desktop_slide_speed;
   Conf.desks.desks_wraparound = dd->desktop_wraparound;

   if ((Conf.desks.dragdir != dd->dragdir) ||
       ((dd->dragbar) && (Conf.desks.dragbar_width < 1)) ||
       ((!dd->dragbar) && (Conf.desks.dragbar_width > 0)))
     {
	if (dd->dragbar)
	   Conf.desks.dragbar_width = 16;
	else
	   Conf.desks.dragbar_width = 0;
	Conf.desks.dragdir = dd->dragdir;
	DesksControlsRefresh();
     }

   autosave();
}

static void
CB_DesktopDisplayRedraw(Dialog * d, int val, void *data)
{
   DeskDlgData        *dd = DLG_DATA_GET(d, DeskDlgData);
   DItem              *di;
   int                 i;
   int                 w, h;
   Win                 win;
   char                s[64];
   ImageClass         *ic;

   if ((val != 1) && (dd->prev_desktops == dd->desktops))
      return;

   dd->prev_desktops = dd->desktops;
   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   if (!dd->initialised)
     {
	ic = ImageclassFind("SETTINGS_DESKTOP_AREA", 1);
	ImageclassApply(ic, win, 0, 0, STATE_NORMAL, ST_SOLID);
	dd->initialised = 1;
     }

   for (i = 0; i < dd->desktops; i++)
     {
	if (!dd->wins[i])
	  {
	     Background         *bg;

	     dd->wins[i] =
		ECreateWindow(win, 0, 0, Mode.backgrounds.mini_w,
			      Mode.backgrounds.mini_h, 0);
	     ESetWindowBorderWidth(dd->wins[i], 1);

	     bg = DeskBackgroundGet(DeskGet(i));
	     if (bg)
	       {
		  Pixmap              pmap;

		  pmap = EGetWindowBackgroundPixmap(dd->wins[i]);
		  BackgroundApplyPmap(bg, dd->wins[i], pmap,
				      Mode.backgrounds.mini_w,
				      Mode.backgrounds.mini_h);
	       }
	     else
	       {
		  ic = ImageclassFind("SETTINGS_DESKTOP_AREA", 1);
		  ImageclassApply(ic, dd->wins[i], 0, 0, STATE_NORMAL,
				  ST_SOLID);
	       }
	  }
     }

   for (i = dd->desktops - 1; i >= 0; i--)
     {
	int                 num;

	num = dd->desktops - 1;
	if (num < 1)
	   num = 1;
	EMoveWindow(dd->wins[i], (i * (w - Mode.backgrounds.mini_w - 2)) / num,
		    (i * (h - Mode.backgrounds.mini_h - 2)) / num);
	ERaiseWindow(dd->wins[i]);
	EMapWindow(dd->wins[i]);
     }

   for (i = dd->desktops; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
     {
	if (!dd->wins[i])
	   continue;
	EUnmapWindow(dd->wins[i]);
     }

   Esnprintf(s, sizeof(s), "%i", dd->desktops);
   DialogItemSetText(dd->desk_text, s);
}

static void
CB_DesktopDisplayAreaRedraw(DItem * di, int val __UNUSED__,
			    void *data __UNUSED__)
{
   CB_DesktopDisplayRedraw(DialogItemGetDialog(di), 1, di);
}

static void
_DlgFillDesks(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *slider, *radio;
   DeskDlgData        *dd;

   dd = DLG_DATA_SET(d, DeskDlgData);
   if (!dd)
      return;

   dd->desktops = Conf.desks.num;
   dd->prev_desktops = -1;
   dd->desktop_slide = Conf.desks.slidein;
   dd->desktop_slide_speed = Conf.desks.slidespeed;
   dd->desktop_wraparound = Conf.desks.desks_wraparound;
   if (Conf.desks.dragbar_width < 1)
      dd->dragbar = 0;
   else
      dd->dragbar = 1;
   dd->dragdir = Conf.desks.dragdir;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Number of virtual desktops:"));

   di = dd->desk_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, "X");

   di = slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 1, 32);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetValPtr(di, &dd->desktops);

   di = DialogAddItem(table, DITEM_AREA);
   DialogItemSetColSpan(di, 2);
   DialogItemAreaSetSize(di, 2 * Mode.backgrounds.mini_w,
			 2 * Mode.backgrounds.mini_h);
   DialogItemAreaSetInitFunc(di, CB_DesktopDisplayAreaRedraw);

   DialogItemSetCallback(slider, CB_DesktopDisplayRedraw, 0, di);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide desktops around when changing"));
   DialogItemCheckButtonSetPtr(di, &dd->desktop_slide);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Desktop Slide speed:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &dd->desktop_slide_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Wrap desktops around"));
   DialogItemCheckButtonSetPtr(di, &dd->desktop_wraparound);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display desktop dragbar"));
   DialogItemCheckButtonSetPtr(di, &dd->dragbar);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Drag bar position:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Top"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Bottom"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->dragdir);
}

const DialogDef     DlgDesks = {
   "CONFIGURE_DESKTOPS",
   N_("Desks"),
   N_("Multiple Desktop Settings"),
   SOUND_SETTINGS_DESKTOPS,
   "pix/desktops.png",
   N_("Enlightenment Multiple Desktop\n" "Settings Dialog"),
   _DlgFillDesks,
   DLG_OAC, CB_ConfigureDesktops,
};

typedef struct {
   int                 area_x;
   int                 area_y;
   int                 edge_flip;
   int                 edge_resist;
   DItem              *area_text;
   char                area_wraparound;
   int                 prev_ax, prev_ay;
   Win                 awin;
} AreaDlgData;

static void
CB_ConfigureAreas(Dialog * d, int val, void *data __UNUSED__)
{
   AreaDlgData        *dd = DLG_DATA_GET(d, AreaDlgData);

   if (val >= 2)
      return;

   SetNewAreaSize(dd->area_x, dd->area_y);
   Conf.desks.areas_wraparound = dd->area_wraparound;
   Conf.desks.edge_flip_mode = dd->edge_flip;
   if (dd->edge_resist < 1)
      dd->edge_resist = 1;
   Conf.desks.edge_flip_resistance = dd->edge_resist;

   autosave();
}

static void
CB_AreaDisplayRedraw(Dialog * d, int val, void *data)
{
   AreaDlgData        *dd = DLG_DATA_GET(d, AreaDlgData);
   char                s[64];
   DItem              *di;
   Win                 win;
   int                 w, h, ww, hh;

   if ((val != 1) && (dd->prev_ax == dd->area_x) && (dd->prev_ay == dd->area_y))
      return;

   dd->prev_ax = dd->area_x;
   dd->prev_ay = dd->area_y;

   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   if (val == 1)
     {
	ImageClass         *ic;
	Pixmap              pmap;

	ic = ImageclassFind("SETTINGS_AREA_AREA", 1);
	ImageclassApply(ic, win, 0, 0, STATE_NORMAL, ST_SOLID);

	/* Note: awin is destroyed when the dialog is destroyed */
	dd->awin = ECreateWindow(win, 0, 0, 18, 14, 0);
	ic = ImageclassFind("SETTINGS_AREADESK_AREA", 1);
	pmap = EGetWindowBackgroundPixmap(dd->awin);
	ImageclassApplySimple(ic, dd->awin, pmap, STATE_NORMAL, 0, 0, 18, 14);
     }
   ww = 18 * dd->prev_ax;
   hh = 14 * dd->prev_ay;
   EMoveResizeWindow(dd->awin, (w - ww) / 2, (h - hh) / 2, ww, hh);
   EMapWindow(dd->awin);

   Esnprintf(s, sizeof(s), "%i x %i", dd->prev_ax, dd->prev_ay);
   DialogItemSetText(dd->area_text, s);
}

static void
CB_AreaDisplayAreaRedraw(DItem * di, int val __UNUSED__, void *data __UNUSED__)
{
   CB_AreaDisplayRedraw(DialogItemGetDialog(di), 1, di);
}

static void
_DlgFillAreas(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *slider, *slider2, *table2, *radio;
   AreaDlgData        *dd;

   dd = DLG_DATA_SET(d, AreaDlgData);
   if (!dd)
      return;

   dd->area_wraparound = Conf.desks.areas_wraparound;

   dd->edge_flip = Conf.desks.edge_flip_mode;
   dd->edge_resist = Conf.desks.edge_flip_resistance;

   DesksGetAreaSize(&dd->area_x, &dd->area_y);

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, _("Virtual Desktop size:"));

   di = dd->area_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, "X");

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 2, 0, 0, 0);

   DialogAddItem(table2, DITEM_NONE);

   di = slider = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetValPtr(di, &dd->area_x);

   di = slider2 = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetValPtr(di, &dd->area_y);

   di = DialogAddItem(table2, DITEM_AREA);
   DialogItemAreaSetSize(di, 160, 120);
   DialogItemAreaSetInitFunc(di, CB_AreaDisplayAreaRedraw);

   DialogItemSetCallback(slider, CB_AreaDisplayRedraw, 0, di);
   DialogItemSetCallback(slider2, CB_AreaDisplayRedraw, 0, di);

   DialogAddItem(table, DITEM_SEPARATOR);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Wrap virtual desktops around"));
   DialogItemCheckButtonSetPtr(di, &dd->area_wraparound);

   DialogAddItem(table, DITEM_SEPARATOR);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Edge Flip Mode:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Off"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_OFF);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("On"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_ON);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Only when moving window"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_MOVE);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->edge_flip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, _("Resistance at edge of screen:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 1, 100);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 10);
   DialogItemSliderSetValPtr(di, &dd->edge_resist);
}

const DialogDef     DlgAreas = {
   "CONFIGURE_AREA",
   N_("Areas"),
   N_("Virtual Desktop Settings"),
   SOUND_SETTINGS_AREA,
   "pix/areas.png",
   N_("Enlightenment Virtual Desktop\n" "Settings Dialog"),
   _DlgFillAreas,
   DLG_OAC, CB_ConfigureAreas,
};
#endif /* ENABLE_DIALOGS */

/*
 * IPC functions
 */

static void
DeskOpGoto(unsigned int desk)
{
   Desk               *dsk;
   Desk               *pd = DesksGetCurrent();

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   DeskGoto(dsk);

   if (DesksGetCurrent() != pd)
      SoundPlay(SOUND_DESKTOP_SHUT);
}

static void
DeskOpGotoRel(int drel)
{
   int                 desk;

   desk = (int)DesksGetCurrentNum() + drel;
   if (Conf.desks.desks_wraparound)
      desk = (desk + Conf.desks.num) % Conf.desks.num;

   DeskOpGoto((unsigned int)desk);
}

static void
DeskOpDrag(int desk)
{
   DeskDragStart(desk);
}

static void
DesksIpcDesk(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len, value;
   unsigned int        desk;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   desk = DesksGetCurrentNum();

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Current Desktop: %d/%d\n", desk, Conf.desks.num);
     }
   else if (!strncmp(cmd, "set", 3))
     {
	sscanf(prm, "%i", &desk);
	ChangeNumberOfDesktops(desk);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	Desk               *dsk;

	for (desk = 0; desk < Conf.desks.num; desk++)
	  {
	     dsk = _DeskGet(desk);
	     IpcPrintf
		("Desk %d: viewable=%d order=%d  x,y=%4d,%4d wxh=%4dx%4d  area x,y=%d,%d  pmap=%#lx\n",
		 desk, dsk->viewable, desks.order[desk],
		 EoGetX(dsk), EoGetY(dsk), EoGetW(dsk), EoGetH(dsk),
		 dsk->current_area_x, dsk->current_area_y, dsk->bg.pmap);
	  }
     }
   else if (!strncmp(cmd, "goto", 2))
     {
	sscanf(prm, "%i", &desk);
	DeskOpGoto(desk);
     }
   else if (!strncmp(cmd, "next", 2))
     {
	DeskOpGotoRel(1);
     }
   else if (!strncmp(cmd, "prev", 2))
     {
	DeskOpGotoRel(-1);
     }
   else if (!strncmp(cmd, "back", 2))
     {
	DeskOpGoto(desks.prev_num);
     }
   else if (!strncmp(cmd, "this", 2))
     {
	DeskOpGotoRel(0);
     }
   else if (!strncmp(cmd, "raise", 2))
     {
	sscanf(prm, "%i", &desk);
	SoundPlay(SOUND_DESKTOP_RAISE);
	DeskRaise(desk);
     }
   else if (!strncmp(cmd, "lower", 2))
     {
	sscanf(prm, "%i", &desk);
	SoundPlay(SOUND_DESKTOP_LOWER);
	DeskLower(desk);
     }
   else if (!strcmp(cmd, "drag"))
     {
	if (prm[0])
	   desk = atoi(prm);
	DeskOpDrag(desk);
     }
   else if (!strcmp(cmd, "clear"))
     {
	if (!strcmp(prm, "on"))
	   value = 1;
	else if (!strcmp(prm, "off"))
	   value = 0;
	else
	   value = !Mode.showing_desktop;
	EwinsShowDesktop(value);
     }
   else if (!strncmp(cmd, "arrange", 3))
     {
	ArrangeEwins(prm);
     }
}

static void
DesksIpcArea(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;
   int                 ax, ay, dx, dy;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   DeskCurrentGetArea(&ax, &ay);

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Current Area: %d %d\n", ax, ay);
     }
   else if (!strncmp(cmd, "set", 3))
     {
	sscanf(params, "%*s %i %i", &ax, &ay);
	SetNewAreaSize(ax, ay);
     }
   else if (!strncmp(cmd, "goto", 2))
     {
	sscanf(params, "%*s %i %i", &ax, &ay);
	DeskCurrentGotoArea(ax, ay);
     }
   else if (!strncmp(cmd, "move", 2))
     {
	dx = dy = 0;
	sscanf(params, "%*s %i %i", &dx, &dy);
	DeskCurrentMoveAreaBy(dx, dy);
     }
   else if (!strncmp(cmd, "lgoto", 2))
     {
	sscanf(params, "%*s %i", &ax);
	SetCurrentLinearArea(ax);
     }
   else if (!strncmp(cmd, "lmove", 2))
     {
	dx = 0;
	sscanf(params, "%*s %i", &dx);
	MoveCurrentLinearAreaBy(dx);
     }
}

static const IpcItem DesksIpcArray[] = {
   {
    DesksIpcDesk,
    "desk", NULL,
    "Desktop functions",
    "  desk ?               Desktop info\n"
    "  desk drag            Start deskdrag\n"
    "  desk set <nd>        Set number of desktops\n"
    "  desk goto <d>        Goto specified desktop\n"
    "  desk list            Show desk info\n"
    "  desk next            Goto next desktop\n"
    "  desk prev            Goto previous desktop\n"
    "  desk back            Goto previous active desktop\n"
    "  desk this            Goto this desktop\n"
    "  desk lower <d>       Lower desktop\n"
    "  desk raise <d>       Raise desktop\n"
    "  desk arrange         Arrange windows on desktop\"\n"
    "  desk clear [on/off]  \"Show Desktop\"\n"}
   ,
   {
    DesksIpcArea,
    "area", NULL,
    "Area functions",
    "  area ?               Area info\n"
    "  area set <nx> <ny>   Set area size\n"
    "  area goto <ax> <ay>  Goto specified area\n"
    "  area move <dx> <dy>  Move relative to current area\n"
    "  area lgoto <al>      Goto specified linear area\n"
    "  area lmove <dl>      Move relative to current linear area\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(DesksIpcArray)/sizeof(IpcItem))

static void
DesksCfgFuncCount(void *item __UNUSED__, const char *value)
{
   ChangeNumberOfDesktops(atoi(value));
}

static void
DesksCfgFuncDragdir(void *item __UNUSED__, const char *value)
{
   DeskDragdirSet(value);
}

static void
DesksCfgFuncDragdbarOrder(void *item __UNUSED__, const char *value)
{
   DeskDragbarOrderSet(value);
}

static void
AreasCfgFuncSizeX(void *item __UNUSED__, const char *value)
{
   int                 ax, ay;

   DesksGetAreaSize(&ax, &ay);
   SetNewAreaSize(atoi(value), ay);
}

static void
AreasCfgFuncSizeY(void *item __UNUSED__, const char *value)
{
   int                 ax, ay;

   DesksGetAreaSize(&ax, &ay);
   SetNewAreaSize(ax, atoi(value));
}

static const CfgItem DesksCfgItems[] = {
   CFG_FUNC_INT(Conf.desks, num, 2, DesksCfgFuncCount),
   CFG_FUNC_INT(Conf.desks, dragdir, 2, DesksCfgFuncDragdir),
   CFG_ITEM_INT(Conf.desks, dragbar_width, 16),
   CFG_ITEM_INT(Conf.desks, dragbar_length, 0),
   CFG_FUNC_INT(Conf.desks, dragbar_ordering, 1, DesksCfgFuncDragdbarOrder),
   CFG_ITEM_BOOL(Conf.desks, desks_wraparound, 0),
   CFG_ITEM_BOOL(Conf.desks, slidein, 1),
   CFG_ITEM_INT(Conf.desks, slidespeed, 6000),

   CFG_FUNC_INT(Conf.desks, areas_nx, 2, AreasCfgFuncSizeX),
   CFG_FUNC_INT(Conf.desks, areas_ny, 1, AreasCfgFuncSizeY),
   CFG_ITEM_BOOL(Conf.desks, areas_wraparound, 0),

   CFG_ITEM_INT(Conf.desks, edge_flip_mode, EDGE_FLIP_ON),
   CFG_ITEM_INT(Conf.desks, edge_flip_resistance, 25),
};
#define N_CFG_ITEMS (sizeof(DesksCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModDesktops;

const EModule       ModDesktops = {
   "desktops", "desk",
   DesksSighan,
   {N_IPC_FUNCS, DesksIpcArray},
   {N_CFG_ITEMS, DesksCfgItems}
};
