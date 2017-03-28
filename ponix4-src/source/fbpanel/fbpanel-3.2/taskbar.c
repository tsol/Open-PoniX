#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
//#include <X11/xpm.h>

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk-pixbuf-xlib/gdk-pixbuf-xlib.h>
#include <gdk/gdk.h>



#include "panel.h"
#include "misc.h"
#include "plugin.h"
#include "icon.xpm"
#include "gtkhtbar.h"
#include "gtkvtbar.h"


//#define DEBUG
#include "dbg.h"
/*
 * TODO : icon_copied
 * 21/03/04 aanatoly
 * v   implement wm icon scaling ??
 *     implement net_wm_icon 
 */

typedef struct wmpix_t {
    struct wmpix_t *next;
    GdkPixbuf *pixbuf;    
    XClassHint ch;
} wmpix_t;

struct _taskbar;
typedef struct _task{
    struct _taskbar *tb;
    struct task *next;
    Window win;
    char *name, *iname;
    GtkWidget *button, *label, *sep;
    GtkWidget *image;
    
    GdkPixbuf *pixbuf;
    
    int refcount;
    XClassHint ch;
    int pos_x;
    int width;
    int desktop;
    net_wm_state nws;
    net_wm_window_type nwwt;
    unsigned int focused:1;
    unsigned int iconified:1;
    unsigned int icon_copied:1;
} task;



typedef struct _taskbar{
    plugin *plug;
    Window *wins;
    int win_num;
    GHashTable  *task_list;
    GtkWidget *hbox, *bbox, *space;
    GtkTooltips *tips;
    GdkPixbuf *gen_pixbuf;
    
    int num_tasks;
    int task_width;
    int vis_task_num;
    int req_width;
    int hbox_width;
    int cur_desk;
    task *focused;
    char **desk_names;
    int desk_namesno;
    int desk_num;
    wmpix_t *wmpix; 
    int wmpixno;
    
    int accept_skip_pager;
    unsigned int iconsize;
    unsigned int task_width_max;
    unsigned int hidden:1;
    unsigned int layer:1;
    unsigned int tooltips:1;
    unsigned int icons_only:1;
} taskbar;

#define TASK_WIDTH_MAX   200
#define TASK_PADDING     4
static void display_tk(taskbar *tb, task *tk);
static void tb_propertynotify(taskbar *tb, XEvent *ev);
static GdkFilterReturn tb_event_filter( XEvent *, GdkEvent *, taskbar *);
static void taskbar_destructor(plugin *p);


#define TASK_VISIBLE(tb, tk) \
 ((tk)->desktop == (tb)->cur_desk || (tk)->desktop == -1 /* 0xFFFFFFFF */ )

#define TASK_MANAGABLE(tk) \
 (!((tk)->skip_taskbar || (tb->accept_skip_pager && (tk)->skip_pager))) 


static int
accept_net_wm_state(net_wm_state *nws, int accept_skip_pager)
{
    ENTER;
    RET(!(nws->skip_taskbar || (accept_skip_pager && nws->skip_pager)));
}

static int
accept_net_wm_window_type(net_wm_window_type *nwwt)
{
    ENTER;
    RET(!(nwwt->desktop || nwwt->dock || nwwt->splash));
}




static void
get_wmclass(task *tk)
{
    ENTER;
    if (tk->ch.res_name)
        XFree(tk->ch.res_name);
    if (tk->ch.res_class)
        XFree(tk->ch.res_class);
    XGetClassHint (gdk_display, tk->win, &tk->ch);
    RET();
}




static void
tk_set_names(task *tk)
{
    char *name;
    
    ENTER;
    g_return_if_fail ((tk->name == NULL) && (tk->iname == NULL));
    //tk->name = tk->iname = NULL;
    name = get_textproperty(tk->win,  XA_WM_NAME);
    DBG("utf name: <%s>\n", name);
    if (name) {		
	tk->name = g_strdup_printf(" %s ", name);
	tk->iname = g_strdup_printf("[%s]", name);
	g_free(name);
    }
    RET();
}

static void
tk_free_names(task *tk)
{
    
    ENTER;
    g_free(tk->name);
    g_free(tk->iname);
    tk->name = tk->iname = NULL;    
}


static task *
find_task (taskbar * tb, Window win)
{
    ENTER;
    RET(g_hash_table_lookup(tb->task_list, &win));
}


static void
del_task (taskbar * tb, task *tk, int hdel)
{
    gtk_widget_destroy(tk->button);
    tb->num_tasks--;
    if (tk->icon_copied)
    {
        /* TODO */
    }
    tk_free_names(tk);	    
    if (tb->focused == tk)
        tb->focused = NULL;
    if (hdel)
        g_hash_table_remove(tb->task_list, &tk->win);
    g_free(tk);
}


static GdkPixbuf *
get_user_icon(taskbar *tb, task *tk)
{
    wmpix_t *tmp;

    ENTER;
    if (tk->ch.res_class) {
        for (tmp = tb->wmpix; tmp; tmp = tmp->next) {
            if ((!tmp->ch.res_name || !strcmp(tmp->ch.res_name, tk->ch.res_name))
                  && (!tmp->ch.res_class || !strcmp(tmp->ch.res_class, tk->ch.res_class))) {
                
                g_object_ref(tmp->pixbuf);
                RET(tmp->pixbuf);
	    }
	}
    }
    RET(NULL);
}


static GdkColormap*
get_cmap (GdkPixmap *pixmap)
{
  GdkColormap *cmap;

  cmap = gdk_drawable_get_colormap (pixmap);
  if (cmap)
    g_object_ref (G_OBJECT (cmap));

  if (cmap == NULL)
    {
      if (gdk_drawable_get_depth (pixmap) == 1)
        {
          /* try null cmap */
          cmap = NULL;
        }
      else
        {
          /* Try system cmap */
          GdkScreen *screen = gdk_drawable_get_screen (GDK_DRAWABLE (pixmap));
          cmap = gdk_screen_get_system_colormap (screen);
          g_object_ref (G_OBJECT (cmap));
        }
    }

  /* Be sure we aren't going to blow up due to visual mismatch */
  if (cmap &&
      (gdk_colormap_get_visual (cmap)->depth !=
       gdk_drawable_get_depth (pixmap)))
    cmap = NULL;
  
  return cmap;
}

GdkPixbuf*
_wnck_gdk_pixbuf_get_from_pixmap (GdkPixbuf   *dest,
                                  Pixmap       xpixmap,
                                  int          src_x,
                                  int          src_y,
                                  int          dest_x,
                                  int          dest_y,
                                  int          width,
                                  int          height)
{
    GdkDrawable *drawable;
    GdkPixbuf *retval;
    GdkColormap *cmap;
  
    retval = NULL;
  
    drawable = gdk_xid_table_lookup (xpixmap);

    if (drawable)
        g_object_ref (G_OBJECT (drawable));
    else
        drawable = gdk_pixmap_foreign_new (xpixmap);

    cmap = get_cmap (drawable);

    /* GDK is supposed to do this but doesn't in GTK 2.0.2,
     * fixed in 2.0.3
     */
    if (width < 0)
        gdk_drawable_get_size (drawable, &width, NULL);
    if (height < 0)
        gdk_drawable_get_size (drawable, NULL, &height);
  
    retval = gdk_pixbuf_get_from_drawable (dest,
          drawable,
          cmap,
          src_x, src_y,
          dest_x, dest_y,
          width, height);

    if (cmap)
        g_object_unref (G_OBJECT (cmap));
    g_object_unref (G_OBJECT (drawable));

    return retval;
}

static GdkPixbuf*
apply_mask (GdkPixbuf *pixbuf,
            GdkPixbuf *mask)
{
  int w, h;
  int i, j;
  GdkPixbuf *with_alpha;
  guchar *src;
  guchar *dest;
  int src_stride;
  int dest_stride;
  
  w = MIN (gdk_pixbuf_get_width (mask), gdk_pixbuf_get_width (pixbuf));
  h = MIN (gdk_pixbuf_get_height (mask), gdk_pixbuf_get_height (pixbuf));
  
  with_alpha = gdk_pixbuf_add_alpha (pixbuf, FALSE, 0, 0, 0);

  dest = gdk_pixbuf_get_pixels (with_alpha);
  src = gdk_pixbuf_get_pixels (mask);

  dest_stride = gdk_pixbuf_get_rowstride (with_alpha);
  src_stride = gdk_pixbuf_get_rowstride (mask);
  
  i = 0;
  while (i < h)
    {
      j = 0;
      while (j < w)
        {
          guchar *s = src + i * src_stride + j * 3;
          guchar *d = dest + i * dest_stride + j * 4;
          
          /* s[0] == s[1] == s[2], they are 255 if the bit was set, 0
           * otherwise
           */
          if (s[0] == 0)
            d[3] = 0;   /* transparent */
          else
            d[3] = 255; /* opaque */
          
          ++j;
        }
      
      ++i;
    }

  return with_alpha;
}

static GdkPixbuf *
get_wm_icon(Window tkwin, int iw, int ih)
{
    XWMHints *hints;
    Pixmap xpixmap = None, xmask = None;
    Window win;
    unsigned int w, h, d;
    GdkPixbuf *ret, *masked, *pixmap, *mask = NULL;
 
    ENTER;
    hints = (XWMHints *) get_xaproperty (tkwin, XA_WM_HINTS, XA_WM_HINTS, 0);
    if (!hints)
        RET(NULL);
    
    if ((hints->flags & IconPixmapHint)) 
        xpixmap = hints->icon_pixmap;
    if ((hints->flags & IconMaskHint))
        xmask = hints->icon_mask;

    XFree (hints);
    if (xpixmap == None)
        RET(NULL);
    
    XGetGeometry (GDK_DISPLAY(), xpixmap, &win, &d, &d, &w, &h, &d, &d);
    DBG("tkwin=%x icon pixmap w=%d h=%d\n", tkwin, w, h);
    pixmap = _wnck_gdk_pixbuf_get_from_pixmap (NULL, xpixmap, 0, 0, 0, 0, w, h);
    if (!pixmap)
        RET(NULL);
    if (xmask != None) {
        XGetGeometry (GDK_DISPLAY(), xmask, &win, &d, &d, &w, &h, &d, &d);
        mask = _wnck_gdk_pixbuf_get_from_pixmap (NULL, xmask, 0, 0, 0, 0, w, h);

        if (mask) {
            masked = apply_mask (pixmap, mask);
            g_object_unref (G_OBJECT (pixmap));
            g_object_unref (G_OBJECT (mask));
            pixmap = masked;
        }
    }
    if (!pixmap)
        RET(NULL);
    ret = gdk_pixbuf_scale_simple (pixmap, iw, ih, GDK_INTERP_TILES);
    g_object_unref(pixmap);

    RET(ret);
}

inline static GdkPixbuf*
get_generic_icon(taskbar *tb)
{
    ENTER;
    g_object_ref(tb->gen_pixbuf);
    RET(tb->gen_pixbuf);
}

static void
get_task_icon (taskbar *tb, task *tk)
{
    ENTER;
    g_assert ((tb != NULL) && (tk != NULL));
    g_return_if_fail(tk != NULL);


    if ((tk->pixbuf = get_user_icon(tb, tk)))
        RET();
    if ((tk->pixbuf = get_wm_icon(tk->win, tb->iconsize, tb->iconsize)))
        RET();
    tk->pixbuf = get_generic_icon(tb); // always exists 
    RET();
}



static void
callback_leave( GtkWidget *widget, gpointer   data )
{
    task *tk = (task *) data;

    ENTER;
    display_tk(tk->tb, tk);
    RET();
}


static void
button_press_event(GtkWidget *widget, GdkEventButton *event, task *tk)
{
    ENTER;
    if ((event->type != GDK_BUTTON_RELEASE) || (!GTK_BUTTON(widget)->in_button))
        RET();
    if (event->button == 1) {
        if (tk->iconified)    {
            //tk->iconified = 0;
            XMapRaised (GDK_DISPLAY(),  tk->win);
            DBG("xmap  %x\n", tk->win);
        } else {
            if (tk->focused) {
                //tk->iconified = 1;
                XIconifyWindow (GDK_DISPLAY(), tk->win, DefaultScreen(GDK_DISPLAY()));
                DBG("iconify %x\n", tk->win);
            } else {
                XRaiseWindow (GDK_DISPLAY(), tk->win);
                XSetInputFocus (GDK_DISPLAY(), tk->win, RevertToNone, CurrentTime);
                DBG("xraise %x\n", tk->win);
            }
        }
    } else if (event->button == 2) {
        Xclimsg(tk->win, a_NET_WM_STATE,
              2 /*a_NET_WM_STATE_TOGGLE*/,
              a_NET_WM_STATE_SHADED,
              0, 0, 0);    
    } else if (event->button == 3) {
        XLowerWindow (GDK_DISPLAY(), tk->win);
    }
    RET();
}

static void
display_tk(taskbar *tb, task *tk)
{    
    ENTER;
    
    g_assert ((tb != NULL) && (tk != NULL));
    if (TASK_VISIBLE(tb, tk)) {
	GtkStateType state;

        state = (tk->focused) ?  GTK_STATE_ACTIVE : GTK_STATE_NORMAL;
	gtk_widget_set_state (GTK_WIDGET (tk->button), state);
	gtk_widget_show(tk->button);
        if (tb->tooltips)
            gtk_tooltips_set_tip(tb->tips, tk->button, tk->name, NULL);
	RET();
    }    
    gtk_widget_hide(tk->button);
    RET();
}

static void
display_tb(taskbar *tb)
{
    int sw, i;
    task *tk;
    ENTER;

    if (!tb->wins)
        RET();

    for (i = 0, tb->vis_task_num = 0; i < tb->win_num; i++) {
        if ((tk = g_hash_table_lookup(tb->task_list, &tb->wins[i]))) {
            display_tk(tk->tb, tk);
            if (TASK_VISIBLE(tk->tb, tk))
                tb->vis_task_num++;
        }
    }
    if (!(tb->vis_task_num && tb->hbox_width))
        sw = tb->task_width_max;
    else {
        sw = tb->hbox_width / tb->vis_task_num;
        if (sw > tb->task_width_max)
            sw = tb->task_width_max;
    }
    if (tb->task_width != sw) {
        tb->task_width = sw;
    }
    gtk_widget_queue_draw(tb->hbox);
    RET();          
}


static void
tk_size_alloc(GtkWidget *widget, GtkAllocation *data, task *tk )
{
    ENTER;
    DBG("tk_size_alloc: alloc=(%d, %d)\n", data->width, data->height);
    RET();
}

static void
tk_size_req(GtkWidget *widget, GtkRequisition *req, task *tk )
{
    ENTER;
    DBG("tk_size_req: IN  req=(%d, %d)\n", req->width, req->height);
    if (req->width > tk->tb->task_width_max)
        req->width = tk->tb->task_width_max;

    if (tk->tb->plug->panel->orientation == ORIENT_HORIZ) {
        if (req->width > tk->tb->task_width_max)
            req->width = tk->tb->task_width_max;
    } else {
        if (req->height > tk->tb->task_width_max)
            req->height = tk->tb->task_width_max;
    }
    DBG("tk_size_req: OUT req=(%d, %d)\n", req->width, req->height);
    RET();
}


static void
tk_build_gui(taskbar *tb, task *tk)
{
    GtkWidget *w1;
    
    ENTER;
    g_assert ((tb != NULL) && (tk != NULL));

    // TODO
    XSelectInput (GDK_DISPLAY(), tk->win, PropertyChangeMask);
    //XSelectInput (GDK_DISPLAY(), tk->win, PropertyChangeMask | StructureNotifyMask);

    /* button */
    tk->button = gtk_button_new();
    gtk_widget_add_events (tk->button, GDK_BUTTON_RELEASE_MASK );

    //gtk_widget_set_usize(tk->button, tb->task_width_max, 20);
    DBG("tk usize %d %d\n", tb->task_width_max, 20);
    gtk_container_set_border_width(GTK_CONTAINER(tk->button), 0);
    /*g_signal_connect (G_OBJECT (tk->button), "clicked",
      G_CALLBACK (callback_clicked), (gpointer) tk);
    g_signal_connect(G_OBJECT(tk->button), "button_press_event",
    G_CALLBACK(button_press_event), (gpointer)tk);
    */
    g_signal_connect(G_OBJECT(tk->button), "button_release_event",
          G_CALLBACK(button_press_event), (gpointer)tk);
    g_signal_connect (G_OBJECT (tk->button), "leave",
          G_CALLBACK (callback_leave), (gpointer) tk);
    g_signal_connect (G_OBJECT (tk->button), "size-allocate",
          G_CALLBACK (tk_size_alloc), (gpointer) tk);
    g_signal_connect (G_OBJECT (tk->button), "size-request",
          (GCallback) tk_size_req, tk);
    
    //gtk_button_set_relief(GTK_BUTTON(tk->button), GTK_RELIEF_NONE);

    /* pix and name */
    w1 = tb->plug->panel->my_box_new(FALSE, 1);
    gtk_container_set_border_width(GTK_CONTAINER(w1), 0);

    /* pix */

    get_wmclass(tk);
    get_task_icon(tb, tk);
    tk->image = gtk_image_new_from_pixbuf(tk->pixbuf );
    gtk_widget_show(tk->image);
    gtk_box_pack_start(GTK_BOX(w1), tk->image, FALSE, FALSE, 1);

    /* name */
    tk->label = gtk_label_new(tk->iconified ? tk->iname : tk->name);
    gtk_label_set_justify(GTK_LABEL(tk->label), GTK_JUSTIFY_LEFT);
    gtk_widget_show(tk->label);
    gtk_box_pack_start(GTK_BOX(w1), tk->label, FALSE, TRUE, 0);
    
    gtk_widget_show(w1);

    gtk_container_add (GTK_CONTAINER (tk->button), w1);
  

    //print_sizes(0);
    //TODO
    //gtk_widget_set_usize(tk->button, tb->task_width, tb->iconsize);
    gtk_box_pack_start(GTK_BOX(tb->bbox), tk->button, FALSE, TRUE, 0);
    GTK_WIDGET_UNSET_FLAGS (tk->button, GTK_CAN_FOCUS);    

    gtk_widget_show(tk->button);
    //DBG("show %x %s\n", tk->win, tk->name);
    if (!TASK_VISIBLE(tb, tk)) {
        gtk_widget_hide(tk->button);
        //DBG("hide %x %s\n", tk->win, tk->name);
    }

    RET();
}

/* tell to remove element with zero refcount */
static gboolean
remove_stale_tasks(Window *win, task *tk, gpointer data)
{
    if (tk->refcount-- == 0) {
        //DBG("do_net_list <del>: 0x%x %s\n", tk->win, tk->name);
        del_task(tk->tb, tk, 0);
        RET(TRUE);
    }
    RET(FALSE);
}

/*****************************************************
 * handlers for NET actions                          *
 *****************************************************/

GdkFilterReturn
tb_event_filter( XEvent *xev, GdkEvent *event, taskbar *tb)
{
    
    ENTER;
    //RET(GDK_FILTER_CONTINUE);
    g_assert(tb != NULL);
    if (xev->type == PropertyNotify )
	tb_propertynotify(tb, xev);
    RET(GDK_FILTER_CONTINUE);
}


static void
do_net_client_list(taskbar *tb)
{
    int i;
    task *tk;
    
    ENTER;
    if (tb->wins)
        XFree(tb->wins);
    tb->wins = get_xaproperty (GDK_ROOT_WINDOW(), a_NET_CLIENT_LIST, XA_WINDOW, &tb->win_num);
    if (!tb->wins) 
	RET();
    //DBG("do_net_list -----enter------\n");
    for (i = 0; i < tb->win_num; i++) {
        if ((tk = g_hash_table_lookup(tb->task_list, &tb->wins[i]))) {
            tk->refcount++;
            //DBG("do_net_list <...>: 0x%x %s\n", tk->win, tk->name);
        } else {
            net_wm_window_type nwwt;
            net_wm_state nws;

            get_net_wm_state(tb->wins[i], &nws);
            if (!accept_net_wm_state(&nws, tb->accept_skip_pager))
                continue;
            get_net_wm_window_type(tb->wins[i], &nwwt);
            if (!accept_net_wm_window_type(&nwwt))
                continue;
            
            tk = g_new0(task, 1);
            tk->refcount++;
            tb->num_tasks++;
            tk->win = tb->wins[i];
            tk->tb = tb;
            tk->iconified = (get_wm_state (tk->win) == IconicState);
            tk->desktop = get_net_wm_desktop(tk->win);
            tk->nws = nws;
            tk->nwwt = nwwt;
            tk_set_names(tk);
            //DBG("do_net_list <add>: #desk=%d 0x%x %s\n", tk->desktop, tk->win, tk->name);
            // TODO
            //get_wmclass(tk);
            //get_task_hinticon(tb, tk);

            tk_build_gui(tb, tk);
            g_hash_table_insert(tb->task_list, &tk->win, tk);
        }
    }
    
    /* remove windows that arn't in the NET_CLIENT_LIST anymore */
    g_hash_table_foreach_remove(tb->task_list, (GHRFunc) remove_stale_tasks, NULL);
    //DBG("do_net_list -----exit------\n");
    RET();
}

static void
do_net_active_window(taskbar *tb)
{
    Window *f;
    task *tk = NULL, *otk = NULL;

    
    ENTER;
    g_assert (tb != NULL);
    otk = tb->focused;
    f = get_xaproperty(GDK_ROOT_WINDOW(), a_NET_ACTIVE_WINDOW, XA_WINDOW, 0);
    if (f) {
        if ((tk = find_task(tb, *f)) != NULL) {
            tb->focused = tk;
            tk->focused = 1;
            display_tk(tb, tk);
        }
        XFree(f);
    } else {
        tb->focused = NULL;
    }

    if (otk && (!tk || (otk->win != tk->win))) {
        otk->focused = 0;
        display_tk(tb, otk);
    }
    RET();
}

static void
tb_propertynotify(taskbar *tb, XEvent *ev)
{
    Atom at;
    Window win;

    
    ENTER;
    DBG("tb_propertynotify: tb=%p win=%x\n", tb, ev->xproperty.window);
    at = ev->xproperty.atom;
    win = ev->xproperty.window;
    if (win == GDK_ROOT_WINDOW()) {
	if (at == a_NET_CLIENT_LIST) {
	    do_net_client_list(tb);
	    display_tb(tb);
	} else if (at == a_NET_CURRENT_DESKTOP) {
	    tb->cur_desk = get_net_current_desktop();
	    display_tb(tb);
	} else if (at == a_NET_NUMBER_OF_DESKTOPS) {
	    tb->desk_num = get_net_number_of_desktops();
	    display_tb(tb);
	} else if (at == a_NET_ACTIVE_WINDOW) {
	    do_net_active_window(tb);
	}
    } else {
	task *tk = find_task(tb, win);
        
        //DBG("tb_propertynotify: win = <%d>\n", win);
	if (!tk) RET();
	if (at == a_NET_WM_DESKTOP) {
	    tk->desktop = get_net_wm_desktop(win);
	    display_tb(tb);	
	}  else if (at == XA_WM_NAME) {
	    tk_free_names(tk);
	    tk_set_names(tk);
	    gtk_label_set_text(GTK_LABEL(tk->label), tk->iconified ? tk->iname : tk->name);
	    display_tk(tb, tk);
	}  else if (at == XA_WM_CLASS) {
	    get_wmclass(tk);

	} else if (at == a_WM_STATE)    {
	    /* iconified state changed? */
	    tk->iconified = (get_wm_state (tk->win) == IconicState);
            gtk_label_set_text (GTK_LABEL(tk->label),
                  tk->iconified ? tk->iname : tk->name);
	    /*DBG( "tb_propertynotify: WM_STATE: win=0x%x oo=%d mo=%d title=%s\n",
              tk->win, get_wm_state (tk->win), tk->iconified, tk->name);*/
	    display_tk(tb, tk);
	} else if (at == XA_WM_HINTS)	{
	    /* some windows set their WM_HINTS icon after mapping */
	    get_wmclass(tk);
            g_object_unref(tk->pixbuf);
	    get_task_icon (tb, tk);
	    gtk_image_set_from_pixbuf (GTK_IMAGE(tk->image), tk->pixbuf);
	    display_tk(tb, tk);
	} else if (at == a_NET_WM_STATE) {
            net_wm_state nws;
	    DBG( "tb_propertynotify::_NET_WM_STATE\n");
	    get_net_wm_state(tk->win, &nws);
            if (!accept_net_wm_state(&nws, tb->accept_skip_pager)) {
		del_task(tb, tk, 1);
		display_tb(tb);
	    }
	} else if (at == a_NET_WM_WINDOW_TYPE) {
            net_wm_window_type nwwt;
	    DBG( "tb_propertynotify::_NET_WM_WINDOW_TYPE\n");
	    get_net_wm_window_type(tk->win, &nwwt);
            if (!accept_net_wm_window_type(&nwwt)) {
		del_task(tb, tk, 1);
		display_tb(tb);
	    }
	}
	
    }
    RET();
}



static void
hbox_size_alloc(GtkWidget *widget, GtkAllocation *allocation, taskbar *tb )
{
    ENTER;
    tb->hbox_width = allocation->width;
    DBG("hbox_size_alloc: alloc=(%d, %d)\n", allocation->width, allocation->height);
    RET();
}


static void
taskbar_build_gui(plugin *p)
{
    taskbar *tb = (taskbar *)p->priv;

    ENTER;
    tb->hbox = tb->plug->panel->my_box_new(FALSE, 0);
    gtk_widget_show(tb->hbox);
    gtk_container_set_border_width(GTK_CONTAINER(tb->hbox), 0);

    if (tb->plug->panel->orientation == ORIENT_HORIZ)
        tb->bbox = gtk_htbar_new(tb->task_width_max, 2);
    else 
        tb->bbox = gtk_vtbar_new(tb->iconsize+1, 2);
    gtk_widget_show(tb->bbox);
    gtk_box_pack_start(GTK_BOX(tb->hbox), tb->bbox, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (tb->hbox), "size_allocate",
          G_CALLBACK (hbox_size_alloc), (gpointer) tb);
    tb->gen_pixbuf =  gdk_pixbuf_new_from_xpm_data((char **)icon_xpm);

    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), PropertyChangeMask);
    gdk_window_add_filter(gdk_get_default_root_window(), (GdkFilterFunc)tb_event_filter, tb);
    gdk_window_add_filter(NULL, (GdkFilterFunc)tb_event_filter, tb );
    tb->desk_num = get_net_number_of_desktops();
    tb->cur_desk = get_net_current_desktop();
    tb->focused = NULL;
    if (tb->tooltips)
        tb->tips = gtk_tooltips_new();

    gtk_container_add (GTK_CONTAINER (p->pwid), tb->hbox);
    gtk_container_set_border_width(GTK_CONTAINER(p->pwid), 0);
    //gtk_box_pack_start(GTK_BOX(p->pwid), eb, TRUE, TRUE, 0);
    gtk_widget_show_all(tb->hbox);
    RET();
}

static int
read_application(plugin *p)
{
    taskbar *tb = (taskbar *)p->priv;
    GdkPixbuf *gp = NULL, *gps;
    line s;
    gchar *fname, *appname, *classname;
    wmpix_t *wp = NULL;
    GError *err = NULL;
    int rv = 0;
    
    ENTER;
    s.len = 256;
    fname = appname = classname = NULL;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "taskbar: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "image")) 
                fname = expand_tilda(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "appname"))
                appname = g_strdup(s.t[1]);
            else if (!g_ascii_strcasecmp(s.t[0], "classname"))
                classname = g_strdup(s.t[1]);
            else {
                ERR( "taskbar: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "taskbar: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    rv = 1;
    gp = gdk_pixbuf_new_from_file(fname, &err);
    if (!gp) {
        ERR( "taskbar: can't read pixmap %s\n", fname);
        goto error;
    }
    gps = gdk_pixbuf_scale_simple (gp, tb->iconsize, tb->iconsize, GDK_INTERP_HYPER);
    g_object_unref(gp);
    if (!gps) {
        ERR( "taskbar: can't scale pixmap %s\n", fname);
        goto error;
    }
    g_free(fname);
    wp = g_new0 (wmpix_t, 1);
    wp->next = tb->wmpix;
    wp->pixbuf = gps;
    wp->ch.res_name = appname;
    wp->ch.res_class = classname;
    tb->wmpix = wp;
    tb->wmpixno++;

    RET(1);

 error:
    g_free(fname);
    g_free(appname);
    g_free(classname);
    RET(rv);
}

static int
taskbar_constructor(plugin *p)
{
    taskbar *tb;
    line s;
    
    ENTER;
    tb = g_new0(taskbar, 1);
    tb->plug = p;
    p->priv = tb;
    tb->iconsize =  (p->panel->orientation == ORIENT_HORIZ) ?
        (p->panel->ah - 4) : (p->panel->aw - 4);
    tb->tooltips = 1;
    tb->icons_only = 0;
    tb->accept_skip_pager = 1;
    tb->wmpixno = 0;
    tb->task_width_max = TASK_WIDTH_MAX;
    tb->task_list = g_hash_table_new(g_int_hash, g_int_equal);
    s.len = 256;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        if (s.type == LINE_NONE) {
            ERR( "taskbar: illegal token %s\n", s.str);
            goto error;
        }
        if (s.type == LINE_VAR) {
            if (!g_ascii_strcasecmp(s.t[0], "tooltips")) {
                tb->tooltips = str2num(bool_pair, s.t[1], 1);
            } else if (!g_ascii_strcasecmp(s.t[0], "IconsOnly")) {
                tb->icons_only = str2num(bool_pair, s.t[1], 0);
            }  else if (!g_ascii_strcasecmp(s.t[0], "AcceptSkipPager")) {
                tb->accept_skip_pager = str2num(bool_pair, s.t[1], 1);
            } else if (!g_ascii_strcasecmp(s.t[0], "MaxTaskWidth")) {
                tb->task_width_max = atoi(s.t[1]);
                DBG("task_width_max = %d\n", tb->task_width_max);
            } else {
                ERR( "taskbar: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else if (s.type == LINE_BLOCK_START) {
            if (!g_ascii_strcasecmp(s.t[0], "application")) {
                if (!read_application(p)) {
                    goto error;
                }
            } else {
                ERR( "taskbar: unknown var %s\n", s.t[0]);
                goto error;
            }
        } else {
            ERR( "taskbar: illegal in this context %s\n", s.str);
            goto error;
        }
    }
    if (tb->task_width_max > TASK_WIDTH_MAX)
        tb->task_width_max = TASK_WIDTH_MAX;
    taskbar_build_gui(p);
    do_net_client_list(tb);
    display_tb(tb);
    do_net_active_window(tb);   
    RET(1);
    
 error:
    taskbar_destructor(p);
    RET(0);
}


static void
taskbar_destructor(plugin *p)
{
    taskbar *tb = (taskbar *)p->priv;
    wmpix_t *wp;
    
    ENTER;
    gdk_window_remove_filter(gdk_get_default_root_window(), (GdkFilterFunc)tb_event_filter, tb);
    gdk_window_remove_filter(NULL, (GdkFilterFunc)tb_event_filter, tb );
    gtk_widget_destroy(tb->hbox);
    while (tb->wmpix) {
        wp = tb->wmpix;
        tb->wmpix = tb->wmpix->next;
        g_free(wp->ch.res_name);
        g_free(wp->ch.res_class);
        g_object_unref(wp->pixbuf);
        g_free(wp);
    }
    RET();
}

plugin_class taskbar_plugin_class = {
    fname: NULL,
    count: 0,

    type : "taskbar",
    name : "taskbar",
    version: "1.0",
    description : "Taskbar shows all opened windows and allow to iconify them, shade or get focus",
    
    constructor : taskbar_constructor,
    destructor  : taskbar_destructor,
};
