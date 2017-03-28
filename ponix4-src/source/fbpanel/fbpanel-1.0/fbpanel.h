/* fbpanel.h -- global definitions and vars are here

   Copyright (C) 2002 Anatoly Asviyan <aanatoly@users.sf.net>

   This file is part of fbpanel.

   fbpanel is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   fbpanel is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with sawfish; see the file COPYING.   If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */


#ifndef FBPANEL_H
#define FBPANEL_H

#include <X11/Xatom.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>

#include <stdio.h>


#define MWM_HINTS_DECORATIONS         (1L << 1)
typedef struct _mwmhints
{
    unsigned long flags;
    unsigned long functions;
    unsigned long decorations;
    long inputMode;
    unsigned long status;
}
MWMHints;


extern Window topxwin;
extern GtkWidget *window;
extern Atom atoms[];

#define atom_KWM_WIN_ICON    atoms[0]
#define atom__MOTIF_WM_HINTS atoms[1]
#define atom__WIN_HINTS      atoms[2]
#define atom__WIN_LAYER      atoms[3]
#define atom__WIN_STATE      atoms[4]
#define atom_WM_STATE        atoms[5]
#define atpm_WM_CLASS        atoms[6]

#define atom__NET_CLIENT_LIST         atoms[7]
#define atom__NET_NUMBER_OF_DESKTOPS  atoms[8]
#define atom__NET_CURRENT_DESKTOP     atoms[9]
#define atom__NET_DESKTOP_NAMES       atoms[10]
#define atom__NET_ACTIVE_WINDOW       atoms[11]


#define atom__NET_WM_STATE              atoms[12]
#define atom__NET_WM_STATE_SKIP_TASKBAR atoms[13]
#define atom__NET_WM_STATE_SKIP_PAGER   atoms[14]
#define atom__NET_WM_STATE_STICKY       atoms[15]
#define atom__NET_WM_STATE_HIDDEN       atoms[16]
#define atom__NET_WM_DESKTOP            atoms[17]
#define atom__NET_WM_NAME               atoms[18]
#define atom__NET_WM_ALLOWED_ACTIONS    atoms[19]
#define atom__NET_WM_ACTION_CLOSE       atoms[20]
#define atom__WIN_WORKSPACE_NAMES       atoms[21]
#define atom__NET_WM_STRUT              atoms[22]


#define WIN_STATE_STICKY          (1<<0)	/* everyone knows sticky */
#define WIN_STATE_MINIMIZED       (1<<1)	/* ??? */
#define WIN_STATE_MAXIMIZED_VERT  (1<<2)	/* window in maximized V state */
#define WIN_STATE_MAXIMIZED_HORIZ (1<<3)	/* window in maximized H state */
#define WIN_STATE_HIDDEN          (1<<4)	/* not on taskbar but window visible */
#define WIN_STATE_SHADED          (1<<5)	/* shaded (NeXT style) */
#define WIN_STATE_HID_WORKSPACE   (1<<6)	/* not on current desktop */
#define WIN_STATE_HID_TRANSIENT   (1<<7)	/* owner of transient is hidden */
#define WIN_STATE_FIXED_POSITION  (1<<8)	/* window is fixed in position even */
#define WIN_STATE_ARRANGE_IGNORE  (1<<9)	/* ignore for auto arranging */

#define WIN_HINTS_SKIP_FOCUS      (1<<0)	/* "alt-tab" skips this win */
#define WIN_HINTS_SKIP_WINLIST    (1<<1)	/* not in win list */
#define WIN_HINTS_SKIP_TASKBAR    (1<<2)	/* not on taskbar */
#define WIN_HINTS_GROUP_TRANSIENT (1<<3)	/* ??????? */
#define WIN_HINTS_FOCUS_ON_CLICK  (1<<4)	/* app only accepts focus when clicked */
#define WIN_HINTS_DO_NOT_COVER    (1<<5)	/* attempt to not cover this window */

#define MAX_TASK_WIDTH 150
#define TASK_PADDING 4
#define ICONSIZE 20
#define PANEL_HEIGHT 27
#define PANEL_BORDER 1



char **get_textproperty(Window win, Atom prop, int *nitems);
void *get_xaproperty (Window win, Atom prop, Atom type, int *nitems);
gchar *myfgets(gchar *str, int num, FILE *fp);

GtkWidget *tb_init(FILE *fp);
GtkWidget *clip_init(FILE *fp);
GtkWidget *lb_init(FILE *fp);
GtkWidget *clock_init(FILE *fp);



#endif
