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
#include "cursors.h"
#include "desktops.h"		/* FIXME - Should not be here */
#include "dialog.h"
#include "emodule.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"
#include "icons.h"
#include "settings.h"
#include "timers.h"
#include "xwin.h"

#define EwinListFocusRaise(ewin) EobjListFocusRaise(EoObj(ewin))

static char         focus_inhibit = 1;
static char         focus_is_set = 0;
static char         click_pending_update_grabs = 0;
static int          focus_pending_why = 0;
static EWin        *focus_pending_ewin = NULL;
static EWin        *focus_pending_new = NULL;
static EWin        *focus_pending_raise = NULL;
static Timer       *focus_timer_autoraise = NULL;
static int          focus_request = 0;

void
FocusEnable(int on)
{
   if (on)
     {
	if (focus_inhibit > 0)
	   focus_inhibit--;
     }
   else
     {
	focus_inhibit++;
     }

   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("FocusEnable inhibit=%d\n", focus_inhibit);
}

/*
 * Return !0 if it is OK to focus ewin.
 */
static int
FocusEwinValid(EWin * ewin, int want_on_screen, int click, int want_visible)
{
   if (!ewin)
      return 0;

#if 0
   Eprintf("FocusEwinValid %#lx %s: st=%d sh=%d inh=%d cl=%d(%d) vis=%d(%d)\n",
	   EwinGetClientXwin(ewin), EwinGetTitle(ewin),
	   ewin->state.state, EoIsShown(ewin), ewin->state.inhibit_focus,
	   click, ewin->props.focusclick, want_visible, ewin->state.visibility);
#endif

   if (ewin->state.inhibit_focus)
      return 0;

   if (!EoIsMapped(ewin) || !EoIsShown(ewin) ||
       ewin->state.state != EWIN_STATE_MAPPED)
      return 0;

   if (ewin->props.focusclick && !click)
      return 0;

   if (want_visible && ewin->state.visibility == VisibilityFullyObscured)
      return 0;

   return !want_on_screen || EwinIsOnScreen(ewin);
}

/*
 * Return the ewin to focus after entering area or losing focused window.
 */
static EWin        *
FocusEwinSelect(void)
{
   EWin               *const *lst, *ewin;
   int                 num, i;

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_POINTER:
	ewin = GetEwinPointerInClient();
	if (ewin && !FocusEwinValid(ewin, 1, 0, 0))
	   ewin = NULL;
	break;

     case MODE_FOCUS_SLOPPY:
	ewin = GetEwinPointerInClient();
	if (ewin && FocusEwinValid(ewin, 1, 0, 0))
	   break;
	goto do_select;

     case MODE_FOCUS_CLICK:
	goto do_select;

      do_select:
	ewin = NULL;
	lst = EwinListFocusGet(&num);
	for (i = 0; i < num; i++)
	  {
	     if (!FocusEwinValid(lst[i], 1, 0, 0) ||
		 lst[i]->props.skip_focuslist)
		continue;
	     ewin = lst[i];
	     break;
	  }
	break;
     }

   return ewin;
}

static int
AutoraiseTimeout(void *data)
{
   EWin               *ewin = (EWin *) data;

   if (Conf.focus.mode == MODE_FOCUS_CLICK)
      goto done;

   if (EwinFindByPtr(ewin))	/* May be gone */
      EwinRaise(ewin);

 done:
   focus_timer_autoraise = NULL;
   return 0;
}

static void
FocusRaisePending(void)
{
   EWin               *ewin = focus_pending_raise;
   unsigned int        mask;

   /* The focusing cycle ends when no more modifiers are depressed */
   mask = 0;
   EQueryPointer(NULL, NULL, NULL, NULL, &mask);
   if ((mask & Mode.masks.mod_key_mask) != 0)
      return;

   if (EwinFindByPtr(ewin))	/* May be gone */
      EwinListFocusRaise(ewin);

   GrabKeyboardRelease();

   focus_pending_raise = NULL;
}

/*
 * dir > 0: Focus previously focused window
 * else   : Focus least recently focused window
 */
static void
FocusCycleEwin(int dir)
{
   EWin               *const *lst;
   EWin               *ewin;
   int                 i, j, num;

   lst = EwinListFocusGet(&num);
   if (num <= 1)
      return;

   dir = (dir > 0) ? 1 : -1;

   for (j = 0; j < num; j++)
     {
	if (lst[j] == Mode.focuswin)
	   break;
     }
   for (i = 1; i < num; i++)
     {
	ewin = lst[(j + i * dir + num) % num];
	if (!FocusEwinValid(ewin, 1, 0, 0) || ewin->props.skip_focuslist)
	   continue;
	FocusToEWin(ewin, FOCUS_PREV);
	break;
     }
}

static void
ClickGrabsSet(EWin * ewin)
{
   int                 set = 0;

   if ((Conf.focus.clickraises && !EwinListStackIsRaised(ewin)) ||
       (!ewin->state.active && !ewin->state.inhibit_focus))
      set = 1;

   if (set)
     {
	if (!ewin->state.click_grab_isset)
	  {
	     GrabButtonSet(AnyButton, AnyModifier, EwinGetContainerWin(ewin),
			   ButtonPressMask, ECSR_PGRAB, 1);
	     if (EDebug(EDBUG_TYPE_GRABS))
		Eprintf("ClickGrabsSet: %#lx set %s\n",
			EwinGetClientXwin(ewin), EwinGetTitle(ewin));
	     ewin->state.click_grab_isset = 1;
	  }
     }
   else
     {
	if (ewin->state.click_grab_isset)
	  {
	     GrabButtonRelease(AnyButton, AnyModifier,
			       EwinGetContainerWin(ewin));
	     if (EDebug(EDBUG_TYPE_GRABS))
		Eprintf("ClickGrabsSet: %#lx unset %s\n",
			EwinGetClientXwin(ewin), EwinGetTitle(ewin));
	     ewin->state.click_grab_isset = 0;
	  }
     }
}

static void
FocusEwinSetActive(EWin * ewin, int active)
{
   if (ewin->state.active == (unsigned)active)
      return;

   ewin->state.active = active;
   EwinBorderUpdateState(ewin);
   EwinUpdateOpacity(ewin);

   if (active && ewin->state.attention)
     {
	ewin->state.attention = 0;
	HintsSetWindowState(ewin);
     }
}

static void
doClickGrabsUpdate(void)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	ClickGrabsSet(ewin);
     }
   click_pending_update_grabs = 0;
}

void
ClickGrabsUpdate(void)
{
   click_pending_update_grabs = 1;
}

static void
doFocusToEwin(EWin * ewin, int why)
{
   int                 do_focus = 0;
   int                 do_raise = 0, do_warp = 0;

   if (focus_inhibit)
      return;

   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("doFocusToEWin %#lx %s why=%d\n",
	      (ewin) ? EwinGetClientXwin(ewin) : 0,
	      (ewin) ? EwinGetTitle(ewin) : "None", why);

   switch (why)
     {
     case FOCUS_NEXT:
     case FOCUS_PREV:
	if (Conf.focus.raise_on_next)
	   do_raise = 1;
	if (Conf.focus.warp_on_next)
	   do_warp = 1;
	/* Fall thru */
     default:
     case FOCUS_SET:
     case FOCUS_ENTER:
     case FOCUS_LEAVE:		/* Unused */
     case FOCUS_CLICK:
	if (ewin && ewin == Mode.focuswin)
	   return;
	if (!ewin)		/* Unfocus */
	   break;
	if (!FocusEwinValid(ewin, 1, why == FOCUS_CLICK, 0))
	   return;
	break;

     case FOCUS_INIT:
     case FOCUS_DESK_ENTER:
	ewin = FocusEwinSelect();
	break;

     case FOCUS_DESK_LEAVE:
	focus_is_set = 0;
	/* FALLTHROUGH */
     case FOCUS_NONE:
	ewin = NULL;
	if (ewin == Mode.focuswin)
	   return;
	break;

     case FOCUS_EWIN_UNMAP:
	if (Mode.focuswin)
	   return;
	ewin = FocusEwinSelect();
	if (ewin == Mode.focuswin)
	   ewin = NULL;
	break;

     case FOCUS_EWIN_NEW:
	if (Conf.focus.all_new_windows_get_focus)
	   goto check_focus_new;

	if (Mode.place.doing_manual)
	   goto check_focus_new;

	if (ewin->props.focus_when_mapped)
	   goto check_focus_new;

	if (Conf.focus.new_windows_get_focus_if_group_focused && Mode.focuswin)
	  {
	     if (EwinGetWindowGroup(ewin) == EwinGetWindowGroup(Mode.focuswin))
		goto check_focus_new;
	  }

	if (EwinIsTransient(ewin))
	  {
	     if (Conf.focus.new_transients_get_focus)
	       {
		  do_focus = 1;
	       }
	     else if (Conf.focus.new_transients_get_focus_if_group_focused &&
		      Mode.focuswin)
	       {
		  if ((EwinGetTransientFor(ewin) ==
		       EwinGetClientXwin(Mode.focuswin)) ||
		      (EwinGetWindowGroup(ewin) ==
		       EwinGetWindowGroup(Mode.focuswin)))
		     do_focus = 1;
	       }

	     if (!do_focus)
		return;
	     DeskGotoByEwin(ewin);
	     goto check_focus_new;
	  }

	return;

      check_focus_new:
	if (!FocusEwinValid(ewin, 1, 0, 0))
	   return;
	break;
     }

   if (ewin == Mode.focuswin && focus_is_set)
      return;

   /* Check if ewin is a valid focus window target */

   if (!ewin)
      goto done;

   /* NB! ewin != NULL */

   if (why != FOCUS_CLICK && ewin->props.focusclick)
      return;

   if (Conf.autoraise.enable)
     {
	TIMER_DEL(focus_timer_autoraise);

	if (Conf.focus.mode != MODE_FOCUS_CLICK)
	   TIMER_ADD(focus_timer_autoraise, Conf.autoraise.delay,
		     AutoraiseTimeout, ewin);
     }

   if (do_raise)
      EwinRaise(ewin);

   if (Conf.focus.warp_always)
      do_warp = 1;
   if (do_warp)
      EwinWarpTo(ewin, 0);

   switch (why)
     {
     case FOCUS_PREV:
     case FOCUS_NEXT:
	GrabKeyboardSet(VROOT);	/* Causes idler to be called on KeyRelease */
	focus_pending_raise = ewin;
	break;
     case FOCUS_DESK_ENTER:
	if (Conf.focus.mode == MODE_FOCUS_CLICK)
	   break;
	/* FALLTHROUGH */
     default:
     case FOCUS_INIT:
	EwinListFocusRaise(ewin);
	break;
     }

   SoundPlay(SOUND_FOCUS_SET);
 done:

   ClickGrabsUpdate();

   /* Unset old focus window (if any) highlighting */
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 0);
   ICCCM_Cmap(ewin);

   /* Quit if pointer is not on our screen */

   if (!Mode.events.on_screen)
     {
	Mode.focuswin = NULL;
	return;
     }

   /* Set new focus window (if any) highlighting */
   Mode.focuswin = ewin;
   if (Mode.focuswin)
      FocusEwinSetActive(Mode.focuswin, 1);

   if (why == FOCUS_DESK_LEAVE)
      return;

   ICCCM_Focus(ewin);
   focus_is_set = 1;
}

void
FocusToEWin(EWin * ewin, int why)
{
   if (EDebug(EDBUG_TYPE_FOCUS))
      Eprintf("FocusToEWin(%d) %#lx %s why=%d\n", focus_inhibit,
	      (ewin) ? EwinGetClientXwin(ewin) : 0,
	      (ewin) ? EwinGetTitle(ewin) : "None", why);

   switch (why)
     {
     case FOCUS_EWIN_NEW:
	if (!Conf.focus.all_new_windows_get_focus)
	   return;
	if (!FocusEwinValid(ewin, 0, 0, 0))
	   break;
	focus_pending_new = ewin;
	focus_pending_why = why;
	focus_pending_ewin = ewin;
	break;

     default:
	if (ewin && !FocusEwinValid(ewin, 0, why == FOCUS_CLICK, 0))
	   break;
	focus_pending_why = why;
	focus_pending_ewin = ewin;
	break;

     case FOCUS_EWIN_UNMAP:
	focus_pending_why = why;
	focus_pending_ewin = NULL;
	if (ewin == Mode.focuswin)
	  {
	     Mode.focuswin = NULL;
	     focus_is_set = 0;
	     if (!EoIsGone(ewin))
		FocusEwinSetActive(ewin, 0);
	  }
	if (ewin == focus_pending_new)
	   focus_pending_new = NULL;
	break;
     }
   focus_request = (int)NextRequest(disp) - 1;
}

static void
FocusSet(void)
{
   if (focus_pending_new && Conf.focus.all_new_windows_get_focus)
      doFocusToEwin(focus_pending_new, FOCUS_EWIN_NEW);
   else
      doFocusToEwin(focus_pending_ewin, focus_pending_why);
   focus_pending_why = 0;
   focus_pending_ewin = focus_pending_new = NULL;
}

void
FocusNewDeskBegin(void)
{
   /* Freeze keyboard */
   XGrabKeyboard(disp, WinGetXwin(VROOT), False, GrabModeAsync,
		 GrabModeSync, CurrentTime);

   focus_pending_new = NULL;
   doFocusToEwin(NULL, FOCUS_DESK_LEAVE);
}

void
FocusNewDesk(void)
{
   FocusToEWin(NULL, FOCUS_DESK_ENTER);

   /* Unfreeze keyboard */
   XUngrabKeyboard(disp, CurrentTime);
}

static void
_FocusScreenSendEvent(Window xwin, int x, int y, Time t, int enter)
{
   XEvent              xe;

   xe.type = (enter) ? EnterNotify : LeaveNotify;
   xe.xcrossing.window = xwin;
   xe.xcrossing.root = xwin;
   xe.xcrossing.subwindow = 0;
   xe.xcrossing.time = t;
   xe.xcrossing.x = xe.xcrossing.x_root = x;
   xe.xcrossing.y = xe.xcrossing.y_root = y;
   xe.xcrossing.mode = NotifyNormal;
   xe.xcrossing.detail = NotifyNonlinear;
   xe.xcrossing.same_screen = (enter) ? True : False;
   xe.xcrossing.focus = (enter) ? False : True;
   xe.xcrossing.state = 0;

   XSendEvent(disp, xwin, False, EnterWindowMask | LeaveWindowMask, &xe);
}

void
FocusScreen(int scr)
{
   Window              xwin;
   Time                t;
   int                 x, y;

   if (scr < 0 || scr >= ScreenCount(disp))
      return;

   /* IIRC warping to a different screen once did cause
    * LeaveNotify's on the current root window. This does not
    * happen in xorg 1.5.3 (and probably other versions).
    * So, send LeaveNotify to current root and EnterNotify
    * to new root. */

   t = EGetTimestamp();

   /* Report LeaveNotify on current root window */
   xwin = WinGetXwin(VROOT);
   EXQueryPointer(xwin, &x, &y, NULL, NULL);
   _FocusScreenSendEvent(xwin, x, y, t, 0);

   /* Do warp and report EnterNotify on new root window */
   xwin = RootWindow(disp, scr);
   x = DisplayWidth(disp, scr) / 2;
   y = DisplayHeight(disp, scr) / 2;
   EXWarpPointer(xwin, x, y);
   _FocusScreenSendEvent(xwin, x, y, t, 1);
}

static void
FocusInit(void)
{
   /* Start focusing windows */
   FocusEnable(1);

   FocusToEWin(NULL, FOCUS_INIT);
   FocusSet();

   /* Enable window placement features */
   Mode.place.enable_features++;
}

static void
FocusExit(void)
{
}

/*
 * Focus event handlers
 */

void
FocusHandleEnter(EWin * ewin, XEvent * ev)
{
   Mode.mouse_over_ewin = ewin;

#if 1				/* FIXME - Remove? */
   if (ev->xcrossing.mode == NotifyUngrab &&
       ev->xcrossing.detail == NotifyNonlinearVirtual)
     {
	if (EDebug(1))
	   Eprintf("Previously ignored: focused: %s, enter: %s\n",
		   EoGetNameSafe(Mode.focuswin), EoGetNameSafe(ewin));
     }
#endif

   if ((int)ev->xcrossing.serial - focus_request < 0)
     {
	/* This event was caused by a request older than the latest
	 * focus assignment request - ignore */
	if (EDebug(EDBUG_TYPE_FOCUS))
	   Eprintf("FocusHandleEnter: Ignore serial < %#x\n", focus_request);
	return;
     }

   if (!ewin)
     {
	/* Entering root may mean entering this screen */
	FocusToEWin(NULL, FOCUS_DESK_ENTER);
	return;
     }

   switch (Conf.focus.mode)
     {
     default:
     case MODE_FOCUS_CLICK:
	break;
     case MODE_FOCUS_SLOPPY:
	if (FocusEwinValid(ewin, 1, 0, 0))
	   FocusToEWin(ewin, FOCUS_ENTER);
	break;
     case MODE_FOCUS_POINTER:
	if (FocusEwinValid(ewin, 1, 0, 0))
	   FocusToEWin(ewin, FOCUS_ENTER);
	else
	   FocusToEWin(NULL, FOCUS_NONE);
	break;
     }
}

void
FocusHandleLeave(EWin * ewin, XEvent * ev)
{
   if ((int)ev->xcrossing.serial - focus_request < 0)
     {
	/* This event was caused by a request older than the latest
	 * focus assignment request - ignore */
	if (EDebug(EDBUG_TYPE_FOCUS))
	   Eprintf("FocusHandleLeave: Ignore serial < %#x\n", focus_request);
	return;
     }

   /* Leaving root may mean entering other screen */
   if (!ewin)
     {
	if (ev->xcrossing.mode == NotifyNormal &&
	    ev->xcrossing.detail != NotifyInferior)
	   FocusToEWin(NULL, FOCUS_DESK_LEAVE);
     }
}

void
FocusHandleChange(EWin * ewin __UNUSED__, XEvent * ev __UNUSED__)
{
#if 0				/* Debug */
   if (ewin == Mode.focuswin && ev->type == FocusOut)
      Eprintf("??? Lost focus: %s\n", EwinGetTitle(ewin));
#endif
}

void
FocusHandleClick(EWin * ewin, Win win)
{
   if (Conf.focus.clickraises)
      EwinRaise(ewin);

   FocusToEWin(ewin, FOCUS_CLICK);

   /* Allow click to pass thorugh */
   if (EDebug(EDBUG_TYPE_GRABS))
      Eprintf("FocusHandleClick %#lx %#lx\n", WinGetXwin(win),
	      EwinGetContainerXwin(ewin));
   if (win == EwinGetContainerWin(ewin))
     {
	ESync(ESYNC_FOCUS);
	XAllowEvents(disp, ReplayPointer, CurrentTime);
	ESync(ESYNC_FOCUS);
     }
}

#if ENABLE_DIALOGS
/*      
 * Configuration dialog
 */

typedef struct {
   struct {
      int                 mode;
      char                clickalways;
      char                new_focus;
      char                new_focus_if_group;
      char                popup_focus;
      char                popup_focus_if_group;
      char                raise_focus;
      char                warp_focus;
      char                warp_always;
   } focus;

   struct {
      char                enable;
      int                 time;
   } autoraise;

   struct {
      char                enable;
      char                warp_after_focus;
      char                raise_after_focus;
      char                showsticky;
      char                showshaded;
      char                showiconified;
      char                showalldesks;
      char                warpfocused;
      char                show_shape;
      int                 icon_mode;
   } focuslist;
} FocusDlgData;

static void
CB_ConfigureFocus(Dialog * d, int val, void *data __UNUSED__)
{
   FocusDlgData       *dd = DLG_DATA_GET(d, FocusDlgData);

   if (val >= 2)
      return;

   Conf.focus.mode = dd->focus.mode;
   Conf.focus.clickraises = dd->focus.clickalways;
   Conf.focus.all_new_windows_get_focus = dd->focus.new_focus;
   Conf.focus.new_windows_get_focus_if_group_focused =
      dd->focus.new_focus_if_group;
   Conf.focus.new_transients_get_focus = dd->focus.popup_focus;
   Conf.focus.new_transients_get_focus_if_group_focused =
      dd->focus.popup_focus_if_group;
   Conf.focus.raise_on_next = dd->focus.raise_focus;
   Conf.focus.warp_on_next = dd->focus.warp_focus;
   Conf.focus.warp_always = dd->focus.warp_always;

   Conf.autoraise.enable = dd->autoraise.enable;
   Conf.autoraise.delay = 10 * dd->autoraise.time;

   Conf.warplist.enable = dd->focuslist.enable;
   Conf.warplist.warp_on_select = dd->focuslist.warp_after_focus;
   Conf.warplist.raise_on_select = dd->focuslist.raise_after_focus;
   Conf.warplist.showsticky = dd->focuslist.showsticky;
   Conf.warplist.showshaded = dd->focuslist.showshaded;
   Conf.warplist.showiconified = dd->focuslist.showiconified;
   Conf.warplist.showalldesks = dd->focuslist.showalldesks;
   Conf.warplist.warpfocused = dd->focuslist.warpfocused;
   Conf.warplist.show_shape = dd->focuslist.show_shape;
   Conf.warplist.icon_mode = dd->focuslist.icon_mode;

   ClickGrabsUpdate();

   autosave();
}

static void
_DlgFillFocus(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio, *radio2;
   FocusDlgData       *dd;

   dd = DLG_DATA_SET(d, FocusDlgData);
   if (!dd)
      return;

   dd->focus.mode = Conf.focus.mode;
   dd->focus.clickalways = Conf.focus.clickraises;
   dd->focus.new_focus = Conf.focus.all_new_windows_get_focus;
   dd->focus.new_focus_if_group =
      Conf.focus.new_windows_get_focus_if_group_focused;
   dd->focus.popup_focus = Conf.focus.new_transients_get_focus;
   dd->focus.popup_focus_if_group =
      Conf.focus.new_transients_get_focus_if_group_focused;
   dd->focus.raise_focus = Conf.focus.raise_on_next;
   dd->focus.warp_focus = Conf.focus.warp_on_next;
   dd->focus.warp_always = Conf.focus.warp_always;

   dd->autoraise.enable = Conf.autoraise.enable;
   dd->autoraise.time = Conf.autoraise.delay / 10;

   dd->focuslist.enable = Conf.warplist.enable;
   dd->focuslist.raise_after_focus = Conf.warplist.raise_on_select;
   dd->focuslist.warp_after_focus = Conf.warplist.warp_on_select;
   dd->focuslist.showsticky = Conf.warplist.showsticky;
   dd->focuslist.showshaded = Conf.warplist.showshaded;
   dd->focuslist.showiconified = Conf.warplist.showiconified;
   dd->focuslist.showalldesks = Conf.warplist.showalldesks;
   dd->focuslist.warpfocused = Conf.warplist.warpfocused;
   dd->focuslist.show_shape = Conf.warplist.show_shape;
   dd->focuslist.icon_mode = Conf.warplist.icon_mode;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows pointer"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows pointer sloppily"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus follows mouse clicks"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->focus.mode);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Clicking in a window always raises it"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.clickalways);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("All new windows first get the focus"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.new_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _
		     ("New windows get the focus if their window group is focused"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.new_focus_if_group);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Only new dialog windows get the focus"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.popup_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _
		     ("Only new dialogs whose owner is focused get the focus"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.popup_focus_if_group);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise windows while switching focus"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.raise_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Send mouse pointer to window while switching focus"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.warp_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Always send mouse pointer to window on focus switch"));
   DialogItemCheckButtonSetPtr(di, &dd->focus.warp_always);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise windows automatically"));
   DialogItemCheckButtonSetPtr(di, &dd->autoraise.enable);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Autoraise delay:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 0, 300);
   DialogItemSliderSetUnits(di, 10);
   DialogItemSliderSetJump(di, 25);
   DialogItemSliderSetValPtr(di, &dd->autoraise.time);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display and use focus list"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.enable);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include sticky windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.showsticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include shaded windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.showshaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include iconified windows in focus list"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.showiconified);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Include windows on other desks in focus list"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.showalldesks);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Focus windows while switching"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.warpfocused);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Outline windows while switching"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.show_shape);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raise windows after focus switch"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.raise_after_focus);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Send mouse pointer to window after focus switch"));
   DialogItemCheckButtonSetPtr(di, &dd->focuslist.warp_after_focus);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _
		     ("Focuslist image display policy (if one operation fails, try the next):"));

   radio2 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("First E Icon, then App Icon"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_IMG_APP);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("First App Icon, then E Icon"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_APP_IMG);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("None"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, EWIN_ICON_MODE_NONE);
   DialogItemRadioButtonGroupSetValPtr(radio2, &dd->focuslist.icon_mode);
}

const DialogDef     DlgFocus = {
   "CONFIGURE_FOCUS",
   N_("Focus"),
   N_("Focus Settings"),
   SOUND_SETTINGS_FOCUS,
   "pix/focus.png",
   N_("Enlightenment Focus\n" "Settings Dialog"),
   _DlgFillFocus,
   DLG_OAC, CB_ConfigureFocus,
};
#endif /* ENABLE_DIALOGS */

/*
 * Focus Module
 */

static int
FocusInitTimeout(void *data __UNUSED__)
{
   FocusInit();
   return 0;
}

static void
_FocusIdler(void *data __UNUSED__)
{
   if (!focus_inhibit && focus_pending_why)
      FocusSet();
   if (click_pending_update_grabs)
      doClickGrabsUpdate();
   if (focus_pending_raise)
      FocusRaisePending();
}

static void
FocusSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_START:
	/* Delay focusing a bit to allow things to settle down */
	IdlerAdd(_FocusIdler, NULL);
	TIMER_ADD_NP(500, FocusInitTimeout, NULL);
	break;

     case ESIGNAL_EXIT:
	FocusExit();
	break;
     }
}

static void
FocusIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
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
	EWin               *ewin;

	ewin = GetFocusEwin();
	if (ewin)
	   IpcPrintf("Focused: %#lx\n", EwinGetClientXwin(ewin));
	else
	   IpcPrintf("Focused: none\n");
     }
   else if (!strncmp(cmd, "mode", 2))
     {
	int                 mode = Conf.focus.mode;

	if (!strcmp(prm, "click"))
	  {
	     mode = MODE_FOCUS_CLICK;
	     Mode.grabs.pointer_grab_active = 1;
	  }
	else if (!strcmp(prm, "clicknograb"))
	  {
	     mode = MODE_FOCUS_CLICK;
	     Mode.grabs.pointer_grab_active = 0;
	  }
	else if (!strcmp(prm, "pointer"))
	  {
	     mode = MODE_FOCUS_POINTER;
	  }
	else if (!strcmp(prm, "sloppy"))
	  {
	     mode = MODE_FOCUS_SLOPPY;
	  }
	else if (!strcmp(prm, "?"))
	  {
	     if (Conf.focus.mode == MODE_FOCUS_CLICK)
	       {
		  if (Mode.grabs.pointer_grab_active)
		     p = "click";
		  else
		     p = "clicknograb";
	       }
	     else if (Conf.focus.mode == MODE_FOCUS_SLOPPY)
		p = "sloppy";
	     else if (Conf.focus.mode == MODE_FOCUS_POINTER)
		p = "pointer";
	     else
		p = "unknown";
	     IpcPrintf("Focus Mode: %s\n", p);
	  }
	else
	  {
	     IpcPrintf("Error: unknown focus type\n");
	  }
	if (Conf.focus.mode != mode)
	  {
	     Conf.focus.mode = mode;
	     ClickGrabsUpdate();
	     autosave();
	  }
     }
   else if (!strncmp(cmd, "next", 2))
     {
	/* Focus previously focused window */
	if (Conf.warplist.enable)
	   WarpFocus(1);
	else
	   FocusCycleEwin(1);
     }
   else if (!strncmp(cmd, "prev", 2))
     {
	/* Focus least recently focused window */
	if (Conf.warplist.enable)
	   WarpFocus(-1);
	else
	   FocusCycleEwin(-1);
     }
}

static const IpcItem FocusIpcArray[] = {
   {
    FocusIpc,
    "focus", "sf",
    "Focus functions",
    "  focus ?                     Show focus info\n"
    "  focus mode                  Set focus mode. Modes:\n"
    "    click:       The traditional click-to-focus mode.\n"
    "    clicknograb: A similar focus mode, but without the grabbing of the click\n"
    "      (you cannot click anywhere in a window to focus it)\n"
    "    pointer:     The focus will follow the mouse pointer\n"
    "    sloppy:      The focus follows the mouse, but when over the desktop background\n"
    "                 the last window does not lose the focus\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(FocusIpcArray)/sizeof(IpcItem))

static const CfgItem FocusCfgItems[] = {
   CFG_ITEM_INT(Conf.focus, mode, MODE_FOCUS_SLOPPY),
   CFG_ITEM_BOOL(Conf.focus, clickraises, 1),
   CFG_ITEM_BOOL(Conf.focus, transientsfollowleader, 1),
   CFG_ITEM_BOOL(Conf.focus, switchfortransientmap, 1),
   CFG_ITEM_BOOL(Conf.focus, all_new_windows_get_focus, 0),
   CFG_ITEM_BOOL(Conf.focus, new_windows_get_focus_if_group_focused, 1),
   CFG_ITEM_BOOL(Conf.focus, new_transients_get_focus, 0),
   CFG_ITEM_BOOL(Conf.focus, new_transients_get_focus_if_group_focused, 1),
   CFG_ITEM_BOOL(Conf.focus, raise_on_next, 1),
   CFG_ITEM_BOOL(Conf.focus, warp_on_next, 0),
   CFG_ITEM_BOOL(Conf.focus, warp_always, 0),

   CFG_ITEM_BOOL(Conf, autoraise.enable, 0),
   CFG_ITEM_INT(Conf, autoraise.delay, 500),
};
#define N_CFG_ITEMS (sizeof(FocusCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModFocus;

const EModule       ModFocus = {
   "focus", NULL,
   FocusSighan,
   {N_IPC_FUNCS, FocusIpcArray},
   {N_CFG_ITEMS, FocusCfgItems}
};
