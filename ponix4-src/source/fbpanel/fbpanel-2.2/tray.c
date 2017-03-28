#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <X11/Xmu/WinUtil.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "panel.h"
#include "misc.h"
#include "plugin.h"

//#define DEBUG
#include "dbg.h"


typedef struct {
    GtkWidget *mainw;
    plugin *plug;
    Window *wins;
    int win_num;
} tray;

//static void run_gtktray(tray *tr);


static void
add_win( tray *tr, Window win)
{
    GtkWidget *socket;

    ENTER;
    socket = gtk_socket_new();
    gtk_widget_show (socket);
    gtk_box_pack_start(GTK_BOX(tr->mainw), socket, FALSE, FALSE, 0);
    XUnmapWindow(GDK_DISPLAY(), win);
    XSync(GDK_DISPLAY(), False);
    gtk_socket_steal(GTK_SOCKET(socket), win);
    XMapWindow(GDK_DISPLAY(), win);
    RET();
}

static  void
clicked( GtkWidget *widget, tray *tr)
{
    Window win, root;
    int w, h, dummy;
    ENTER;
    win = Select_Window(GDK_DISPLAY());
    win = XmuClientWindow (GDK_DISPLAY(), win);
    XGetGeometry (GDK_DISPLAY(), win, &root, &dummy, &dummy, &w, &h, &dummy, &dummy);
    DBG("win=0x%x (%d,%d)\n", win, w, h);
    if (w > 30 || h > 30) {
        fprintf(stderr, "too big\n");
        RET();
    }
    add_win(tr, win);
    RET();
}


GdkFilterReturn
tr_event_filter( XEvent *xev, GdkEvent *event, tray *tr)
{
    
    ENTER;
    g_assert(tr != NULL);
    if (xev->type == PropertyNotify && xev->xproperty.window == GDK_ROOT_WINDOW()
          && xev->xproperty.atom == a_NET_CLIENT_LIST) {

        Window *wins;
        int win_num, oi, ni;
        
        wins = get_xaproperty (GDK_ROOT_WINDOW(), a_NET_CLIENT_LIST, XA_WINDOW, &win_num);
        if (!wins) 
            RET(GDK_FILTER_CONTINUE);

        for (oi = ni = 0; (ni < win_num) && (oi < tr->win_num); ni++) {
            for (;(oi < tr->win_num) && (wins[ni] != tr->wins[oi]); oi++); 
            if (oi == tr->win_num)
                break;
        }
        for (; ni < win_num; ni++) {
            Window *tmpw;
            int tmpi;
            tmpw = get_xaproperty (wins[ni], a_KDE_NET_WM_SYSTEM_TRAY_WINDOW_FOR, XA_WINDOW, &tmpi);
            if (tmpw) {
                XFree(tmpw);
                DBG("tray win %x\n", wins[ni]);
                add_win(tr, wins[ni]);
            }
        }
            
        if (tr->wins)
            XFree(tr->wins);
        tr->wins = wins;
        tr->win_num = win_num;
    }
    RET(GDK_FILTER_CONTINUE);
}


static void
tray_destructor(plugin *p)
{
    tray *tr = (tray *)p->priv;

    ENTER;
    gdk_window_remove_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)tr_event_filter, tr);
    gtk_widget_destroy(tr->mainw);
    g_free(tr);
    RET();
}

    


static int
tray_constructor(plugin *p)
{
    line s;
    s.len = 256;
    tray *tr;
    GtkWidget *button;

    ENTER;
    while (get_line(p->fp, &s) != LINE_BLOCK_END) {
        ERR( "image: illegal in this context %s\n", s.str);
        RET(0);
    }

    tr = g_new0(tray, 1);
    g_return_val_if_fail(tr != NULL, 0);
    p->priv = tr;
    tr->plug = p;
    tr->mainw =  p->panel->my_box_new(FALSE, 1);
    button = gtk_button_new_with_label("x");
    gtk_signal_connect (GTK_OBJECT (button), "clicked",
          GTK_SIGNAL_FUNC (clicked), (gpointer) tr);
    gtk_widget_show(button);
    gtk_box_pack_start(GTK_BOX(tr->mainw), button, FALSE, FALSE, 0);
    gtk_widget_show(tr->mainw);
    gtk_container_add(GTK_CONTAINER(p->pwid), tr->mainw);
    XSelectInput (GDK_DISPLAY(), GDK_ROOT_WINDOW(), PropertyChangeMask);
    gdk_window_add_filter(GDK_ROOT_PARENT(), (GdkFilterFunc)tr_event_filter, tr);
    RET(1);

}


plugin_class tray_plugin_class = {
    fname: NULL,
    count: 0,

    type : "tray",
    name : "tray",
    version: "1.0",
    description : "Old KDE/GNOME Tray",

    constructor : tray_constructor,
    destructor  : tray_destructor,
};
