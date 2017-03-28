/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Command.h>
#include "aeclients.h"
#include "launch.h"
#include "switch.h"
#include "panel.h"
#include "xaw-util.h"

#define NAME_SIZE 25

void add_menu_item(menu_t, char *, char *);
menu_t add_sub_menu(menu_t menu, char *label);
void make_client_button(menu_t, Window);
void update_client_button(menu_t, Window);
void remove_client_button(Window);
void watch_window(Window, long);
void check_event(Widget, XtPointer, XEvent *, Boolean *);
int ignore_xerror(Display *, XErrorEvent *);
void quit_cb(Widget, XtPointer, XtPointer);
void fork_exec_cb(Widget, XtPointer, XtPointer);
void raise_win_cb(Widget, XtPointer, XtPointer);

Widget clients_box, dummy;

int main(int argc, char **argv)
{
    Widget toplevel, paned, cmds_box, menu_button, launch_menu, quit_button;
    int i;
    char *opt_config = NULL;
    struct sigaction act;

    toplevel = XtInitialize(argv[0], "AePanel", NULL, 0, &argc, argv);

    for (i = 1; i < argc; i++) {
        if ARG("config", "rc", 1)  {
            opt_config = argv[++i];
            continue;
        }
        /* nothing matched */
        fprintf(stderr, "usage: aepanel [--config|-rc <file>]\n");
        exit(2);
    }

    act.sa_handler = sig_handler;
    act.sa_flags = 0;
    sigaction(SIGCHLD, &act, NULL);

    dpy = XtDisplay(toplevel);
    root = DefaultRootWindow(dpy);
    wm_state = XInternAtom(dpy, "WM_STATE", False);
    client_tab = XUniqueContext();
    XSetErrorHandler(ignore_xerror);

    dummy = XtVaCreateWidget("dummy",
        coreWidgetClass, toplevel, XtNwidth, 1, XtNheight, 1, NULL);
    watch_window(root, SubstructureNotifyMask);
    XtAddRawEventHandler(dummy,
        SubstructureNotifyMask|StructureNotifyMask|PropertyChangeMask,
        False, check_event, NULL);

    paned = XtVaCreateManagedWidget("paned",
        panedWidgetClass, toplevel, NULL);
    cmds_box = XtVaCreateManagedWidget("commands",
        boxWidgetClass, paned, NULL);
    menu_button = XtVaCreateManagedWidget("Menu",
        menuButtonWidgetClass, cmds_box, NULL);
    launch_menu = XtVaCreatePopupShell("menu",
        simpleMenuWidgetClass, menu_button, NULL);
    quit_button = XtVaCreateManagedWidget("Quit",
        commandWidgetClass, cmds_box, NULL);
    XtAddCallback(quit_button,
        XtNcallback, quit_cb, NULL);
    clients_box = XtVaCreateManagedWidget("clients",
        boxWidgetClass, paned, NULL);

    make_launch_menu(opt_config, launch_menu, add_menu_item, add_sub_menu);
    update_clients(clients_box, root, make_client_button, update_client_button);

    XtRealizeWidget(toplevel);
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

void make_client_button(menu_t clients_box, Window w)
{
    Widget button;
    char buf[NAME_SIZE];

    get_wm_name(w, buf, sizeof buf);

    button = XtVaCreateManagedWidget(buf, commandWidgetClass, clients_box, NULL);
    XtAddCallback(button, XtNcallback, raise_win_cb, (XtPointer)w);

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
        XtVaSetValues((Widget) button, XtNlabel, buf, NULL);
    }
}

void remove_client_button(Window w)
{
    XPointer button;

    if (XFindContext(dpy, w, client_tab, &button) == Success) {
        XtDestroyWidget((Widget) button);
        XDeleteContext(dpy, w, client_tab);
    }
}

void watch_window(Window w, long mask)
{
    XtRegisterDrawable(dpy, w, dummy);
    XSelectInput(dpy, w, mask);
}

void check_event(Widget w, XtPointer data, XEvent *e, Boolean *dispatch)
{
    switch (e->type) {
        case MapNotify:
            update_clients(clients_box, e->xmap.window, make_client_button, update_client_button);
            break;
        case PropertyNotify:
            update_clients(clients_box, e->xproperty.window, make_client_button, update_client_button);
            break;
        case DestroyNotify:
            remove_client_button(e->xdestroywindow.window);
            break;
    }
}

/* icky icky icky */

int ignore_xerror(Display *dpy, XErrorEvent *e)
{
    return 0;
}

void quit_cb(Widget w, XtPointer data, XtPointer call)
{
    exit(0);
}

void fork_exec_cb(Widget w, XtPointer data, XtPointer call)
{
    fork_exec(data);
}

void raise_win_cb(Widget w, XtPointer data, XtPointer call)
{
    raise_win((Window)data);
}
