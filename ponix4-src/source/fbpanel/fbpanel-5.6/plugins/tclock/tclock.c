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


#define DEFAULT_TIP_FORMAT    "%A %x"
#define DEFAULT_CLOCK_FORMAT  "%R"

typedef struct {
    plugin_instance plugin;
    GtkWidget *eb;
    GtkWidget *main;
    GtkWidget *clockw;
    char *tfmt;
    char *cfmt;
    char *action;
    short lastDay;
    int timer;
} tclock_priv;

//static tclock_priv me;



static  gboolean
clicked( GtkWidget *widget, gpointer dummy, tclock_priv *dc)
{
    ENTER2;
    DBG("%s\n", dc->action);
    system (dc->action);
    RET2(TRUE);
}



static gint
clock_update(gpointer data )
{
    char output[64], str[64];
    time_t now;
    struct tm * detail;
    tclock_priv *dc;
    gchar *utf8;
    
    ENTER;
    g_assert(data != NULL);
    dc = (tclock_priv *)data;
    
    time(&now);
    detail = localtime(&now);
    strftime(output, sizeof(output), dc->cfmt, detail) ;
    g_snprintf(str, 64, "<b>%s</b>", output);
    gtk_label_set_markup (GTK_LABEL(dc->clockw), str) ;

    if (detail->tm_mday != dc->lastDay) {
	dc->lastDay = detail->tm_mday ;

	strftime (output, sizeof(output), dc->tfmt, detail) ;
        if ((utf8 = g_locale_to_utf8(output, -1, NULL, NULL, NULL))) {
            gtk_widget_set_tooltip_markup(dc->main, utf8);
            g_free(utf8);
        }
    }
    RET(TRUE);
}


static int
tclock_constructor(plugin_instance *p)
{
    line s;
    tclock_priv *dc;
    char output [40] ;
    time_t now ;
    struct tm * detail ;
    
    ENTER;
    dc = g_new0(tclock_priv, 1);
    g_return_val_if_fail(dc != NULL, 0);
    p->priv = dc;
    
    dc->cfmt = dc->tfmt = dc->action = 0;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "tclock: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "ClockFmt")) 
                dc->cfmt = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "TooltipFmt"))
                dc->tfmt = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "Action"))
                dc->action = g_strdup(s.t[1]);
            else {
                ERR( "tclock: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "tclock: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    if (!dc->cfmt)
        dc->cfmt = g_strdup(DEFAULT_CLOCK_FORMAT);
    if (!dc->tfmt)
        dc->tfmt = g_strdup(DEFAULT_TIP_FORMAT);
    dc->main = gtk_event_box_new();
    //gtk_widget_add_events (dc->main, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
    //button = gtk_button_new();
    //gtk_container_add(GTK_CONTAINER(dc->main), button);
    if (dc->action)
        g_signal_connect (G_OBJECT (dc->main), "button_press_event",
              G_CALLBACK (clicked), (gpointer) dc);
    time(&now);
    detail = localtime(&now);
    strftime(output, sizeof(output), dc->cfmt, detail) ;
    dc->clockw = gtk_label_new(output);
    gtk_misc_set_alignment(GTK_MISC(dc->clockw), 0.5, 0.5);
    gtk_misc_set_padding(GTK_MISC(dc->clockw), 4, 0);
    //gtk_widget_show(dc->clockw);
    gtk_container_add(GTK_CONTAINER(dc->main), dc->clockw);
    gtk_widget_show_all(dc->main);
    dc->timer = g_timeout_add(1000, (GSourceFunc) clock_update, (gpointer)dc);
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
tclock_destructor(plugin_instance *p)
{
  tclock_priv *dc = (tclock_priv *)p->priv;

  ENTER;
  dc = (tclock_priv *) p->priv;
  if (dc->timer)
      g_source_remove(dc->timer);
  gtk_widget_destroy(dc->main);
  g_free(dc->cfmt);
  g_free(dc->tfmt);
  g_free(dc->action);
  g_free(dc);
  RET();
}

static plugin_class class = {
    .count       = 0,
    .type        = "tclock",
    .name        = "Text Clock",
    .version     = "1.0",
    .description = "Text clock/date with tooltip",
    .priv_size   = sizeof(tclock_priv),

    .constructor = tclock_constructor,
    .destructor = tclock_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;
