/* pager.c -- The display program of sawfish.wm.ext.pager
   $Id: pager.c,v 1.4 2002/04/06 15:01:14 pfeiffer Exp $

   Copyright (C) 2002 Daniel Pfeiffer <occitan@esperanto.org>
   Copyright (C) 2000 Satyaki Das <satyaki@theforce.stanford.edu>
                      Hakon Alstadheim

   This file is part of sawfish.

   sawfish is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   sawfish is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with sawfish; see the file COPYING.   If not, write to
   the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA. */

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <stdlib.h>
#include <locale.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>


#include "fbpanel.h"




char *atom_names[] = {
    "KWM_WIN_ICON",
    "_MOTIF_WM_HINTS",
    "_WIN_HINTS",
    "_WIN_LAYER",
    "_WIN_STATE",
    "WM_STATE",
    "WM_CLASS",
    
    "_NET_CLIENT_LIST",
    "_NET_NUMBER_OF_DESKTOPS",
    "_NET_CURRENT_DESKTOP",
    "_NET_DESKTOP_NAMES",
    "_NET_ACTIVE_WINDOW",

    "_NET_WM_STATE",
    "_NET_WM_STATE_SKIP_TASKBAR",
    "_NET_WM_STATE_SKIP_PAGER",
    "_NET_WM_STATE_STICKY", 

    "_NET_WM_DESKTOP",
    "_NET_WM_NAME",

    "_WIN_WORKSPACE_NAMES", 
};

#define ATOM_COUNT (sizeof (atom_names) / sizeof (atom_names[0]))

Atom atoms[ATOM_COUNT];

GtkWidget *window, *hbox;

Window topxwin;



gint delete_event(GtkWidget *, GdkEvent *, gpointer);
gint destroy_event(GtkWidget *, GdkEvent *, gpointer);



gchar *
myfgets(gchar *str, int num, FILE *fp)
{
    while (fgets(str, num, fp)) {
	g_strstrip(str);
	if (str[0] == '#' || str[0] == 0)
	    continue;
	else
	    return str;
    }
    return NULL;
}


void *
get_xaproperty (Window win, Atom prop, Atom type, int *nitems)
{
    Atom type_ret;
    int format_ret;
    unsigned long items_ret;
    unsigned long after_ret;
    unsigned char *prop_data;

    prop_data = NULL;

    XGetWindowProperty (GDK_DISPLAY(), win, prop, 0, 0x7fffffff, False,
                        type, &type_ret, &format_ret, &items_ret,
                        &after_ret, &prop_data);    
    if (nitems)
        *nitems = (prop_data) ? items_ret : 0;
    /* must be freed with XFree(prop_data) */
    return prop_data;
}


char **
get_textproperty(Window win, Atom prop, int *nitems)
{
    XTextProperty text_prop;
    char **list;
    int ret;
    
    if (XGetTextProperty(GDK_DISPLAY(), win, &text_prop, prop)) {
	ret = XmbTextPropertyToTextList(GDK_DISPLAY(), &text_prop, &list, nitems);
	XFree(text_prop.value);
	if ((ret == Success) && (*nitems > 0) && *list) {
	    /* must be freed with XFreeStringList(list); */
	    return list;
	} 
    }
    return NULL;
}


void panel_props()
{
    Atom state[3];
    unsigned int val;
    GtkRequisition requisition;
    
    gtk_widget_size_request (window, &requisition);
    gdk_window_set_decorations(window->window, 0);
    gtk_widget_set_usize(window, gdk_screen_width(), requisition.height);
    gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	
    state[0] = atom__NET_WM_STATE_STICKY;
    state[1] = atom__NET_WM_STATE_SKIP_PAGER;
    state[2] = atom__NET_WM_STATE_SKIP_TASKBAR;
    XChangeProperty (GDK_DISPLAY(), topxwin, atom__NET_WM_STATE, XA_ATOM, 32,
    	PropModeReplace, (unsigned char *) state, 3);

    val = WIN_STATE_STICKY | WIN_STATE_FIXED_POSITION;
    XChangeProperty (GDK_DISPLAY(), topxwin, atom__WIN_STATE, XA_CARDINAL, 32,
	PropModeReplace, (unsigned char *) &val, 1);

    val = WIN_HINTS_SKIP_FOCUS | WIN_HINTS_SKIP_WINLIST | WIN_HINTS_SKIP_TASKBAR
	| WIN_HINTS_DO_NOT_COVER;
    XChangeProperty (GDK_DISPLAY(), topxwin, atom__WIN_HINTS, XA_CARDINAL, 32,
	PropModeReplace, (unsigned char *) &val, 1);

}


void
panel_pos()
{
    gdk_window_move_resize (window->window, 0, gdk_screen_height()-22, gdk_screen_width(), 22);
}

void
handle_error (Display * d, XErrorEvent * ev)
{
}

    
int main(int argc, char *argv[])
{
    GtkWidget *ret, *sep;
 
      
    setlocale(LC_CTYPE, "");
    gtk_set_locale ();
    gtk_init(&argc, &argv);
    XSetLocaleModifiers("");
    XInternAtoms (GDK_DISPLAY(), atom_names, ATOM_COUNT, False, atoms);
    XSetErrorHandler ((XErrorHandler) handle_error);
    
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_container_set_border_width(GTK_CONTAINER(window), 0);
    gtk_signal_connect(GTK_OBJECT(window), "delete_event",
	GTK_SIGNAL_FUNC(delete_event), NULL);
    gtk_signal_connect(GTK_OBJECT(window), "destroy",
	GTK_SIGNAL_FUNC(destroy_event), NULL);
    gtk_widget_realize(window);
    topxwin = GDK_WINDOW_XWINDOW (GTK_WIDGET(window)->window);
  
    
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(window), hbox);

    ret = clip_init();
    gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
    sep = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX (hbox), sep, FALSE, FALSE, 0);
    
    ret = lb_init(NULL);
    gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
    sep = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX (hbox), sep, FALSE, FALSE, 0);
    
    ret = tb_init(NULL);
    gtk_box_pack_start(GTK_BOX (hbox), ret, TRUE, TRUE, 0);
  

    
    ret = clock_init();
    gtk_box_pack_end(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
    sep = gtk_vseparator_new();
    gtk_box_pack_end(GTK_BOX (hbox), sep, FALSE, FALSE, 0);
   
    
    gtk_widget_show_all(hbox);         
 
    panel_props();

    gtk_widget_show_all(window);
 
    

    panel_pos();

    gtk_main();
    return 0;
}

gint delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  return FALSE;
}

gint destroy_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  gtk_main_quit();
  return FALSE;
}
