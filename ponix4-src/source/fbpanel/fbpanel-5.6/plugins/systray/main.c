#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"
#include "bg.h"
#include "gtkbgbox.h"


#include "eggtraymanager.h"
#include "fixedtip.h"


//#define DEBUGPRN
#include "dbg.h"


typedef struct {
    plugin_instance plugin;
    GtkWidget *mainw;
    plugin_instance *plug;
    GtkWidget *box;
    /////
    EggTrayManager *tray_manager;
    int icon_num;
} tray_priv;

static void tray_notify_style_event(GtkWidget *w, GParamSpec *arg1, GtkWidget *widget);


static void
tray_added (EggTrayManager *manager, GtkWidget *icon, tray_priv *tr)
{
    ENTER;
    gtk_box_pack_end (GTK_BOX (tr->box), icon, FALSE, FALSE, 0);
    gtk_widget_show(icon);
    gdk_display_sync(gtk_widget_get_display(icon));

    if (!tr->icon_num) {
        DBG("first icon\n");
        gtk_widget_show_all(tr->box);
    }
    tr->icon_num++;
    RET();
}

static void
tray_removed (EggTrayManager *manager, GtkWidget *icon, tray_priv *tr)
{
    ENTER;
    tr->icon_num--;
    DBG("del icon\n");
    if (!tr->icon_num) {
        gtk_widget_hide(tr->box);
        DBG("last icon\n");
    }
    RET();
}

static void
message_sent (EggTrayManager *manager, GtkWidget *icon, const char *text, glong id, glong timeout,
              void *data)
{
    /* FIXME multihead */
    int x, y;
    
    ENTER;
    gdk_window_get_origin (icon->window, &x, &y);
    fixed_tip_show (0, x, y, FALSE, gdk_screen_height () - 50, text);
    RET();
}

static void
message_cancelled (EggTrayManager *manager, GtkWidget *icon, glong id,
                   void *data)
{
    ENTER;
    RET();
  
}



static void
tray_destructor(plugin_instance *p)
{
    tray_priv *tr = (tray_priv *)p->priv;

    ENTER;
    /* Make sure we drop the manager selection */
    if (tr->tray_manager)
        g_object_unref (G_OBJECT (tr->tray_manager));
    fixed_tip_hide ();
    g_free(tr);
    RET();
}

    

static void
tray_notify_style_event(GtkWidget *w, GParamSpec *arg1, GtkWidget *widget)
{
    ENTER;
    /* generates expose event on plugged (reparented) windows */
    gtk_widget_set_size_request(w, w->allocation.width, w->allocation.height);
    gtk_widget_hide(widget);
    if (gtk_events_pending())
        gtk_main_iteration();
    gtk_widget_show(widget);
    gtk_widget_set_size_request(w, -1, -1);
    //gtk_container_foreach (GTK_CONTAINER (widget), (GtkCallback) gtk_widget_hide, NULL);
    //gtk_container_foreach (GTK_CONTAINER (widget), (GtkCallback) gtk_widget_show, NULL);
    RET();
}


static int
tray_constructor(plugin_instance *p)
{
    line s;
    tray_priv *tr;
    GdkScreen *screen;
    //GtkWidget *frame;
    
    ENTER;
    class_get("tray"); //create extra ref so the plugin could not be unloaded
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        ERR("tray: illegal in this context %s\n", s.str);
        RET(0);
    }

    
    tr = g_new0(tray_priv, 1);
    g_return_val_if_fail(tr != NULL, 0);
    p->priv = tr;
    tr->plug = p;
    tr->icon_num = 0;
    tr->box = p->panel->my_box_new(FALSE, 1);
    g_signal_connect_after (p->pwid, "notify::style", G_CALLBACK (tray_notify_style_event), tr->box);
    gtk_container_add(GTK_CONTAINER(p->pwid), tr->box);        
    //if (p->panel->transparent)
    //    gtk_bgbox_set_background(p->pwid, BG_INHERIT, p->panel->tintcolor, p->panel->alpha);


    gtk_container_set_border_width(GTK_CONTAINER(p->pwid), 0);
    screen = gtk_widget_get_screen (GTK_WIDGET (p->panel->topgwin));
    
    if (egg_tray_manager_check_running(screen)) {
        tr->tray_manager = NULL;
        ERR("tray: another systray already running\n");
        RET(1);
    }
    tr->tray_manager = egg_tray_manager_new ();
    if (!egg_tray_manager_manage_screen (tr->tray_manager, screen))
        g_printerr ("tray: System tray didn't get the system tray manager selection\n");
    
    g_signal_connect (tr->tray_manager, "tray_icon_added", G_CALLBACK (tray_added), tr);
    g_signal_connect (tr->tray_manager, "tray_icon_removed", G_CALLBACK (tray_removed), tr);
    g_signal_connect (tr->tray_manager, "message_sent", G_CALLBACK (message_sent), tr);
    g_signal_connect (tr->tray_manager, "message_cancelled", G_CALLBACK (message_cancelled), tr);
    
    gtk_widget_show_all(tr->box);
    RET(1);

}


static plugin_class class = {
    .count       = 0,
    .type        = "tray",
    .name        = "System tray",
    .version     = "1.0",
    .description = "System tray aka Notification Area",
    .priv_size   = sizeof(tray_priv),

    .constructor = tray_constructor,
    .destructor = tray_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;
