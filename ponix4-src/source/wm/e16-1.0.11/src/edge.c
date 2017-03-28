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
#include "desktops.h"
#include "eobj.h"
#include "ewins.h"
#include "menus.h"		/* FIXME - Should not be here */
#include "timers.h"
#include "xwin.h"

static EObj        *w1 = NULL, *w2 = NULL, *w3 = NULL, *w4 = NULL;
static Timer       *edge_timer = NULL;

static int
EdgeTimeout(void *data)
{
   int                 val;
   int                 ax, ay, aw, ah, dx, dy, dax, day;
   EWin               *ewin;

   if (MenusActive())
      goto done;
   if (Conf.desks.edge_flip_mode == EDGE_FLIP_OFF)
      goto done;

   /* Quit if pointer has left screen */
   if (!EQueryPointer(NULL, NULL, NULL, NULL, NULL))
      goto done;

   /* Quit if in fullscreen window */
   ewin = GetEwinPointerInClient();
   if (ewin && ewin->state.fullscreen)
      goto done;

   DeskCurrentGetArea(&ax, &ay);
   DesksGetAreaSize(&aw, &ah);
   val = PTR2INT(data);
   dx = 0;
   dy = 0;
   dax = 0;
   day = 0;
   switch (val)
     {
     case 0:
	if (ax == 0 && !Conf.desks.areas_wraparound)
	   goto done;
	dx = WinGetW(VROOT) - 2;
	dax = -1;
	break;
     case 1:
	if (ax == (aw - 1) && !Conf.desks.areas_wraparound)
	   goto done;
	dx = -(WinGetW(VROOT) - 2);
	dax = 1;
	break;
     case 2:
	if (ay == 0 && !Conf.desks.areas_wraparound)
	   goto done;
	dy = WinGetH(VROOT) - 2;
	day = -1;
	break;
     case 3:
	if (ay == (ah - 1) && !Conf.desks.areas_wraparound)
	   goto done;
	dy = -(WinGetH(VROOT) - 2);
	day = 1;
	break;
     default:
	break;
     }
   if (aw == 1)
      dx = 0;
   if (ah == 1)
      dy = 0;
   Mode.events.px = Mode.events.mx;
   Mode.events.py = Mode.events.my;
   Mode.events.mx = Mode.events.cx += dx;
   Mode.events.my = Mode.events.cy += dy;
   EWarpPointer(VROOT, Mode.events.mx, Mode.events.my);
   DeskCurrentMoveAreaBy(dax, day);
   Mode.events.px = Mode.events.mx;
   Mode.events.py = Mode.events.my;

 done:
   edge_timer = NULL;
   return 0;
}

static void
EdgeEvent(int dir)
{
   static int          lastdir = -1;

#if 0
   Eprintf("EdgeEvent %d -> %d\n", lastdir, dir);
#endif
   if (lastdir == dir || Conf.desks.edge_flip_mode == EDGE_FLIP_OFF)
      return;

   if (Conf.desks.edge_flip_mode == EDGE_FLIP_MOVE && Mode.mode != MODE_MOVE)
      return;

   TIMER_DEL(edge_timer);
   if (dir >= 0)
     {
	if (Conf.desks.edge_flip_resistance <= 0)
	   Conf.desks.edge_flip_resistance = 1;
	TIMER_ADD(edge_timer, 10 * Conf.desks.edge_flip_resistance,
		  EdgeTimeout, INT2PTR(dir));
     }
   lastdir = dir;
}

static void
EdgeHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   int                 dir;

   dir = PTR2INT(prm);

   switch (ev->type)
     {
     default:
	break;

     case EnterNotify:
	EdgeEvent(dir);
	break;

     case LeaveNotify:
	EdgeEvent(-1);
	break;
     }
}

void
EdgeCheckMotion(int x, int y)
{
   int                 dir;

   if (x == 0)
      dir = 0;
   else if (x == WinGetW(VROOT) - 1)
      dir = 1;
   else if (y == 0)
      dir = 2;
   else if (y == WinGetH(VROOT) - 1)
      dir = 3;
   else
      dir = -1;
   EdgeEvent(dir);
}

static void
EdgeWindowShow(int which, int on)
{
   EObj               *eo;
   int                 x, y, w, h;

   x = y = 0;
   w = h = 1;

   switch (which)
     {
     default:
     case 1:			/* Left */
	eo = w1;
	h = WinGetH(VROOT);
	break;
     case 2:			/* Right */
	eo = w2;
	x = WinGetW(VROOT) - 1;
	h = WinGetH(VROOT);
	break;
     case 3:			/* Top */
	eo = w3;
	w = WinGetW(VROOT);
	break;
     case 4:			/* Bottom */
	eo = w4;
	y = WinGetH(VROOT) - 1;
	w = WinGetW(VROOT);
	break;
     }

   if (on)
     {
	EobjMoveResize(eo, x, y, w, h);
	EobjMap(eo, 0);
     }
   else
     {
	EobjUnmap(eo);
     }
}

void
EdgeWindowsShow(void)
{
   int                 ax, ay, cx, cy;

   if (Conf.desks.edge_flip_mode == EDGE_FLIP_OFF)
     {
	EdgeWindowsHide();
	return;
     }

   if (!w1)
     {
	w1 = EobjWindowCreate(EOBJ_TYPE_EVENT,
			      0, 0, 1, WinGetH(VROOT), 0, "Edge-L");
	w2 = EobjWindowCreate(EOBJ_TYPE_EVENT,
			      WinGetW(VROOT) - 1, 0, 1, WinGetH(VROOT),
			      0, "Edge-R");
	w3 = EobjWindowCreate(EOBJ_TYPE_EVENT,
			      0, 0, WinGetW(VROOT), 1, 0, "Edge-T");
	w4 = EobjWindowCreate(EOBJ_TYPE_EVENT,
			      0, WinGetH(VROOT) - 1, WinGetW(VROOT), 1,
			      0, "Edge-B");
	ESelectInput(EobjGetWin(w1), EnterWindowMask | LeaveWindowMask);
	ESelectInput(EobjGetWin(w2), EnterWindowMask | LeaveWindowMask);
	ESelectInput(EobjGetWin(w3), EnterWindowMask | LeaveWindowMask);
	ESelectInput(EobjGetWin(w4), EnterWindowMask | LeaveWindowMask);
	EventCallbackRegister(EobjGetWin(w1), EdgeHandleEvents, (void *)0);
	EventCallbackRegister(EobjGetWin(w2), EdgeHandleEvents, (void *)1);
	EventCallbackRegister(EobjGetWin(w3), EdgeHandleEvents, (void *)2);
	EventCallbackRegister(EobjGetWin(w4), EdgeHandleEvents, (void *)3);
     }
   DeskCurrentGetArea(&cx, &cy);
   DesksGetAreaSize(&ax, &ay);

   EdgeWindowShow(1, cx != 0 || Conf.desks.areas_wraparound);
   EdgeWindowShow(2, cx != (ax - 1) || Conf.desks.areas_wraparound);
   EdgeWindowShow(3, cy != 0 || Conf.desks.areas_wraparound);
   EdgeWindowShow(4, cy != (ay - 1) || Conf.desks.areas_wraparound);
}

void
EdgeWindowsHide(void)
{
   if (!w1)
      return;

   EobjUnmap(w1);
   EobjUnmap(w2);
   EobjUnmap(w3);
   EobjUnmap(w4);
}
