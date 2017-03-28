#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "fbpanel.h"

static GtkWidget *clockw;

static gint
update_clock( gpointer data )
{
    char *time_str;
    time_t now;

    now = time (0);
    time_str = ctime (&now) + 11;
    time_str[5] = 0;
    gtk_label_set_text(GTK_LABEL(clockw), time_str);
    return TRUE;
}



GtkWidget *
clock_init()
{
    GtkWidget *b1;
    
    b1 = gtk_button_new();
    clockw =  gtk_label_new("26:87");
    gtk_widget_show(clockw);
    gtk_container_add(GTK_CONTAINER(b1), clockw);
    gtk_widget_show(b1);
    update_clock(NULL);
    gtk_timeout_add( 50000, update_clock, NULL);
    return b1;
}
