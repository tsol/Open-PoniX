/*
 * net usage plugin to fbpanel
 *
 * Copyright (C) 2004 by Alexandre Pereira da Silva <alexandre.pereira@poli.usp.br>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * 
 */
/*A little bug fixed by Mykola <mykola@2ka.mipt.ru>:) */



#include "../chart/chart.h"
#include <stdlib.h>
#include <string.h>

//#define DEBUGPRN
#include "dbg.h"


#define CHECK_PERIOD   2 /* second */

/* net.c */
struct net_stat {
    gulong tx, rx;
};

typedef struct {
    chart_priv chart;
    struct net_stat net_prev;
    int timer;
    char *iface;
    gint max_tx;
    gint max_rx;
    gulong max;
    gchar *colors[2];
} net_priv;

static chart_class *k;


static void net_destructor(plugin_instance *p);


#if defined __linux__
static int
net_get_load(net_priv *c)
{
    struct net_stat net, net_diff;
    FILE *stat;
    float total[2];
    char buf[256], *s = NULL;

    ENTER;
    memset(&net, 0, sizeof(net));
    memset(&net_diff, 0, sizeof(net_diff));
    stat = fopen("/proc/net/dev", "r");
    if(!stat)
        goto end;
    fgets(buf, 256, stat);
    fgets(buf, 256, stat);

    while (!s && !feof(stat) && fgets(buf, 256, stat))  
        s = g_strrstr(buf, c->iface);
    fclose(stat);
    if (!s)
        goto end;
    s = g_strrstr(s, ":");     
    if (!s)
        goto end;
    s++;
    if (sscanf(s,
            "%lu  %*d     %*d  %*d  %*d  %*d   %*d        %*d       %lu",
            &net.rx, &net.tx)!= 2) {
        DBG("can't read %s statistics\n", c->iface);
        goto end;
    }
    net_diff.tx = ((net.tx - c->net_prev.tx) >> 10) / CHECK_PERIOD;
    net_diff.rx = ((net.rx - c->net_prev.rx) >> 10) / CHECK_PERIOD;
end:
    
    c->net_prev = net;
    total[0] = (float)(net_diff.tx) / c->max;
    total[1] = (float)(net_diff.rx) / c->max;
    DBG("%f %ul %ul\n", total, net_diff.tx, net_diff.rx);
    k->add_tick(&c->chart, total);
    g_snprintf(buf, sizeof(buf), "<b>%s:</b>\nD %lu Kbs, U %lu Kbs",
        c->iface, net_diff.rx, net_diff.tx);
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, buf);
    RET(TRUE);

}
#else

static int
net_get_load(net_priv *c)
{
    ENTER;
    RET(0);
}

#endif


static int
net_constructor(plugin_instance *p)
{
    net_priv *c;

    if (!(k = class_get("chart")))
        RET(0);
    if (!PLUGIN_CLASS(k)->constructor(p))
        RET(0);
    c = (net_priv *) p;
 
    c->iface = "eth0";
    c->max_rx = 120;
    c->max_tx = 12;
    c->colors[0] = "violet";
    c->colors[1] = "blue";
    XCG(p->xc, "interface", &c->iface, str);
    XCG(p->xc, "RxLimit", &c->max_rx, int);
    XCG(p->xc, "TxLimit", &c->max_tx, int);
    XCG(p->xc, "TxColor", &c->colors[0], str);
    XCG(p->xc, "RxColor", &c->colors[1], str);

    c->max = c->max_rx + c->max_tx;
    k->set_rows(&c->chart, 2, c->colors);
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, "<b>Net</b>");
    net_get_load(c);
    c->timer = g_timeout_add(CHECK_PERIOD * 1000,
        (GSourceFunc) net_get_load, (gpointer) c);
    RET(1);
}


static void
net_destructor(plugin_instance *p)
{
    net_priv *c = (net_priv *) p;

    ENTER;
    if (c->timer)
        g_source_remove(c->timer);
    PLUGIN_CLASS(k)->destructor(p);
    class_put("chart");
    RET();
}


static plugin_class class = {
    .count       = 0,
    .type        = "net",
    .name        = "Net usage",
    .version     = "1.0",
    .description = "Display net usage",
    .priv_size   = sizeof(net_priv),

    .constructor = net_constructor,
    .destructor  = net_destructor,
};
static plugin_class *class_ptr = (plugin_class *) &class;
