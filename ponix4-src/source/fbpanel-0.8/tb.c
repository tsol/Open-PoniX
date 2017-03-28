#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>

#include <gdk-pixbuf/gdk-pixbuf.h>

#include "fbpanel.h"
#include "icon.xpm"

#define CFGFILE ".fbpanel/taskbar"
#define ICONSIZE   20


typedef struct wmpix_t {
    struct wmpix_t *next;
    GdkPixmap *pix;
    GdkBitmap *mask;
    char *plist[2];
} wmpix_t;

static wmpix_t *wmpix = NULL; 
static int wmpixno = 0;

typedef struct task
{
    struct task *next;
    Window win;
    Pixmap icon;
    //Pixmap mask;
    char *name, *iname;
    GtkWidget *button, *pixmap, *label, *sep;
    GdkPixmap *pix;
    GdkBitmap *mask;
    int freepix;
    char **plist;
    int pos_x;
    int width;
    int desktop;
    unsigned int sticky:1;
    unsigned int hidden:1;
    unsigned int focused:1;
    unsigned int iconified:1;
    unsigned int icon_copied:1;
} task;

typedef struct taskbar
{
    Window win;
    task *task_list;
    GtkWidget *hbox, *exp;
    int num_tasks;
    int task_width;
    int hbox_width;
    int my_desktop;
    task *focused;
    char **desk_names;
    int desk_namesno;
    int desk_count;
    unsigned int hidden:1;
    unsigned int at_top:1;
} taskbar;


GtkStyle *style;
GdkPixmap *gen_pixmap;
GdkBitmap *gen_mask;
taskbar tb;

void display_tk(taskbar *tb, task *tk);
void tb_propertynotify(XEvent *ev);
GdkFilterReturn tb_event_filter( GdkXEvent *, GdkEvent *, void *);

static void read_wmpix(FILE *fp);



static void
read_file(FILE *fp)
{
    gchar str[512];
    
    while (myfgets(str, 512, fp)) {
	if (strcmp(str, "[")) {
	    g_warning("expected '[' but got %s\n", str);
	    return;
	}
	read_wmpix(fp);
    }
}


static void
read_wmpix(FILE *fp)
{
    GdkPixbuf *gp, *gps;
    GdkPixmap *pix;
    GdkBitmap *mask;
    gchar str[512];
    gchar fname[512];
    gchar appname[512];
    gchar classname[512];
    wmpix_t *wp;

    appname[0] = classname[0] = fname[0] = 0;
    while (myfgets(str, 512, fp) && (str[0] != ']')) {
	if (!strncmp(str, "Image::", strlen("Image::"))) 
	    strcpy(fname, str + strlen("Image::"));
	else if (!strncmp(str, "AppName::", strlen("Appname::"))) 
	    strcpy(appname, str + strlen("Appname::"));
	else if (!strncmp(str, "ClassName::", strlen("Classname::"))) 
	    strcpy(classname, str + strlen("Classname::"));
	else
	    fprintf(stderr, "Illegal token %s\n", str);
	
    }
    gp = gdk_pixbuf_new_from_file(fname);
    if (!gp) return;
    
    gps =  gdk_pixbuf_scale_simple (gp, ICONSIZE, ICONSIZE,
	GDK_INTERP_TILES);
    gdk_pixbuf_render_pixmap_and_mask(gps, &pix, &mask, 127);


    wp = g_new0 (wmpix_t, 1);
    g_return_if_fail (wp != NULL);
    wp->next = wmpix;
    wp->plist[0] = strdup(appname);
    wp->plist[1] = strdup(classname);
    wp->pix = pix;
    wp->mask = mask;
    wmpix = wp;
    wmpixno++;
    /*
    printf("Added plist[0]='%s'  plist[1]='%s'\n",
	wp->plist[0] ? wp->plist[0] : "(null)",
	wp->plist[1] ? wp->plist[1] : "(null)");
    */
}


GdkFilterReturn tb_event_filter( GdkXEvent *xevent,
    GdkEvent *event,
    void *sia)
{
    if (((XEvent *) xevent)->type == PropertyNotify )
	tb_propertynotify( (XEvent *)xevent);
    return GDK_FILTER_CONTINUE;
}



void print_sizes(int i)
{
    GtkRequisition req1, req2;
    gtk_widget_size_request (tb.hbox, &req1);
    gtk_widget_size_request (tb.exp, &req2);
    printf("%d req:: hbox[%dx%x] :: exp[%dx%d]\n", i, req1.width, req1.height, req2.width, req2.height);
    printf("%d aloc:: exp[%dx%d]\n", i, tb.exp->allocation.width, tb.exp->allocation.height);

}
/*
int
is_hidden(Window win)
{
    Atom *state;
    int num, ret = 0;
    
    state = get_xaproperty(win, atom__NET_WM_STATE, XA_ATOM, &num);
    if (!state)
        return 0;
    
    while (--num >= 0)
	if (state[num] == atom__NET_WM_STATE_SKIP_PAGER ||
	    state[num] == atom__NET_WM_STATE_SKIP_TASKBAR) {
	    ret = 1;
	    break;
	}
    XFree(state);
    return ret;
}
*/

int
is_iconified (Window win)
{
    unsigned long *data;
    int ret;

    data = get_xaproperty (win, atom_WM_STATE, atom_WM_STATE, 0);
    if (!data)
        return 0;
    
    ret =  (data[0] == IconicState);
    XFree (data);
    return ret;
}
void get_wmclass(task *tk)
{
    int nn, i;
    
    tk->plist = get_textproperty(tk->win,  XA_WM_CLASS, &nn);
    if (!tk->plist)
        return;
}

int get_net_wm_desktop(Window win)
{
    int desk;
    unsigned long *data;

    data = get_xaproperty (win, atom__NET_WM_DESKTOP, XA_CARDINAL, 0);
    if (!data)
        return 0;
    
    desk = *data;
    XFree (data);
    return desk;
}

int
get_net_number_of_desktops()
{
    int desknum;
    unsigned long *data;

    data = get_xaproperty (GDK_ROOT_WINDOW(), atom__NET_NUMBER_OF_DESKTOPS,
	XA_CARDINAL, 0);
    if (!data)
        return 0;

    desknum = *data;
    XFree (data);
    return desknum;
}

    
int
get_net_current_desktop ()
{
    int desk;
    unsigned long *data;

    data = get_xaproperty (GDK_ROOT_WINDOW(), atom__NET_CURRENT_DESKTOP, XA_CARDINAL, 0);
    if (!data)
        return 0;

    desk = *data;
    XFree (data);
    return desk;
}


task *
find_task (taskbar * tb, Window win)
{
    task *list = tb->task_list;
    while (list)
    {
	if (list->win == win)
	    return list;
	list = list->next;
    }
    return NULL;
}

void adjust_task_width(taskbar * tb)
{
    int dno = 0;
    task *tk;

    for (tk = tb->task_list;tk; tk = tk->next) {
	if ( (!tk->hidden) && (tk->desktop == tb->my_desktop || tk->sticky))
	    dno++;
    }
    if (!dno)
	tb->task_width = MAX_TASK_WIDTH;
    else {
	tb->task_width = (tb->hbox_width) / dno - 2*TASK_PADDING;
	if (tb->task_width > MAX_TASK_WIDTH)
	    tb->task_width = MAX_TASK_WIDTH;
    }
    //fprintf(stderr, "dno=%d, task_width=%d hbox_width=%d\n", dno, tb->task_width, tb->hbox_width);
}

void
tk_set_names(task *tk)
{
    int nn;
    char **plist;
    
    plist = get_textproperty(tk->win,  XA_WM_NAME, &nn);
    tk->name = tk->iname = NULL;
    if (plist) {		
	tk->name = g_strdup(plist[0]);
	tk->iname = g_strdup_printf("[%s]", tk->name);
	XFreeStringList(plist);
    }
}

void
tk_free_names(task *tk)
{

    if (tk->name) {
	g_free(tk->name);
	tk->name = NULL;
    }
    if (tk->iname) {
	g_free(tk->iname);
	tk->iname = NULL;
    }
}

void
del_task (taskbar * tb, Window win)
{
    task *next, *prev = 0, *list = tb->task_list;

    //
    while (list)
    {
	next = list->next;
	if (list->win == win)
	{
	    gtk_widget_destroy(list->button);
	    //fprintf(stderr, "del_task\n");
	    gtk_widget_destroy(list->sep);
	    //fprintf(stderr, "del_task 2\n");
	    /* unlink and free this task */
	    tb->num_tasks--;
	    if (list->icon_copied)
	    {
		/*
		XFreePixmap (dd, list->icon);
		if (list->mask != None)
		    XFreePixmap (dd, list->mask);
		*/
	    }
	    tk_free_names(list);	    
	    if (tb->focused == list)
		tb->focused = NULL;
	    g_free (list);
	    if (prev == 0)
		tb->task_list = next;
	    else
		prev->next = next;
	    //adjust_task_width(tb);
	    return;
	}
	prev = list;
	list = next;
    }
}

void
get_net_wm_state(task *tk)
{
    Atom *state;
    int num3;
    
    tk->hidden = tk->sticky = 0;
    if (!(state = get_xaproperty(tk->win, atom__NET_WM_STATE, XA_ATOM, &num3)))
        return;

    while (--num3 >= 0) {
	//fprintf(stderr, "get_net_wm_state:: %d :: ", state[num3]);
	if (state[num3] == atom__NET_WM_STATE_SKIP_PAGER ||
	    state[num3] == atom__NET_WM_STATE_SKIP_TASKBAR) {

	    //fprintf(stderr, "skip something\n");
	    tk->hidden = 1;
	    return;
	} else if (state[num3] == atom__NET_WM_STATE_STICKY) {
	    //fprintf(stderr, "%s\n",  atom_names[14]);
	    tk->sticky = 1;
	} else {
	    //fprintf(stderr, "xxunkownstate\n");
	}
    }
    XFree(state);
}


void
get_task_hinticon (task *tk)
{
    XWMHints *hin;
    Pixmap icon_pix = None, icon_mask = None;
    Window win;
    int x, y, w, h, d, bw;
    char **data;
    XpmAttributes xa;
    GdkPixbuf *gp, *gps;
    
    g_return_if_fail(tk != NULL);
    //fprintf(stderr, "get_task_hinticon 0\n");
    if (tk->freepix) {
	gdk_pixmap_unref(tk->pix);
	gdk_pixmap_unref(tk->mask);
    }
    tk->freepix = 0;
    tk->pix = tk->mask = NULL;
    hin = (XWMHints *) get_xaproperty (tk->win, XA_WM_HINTS, XA_WM_HINTS, 0);
    if (hin) {
	//fprintf(stderr, "get_task_hinticon\n");
	if ((hin->flags & IconPixmapHint)) {
	    if ((hin->flags & IconMaskHint)) {
		icon_mask = hin->icon_mask;
	    } else {
		icon_mask = hin->icon_pixmap;
	    }
	    icon_pix = hin->icon_pixmap;
	   
	    XGetGeometry (GDK_DISPLAY(), icon_pix, &win, &x, &y, &w, &h, &bw, &d);
	    xa.visual = GDK_VISUAL_XVISUAL(gdk_window_get_visual(window->window));
	    xa.colormap = GDK_COLORMAP_XCOLORMAP(gdk_window_get_colormap(window->window));
	    xa.depth = gdk_window_get_visual(window->window)->depth;
	    xa.valuemask = XpmDepth | XpmColormap | XpmVisual;
    
	    if (XpmCreateDataFromPixmap(GDK_DISPLAY(), &data, icon_pix, icon_mask, &xa) == XpmSuccess) {
		gp = gdk_pixbuf_new_from_xpm_data((const char **)data);
		if (gp) {
		    gps =  gdk_pixbuf_scale_simple (gp, ICONSIZE, ICONSIZE, GDK_INTERP_TILES);
		    gdk_pixbuf_render_pixmap_and_mask(gps, &tk->pix, &tk->mask, 127);
		    gdk_pixbuf_unref(gp);
		    gdk_pixbuf_unref(gps);
		    tk->freepix = 1;
		}
	    }
	}
	XFree (hin);
    }
    if (!tk->pix && tk->plist) {
	wmpix_t *tmp;
	/*printf("WIN name=%s\n...       appname=%s  classname=%s\n",
	  tk->name, tk->plist[0], tk->plist[1]);
	*/
	for (tmp = wmpix; tmp; tmp = tmp->next) {
	    /*
	      printf("...trying appname=%s  classname=%s\n",
	      tmp->plist[0], tmp->plist[1]);
	    */
	    if ((!tmp->plist[1][0]  || !strcmp(tmp->plist[1], tk->plist[1]))
		&& (!tmp->plist[0][0]  || !strcmp(tmp->plist[0], tk->plist[0]))) {

		//printf("win %s got new pixmap\n", tk->name);
		tk->pix = tmp->pix;
		tk->mask = tmp->mask;
		break;
	    }
	}
    }
     
    if (!tk->pix) {
	tk->pix = gen_pixmap;
	tk->mask = gen_mask;
    }
}

void callback_clicked( GtkWidget *widget, gpointer   data )
{
    task *tk = (task *) data;

    //fprintf(stderr, "cb: %x: ico=%d focus=%d\n", tk->win, tk->iconified, tk->focused);
    if (tk->iconified)    {
	tk->iconified = 0;
	XMapWindow (GDK_DISPLAY(),  tk->win);
    } else if (tk->focused) {
      //tk->iconified = 1;
	XIconifyWindow (GDK_DISPLAY(), tk->win, gdk_screen);
	//fprintf(stderr, "iconify %x\n", tk->win);
    } else {
	XRaiseWindow (GDK_DISPLAY(), tk->win);
	XSetInputFocus (GDK_DISPLAY(), tk->win, RevertToNone, CurrentTime);
    }
}

void callback_leave( GtkWidget *widget, gpointer   data )
{
    task *tk = (task *) data;

    //fprintf(stderr, "leave\n");
    display_tk(&tb, tk);
    return;
    
}


task *
alloc_task (Window win)
{
    task *tk;
    
    tk = g_new0 (task, 1);
    g_return_val_if_fail (tk != NULL, NULL);
    tk->win = win;
    return tk;
}

task *
add_task(taskbar * tb, task *tk)
{
    task *list;    
    GtkWidget *w1;

    g_assert ((tb != NULL) && (tk != NULL));

    tk_set_names(tk);
    tk->desktop = get_net_wm_desktop(tk->win);
    tk->iconified = is_iconified (tk->win);
    XSelectInput (GDK_DISPLAY(), tk->win, PropertyChangeMask);

    /* now append it to our linked list */
    tb->num_tasks++;
    //adjust_task_width(tb);
    
    /* button */
    tk->button = gtk_button_new();
    gtk_container_set_border_width(GTK_CONTAINER(tk->button), 0);
    gtk_signal_connect (GTK_OBJECT (tk->button), "clicked",
	GTK_SIGNAL_FUNC (callback_clicked), (gpointer) tk);
    gtk_signal_connect (GTK_OBJECT (tk->button), "leave",
	GTK_SIGNAL_FUNC (callback_leave), (gpointer) tk);
    
    gtk_button_set_relief(GTK_BUTTON(tk->button), GTK_RELIEF_NONE);
    w1 = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(w1), 0);
 
    /*pixmap*/
    get_wmclass(tk);
    get_task_hinticon(tk);

    tk->pixmap = gtk_pixmap_new(tk->pix, tk->mask );
  
    /*label*/
    tk->label = gtk_label_new(tk->iconified ? tk->iname : tk->name);
    gtk_label_set_justify(GTK_LABEL(tk->label), GTK_JUSTIFY_LEFT);
    
    gtk_box_pack_start(GTK_BOX(w1), tk->pixmap, FALSE, FALSE, 1);
    gtk_box_pack_start(GTK_BOX(w1), tk->label, FALSE, FALSE, 10);
    gtk_widget_show(tk->pixmap);
    gtk_widget_show(tk->label);
    gtk_widget_show(w1);

    
    gtk_container_add (GTK_CONTAINER (tk->button), w1);
    //print_sizes(0);

    gtk_widget_set_usize(tk->button, tb->task_width, ICONSIZE);
    gtk_box_pack_start(GTK_BOX(tb->hbox), tk->button, FALSE, FALSE, 0);
    
    tk->sep = gtk_vseparator_new();
    gtk_widget_show(tk->sep);
    gtk_box_pack_start(GTK_BOX(tb->hbox), tk->sep,FALSE, FALSE, 0);
    //print_sizes(1);
    
    if ( (!tk->hidden) && (tk->desktop == tb->my_desktop || tk->sticky))
	gtk_widget_show(tk->button);
    
    list = tb->task_list;
    if (!list)   {
	tb->task_list = tk;
	return tk;
    }
    while (1) {
	if (!list->next) {
	    list->next = tk;
	    return tk;
	}
	list = list->next;
    }
}


void
do_net_client_list()
{
    Window *win;
    int num, j=0;
    task *list, *next;
	
    /* try unified window spec first */
    win = get_xaproperty (GDK_ROOT_WINDOW(), atom__NET_CLIENT_LIST, XA_WINDOW, &num);
    if (!win)
        return;
    
    if (!num)
	return;

    /* remove windows that arn't in the _WIN_CLIENT_LIST anymore */
    list = tb.task_list;
    while (list)   {
	next = list->next;
	for (j = num - 1; j >= 0; j--)
	    if (list->win == win[j])
		goto dontdel;
	del_task (&tb, list->win);
    dontdel:
	list = next;
    }

    /* add any new windows */
    for (j = 0; j < num; j++)   {
	if (!find_task (&tb, win[j])) {
	    task *tk, tkk;
	    tkk.win = win[j];
	    get_net_wm_state(&tkk);

	    if (win[j] != topxwin && !tkk.hidden) {
		tk = alloc_task(win[j]);
		tk->sticky = tkk.sticky;
		add_task (&tb, tk);
	    }
	}
    }
    XFree(win);
}

void display_tk(taskbar *tb, task *tk)
{
    if (!tk->hidden && (tk->desktop == tb->my_desktop || tk->sticky)) {
	GtkStateType state;

	if (tk->focused)
	    state = GTK_STATE_ACTIVE;
	else
	    state = GTK_STATE_NORMAL;
	/* task should be visible */
	gtk_widget_set_state (GTK_WIDGET (tk->button), state);
	gtk_widget_set_usize(tk->button, tb->task_width, ICONSIZE);
	

	gtk_widget_show(tk->button);
	gtk_widget_show(tk->sep);
	//gtk_widget_queue_draw (GTK_WIDGET (tk->button));
	return;
    }
    
    gtk_widget_hide(tk->button);
    gtk_widget_hide(tk->sep);	
    return;
}

void
display_tb()
{
    task *tk;

    for (tk = tb.task_list; tk; tk = tk->next) 
	display_tk(&tb, tk);
    
    gtk_widget_queue_draw(tb.hbox);
}

void
do_net_active_window()
{
    Window *f;
    task *tk = NULL, *otk = NULL;
	    
    otk = tb.focused;
    f = get_xaproperty(GDK_ROOT_WINDOW(), atom__NET_ACTIVE_WINDOW, XA_WINDOW, 0);
    if (f) {
        if ((tk = find_task(&tb, *f)) != NULL) {
            tb.focused = tk;
            tk->focused = 1;
            display_tk(&tb, tk);
        }
        XFree(f);
    } else {
        tb.focused = NULL;
    }

    if (otk && (!tk || (otk->win != tk->win))) {
        otk->focused = 0;
        display_tk(&tb, otk);
        /*
          fprintf(stderr, "do_net_active_window: %x: ico=%d focus=%d\n",
          otk->win, otk->iconified, otk->focused);
        */
    }

}

void tb_propertynotify(XEvent *ev)
{
    Atom at = ev->xproperty.atom;
    Window win = ev->xproperty.window;
    
    if (win == GDK_ROOT_WINDOW()) {
	if (at == atom__NET_CLIENT_LIST) {
	    do_net_client_list();
	    adjust_task_width(&tb);
	    display_tb();
	} else if (at == atom__NET_CURRENT_DESKTOP) {
	    tb.my_desktop = get_net_current_desktop();
	    adjust_task_width(&tb);
	    display_tb();
	} else if (at == atom__NET_NUMBER_OF_DESKTOPS) {
	    tb.desk_count = get_net_number_of_desktops();
	    adjust_task_width(&tb);
	    display_tb();
	} else if (at == atom__NET_ACTIVE_WINDOW) {
	    do_net_active_window();
	}
    } else {
	task *tk = find_task(&tb, win);

	if (!tk) return;

	if (at == atom__NET_WM_DESKTOP) {
	    tk->desktop = get_net_wm_desktop(win);
	    adjust_task_width(&tb);
	    display_tb();	
	}  else if (at == XA_WM_NAME) {
	    tk_free_names(tk);
	    tk_set_names(tk);
	    gtk_label_set_text(GTK_LABEL(tk->label), tk->iconified ? tk->iname : tk->name);
	    display_tk(&tb, tk);
	}  else if (at == XA_WM_CLASS) {
	    get_wmclass(tk);

	} else if (at == atom_WM_STATE)    {
	    /* iconified state changed? */
	    //fprintf(stderr, "atom_WM_STATE tk->iconified=%d\n", tk->iconified);
	    tk->iconified = is_iconified (tk->win);
	    gtk_label_set_text (GTK_LABEL(tk->label), tk->iconified ? tk->iname : tk->name);
	    //fprintf(stderr, "icon=%d title=%s\n", tk->iconified, tk->name);
	    display_tk(&tb, tk);
	} else if (at == XA_WM_HINTS)	{
	    /* some windows set their WM_HINTS icon after mapping */
	    get_wmclass(tk);
	    get_task_hinticon (tk);
	    gtk_pixmap_set(GTK_PIXMAP(tk->pixmap), tk->pix, tk->mask);
	    display_tk(&tb, tk);
	} else if (at == atom__NET_WM_STATE) {	   
	    //fprintf(stderr, "tb_propertynotify::_NET_WM_STATE\n");
	    get_net_wm_state(tk);
	    if (tk->hidden) {
		del_task(&tb, tk->win);
		adjust_task_width(&tb);
		display_tb();
	    }
	}
	
    }
}



void hbox_size_cb( GtkWidget *widget, gpointer   data )
{
    tb.hbox_width = ((GtkAllocation *) data)->width;
    //fprintf(stderr, "hbox_width=%d\n", tb.hbox_width);
    return;
}

GtkWidget *
tb_init(gchar *cfgfile)
{
    FILE *fp;
    gchar *f = NULL;
    
    if (cfgfile)
	f = g_strdup(cfgfile);
    else
	f = g_strdup_printf("%s/%s", getenv("HOME"),  CFGFILE);

    if ((fp = fopen(f, "r")) != NULL)
	read_file(fp);
    g_free(f);
    
    tb.hbox = gtk_hbox_new(FALSE, 0);
    gtk_container_set_border_width(GTK_CONTAINER(tb.hbox), PANEL_BORDER);
    //gtk_signal_connect(GTK_OBJECT(tb.hbox), "expose_event",
    //	GTK_SIGNAL_FUNC(display_tb), NULL);
    gtk_signal_connect (GTK_OBJECT (tb.hbox), "size_allocate",
	GTK_SIGNAL_FUNC (hbox_size_cb), (gpointer) NULL);
    //sep = gtk_vseparator_new();
    //gtk_box_pack_start(GTK_BOX(tb.hbox), sep, FALSE, FALSE, 0);
    //sep = gtk_vseparator_new();
    //gtk_box_pack_end(GTK_BOX(tb.hbox), sep, FALSE, FALSE, 0);
   
    style = gtk_widget_get_style( window );
    gen_pixmap = gdk_pixmap_create_from_xpm_d (window->window,  &gen_mask,
	&style->bg[GTK_STATE_NORMAL], (gchar **)icon_xpm );
    
    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), PropertyChangeMask);
    XSelectInput (GDK_DISPLAY(), topxwin, PropertyChangeMask | FocusChangeMask |
	StructureNotifyMask);
    gdk_window_add_filter(NULL, (GdkFilterFunc)tb_event_filter, NULL );
    gdk_window_add_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)tb_event_filter, NULL);

    tb.desk_count = get_net_number_of_desktops();
    tb.my_desktop = get_net_current_desktop();
    tb.task_width = MAX_TASK_WIDTH;
    tb.focused = NULL;

    tb.exp = gtk_button_new_with_label("-----");
    gtk_button_set_relief(GTK_BUTTON(tb.exp), GTK_RELIEF_NONE);
    gtk_widget_set_sensitive (tb.exp, FALSE);
    gtk_widget_show(tb.exp);
    gtk_box_pack_end(GTK_BOX(tb.hbox), tb.exp, TRUE, TRUE, 0);
    do_net_client_list();
    adjust_task_width(&tb);
    display_tb();
    do_net_active_window();

    return tb.hbox;
}
