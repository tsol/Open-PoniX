/*
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
/*
 * Stuff for compiling without ecore_x.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "config.h"
#include "e16-ecore_hints.h"

/* Window property change actions (must match _NET_WM_STATE_... ones) */
#define ECORE_X_PROP_LIST_REMOVE    0
#define ECORE_X_PROP_LIST_ADD       1
#define ECORE_X_PROP_LIST_TOGGLE    2

#ifdef USE_ECORE_X

extern Display     *_ecore_x_disp;

void
ecore_x_icccm_state_set_iconic(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_ICONIC);
}

void
ecore_x_icccm_state_set_normal(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_NORMAL);
}

void
ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, ECORE_X_WINDOW_STATE_HINT_WITHDRAWN);
}

#else /* USE_ECORE_X */

#define DEBUG_CHECK 0
#if DEBUG_CHECK
#include <assert.h>
#endif
#include "hints.h"
#include "xwin.h"

#define _ecore_x_disp disp

#define N_ITEMS(x) (sizeof(x)/sizeof(x[0]))

/*
 * General stuff
 */

/*
 * Send client message (format 32)
 */
int
ecore_x_client_message32_send(Ecore_X_Window win, Ecore_X_Atom type,
			      unsigned int mask,
			      unsigned int d0, unsigned int d1,
			      unsigned int d2, unsigned int d3, unsigned int d4)
{
   XEvent              xev;

   xev.xclient.type = ClientMessage;
   xev.xclient.window = win;
   xev.xclient.message_type = type;
   xev.xclient.format = 32;
   xev.xclient.data.l[0] = d0;
   xev.xclient.data.l[1] = d1;
   xev.xclient.data.l[2] = d2;
   xev.xclient.data.l[3] = d3;
   xev.xclient.data.l[4] = d4;

   return XSendEvent(_ecore_x_disp, win, False, mask, &xev);
}

/*
 * Set size 32 item (array) property
 */
static void
_ecore_x_window_prop32_set(Ecore_X_Window win, Ecore_X_Atom atom,
			   Ecore_X_Atom type, unsigned int *val, int num)
{
#if SIZEOF_INT == SIZEOF_LONG
   XChangeProperty(_ecore_x_disp, win, atom, type, 32, PropModeReplace,
		   (unsigned char *)val, num);
#else
   unsigned long      *pl;
   int                 i;

   pl = (unsigned long *)malloc(num * sizeof(long));
   if (!pl)
      return;
   for (i = 0; i < num; i++)
      pl[i] = val[i];
   XChangeProperty(_ecore_x_disp, win, atom, type, 32, PropModeReplace,
		   (unsigned char *)pl, num);
   free(pl);
#endif
}

/*
 * Get size 32 item (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
static int
_ecore_x_window_prop32_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				Ecore_X_Atom type, unsigned int **val, int num)
{
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;
   unsigned int       *lst;
   int                 i;

   prop_ret = NULL;
   if (XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
			  type, &type_ret, &format_ret, &num_ret,
			  &bytes_after, &prop_ret) != Success)
      return -1;

   if (type_ret != type || format_ret != 32)
     {
	num = -1;
     }
   else if (num_ret == 0 || !prop_ret)
     {
	num = 0;
     }
   else
     {
	if (num >= 0)
	  {
	     if ((int)num_ret < num)
		num = (int)num_ret;
	     lst = *val;
	  }
	else
	  {
	     num = (int)num_ret;
	     lst = (unsigned int *)malloc(num * sizeof(unsigned int));
	     *val = lst;
	     if (!lst)
		return 0;
	  }
	for (i = 0; i < num; i++)
	   lst[i] = ((unsigned long *)prop_ret)[i];
     }
   if (prop_ret)
      XFree(prop_ret);

   return num;
}

/*
 * Set CARD32 (array) property
 */
void
ecore_x_window_prop_card32_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       unsigned int *val, unsigned int num)
{
   _ecore_x_window_prop32_set(win, atom, XA_CARDINAL, val, (int)num);
}

/*
 * Get CARD32 (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_card32_get(Ecore_X_Window win, Ecore_X_Atom atom,
			       unsigned int *val, unsigned int len)
{
   return _ecore_x_window_prop32_list_get(win, atom, XA_CARDINAL,
					  &val, (int)len);
}

/*
 * Get CARD32 (array) property of any length
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_card32_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				    unsigned int **plst)
{
   return _ecore_x_window_prop32_list_get(win, atom, XA_CARDINAL, plst, -1);
}

#endif /* USE_ECORE_X */

/*
 * Set simple string list property
 */
void
ecore_x_window_prop_string_list_set(Ecore_X_Window win, Ecore_X_Atom atom,
				    char **lst, int num)
{
   XTextProperty       xtp;

   if (XmbTextListToTextProperty(_ecore_x_disp, lst, num,
				 XStdICCTextStyle, &xtp) != Success)
      return;
   XSetTextProperty(_ecore_x_disp, win, &xtp, atom);
   XFree(xtp.value);
}

/*
 * Get simple string list property
 *
 * If the property was successfully fetched the number of items stored in
 * lst is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_string_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				    char ***plst)
{
   char              **pstr = NULL;
   XTextProperty       xtp;
   int                 i, items;
   char              **list;
   Status              s;

   *plst = NULL;

   if (!XGetTextProperty(_ecore_x_disp, win, &xtp, atom))
      return -1;

   if (xtp.format == 8)
     {
	s = XmbTextPropertyToTextList(_ecore_x_disp, &xtp, &list, &items);
	if (s == Success)
	  {
	     if (items > 0)
	       {
		  pstr = (char **)malloc(items * sizeof(char *));
		  if (!pstr)
		     goto done;
		  for (i = 0; i < items; i++)
		     pstr[i] = (list[i] && (*list[i] || i < items - 1)) ?
			strdup(list[i]) : NULL;
	       }
	     if (list)
		XFreeStringList(list);
	     goto done;
	  }
     }

   /* Bad format or XmbTextPropertyToTextList failed - Now what? */
   items = 1;
   pstr = (char **)malloc(sizeof(char *));
   if (!pstr)
      goto done;
   pstr[0] = (xtp.value) ? strdup((char *)xtp.value) : NULL;

 done:
   XFree(xtp.value);

   *plst = pstr;
   if (!pstr)
      items = 0;
   return items;
}

#ifndef USE_ECORE_X

/*
 * Set simple string property
 */
void
ecore_x_window_prop_string_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       const char *str)
{
   ecore_x_window_prop_string_list_set(win, atom, (char **)(&str), 1);
}

/*
 * Get simple string property
 */
char               *
ecore_x_window_prop_string_get(Ecore_X_Window win, Ecore_X_Atom atom)
{
   XTextProperty       xtp;
   char               *str;
   int                 items;
   char              **list;
   Status              s;

   if (!XGetTextProperty(_ecore_x_disp, win, &xtp, atom))
      return NULL;

   if (xtp.format == 8)
     {
	s = XmbTextPropertyToTextList(_ecore_x_disp, &xtp, &list, &items);
	if ((s == Success) && (items > 0))
	  {
	     str = (*list) ? strdup(*list) : NULL;
	     XFreeStringList(list);
	  }
	else
	   str = (xtp.value) ? strdup((char *)xtp.value) : NULL;
     }
   else
      str = (xtp.value) ? strdup((char *)xtp.value) : NULL;

   XFree(xtp.value);

   return str;
}

/*
 * Set UTF-8 string property
 */
static void
_ecore_x_window_prop_string_utf8_set(Ecore_X_Window win, Ecore_X_Atom atom,
				     const char *str)
{
   XChangeProperty(_ecore_x_disp, win, atom, ECORE_X_ATOM_UTF8_STRING, 8,
		   PropModeReplace, (unsigned char *)str, strlen(str));
}

/*
 * Get UTF-8 string property
 */
static char        *
_ecore_x_window_prop_string_utf8_get(Ecore_X_Window win, Ecore_X_Atom atom)
{
   char               *str;
   unsigned char      *prop_ret;
   Atom                type_ret;
   unsigned long       bytes_after, num_ret;
   int                 format_ret;

   str = NULL;
   prop_ret = NULL;
   XGetWindowProperty(_ecore_x_disp, win, atom, 0, 0x7fffffff, False,
		      ECORE_X_ATOM_UTF8_STRING, &type_ret,
		      &format_ret, &num_ret, &bytes_after, &prop_ret);
   if (prop_ret && num_ret > 0 && format_ret == 8)
     {
	str = (char *)malloc(num_ret + 1);
	if (str)
	  {
	     memcpy(str, prop_ret, num_ret);
	     str[num_ret] = '\0';
	  }
     }
   if (prop_ret)
      XFree(prop_ret);

   return str;
}

/*
 * Set X ID (array) property
 */
void
ecore_x_window_prop_xid_set(Ecore_X_Window win, Ecore_X_Atom atom,
			    Ecore_X_Atom type, Ecore_X_ID * lst,
			    unsigned int num)
{
   _ecore_x_window_prop32_set(win, atom, type, lst, (int)num);
}

/*
 * Get X ID (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_xid_get(Ecore_X_Window win, Ecore_X_Atom atom,
			    Ecore_X_Atom type, Ecore_X_ID * lst,
			    unsigned int len)
{
   return _ecore_x_window_prop32_list_get(win, atom, type, &lst, (int)len);
}

/*
 * Get X ID (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_xid_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				 Ecore_X_Atom type, Ecore_X_ID ** val)
{
   return _ecore_x_window_prop32_list_get(win, atom, type, val, -1);
}

/*
 * Remove/add/toggle X ID list item.
 */
void
ecore_x_window_prop_xid_list_change(Ecore_X_Window win, Ecore_X_Atom atom,
				    Ecore_X_Atom type, Ecore_X_ID item, int op)
{
   Ecore_X_ID         *lst, *lst_r;
   int                 i, num;

   num = ecore_x_window_prop_xid_list_get(win, atom, type, &lst);
   if (num < 0)
      return;			/* Error - assuming invalid window */

   /* Is it there? */
   for (i = 0; i < num; i++)
     {
	if (lst[i] == item)
	   break;
     }

   if (i < num)
     {
	/* Was in list */
	if (op == ECORE_X_PROP_LIST_ADD)
	   goto done;
	/* Remove it */
	num--;
	for (; i < num; i++)
	   lst[i] = lst[i + 1];
     }
   else
     {
	/* Was not in list */
	if (op == ECORE_X_PROP_LIST_REMOVE)
	   goto done;
	/* Add it */
	num++;
	lst_r = (Ecore_X_ID *) realloc(lst, num * sizeof(Ecore_X_ID));
	if (!lst_r)
	   goto done;
	lst = lst_r;
	lst[i] = item;
     }

   ecore_x_window_prop_xid_set(win, atom, type, lst, num);

 done:
   if (lst)
      free(lst);
}

/*
 * Set Atom (array) property
 */
void
ecore_x_window_prop_atom_set(Ecore_X_Window win, Ecore_X_Atom atom,
			     Ecore_X_Atom * lst, unsigned int num)
{
   ecore_x_window_prop_xid_set(win, atom, XA_ATOM, lst, num);
}

/*
 * Get Atom (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_atom_get(Ecore_X_Window win, Ecore_X_Atom atom,
			     Ecore_X_Atom * lst, unsigned int len)
{
   return ecore_x_window_prop_xid_get(win, atom, XA_ATOM, lst, len);
}

/*
 * Get Atom (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_atom_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				  Ecore_X_Atom ** plst)
{
   return ecore_x_window_prop_xid_list_get(win, atom, XA_ATOM, plst);
}

/*
 * Remove/add/toggle atom list item.
 */
void
ecore_x_window_prop_atom_list_change(Ecore_X_Window win, Ecore_X_Atom atom,
				     Ecore_X_Atom item, int op)
{
   ecore_x_window_prop_xid_list_change(win, atom, XA_ATOM, item, op);
}

/*
 * Set Window (array) property
 */
void
ecore_x_window_prop_window_set(Ecore_X_Window win, Ecore_X_Atom atom,
			       Ecore_X_Window * lst, unsigned int num)
{
   ecore_x_window_prop_xid_set(win, atom, XA_WINDOW, lst, num);
}

/*
 * Get Window (array) property
 *
 * At most len items are returned in val.
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_window_get(Ecore_X_Window win, Ecore_X_Atom atom,
			       Ecore_X_Window * lst, unsigned int len)
{
   return ecore_x_window_prop_xid_get(win, atom, XA_WINDOW, lst, len);
}

/*
 * Get Window (array) property
 *
 * If the property was successfully fetched the number of items stored in
 * val is returned, otherwise -1 is returned.
 * The returned array must be freed with free().
 * Note: Return value 0 means that the property exists but has no elements.
 */
int
ecore_x_window_prop_window_list_get(Ecore_X_Window win, Ecore_X_Atom atom,
				    Ecore_X_Window ** plst)
{
   return ecore_x_window_prop_xid_list_get(win, atom, XA_WINDOW, plst);
}

#endif /* USE_ECORE_X */

#ifndef USE_ECORE_X
/*
 * ICCCM stuff
 */

static const char  *const atoms_icccm_names[] = {
/* *INDENT-OFF* */

   /* ICCCM */
   "WM_STATE",
   "WM_WINDOW_ROLE",
   "WM_CLIENT_LEADER",
   "WM_COLORMAP_WINDOWS",
   "WM_CHANGE_STATE",
   "WM_PROTOCOLS",
   "WM_DELETE_WINDOW",
   "WM_TAKE_FOCUS",
#if 0
   "WM_SAVE_YOURSELF",
#endif

   /* Misc. */
   "UTF8_STRING",

/* *INDENT-ON* */
};
unsigned int        atoms_icccm[CHECK_COUNT_ICCCM];

void
ecore_x_icccm_init(void)
{
#if DEBUG_CHECK
   assert(CHECK_COUNT_ICCCM == N_ITEMS(atoms_icccm));
#endif
   AtomListIntern(atoms_icccm_names, N_ITEMS(atoms_icccm), atoms_icccm);
}

static void
ecore_x_icccm_state_set(Ecore_X_Window win, unsigned int state)
{
   unsigned long       c[2];

   c[0] = state;
   c[1] = 0;
   XChangeProperty(_ecore_x_disp, win, ECORE_X_ATOM_WM_STATE,
		   ECORE_X_ATOM_WM_STATE, 32, PropModeReplace,
		   (unsigned char *)c, 2);
}

void
ecore_x_icccm_state_set_iconic(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, IconicState);
}

void
ecore_x_icccm_state_set_normal(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, NormalState);
}

void
ecore_x_icccm_state_set_withdrawn(Ecore_X_Window win)
{
   ecore_x_icccm_state_set(win, WithdrawnState);
}

static void
ecore_x_icccm_client_message_send(Ecore_X_Window win,
				  Ecore_X_Atom atom, Ecore_X_Time ts)
{
   ecore_x_client_message32_send(win, ECORE_X_ATOM_WM_PROTOCOLS, NoEventMask,
				 atom, ts, 0, 0, 0);
}

void
ecore_x_icccm_delete_window_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_DELETE_WINDOW, ts);
}

void
ecore_x_icccm_take_focus_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_TAKE_FOCUS, ts);
}

#if 0
void
ecore_x_icccm_save_yourself_send(Ecore_X_Window win, Ecore_X_Time ts)
{
   ecore_x_icccm_client_message_send(win, ECORE_X_ATOM_WM_SAVE_YOURSELF, ts);
}
#endif

void
ecore_x_icccm_title_set(Ecore_X_Window win, const char *title)
{
   ecore_x_window_prop_string_set(win, ECORE_X_ATOM_WM_NAME, title);
}

char               *
ecore_x_icccm_title_get(Ecore_X_Window win)
{
   return ecore_x_window_prop_string_get(win, ECORE_X_ATOM_WM_NAME);
}

void
ecore_x_icccm_name_class_set(Ecore_X_Window win, const char *name,
			     const char *clss)
{
   XClassHint         *xch;

   xch = XAllocClassHint();
   if (!xch)
      return;
   xch->res_name = (char *)name;
   xch->res_class = (char *)clss;
   XSetClassHint(_ecore_x_disp, win, xch);
   XFree(xch);
}

void
ecore_x_icccm_name_class_get(Ecore_X_Window win, char **name, char **clss)
{
   XClassHint          xch;

   *name = *clss = NULL;
   xch.res_name = NULL;
   xch.res_class = NULL;
   if (XGetClassHint(_ecore_x_disp, win, &xch))
     {
	if (name && xch.res_name)
	   *name = strdup(xch.res_name);
	if (clss && xch.res_class)
	   *clss = strdup(xch.res_class);
	XFree(xch.res_name);
	XFree(xch.res_class);
     }
}

#endif /* USE_ECORE_X */

#ifndef USE_ECORE_X
/*
 * _NET_WM hints (EWMH)
 */

static const char  *const atoms_netwm_names[] = {
/* *INDENT-OFF* */

   /* Window manager info */
   "_NET_SUPPORTED",
   "_NET_SUPPORTING_WM_CHECK",

   /* Desktop status/requests */
   "_NET_NUMBER_OF_DESKTOPS",
   "_NET_VIRTUAL_ROOTS",
   "_NET_DESKTOP_GEOMETRY",
   "_NET_DESKTOP_NAMES",
   "_NET_DESKTOP_VIEWPORT",
   "_NET_WORKAREA",
   "_NET_CURRENT_DESKTOP",
   "_NET_SHOWING_DESKTOP",

   "_NET_ACTIVE_WINDOW",
   "_NET_CLIENT_LIST",
   "_NET_CLIENT_LIST_STACKING",

   /* Client window props/client messages */
   "_NET_WM_NAME",
   "_NET_WM_VISIBLE_NAME",
   "_NET_WM_ICON_NAME",
   "_NET_WM_VISIBLE_ICON_NAME",

   "_NET_WM_DESKTOP",

   "_NET_WM_WINDOW_TYPE",
   "_NET_WM_WINDOW_TYPE_DESKTOP",
   "_NET_WM_WINDOW_TYPE_DOCK",
   "_NET_WM_WINDOW_TYPE_TOOLBAR",
   "_NET_WM_WINDOW_TYPE_MENU",
   "_NET_WM_WINDOW_TYPE_UTILITY",
   "_NET_WM_WINDOW_TYPE_SPLASH",
   "_NET_WM_WINDOW_TYPE_DIALOG",
   "_NET_WM_WINDOW_TYPE_NORMAL",

   "_NET_WM_STATE",
   "_NET_WM_STATE_MODAL",
   "_NET_WM_STATE_STICKY",
   "_NET_WM_STATE_MAXIMIZED_VERT",
   "_NET_WM_STATE_MAXIMIZED_HORZ",
   "_NET_WM_STATE_SHADED",
   "_NET_WM_STATE_SKIP_TASKBAR",
   "_NET_WM_STATE_SKIP_PAGER",
   "_NET_WM_STATE_HIDDEN",
   "_NET_WM_STATE_FULLSCREEN",
   "_NET_WM_STATE_ABOVE",
   "_NET_WM_STATE_BELOW",
   "_NET_WM_STATE_DEMANDS_ATTENTION",

   "_NET_WM_ALLOWED_ACTIONS",
   "_NET_WM_ACTION_MOVE",
   "_NET_WM_ACTION_RESIZE",
   "_NET_WM_ACTION_MINIMIZE",
   "_NET_WM_ACTION_SHADE",
   "_NET_WM_ACTION_STICK",
   "_NET_WM_ACTION_MAXIMIZE_HORZ",
   "_NET_WM_ACTION_MAXIMIZE_VERT",
   "_NET_WM_ACTION_FULLSCREEN",
   "_NET_WM_ACTION_CHANGE_DESKTOP",
   "_NET_WM_ACTION_CLOSE",
   "_NET_WM_ACTION_ABOVE",
   "_NET_WM_ACTION_BELOW",

   "_NET_WM_STRUT",
   "_NET_WM_STRUT_PARTIAL",

   "_NET_FRAME_EXTENTS",

   "_NET_WM_ICON",

   "_NET_WM_USER_TIME",
   "_NET_WM_USER_TIME_WINDOW",

#if 0				/* Not used */
   "_NET_WM_ICON_GEOMETRY",
   "_NET_WM_PID",
   "_NET_WM_HANDLED_ICONS",

   "_NET_WM_PING",
#endif
   "_NET_WM_SYNC_REQUEST",
   "_NET_WM_SYNC_REQUEST_COUNTER",

   "_NET_WM_WINDOW_OPACITY",

   /* Misc window ops */
   "_NET_CLOSE_WINDOW",
   "_NET_MOVERESIZE_WINDOW",
   "_NET_WM_MOVERESIZE",
   "_NET_RESTACK_WINDOW",

#if 0				/* Not yet implemented */
   "_NET_REQUEST_FRAME_EXTENTS",
#endif

   /* Startup notification */
   "_NET_STARTUP_ID",
   "_NET_STARTUP_INFO_BEGIN",
   "_NET_STARTUP_INFO",

/* *INDENT-ON* */
};
unsigned int        atoms_netwm[CHECK_COUNT_NETWM];

void
ecore_x_netwm_init(void)
{
#if DEBUG_CHECK
   assert(CHECK_COUNT_NETWM == N_ITEMS(atoms_netwm));
#endif
   AtomListIntern(atoms_netwm_names, N_ITEMS(atoms_netwm), atoms_netwm);
}

/*
 * WM identification
 */
void
ecore_x_netwm_wm_identify(Ecore_X_Window root, Ecore_X_Window check,
			  const char *wm_name)
{
   ecore_x_window_prop_window_set(root, ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK,
				  &check, 1);
   ecore_x_window_prop_window_set(check, ECORE_X_ATOM_NET_SUPPORTING_WM_CHECK,
				  &check, 1);
   _ecore_x_window_prop_string_utf8_set(check, ECORE_X_ATOM_NET_WM_NAME,
					wm_name);
   /* This one isn't mandatory */
   _ecore_x_window_prop_string_utf8_set(root, ECORE_X_ATOM_NET_WM_NAME,
					wm_name);
}

/*
 * Desktop configuration and status
 */

void
ecore_x_netwm_desk_count_set(Ecore_X_Window root, unsigned int n_desks)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_NUMBER_OF_DESKTOPS,
				  &n_desks, 1);
}

void
ecore_x_netwm_desk_roots_set(Ecore_X_Window root, Ecore_X_Window * vroots,
			     unsigned int n_desks)
{
   ecore_x_window_prop_window_set(root, ECORE_X_ATOM_NET_VIRTUAL_ROOTS, vroots,
				  n_desks);
}

void
ecore_x_netwm_desk_names_set(Ecore_X_Window root, const char **names,
			     unsigned int n_desks)
{
   char                ss[32], *buf, *buf_r;
   const char         *s;
   unsigned int        i;
   int                 l, len;

   buf = NULL;
   len = 0;

   for (i = 0; i < n_desks; i++)
     {
	s = (names) ? names[i] : NULL;
	if (!s)
	  {
	     /* Default to "Desk-<number>" */
	     sprintf(ss, "Desk-%d", i);
	     s = ss;
	  }

	l = strlen(s) + 1;
	buf_r = (char *)realloc(buf, len + l);
	if (!buf_r)
	   goto done;
	buf = buf_r;
	memcpy(buf + len, s, l);
	len += l;
     }

   XChangeProperty(_ecore_x_disp, root, ECORE_X_ATOM_NET_DESKTOP_NAMES,
		   ECORE_X_ATOM_UTF8_STRING, 8, PropModeReplace,
		   (unsigned char *)buf, len);

 done:
   free(buf);
}

void
ecore_x_netwm_desk_size_set(Ecore_X_Window root, unsigned int width,
			    unsigned int height)
{
   unsigned int        size[2];

   size[0] = width;
   size[1] = height;
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_DESKTOP_GEOMETRY, size,
				  2);
}

void
ecore_x_netwm_desk_workareas_set(Ecore_X_Window root, unsigned int *areas,
				 unsigned int n_desks)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_WORKAREA, areas,
				  4 * n_desks);
}

void
ecore_x_netwm_desk_current_set(Ecore_X_Window root, unsigned int desk)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_CURRENT_DESKTOP, &desk,
				  1);
}

void
ecore_x_netwm_desk_viewports_set(Ecore_X_Window root, unsigned int *origins,
				 unsigned int n_desks)
{
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_DESKTOP_VIEWPORT,
				  origins, 2 * n_desks);
}

void
ecore_x_netwm_showing_desktop_set(Ecore_X_Window root, int on)
{
   unsigned int        val;

   val = (on) ? 1 : 0;
   ecore_x_window_prop_card32_set(root, ECORE_X_ATOM_NET_SHOWING_DESKTOP, &val,
				  1);
}

/*
 * Client status
 */

/* Mapping order */
void
ecore_x_netwm_client_list_set(Ecore_X_Window root, Ecore_X_Window * p_clients,
			      unsigned int n_clients)
{
   ecore_x_window_prop_window_set(root, ECORE_X_ATOM_NET_CLIENT_LIST, p_clients,
				  n_clients);
}

/* Stacking order */
void
ecore_x_netwm_client_list_stacking_set(Ecore_X_Window root,
				       Ecore_X_Window * p_clients,
				       unsigned int n_clients)
{
   ecore_x_window_prop_window_set(root, ECORE_X_ATOM_NET_CLIENT_LIST_STACKING,
				  p_clients, n_clients);
}

void
ecore_x_netwm_client_active_set(Ecore_X_Window root, Ecore_X_Window win)
{
   ecore_x_window_prop_window_set(root, ECORE_X_ATOM_NET_ACTIVE_WINDOW, &win,
				  1);
}

/*
 * Client window properties
 */

void
ecore_x_netwm_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_NAME, name);
}

int
ecore_x_netwm_name_get(Ecore_X_Window win, char **name)
{
   char               *s;

   s = _ecore_x_window_prop_string_utf8_get(win, ECORE_X_ATOM_NET_WM_NAME);
   *name = s;

   return ! !s;
}

void
ecore_x_netwm_visible_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_VISIBLE_NAME,
					name);
}

int
ecore_x_netwm_visible_name_get(Ecore_X_Window win, char **name)
{
   char               *s;

   s = _ecore_x_window_prop_string_utf8_get(win,
					    ECORE_X_ATOM_NET_WM_VISIBLE_NAME);
   *name = s;

   return ! !s;
}

void
ecore_x_netwm_icon_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_WM_ICON_NAME,
					name);
}

int
ecore_x_netwm_icon_name_get(Ecore_X_Window win, char **name)
{
   char               *s;

   s = _ecore_x_window_prop_string_utf8_get(win, ECORE_X_ATOM_NET_WM_ICON_NAME);
   *name = s;

   return ! !s;
}

void
ecore_x_netwm_visible_icon_name_set(Ecore_X_Window win, const char *name)
{
   _ecore_x_window_prop_string_utf8_set(win,
					ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME,
					name);
}

int
ecore_x_netwm_visible_icon_name_get(Ecore_X_Window win, char **name)
{
   char               *s;

   s = _ecore_x_window_prop_string_utf8_get(win,
					    ECORE_X_ATOM_NET_WM_VISIBLE_ICON_NAME);
   *name = s;

   return ! !s;
}

void
ecore_x_netwm_desktop_set(Ecore_X_Window win, unsigned int desk)
{
   ecore_x_window_prop_card32_set(win, ECORE_X_ATOM_NET_WM_DESKTOP, &desk, 1);
}

int
ecore_x_netwm_desktop_get(Ecore_X_Window win, unsigned int *desk)
{
   return ecore_x_window_prop_card32_get(win, ECORE_X_ATOM_NET_WM_DESKTOP,
					 desk, 1);
}

int
ecore_x_netwm_user_time_get(Ecore_X_Window win, unsigned int *ts)
{
   return ecore_x_window_prop_card32_get(win, ECORE_X_ATOM_NET_WM_USER_TIME,
					 ts, 1);
}

void
ecore_x_netwm_opacity_set(Ecore_X_Window win, unsigned int opacity)
{
   ecore_x_window_prop_card32_set(win, ECORE_X_ATOM_NET_WM_WINDOW_OPACITY,
				  &opacity, 1);
}

int
ecore_x_netwm_opacity_get(Ecore_X_Window win, unsigned int *opacity)
{
   return ecore_x_window_prop_card32_get(win,
					 ECORE_X_ATOM_NET_WM_WINDOW_OPACITY,
					 opacity, 1);
}

#if 0				/* Not used */
void
ecore_x_netwm_startup_id_set(Ecore_X_Window win, const char *id)
{
   _ecore_x_window_prop_string_utf8_set(win, ECORE_X_ATOM_NET_STARTUP_ID, id);
}
#endif

int
ecore_x_netwm_startup_id_get(Ecore_X_Window win, char **id)
{
   char               *s;

   s = _ecore_x_window_prop_string_utf8_get(win, ECORE_X_ATOM_NET_STARTUP_ID);
   *id = s;

   return ! !s;
}

#endif /* USE_ECORE_X */
