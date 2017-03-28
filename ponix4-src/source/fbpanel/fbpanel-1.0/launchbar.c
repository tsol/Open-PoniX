/* launchbar.c -- launchbar module of fbpanel project
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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include <gdk-pixbuf/gdk-pixbuf.h>

#include "fbpanel.h"

typedef struct btn {
    GtkWidget *button, *pixmap;
    gchar *action;
}
btn;

#define MAXBUTTONS 20
typedef struct launchbar {
    GtkWidget *box;
    GtkTooltips *tips;
    btn btns[MAXBUTTONS];
    int btn_num;
}
launchbar;


static launchbar lb;

#define ERRSTR "launchbar:config file error:"

void read_button(FILE *fp);


static void
clicked( GtkWidget *widget, gpointer data )
{
    int no = (int) data;
    system(lb.btns[no].action);
}



static void
load_config(FILE *fp)
{
    gchar str[512];
    
    while (myfgets(str, 512, fp)) {
	if (strcmp(str, "[")) {
	    g_warning(ERRSTR "expected '[' but got %s\n", str);
	    return;
	}
	read_button(fp);

	if (lb.btn_num >= MAXBUTTONS) {
	    g_print(ERRSTR "max number of buttons (%d) was reached. skipping the rest\n",
		lb.btn_num );
	    return;
	}
    }
}

void read_button(FILE *fp)
{
    gchar str[512];
    gchar fname[512];
    gchar tooltip[512];
    gchar action[512];
    GdkPixbuf *gp, *gps;
    GdkPixmap *pix;
    GdkBitmap *mask;
    GtkWidget *pixmap, *button;
  
    fname[0] = tooltip[0] = action[0] = 0;
    while (myfgets(str, 512, fp) && (str[0] != ']')) {
	if (!strncmp(str, "Image::", strlen("Image::"))) 
	    strcpy(fname, str + strlen("Image::"));
	else if (!strncmp(str, "Tooltip::", strlen("Tooltip::"))) 
	    strcpy(tooltip, str + strlen("Tooltip::"));
	else if (!strncmp(str, "Action::", strlen("Action::"))) 
	    strcpy(action, str + strlen("Action::"));
	else
	    fprintf(stderr, "Illegal token %s\n", str);
	
    }
    g_strstrip(fname);
    g_strstrip(tooltip);
    g_strstrip(action);
    
    // button
    gp = gdk_pixbuf_new_from_file(fname);
    if (!gp) {
	g_warning(ERRSTR "can't read image %s\n", fname);
	button = gtk_button_new_with_label("?");
    } else {
	gps =  gdk_pixbuf_scale_simple (gp, ICONSIZE, ICONSIZE, GDK_INTERP_TILES);
	gdk_pixbuf_render_pixmap_and_mask(gps, &pix, &mask, 127);
	gdk_pixbuf_unref(gp);
	gdk_pixbuf_unref(gps);
	button = gtk_button_new();
	pixmap = gtk_pixmap_new(pix, mask);
	gtk_widget_show(pixmap);
	lb.btns[lb.btn_num].pixmap = pixmap;
	gtk_container_add(GTK_CONTAINER(button), pixmap);
    }
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button), 0);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (clicked), (gpointer) lb.btn_num);
    gtk_widget_show(button);
    GTK_WIDGET_UNSET_FLAGS (button, GTK_CAN_FOCUS);

    // tooltip
    if (tooltip[0])
	gtk_tooltips_set_tip(GTK_TOOLTIPS (lb.tips), button, tooltip, NULL);
    
    gtk_box_pack_start(GTK_BOX(lb.box), button, FALSE, FALSE, 0);
    lb.btns[lb.btn_num].button = button; 
    lb.btns[lb.btn_num].action = g_strdup(action);
    lb.btn_num++;
}



GtkWidget *
lb_init(FILE *fp)
{
    lb.box = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (lb.box), 0);
    gtk_widget_show(lb.box);
    lb.tips = gtk_tooltips_new();
    
    if (fp)
        load_config(fp);
    
    return  lb.box;
}
