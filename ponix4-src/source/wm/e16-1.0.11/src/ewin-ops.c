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
#include "borders.h"
#include "desktops.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "ewin-ops.h"
#include "focus.h"
#include "groups.h"
#include "hints.h"
#include "iclass.h"		/* FIXME - Should not be here */
#include "screen.h"
#include "snaps.h"
#include "timers.h"
#include "xwin.h"

static const WinOp  winops[] = {
   {"border", 2, 1, 0, EWIN_OP_BORDER},
   {"title", 2, 1, 1, EWIN_OP_TITLE},

   {"focusclick", 0, 1, 1, EWIN_OP_FOCUS_CLICK},	/* Place before "focus" */

   {"close", 2, 1, 0, EWIN_OP_CLOSE},
   {"kill", 0, 1, 0, EWIN_OP_KILL},
   {"iconify", 2, 1, 1, EWIN_OP_ICONIFY},
   {"alone", 0, 1, 0, EWIN_OP_ALONE},
   {"opacity", 2, 1, 1, EWIN_OP_OPACITY},
   {"focused_opacity", 0, 1, 1, EWIN_OP_FOCUSED_OPACITY},
   {"shadow", 0, 1, 1, EWIN_OP_SHADOW},	/* Place before "shade" */
   {"shade", 2, 1, 1, EWIN_OP_SHADE},
   {"stick", 2, 1, 1, EWIN_OP_STICK},
   {"focus", 2, 1, 0, EWIN_OP_FOCUS},

   {"desk", 2, 1, 1, EWIN_OP_DESK},
   {"area", 2, 1, 1, EWIN_OP_AREA},
   {"move", 2, 1, 1, EWIN_OP_MOVE},
   {"size", 2, 1, 1, EWIN_OP_SIZE},
   {"sz", 2, 1, 1, EWIN_OP_SIZE},
   {"move_relative", 0, 1, 0, EWIN_OP_MOVE_REL},
   {"mr", 2, 1, 0, EWIN_OP_MOVE_REL},
   {"resize_relative", 0, 1, 0, EWIN_OP_SIZE_REL},
   {"sr", 2, 1, 0, EWIN_OP_SIZE_REL},

   {"toggle_width", 0, 1, 0, EWIN_OP_MAX_WIDTH},
   {"tw", 2, 1, 0, EWIN_OP_MAX_WIDTH},
   {"toggle_height", 0, 1, 0, EWIN_OP_MAX_HEIGHT},
   {"th", 0, 1, 0, EWIN_OP_MAX_HEIGHT},
   {"toggle_size", 0, 1, 0, EWIN_OP_MAX_SIZE},
   {"ts", 2, 1, 0, EWIN_OP_MAX_SIZE},
   {"fullscreen", 2, 1, 1, EWIN_OP_FULLSCREEN},
   {"zoom", 2, 1, 0, EWIN_OP_ZOOM},

   {"layer", 2, 1, 1, EWIN_OP_LAYER},
   {"raise", 2, 1, 0, EWIN_OP_RAISE},
   {"lower", 2, 1, 0, EWIN_OP_LOWER},

   {"snap", 0, 1, 0, EWIN_OP_SNAP},

   {"ignore_arrange", 0, 1, 1, EWIN_OP_IGNORE_ARRANGE},
   {"never_use_area", 0, 1, 1, EWIN_OP_NEVER_USE_AREA},
   {"no_button_grabs", 0, 1, 1, EWIN_OP_NO_BUTTON_GRABS},
   {"skiplists", 4, 1, 1, EWIN_OP_SKIP_LISTS},
   {"autoshade", 0, 1, 1, EWIN_OP_AUTOSHADE},

   {"no_app_focus", 0, 1, 1, EWIN_OP_INH_APP_FOCUS},
   {"no_app_move", 0, 1, 1, EWIN_OP_INH_APP_MOVE},
   {"no_app_size", 0, 1, 1, EWIN_OP_INH_APP_SIZE},
   {"no_user_close", 0, 1, 1, EWIN_OP_INH_USER_CLOSE},
   {"no_user_move", 0, 1, 1, EWIN_OP_INH_USER_MOVE},
   {"no_user_size", 0, 1, 1, EWIN_OP_INH_USER_SIZE},
   {"no_wm_focus", 0, 1, 1, EWIN_OP_INH_WM_FOCUS},

   {"fade", 0, 1, 1, EWIN_OP_FADE},
   {"no_redir", 4, 1, 1, EWIN_OP_NO_REDIRECT},
   {"no_argb", 0, 1, 1, EWIN_OP_NO_ARGB},

   {NULL, 0, 0, 0, EWIN_OP_INVALID}	/* Terminator */
};

const WinOp        *
EwinOpFind(const char *op)
{
   const WinOp        *wop;

   wop = winops;
   for (; wop->name; wop++)
     {
	if (wop->len)
	  {
	     if (!strncmp(op, wop->name, wop->len))
		return wop;
	  }
	else
	  {
	     if (!strcmp(op, wop->name))
		return wop;
	  }
     }

   return NULL;
}

static void
EwinDetermineArea(EWin * ewin)
{
   Desk               *dsk;
   int                 ax, ay;

   dsk = EoGetDesk(ewin);
   ewin->vx = dsk->current_area_x * EoGetW(dsk) + EoGetX(ewin);
   ewin->vy = dsk->current_area_y * EoGetH(dsk) + EoGetY(ewin);

   if (EwinIsOnScreen(ewin))
     {
	ax = dsk->current_area_x;
	ay = dsk->current_area_y;
     }
   else
     {
	ax = (ewin->vx + EoGetW(ewin) / 2) / EoGetW(dsk);
	ay = (ewin->vy + EoGetH(ewin) / 2) / EoGetH(dsk);
	DesksFixArea(&ax, &ay);
     }

   if (ax != ewin->area_x || ay != ewin->area_y)
     {
	ewin->area_x = ax;
	ewin->area_y = ay;
	HintsSetWindowArea(ewin);
     }
}

#define MRF_DESK	(1<<0)
#define MRF_MOVE	(1<<1)
#define MRF_RESIZE	(1<<2)
#define MRF_RAISE	(1<<3)
#define MRF_FLOAT	(1<<4)
#define MRF_UNFLOAT	(1<<5)

static void
doEwinMoveResize(EWin * ewin, Desk * dsk, int x, int y, int w, int h, int flags)
{
   static int          call_depth = 0;
   int                 dx, dy, sw, sh, xo, yo;
   char                move, resize, reparent, raise, floating, configure;
   EWin              **lst;
   int                 i, num;
   Desk               *pdesk;

   if (ewin->state.zoomed)
      return;

   if (call_depth > 256)
      return;
   call_depth++;

   if (EDebug(EDBUG_TYPE_MOVERESIZE))
      Eprintf("doEwinMoveResize(%d,%d) %#lx f=%x d=%d %d+%d %d*%d %s\n",
	      call_depth, Mode.mode, EwinGetClientXwin(ewin), flags,
	      (dsk) ? (int)dsk->num : -1, x, y, w, h, EwinGetTitle(ewin));

   pdesk = (ewin->o.stacked >= 0) ? EoGetDesk(ewin) : NULL;
   reparent = move = resize = raise = 0;
   floating = EoIsFloating(ewin);

   if (flags & MRF_FLOAT)
     {
	if (floating == 0)
	  {
	     dsk = (!pdesk) ? EoGetDesk(ewin) : pdesk;
	     floating = 1;
	  }
	else if (floating == 1)
	  {
	     dsk = DeskGet(0);
	     floating = 2;
	  }
	flags |= MRF_RAISE;
     }
   else if (flags & MRF_UNFLOAT)
     {
	floating = 0;
	flags |= MRF_RAISE;
     }
   else if (EoIsSticky(ewin) && !floating)
     {
	dsk = DesksGetCurrent();
     }
   else if (!dsk)
     {
	dsk = EoGetDesk(ewin);
     }

   if (dsk != pdesk)
     {
	reparent = 1;
	flags |= MRF_DESK;
     }

   if (Mode.mode == MODE_NONE && !(flags & MRF_NOCHECK_ONSCREEN))
     {
	/* Don't throw windows offscreen */
	sw = WinGetW(VROOT);
	sh = WinGetH(VROOT);
	if (EoIsSticky(ewin))
	  {
	     xo = yo = 0;
	  }
	else
	  {
	     int                 ax, ay;

	     DeskGetArea(dsk, &ax, &ay);
	     xo = -ax * sw;
	     yo = -ay * sh;
	     sw *= Conf.desks.areas_nx;
	     sh *= Conf.desks.areas_ny;
	  }

	/* Keep at least 8 pixels (or window dimension) on-screen */
	dx = 8;
	if (dx > EoGetW(ewin))
	   dx = EoGetW(ewin);
	dy = 8;
	if (dy > EoGetH(ewin))
	   dy = EoGetH(ewin);

	if (x < xo - EoGetW(ewin) + dx)
	   x = xo - EoGetW(ewin) + dx;
	else if (x > xo + sw - dx)
	   x = xo + sw - dx;
	if (y < yo - EoGetH(ewin) + dy)
	   y = yo - EoGetH(ewin) + dy;
	else if (y > yo + sh - dy)
	   y = yo + sh - dy;
     }

   if (flags & MRF_RAISE)
      raise = 1;

   if (!(flags & MRF_MOVE))
     {
	x = EoGetX(ewin);
	y = EoGetY(ewin);
     }

   if (!(flags & MRF_RESIZE))
     {
	w = EoGetW(ewin);
	h = EoGetH(ewin);
     }
   else
     {
	if (w > 32000)
	   w = 32000;
	if (h > 32000)
	   h = 32000;
	if (ewin->ops && ewin->ops->Layout)
	  {
	     ewin->ops->Layout(ewin, &x, &y, &w, &h);
	  }
	else
	  {
	     ICCCM_SizeMatch(ewin, w, h, &w, &h);
	  }
	if (w <= 0)
	   w = 1;
	if (h <= 0)
	   h = 1;

	if ((w != ewin->client.w) || (h != ewin->client.h))
	   resize = 2;
	ewin->client.w = w;
	ewin->client.h = h;

	if (ewin->state.shaded)
	  {
	     w = EoGetW(ewin);
	     h = EoGetH(ewin);
	     if (resize)
		EwinBorderMinShadeSize(ewin, &w, &h);
	  }
	else
	  {
	     w = ewin->client.w + ewin->border->border.left +
		ewin->border->border.right;
	     h = ewin->client.h + ewin->border->border.top +
		ewin->border->border.bottom;
	  }
     }

   dx = x - EoGetX(ewin);
   dy = y - EoGetY(ewin);
   if ((dx != 0) || (dy != 0))
      move = 1;
   ewin->client.x = x + ewin->border->border.left;
   ewin->client.y = y + ewin->border->border.top;

#if 0
   Eprintf("repa=%d float=%d raise=%d move=%d resz=%d\n",
	   reparent, floating, raise, move, resize);
#endif
   if (EoIsShown(ewin) && (move || reparent))
      ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);

   if (reparent)
     {
	EoReparent(ewin, EoObj(dsk), x, y);
	if (flags & MRF_RESIZE)
	   EoResize(ewin, w, h);
     }
   else
     {
	EoMoveResize(ewin, x, y, w, h);
     }

   configure = 0;
   if (Mode.mode == MODE_NONE || resize || Conf.movres.update_while_moving)
     {
	configure = 1;
#if USE_XSYNC
	if (Conf.movres.enable_sync_request)
	   EwinSyncRequestSend(ewin);
#endif
     }

   if (flags & MRF_RESIZE)
     {
	if (!ewin->state.shaded)
	   EMoveResizeWindow(ewin->win_container,
			     ewin->border->border.left,
			     ewin->border->border.top,
			     ewin->client.w, ewin->client.h);
#if 0
	else
	   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
#endif

	EMoveResizeWindow(EwinGetClientWin(ewin), 0, 0, ewin->client.w,
			  ewin->client.h);
	EwinBorderCalcSizes(ewin, 0);
	if (resize && ewin->state.shaped)
	   ewin->update.shape = 1;
     }

   EwinPropagateShapes(ewin);

   /* Clear maximized state on move or resize */
   if ((move || resize) && !(flags & MRF_KEEP_MAXIMIZED))
     {
	if (ewin->state.maximized_horz || ewin->state.maximized_vert)
	  {
	     ewin->state.maximized_horz = 0;
	     ewin->state.maximized_vert = 0;
	     HintsSetWindowState(ewin);
	  }
     }

   if (raise)
     {
	EoSetFloating(ewin, floating);
	EwinRaise(ewin);
     }

   if (Mode.mode == MODE_NONE || Conf.movres.update_while_moving)
      ICCCM_Configure(ewin);

   if (configure)
     {
#if USE_XSYNC
	if (Conf.movres.enable_sync_request)
	   EwinSyncRequestWait(ewin);
#endif
     }

   if (flags & (MRF_DESK | MRF_MOVE | MRF_FLOAT | MRF_UNFLOAT))
     {
	lst = EwinListTransients(ewin, &num, 0);
	for (i = 0; i < num; i++)
	   doEwinMoveResize(lst[i], dsk, EoGetX(lst[i]) + dx,
			    EoGetY(lst[i]) + dy, 0, 0,
			    flags & (MRF_DESK | MRF_MOVE |
				     MRF_FLOAT | MRF_UNFLOAT |
				     MRF_NOCHECK_ONSCREEN |
				     MRF_KEEP_MAXIMIZED));
	Efree(lst);
     }

   EwinDetermineArea(ewin);
   if (Mode.op_source == OPSRC_USER)
      EwinSetPlacementGravity(ewin, x, y);

   if ((flags & (MRF_DESK | MRF_MOVE | MRF_RESIZE)) &&
       ewin->ops && ewin->ops->MoveResize)
      ewin->ops->MoveResize(ewin, resize);

   if (Mode.mode == MODE_NONE)
     {
	if (TransparencyUpdateNeeded())
	   EwinBorderDraw(ewin, resize, 1);	/* Update the border */

	SnapshotEwinUpdate(ewin, SNAP_USE_POS | SNAP_USE_SIZE);

	if (EoIsShown(ewin))
	   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
     }

   if (dsk != pdesk)
     {
	HintsSetWindowDesktop(ewin);
	SnapshotEwinUpdate(ewin, SNAP_USE_DESK);
     }

   call_depth--;
}

void
EwinMove(EWin * ewin, int x, int y, int flags)
{
   doEwinMoveResize(ewin, NULL, x, y, 0, 0,
		    MRF_MOVE | (flags & MRF_NOCHECK_ONSCREEN));
}

void
EwinResize(EWin * ewin, int w, int h, int flags)
{
   doEwinMoveResize(ewin, NULL, 0, 0, w, h,
		    MRF_RESIZE | (flags & MRF_KEEP_MAXIMIZED));
}

void
EwinMoveResize(EWin * ewin, int x, int y, int w, int h, int flags)
{
   doEwinMoveResize(ewin, NULL, x, y, w, h,
		    MRF_MOVE | MRF_RESIZE |
		    (flags & (MRF_NOCHECK_ONSCREEN | MRF_KEEP_MAXIMIZED)));
}

void
EwinMoveResizeWithGravity(EWin * ewin, int x, int y, int w, int h, int grav)
{
   EwinGetPosition(ewin, x, y, grav, &x, &y);
   doEwinMoveResize(ewin, NULL, x, y, w, h, MRF_MOVE | MRF_RESIZE);
}

void
EwinMoveToDesktop(EWin * ewin, Desk * dsk)
{
   doEwinMoveResize(ewin, dsk, 0, 0, 0, 0, MRF_DESK);
}

void
EwinMoveToDesktopAt(EWin * ewin, Desk * dsk, int x, int y)
{
   doEwinMoveResize(ewin, dsk, x, y, 0, 0, MRF_DESK | MRF_MOVE);
}

void
EwinOpMove(EWin * ewin, int source, int x, int y)
{
   Mode.op_source = source;
   EwinMove(ewin, x, y, 0);
   Mode.op_source = OPSRC_NA;
}

void
EwinOpResize(EWin * ewin, int source, int w, int h)
{
   Mode.op_source = source;
   EwinResize(ewin, w, h, 0);
   Mode.op_source = OPSRC_NA;
}

void
EwinOpMoveResize(EWin * ewin, int source, int x, int y, int w, int h)
{
   Mode.op_source = source;
   EwinMoveResize(ewin, x, y, w, h, 0);
   Mode.op_source = OPSRC_NA;
}

void
EwinOpMoveToDesktopAt(EWin * ewin, int source, Desk * dsk, int x, int y)
{
   Mode.op_source = source;
   EwinMoveToDesktopAt(ewin, dsk, x, y);
   Mode.op_source = OPSRC_NA;
}

void
EwinOpFloatAt(EWin * ewin, int source, int x, int y)
{
   Mode.op_source = source;
   doEwinMoveResize(ewin, EoGetDesk(ewin), x, y, 0, 0, MRF_MOVE | MRF_FLOAT);
   Mode.op_source = OPSRC_NA;
}

void
EwinOpUnfloatAt(EWin * ewin, int source, Desk * dsk, int x, int y)
{
   Mode.op_source = source;
   doEwinMoveResize(ewin, dsk, x, y, 0, 0, MRF_MOVE | MRF_UNFLOAT);
   Mode.op_source = OPSRC_NA;
}

void
EwinIconify(EWin * ewin)
{
   static int          call_depth = 0;
   EWin              **lst, *e;
   int                 i, num;

   if (!ewin)
      return;

   if (ewin->state.inhibit_iconify)
      return;

   if (ewin->state.state != EWIN_STATE_MAPPED)
      return;

   if (call_depth > 256)
      return;
   call_depth++;

   Zoom(ewin, 0);

   /* Save position at which the window was iconified */
   EwinRememberPositionSet(ewin);

   if (!EwinIsTransient(ewin))
      ModulesSignal(ESIGNAL_EWIN_ICONIFY, ewin);

   ewin->state.iconified = 1;
   EwinHide(ewin);

   ICCCM_Iconify(ewin);

   lst = EwinListTransients(ewin, &num, 0);
   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (e->state.iconified)
	   continue;

	EwinIconify(e);
     }
#if ENABLE_GNOME
   if (lst && call_depth == 1)
      GNOME_SetClientList();
#endif
   Efree(lst);

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);

   call_depth--;
}

void
EwinAlone(EWin * ewin)
{
   EWin               *const *lst, *item;
   int                 i, num;

   lst = EwinListGetForDesk(&num, EoGetDesk(ewin));

   for (i = 0; i < num; i++)
     {
	item = lst[i];

	if (item == ewin || EwinIsTransient(item) ||
	    item->state.iconified || item->state.donthide ||
	    item->area_x != ewin->area_x || item->area_y != ewin->area_y)
	   continue;
	EwinIconify(item);
     }
}

static void
GetOnScreenPos(int x, int y, int w, int h, int *px, int *py)
{
   int                 dx, dy;

   if (x + w > 4 && x <= WinGetW(VROOT) - 4 &&
       y + h > 4 && y <= WinGetH(VROOT) - 4)
      goto done;

   dx = w / 2;
   dy = h / 2;
   x = (x + dx) % WinGetW(VROOT);
   if (x < 0)
      x += WinGetW(VROOT);
   x -= dx;
   y = (y + dy) % WinGetH(VROOT);
   if (y < 0)
      y += WinGetH(VROOT);
   y -= dy;

 done:
   *px = x;
   *py = y;
}

static void
EwinDeIconify1(EWin * ewin, int dx, int dy)
{
   static int          call_depth = 0;
   EWin              **lst, *e;
   int                 i, num;
   int                 x, y;

   if (call_depth > 256)
      return;
   call_depth++;

   if (ewin->state.state != EWIN_STATE_ICONIC || !ewin->state.iconified)
      return;

   EwinRememberPositionGet(ewin, DesksGetCurrent(), &x, &y);

   EwinMoveToDesktopAt(ewin, DesksGetCurrent(), x + dx, y + dy);

   if (!EwinIsTransient(ewin))
      ModulesSignal(ESIGNAL_EWIN_DEICONIFY, ewin);

   ewin->state.iconified = 0;
   ewin->state.showingdesk = 0;

   EwinRaise(ewin);
   EwinShow(ewin);
   ICCCM_DeIconify(ewin);

   lst = EwinListTransients(ewin, &num, 0);
   for (i = 0; i < num; i++)
     {
	e = lst[i];
	if (!e->state.iconified)
	   continue;

	EwinDeIconify1(e, dx, dy);
     }
#if ENABLE_GNOME
   if (lst && call_depth == 1)
      GNOME_SetClientList();
#endif
   Efree(lst);

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);

   call_depth--;
}

void
EwinDeIconify(EWin * ewin)
{
   int                 x, y, ox, oy, dx, dy;

   EwinRememberPositionGet(ewin, DesksGetCurrent(), &x, &y);
   ox = x;
   oy = y;

   /* If we iconified an offscreen window, get it back on screen */
   if (!ewin->state.showingdesk)
      GetOnScreenPos(x, y, EoGetW(ewin), EoGetH(ewin), &x, &y);

   dx = x - ox;
   dy = y - oy;

   EwinDeIconify1(ewin, dx, dy);
}

static void
EwinUnStick(EWin * ewin)
{
   if (!EoIsSticky(ewin))
      return;

   SoundPlay(SOUND_WINDOW_UNSTICK);

   EoSetSticky(ewin, 0);
   EwinMoveToDesktopAt(ewin, DesksGetCurrent(), EoGetX(ewin), EoGetY(ewin));
   EwinBorderUpdateState(ewin);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   HintsSetWindowDesktop(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_STICKY);
}

static void
EwinStick(EWin * ewin)
{
   int                 x, y, dx, dy;

   if (EoIsSticky(ewin))
      return;

   SoundPlay(SOUND_WINDOW_STICK);

   /* Avoid "losing" windows made sticky while not in the current viewport */
   dx = EoGetW(ewin) / 2;
   dy = EoGetH(ewin) / 2;
   x = (EoGetX(ewin) + dx) % WinGetW(VROOT);
   if (x < 0)
      x += WinGetW(VROOT);
   x -= dx;
   y = (EoGetY(ewin) + dy) % WinGetH(VROOT);
   if (y < 0)
      y += WinGetH(VROOT);
   y -= dy;

   EoSetSticky(ewin, 1);
   EwinMoveToDesktopAt(ewin, DesksGetCurrent(), x, y);
   EwinBorderUpdateState(ewin);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   HintsSetWindowDesktop(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_STICKY);
}

#define SHADE_LEFT	0	/* __LEFT */
#define SHADE_RIGHT	1	/* __RIGHT */
#define SHADE_UP	2	/* __UP/__TOP */
#define SHADE_DOWN	3	/* __DOWN/__BOTTOM */

void
EwinInstantShade(EWin * ewin, int force)
{
   int                 x, y, w, h;
   int                 minw, minh;

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0)
       && (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      return;
   if (ewin->state.zoomed)
      return;
   if (ewin->state.shaded && !force)
      return;

   x = EoGetX(ewin);
   y = EoGetY(ewin);
   w = EoGetW(ewin);
   h = EoGetH(ewin);

   EwinBorderMinShadeSize(ewin, &minw, &minh);

   switch (ewin->border->shadedir)
     {
     default:
     case SHADE_LEFT:
	w = minw;
	break;
     case SHADE_RIGHT:
	if (!force)
	   x = x + w - minw;
	w = minw;
	break;
     case SHADE_UP:
	h = minh;
	break;
     case SHADE_DOWN:
	if (!force)
	   y = y + h - minh;
	h = minh;
	break;
     }

   ewin->state.shaded = 2;
   EoMoveResize(ewin, x, y, w, h);
   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
   EwinMoveResize(ewin, x, y, ewin->client.w, ewin->client.h,
		  MRF_KEEP_MAXIMIZED);
}

void
EwinInstantUnShade(EWin * ewin)
{
   XSetWindowAttributes att;
   int                 x, y, w, h;

   if (ewin->state.zoomed)
      return;
   if (!ewin->state.shaded)
      return;

   x = EoGetX(ewin);
   y = EoGetY(ewin);

   switch (ewin->border->shadedir)
     {
     default:
	break;
     case SHADE_RIGHT:
	w = ewin->client.w + ewin->border->border.left +
	   ewin->border->border.right;
	x = x + EoGetW(ewin) - w;
	break;
     case SHADE_DOWN:
	h = ewin->client.h + ewin->border->border.top +
	   ewin->border->border.bottom;
	y = y + EoGetH(ewin) - h;
	break;
     }

   /* Reset gravity */
   att.win_gravity = NorthWestGravity;
   EChangeWindowAttributes(EwinGetClientWin(ewin), CWWinGravity, &att);
   /* Force move after gravity change */
   EWindowSetGeometry(EwinGetClientWin(ewin), -1, -1,
		      ewin->client.w, ewin->client.h, 0);

   ewin->state.shaded = 0;
   EwinMoveResize(ewin, x, y, ewin->client.w, ewin->client.h,
		  MRF_KEEP_MAXIMIZED);
}

#define _EWIN_ADJUST_SHAPE(ewin, xo, yo) \
  do { \
    EShapeSetShape(ewin->win_container, xo, yo, EwinGetClientWin(ewin)); \
    ewin->update.shape = 1; \
  } while (0)

typedef struct {
   int                 x, y, w, h;
} _xywh;
typedef struct {
   EWin               *ewin;
   int                 k, dk;
   _xywh               start;
   _xywh               final;
   int                 a, b, c;
} _ewin_shade_data;

static void
_EwinShadeStart(_ewin_shade_data * esd)
{
   EWin               *ewin = esd->ewin;
   int                 minw, minh;
   XSetWindowAttributes att;

   esd->k = 0;
   esd->dk = 1024 * Conf.shading.speed * Conf.animation.step / 1000000;

   esd->start.x = EoGetX(ewin);
   esd->start.y = EoGetY(ewin);
   esd->start.w = EoGetW(ewin);
   esd->start.h = EoGetH(ewin);
   esd->final = esd->start;

   ewin->state.shading = 1;

   EwinBorderMinShadeSize(ewin, &minw, &minh);

   switch (ewin->border->shadedir)
     {
     default:
     case SHADE_LEFT:
	att.win_gravity = EastGravity;
	esd->a = esd->start.w;
	esd->b = minw;
	esd->final.w = esd->b;
	break;
     case SHADE_RIGHT:
	att.win_gravity = WestGravity;
	esd->a = esd->start.w;
	esd->b = minw;
	esd->c = esd->start.x + esd->start.w;
	esd->final.x = esd->c - esd->b;
	esd->final.w = esd->b;
	break;
     case SHADE_UP:
	att.win_gravity = SouthGravity;
	esd->a = esd->start.h;
	esd->b = minh;
	esd->final.h = esd->b;
	break;
     case SHADE_DOWN:
	att.win_gravity = SouthGravity;
	esd->a = esd->start.h;
	esd->b = minh;
	esd->c = esd->start.y + esd->start.h;
	esd->final.y = esd->c - esd->b;
	esd->final.h = esd->b;
	break;
     }

   EChangeWindowAttributes(EwinGetClientWin(ewin), CWWinGravity, &att);
}

static void
_EwinShadeEnd(_ewin_shade_data * esd)
{
   EWin               *ewin = esd->ewin;

   EoMoveResize(ewin, esd->final.x, esd->final.y, esd->final.w, esd->final.h);
   ewin->state.shaded = 2;
   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
   if (ewin->state.shaped)
      _EWIN_ADJUST_SHAPE(ewin, 0, 0);

   EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
		  ewin->client.w, ewin->client.h, MRF_KEEP_MAXIMIZED);

   ewin->state.shading = 0;

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_SHADED);

   Efree(esd);
}

static int
_EwinShadeRun(void *data)
{
   _ewin_shade_data   *esd = (_ewin_shade_data *) data;
   EWin               *ewin = esd->ewin;
   int                 k, x, y, w, h, ww, hh, cow, coh, shx, shy;

   if (!EwinFindByPtr(ewin))	/* Check, window may be gone */
     {
	Efree(esd);
	return 0;
     }

   k = esd->k;

   x = esd->start.x;
   y = esd->start.y;
   w = esd->start.w;
   h = esd->start.h;

   cow = ewin->client.w;
   coh = ewin->client.h;

   shx = shy = 0;

   switch (ewin->border->shadedir)
     {
     default:
     case SHADE_LEFT:
	w = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	if (w < 1)
	   w = 1;
	ww = w - ewin->border->border.left - ewin->border->border.right;
	if (ww < 1)
	   ww = 1;
	cow = ww;
	shx = ww - ewin->client.w;
	break;
     case SHADE_RIGHT:
	w = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	if (w < 1)
	   w = 1;
	x = esd->c - w;
	ww = w - ewin->border->border.left - ewin->border->border.right;
	if (ww < 1)
	   ww = 1;
	cow = ww;
	break;
     case SHADE_UP:
	h = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	if (h < 1)
	   h = 1;
	hh = h - ewin->border->border.top - ewin->border->border.bottom;
	if (hh < 1)
	   hh = 1;
	coh = hh;
	shy = hh - ewin->client.h;
	break;
     case SHADE_DOWN:
	h = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	if (h < 1)
	   h = 1;
	y = esd->c - h;
	hh = h - ewin->border->border.top - ewin->border->border.bottom;
	if (hh < 1)
	   hh = 1;
	coh = hh;
	shy = hh - ewin->client.h;
	break;
     }
   EMoveResizeWindow(ewin->win_container,
		     ewin->border->border.left,
		     ewin->border->border.top, cow, coh);
   if (ewin->state.shaped)
      _EWIN_ADJUST_SHAPE(ewin, shx, shy);
   EoMoveResize(ewin, x, y, w, h);
   EwinBorderCalcSizes(ewin, 1);

   esd->k = k += esd->dk;
   if (k < 1024)
      return 1;

   _EwinShadeEnd(esd);
   return 0;
}

void
EwinShade(EWin * ewin)
{
   _ewin_shade_data   *esd;

   if ((ewin->border->border.left == 0) && (ewin->border->border.right == 0) &&
       (ewin->border->border.top == 0) && (ewin->border->border.bottom == 0))
      return;
   if (ewin->state.zoomed)
      return;
   if (ewin->state.shaded || ewin->state.shading || ewin->state.iconified)
      return;
   if ((ewin->border) && (!strcmp(ewin->border->name, "BORDERLESS")))
      return;

   SoundPlay(SOUND_SHADE);

   esd = EMALLOC(_ewin_shade_data, 1);
   esd->ewin = ewin;
   _EwinShadeStart(esd);
   if ((Conf.shading.animate) || (ewin->type == EWIN_TYPE_MENU))
      AnimatorAdd(_EwinShadeRun, esd);
   else
      _EwinShadeEnd(esd);
}

static void
_EwinUnshadeStart(_ewin_shade_data * esd)
{
   EWin               *ewin = esd->ewin;
   int                 cow, coh, clx, cly;
   XSetWindowAttributes att;

   esd->k = 0;
   esd->dk = 1024 * Conf.shading.speed * Conf.animation.step / 1000000;

   esd->start.x = EoGetX(ewin);
   esd->start.y = EoGetY(ewin);
   esd->start.w = EoGetW(ewin);
   esd->start.h = EoGetH(ewin);
   esd->final = esd->start;

   ewin->state.shading = 1;
   ewin->state.shaded = 0;

   cow = ewin->client.w;
   coh = ewin->client.h;

   clx = cly = 0;

   switch (ewin->border->shadedir)
     {
     default:
     case SHADE_LEFT:
	att.win_gravity = EastGravity;
	esd->a = ewin->border->border.left + ewin->border->border.right;
	esd->b = ewin->client.w + esd->a;
	esd->c = 0;		/* Not used */
	esd->final.w = esd->b;
	cow = 1;
	clx = -ewin->client.w;
	break;
     case SHADE_RIGHT:
	att.win_gravity = WestGravity;
	esd->a = ewin->border->border.left + ewin->border->border.right;
	esd->b = ewin->client.w + esd->a;
	esd->c = esd->start.x + esd->start.w;	/* NB! w != a is possible */
	esd->final.x = esd->c - esd->b;
	esd->final.w = esd->b;
	cow = 1;
	break;
     case SHADE_UP:
	att.win_gravity = SouthGravity;
	esd->a = ewin->border->border.top + ewin->border->border.bottom;
	esd->b = ewin->client.h + esd->a;
	esd->c = 0;		/* Not used */
	esd->final.h = esd->b;
	coh = 1;
	cly = 1 - ewin->client.h;
	break;
     case SHADE_DOWN:
	att.win_gravity = SouthGravity;
	esd->a = ewin->border->border.top + ewin->border->border.bottom;
	esd->b = ewin->client.h + esd->a;
	esd->c = esd->start.y + esd->start.h;	/* NB! h != a is possible */
	esd->final.y = esd->c - esd->b;
	esd->final.h = esd->b;
	coh = 1;
	cly = 1 - ewin->client.h;
	break;
     }

   EChangeWindowAttributes(EwinGetClientWin(ewin), CWWinGravity, &att);
   EMoveResizeWindow(ewin->win_container,
		     ewin->border->border.left, ewin->border->border.top,
		     cow, coh);
   EWindowSync(EwinGetClientWin(ewin));	/* Gravity - recache */
   EMoveResizeWindow(EwinGetClientWin(ewin), clx, cly,
		     ewin->client.w, ewin->client.h);
}

static void
_EwinUnshadeEnd(_ewin_shade_data * esd)
{
   EWin               *ewin = esd->ewin;
   XSetWindowAttributes att;

   EoMoveResize(ewin, esd->final.x, esd->final.y, esd->final.w, esd->final.h);

   /* Reset gravity */
   att.win_gravity = NorthWestGravity;
   EChangeWindowAttributes(EwinGetClientWin(ewin), CWWinGravity, &att);

   EMoveResizeWindow(EwinGetClientWin(ewin), 0, 0, ewin->client.w,
		     ewin->client.h);
   EMoveResizeWindow(ewin->win_container, ewin->border->border.left,
		     ewin->border->border.top, ewin->client.w, ewin->client.h);

   if (ewin->state.shaped)
      _EWIN_ADJUST_SHAPE(ewin, 0, 0);

   EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
		  ewin->client.w, ewin->client.h, MRF_KEEP_MAXIMIZED);

   ewin->state.shading = 0;

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_SHADED);

   Efree(esd);
}

static int
_EwinUnshadeRun(void *data)
{
   _ewin_shade_data   *esd = (_ewin_shade_data *) data;
   EWin               *ewin = esd->ewin;
   int                 k, x, y, w, h, ww, hh, cow, coh, shx, shy;

   if (!EwinFindByPtr(ewin))	/* Check, window may be gone */
     {
	Efree(esd);
	return 0;
     }

   k = esd->k;

   x = esd->start.x;
   y = esd->start.y;
   w = esd->start.w;
   h = esd->start.h;

   cow = ewin->client.w;
   coh = ewin->client.h;

   shx = shy = 0;

   switch (ewin->border->shadedir)
     {
     default:
     case SHADE_LEFT:
	w = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	ww = w - esd->a;
	if (ww <= 0)
	   ww = 1;
	cow = ww;
	shx = ww - ewin->client.w;
	break;
     case SHADE_RIGHT:
	w = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	x = esd->c - w;
	ww = w - esd->a;
	if (ww <= 0)
	   ww = 1;
	cow = ww;
	break;
     case SHADE_UP:
	h = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	hh = h - esd->a;
	if (hh <= 0)
	   hh = 1;
	coh = hh;
	shy = hh - ewin->client.h;
	break;
     case SHADE_DOWN:
	h = ((esd->a * (1024 - k)) + (esd->b * k)) >> 10;
	y = esd->c - h;
	hh = h - esd->a;
	if (hh <= 0)
	   hh = 1;
	coh = hh;
	shy = hh - ewin->client.h;
	break;
     }
   EMoveResizeWindow(ewin->win_container,
		     ewin->border->border.left,
		     ewin->border->border.top, cow, coh);
   if (ewin->state.shaped)
      _EWIN_ADJUST_SHAPE(ewin, shx, shy);
   EoMoveResize(ewin, x, y, w, h);
   EwinBorderCalcSizes(ewin, 1);

   esd->k = k += esd->dk;
   if (k < 1024)
      return 1;

   _EwinUnshadeEnd(esd);
   return 0;
}

void
EwinUnShade(EWin * ewin)
{
   _ewin_shade_data   *esd;

   if (ewin->state.zoomed)
      return;
   TIMER_DEL(ewin->timer);
   if (!ewin->state.shaded || ewin->state.shading || ewin->state.iconified)
      return;

   SoundPlay(SOUND_UNSHADE);

   esd = EMALLOC(_ewin_shade_data, 1);
   esd->ewin = ewin;
   _EwinUnshadeStart(esd);
   if ((Conf.shading.animate) || (ewin->type == EWIN_TYPE_MENU))
      AnimatorAdd(_EwinUnshadeRun, esd);
   else
      _EwinUnshadeEnd(esd);
}

void
EwinOpFullscreen(EWin * ewin, int source __UNUSED__, int on)
{
   int                 x, y, w, h, ww, hh;
   EWin              **lst;
   int                 i, num;
   const Border       *b;

   if (ewin->state.fullscreen == (unsigned)on)
      return;
   if (ewin->state.zoomed)
      return;

   if (on)
     {
	if (on == 1)
	  {
	     if (ewin->state.inhibit_fullscreeen)
		return;
	     ewin->save_fs.x = EoGetX(ewin);
	     ewin->save_fs.y = EoGetY(ewin);
	     ewin->save_fs.w = ewin->client.w;
	     ewin->save_fs.h = ewin->client.h;
	     ewin->save_fs.layer = EoGetLayer(ewin);
	  }
	if (ewin->state.placed)
	  {
	     x = EoGetX(ewin);
	     y = EoGetY(ewin);
	  }
	else
	  {
	     EventsUpdateXY(&x, &y);
	  }
	ScreenGetAvailableArea(x, y, &x, &y, &w, &h,
			       Conf.place.ignore_struts_fullscreen);

	ewin->state.fullscreen = 1;

	/* Fixup if available space doesn't match ICCCM size constraints */
	ICCCM_SizeMatch(ewin, w, h, &ww, &hh);
	if (w == ww && h == hh)
	  {
	     b = BorderFind("BORDERLESS");
	  }
	else
	  {
	     b = BorderCreateFiller(ww, hh, w, h);
	     w = ww;
	     h = hh;
	  }
	EwinBorderSetTo(ewin, b);

	if (Conf.place.raise_fullscreen)
	  {
	     EoSetLayer(ewin, 8);
	     lst = EwinListTransients(ewin, &num, 0);
	     for (i = 0; i < num; i++)
	       {
		  lst[i]->save_fs.layer = EoGetLayer(lst[i]);
		  EoSetLayer(lst[i], lst[i]->save_fs.layer +
			     EoGetLayer(ewin) - ewin->save_fs.layer);
	       }
	     Efree(lst);
	  }

	EwinRaise(ewin);
	EwinMoveResize(ewin, x, y, w, h, MRF_KEEP_MAXIMIZED);
	EwinStateUpdate(ewin);
     }
   else
     {
	x = ewin->save_fs.x;
	y = ewin->save_fs.y;
	w = ewin->save_fs.w;
	h = ewin->save_fs.h;
	GetOnScreenPos(x, y, w, h, &x, &y);
	b = ewin->normal_border;
	EwinBorderSetTo(ewin, b);

	if (Conf.place.raise_fullscreen)
	  {
	     lst = EwinListTransients(ewin, &num, 0);
	     for (i = 0; i < num; i++)
		EoSetLayer(lst[i], lst[i]->save_fs.layer);
	     Efree(lst);
	  }
	EoSetLayer(ewin, ewin->save_fs.layer);

	ewin->state.fullscreen = 0;
	EwinStateUpdate(ewin);
	EwinRaise(ewin);
	EwinMoveResize(ewin, x, y, w, h, MRF_KEEP_MAXIMIZED);
     }

   HintsSetWindowState(ewin);
}

void
EwinsShowDesktop(int on)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListGetForDesk(&num, DesksGetCurrent());

   if (on)
     {
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];

	     if (EwinIsTransient(ewin) ||
		 ewin->state.iconified || ewin->state.donthide)
		continue;

	     ewin->state.showingdesk = 1;
	     EwinIconify(ewin);
	  }
     }
   else
     {
	for (i = num - 1; i >= 0; i--)
	  {
	     ewin = lst[i];

	     if (!ewin->state.showingdesk)
		continue;

	     EwinDeIconify(ewin);
	  }
     }
   Mode.showing_desktop = on;
   EWMH_SetShowingDesktop(on);
}

void
EwinMoveToArea(EWin * ewin, int ax, int ay)
{
   DesksFixArea(&ax, &ay);
   EwinMove(ewin, EoGetX(ewin) + (WinGetW(VROOT) * (ax - ewin->area_x)),
	    EoGetY(ewin) + (WinGetH(VROOT) * (ay - ewin->area_y)), 0);
}

void
EwinOpActivate(EWin * ewin, int source, int raise)
{
   int                 unshade;

   if (source == OPSRC_APP && EwinInhGetApp(ewin, focus))
      return;

   unshade = ewin->state.shaded /* && !ewin->state.iconified */ ;

   if (!ewin->state.animated && !ewin->state.iconified)
     {
	/* If somehow lost outside desktop, move it to center */
	if (!EwinIsOnDesktop(ewin))
	   ArrangeEwinCentered(ewin);
	DeskGotoByEwin(ewin);
     }
   if (raise)
      EwinOpRaise(ewin, source);
   if (ewin->state.iconified)
      EwinOpIconify(ewin, source, 0);
   if (unshade)
      EwinOpShade(ewin, source, 0);
   FocusToEWin(ewin, FOCUS_SET);
}

void
EwinOpClose(EWin * ewin, int source __UNUSED__)
{
   EWin              **gwins;
   int                 num, i;

   if (!ewin)
      return;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_KILL,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	ICCCM_Delete(gwins[i]);
	SoundPlay(SOUND_WINDOW_CLOSE);
     }
   Efree(gwins);
}

void
EwinOpKill(EWin * ewin, int source __UNUSED__)
{
   SoundPlay(SOUND_WINDOW_CLOSE);
   EwinKill(ewin);
}

void
EwinOpRaise(EWin * ewin, int source __UNUSED__)
{
   EWin              **gwins = NULL;
   int                 i, num;

   SoundPlay(SOUND_RAISE);
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_STACKING,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
      EwinRaise(gwins[i]);
   Efree(gwins);
}

void
EwinOpLower(EWin * ewin, int source __UNUSED__)
{
   EWin              **gwins = NULL;
   int                 i, num;

   SoundPlay(SOUND_LOWER);
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_STACKING,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
      EwinLower(gwins[i]);
   Efree(gwins);
}

void
EwinOpStick(EWin * ewin, int source __UNUSED__, int on)
{
   EWin              **gwins = NULL;
   int                 i, num;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_STICK,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (on)
	   EwinStick(gwins[i]);
	else
	   EwinUnStick(gwins[i]);
     }
   Efree(gwins);
}

void
EwinOpSkipLists(EWin * ewin, int source __UNUSED__, int skip)
{
   ewin->props.skip_ext_task = skip;
   ewin->props.skip_winlist = skip;
   ewin->props.skip_focuslist = skip;

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

#if 0				/* Unused */
void
EwinOpSkipTask(EWin * ewin, int skip)
{
   ewin->props.skip_ext_task = skip;

   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpSkipFocus(EWin * ewin, int skip)
{
   ewin->props.skip_focuslist = skip;
   EwinStateUpdate(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpSkipWinlist(EWin * ewin, int skip)
{
   ewin->props.skip_winlist = skip;
   EwinStateUpdate(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_SKIP_LISTS);
}

void
EwinOpNeverFocus(EWin * ewin, int on)
{
   ewin->props.never_focus = on;
   EwinStateUpdate(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_FOCUS_NEVER);
}
#endif

void
EwinOpIconify(EWin * ewin, int source __UNUSED__, int on)
{
   EWin              **gwins = NULL;
   int                 i, num;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ICONIFY,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (on)
	   EwinIconify(gwins[i]);
	else
	   EwinDeIconify(gwins[i]);
     }
   Efree(gwins);
}

void
EwinOpShade(EWin * ewin, int source __UNUSED__, int on)
{
   EWin              **gwins = NULL;
   int                 i, num;

   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_SHADE,
				      Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (on)
	   EwinShade(gwins[i]);
	else
	   EwinUnShade(gwins[i]);
     }
   Efree(gwins);
}

void
EwinOpSetLayer(EWin * ewin, int source __UNUSED__, int layer)
{
   if (EoGetLayer(ewin) > layer)
     {
	SoundPlay(SOUND_WINDOW_CHANGE_LAYER_DOWN);
     }
   else if (EoGetLayer(ewin) < layer)
     {
	SoundPlay(SOUND_WINDOW_CHANGE_LAYER_UP);
     }
   EoSetLayer(ewin, layer);
   EwinRaise(ewin);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_LAYER);
}

void
EwinOpSetBorder(EWin * ewin, int source __UNUSED__, const char *name)
{
   EWin              **gwins = NULL;
   int                 i, num;
   char                has_shaded;
   Border             *b;
   char                shadechange = 0;

   b = BorderFind(name);
   if (!b)
      return;

   has_shaded = 0;
   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_SET_WINDOW_BORDER,
				 Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (gwins[i]->state.shaded)
	  {
	     has_shaded = 1;
	     break;
	  }
     }
   if (has_shaded)
     {
	if ((b->border.left == 0) && (b->border.right == 0)
	    && (b->border.top == 0) && (b->border.bottom == 0))
	   return;
     }

   for (i = 0; i < num; i++)
     {
	if (b != gwins[i]->border)
	  {
	     SoundPlay(SOUND_WINDOW_BORDER_CHANGE);
	     shadechange = 0;
	     if (gwins[i]->state.shaded)
	       {
		  shadechange = 1;
		  EwinInstantUnShade(gwins[i]);
	       }
	     EwinBorderChange(gwins[i], b, 1);
	     if (shadechange)
		EwinInstantShade(gwins[i], 0);
	  }
	SnapshotEwinUpdate(gwins[i], SNAP_USE_BORDER);
     }
   Efree(gwins);
}

void
EwinOpSetOpacity(EWin * ewin, int source __UNUSED__, int opacity)
{
   unsigned int        op;

   op = OpacityFromPercent(opacity);
   ewin->ewmh.opacity = op;
   HintsSetWindowOpacity(ewin);
   EwinUpdateOpacity(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_OPACITY);
}

void
EwinOpSetFocusedOpacity(EWin * ewin, int source __UNUSED__, int opacity)
{
   unsigned int        op;

   op = OpacityFromPercent(opacity);
   ewin->props.focused_opacity = op;
   EwinUpdateOpacity(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_OPACITY);
}

void
EwinOpMoveToDesk(EWin * ewin, int source __UNUSED__, Desk * dsk, int inc)
{
   dsk = DeskGetRelative(dsk, inc);

   EoSetSticky(ewin, 0);
   EwinMoveToDesktop(ewin, dsk);
   EwinRaise(ewin);
   EwinBorderUpdateState(ewin);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
   HintsSetWindowDesktop(ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_STICKY);
}

#if 0				/* Unused */
void
EwinMoveToLinearArea(EWin * ewin, int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   EwinMoveToArea(ewin, ax, ay);
}

void
EwinMoveLinearAreaBy(EWin * ewin, int a)
{
   EwinMoveToLinearArea(ewin, AreaXYToLinear(ewin->area_x, ewin->area_y) + a);
}

void
EwinOpMoveToArea(EWin * ewin, int x, int y)
{
   EwinMoveToArea(ewin, x, y);
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
}
#endif

#if 0				/* Not used? */
static int
doMoveWinToLinearArea(EWin * ewin, const char *params)
{
   int                 da;

   if (params)
     {
	sscanf(params, "%i", &da);
	EwinMoveToLinearArea(ewin, da);
     }
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
   return 0;
}

static int
doMoveWinByLinearArea(EWin * ewin, const char *params)
{
   EWin               *ewin;
   int                 da;

   if (params)
     {
	sscanf(params, "%i", &da);
	EwinMoveLinearAreaBy(ewin, da);
     }
   SnapshotEwinUpdate(ewin, SNAP_USE_POS);
   return 0;
}
#endif

#if 0				/* FIXME - Unused? */
static int
doScrollWindows(EWin * edummy __UNUSED__, const char *params)
{
   int                 x, y, num, i;
   EWin               *const *lst;

   if (!params)
      return 0;

   x = 0;
   y = 0;
   sscanf(params, "%i %i", &x, &y);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((lst[i]->desktop == DesksGetCurrent()) && (!lst[i]->sticky) &&
	    (!lst[i]->floating))
	   EwinMove(lst[i], lst[i]->x + x, lst[i]->y + y);
     }
   return 0;
}
#endif
