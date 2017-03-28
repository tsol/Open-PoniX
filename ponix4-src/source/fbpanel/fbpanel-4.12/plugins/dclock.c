/* dclock is an adaptation of blueclock by Jochen Baier <email@Jochen-Baier.de> */

#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUG
#include "dbg.h"


#define DEFAULT_TIP_FORMAT    "%A %x"
#define CLOCK_24H_FMT  "%R"
#define CLOCK_12H_FMT  "%I:%M"
#define COLON_WIDTH   7
#define DIGIT_WIDTH   11
#define DIGIT_HEIGHT  15
#define DIGIT_PAD_H   1
#define COLON_PAD_H   3

typedef struct {
    GtkWidget *main;
    GtkWidget *pwid;
    GtkTooltips *tip;
    char *tfmt;
    char *cfmt;
    struct tm time;
    char *action;
    int timer;
    GdkPixbuf *glyphs; //vert row of '0'-'9' and ':'
    GdkPixbuf *clock;
    guint32 color;
} dclock;

//static dclock me;



static  gboolean
clicked(GtkWidget *widget, gpointer dummy, dclock *dc)
{
    ENTER;
    DBG("%s\n", dc->action);
    system (dc->action);
    RET(TRUE);
}



static gint
clock_update(dclock *dc )
{
    char output[64], *tmp, *utf8;
    time_t now;
    struct tm * detail;
    int i, w;
    
    ENTER;
    time(&now);
    detail = localtime(&now);
    if (detail->tm_min == dc->time.tm_min &&
          detail->tm_hour == dc->time.tm_hour)
        RET(TRUE);
    dc->time = *detail;
    if (!strftime(output, sizeof(output), dc->cfmt, detail))
        RET(TRUE);
    DBG("making new clock pixbuf ");
    gdk_pixbuf_fill(dc->clock, 0);
    for (tmp = output, w = 0; *tmp; tmp++) {
        DBGE("%c", *tmp);
        if (isdigit(*tmp)) {
            i = *tmp - '0';
            gdk_pixbuf_copy_area(dc->glyphs, i * 20, 0, DIGIT_WIDTH, DIGIT_HEIGHT,
                  dc->clock, w, DIGIT_PAD_H);
            w += DIGIT_WIDTH;
        } else if (*tmp == ':') {
            gdk_pixbuf_copy_area(dc->glyphs, 10 * 20, 0, COLON_WIDTH, DIGIT_HEIGHT - 3,
                  dc->clock, w, COLON_PAD_H + DIGIT_PAD_H);
            w += COLON_WIDTH;
        } else {
            ERR("dclock: got %c while expecting for digit or ':'\n", *tmp);
        }
    }
    DBG("\n");
    gtk_widget_queue_draw(dc->main);
    strftime (output, sizeof(output), dc->tfmt, detail) ;
    if ((utf8 = g_locale_to_utf8(output, -1, NULL, NULL, NULL))) {
        gtk_tooltips_set_tip(dc->tip, dc->pwid, utf8, NULL) ;
        g_free(utf8);
    }
  

    RET(TRUE);
}

#define BLUE_R    0
#define BLUE_G    0
#define BLUE_B    0xFF

static void
dclock_set_color(GdkPixbuf *glyphs, guint32 color)
{
    guchar *p1, *p2;
    int w, h;
    guint r, g, b;
    
    ENTER;
    p1 = gdk_pixbuf_get_pixels(glyphs);
    h = gdk_pixbuf_get_height(glyphs);
    r = (color & 0x00ff0000) >> 16;
    g = (color & 0x0000ff00) >> 8;
    b = (color & 0x000000ff);
    DBG("%dx%d: %02x %02x %02x %02x\n",
          gdk_pixbuf_get_width(glyphs), gdk_pixbuf_get_height(glyphs),
          r, g, b, a);
    while (h--) {
        for (p2 = p1, w = gdk_pixbuf_get_width(glyphs); w; w--, p2 += 4) {
            DBG("here %02x %02x %02x %02x\n", p2[0], p2[1], p2[2], p2[3]);
            if (p2[3] == 0 || !(p2[0] || p2[1] || p2[2])) continue;
            p2[0] = r;
            p2[1] = g;
            p2[2] = b;
        }
        p1 += gdk_pixbuf_get_rowstride(glyphs);
    }
    DBG("here\n");
    RET();
}

static int
dclock_constructor(plugin *p)
{
    line s;
    dclock *dc;
    
    ENTER;
    ERR("dclock: use 'tclock' plugin for text version of a time and date\n");
    dc = g_new0(dclock, 1);
    g_return_val_if_fail(dc != NULL, 0);
    p->priv = dc;
    dc->pwid = p->pwid;
    dc->glyphs = gdk_pixbuf_new_from_file(
        PREFIX "/share/fbpanel/images/dclock_glyphs.png", NULL);
    if (!dc->glyphs)
        RET(0);

    dc->clock = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8,
          COLON_WIDTH + 4 * DIGIT_WIDTH, DIGIT_HEIGHT + DIGIT_PAD_H);
    gdk_pixbuf_fill(dc->clock, 0);
    s.len = 256;
    dc->cfmt = dc->tfmt = dc->action = 0;
    dc->color = 0xff000000;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "dclock: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "TooltipFmt"))
                dc->tfmt = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "ClockFmt")) {
                if (strcmp(s.t[0], CLOCK_12H_FMT) &&
                      strcmp(s.t[0], CLOCK_24H_FMT)) {
                    ERR("dclock: in this version ClockFmt is limited to one of these\n");
                    ERR("dclock: %s\n", CLOCK_12H_FMT);
                    ERR("dclock: %s\n", CLOCK_24H_FMT);
                } else 
                    dc->cfmt = g_strdup(s.t[1]);
            } else if (!g_ascii_strcasecmp(s.t[0], "Color")) {
                GdkColor color;
                if (gdk_color_parse (s.t[1], &color)) 
                    dc->color = gcolor2rgb24(&color);
            }  else if (!g_ascii_strcasecmp(s.t[0], "Action"))
                dc->action = g_strdup(s.t[1]);
            else {
                ERR( "dclock: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "dclock: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    if (!dc->cfmt)
        dc->cfmt = g_strdup(CLOCK_24H_FMT);
    if (!dc->tfmt)
        dc->tfmt = g_strdup(DEFAULT_TIP_FORMAT);
    if (dc->color != 0xff000000)
        dclock_set_color(dc->glyphs, dc->color);
    dc->main = gtk_image_new_from_pixbuf(dc->clock);
    gtk_misc_set_alignment(GTK_MISC(dc->main), 0.5, 0.5);
    gtk_misc_set_padding(GTK_MISC(dc->main), 4, 0);
    gtk_container_add(GTK_CONTAINER(p->pwid), dc->main);
    //gtk_widget_show(dc->clockw);
    if (dc->action)
        g_signal_connect (G_OBJECT (dc->pwid), "button_press_event",
              G_CALLBACK (clicked), (gpointer) dc);
    gtk_widget_show_all(dc->main);
    dc->tip = gtk_tooltips_new();
    dc->timer = g_timeout_add(1000, (GSourceFunc) clock_update, (gpointer)dc);
    
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
      g_source_remove(dc->timer);
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
    description : "Digital blue clock with tooltip",

    constructor : dclock_constructor,
    destructor  : dclock_destructor,
};
