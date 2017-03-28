/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "xwin.h"

/* WIN_WM_NAME STRING - contains a string identifier for the WM's name */
#define XA_WIN_WM_NAME                     "_WIN_WM_NAME"

/* WIN_WM_NAME VERSION - contains a string identifier for the WM's version */
#define XA_WIN_WM_VERSION                  "_WIN_WM_VERSION"

/* WIN_AREA CARD32[2] contains the current desktop area X,Y */
#define XA_WIN_AREA                        "_WIN_AREA"

/* WIN_AREA CARD32[2] contains the current desktop area size WxH */
#define XA_WIN_AREA_COUNT                  "_WIN_AREA_COUNT"

/* array of atoms - atom being one of the following atoms */
#define XA_WIN_PROTOCOLS                   "_WIN_PROTOCOLS"

/* array of iocn in various sizes */
/* Type: array of CARD32 */
/*       first item is icon count (n) */
/*       second item is icon record length (in CARD32s) */
/*       this is followed by (n) icon records as follows */
/*           pixmap (XID) */
/*           mask (XID) */
/*           width (CARD32) */
/*           height (CARD32) */
/*           depth (of pixmap, mask is assumed to be of depth 1) (CARD32) */
/*           drawable (screen root drawable of pixmap) (XID) */
/*           ... additional fields can be added at the end of this list */
#define XA_WIN_ICONS                    "_WIN_ICONS"

/* WIN_WORKSPACE CARD32 contains the current desktop number */
#define XA_WIN_WORKSPACE                   "_WIN_WORKSPACE"
/* WIN_WORKSPACE_COUNT CARD32 contains the number of desktops */
#define XA_WIN_WORKSPACE_COUNT             "_WIN_WORKSPACE_COUNT"

/* WIN_WORKSPACE_NAMES StringList (Text Property) of workspace names */
/* unused by enlightenment */
#define XA_WIN_WORKSPACE_NAMES             "_WIN_WORKSPACE_NAMES"

/* ********** Don't use this.. iffy at best. *********** */
/* The available work area for client windows. The WM can set this and the WM */
/* and/or clients may change it at any time. If it is changed the WM and/or  */
/* clients should honor the changes. If this property does not exist a client */
/* or WM can create it. */
/*
 * CARD32              min_x;
 * CARD32              min_y;
 * CARD32              max_x;
 * CARD32              max_y;
 */
#define XA_WIN_WORKAREA                    "_WIN_WORKAREA"
/* array of 4 CARD32's */

/* This is a list of window id's the WM is currently managing - primarily */
/* for being able to have external "tasklist" apps */
#define XA_WIN_CLIENT_LIST                 "_WIN_CLIENT_LIST"
/* array of N XID's */

/*********************************************************/
/* Properties on client windows                          */
/*********************************************************/

/* The layer the window exists in */
/*      0 = Desktop */
/*      1 = Below */
/*      2 = Normal (default app layer) */
/*      4 = OnTop */
/*      6 = Dock (always on top - for panel) */
/* The app sets this alone, not the WM. If this property changes the WM */
/* should comply and change the appearance/behavior of the Client window */
/* if this hint does not exist the WM Will create it on the Client window */
#define WIN_LAYER_DESKTOP                0
#define WIN_LAYER_BELOW                  2
#define WIN_LAYER_NORMAL                 4
#define WIN_LAYER_ONTOP                  6
#define WIN_LAYER_DOCK                   8
#define WIN_LAYER_ABOVE_DOCK             10
#define WIN_LAYER_MENU                   12
#define XA_WIN_LAYER                     "_WIN_LAYER"
/* WIN_LAYER = CARD32 */

/* flags for the window's state. The WM will change these as needed when */
/* state changes. If the property contains info on client map, E will modify */
/* the windows state accordingly. if the Hint does not exist the WM will */
/* create it on the client window. 0 for the bit means off, 1 means on. */
/* unused (default) values are 0 */

/* removed Minimized - no explanation of what it really means - ambiguity */
/* should not be here if not clear */
#define WIN_STATE_STICKY          (1<<0)	/* everyone knows sticky */
#define WIN_STATE_RESERVED_BIT1   (1<<1)	/* removed minimize here */
#define WIN_STATE_MAXIMIZED_VERT  (1<<2)	/* window in maximized V state */
#define WIN_STATE_MAXIMIZED_HORIZ (1<<3)	/* window in maximized H state */
#define WIN_STATE_HIDDEN          (1<<4)	/* not on taskbar but window visible */
#define WIN_STATE_SHADED          (1<<5)	/* shaded (NeXT style) */
#define WIN_STATE_HID_WORKSPACE   (1<<6)	/* not on current desktop */
#define WIN_STATE_HID_TRANSIENT   (1<<7)	/* owner of transient is hidden */
#define WIN_STATE_FIXED_POSITION  (1<<8)	/* window is fixed in position even */
#define WIN_STATE_ARRANGE_IGNORE  (1<<9)	/* ignore for auto arranging */
					 /* when scrolling about large */
					 /* virtual desktops ala fvwm */
#define XA_WIN_STATE              "_WIN_STATE"
/* WIN_STATE = CARD32 */

/* Preferences for behavior for app */
/* ONLY the client sets this */
#define WIN_HINTS_SKIP_FOCUS             (1<<0)	/* "alt-tab" skips this win */
#define WIN_HINTS_SKIP_WINLIST           (1<<1)	/* not in win list */
#define WIN_HINTS_SKIP_TASKBAR           (1<<2)	/* not on taskbar */
#define WIN_HINTS_GROUP_TRANSIENT        (1<<3)	/* ??????? */
#define WIN_HINTS_FOCUS_ON_CLICK         (1<<4)	/* app only accepts focus when clicked */
#define WIN_HINTS_DO_NOT_COVER           (1<<5)	/* attempt to not cover this window */
#define XA_WIN_HINTS                     "_WIN_HINTS"
/* WIN_HINTS = CARD32 */

/* Application state - also "color reactiveness" - the app can keep changing */
/* this property when it changes its state and the WM or monitoring program */
/* will pick this up and display somehting accordingly. ONLY the client sets */
/* this. */
#define WIN_APP_STATE_NONE                 0
#define WIN_APP_STATE_ACTIVE1              1
#define WIN_APP_STATE_ACTIVE2              2
#define WIN_APP_STATE_ERROR1               3
#define WIN_APP_STATE_ERROR2               4
#define WIN_APP_STATE_FATAL_ERROR1         5
#define WIN_APP_STATE_FATAL_ERROR2         6
#define WIN_APP_STATE_IDLE1                7
#define WIN_APP_STATE_IDLE2                8
#define WIN_APP_STATE_WAITING1             9
#define WIN_APP_STATE_WAITING2             10
#define WIN_APP_STATE_WORKING1             11
#define WIN_APP_STATE_WORKING2             12
#define WIN_APP_STATE_NEED_USER_INPUT1     13
#define WIN_APP_STATE_NEED_USER_INPUT2     14
#define WIN_APP_STATE_STRUGGLING1          15
#define WIN_APP_STATE_STRUGGLING2          16
#define WIN_APP_STATE_DISK_TRAFFIC1        17
#define WIN_APP_STATE_DISK_TRAFFIC2        18
#define WIN_APP_STATE_NETWORK_TRAFFIC1     19
#define WIN_APP_STATE_NETWORK_TRAFFIC2     20
#define WIN_APP_STATE_OVERLOADED1          21
#define WIN_APP_STATE_OVERLOADED2          22
#define WIN_APP_STATE_PERCENT000_1         23
#define WIN_APP_STATE_PERCENT000_2         24
#define WIN_APP_STATE_PERCENT010_1         25
#define WIN_APP_STATE_PERCENT010_2         26
#define WIN_APP_STATE_PERCENT020_1         27
#define WIN_APP_STATE_PERCENT020_2         28
#define WIN_APP_STATE_PERCENT030_1         29
#define WIN_APP_STATE_PERCENT030_2         30
#define WIN_APP_STATE_PERCENT040_1         31
#define WIN_APP_STATE_PERCENT040_2         32
#define WIN_APP_STATE_PERCENT050_1         33
#define WIN_APP_STATE_PERCENT050_2         34
#define WIN_APP_STATE_PERCENT060_1         35
#define WIN_APP_STATE_PERCENT060_2         36
#define WIN_APP_STATE_PERCENT070_1         37
#define WIN_APP_STATE_PERCENT070_2         38
#define WIN_APP_STATE_PERCENT080_1         39
#define WIN_APP_STATE_PERCENT080_2         40
#define WIN_APP_STATE_PERCENT090_1         41
#define WIN_APP_STATE_PERCENT090_2         42
#define WIN_APP_STATE_PERCENT100_1         43
#define WIN_APP_STATE_PERCENT100_2         44
#define XA_WIN_APP_STATE                   "_WIN_APP_STATE"
/* WIN_APP_STATE = CARD32 */

/* Expanded space occupied - this is the area on screen the app's window */
/* will occupy when "expanded" - ie if you have a button on an app that */
/* "hides" it by reducing its size, this is the geometry of the expanded */
/* window - so the window manager can allow for this when doign auto */
/* positioing of client windows assuming the app can at any point use this */
/* this area and thus try and keep it clear. ONLY the client sets this */
/*
 * CARD32              x;
 * CARD32              y;
 * CARD32              width;
 * CARD32              height;
 */
#define XA_WIN_EXPANDED_SIZE               "_WIN_EXPANDED_SIZE"
/* array of 4 CARD32's */

/* CARD32 that contians the desktop number the application is on If the */
/* application's state is "sticky" it is irrelevant. Only the WM should */
/* change this. */
#define XA_WIN_WORKSPACE                   "_WIN_WORKSPACE"

/* This atom is a 32-bit integer that is either 0 or 1 (currently). */
/* 0 denotes everything is as per usual but 1 denotes that ALL configure */
/* requests by the client on the client window with this property are */
/* not just a simple "moving" of the window, but the result of a user */
/* moving the window BUT the client handling that interaction by moving */
/* its own window. The window manager should respond accordingly by assuming */
/* any configure requests for this window whilst this atom is "active" in */
/* the "1" state are a client move and should handle flipping desktops if */
/* the window is being dragged "off screem" or across desktop boundaries */
/* etc. This atom is ONLY ever set by the client */
#define XA_WIN_CLIENT_MOVING               "_WIN_CLIENT_MOVING"
/* WIN_CLIENT_MOVING = CARD32 */

/* Designed for checking if the WIN_ supporting WM is still there  */
/* and kicking about - basically check this property - check the window */
/* ID it points to - then check that window Id has this property too */
/* if that is the case the WIN_ supporting WM is there and alive and the */
/* list of WIN_PROTOCOLS is valid */
#define XA_WIN_SUPPORTING_WM_CHECK         "_WIN_SUPPORTING_WM_CHECK"
/* CARD32 */

/*********************************************************/
/* How an app can modify things after mapping            */
/*********************************************************/

/* For a client to change layer or state it should send a client message */
/* to the root window as follows: */
/*
 * Display             *disp;
 * Window               root, client_window;
 * XClientMessageEvent  xev;
 * CARD32                new_layer;
 * 
 *     xev.type = ClientMessage;
 *     xev.window = client_window;
 *     xev.message_type = EInternAtom(XA_WIN_LAYER);
 *     xev.format = 32;
 *     xev.data.l[0] = new_layer;
 *     xev.data.l[1] = CurrentTime;
 *     XSendEvent(disp, root, False, SubstructureNotifyMask, (XEvent *) &xev);
 */
/*
 * Display             *disp;
 * Window               root, client_window;
 * XClientMessageEvent  xev;
 * CARD32               mask_of_members_to_change, new_members;
 * 
 *     xev.type = ClientMessage;
 *     xev.window = client_window;
 *     xev.message_type = EInternAtom(XA_WIN_STATE);
 *     xev.format = 32;
 *     xev.data.l[0] = mask_of_members_to_change;
 *     xev.data.l[1] = new_members;
 *     xev.data.l[2] = CurrentTimep;
 *     XSendEvent(disp, root, False, SubstructureNotifyMask, (XEvent *) &xev);
 */
/*
 * Display             *disp;
 * Window               root, client_window;
 * XClientMessageEvent  xev;
 * CARD32               new_desktop_number;
 * 
 *     xev.type = ClientMessage;
 *     xev.window = client_window;
 *     xev.message_type = EInternAtom(XA_WIN_WORKSPACE);
 *     xev.format = 32;
 *     xev.data.l[0] = new_desktop_number;
 *     xev.data.l[2] = CurrentTimep;
 *     XSendEvent(disp, root, False, SubstructureNotifyMask, (XEvent *) &xev);
 */

#if 0				/* Does nothing useful */
static void
GNOME_GetHintIcons(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num, i;
   Ecore_X_ID         *plst;
   Pixmap              pmap;
   Pixmap              mask;

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_ICONS);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_xid_list_get(EwinGetClientXwin(ewin), atom_get,
					  XA_PIXMAP, &plst);
   if (num < 2)
      return;

   for (i = 0; i < num / 2; i++)
     {
	pmap = plst[2 * i];
	mask = plst[2 * i + 1];
     }
   free(plst);
}
#endif

static void
GNOME_GetHintLayer(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        layer;

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_LAYER);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					&layer, 1);
   if (num <= 0)
      return;

   EoSetLayer(ewin, layer);
   EwinChange(ewin, EWIN_CHANGE_LAYER);
}

static void
GNOME_GetHintState(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        flags;

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_STATE);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					&flags, 1);
   if (num <= 0)
      return;

   if (flags & WIN_STATE_SHADED)
      ewin->state.shaded = 1;
   if (flags & WIN_STATE_STICKY)
      EoSetSticky(ewin, 1);
   if (flags & WIN_STATE_FIXED_POSITION)
      EwinInhSetUser(ewin, move, 1);
   if (flags & WIN_STATE_ARRANGE_IGNORE)
      ewin->props.ignorearrange = 1;
}

#if 0				/* Does nothing */
static void
GNOME_GetHintAppState(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        flags;

   /* have nothing interesting to do with an app state (lamp) right now */

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_APP_STATE);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					&flags, 1);
   if (num <= 0)
      return;
}
#endif

static void
GNOME_GetHintDesktop(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        desk;

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_WORKSPACE);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					&desk, 1);
   if (num <= 0)
      return;

   EoSetDesk(ewin, DeskGet(desk));
   EwinChange(ewin, EWIN_CHANGE_DESKTOP);
}

static void
GNOME_GetHint(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        flags;

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_HINTS);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					&flags, 1);
   if (num <= 0)
      return;

   if (flags & WIN_HINTS_SKIP_TASKBAR)
      ewin->props.skip_ext_task = 1;
   if (flags & WIN_HINTS_SKIP_FOCUS)
      ewin->props.skip_focuslist = 1;
   if (flags & WIN_HINTS_SKIP_WINLIST)
      ewin->props.skip_winlist = 1;
   if (flags & WIN_HINTS_FOCUS_ON_CLICK)
      ewin->props.focusclick = 1;
   if (flags & WIN_HINTS_DO_NOT_COVER)
      ewin->props.never_use_area = 1;
}

void
GNOME_SetHint(const EWin * ewin)
{
   static Atom         atom_set = 0;
   unsigned int        val;

   if ((ewin->type == EWIN_TYPE_MENU) || (ewin->type == EWIN_TYPE_PAGER))
      return;
   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_STATE);
   val = 0;
   if (EoIsSticky(ewin))
      val |= WIN_STATE_STICKY;
   if (ewin->state.shaded)
      val |= WIN_STATE_SHADED;
   if (EwinInhGetUser(ewin, move))
      val |= WIN_STATE_FIXED_POSITION;
   ecore_x_window_prop_card32_set(EwinGetClientXwin(ewin), atom_set, &val, 1);
}

void
GNOME_SetEwinArea(const EWin * ewin)
{
   static Atom         atom_set = 0;
   unsigned int        val[2];

   if ((ewin->type == EWIN_TYPE_MENU) || (ewin->type == EWIN_TYPE_PAGER))
      return;
   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_AREA);
   val[0] = ewin->area_x;
   val[1] = ewin->area_y;
   ecore_x_window_prop_card32_set(EwinGetClientXwin(ewin), atom_set, val, 2);
}

void
GNOME_SetEwinDesk(const EWin * ewin)
{
   static Atom         atom_set = 0;
   unsigned int        val;

   if ((ewin->type == EWIN_TYPE_MENU) || (ewin->type == EWIN_TYPE_PAGER))
      return;
   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_WORKSPACE);
   val = EoGetDeskNum(ewin);
   ecore_x_window_prop_card32_set(EwinGetClientXwin(ewin), atom_set, &val, 1);
}

#if 0				/* Does nothing */
static void
GNOME_GetExpandedSize(EWin * ewin, Atom atom_change)
{
   static Atom         atom_get = 0;
   int                 num;
   unsigned int        exp[4];

   if (EwinIsInternal(ewin))
      return;

   if (!atom_get)
      atom_get = EInternAtom(XA_WIN_EXPANDED_SIZE);
   if ((atom_change) && (atom_change != atom_get))
      return;

   num = ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), atom_get,
					exp, 4);
   if (num >= 4)
     {
#if 0				/* Not actually used */
	ewin->expanded_x = retval[0];
	ewin->expanded_y = retval[1];
	ewin->expanded_width = retval[2];
	ewin->expanded_height = retval[3];
#endif
     }
}
#endif

static void
GNOME_SetUsedHints(void)
{
   static Atom         atom_set = 0;
   Ecore_X_Atom        list[10];

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_PROTOCOLS);
   list[0] = EInternAtom(XA_WIN_LAYER);
   list[1] = EInternAtom(XA_WIN_STATE);
   list[2] = EInternAtom(XA_WIN_HINTS);
   list[3] = EInternAtom(XA_WIN_APP_STATE);
   list[4] = EInternAtom(XA_WIN_EXPANDED_SIZE);
   list[5] = EInternAtom(XA_WIN_ICONS);
   list[6] = EInternAtom(XA_WIN_WORKSPACE);
   list[7] = EInternAtom(XA_WIN_WORKSPACE_COUNT);
   list[8] = EInternAtom(XA_WIN_WORKSPACE_NAMES);
   list[9] = EInternAtom(XA_WIN_CLIENT_LIST);
   ecore_x_window_prop_atom_set(WinGetXwin(VROOT), atom_set, list, 10);
}

void
GNOME_SetCurrentArea(void)
{
   static Atom         atom_set = 0;
   unsigned int        val[2];
   int                 ax, ay;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_AREA);
   DeskCurrentGetArea(&ax, &ay);
   val[0] = ax;
   val[1] = ay;
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, val, 2);
}

void
GNOME_SetCurrentDesk(void)
{
   static Atom         atom_set = 0;
   unsigned int        val;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_WORKSPACE);
   val = DesksGetCurrentNum();
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, &val, 1);
}

static void
GNOME_SetWMCheck(Window win_wm_check)
{
   static Atom         atom_set = 0;
   unsigned int        val;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_SUPPORTING_WM_CHECK);
   val = win_wm_check;
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, &val, 1);
   ecore_x_window_prop_card32_set(win_wm_check, atom_set, &val, 1);
}

void
GNOME_SetDeskCount(void)
{
   static Atom         atom_set = 0;
   unsigned int        val;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_WORKSPACE_COUNT);
   val = DesksGetNumber();
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, &val, 1);
}

void
GNOME_SetAreaCount(void)
{
   static Atom         atom_set = 0;
   int                 ax, ay;
   unsigned int        val[2];

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_AREA_COUNT);
   DesksGetAreaSize(&ax, &ay);
   val[0] = ax;
   val[1] = ay;
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, val, 2);
}

void
GNOME_SetDeskNames(void)
{
   static Atom         atom_set = 0;
   char                s[1024], **names;
   int                 i, n_desks;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_WORKSPACE_NAMES);

   n_desks = DesksGetNumber();
   names = EMALLOC(char *, n_desks);

   if (!names)
      return;

   for (i = 0; i < n_desks; i++)
     {
	Esnprintf(s, sizeof(s), "%i", i);
	names[i] = Estrdup(s);
     }

   ecore_x_window_prop_string_list_set(WinGetXwin(VROOT), atom_set, names,
				       n_desks);

   for (i = 0; i < n_desks; i++)
      Efree(names[i]);
   Efree(names);
}

void
GNOME_SetClientList(void)
{
   static Atom         atom_set = 0;
   unsigned int       *wl;
   int                 j, i, num;
   EWin               *const *lst;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_CLIENT_LIST);

   lst = EwinListOrderGet(&num);
   wl = NULL;
   j = 0;
   if (lst)
     {
	wl = EMALLOC(unsigned int, num);

	for (i = 0; i < num; i++)
	  {
	     if (!lst[i]->props.skip_ext_task && !EwinIsTransientChild(lst[i]))
		wl[j++] = EwinGetClientXwin(lst[i]);
	  }
     }
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, wl, j);
   Efree(wl);
}

static void
GNOME_SetWMNameVer(void)
{
   static Atom         atom_set = 0, atom_set2 = 0;

   if (!atom_set)
      atom_set = EInternAtom(XA_WIN_WM_NAME);
   ecore_x_window_prop_string_set(WinGetXwin(VROOT), atom_set, e_wm_name);

   if (!atom_set2)
      atom_set2 = EInternAtom(XA_WIN_WM_VERSION);
   ecore_x_window_prop_string_set(WinGetXwin(VROOT), atom_set2, e_wm_version);
}

void
GNOME_DelHints(const EWin * ewin)
{
   static Atom         atom_get[6] = { 0, 0, 0, 0, 0, 0 };
   Window              win;

   if (!atom_get[0])
     {
	atom_get[0] = EInternAtom(XA_WIN_WORKSPACE);
	atom_get[1] = EInternAtom(XA_WIN_LAYER);
	atom_get[2] = EInternAtom(XA_WIN_STATE);
	atom_get[3] = EInternAtom(XA_WIN_HINTS);
	atom_get[4] = EInternAtom(XA_WIN_APP_STATE);
	atom_get[5] = EInternAtom(XA_WIN_AREA);
     }

   win = EwinGetClientXwin(ewin);
   XDeleteProperty(disp, win, atom_get[0]);
   XDeleteProperty(disp, win, atom_get[1]);
   XDeleteProperty(disp, win, atom_get[2]);
   XDeleteProperty(disp, win, atom_get[3]);
   XDeleteProperty(disp, win, atom_get[4]);
   XDeleteProperty(disp, win, atom_get[5]);
}

void
GNOME_GetHints(EWin * ewin, Atom atom_change)
{
   GNOME_GetHintDesktop(ewin, atom_change);
   GNOME_GetHintLayer(ewin, atom_change);
   GNOME_GetHintState(ewin, atom_change);
   GNOME_GetHint(ewin, atom_change);
#if 0				/* Do nothing */
   GNOME_GetHintIcons(ewin, atom_change);
   GNOME_GetHintAppState(ewin, atom_change);
   GNOME_GetExpandedSize(ewin, atom_change);
#endif
}

void
GNOME_SetHints(Window win_wm_check)
{
   GNOME_SetWMNameVer();
   GNOME_SetUsedHints();
   GNOME_SetWMCheck(win_wm_check);
   {
      Atom                atom_set;
      unsigned int        val;

      atom_set = EInternAtom("_WIN_DESKTOP_BUTTON_PROXY");
      Mode.button_proxy_win =
	 XCreateSimpleWindow(disp, WinGetXwin(VROOT), -80, -80, 24, 24, 0,
			     0, 0);
      val = Mode.button_proxy_win;
      ecore_x_window_prop_card32_set(WinGetXwin(VROOT), atom_set, &val, 1);
      ecore_x_window_prop_card32_set(Mode.button_proxy_win, atom_set, &val, 1);
   }
}

int
GNOME_ProcessClientClientMessage(EWin * ewin, XClientMessageEvent * event)
{
   static Atom         a4 = 0, a5 = 0;

   if (!a4)
      a4 = EInternAtom("_WIN_LAYER");
   if (!a5)
      a5 = EInternAtom("_WIN_STATE");

   if (event->message_type == a4)
     {
	unsigned int        val;

	val = event->data.l[0];
	EoSetLayer(ewin, val);
	ecore_x_window_prop_card32_set(EwinGetClientXwin(ewin), a4, &val, 1);
	EwinRaise(ewin);
	return 1;
     }
   if (event->message_type == a5)
     {
	if (event->data.l[0] & WIN_STATE_FIXED_POSITION)
	  {
	     if (event->data.l[1] & WIN_STATE_FIXED_POSITION)
		EwinInhSetUser(ewin, move, 1);
	     else
		EwinInhSetUser(ewin, move, 0);
	  }
	if (event->data.l[0] & WIN_STATE_ARRANGE_IGNORE)
	  {
	     if (event->data.l[1] & WIN_STATE_ARRANGE_IGNORE)
		ewin->props.ignorearrange = 1;
	     else
		ewin->props.ignorearrange = 0;
	  }
	if ((event->data.l[0] & WIN_STATE_STICKY)
	    && (!ewin->props.ignorearrange))
	  {
	     EwinOpStick(ewin, OPSRC_USER,
			 (event->data.l[1] & WIN_STATE_STICKY) != 0);
	  }
	if (event->data.l[0] & WIN_STATE_SHADED)
	  {
	     EwinOpShade(ewin, OPSRC_USER,
			 (event->data.l[1] & WIN_STATE_SHADED) != 0);
	  }
	HintsSetWindowState(ewin);
	return 1;
     }

   return 0;
}

int
GNOME_ProcessRootClientMessage(XClientMessageEvent * event)
{
   static Atom         a2 = 0, a3 = 0;

   if (!a2)
      a2 = EInternAtom("_WIN_AREA");
   if (!a3)
      a3 = EInternAtom("_WIN_WORKSPACE");

   if (event->message_type == a2)
     {
	DeskCurrentGotoArea(event->data.l[0], event->data.l[1]);
	return 1;
     }
   if (event->message_type == a3)
     {
	DeskGotoNum(event->data.l[0]);
	return 1;
     }

   return 0;
}
