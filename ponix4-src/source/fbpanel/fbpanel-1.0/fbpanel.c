/* fbpanel.c -- main layout engine of fbpanel project
 *
 * Copyright (C) 2002 Anatoly Asviyan <aanatoly@users.sf.net>
 *
 * This file is part of fbpanel.
 *
 * fbpanel is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * fbpanel is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sawfish; see the file COPYING.   If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

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

char version[] = "1.0";

#define CLOCK_CONFIG     "clock"
#define LAUNCHBAR_CONFIG "launchbar"
#define TASKBAR_CONFIG   "taskbar"
#define CLIP_CONFIG      "clip"

//#define DEBUG
#ifdef DEBUG
#define DBG(format, args...)  fprintf (stderr, format , ## args)
#else
#define DBG(args...)
#endif



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
    "_NET_WM_STATE_HIDDEN",
    
    "_NET_WM_DESKTOP",
    "_NET_WM_NAME",

    "_NET_WM_ALLOWED_ACTIONS",
    "_NET_WM_ACTION_CLOSE",
    "_WIN_WORKSPACE_NAMES",
    "_NET_WM_STRUT",
};

#define ATOM_COUNT (sizeof (atom_names) / sizeof (atom_names[0]))

Atom atoms[ATOM_COUNT];

GtkWidget *window, *hbox;

Window topxwin;



//#define MAX(x,y) (((x) > (y)) ? (x) : (y))

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
    int ret, pos;
    XWMHints wmhints;    
    unsigned int strut_new[4];
       
    gtk_widget_size_request (window, &requisition);
    gdk_window_set_decorations(window->window, 0);
    gtk_widget_set_usize(window, gdk_screen_width(), requisition.height);
    gtk_window_set_policy(GTK_WINDOW(window), FALSE, FALSE, FALSE);
	
    memset(strut_new, 0, sizeof(strut_new));
    pos = 3; /* bottom */
    strut_new[pos] = requisition.height;
    XChangeProperty (GDK_DISPLAY(), topxwin, atom__NET_WM_STRUT, XA_CARDINAL,
                     32, PropModeReplace, (unsigned char *) strut_new, 4);
    
    state[0] = atom__NET_WM_STATE_STICKY;
    state[1] = atom__NET_WM_STATE_SKIP_PAGER;
    state[2] = atom__NET_WM_STATE_SKIP_TASKBAR;
    ret = XChangeProperty (GDK_DISPLAY(), topxwin, atom__NET_WM_STATE, XA_ATOM, 32,
                           PropModeReplace, (unsigned char *) state, 3);
    
    if (ret != Success)
        DBG("XChangeProperty(%d)  failed. Error is %d\n",__LINE__, ret);

  
    val = WIN_STATE_STICKY | WIN_STATE_FIXED_POSITION;
    ret = XChangeProperty (GDK_DISPLAY(), topxwin, atom__WIN_STATE, XA_CARDINAL, 32,
                           PropModeReplace, (unsigned char *) &val, 1);
    if (ret != Success)
        DBG( "XChangeProperty(%d)  failed. Error is %d\n",__LINE__, ret);

    val = WIN_HINTS_SKIP_FOCUS | WIN_HINTS_SKIP_WINLIST | WIN_HINTS_SKIP_TASKBAR
	| WIN_HINTS_DO_NOT_COVER;
    ret = XChangeProperty (GDK_DISPLAY(), topxwin, atom__WIN_HINTS, XA_CARDINAL, 32,
                           PropModeReplace, (unsigned char *) &val, 1);
    if (ret != Success)
        DBG( "XChangeProperty(%d) failed. Error is %d\n", __LINE__, ret);

    /* make our window unfocusable */
    wmhints.flags = InputHint;
    wmhints.input = 0;
    /*XSetWMHints (GDK_DISPLAY(), topxwin, &wmhints);*/
    XChangeProperty (GDK_DISPLAY(), topxwin, XA_WM_HINTS, XA_WM_HINTS,
                     32, PropModeReplace,
                     (unsigned char *) &wmhints, sizeof (XWMHints) / 4);


}

void panel_props2()
{

    XClientMessageEvent xev;
    Atom state[3];
 
    
    xev.type = ClientMessage;
    xev.window = topxwin;
    xev.message_type = atom__NET_WM_DESKTOP;
    xev.format = 32;
    xev.data.l[0] = 0xFFFFFFFF;
    XSendEvent (GDK_DISPLAY(), GDK_ROOT_WINDOW(), False,
                (SubstructureNotifyMask|SubstructureRedirectMask), (XEvent *) &xev);

    state[0] = atom__NET_WM_ACTION_CLOSE;
    XChangeProperty (GDK_DISPLAY(), topxwin, atom__NET_WM_ALLOWED_ACTIONS,
                     XA_ATOM, 32, PropModeReplace, (unsigned char *) state, 1);
   
 }
    
void
panel_pos()
{
    GtkRequisition requisition;

    gtk_widget_size_request (window, &requisition);
    gdk_window_move_resize (window->window, 0,
                            gdk_screen_height() - requisition.height,
                            gdk_screen_width(), requisition.height);
}

void
handle_error (Display * d, XErrorEvent * ev)
{
}



static void simple_expose( GtkWidget *widget, gpointer   data )
{
    gtk_widget_queue_draw (widget);
}


void print_help()
{
    printf("fbpanel  %s\n", version);
    printf("  -v         - print version and exit\n");
    printf("  -h         - print this help and exit\n");
}

GtkWidget *separator_init()
{
    GtkWidget *sep, *eb;
    
    eb = gtk_event_box_new();
    gtk_widget_show(eb);
    gtk_signal_connect(GTK_OBJECT(eb), "expose_event",
                       GTK_SIGNAL_FUNC(gtk_widget_queue_draw), NULL);
    sep = gtk_vseparator_new();
    gtk_widget_show(sep);
    gtk_container_add (GTK_CONTAINER (eb), sep);
    gtk_widget_show(eb);
    return eb;
}

int main(int argc, char *argv[])
{
    GtkWidget *ret;
    int i;
    gchar *f;
    FILE *fp;
    
    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v")) {
            printf("fbpanel %s\n", version);
            exit(0);
        }
        if (!strcmp(argv[i], "-h")) {
            print_help();
            exit(0);
        }
        print_help();
        exit(1);
    }   
      
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


    // clip 
    f = g_strdup_printf("%s/.fbpanel/%s", getenv("HOME"), CLIP_CONFIG);
    fp = fopen(f, "r");
    ret = clip_init(fp);
    if (fp)
        fclose(fp);
    g_free(f);
    if (ret) 
        gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
    ret = separator_init();
    if (ret) 
        gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
     
    // launchbar
    f = g_strdup_printf("%s/.fbpanel/%s", getenv("HOME"), LAUNCHBAR_CONFIG);
    fp = fopen(f, "r");
    ret = lb_init(fp);
    if (fp)
        fclose(fp);
    g_free(f);
    if (ret) 
        gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
    ret = separator_init();
    if (ret) 
        gtk_box_pack_start(GTK_BOX (hbox), ret, FALSE, FALSE, 0);
     
    // taskbar
    f = g_strdup_printf("%s/.fbpanel/%s", getenv("HOME"), TASKBAR_CONFIG);
    fp = fopen(f, "r");
    ret = tb_init(fp);
    if (fp)
        fclose(fp);
    g_free(f);
    if (ret)
        gtk_box_pack_start(GTK_BOX (hbox), ret, TRUE, TRUE, 0);

    // clock
    f = g_strdup_printf("%s/.fbpanel/%s", getenv("HOME"), CLOCK_CONFIG);
    fp = fopen(f, "r");
    ret = clock_init(fp);
    if (fp)
        fclose(fp);
    g_free(f);
    if (ret) 
        gtk_box_pack_end(GTK_BOX (hbox), ret, FALSE, FALSE, 0);

    gtk_widget_show_all(hbox);         
    panel_props();
    gtk_widget_show_all(window);
    panel_props2();
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
