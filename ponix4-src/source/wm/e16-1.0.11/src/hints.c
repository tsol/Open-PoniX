/*
 * Copyright (C) 2003-2011 Kim Woelders
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
/*
 * Feeble attempt to collect hint stuff in one place
 */
#include "E.h"
#include "borders.h"		/* FIXME - Should not be here */
#include "desktops.h"		/* FIXME - Should not be here */
#include "e16-ecore_hints.h"
#include "events.h"
#include "ewins.h"
#include "hints.h"
#include "xwin.h"
#include <X11/Xatom.h>

static void         EHintsSetDeskInfo(void);
static void         EHintsSetAreaInfo(void);

#define N_ITEMS(x) (sizeof(x)/sizeof(x[0]))

static const char  *const atoms_misc_names[] = {
   /* Misc atoms */
   "MANAGER",

   /* Root background atoms */
   "_XROOTPMAP_ID",
   "_XROOTCOLOR_PIXEL",

   /* E16 atoms */
   "ENLIGHTENMENT_VERSION",

   "ENLIGHTENMENT_COMMS",
   "ENL_MSG",

   "ENL_INTERNAL_AREA_DATA",
   "ENL_INTERNAL_DESK_DATA",
   "ENL_WIN_DATA",
   "ENL_WIN_BORDER"
};

unsigned int        atoms_misc[10];

static unsigned int desk_info = 0;

void
AtomListIntern(const char *const *names, unsigned int num, unsigned int *atoms)
{
#if SIZEOF_INT == SIZEOF_LONG
   XInternAtoms(disp, (char **)names, num, False, (Atom *) atoms);
#else
   unsigned int        i;
   Atom               *_atoms;

   _atoms = EMALLOC(Atom, num);
   if (!_atoms)
      return;

   XInternAtoms(disp, (char **)names, num, False, _atoms);
   for (i = 0; i < num; i++)
      atoms[i] = _atoms[i];

   Efree(_atoms);
#endif
}

void
HintsInit(void)
{
   Window              win;

   AtomListIntern(atoms_misc_names, N_ITEMS(atoms_misc_names), atoms_misc);

   win = XCreateSimpleWindow(disp, WinGetXwin(VROOT), -200, -200, 5, 5,
			     0, 0, 0);

   ICCCM_Init();
   MWM_SetInfo();
#if ENABLE_GNOME
   GNOME_SetHints(win);
#endif
   EWMH_Init(win);

   ecore_x_window_prop_string_set(WinGetXwin(VROOT), E16_ATOM_VERSION,
				  e_wm_version);

   if (Mode.wm.window)
     {
	HintsSetWindowName(VROOT, "Enlightenment");
	HintsSetWindowClass(VROOT, "Virtual-Root", "Enlightenment");
     }

   Mode.root.ext_pmap = HintsGetRootPixmap(VROOT);
   Mode.root.ext_pmap_valid = EDrawableCheck(Mode.root.ext_pmap, 0);
}

/*
 * Functions that set X11-properties from E-internals
 */

void
HintsSetRootHints(Win win __UNUSED__)
{
   /* Nothing done here for now */
}

void
HintsSetClientList(void)
{
#if ENABLE_GNOME
   GNOME_SetClientList();
#endif
   EWMH_SetClientList();
   EWMH_SetClientStacking();
}

void
HintsSetClientStacking(void)
{
   EWMH_SetClientStacking();
}

void
HintsSetDesktopConfig(void)
{
#if ENABLE_GNOME
   GNOME_SetDeskCount();
   GNOME_SetDeskNames();
#endif
   EWMH_SetDesktopCount();
   EWMH_SetDesktopRoots();
   EWMH_SetDesktopNames();
   EWMH_SetWorkArea();
}

void
HintsSetViewportConfig(void)
{
#if ENABLE_GNOME
   GNOME_SetAreaCount();
#endif
   EWMH_SetDesktopSize();
}

void
HintsSetCurrentDesktop(void)
{
#if ENABLE_GNOME
   GNOME_SetCurrentDesk();
#endif
   EWMH_SetCurrentDesktop();
   EHintsSetDeskInfo();
}

void
HintsSetDesktopViewport(void)
{
#if ENABLE_GNOME
   GNOME_SetCurrentArea();
#endif
   EWMH_SetDesktopViewport();
   EHintsSetAreaInfo();
}

void
HintsSetActiveWindow(Window win)
{
   EWMH_SetActiveWindow(win);
}

void
HintsSetWindowName(Win win, const char *name)
{
   if (!name)
      return;

   ecore_x_icccm_title_set(WinGetXwin(win), name);

   EWMH_SetWindowName(WinGetXwin(win), name);
}

void
HintsSetWindowClass(Win win, const char *name, const char *clss)
{
   if (!name)
      name = "NoName";
   if (!clss)
      clss = "NoClass";

   ecore_x_icccm_name_class_set(WinGetXwin(win), name, clss);
}

void
HintsSetWindowDesktop(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_SetEwinDesk(ewin);
#endif
   EWMH_SetWindowDesktop(ewin);
}

void
HintsSetWindowArea(const EWin * ewin __UNUSED__)
{
#if ENABLE_GNOME
   GNOME_SetEwinArea(ewin);
#endif
}

void
HintsSetWindowState(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_SetHint(ewin);
#endif
   EWMH_SetWindowState(ewin);
   EWMH_SetWindowActions(ewin);
}

void
HintsSetWindowOpacity(const EWin * ewin)
{
   EWMH_SetWindowOpacity(ewin);
}

void
HintsSetWindowBorder(const EWin * ewin)
{
   EWMH_SetWindowBorder(ewin);
}

/*
 * Functions that set E-internals from X11-properties
 */

void
HintsGetWindowHints(EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_GetHints(ewin, 0);
#endif
   EWMH_GetWindowHints(ewin);
}

/*
 * Functions that delete X11-properties
 */

void
HintsDelWindowHints(const EWin * ewin)
{
#if ENABLE_GNOME
   GNOME_DelHints(ewin);
#endif
   EWMH_DelWindowHints(ewin);
}

/*
 * Functions processing received X11 messages
 */

void
HintsProcessPropertyChange(EWin * ewin, XEvent * ev)
{
   if (ICCCM_ProcessPropertyChange(ewin, ev->xproperty.atom))
      return;
   if (EWMH_ProcessPropertyChange(ewin, ev->xproperty.atom))
      return;
#if 0				/* No! - ENABLE_GNOME */
   if (GNOME_GetHints(ewin, ev->xproperty.atom))
      return;
#endif
#if 0
   if (EDebug(1))
     {
	Eprintf("HintsProcessPropertyChange:\n");
	EventShow(ev);
     }
#endif
}

void
HintsProcessClientClientMessage(EWin * ewin, XClientMessageEvent * event)
{
   if (ICCCM_ProcessClientClientMessage(ewin, event))
      return;
   if (EWMH_ProcessClientClientMessage(ewin, event))
      return;
#if ENABLE_GNOME
   if (GNOME_ProcessClientClientMessage(ewin, event))
      return;
#endif
   if (EDebug(1))
     {
	Eprintf("HintsProcessClientClientMessage:\n");
	EventShow((XEvent *) event);
     }
}

void
HintsProcessRootClientMessage(XClientMessageEvent * event)
{
   if (ICCCM_ProcessRootClientMessage(event))
      return;
   if (EWMH_ProcessRootClientMessage(event))
      return;
#if ENABLE_GNOME
   if (GNOME_ProcessRootClientMessage(event))
      return;
#endif
   if (EDebug(1))
     {
	Eprintf("HintsProcessRootClientMessage:\n");
	EventShow((XEvent *) event);
     }
}

Pixmap
HintsGetRootPixmap(Win win)
{
   Ecore_X_Pixmap      pm;

   pm = None;
   ecore_x_window_prop_xid_get(WinGetXwin(win), E_XROOTPMAP_ID, XA_PIXMAP,
			       &pm, 1);

   return pm;
}

void
HintsSetRootInfo(Win win, Pixmap pmap, unsigned int color)
{
   Ecore_X_Pixmap      pm;

   pm = pmap;
   ecore_x_window_prop_xid_set(WinGetXwin(win), E_XROOTPMAP_ID, XA_PIXMAP, &pm,
			       1);

   ecore_x_window_prop_card32_set(WinGetXwin(win), E_XROOTCOLOR_PIXEL, &color,
				  1);
}

typedef union {
   struct {
      unsigned            version:8;
      unsigned            rsvd:22;
      unsigned            docked:1;
      unsigned            iconified:1;
   } b;
   int                 all:32;
} EWinInfoFlags;

#define ENL_DATA_ITEMS      12
#define ENL_DATA_VERSION     0

void
EHintsSetInfo(const EWin * ewin)
{
   int                 c[ENL_DATA_ITEMS];
   unsigned int        flags[2];
   EWinInfoFlags       f;

   if (EwinIsInternal(ewin))
      return;

   f.all = 0;
   f.b.version = ENL_DATA_VERSION;
   f.b.docked = ewin->state.docked;
   f.b.iconified = ewin->state.iconified;
   c[0] = f.all;

   EwinFlagsEncode(ewin, flags);
   c[1] = flags[0];
   c[2] = flags[1];

   c[3] = ewin->save_max.x;
   c[4] = ewin->save_max.y;
   c[5] = ewin->save_max.w;
   c[6] = ewin->save_max.h;
   c[7] = ewin->save_fs.x;
   c[8] = ewin->save_fs.y;
   c[9] = ewin->save_fs.w;
   c[10] = ewin->save_fs.h;
   c[11] = ewin->save_fs.layer;

   ecore_x_window_prop_card32_set(EwinGetClientXwin(ewin), E16_ATOM_WIN_DATA,
				  (unsigned int *)c, ENL_DATA_ITEMS);

   ecore_x_window_prop_string_set(EwinGetClientXwin(ewin), E16_ATOM_WIN_BORDER,
				  ewin->normal_border->name);

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap set einf  %#lx: %4d+%4d %4dx%4d: %s\n",
	      EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetTitle(ewin));
}

void
EHintsGetInfo(EWin * ewin)
{
   char               *str;
   int                 num;
   int                 c[ENL_DATA_ITEMS + 1];
   unsigned int        flags[2];
   EWinInfoFlags       f;

   if (EwinIsInternal(ewin))
      return;

   /* Fix window position on hidden desks if restarting after a crash */
   if (desk_info & (1U << EoGetDeskNum(ewin)))
      ewin->client.x -= WinGetW(VROOT);

   num =
      ecore_x_window_prop_card32_get(EwinGetClientXwin(ewin), E16_ATOM_WIN_DATA,
				     (unsigned int *)c, ENL_DATA_ITEMS + 1);
   if (num < 0)
      return;

   ewin->state.identified = 1;
   ewin->state.placed = 1;

   if (num < 2)
      return;

   f.all = c[0];
   if (f.b.version != ENL_DATA_VERSION)
      return;
   ewin->icccm.start_iconified = f.b.iconified;
   ewin->state.docked = f.b.docked;

   flags[0] = c[1];
   flags[1] = c[2];
   EwinFlagsDecode(ewin, flags);

   if (num == ENL_DATA_ITEMS)
     {
	ewin->save_max.x = c[3];
	ewin->save_max.y = c[4];
	ewin->save_max.w = c[5];
	ewin->save_max.h = c[6];
	ewin->save_fs.x = c[7];
	ewin->save_fs.y = c[8];
	ewin->save_fs.w = c[9];
	ewin->save_fs.h = c[10];
	ewin->save_fs.layer = c[11];
     }

   str =
      ecore_x_window_prop_string_get(EwinGetClientXwin(ewin),
				     E16_ATOM_WIN_BORDER);
   if (str)
      EwinBorderSetInitially(ewin, str);
   Efree(str);

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get einf  %#lx: %4d+%4d %4dx%4d: %s\n",
	      EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetTitle(ewin));
}

static void
EHintsSetDeskInfo(void)
{
   int                 i, ax, ay, n_desks;
   unsigned int        c[2], desk_pos_info;

   if (!DesksGetCurrent())	/* Quit if current desk isn't assigned yet */
      return;

   n_desks = DesksGetNumber();
   if (n_desks <= 0)
      return;

   desk_pos_info = 0;
   for (i = 0; i < n_desks; i++)
     {
	Desk               *dsk = DeskGet(i);

	DeskGetArea(dsk, &ax, &ay);
	if (EoGetY(dsk) == 0 && EoGetX(dsk) == WinGetW(VROOT))
	   desk_pos_info |= 1U << i;	/* Desk is "hidden" */
     }

   c[0] = DesksGetCurrentNum();
   c[1] = Mode.wm.exiting ? 0 : desk_pos_info;
   ecore_x_window_prop_card32_set(WinGetXwin(VROOT),
				  E16_ATOM_INTERNAL_DESK_DATA, c, 2);

   if (Mode.wm.exiting && Mode.root.ext_pmap_valid)
     {
	HintsSetRootInfo(VROOT, Mode.root.ext_pmap, 0);
	ESetWindowBackgroundPixmap(VROOT, Mode.root.ext_pmap);
     }
}

static void
EHintsSetAreaInfo(void)
{
   int                 i, ax, ay, n_desks;
   unsigned int       *c;

   n_desks = DesksGetNumber();
   if (n_desks <= 0)
      return;

   c = EMALLOC(unsigned int, 2 * n_desks);
   if (!c)
      return;

   for (i = 0; i < n_desks; i++)
     {
	Desk               *dsk = DeskGet(i);

	DeskGetArea(dsk, &ax, &ay);
	c[(i * 2)] = ax;
	c[(i * 2) + 1] = ay;
     }

   ecore_x_window_prop_card32_set(WinGetXwin(VROOT),
				  E16_ATOM_INTERNAL_AREA_DATA, c, 2 * n_desks);

   Efree(c);
}

void
EHintsGetDeskInfo(void)
{
   unsigned int       *c;
   int                 num, i, n_desks;

   n_desks = DesksGetNumber();
   c = EMALLOC(unsigned int, 2 * n_desks);

   if (!c)
      return;

   num = ecore_x_window_prop_card32_get(WinGetXwin(VROOT),
					E16_ATOM_INTERNAL_AREA_DATA,
					c, 2 * n_desks);
   if (num > 0)
     {
	for (i = 0; i < (num / 2); i++)
	   DeskSetArea(DeskGet(i), c[(i * 2)], c[(i * 2) + 1]);
     }

   num = ecore_x_window_prop_card32_get(WinGetXwin(VROOT),
					E16_ATOM_INTERNAL_DESK_DATA, c, 2);
   if (num > 0)
     {
	DesksSetCurrent(DeskGet(c[0]));
	if (num > 1)
	   desk_info = c[1];
     }
   else
     {
	/* Used to test if we should run cmd_init */
	Mode.wm.session_start = 1;
     }

   Efree(c);
}

void
EHintsSetInfoOnAll(void)
{
   int                 i, num;
   EWin               *const *lst;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("SetEInfoOnAll\n");

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
      if (!EwinIsInternal(lst[i]))
	 EHintsSetInfo(lst[i]);

   EHintsSetDeskInfo();
}

/*
 * Selections.
 */

struct _selection {
   Atom                atom;
   Time                time;
   Win                 win;
   EventCallbackFunc  *func;
   void               *data;
};

ESelection         *
SelectionAcquire(const char *name, EventCallbackFunc * func, void *data)
{
   ESelection         *sel;
   char                buf[128];

   sel = ECALLOC(ESelection, 1);
   if (!sel)
      return sel;

   Esnprintf(buf, sizeof(buf), "%s%d", name, Dpy.screen);

   sel->atom = EInternAtom(buf);
   sel->time = EGetTimestamp();
   sel->win = ECreateEventWindow(VROOT, -100, -100, 1, 1);

   sel->func = func;
   sel->data = data;

   XSetSelectionOwner(disp, sel->atom, WinGetXwin(sel->win), sel->time);
   if (XGetSelectionOwner(disp, sel->atom) != WinGetXwin(sel->win))
     {
	DialogOK(_("Selection Error!"), _("Could not acquire selection: %s"),
		 buf);
	EDestroyWindow(sel->win);
	Efree(sel);
	return NULL;
     }

   if (sel->func)
     {
	ESelectInput(sel->win, SubstructureNotifyMask);
	EventCallbackRegister(sel->win, sel->func, sel->data);
     }

   ecore_x_client_message32_send(WinGetXwin(VROOT), E_XA_MANAGER,
				 StructureNotifyMask, CurrentTime, sel->atom,
				 WinGetXwin(sel->win), 0, 0);

   if (EDebug(EDBUG_TYPE_SELECTION))
      Eprintf("Window %#lx is now %s owner, time=%lu\n",
	      WinGetXwin(sel->win), buf, sel->time);

   return sel;
}

void
SelectionRelease(ESelection * sel)
{
   if (!sel)
      return;

   if (EDebug(EDBUG_TYPE_SELECTION))
      Eprintf("Window %#lx is no longer %s owner\n",
	      WinGetXwin(sel->win), XGetAtomName(disp, sel->atom));

   XSetSelectionOwner(disp, sel->atom, None, sel->time);
   if (sel->func)
     {
	EventCallbackUnregister(sel->win, sel->func, sel->data);
     }
   EDestroyWindow(sel->win);
   Efree(sel);
}
