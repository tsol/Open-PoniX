/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include "aeclients.h"
#include "launch.h"
#include "switch.h"
#include "xaw-util.h"

#define LAUNCH 0
#define SWITCH 1

void add_menu_item(menu_t, char *, char *); 
menu_t add_sub_menu(menu_t, char *);
void make_client_button(menu_t, Window);
void raise_win_cb(Widget, XtPointer, XtPointer);
void fork_exec_cb(Widget, XtPointer, XtPointer);

int main(int argc, char **argv)
{
    Widget toplevel, rootmenu;
    int i, mode = LAUNCH;
    char *opt_config = NULL;

    toplevel = XtInitialize(argv[0], "AeMenu", NULL, 0, &argc, argv);
    dpy = XtDisplay(toplevel);
    root = DefaultRootWindow(dpy);
    rootmenu = XtVaCreatePopupShell("menu", simpleMenuWidgetClass, toplevel, NULL);

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


    if (mode == LAUNCH) {
        make_launch_menu(opt_config, rootmenu, add_menu_item, add_sub_menu);
    } else {
        wm_state = XInternAtom(dpy, "WM_STATE", False);
        update_clients(rootmenu, root, make_client_button, NULL);
    }

    popup_cb(NULL, rootmenu, NULL);
    XtMainLoop();
    return 0;
}

void add_menu_item(menu_t menu, char *label, char *cmd)
{
    Widget menu_item = XtVaCreateManagedWidget(label, smeBSBObjectClass, menu, NULL);
    XtAddCallback(menu_item, XtNcallback, fork_exec_cb, cmd);
}

menu_t add_sub_menu(menu_t menu, char *label)
{
    Widget new_menu, menu_item;
    new_menu = XtVaCreatePopupShell(label, simpleMenuWidgetClass, menu, NULL);
    menu_item = XtVaCreateManagedWidget(label, smeBSBObjectClass, menu,
        XtNmenuName, label, NULL);
    XtAddCallback(menu_item, XtNcallback, popup_cb, (XtPointer)new_menu);
    return new_menu;
}

void make_client_button(menu_t menu, Window w)
{
    Widget menu_item;
    char buf[BUF_SIZE];

    get_wm_name(w, buf, sizeof buf);
    menu_item = XtVaCreateManagedWidget(buf, smeBSBObjectClass, menu, NULL);
    XtAddCallback(menu_item, XtNcallback, raise_win_cb, (XtPointer)w);
}

void raise_win_cb(Widget w, XtPointer data, XtPointer call)
{
    raise_win((Window)data);
    exit(0);
}

void fork_exec_cb(Widget w, XtPointer data, XtPointer call)
{
    fork_exec(data);
    exit(0);
}
