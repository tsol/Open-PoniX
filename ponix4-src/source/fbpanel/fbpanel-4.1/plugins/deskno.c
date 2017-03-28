// reused dclock.c and variables from pager.c
// 11/23/04 by cmeury

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"

// #define DEBUG
#include "dbg.h"

typedef struct {
    GtkWidget *eb;
    GtkWidget *main;
    GtkWidget *namew;
    GtkTooltips *tip;
    char *action;
    int timer;
	
} deskno;

static  void
clicked( GtkWidget *widget, gpointer data)
{
    ENTER;
    int desknum = get_net_current_desktop();
    int desks = get_net_number_of_desktops();
    int newdesk;
    if(desknum == (desks - 1))
	newdesk = 0;
    else
	newdesk = desknum + 1;
    g_assert(data != NULL);
    Xclimsg(GDK_ROOT_WINDOW(), a_NET_CURRENT_DESKTOP, newdesk, 0, 0, 0, 0);
    RET();
}



static gint
name_update(GtkWidget *widget,gpointer data )
{
    deskno *dc;
    char buffer [5];
    int n;
    int desknum = get_net_current_desktop() + 1;
    n = sprintf(buffer, " %d ", desknum);

    ENTER;
    g_assert(data != NULL);
    dc = (deskno *)data;
    
    gtk_label_set_text (GTK_LABEL(dc->namew), buffer) ;
    RET(TRUE);
}


static int
deskno_constructor(plugin *p)
{
    deskno *dc;
    GtkWidget *button;
    char buffer [5];
    
    ENTER;
    dc = g_new0(deskno, 1);
    g_return_val_if_fail(dc != NULL, 0);
    p->priv = dc;
    
    dc->main = gtk_event_box_new();

    button = gtk_button_new();
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (clicked), (gpointer) dc);
    dc->namew = gtk_label_new(buffer);
    gtk_container_add(GTK_CONTAINER(button), dc->namew);
    gtk_container_add(GTK_CONTAINER(p->pwid), button);
    gtk_widget_show_all(p->pwid);
    name_update(button, dc);
    g_signal_connect (G_OBJECT (fbev), "current_desktop", G_CALLBACK (name_update), (gpointer) dc);
    RET(1);

 error:
    g_free(dc->action);
    g_free(dc);
    RET(0);
}


static void
deskno_destructor(plugin *p)
{
  deskno *dc = (deskno *)p->priv;

  ENTER;
  dc = (deskno *) p->priv;
  g_signal_handlers_disconnect_by_func(G_OBJECT (fbev), name_update, dc); 
  g_free(dc);
  RET();
}

plugin_class deskno_plugin_class = {
    fname: NULL,
    count: 0,

    type : "deskno",
    name : "Desktop No / Workspace Name",
    version: "0.5",
    description : "Display workspace number, by cmeury",

    constructor : deskno_constructor,
    destructor  : deskno_destructor,
};
