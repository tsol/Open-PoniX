
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUG
#include "dbg.h"


#define DEFAULT_TIP_FORMAT    "%A %x"
#define DEFAULT_CLOCK_FORMAT  "%R"

typedef struct {
    GtkWidget *main;
    GtkWidget *clockw;
    GtkTooltips *tip;
    char *tfmt;
    char *cfmt;
    char *action;
    short lastDay;
    int timer;
} dclock;

//static dclock me;



static  void
clicked( GtkWidget *widget, gpointer data)
{
    ENTER;
    g_assert(data != NULL);
    system(((dclock *)data)->action);
    RET();
}



static gint
clock_update(gpointer data )
{
    char output [40] ;
    time_t now ;
    struct tm * detail ;
    dclock *dc;

    ENTER;
    g_assert(data != NULL);
    dc = (dclock *)data;
    
    time(&now);
    detail = localtime(&now);
    strftime(output, sizeof(output), dc->cfmt, detail) ;
    gtk_label_set_text (GTK_LABEL(dc->clockw), output) ;

    if (detail->tm_mday != dc->lastDay) {
	dc->lastDay = detail->tm_mday ;

	strftime (output, sizeof(output), dc->tfmt, detail) ;
	gtk_tooltips_set_tip(dc->tip, dc->main, output, NULL) ;
    }
    RET(TRUE);
}


static int
dclock_constructor(plugin *p)
{
    line s;
    dclock *dc;
    char output [40] ;
    time_t now ;
    struct tm * detail ;
    
    ENTER;
    dc = g_new0(dclock, 1);
    g_return_val_if_fail(dc != NULL, 0);
    p->priv = dc;
    
    s.len = 256;
    dc->cfmt = dc->tfmt = dc->action = 0;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "image: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_strcasecmp(s.t[0], "ClockFmt")) 
                dc->cfmt = g_strdup(s.t[1]);
            else if (!g_strcasecmp(s.t[0], "TooltipFmt"))
                dc->tfmt = g_strdup(s.t[1]);
            else if (!g_strcasecmp(s.t[0], "Action"))
                dc->action = g_strdup(s.t[1]);
            else {
                ERR( "image: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "image: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    if (!dc->cfmt)
        dc->cfmt = g_strdup(DEFAULT_CLOCK_FORMAT);
    if (!dc->tfmt)
        dc->tfmt = g_strdup(DEFAULT_TIP_FORMAT);
    dc->main = gtk_button_new();
    if (dc->action)
        gtk_signal_connect (GTK_OBJECT (dc->main), "clicked",
              GTK_SIGNAL_FUNC (clicked), (gpointer) dc);
    time(&now);
    detail = localtime(&now);
    strftime(output, sizeof(output), dc->cfmt, detail) ;
    dc->clockw = gtk_label_new(output);
    gtk_widget_show(dc->clockw);
    gtk_container_add(GTK_CONTAINER(dc->main), dc->clockw);
    gtk_widget_show(dc->main);
    dc->tip = gtk_tooltips_new();
    dc->timer = gtk_timeout_add(1000, clock_update, (gpointer)dc);
    gtk_container_add(GTK_CONTAINER(p->pwid), dc->main);
    RET(1);

 error:
    g_free(dc->cfmt);
    g_free(dc->tfmt);
    g_free(dc->action);
    g_free(dc);
    RET(0);
}


static void
dclock_destructor(plugin *p)
{
  dclock *dc = (dclock *)p->priv;

  ENTER;
  dc = (dclock *) p->priv;
  if (dc->timer)
      gtk_timeout_remove(dc->timer);
  gtk_widget_destroy(dc->main);
  g_free(dc->cfmt);
  g_free(dc->tfmt);
  g_free(dc->action);
  g_free(dc);
  RET();
}

plugin_class dclock_plugin_class = {
    fname: NULL,
    count: 0,

    type : "dclock",
    name : "Digital Clock",
    version: "1.0",
    description : "Dispaly Digital clock and Tooltip",

    constructor : dclock_constructor,
    destructor  : dclock_destructor,
};
