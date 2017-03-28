#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUGPRN
#include "dbg.h"

extern panel *the_panel;


typedef struct {
    plugin_instance *plugin;
    GtkWidget *pb;
    int timer;
} mem_priv;

//static mem me;


static gint
clock_update(mem_priv *dc)
{
    static gdouble p;
          
    ENTER;
    p += 0.1;
    if (p > 1)
        p = 0.0;
    gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR(dc->pb), p);
    RET(TRUE);
}


static int
mem_constructor(plugin_instance *p)
{
    mem_priv *dc;
    GdkColor color;
    GtkRcStyle *rc;
    
    ENTER;
    dc = g_new0(mem_priv, 1);
    g_return_val_if_fail(dc != NULL, 0);
    p->priv = dc;
    
    dc->pb = gtk_progress_bar_new();
    rc = gtk_widget_get_modifier_style(dc->pb);
    if (!rc) {
        rc = gtk_rc_style_new();
    }
    if (rc) {
        rc->color_flags[GTK_STATE_PRELIGHT] |= GTK_RC_BG;
        gdk_color_parse("green", &color);
        rc->bg[GTK_STATE_PRELIGHT] = color;
    }
    gtk_widget_modify_style(dc->pb, rc);
    if (the_panel->orientation == ORIENT_HORIZ) {
        gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(dc->pb), GTK_PROGRESS_BOTTOM_TO_TOP);
        gtk_widget_set_size_request(dc->pb, 9, 0);
    } else {
        gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(dc->pb), GTK_PROGRESS_LEFT_TO_RIGHT);
        gtk_widget_set_size_request(dc->pb, 0, 9);
    }
    gtk_widget_show_all(dc->pb);
    dc->timer = g_timeout_add(1000, (GSourceFunc) clock_update, (gpointer)dc);
    gtk_container_add(GTK_CONTAINER(p->pwid), dc->pb);
    RET(1);
}


static void
mem_destructor(plugin_instance *p)
{
    mem_priv *dc = (mem_priv *)p->priv;

    ENTER;
    dc = (mem_priv *) p->priv;
    if (dc->timer)
        g_source_remove(dc->timer);
    gtk_widget_destroy(dc->pb);
    g_free(dc);
    RET();
}

static plugin_class class = {
    .type        = "mem",
    .name        = "Memory Monitor",
    .version     = "1.0",
    .description = "Show memory usage",
    .priv_size   = sizeof(mem_priv),

    .constructor = mem_constructor,
    .destructor  = mem_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;
