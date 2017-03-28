/* System tray main() */

/* 
 * Copyright (C) 2002 Red Hat, Inc.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xatom.h>
#include <gtk/gtk.h>
//#include <X11/cursorfont.h>
//#include <X11/Xmu/WinUtil.h>

#include "eggtraymanager.h"
#include "fixedtip.h"

static GtkOrientation orient;

static EggTrayManager *tray_manager = NULL;
static GSList *all_trays = NULL;

typedef struct {
    GtkWidget *applet;
    GtkWidget *box;
    GtkWidget *frame;
    int size;
} SystemTray;





static void
tray_added (EggTrayManager *manager, GtkWidget *icon, SystemTray *tray)
{
    if (all_trays == NULL)
        return;

    gtk_box_pack_start (GTK_BOX (tray->box), icon, FALSE, FALSE, 0);
    gtk_widget_show (icon);

}

static void
tray_removed (EggTrayManager *manager, GtkWidget *icon, SystemTray *tray)
{
    gtk_widget_destroy(icon);
}

static void
message_sent (EggTrayManager *manager, GtkWidget *icon, const char *text, glong id, glong timeout,
      void *data)
{
    /* FIXME multihead */
    int x, y;
    gdk_window_get_origin (icon->window, &x, &y);
    fixed_tip_show (0, x, y, FALSE, gdk_screen_height () - 50, text);
}

static void
message_cancelled (EggTrayManager *manager, GtkWidget *icon, glong id,
      void *data)
{
    //fprintf(stderr, "%s\n", __FUNCTION__);
}



static void
free_tray (SystemTray *tray)
{
    all_trays = g_slist_remove (all_trays, tray);

    //fprintf(stderr, "%s\n", __FUNCTION__);
    if (all_trays == NULL)
    {
        /* Make sure we drop the manager selection */
        g_object_unref (G_OBJECT (tray_manager));
        tray_manager = NULL;
        fixed_tip_hide ();
    }
  
    g_free (tray);
}

int
main (int argc, char *argv[])
{
    SystemTray *tray;
    Window win;
    int i;

    orient = GTK_ORIENTATION_HORIZONTAL;
    gtk_init(&argc, &argv);
    for (i=1; i < argc; i++) {
        if (!strcmp(argv[i], "-h"))
            orient = GTK_ORIENTATION_HORIZONTAL;
        else if  (!strcmp(argv[i], "-v"))
            orient = GTK_ORIENTATION_VERTICAL;
        else {
            fprintf(stderr, "unknown option <%s>\n", argv[i]);
            exit(1);
        }
    }
    tray = g_new0 (SystemTray, 1);
    if (tray_manager == NULL)
    {
        GdkScreen *screen;

        screen = gdk_screen_get_default();  

        tray_manager = egg_tray_manager_new ();

        if (!egg_tray_manager_manage_screen (tray_manager, screen))
            g_printerr ("System tray didn't get the system tray manager selection\n");

        g_signal_connect (tray_manager, "tray_icon_added",
              G_CALLBACK (tray_added), tray);
        g_signal_connect (tray_manager, "tray_icon_removed",
              G_CALLBACK (tray_removed), tray);
        g_signal_connect (tray_manager, "message_sent",
              G_CALLBACK (message_sent), NULL);
        g_signal_connect (tray_manager, "message_cancelled",
              G_CALLBACK (message_cancelled), NULL);
    }
      
  
    tray->applet = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_policy(GTK_WINDOW(tray->applet), FALSE, FALSE, TRUE);
    tray->frame = gtk_event_box_new ();
    tray->box = (orient == GTK_ORIENTATION_HORIZONTAL)
        ? gtk_hbox_new (FALSE, 1)
        : gtk_vbox_new (FALSE, 1);
    gtk_container_add (GTK_CONTAINER (tray->frame), tray->box);
    gtk_widget_show(tray->box);

    tray->size = 24;
    all_trays = g_slist_append (all_trays, tray);
      
    gtk_container_add (GTK_CONTAINER (tray->applet), tray->frame);
    gtk_widget_show(tray->frame);
    gtk_widget_realize(tray->applet);
    win = GDK_WINDOW_XWINDOW(GTK_WIDGET(tray->applet)->window);
    XChangeProperty(GDK_DISPLAY(), win,
          XInternAtom(GDK_DISPLAY(), "_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR", False),
          XA_WINDOW, 32, PropModeReplace, (unsigned char *) &win, 1);
 
    gtk_widget_show_all (GTK_WIDGET (tray->applet));
  
    gtk_main();
    return 0;
}
 
