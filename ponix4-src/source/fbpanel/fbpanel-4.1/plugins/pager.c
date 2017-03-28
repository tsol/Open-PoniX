/* pager.c -- pager module of fbpanel project
 *
 * Copyright (C) 2002-2003 Anatoly Asviyan <aanatoly@users.sf.net>
 *                         Joe MacDonald   <joe@deserted.net>
 *
 * This file is part of fbpanel.
 *
 * fbpanel is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * fbpanel is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with sawfish; see the file COPYING.   If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"


//#define DEBUG
#include "dbg.h"

/* managed window: all related info that wm holds about its managed windows */
typedef struct task {
    Window win;
    int x, y, w, h;
    gint refcount;
    guint stacking;
    guint desktop;
    char *name, *iname;
    int ws;
    net_wm_state nws;
    net_wm_window_type nwwt;
    guint focused:1;
} task;

typedef struct _desk   desk;
typedef struct _pager  pager;

#define MAX_DESK_NUM   20
/* map of a desktop */
struct _desk {
    GtkWidget *da;
    GdkPixmap *pix;
    int no, dirty, first;
    pager *pg;
};

struct _pager {
    GtkWidget *box, *eb;
    desk *desks[MAX_DESK_NUM];
    guint desknum;
    guint curdesk;
    int dw, dh;
    gfloat scalex, scaley;
    Window *wins;
    int winnum, dirty;
    GHashTable* htable;
    task *focusedtask;
};



#define TASK_VISIBLE(tk) \
 (!( ((tk)->ws != NormalState) || (tk)->nws.hidden || (tk)->nws.skip_pager ))

//if (t->nws.skip_pager || t->nwwt.desktop /*|| t->nwwt.dock || t->nwwt.splash*/ )
   
static void pager_rebuild_all(FbEv *ev, pager *pg);
static void pager_redraw_if_dirty(pager *pg);
/*
static void desk_new_pixmap(desk *d);
static void desk_clear_pixmap(desk *d);
static void desk_generate_expose_event(desk *d);
static gboolean remove_stale_tasks(Window *win, task *t, pager *p);
static gboolean remove_all_tasks(Window *win, task *t, pager *p);
*/

/*****************************************************************
 * Desk Functions                                                *
 *****************************************************************/
static void
desk_clear_pixmap(desk *d)
{
    GtkWidget *widget;

    ENTER;
    DBG("d->no=%d\n", d->no);
    if (!d->pix)
        RET();
    widget = GTK_WIDGET(d->da);
    gdk_draw_rectangle (d->pix,
          ((d->no == d->pg->curdesk) ? 
                widget->style->dark_gc[GTK_STATE_SELECTED] :
                widget->style->dark_gc[GTK_STATE_NORMAL]),
          TRUE, 
          0, 0,
          widget->allocation.width,
          widget->allocation.height);
    
    RET();
}

static inline void
desk_set_dirty(pager *p, task *t) 
{
    ENTER;
    if (t->nws.skip_pager || t->nwwt.desktop /*|| t->nwwt.dock || t->nwwt.splash*/ )
        RET();
    if (t->desktop < p->desknum)
        p->desks[t->desktop]->dirty = 1;
    else
        p->dirty = 1;
    RET();
}

static void
desk_generate_expose_event(desk *d)
{
    GdkRectangle r;
    GtkWidget *widget;
    
    ENTER;
    widget = GTK_WIDGET(d->da);                                   
    r.x = r.y = 0;
    r.width = widget->allocation.width;
    r.height = widget->allocation.height;
    gtk_widget_queue_draw_area (widget, r.x, r.y, r.width, r.height); /* leads to expose event */
    RET();
}

/* Redraw the screen from the backing pixmap */
static gint
desk_expose_event (GtkWidget *widget, GdkEventExpose *event, desk *d)
{
    ENTER;
    DBG("d=%p  widget=%p d->da=%p d->no=%d d->pix=%p\n",
          d, widget, d->da, d->no, d->pix);
   
    gdk_draw_drawable(widget->window,
          widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
          d->pix,
          event->area.x, event->area.y,
          event->area.x, event->area.y,
          event->area.width, event->area.height);
    RET(FALSE);
}

/* Upon realize and every resize creates a new backing pixmap of the appropriate size */
static gint
desk_configure_event (GtkWidget *widget, GdkEventConfigure *event, desk *d)
{
    ENTER;
    DBG("d=%p  d->da=%p d->no=%d d->pix=%p\n", 
          d, d->da, d->no, d->pix);
    if (d->pix)
        g_object_unref(d->pix);
    
    d->pix = gdk_pixmap_new(widget->window,
          widget->allocation.width,
          widget->allocation.height,
          -1);    
    desk_clear_pixmap(d);
    d->dirty = 1;
    pager_redraw_if_dirty(d->pg);
    
    RET(TRUE);
}

static gint
desk_button_press_event(GtkWidget * widget, GdkEventButton * event, desk *d)
{
    ENTER;
    DBG("s=%d\n", d->no);
    Xclimsg(GDK_ROOT_WINDOW(), a_NET_CURRENT_DESKTOP, d->no, 0, 0, 0, 0);
    RET(TRUE);
}

/*
static gint
desk_button_release_event(GtkWidget * widget, GdkEventButton * event, desk *d)
{
    ENTER;
    DBG("t=%d\n", d->no);
    Xclimsg(GDK_ROOT_WINDOW(), a_NET_CURRENT_DESKTOP, d->no, 0, 0, 0, 0);
    RET(TRUE);
}
*/

static gint
desk_scroll_event (GtkWidget *widget, GdkEventScroll *event, desk *d)
{
    int i;

    ENTER;
    DBG("scroll direction = %d\n", event->direction);
    i = d->pg->curdesk;
    if (event->direction == GDK_SCROLL_UP ||event->direction == GDK_SCROLL_LEFT) {
        i--;
        if (i < 0)
            i = d->pg->desknum - 1;
    } else {
        i++;
        if (i >= d->pg->desknum)
            i = 0;
    }
    Xclimsg(GDK_ROOT_WINDOW(), a_NET_CURRENT_DESKTOP, i, 0, 0, 0, 0);
    RET(TRUE);
}

static void
desk_new(pager *pg, int i)
{
    desk *d;

    ENTER;
    g_assert(i < pg->desknum);
    d = pg->desks[i] = g_new0(desk, 1);

    d->pg = pg;
    d->da = gtk_drawing_area_new();
    d->pix = NULL;
    d->dirty = 0;
    d->first = 1;
    d->no = i;
    gtk_widget_set_size_request(GTK_WIDGET(d->da), pg->dw, pg->dh);
    gtk_widget_set_events (d->da, GDK_EXPOSURE_MASK
          | GDK_BUTTON_PRESS_MASK
          | GDK_BUTTON_RELEASE_MASK);
          
    gtk_box_pack_start(GTK_BOX(pg->box), d->da, FALSE, TRUE, 0);
    gtk_widget_show(d->da);

    g_signal_connect (G_OBJECT (d->da), "expose_event",
          (GCallback) desk_expose_event, (gpointer)d);
    g_signal_connect (G_OBJECT (d->da), "configure_event",
          (GCallback) desk_configure_event, (gpointer)d);
    g_signal_connect (G_OBJECT (d->da), "button_press_event",
          (GCallback) desk_button_press_event, (gpointer)d);
    /*
    g_signal_connect (G_OBJECT (d->da), "button_release_event",
    (GCallback) desk_button_release_event, (gpointer)d);
    */
    g_signal_connect (G_OBJECT (d->da), "scroll-event",
          (GCallback) desk_scroll_event, (gpointer)d);       
    RET();
}

static void
desk_free(pager *pg, int i)
{
    desk *d;

    ENTER;
    d = pg->desks[i];
    DBG("i=%d d->no=%d d->da=%p d->pix=%p\n",
          i, d->no, d->da, d->pix);
    if (d->pix)
        g_object_unref(d->pix);
    gtk_widget_destroy(d->da);
    g_free(d);
    RET();
}


/*****************************************************************
 * Task Management Routines                                      *
 *****************************************************************/


/* tell to remove element with zero refcount */
static gboolean
remove_stale_tasks(Window *win, task *t, pager *p)
{
    if (t->refcount-- == 0) {
        desk_set_dirty(p, t);
        if (p->focusedtask == t)
            p->focusedtask = NULL;
        DBG("del %x\n", t->win);
        g_free(t);
        return TRUE;
    }
    return FALSE;
}

/* tell to remove element with zero refcount */
static gboolean
remove_all_tasks(Window *win, task *t, pager *p)
{
    g_free(t);
    return TRUE;
}


static void
get_sizepos(task *t)
{
    Window root, junkwin;
    int dummy, rx, ry;
    XWindowAttributes win_attributes;
    
    ENTER;
    if (!XGetWindowAttributes(GDK_DISPLAY(), t->win, &win_attributes)) {
        if (!XGetGeometry (GDK_DISPLAY(), t->win, &root, &t->x, &t->y, &t->w, &t->h,
                  &dummy, &dummy)) {
            t->x = t->y = t->w = t->h = 2;
        }
            
    } else {
        XTranslateCoordinates (GDK_DISPLAY(), t->win, win_attributes.root, 
              -win_attributes.border_width,
              -win_attributes.border_width,
              &rx, &ry, &junkwin);
        t->x = rx;
        t->y = ry;
        t->w = win_attributes.width;
        t->h = win_attributes.height;
        DBG("win=0x%x WxH=%dx%d\n", t->win,t->w, t->h);
    }
    RET();
}

/*****************************************************************
 * Netwm/WM Interclient Communication                            *
 *****************************************************************/

static void
do_net_active_window(FbEv *ev, pager *p)
{
    Window *fwin;
    task *t;

    ENTER;
    fwin = get_xaproperty(GDK_ROOT_WINDOW(), a_NET_ACTIVE_WINDOW, XA_WINDOW, 0);
    DBG("win=%x\n", fwin ? *fwin : 0);
    if (fwin) {
        t = g_hash_table_lookup(p->htable, fwin);
        if (t != p->focusedtask) {
            if (p->focusedtask)
                desk_set_dirty(p, p->focusedtask);
            p->focusedtask = t;
            if (t)
                desk_set_dirty(p, t);
        }
        XFree(fwin);
    } else {
        if (p->focusedtask) {
            desk_set_dirty(p, p->focusedtask);
            p->focusedtask = NULL;
        }
    }
    pager_redraw_if_dirty(p);
    RET();
}

static void
do_net_current_desktop(FbEv *ev, pager *p)
{
    ENTER;
    p->desks[p->curdesk]->dirty = 1;
    p->curdesk =  get_net_current_desktop ();
    if (p->curdesk >= p->desknum)
        p->curdesk = 0;
    p->desks[p->curdesk]->dirty = 1;
    pager_redraw_if_dirty(p);
    RET();
}


static void
do_net_client_list_stacking(FbEv *ev, pager *p)
{
    int i;
    task *t;

    ENTER;
    if (p->wins)
        XFree(p->wins);
    p->wins = get_xaproperty (GDK_ROOT_WINDOW(), a_NET_CLIENT_LIST_STACKING, XA_WINDOW, &p->winnum);
    if (!p->wins || !p->winnum)
        RET();

    /* refresh existing tasks and add new */
    for (i = 0; i < p->winnum; i++) {
        if ((t = g_hash_table_lookup(p->htable, &p->wins[i]))) {
            t->refcount++;
            if (t->stacking != i) {
                t->stacking = i;
                desk_set_dirty(p, t);
            }
        } else {
            t = g_new0(task, 1);
            t->refcount++;
            t->win = p->wins[i];
            t->ws = get_wm_state (t->win);
            t->desktop = get_net_wm_desktop(t->win);
            get_net_wm_state(t->win, &t->nws);
            get_net_wm_window_type(t->win, &t->nwwt);
            get_sizepos(t);
            if (!gdk_window_lookup(t->win))
                XSelectInput (GDK_DISPLAY(), t->win, PropertyChangeMask | StructureNotifyMask);
            g_hash_table_insert(p->htable, &t->win, t);
            DBG("add %x\n", t->win);
            desk_set_dirty(p, t);
        }
    }
    /* pass throu hash table and delete stale windows */
    g_hash_table_foreach_remove(p->htable, (GHRFunc) remove_stale_tasks, (gpointer)p);
    pager_redraw_if_dirty(p);
    RET();
}


/*****************************************************************
 * Pager Functions                                               *
 *****************************************************************/
/*
static void
pager_unmapnotify(pager *p, XEvent *ev)
{
    Window win = ev->xunmap.window;
    task *t;
    if (!(t = g_hash_table_lookup(p->htable, &win))) 
        RET();
    DBG("pager_unmapnotify: win=0x%x\n", win);
    RET();
    t->ws = WithdrawnState;
    desk_set_dirty(p, t);
    pager_redraw_if_dirty(p);
    RET();
}
*/
static void
pager_configurenotify(pager *p, XEvent *ev)
{
    Window win = ev->xconfigure.window;
    task *t;

    ENTER;
 
    if (!(t = g_hash_table_lookup(p->htable, &win))) 
        RET();
    DBG("win=0x%x\n", win);
    get_sizepos(t);
    desk_set_dirty(p, t);
    pager_redraw_if_dirty(p);
    RET();
}

static void
pager_propertynotify(pager *p, XEvent *ev)
{
    Atom at = ev->xproperty.atom;
    Window win = ev->xproperty.window;
    task *t;

    
    ENTER;
    if ((win == GDK_ROOT_WINDOW()) || !(t = g_hash_table_lookup(p->htable, &win)))
        RET();
      
    DBG("window=0x%x\n", t->win);
    if (at == a_WM_STATE)    {
        DBG("event=WM_STATE\n");
        t->ws = get_wm_state (t->win);
    } else if (at == a_NET_WM_STATE) {
        DBG("event=NET_WM_STATE\n");
        get_net_wm_state(t->win, &t->nws);
    } else if (at == a_NET_WM_DESKTOP) {	   
        DBG("event=NET_WM_DESKTOP\n");
        desk_set_dirty(p, t); // to clean up desks where this task was 
        t->desktop = get_net_wm_desktop(t->win);
    } else {
        RET();
    }
    desk_set_dirty(p, t);
    pager_redraw_if_dirty(p);        
    RET();
}

static GdkFilterReturn
pager_event_filter( XEvent *xev, GdkEvent *event, pager *pg)
{
    ENTER;
    if (xev->type == PropertyNotify )
        pager_propertynotify(pg, xev);
    else if (xev->type == ConfigureNotify )
        pager_configurenotify(pg, xev);
    RET(GDK_FILTER_CONTINUE);
}


static void
pager_redraw_if_dirty(pager *pg)
{
    int i, j, x, y, w, h;
    GtkWidget *widget;
    task *t;
    desk *d;

    ENTER;
    if (pg->dirty)
        for (i = 0; i < pg->desknum; i++) {
            pg->desks[i]->dirty = 1;
        }
    for (i = 0; i < pg->desknum; i++) 
        if (pg->desks[i]->dirty) {
            desk_clear_pixmap(pg->desks[i]);
        }

  
    for (j = 0; j < pg->winnum; j++) {
        if (!(t = g_hash_table_lookup(pg->htable, &pg->wins[j])))
            continue;
        if (!TASK_VISIBLE(t))
            continue;

        x = (gfloat)t->x * pg->scalex;
        y = (gfloat)t->y * pg->scaley;
        w = (gfloat)t->w * pg->scalex;
        h = (t->nws.shaded) ? 2 : (gfloat)t->h * pg->scaley;
        if (w < 2 || h < 2)
            continue;
        i = (t->desktop < pg->desknum) ? t->desktop : 0;
        do {
            d = pg->desks[i];
            if (!d->dirty || !d->pix) 
                continue;
            widget = GTK_WIDGET(d->da); 
            gdk_draw_rectangle (d->pix, 
                  (pg->focusedtask == t) ?       
                  widget->style->bg_gc[GTK_STATE_SELECTED] :
                  widget->style->bg_gc[GTK_STATE_NORMAL], 
                  TRUE,                         
                  x+1, y+1, w-1, h-1);                
            gdk_draw_rectangle (d->pix, 
                  (pg->focusedtask == t) ?       
                  widget->style->fg_gc[GTK_STATE_SELECTED] :
                  widget->style->fg_gc[GTK_STATE_NORMAL], 
                  FALSE,
                  x, y, w, h);
        } while ((t->desktop > pg->desknum) && ++i < pg->desknum);
    }

    for (i = 0; i < pg->desknum; i++) 
        if (pg->desks[i]->dirty) {
            desk_generate_expose_event(pg->desks[i]);
            pg->desks[i]->dirty = 0;
        }
    pg->dirty = 0;
    RET();
}



static void
pager_rebuild_all(FbEv *ev, pager *pg)
{
    int desknum, curdesk, dif, i;

    ENTER;
    desknum = pg->desknum;
    curdesk = pg->curdesk;
    
    pg->desknum = get_net_number_of_desktops();
    if (pg->desknum < 1)
        pg->desknum = 1;
    else if (pg->desknum > MAX_DESK_NUM) {
        pg->desknum = MAX_DESK_NUM;
        ERR("pager: max number of supported desks is %d\n", MAX_DESK_NUM);
    }
    pg->curdesk = get_net_current_desktop();
    if (pg->curdesk >= pg->desknum)
        pg->curdesk = 0;
    DBG("desknum=%d curdesk=%d\n", desknum, curdesk);
    DBG("pg->desknum=%d pg->curdesk=%d\n", pg->desknum, pg->curdesk);
    dif = pg->desknum - desknum;

    if (dif == 0)
        RET();;

    if (dif < 0) {
        /* if desktops were deleted then delete their maps also */
        for (i = desknum; i > pg->desknum; i--) 
            desk_free(pg, i-1);
        //pg->desks = g_renew(desk, pg->desks, pg->desknum);
    } else {
        //pg->desks = g_renew(desk, pg->desks, pg->desknum);
        for (i = desknum; i < pg->desknum; i++) 
            desk_new(pg, i);
    }
    pg->dirty = 1;
    RET();
}
/* Upon realize and every resize creates a new backing pixmap of the appropriate size */
static void
pager_map_event (GtkWidget *widget, pager *p)
{
    ENTER;
    do_net_client_list_stacking(NULL, p);
    do_net_active_window(fbev, p);
    RET();
}

static int
pager_constructor(plugin *p)
{

    line s;
    pager *pg;
    
    ENTER;
    s.len = 256;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        ERR("pager: illegal in this context %s\n", s.str);
        RET(0);
    }
    pg = g_new0(pager, 1);
    g_return_val_if_fail(pg != NULL, 0);
    p->priv = pg;

    pg->htable = g_hash_table_new (g_int_hash, g_int_equal);
    // pager is wrapped into eventbox to ensure non-transparent bg
    pg->eb = gtk_event_box_new();
    gtk_container_set_border_width (GTK_CONTAINER (pg->eb), 0);
    gtk_widget_show(pg->eb);
    pg->box = p->panel->my_box_new(TRUE, 1);
    gtk_container_set_border_width (GTK_CONTAINER (pg->box), 1);
    gtk_widget_show(pg->box);
    g_signal_connect (G_OBJECT (pg->box), "map",
          (GCallback) pager_map_event, (gpointer)pg);
    gtk_container_add(GTK_CONTAINER(pg->eb), pg->box);
    gtk_container_add(GTK_CONTAINER(p->pwid), pg->eb);
    if (p->panel->orientation == ORIENT_HORIZ) {
        pg->dh = p->panel->ah - 2;
        pg->dw = (float)pg->dh * (float)gdk_screen_width() / (float) gdk_screen_height();
    } else {
        pg->dw = p->panel->aw - 2;
        pg->dh = (float)pg->dw * (float)gdk_screen_height() / (float) gdk_screen_width();
    }
    pg->scaley = (gfloat)pg->dh / (gfloat)gdk_screen_height();
    pg->scalex = (gfloat)pg->dw / (gfloat)gdk_screen_width();

    //pg->htable = g_hash_table_new (g_int_hash, g_int_equal);
    pager_rebuild_all(fbev, pg);
    do_net_current_desktop(fbev, pg);
    do_net_client_list_stacking(fbev, pg);
    
    pager_redraw_if_dirty(pg);
    gdk_window_add_filter(NULL, (GdkFilterFunc)pager_event_filter, pg );
    g_signal_connect (G_OBJECT (fbev), "current_desktop",
          G_CALLBACK (do_net_current_desktop), (gpointer) pg);
    g_signal_connect (G_OBJECT (fbev), "active_window",
          G_CALLBACK (do_net_active_window), (gpointer) pg);
    g_signal_connect (G_OBJECT (fbev), "number_of_desktops",
          G_CALLBACK (pager_rebuild_all), (gpointer) pg);
    g_signal_connect (G_OBJECT (fbev), "client_list_stacking",
          G_CALLBACK (do_net_client_list_stacking), (gpointer) pg);
    
    RET(1);
}

static void
pager_destructor(plugin *p)
{
    pager *pg = (pager *)p->priv;

    ENTER;
    g_signal_handlers_disconnect_by_func(G_OBJECT (fbev), do_net_current_desktop, pg);
    g_signal_handlers_disconnect_by_func(G_OBJECT (fbev), do_net_active_window, pg);
    g_signal_handlers_disconnect_by_func(G_OBJECT (fbev), pager_rebuild_all, pg);
    g_signal_handlers_disconnect_by_func(G_OBJECT (fbev), do_net_client_list_stacking, pg);   
    gdk_window_remove_filter(NULL, (GdkFilterFunc)pager_event_filter, pg);
    while (--pg->desknum) {
        desk_free(pg, pg->desknum);
    }
    g_hash_table_foreach_remove(pg->htable, (GHRFunc) remove_all_tasks, (gpointer)pg);
    g_hash_table_destroy(pg->htable);
    gtk_widget_destroy(pg->eb);
    g_free(pg);
    RET();
}


plugin_class pager_plugin_class = {
    fname: NULL,
    count: 0,

    type : "pager",
    name : "pager",
    version: "1.0",
    description : "Simple pager plugin",

    constructor : pager_constructor,
    destructor  : pager_destructor,
};
