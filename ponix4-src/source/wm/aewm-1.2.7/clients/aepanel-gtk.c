/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <gdk/gdkx.h>
#include <gtk/gtk.h>
#include "aeclients.h"
#include "launch.h"
#include "switch.h"
#include "panel.h"

void add_menu_item(menu_t, char *, char *);
menu_t add_sub_menu(menu_t, char *);
void make_client_button(menu_t, Window);
void update_client_button(menu_t, Window);
void remove_client_button(Window);
void watch_window(Window, long);
GdkFilterReturn check_event(GdkXEvent *, GdkEvent *, gpointer);
void show_menu_cb(GtkWidget *, gpointer);
void quit_cb(GtkWidget *, gpointer);
void raise_win_cb(GtkWidget *, Window);
void fork_exec_cb(GtkWidget *, char *);

#define NAME_SIZE 60

GtkWidget *clients_box, *menu_button = NULL;
deskarea_t orig_da;
int opt_bottom;

int main(int argc, char **argv)
{
    GtkWidget *toplevel, *frame, *launch_menu, *bbox, *quit_button;
    int width, height, i;
    char *opt_config = NULL;
    struct sigaction act;
    deskarea_t new_da;

    gtk_init(&argc, &argv);
#ifdef USE_OLD_GTK
    gdk_error_warnings = 0; /* gag me with a spoon... */
#else
    gdk_error_trap_push(); /* slightly better, i guess */
#endif

    for (i = 1; i < argc; i++) {
        if ARG("config", "rc", 1)  {
            opt_config = argv[++i];
            continue;
        }
        if ARG("bottom", "b", 0)  {
            opt_bottom = 1;
            continue;
        }
        /* nothing matched */
        fprintf(stderr, "usage: aepanel [--bottom|-b] [--config|-rc <file>]\n");
        exit(2);
    }

    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);

    dpy = GDK_DISPLAY();
    root = GDK_ROOT_WINDOW();
    gdk_window_get_size(GDK_ROOT_PARENT(), &width, &height);
    client_tab = XUniqueContext();
    wm_state = XInternAtom(dpy, "WM_STATE", False);
    gnome_pda = XInternAtom(dpy, "GNOME_PANEL_DESKTOP_AREA", False);
    watch_window(root, SubstructureNotifyMask);

    toplevel = gtk_window_new(GTK_WINDOW_POPUP);
    gtk_widget_set_usize(toplevel, width, 0);

    launch_menu = gtk_menu_new();

    frame = gtk_frame_new(NULL);
    gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_OUT);
    gtk_container_add(GTK_CONTAINER(toplevel), frame);

    bbox = gtk_hbox_new(FALSE, SPACING);
    gtk_container_set_border_width(GTK_CONTAINER(bbox), SPACING-1);
    gtk_container_add(GTK_CONTAINER(frame), bbox);

    menu_button = gtk_button_new_with_label("Menu");
    gtk_signal_connect(GTK_OBJECT(menu_button), "clicked",
        GTK_SIGNAL_FUNC(show_menu_cb), launch_menu);
    gtk_box_pack_start(GTK_BOX(bbox), menu_button, FALSE, FALSE, 0);

    quit_button = gtk_button_new_with_label("Quit");
    gtk_signal_connect(GTK_OBJECT(quit_button), "clicked",
        GTK_SIGNAL_FUNC(quit_cb), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), quit_button, FALSE, FALSE, 0);

    clients_box = gtk_hbox_new(TRUE, SPACING);
    gtk_container_add(GTK_CONTAINER(bbox), clients_box);
    gtk_container_set_resize_mode(GTK_CONTAINER(clients_box), GTK_RESIZE_QUEUE);

    make_launch_menu(opt_config, launch_menu, add_menu_item, add_sub_menu);
    update_clients(clients_box, root, make_client_button, update_client_button);

    gtk_widget_show_all(toplevel);
    if (opt_bottom)
        gtk_widget_set_uposition(toplevel,
            0, height - toplevel->allocation.height);

    gdk_window_get_size(toplevel->window, &width, &height);
    get_gnome_pda(&orig_da);
    new_da = orig_da;
    if (opt_bottom)
        new_da.bottom = height;
    else
        new_da.top = height;
    set_gnome_pda(&new_da);

    gtk_main();
    return 0;
}

void add_menu_item(menu_t menu, char *label, char *cmd)
{
    GtkWidget *item;

    item = gtk_menu_item_new_with_label(label);
    gtk_menu_append(GTK_MENU(menu), item);
    gtk_signal_connect(GTK_OBJECT(item), "activate",
        GTK_SIGNAL_FUNC(fork_exec_cb), cmd);
    gtk_widget_show(item);
}

menu_t add_sub_menu(menu_t menu, char *label)
{
    GtkWidget *item, *newmenu;

    newmenu = gtk_menu_new();
    item = gtk_menu_item_new_with_label(label);
    gtk_menu_append(GTK_MENU(menu), item);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), newmenu);
    gtk_widget_show(item);

    return newmenu;
}

void make_client_button(menu_t clients_box, Window w)
{
    GtkWidget *button;
    char buf[NAME_SIZE];

    get_wm_name(w, buf, sizeof buf);

    button = gtk_button_new_with_label(buf);
    gtk_signal_connect(GTK_OBJECT(button), "clicked",
        GTK_SIGNAL_FUNC(raise_win_cb), (gpointer) w);
    gtk_box_pack_start(GTK_BOX(clients_box), button, TRUE, TRUE, 0);
    gtk_misc_set_alignment(GTK_MISC(GTK_BIN(button)->child), 0, 0.5);
    gtk_widget_show(button);

    XSaveContext(dpy, w, client_tab, (XPointer)button);
    watch_window(w, StructureNotifyMask|PropertyChangeMask);
}

void update_client_button(menu_t clients_box, Window w)
{
    XPointer button;
    char buf[NAME_SIZE];

    if (get_wm_state(w) == WithdrawnState) {
        remove_client_button(w);
    } else if (XFindContext(dpy, w, client_tab, &button) == Success) {
        get_wm_name(w, buf, sizeof buf);
        gtk_label_set_text(GTK_LABEL(GTK_BIN(button)->child), buf);
    }
}

void remove_client_button(Window w)
{
    XPointer button;

    if (XFindContext(dpy, w, client_tab, &button) == Success) {
        gtk_container_remove(GTK_CONTAINER(clients_box), GTK_WIDGET(button));
        XDeleteContext(dpy, w, client_tab);
    }
}

void watch_window(Window w, long mask)
{
    GdkWindow *gdkwin = gdk_window_lookup(w);

    XSelectInput(dpy, w, mask);
    gdk_window_add_filter(gdkwin, check_event, NULL);
}

GdkFilterReturn check_event(GdkXEvent *gdk_xevent, GdkEvent *event,
    gpointer dummy)
{
    XEvent *e = (XEvent *)gdk_xevent;

    switch (e->type) {
        case MapNotify:
            update_clients(clients_box, e->xmap.window,
                make_client_button, update_client_button);
            break;
        case PropertyNotify:
            update_clients(clients_box, e->xproperty.window,
                make_client_button, update_client_button);
            break;
        case DestroyNotify:
            remove_client_button(e->xdestroywindow.window);
            break;
    }

    return GDK_FILTER_CONTINUE;
}

#ifdef USE_OLD_GTK
void menu_position(GtkMenu *menu, gint *x, gint *y, gpointer data)
#else
void menu_position(GtkMenu *menu, gint *x, gint *y,
    gboolean *push_in, gpointer data)
#endif
{
    GtkWidget *button = GTK_WIDGET(data);
    gint wx, wy;

    gdk_window_get_root_origin(button->window, &wx, &wy);
    *x = wx + button->allocation.x;
    *y = wy + button->allocation.y;

    if (opt_bottom) {
        /* This is crap. I shouldn't have to do anything here, but if
         * the menu-position func gives coords near the bottom of the
         * screen, GTK+ will blindly try to follow them rather than
         * flipping the menu up (around the coord parallel to the
         * screen edge) like it would do in every other case (that is,
         * any instance where the coords are determined automatically).
         * Feh. All toolkits suck. Not only that, but this hack seems
         * to make drawing the menu a little bit slower. */
        GtkRequisition req;
        gtk_widget_size_request(GTK_WIDGET(menu), &req);
        *y -= req.height;
    } else {
        *y += button->allocation.height;
    }
}

void show_menu_cb(GtkWidget *widget, gpointer menu)
{
    gtk_menu_popup(menu, NULL, NULL, menu_position, menu_button, 0, 0);
}

void quit_cb(GtkWidget *widget, gpointer menu)
{
    set_gnome_pda(&orig_da);
    gtk_main_quit();
}

void raise_win_cb(GtkWidget *widget, Window w)
{
    raise_win(w);
}

void fork_exec_cb(GtkWidget *widget, char *data)
{
    fork_exec(data);
}
