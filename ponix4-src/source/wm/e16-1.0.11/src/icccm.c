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
#include "e16-ecore_hints.h"
#include "ewins.h"
#include "hints.h"
#include "session.h"
#include "xwin.h"
#include <X11/Xutil.h>
#if USE_XSYNC
#include <X11/extensions/sync.h>
#endif

static void         ICCCM_SetIconSizes(void);

void
ICCCM_Init(void)
{
#ifndef USE_ECORE_X
   ecore_x_icccm_init();
#endif

   Mode.current_cmap = WinGetCmap(VROOT);

   ICCCM_SetIconSizes();

   if (Mode.wm.window)
     {
	Atom                wm_props[1];

	wm_props[0] = ECORE_X_ATOM_WM_DELETE_WINDOW;
	XSetWMProtocols(disp, WinGetXwin(VROOT), wm_props, 1);
     }
}

int
ICCCM_ProcessClientClientMessage(EWin * ewin, XClientMessageEvent * event)
{
   if (event->message_type == ECORE_X_ATOM_WM_CHANGE_STATE)
     {
	if (event->data.l[0] == IconicState)
	  {
	     EwinIconify(ewin);
	  }
	return 1;
     }

   return 0;
}

int
ICCCM_ProcessRootClientMessage(XClientMessageEvent * event)
{
   Atom                a;

   if (event->message_type == ECORE_X_ATOM_WM_PROTOCOLS)
     {
	a = event->data.l[0];
	if (a == ECORE_X_ATOM_WM_DELETE_WINDOW)
	   SessionExit(EEXIT_EXIT, NULL);
	return 1;
     }

   return 0;
}

void
ICCCM_GetTitle(EWin * ewin)
{
   _EFREE(EwinGetIcccmName(ewin));

   EwinGetIcccmName(ewin) = ecore_x_icccm_title_get(EwinGetClientXwin(ewin));

   EwinChange(ewin, EWIN_CHANGE_NAME);
}

void
ICCCM_Delete(const EWin * ewin)
{
   if (EwinIsInternal(ewin))
     {
	EwinHide((EWin *) ewin);
	return;
     }

   if (ewin->icccm.delete_window)
      ecore_x_icccm_delete_window_send(EwinGetClientXwin(ewin), CurrentTime);
   else
      XKillClient(disp, EwinGetClientXwin(ewin));
}

#if 0				/* Deprecated */
void
ICCCM_Save(const EWin * ewin)
{
   if (EwinIsInternal(ewin))
      return;

   ecore_x_icccm_send_save_yourself(EwinGetClientXwin(ewin));
}
#endif

void
ICCCM_Iconify(const EWin * ewin)
{
   EUnmapWindow(EwinGetClientWin(ewin));
   ecore_x_icccm_state_set_iconic(EwinGetClientXwin(ewin));
}

void
ICCCM_DeIconify(const EWin * ewin)
{
   EMapWindow(EwinGetClientWin(ewin));
   ecore_x_icccm_state_set_normal(EwinGetClientXwin(ewin));
}

void
ICCCM_Withdraw(const EWin * ewin)
{
   /* We have a choice of deleting the WM_STATE property
    * or changing the value to Withdrawn. Since twm/fvwm does
    * it that way, we change it to Withdrawn.
    */
   ecore_x_icccm_state_set_withdrawn(EwinGetClientXwin(ewin));

   XRemoveFromSaveSet(disp, EwinGetClientXwin(ewin));
}

void
ICCCM_SizeMatch(const EWin * ewin, int wi, int hi, int *pwo, int *pho)
{
   int                 w, h;
   int                 i, j;
   double              aspect, dw, dh;

   w = wi;
   h = hi;

   if (w < ewin->icccm.width_min)
      w = ewin->icccm.width_min;
   if (w > ewin->icccm.width_max)
      w = ewin->icccm.width_max;
   if (h < ewin->icccm.height_min)
      h = ewin->icccm.height_min;
   if (h > ewin->icccm.height_max)
      h = ewin->icccm.height_max;

   if (w <= 0 || h <= 0)
      return;

   w -= ewin->icccm.base_w;
   h -= ewin->icccm.base_h;
   if ((w > 0) && (h > 0))
     {
	/* Ignore aspect ratio constraints when fullscreening */
	if (!ewin->state.fullscreen)
	  {
	     aspect = ((double)w) / ((double)h);
	     dw = ewin->icccm.w_inc / 4.;
	     dh = ewin->icccm.h_inc / 4.;
	     if (Mode.mode == MODE_RESIZE_H)
	       {
		  if (aspect < ewin->icccm.aspect_min)
		     h = (int)((double)w / ewin->icccm.aspect_min + dh);
		  else if (aspect > ewin->icccm.aspect_max)
		     h = (int)((double)w / ewin->icccm.aspect_max + dh);
	       }
	     else if (Mode.mode == MODE_RESIZE_V)
	       {
		  if (aspect < ewin->icccm.aspect_min)
		     w = (int)((double)h * ewin->icccm.aspect_min + dw);
		  else if (aspect > ewin->icccm.aspect_max)
		     w = (int)((double)h * ewin->icccm.aspect_max + dw);
	       }
	     else
	       {
		  if (aspect < ewin->icccm.aspect_min)
		    {
		       if (ewin->icccm.aspect_min >= 1.)
			  h = (int)((double)w / ewin->icccm.aspect_min + dh);
		       else
			  w = (int)((double)h * ewin->icccm.aspect_min + dw);
		    }
		  else if (aspect > ewin->icccm.aspect_max)
		    {
		       if (ewin->icccm.aspect_max >= 1.)
			  h = (int)((double)w / ewin->icccm.aspect_max + dh);
		       else
			  w = (int)((double)h * ewin->icccm.aspect_max + dw);
		    }
	       }
	  }
	i = w / ewin->icccm.w_inc;
	j = h / ewin->icccm.h_inc;
	w = i * ewin->icccm.w_inc;
	h = j * ewin->icccm.h_inc;
     }
   w += ewin->icccm.base_w;
   h += ewin->icccm.base_h;

   *pwo = w;
   *pho = h;
}

#if 0				/* Unused */
void
ICCCM_MatchSize(EWin * ewin)
{
   ICCCM_SizeMatch(ewin, ewin->client.w, ewin->client.h, &ewin->client.w,
		   &ewin->client.h);
}
#endif

void
ICCCM_GetIncrementalSize(EWin * ewin, unsigned int w, unsigned int h,
			 unsigned int *wi, unsigned int *hi)
{
   *wi = (w - ewin->icccm.base_w) / ewin->icccm.w_inc;
   *hi = (h - ewin->icccm.base_h) / ewin->icccm.h_inc;
}

void
ICCCM_SetSizeConstraints(EWin * ewin, unsigned int wmin, unsigned int hmin,
			 unsigned int wmax, unsigned int hmax,
			 unsigned int wbase, unsigned int hbase,
			 unsigned int winc, unsigned int hinc,
			 double amin, double amax)
{
   ewin->icccm.width_min = wmin;
   ewin->icccm.height_min = hmin;
   ewin->icccm.width_max = wmax;
   ewin->icccm.height_max = hmax;

   ewin->icccm.base_w = wbase;
   ewin->icccm.base_h = hbase;
   ewin->icccm.w_inc = winc;
   ewin->icccm.h_inc = hinc;

   ewin->icccm.aspect_min = amin;
   ewin->icccm.aspect_max = amax;

   ewin->props.no_resize_h = (wmin == wmax);
   ewin->props.no_resize_v = (hmin == hmax);
}

void
ICCCM_Configure(EWin * ewin)
{
   XEvent              ev;

   if (EwinIsInternal(ewin))
      return;

   ev.type = ConfigureNotify;
   ev.xconfigure.display = disp;
   ev.xconfigure.event = EwinGetClientXwin(ewin);
   ev.xconfigure.window = EwinGetClientXwin(ewin);
#if 0				/* FIXME - Remove? */
   Desk               *dsk;

   dsk = EoGetDesk(ewin);
   ev.xconfigure.x = EoGetX(dsk) + ewin->client.x;
   ev.xconfigure.y = EoGetY(dsk) + ewin->client.y;
#else
   ev.xconfigure.x = ewin->client.x;
   ev.xconfigure.y = ewin->client.y;
#endif
   if (Mode.wm.window)
     {
	ev.xconfigure.x += Mode.wm.win_x;
	ev.xconfigure.y += Mode.wm.win_y;
     }
   ev.xconfigure.width = ewin->client.w;
   ev.xconfigure.height = ewin->client.h;
   ev.xconfigure.border_width = 0;
   ev.xconfigure.above = EoGetXwin(ewin);
   ev.xconfigure.override_redirect = False;
   EXSendEvent(EwinGetClientXwin(ewin), StructureNotifyMask, &ev);
}

void
ICCCM_AdoptStart(const EWin * ewin)
{
   Window              win = EwinGetClientXwin(ewin);

   if (!EwinIsInternal(ewin))
      XAddToSaveSet(disp, win);
}

void
ICCCM_Adopt(const EWin * ewin)
{
   Window              win = EwinGetClientXwin(ewin);

   if (ewin->icccm.start_iconified)
      ecore_x_icccm_state_set_iconic(win);
   else
      ecore_x_icccm_state_set_normal(win);
}

void
ICCCM_Cmap(EWin * ewin)
{
   Colormap            ecmap, dcmap, ccmap;
   XWindowAttributes   xwa;
   int                 i, num;
   Ecore_X_Window     *wlist;

   ecmap = Mode.current_cmap;
   dcmap = WinGetCmap(VROOT);

   if (!ewin)
     {
	if (ecmap == dcmap)
	   return;
	ccmap = dcmap;
	goto set_cmap;
     }

   ccmap = EwinGetClientWin(ewin)->cmap;

   if (ccmap == ecmap || EoGetWin(ewin)->argb)
      return;

   /* Hack - assume that if client cmap is default cmap it doesn't have
    * WM_COLORMAP_WINDOWS */
   if (ccmap == dcmap)
      goto set_cmap;

   num = ecore_x_window_prop_window_list_get(EwinGetClientXwin(ewin),
					     ECORE_X_ATOM_WM_COLORMAP_WINDOWS,
					     &wlist);
   if (num > 0)
     {
	for (i = 0; i < num; i++)
	  {
	     if (XGetWindowAttributes(disp, wlist[i], &xwa))
	       {
		  if (xwa.colormap != dcmap)
		    {
		       XInstallColormap(disp, xwa.colormap);
		       Mode.current_cmap = xwa.colormap;
		    }
	       }
	  }
	Efree(wlist);
	return;
     }

 set_cmap:
   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("ICCCM_Cmap %#lx\n", ccmap);
   XInstallColormap(disp, ccmap);
   Mode.current_cmap = ccmap;
}

void
ICCCM_Focus(const EWin * ewin)
{
   if (EDebug(EDBUG_TYPE_FOCUS))
     {
	if (ewin)
	   Eprintf("ICCCM_Focus T=%#lx R=%#lx %#lx %s\n", Mode.events.time,
		   NextRequest(disp), EwinGetClientXwin(ewin),
		   EwinGetTitle(ewin));
	else
	   Eprintf("ICCCM_Focus T=%#lx R=%#lx None\n", Mode.events.time,
		   NextRequest(disp));
     }

   if (!ewin)
     {
	XSetInputFocus(disp, WinGetXwin(VROOT), RevertToPointerRoot,
		       Mode.events.time);
	HintsSetActiveWindow(None);
	return;
     }

   if (ewin->icccm.take_focus)
     {
	ecore_x_icccm_take_focus_send(EwinGetClientXwin(ewin),
				      Mode.events.time);
     }

   XSetInputFocus(disp, EwinGetClientXwin(ewin), RevertToPointerRoot,
		  Mode.events.time);

   HintsSetActiveWindow(EwinGetClientXwin(ewin));
}

void
ICCCM_GetGeoms(EWin * ewin)
{
   XSizeHints          hint;
   long                mask;

   if (XGetWMNormalHints(disp, EwinGetClientXwin(ewin), &hint, &mask))
     {
	if (!(ewin->state.placed))
	  {
	     if ((hint.flags & USPosition) || ((hint.flags & PPosition)))
	       {
		  if ((hint.flags & PPosition) && (!EoIsSticky(ewin)))
		    {
		       Desk               *dsk;

		       dsk = EoGetDesk(ewin);
		       if (!dsk)
			  dsk = DesksGetCurrent();
		       ewin->client.x -= EoGetX(dsk);
		       ewin->client.y -= EoGetY(dsk);
		       if (ewin->client.x + ewin->client.w >= WinGetW(VROOT))
			 {
			    ewin->client.x += EoGetX(dsk);
			 }
		       else if (ewin->client.x < 0)
			 {
			    ewin->client.x += EoGetX(dsk);
			 }
		       if (ewin->client.y + ewin->client.h >= WinGetH(VROOT))
			 {
			    ewin->client.y += EoGetY(dsk);
			 }
		       else if (ewin->client.y < 0)
			 {
			    ewin->client.y += EoGetY(dsk);
			 }
		    }
		  ewin->state.placed = 1;
	       }
	  }

	if (hint.flags & PMinSize)
	  {
	     ewin->icccm.width_min = MAX(0, hint.min_width);
	     ewin->icccm.height_min = MAX(0, hint.min_height);
	  }
	else
	  {
	     ewin->icccm.width_min = 0;
	     ewin->icccm.height_min = 0;
	  }

	if (hint.flags & PMaxSize)
	  {
	     ewin->icccm.width_max = MIN(hint.max_width, 65535);
	     ewin->icccm.height_max = MIN(hint.max_height, 65535);
	  }
	else
	  {
	     ewin->icccm.width_max = 65535;
	     ewin->icccm.height_max = 65535;
	  }

	if (hint.flags & PResizeInc)
	  {
	     ewin->icccm.w_inc = MAX(1, hint.width_inc);
	     ewin->icccm.h_inc = MAX(1, hint.height_inc);
	  }
	else
	  {
	     ewin->icccm.w_inc = 1;
	     ewin->icccm.h_inc = 1;
	  }

	if (hint.flags & PAspect)
	  {
	     if ((hint.min_aspect.y > 0.0) && (hint.min_aspect.x > 0.0))
	       {
		  ewin->icccm.aspect_min =
		     ((double)hint.min_aspect.x) / ((double)hint.min_aspect.y);
	       }
	     else
	       {
		  ewin->icccm.aspect_min = 0.0;
	       }
	     if ((hint.max_aspect.y > 0.0) && (hint.max_aspect.x > 0.0))
	       {
		  ewin->icccm.aspect_max =
		     ((double)hint.max_aspect.x) / ((double)hint.max_aspect.y);
	       }
	     else
	       {
		  ewin->icccm.aspect_max = 65535.0;
	       }
	  }
	else
	  {
	     ewin->icccm.aspect_min = 0.0;
	     ewin->icccm.aspect_max = 65535.0;
	  }

	if (hint.flags & PBaseSize)
	  {
	     ewin->icccm.base_w = hint.base_width;
	     ewin->icccm.base_h = hint.base_height;
	  }
	else
	  {
	     ewin->icccm.base_w = ewin->icccm.width_min;
	     ewin->icccm.base_h = ewin->icccm.height_min;
	  }

	if (ewin->icccm.width_min < ewin->icccm.base_w)
	   ewin->icccm.width_min = ewin->icccm.base_w;
	if (ewin->icccm.height_min < ewin->icccm.base_h)
	   ewin->icccm.height_min = ewin->icccm.base_h;

	if (ewin->icccm.width_max < ewin->icccm.base_w)
	   ewin->icccm.width_max = ewin->icccm.base_w;
	if (ewin->icccm.height_max < ewin->icccm.base_h)
	   ewin->icccm.height_max = ewin->icccm.base_h;

	if (hint.flags & PWinGravity)
	   ewin->icccm.grav = hint.win_gravity;
	else
	   ewin->icccm.grav = NorthWestGravity;
     }

   ewin->props.no_resize_h = (ewin->icccm.width_min == ewin->icccm.width_max);
   ewin->props.no_resize_v = (ewin->icccm.height_min == ewin->icccm.height_max);

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get icccm %#lx: %4d+%4d %4dx%4d: %s\n",
	      EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetTitle(ewin));
}

#define TryGroup(e) (((e)->icccm.group != None) && ((e)->icccm.group != EwinGetClientXwin(e)))

static void
ICCCM_GetWmClass(EWin * ewin)
{
   _EFREE(EwinGetIcccmCName(ewin));
   _EFREE(EwinGetIcccmClass(ewin));

   ecore_x_icccm_name_class_get(EwinGetClientXwin(ewin),
				&EwinGetIcccmCName(ewin),
				&EwinGetIcccmClass(ewin));
   if (!EwinGetIcccmCName(ewin) && TryGroup(ewin))
      ecore_x_icccm_name_class_get(ewin->icccm.group,
				   &EwinGetIcccmCName(ewin),
				   &EwinGetIcccmClass(ewin));
}

static void
ICCCM_GetWmCommand(EWin * ewin)
{
   int                 argc;
   char              **argv, s[4096], *ss;

   _EFREE(ewin->icccm.wm_command);

   argc = ecore_x_window_prop_string_list_get(EwinGetClientXwin(ewin),
					      ECORE_X_ATOM_WM_COMMAND, &argv);
   if ((argc < 0) && TryGroup(ewin))
      argc = ecore_x_window_prop_string_list_get(ewin->icccm.group,
						 ECORE_X_ATOM_WM_COMMAND,
						 &argv);

   ss = StrlistEncodeEscaped(s, sizeof(s), argv, argc);
   ewin->icccm.wm_command = Estrdup(ss);
   StrlistFree(argv, argc);
}

static void
ICCCM_GetWmClientMachine(EWin * ewin)
{
   _EFREE(ewin->icccm.wm_machine);

   ewin->icccm.wm_machine =
      ecore_x_window_prop_string_get(EwinGetClientXwin(ewin),
				     ECORE_X_ATOM_WM_CLIENT_MACHINE);
   if (!ewin->icccm.wm_machine && TryGroup(ewin))
      ewin->icccm.wm_machine =
	 ecore_x_window_prop_string_get(ewin->icccm.group,
					ECORE_X_ATOM_WM_CLIENT_MACHINE);
}

static void
ICCCM_GetWmIconName(EWin * ewin)
{
   _EFREE(ewin->icccm.wm_icon_name);

   ewin->icccm.wm_icon_name =
      ecore_x_window_prop_string_get(EwinGetClientXwin(ewin),
				     ECORE_X_ATOM_WM_ICON_NAME);
   if (!ewin->icccm.wm_icon_name && TryGroup(ewin))
      ewin->icccm.wm_icon_name =
	 ecore_x_window_prop_string_get(ewin->icccm.group,
					ECORE_X_ATOM_WM_ICON_NAME);
}

static void
ICCCM_GetWmWindowRole(EWin * ewin)
{
   _EFREE(ewin->icccm.wm_role);
   ewin->icccm.wm_role =
      ecore_x_window_prop_string_get(EwinGetClientXwin(ewin),
				     ECORE_X_ATOM_WM_WINDOW_ROLE);
}

void
ICCCM_GetInfo(EWin * ewin)
{
   ICCCM_GetWmClass(ewin);
   ICCCM_GetWmCommand(ewin);
   ICCCM_GetWmClientMachine(ewin);
   ICCCM_GetWmIconName(ewin);
   ICCCM_GetWmWindowRole(ewin);
}

static void
ICCCM_GetWmHints(EWin * ewin)
{
   XWMHints           *hint;

   hint = XGetWMHints(disp, EwinGetClientXwin(ewin));
   if (!hint)
      return;

   /* I have to make sure the thing i'm docking is a dock app */
   if ((hint->flags & StateHint) && (hint->initial_state == WithdrawnState))
     {
	if (hint->flags & (StateHint | IconWindowHint | IconPositionHint |
			   WindowGroupHint))
	  {
	     if ((hint->icon_x == 0) && (hint->icon_y == 0)
		 && hint->window_group == EwinGetClientXwin(ewin))
		ewin->state.docked = 1;
	  }
     }

   ewin->icccm.need_input =
      ((hint->flags & InputHint) && (!hint->input)) ? 0 : 1;

   ewin->icccm.start_iconified =
      ((hint->flags & StateHint) &&
       (hint->initial_state == IconicState)) ? 1 : 0;

   if (hint->flags & IconPixmapHint)
     {
	if (ewin->icccm.icon_pmap != hint->icon_pixmap)
	  {
	     ewin->icccm.icon_pmap = hint->icon_pixmap;
	     EwinChange(ewin, EWIN_CHANGE_ICON_PMAP);
	  }
     }
   else
     {
	ewin->icccm.icon_pmap = None;
     }

   ewin->icccm.icon_mask =
      (hint->flags & IconMaskHint) ? hint->icon_mask : None;

   ewin->icccm.icon_win =
      (hint->flags & IconWindowHint) ? hint->icon_window : None;

   ewin->icccm.group =
      (hint->flags & WindowGroupHint) ? hint->window_group : None;

   if (hint->flags & XUrgencyHint)
     {
	if (!ewin->state.attention)
	   EwinChange(ewin, EWIN_CHANGE_ATTENTION);
	ewin->icccm.urgency = 1;
	ewin->state.attention = 1;
     }
   else
     {
	ewin->icccm.urgency = 0;
     }

   if (ewin->icccm.group == EwinGetClientXwin(ewin))
     {
	ewin->icccm.is_group_leader = 1;
     }
   else
     {
	ewin->icccm.is_group_leader = 0;
     }

   XFree(hint);
}

static void
ICCCM_GetWmProtocols(EWin * ewin)
{
   Atom               *prop;
   int                 i, num;

   if (XGetWMProtocols(disp, EwinGetClientXwin(ewin), &prop, &num))
     {
	ewin->icccm.take_focus = 0;
	ewin->icccm.delete_window = 0;
	for (i = 0; i < num; i++)
	  {
	     if (prop[i] == ECORE_X_ATOM_WM_TAKE_FOCUS)
		ewin->icccm.take_focus = ewin->icccm.need_input = 1;
	     else if (prop[i] == ECORE_X_ATOM_WM_DELETE_WINDOW)
		ewin->icccm.delete_window = 1;
#if USE_XSYNC
	     else if (prop[i] == ECORE_X_ATOM_NET_WM_SYNC_REQUEST)
	       {
		  unsigned int        c;

		  ewin->ewmh.sync_request_enable = 1;
		  ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin),
						 ECORE_X_ATOM_NET_WM_SYNC_REQUEST_COUNTER,
						 &c, 1);
		  ewin->ewmh.sync_request_counter = c;
	       }
#endif
	  }
	XFree(prop);
     }
}

static void
ICCCM_GetWmTransientFor(EWin * ewin)
{
   Window              win;

   ewin->icccm.transient = 0;
   ewin->icccm.transient_for = None;
   if (XGetTransientForHint(disp, EwinGetClientXwin(ewin), &win))
     {
	ewin->icccm.transient = 1;
	ewin->icccm.transient_for = win;
     }
}

static void
ICCCM_GetWmClientLeader(EWin * ewin)
{
   int                 num;
   Ecore_X_Window      cleader;

   num = ecore_x_window_prop_window_get(EwinGetClientXwin(ewin),
					ECORE_X_ATOM_WM_CLIENT_LEADER,
					&cleader, 1);
   if (num > 0)
     {
	ewin->icccm.client_leader = cleader;
	if (!ewin->icccm.group)
	   ewin->icccm.group = cleader;
     }
}

void
ICCCM_GetHints(EWin * ewin)
{
   ICCCM_GetWmHints(ewin);
   ICCCM_GetWmProtocols(ewin);
   ICCCM_GetWmTransientFor(ewin);
   ICCCM_GetWmClientLeader(ewin);
}

void
ICCCM_SetIconSizes(void)
{
   XIconSize          *is;

   is = XAllocIconSize();
   is->min_width = 8;
   is->min_height = 8;
   is->max_width = 48;
   is->max_height = 48;
   is->width_inc = 1;
   is->height_inc = 1;
   XSetIconSizes(disp, WinGetXwin(VROOT), is, 1);
   XFree(is);
}

/*
 * Process received window property change
 */
int
ICCCM_ProcessPropertyChange(EWin * ewin, Atom atom_change)
{
   if (atom_change == ECORE_X_ATOM_WM_NAME)
     {
	ICCCM_GetTitle(ewin);
	return 1;
     }

   /* ICCCM_GetHints */
   if (atom_change == ECORE_X_ATOM_WM_HINTS)
     {
	ICCCM_GetWmHints(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_PROTOCOLS)
     {
	ICCCM_GetWmProtocols(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_TRANSIENT_FOR)
     {
	ICCCM_GetWmTransientFor(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_CLIENT_LEADER)
     {
	ICCCM_GetWmClientLeader(ewin);
	return 1;
     }

   /* ICCCM_GetInfo */
   if (atom_change == ECORE_X_ATOM_WM_ICON_NAME)
     {
	ICCCM_GetWmIconName(ewin);
	return 1;
     }
#if 1				/* FIXME - Any reason to process these? */
   if (atom_change == ECORE_X_ATOM_WM_CLASS)
     {
	ICCCM_GetWmClass(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_COMMAND)
     {
	ICCCM_GetWmCommand(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_CLIENT_MACHINE)
     {
	ICCCM_GetWmClientMachine(ewin);
	return 1;
     }
   if (atom_change == ECORE_X_ATOM_WM_WINDOW_ROLE)
     {
	ICCCM_GetWmWindowRole(ewin);
	return 1;
     }
#endif

   if (atom_change == ECORE_X_ATOM_WM_COLORMAP_WINDOWS)
     {
	ICCCM_Cmap(ewin);
	return 1;
     }

   if (atom_change == ECORE_X_ATOM_WM_NORMAL_HINTS)
     {
	ICCCM_GetGeoms(ewin);
	return 1;
     }

   return 0;
}

#if USE_XSYNC
int
EwinSyncRequestSend(EWin * ewin)
{
   long long           count;

   if (!ewin->ewmh.sync_request_enable || EServerIsGrabbed())
      return 0;

   count = ++ewin->ewmh.sync_request_count;

   if (count == 0)
      ewin->ewmh.sync_request_count = ++count;
   ecore_x_client_message32_send(EwinGetClientXwin(ewin),
				 ECORE_X_ATOM_WM_PROTOCOLS,
				 StructureNotifyMask,
				 ECORE_X_ATOM_NET_WM_SYNC_REQUEST,
				 Mode.events.time,
				 (long)(count & 0xffffffff),
				 (long)(count >> 32), 0);

   return 1;
}

void
EwinSyncRequestWait(EWin * ewin)
{
   XSyncWaitCondition  xswc[2];
   unsigned int        tus;

   if (!ewin->ewmh.sync_request_enable || EServerIsGrabbed())
      return;

   xswc[0].trigger.counter = ewin->ewmh.sync_request_counter;
   xswc[0].trigger.value_type = XSyncAbsolute;
   XSyncIntsToValue(&xswc[0].trigger.wait_value,
		    ewin->ewmh.sync_request_count & 0xffffffff,
		    ewin->ewmh.sync_request_count >> 32);
   xswc[0].trigger.test_type = XSyncPositiveComparison;
   XSyncIntsToValue(&xswc[0].event_threshold, 0, 0);

   xswc[1].trigger.counter = Mode.display.server_time;
   xswc[1].trigger.value_type = XSyncRelative;
   XSyncIntsToValue(&xswc[1].trigger.wait_value, 200, 0);	/* 200 ms */
   xswc[1].trigger.test_type = XSyncPositiveComparison;
   XSyncIntsToValue(&xswc[1].event_threshold, 0, 0);

   tus = GetTimeUs();
   XSyncAwait(disp, xswc, 2);
   if (EDebug(EDBUG_TYPE_SYNC))
      Eprintf("Sync t=%#lx c=%llx: Delay=%u us\n",
	      xswc[0].trigger.counter, ewin->ewmh.sync_request_count,
	      GetTimeUs() - tus);
}
#endif /* USE_XSYNC */
