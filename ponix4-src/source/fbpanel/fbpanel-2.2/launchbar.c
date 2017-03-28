
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>


#include <gdk-pixbuf/gdk-pixbuf.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUG
#include "dbg.h"


typedef struct btn {
    //GtkWidget *button, *pixmap;
    GdkPixmap *pix;
    GdkBitmap *mask;
    gchar *action;
} btn;

#define MAXBUTTONS 20
typedef struct launchbar {
    GtkWidget *box;
    GtkTooltips *tips;
    btn btns[MAXBUTTONS];
    int btn_num;
} launchbar;


static void
launchbar_destructor(plugin *p)
{
    launchbar *lb = (launchbar *)p->priv;
    int i;

    ENTER;
    gtk_widget_destroy(lb->box);
    for (i = 0; i < lb->btn_num; i++) {
        g_free(lb->btns[i].action);
        gdk_bitmap_unref(lb->btns[i].mask);
        gdk_pixmap_unref(lb->btns[i].pix);
    }
    g_free(lb);
    RET();
}


static void
clicked(GtkWidget *widget, gpointer data )
{
    ENTER;
    g_assert(data != NULL);
    system(((btn *)data)->action);
    RET();
}




static int
read_button(plugin *p)
{
    launchbar *lb = (launchbar *)p->priv;
    gchar *fname, *tooltip, *action;
    GdkPixbuf *gp, *gps;
    GtkWidget *pixmap, *button;
    line s;

    ENTER;
    s.len = 256;
    if (lb->btn_num >= MAXBUTTONS) {
        ERR("launchbar: max number of buttons (%d) was reached. skipping the rest\n",
              lb->btn_num );
        RET(0);
    }

    tooltip = fname = action = 0;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "image: illegal token %s\n", s.str);
            RET(0);
        }
        if (s.type == LINE_VAR) {
            if (!g_strcasecmp(s.t[0], "image")) 
                fname = expand_tilda(s.t[1]);
            else if (!g_strcasecmp(s.t[0], "tooltip"))
                tooltip = g_strdup(s.t[1]);
            else if (!g_strcasecmp(s.t[0], "action"))
                action = g_strdup(s.t[1]);
            else {
                ERR( "image: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "image: illegal in this context %s\n", s.str);
            goto error;
        }
    }


    // button
    gp = gdk_pixbuf_new_from_file(fname);
    if (!gp) {
        g_warning("launchbar: can't read image %s\n", fname);
        button = gtk_button_new_with_label("?");
    } else {
        float ratio;
                  
        ratio = (p->panel->orientation == ORIENT_HORIZ) ?
            (float) (p->panel->ah - 4) / (float) gdk_pixbuf_get_height(gp)
            : (float) (p->panel->aw - 4) / (float) gdk_pixbuf_get_width(gp);
        gps =  gdk_pixbuf_scale_simple (gp,
              ratio * ((float) gdk_pixbuf_get_width(gp)),
              ratio * ((float) gdk_pixbuf_get_height(gp)),
              GDK_INTERP_HYPER);
        gdk_pixbuf_render_pixmap_and_mask(gps, &lb->btns[lb->btn_num].pix,
              &lb->btns[lb->btn_num].mask, 127);
        gdk_pixbuf_unref(gp);
        gdk_pixbuf_unref(gps);
        button = gtk_button_new();
        pixmap = gtk_pixmap_new(lb->btns[lb->btn_num].pix, lb->btns[lb->btn_num].mask);
        gtk_widget_show(pixmap);
        //lb->btns[lb->btn_num].pixmap = pixmap;
        gtk_container_add(GTK_CONTAINER(button), pixmap);
    }
    gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
    gtk_container_set_border_width(GTK_CONTAINER(button), 0);
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
                        GTK_SIGNAL_FUNC (clicked), (gpointer) &lb->btns[lb->btn_num]);
    gtk_widget_show(button);
    GTK_WIDGET_UNSET_FLAGS (button, GTK_CAN_FOCUS);

    g_free(fname);
    // tooltip
    if (tooltip) {
        gtk_tooltips_set_tip(GTK_TOOLTIPS (lb->tips), button, tooltip, NULL);
        g_free(tooltip);
    }
    gtk_box_pack_start(GTK_BOX(lb->box), button, FALSE, FALSE, 0);
    //lb->btns[lb->btn_num].button = button; 
    lb->btns[lb->btn_num].action = action;
    lb->btn_num++;
    RET(1);

 error:
    g_free(fname);
    g_free(tooltip);
    g_free(action);
    launchbar_destructor(p);
    RET(0);
}

static int
launchbar_constructor(plugin *p)
{
    launchbar *lb; 
    line s;

    ENTER;
    lb = g_new0(launchbar, 1);
    g_return_val_if_fail(lb != NULL, 0);
    p->priv = lb;

    s.len = 256;
    lb->box = p->panel->my_box_new(FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (lb->box), 0);
    gtk_widget_show(lb->box);
    lb->tips = gtk_tooltips_new();
    gtk_container_add(GTK_CONTAINER(p->pwid), lb->box);
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "launchbar: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_BLOCK_START) {
            if (!g_strcasecmp(s.t[0], "button")) {
                if (!read_button(p)) {
                    ERR( "image: can't init button\n");
                    goto error;
                }
            } else {
                ERR( "image: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "image: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    RET(1);

 error:
    launchbar_destructor(p);
    RET(0);
    
}



plugin_class launchbar_plugin_class = {
    fname: NULL,
    count: 0,

    type : "launchbar",
    name : "launchbar",
    version: "1.0",
    description : "Bar with buttons to launch application",

    constructor : launchbar_constructor,
    destructor  : launchbar_destructor,
};
