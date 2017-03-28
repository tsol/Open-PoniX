/* clip.c -- desktop switcher module of fbpanel project
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
#include "fbpanel.h"


typedef struct clip {
    GtkWidget *hbox, *label;
    GtkWidget *bprev, *bnext, *aprev, *anext;
    int desk_namesno;
    int desk_count;
    char **desk_names;
    int cur_desktop;
    int maxwidth;
}
clip;

void clip_propertynotify(XEvent *ev);
void clip_net_current_desktop();
void clip_display();
void clip_net_number_of_desktops();
void clip_net_desktop_names();
GdkFilterReturn clip_event_filter( GdkXEvent *xevent,  GdkEvent *event,   void *sia);

static clip cp;

GdkFilterReturn clip_event_filter( GdkXEvent *xevent,
    GdkEvent *event,
    void *sia)
{
    if (((XEvent *) xevent)->type == PropertyNotify )
	clip_propertynotify( (XEvent *)xevent);
    return GDK_FILTER_CONTINUE;
}


void
clip_propertynotify(XEvent *ev)
{
    Atom at = ev->xproperty.atom;
    Window win = ev->xproperty.window;
    
    if (win == GDK_ROOT_WINDOW()) {
	if (at == atom__NET_CURRENT_DESKTOP) {
	    clip_net_current_desktop();
	} else if (at == atom__NET_NUMBER_OF_DESKTOPS) {
	    clip_net_number_of_desktops();
	} else if (at == atom__NET_DESKTOP_NAMES) {
	    clip_net_desktop_names();
	}
	clip_display();
    }
}

void
clip_display()
{
    gchar *text;
    
    text = g_strdup_printf("%d/%d %s", cp.cur_desktop+1, cp.desk_count,
	(cp.cur_desktop < cp.desk_namesno) ? cp.desk_names[cp.cur_desktop]
	: "???");
    gtk_label_set_text(GTK_LABEL(cp.label), text);
    g_free(text);
    gtk_widget_queue_draw(cp.label);
}

void
clip_net_current_desktop()
{
    unsigned long *ret;

    cp.cur_desktop = 0;
    ret = get_xaproperty (GDK_ROOT_WINDOW(), atom__NET_CURRENT_DESKTOP, XA_CARDINAL, NULL);
    if (ret)  {
	cp.cur_desktop = *ret;
	XFree (ret);
    }
}
void
clip_net_number_of_desktops()
{
    unsigned long *ret;

    cp.desk_count = 0;
    cp.maxwidth = 0;
    ret = get_xaproperty (GDK_ROOT_WINDOW(), atom__NET_NUMBER_OF_DESKTOPS, XA_CARDINAL, NULL);
    if (ret)  {
	cp.desk_count = *ret;
	XFree (ret);
    }
}

void
clip_net_desktop_names()
{
    GtkRequisition req1;
    gchar *text;
    int i;
    
    if (cp.desk_names)
	XFreeStringList(cp.desk_names);

    cp.maxwidth = 0;
    cp.desk_names = get_textproperty(GDK_ROOT_WINDOW(),	atom__NET_DESKTOP_NAMES,
	&cp.desk_namesno);
    if (!cp.desk_names) {
	cp.desk_namesno = 0;
	return;
    }
    
    for (i=0; i < cp.desk_namesno; i++) {
	text = g_strdup_printf("%d/%d %s", cp.cur_desktop+1, cp.desk_count, cp.desk_names[i]);
	gtk_label_set_text(GTK_LABEL(cp.label), text);
	gtk_widget_size_request (cp.label, &req1);
	//printf("clip(w=%d,h=%d)\n",  req1.width, req1.height);
	if (cp.maxwidth < req1.width)
	    cp.maxwidth = req1.width;
	g_free(text);
    }
    gtk_widget_set_usize(cp.label, cp.maxwidth, req1.height);    
}


void
switch_desk( GtkWidget *widget, gpointer data )
{
    int newdesk = (int) data;
    XClientMessageEvent xev;

    newdesk += cp.cur_desktop;
    if (newdesk < 0)
	newdesk = cp.desk_count -1;
    else if (newdesk >= cp.desk_count)
	newdesk = 0;
    
	
    xev.type = ClientMessage;
    xev.window = GDK_ROOT_WINDOW();
    xev.message_type = atom__NET_CURRENT_DESKTOP;
    xev.format = 32;
    xev.data.l[0] = newdesk;
    XSendEvent (GDK_DISPLAY(), GDK_ROOT_WINDOW(), False,
                (SubstructureNotifyMask|SubstructureRedirectMask), (XEvent *) &xev);
}


GtkWidget *
clip_init(FILE *fp)
{
    GtkWidget *eb;
    GtkShadowType sh = GTK_SHADOW_OUT;

    cp.hbox = gtk_table_new (1, 3, FALSE);

    // left arrow button
    cp.bprev = gtk_button_new();
    gtk_signal_connect (GTK_OBJECT (cp.bprev), "clicked",
	GTK_SIGNAL_FUNC (switch_desk), (gpointer) -1);
    cp.aprev = gtk_arrow_new (GTK_ARROW_LEFT, sh);
    gtk_widget_show (cp.aprev);
    gtk_container_add (GTK_CONTAINER (cp.bprev), cp.aprev);
    gtk_table_attach (GTK_TABLE(cp.hbox), cp.bprev, 0, 1, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 0);
    gtk_widget_show (cp.bprev);
    GTK_WIDGET_UNSET_FLAGS (cp.bprev, GTK_CAN_FOCUS);
    // event box with label
    eb = gtk_event_box_new();
    gtk_widget_show(eb);
    gtk_signal_connect(GTK_OBJECT(eb), "expose_event",
	GTK_SIGNAL_FUNC(gtk_widget_queue_draw), NULL);
    gtk_table_attach (GTK_TABLE(cp.hbox), eb, 1, 2, 0, 1, GTK_EXPAND|GTK_SHRINK,
	GTK_EXPAND | GTK_SHRINK, 5, 0);
    
    cp.label = gtk_label_new("the map");
    gtk_widget_show(cp.label);
    gtk_container_add (GTK_CONTAINER (eb), cp.label);
    // right arrow button
    cp.bnext = gtk_button_new();
    gtk_signal_connect (GTK_OBJECT (cp.bnext), "clicked",
	 GTK_SIGNAL_FUNC (switch_desk), (gpointer) 1);
    cp.anext = gtk_arrow_new (GTK_ARROW_RIGHT, sh);
    gtk_widget_show (cp.anext);
    gtk_container_add (GTK_CONTAINER (cp.bnext), cp.anext);
    gtk_table_attach (GTK_TABLE(cp.hbox), cp.bnext, 2, 3, 0, 1, GTK_SHRINK, GTK_SHRINK, 1, 0);
    gtk_widget_show (cp.bnext);
    GTK_WIDGET_UNSET_FLAGS (cp.bprev, GTK_CAN_FOCUS);
    
    gtk_widget_show (cp.hbox);
    
    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), PropertyChangeMask);
    gdk_window_add_filter(NULL, (GdkFilterFunc)clip_event_filter, NULL );
    gdk_window_add_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)clip_event_filter, NULL);

    clip_net_current_desktop();
    clip_net_number_of_desktops();
    clip_net_desktop_names();
    clip_display();
     
    return cp.hbox;

}
