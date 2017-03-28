/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "aeclients.h"
#include "launch.h"
#include "switch.h"

#define LAUNCH 0
#define SWITCH 1

void make_client_button(menu_t, Window);
void add_menu_item(menu_t, char *, char *);
menu_t add_sub_menu(menu_t, char *);
void fork_exec_cb(GtkWidget *, char *);
void raise_win_cb(GtkWidget *, Window);

int main(int argc, char **argv)
{
    GtkWidget *rootmenu;
    int i, mode = LAUNCH;
    char *opt_config = NULL;

    gtk_init(&argc, &argv);

    for (i = 1; i < argc; i++) {
        if ARG("config", "rc", 1)  {
            opt_config = argv[++i];
            continue;
        }
        if ARG("launch", "l", 0)   {
            mode = LAUNCH;
            continue;
        }
        if ARG("switch", "s", 0)   {
            mode = SWITCH;
            continue;
        }
        /* nothing matched */
        fprintf(stderr, "usage: aemenu [--switch|-s] [--config|-rc <file>]\n");
        exit(2);
    }

    rootmenu = gtk_menu_new();

    if (mode == LAUNCH) {
        make_launch_menu(opt_config, rootmenu, add_menu_item, add_sub_menu);
    } else {
        dpy = GDK_DISPLAY();
        root = GDK_ROOT_WINDOW();
        wm_state = XInternAtom(dpy, "WM_STATE", False);
        update_clients(rootmenu, root, make_client_button, NULL);
    }

    gtk_signal_connect_object(GTK_OBJECT(rootmenu), "deactivate",
        GTK_SIGNAL_FUNC(gtk_main_quit), NULL);
    gtk_menu_popup(GTK_MENU(rootmenu), NULL, NULL, NULL, NULL, 0, 0);

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

void make_client_button(menu_t menu, Window w)
{
    GtkWidget *item;
    char buf[BUF_SIZE];

    get_wm_name(w, buf, sizeof buf);

    item = gtk_menu_item_new_with_label(buf);
    gtk_menu_append(GTK_MENU(menu), item);
    gtk_signal_connect(GTK_OBJECT(item), "activate",
        GTK_SIGNAL_FUNC(raise_win_cb), (gpointer) w);
    gtk_widget_show(item);
}

void fork_exec_cb(GtkWidget *widget, char *data)
{
    fork_exec(data);
    gtk_main_quit();
}

void raise_win_cb(GtkWidget *widget, Window w)
{
    raise_win(w);
    gtk_main_quit();
}
