/*
 * CPU usage plugin to fbpanel
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



#include "misc.h"
#include "../chart/chart.h"

//#define DEBUGPRN
#include "dbg.h"

/* cpu.c */
struct cpu_stat {
    gulong u, n, s, i, w; // user, nice, system, idle, wait
};

typedef struct {
    chart_priv chart;
    struct cpu_stat cpu_prev;
    int timer;
    gchar *colors[1];
} cpu_priv;



static chart_class *k;

static void cpu_destructor(plugin_instance *p);

#if defined __linux__
static int
cpu_get_load(cpu_priv *c)
{
    gfloat a, b;
    struct cpu_stat cpu, cpu_diff;
    FILE *stat;
    float total;
    gchar buf[40];

    ENTER;
    total = 0;
    stat = fopen("/proc/stat", "r");
    if(!stat)
        goto end;
    fscanf(stat, "cpu %lu %lu %lu %lu %lu", &cpu.u, &cpu.n, &cpu.s,
        &cpu.i, &cpu.w);
    fclose(stat);

    cpu_diff.u = cpu.u - c->cpu_prev.u;
    cpu_diff.n = cpu.n - c->cpu_prev.n;
    cpu_diff.s = cpu.s - c->cpu_prev.s;
    cpu_diff.i = cpu.i - c->cpu_prev.i;
    cpu_diff.w = cpu.w - c->cpu_prev.w;
    c->cpu_prev = cpu;

    a = cpu_diff.u + cpu_diff.n + cpu_diff.s;
    b = a + cpu_diff.i + cpu_diff.w;
    total = b ?  a / b : 1.0;
    
end:    
    DBG("total=%f a=%f b=%f\n", total, a, b);
    g_snprintf(buf, sizeof(buf), "<b>Cpu:</b> %d%%", (int)(total * 100));
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, buf);
    k->add_tick(&c->chart, &total);
    RET(TRUE);

}
#else
static int
cpu_get_load(cpu_priv *c)
{
    ENTER;
    RET(0);
}
#endif

static int
cpu_constructor(plugin_instance *p)
{
    cpu_priv *c;
    
    if (!(k = class_get("chart")))
        RET(0);
    if (!PLUGIN_CLASS(k)->constructor(p))
        RET(0);
    c = (cpu_priv *) p;
    c->colors[0] = "green";
    XCG(p->xc, "Color", &c->colors[0], str);
    
    k->set_rows(&c->chart, 1, c->colors);
    gtk_widget_set_tooltip_markup(((plugin_instance *)c)->pwid, "<b>Cpu</b>");
    cpu_get_load(c);
    c->timer = g_timeout_add(1000, (GSourceFunc) cpu_get_load, (gpointer) c);
    RET(1);
}


static void
cpu_destructor(plugin_instance *p)
{
    cpu_priv *c = (cpu_priv *) p;

    ENTER;
    g_source_remove(c->timer);
    PLUGIN_CLASS(k)->destructor(p);
    class_put("chart");
    RET();
}



static plugin_class class = {
    .count       = 0,
    .type        = "cpu",
    .name        = "Cpu usage",
    .version     = "1.0",
    .description = "Display cpu usage",
    .priv_size   = sizeof(cpu_priv),
    .constructor = cpu_constructor,
    .destructor  = cpu_destructor,
};

static plugin_class *class_ptr = (plugin_class *) &class;
