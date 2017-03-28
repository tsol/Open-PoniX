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

#include "plugin.h"
#include "panel.h"
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <sys/sysinfo.h>

#define KILOBYTE 1024
#define MAX_WGSIZE 100

//#define DEBUG
#include "dbg.h"

struct cpu_stat
{
    unsigned long long int a;
    unsigned long long int b;
    unsigned long long int c;
    unsigned long long int d;
    struct timeval t;
};


typedef struct {
    GdkGC *gc_cpu;
    GdkColor *ccpu;
    GtkWidget *da;
    GtkWidget *evbox;
    GdkPixmap *pixmap;
    GtkTooltips *tip;

    int timer;
    unsigned long long int stats_cpu[MAX_WGSIZE];
    unsigned int ini_stats;
    int Wwg;
    int Hwg;
    struct cpu_stat cpu_anterior;
} cpu_t;

#if 0
static const char *
make_human_readable_str(unsigned long long val)
{
    /* The code will adjust for additional (appended) units. */
    static char zero_and_units[] = { '0', 0, 'K', 'M', 'G', 'T' };
    static const char fmt[] = "%Lu";
    static const char fmt_tenths[] = "%Lu.%d%c";
    
    static char str[21];		/* Sufficient for 64 bit unsigned integers. */
    
    int frac;
    const char *u;
    const char *f;
    
    u = zero_and_units;
    f = fmt;
    frac = 0;
    
    if (val == 0) {
	return u;
    }
    
    u++;
    while ((val >= KILOBYTE)
          && (u < zero_and_units + sizeof(zero_and_units) - 1)) {
	f = fmt_tenths;
	++u;
	frac = ((((int)(val % KILOBYTE)) * 10) + (KILOBYTE/2)) / KILOBYTE;
	val /= KILOBYTE;
    }
    if (frac >= 10) {		/* We need to round up here. */
	++val;
	frac = 0;
    }
    
    /* If f==fmt then 'frac' and 'u' are ignored. */
    snprintf(str, sizeof(str), f, val, frac, *u);
    
    return str;
}
#endif

static int
cpu_update(cpu_t *c)
{
    //char  buf[1024];
    int cpu_u=0, cpu_s=0, cpu_n=0, cpu_i=100;
    int mem_t, mem_u, mem_f;
    unsigned int i, j; //, pchars;
    struct sysinfo sinfo;
    
    struct cpu_stat cpu, cpu_r;
    struct timezone *tz=NULL;
    FILE *stat;

    ENTER;
    if(!c->pixmap)
       RET(TRUE); 
     
    stat = fopen("/proc/stat", "r");
    if(!stat)
        RET(TRUE);
    fscanf(stat, "cpu %lld %lld %lld %lld", &cpu.a, &cpu.b, &cpu.c, &cpu.d);
    gettimeofday(&cpu.t, tz);
    fclose(stat);

    cpu_r.a = cpu.a - c->cpu_anterior.a;
    cpu_r.b = cpu.b - c->cpu_anterior.b;
    cpu_r.c = cpu.c - c->cpu_anterior.c;
    cpu_r.d = cpu.d - c->cpu_anterior.d;
    cpu_r.t.tv_usec = cpu.t.tv_usec - c->cpu_anterior.t.tv_usec;
    cpu_r.t.tv_sec = cpu.t.tv_sec - c->cpu_anterior.t.tv_sec;

    if(c->cpu_anterior.t.tv_usec>0)
    {
	cpu_u = cpu_r.a*100.0/(cpu_r.a+cpu_r.b+cpu_r.c+cpu_r.d);
	cpu_s = cpu_r.b*100.0/(cpu_r.a+cpu_r.b+cpu_r.c+cpu_r.d);
	cpu_n = cpu_r.c*100.0/(cpu_r.a+cpu_r.b+cpu_r.c+cpu_r.d);
	cpu_i = cpu_r.d*100.0/(cpu_r.a+cpu_r.b+cpu_r.c+cpu_r.d);
    }
    c->cpu_anterior = cpu;
    
    c->stats_cpu[c->ini_stats++] = cpu_u + cpu_s + cpu_n;
    c->ini_stats = c->ini_stats % c->Wwg;
    
    sysinfo(&sinfo);
    mem_t = sinfo.totalram;
    mem_f = sinfo.freeram + sinfo.bufferram;
    mem_u = mem_t - mem_f;

    /*
    pchars = snprintf(buf, 1024, "Cpu use\nTotal: %d\nUser: %d\nSystem: %d\nNice: %d\nIdle: %d\n\nRam memory\n",
          cpu_u + cpu_s + cpu_n, cpu_u, cpu_s, cpu_n, cpu_i);

    pchars += snprintf(&buf[pchars], 1024-pchars, "Total: %6s\n", make_human_readable_str(mem_t));
    pchars += snprintf(&buf[pchars], 1024-pchars, "Used: %6s\n", make_human_readable_str(mem_u));
    pchars += snprintf(&buf[pchars], 1024-pchars, "Free: %6s\n", make_human_readable_str(mem_f));
    pchars += snprintf(&buf[pchars], 1024-pchars, "\nSwap\nTotal: %6s\n", make_human_readable_str(sinfo.totalswap));
    pchars += snprintf(&buf[pchars], 1024-pchars, "Used: %6s\n", make_human_readable_str(sinfo.totalswap - sinfo.freeswap));
    pchars += snprintf(&buf[pchars], 1024-pchars, "Free: %6s\n", make_human_readable_str(sinfo.freeswap));
    pchars += snprintf(&buf[pchars], 1024-pchars, "\nEstatistics\nNumber of process: %d\n", sinfo.procs);
    pchars += snprintf(&buf[pchars], 1024-pchars, "Uptime: %ldh\nLoad average: %ld %ld %ld",
          sinfo.uptime/3600, sinfo.loads[0], sinfo.loads[1], sinfo.loads[2]);

    if(c->evbox)
	gtk_tooltips_set_tip(c->tip, c->evbox, buf, NULL);
    */
 
 
    
    gdk_draw_rectangle(c->pixmap, c->da->style->black_gc, TRUE, 0, 0, c->Wwg, c->Hwg);
    for(i=c->ini_stats, j=0; i<c->ini_stats+c->Wwg; i++, j++)
    {
	int val;
	
	val = (int)(c->Hwg*(1.0*(c->stats_cpu[i%c->Wwg])/100));
	gdk_draw_line(c->pixmap, c->gc_cpu, j, c->Hwg, j, c->Hwg - val);
    }
    gtk_widget_queue_draw(c->da);
    RET(TRUE);
}

static gint
configure_event(GtkWidget *widget, GdkEventConfigure *event, cpu_t *c)
{
    ENTER;
    if (c->pixmap)
        g_object_unref(c->pixmap);
    c->pixmap = gdk_pixmap_new (widget->window,
          widget->allocation.width,
          widget->allocation.height,
          -1);
    gdk_draw_rectangle (c->pixmap,
          widget->style->black_gc,
          TRUE,
          0, 0,
          widget->allocation.width,
          widget->allocation.height);
    
   RET(TRUE);
}


static gint
expose_event(GtkWidget *widget, GdkEventExpose *event, cpu_t *c)
{
    ENTER;
    gdk_draw_drawable (widget->window,
          c->da->style->black_gc,
          c->pixmap,
          event->area.x, event->area.y,
          event->area.x, event->area.y,
          event->area.width, event->area.height);
    
    RET(FALSE);
}

static int
cpu_constructor(plugin *p)
{
    cpu_t *c;

    ENTER;
    c = g_new0(cpu_t, 1);
    p->priv = c;
    
    if (p->panel->orientation == ORIENT_VERT)
    {
	c->Wwg = p->panel->aw-2;
	c->Hwg = p->panel->aw-2;
    }else	
    {
	c->Wwg = p->panel->ah-2;
	c->Hwg = p->panel->ah-2;
    };	
    
    c->da = gtk_drawing_area_new();
    gtk_widget_set_size_request(c->da, c->Wwg, c->Hwg);
    c->evbox = gtk_event_box_new();

    gtk_container_add(GTK_CONTAINER(c->evbox), c->da);
    gtk_widget_show(c->evbox);
    gtk_widget_show(c->da);

    c->tip = gtk_tooltips_new();
 
    c->gc_cpu = gdk_gc_new(p->panel->topgwin->window);
    DBG("here1\n");
    c->ccpu = (GdkColor *)malloc(sizeof(GdkColor));
    gdk_color_parse("green",  c->ccpu);
    gdk_colormap_alloc_color(gdk_drawable_get_colormap(p->panel->topgwin->window),  c->ccpu, FALSE, TRUE);
    gdk_gc_set_foreground(c->gc_cpu,  c->ccpu);
 
    gtk_container_add(GTK_CONTAINER(p->pwid), c->evbox);

    g_signal_connect (G_OBJECT (c->da),"configure_event",
          G_CALLBACK (configure_event), (gpointer) c);
    g_signal_connect (G_OBJECT (c->da), "expose_event",
          G_CALLBACK (expose_event), (gpointer) c);
    
    c->timer = g_timeout_add(1000, (GSourceFunc) cpu_update, (gpointer) c);
    RET(1);
}

static void
cpu_destructor(plugin *p)
{
    cpu_t *c = (cpu_t *) p->priv;

    ENTER;
    g_object_unref(c->pixmap);
    g_object_unref(c->gc_cpu);
    g_free(c->ccpu);
    g_source_remove(c->timer);
    g_free(p->priv);
    RET();
}


plugin_class cpu_plugin_class = {
    fname: NULL,
    count: 0,

    type : "cpu",
    name : "Cpu usage",
    version: "1.0",
    description : "Display cpu usage",

    constructor : cpu_constructor,
    destructor  : cpu_destructor,
};
