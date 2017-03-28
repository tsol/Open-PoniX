/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include "switch.h"

Display *dpy;
Window root;
Atom wm_state;
XContext client_tab;

long get_wm_state(Window w)
{
    Atom real_type; int real_format;
    long state = WithdrawnState;
    unsigned long items_read, bytes_left;
    unsigned char *data;

    if (XGetWindowProperty(dpy, w, wm_state, 0L, 2L, False,
            wm_state, &real_type, &real_format, &items_read, &bytes_left,
            &data) == Success && items_read) {
        state = *(long *)data;
        XFree(data);
    }
    return state;
}

void get_wm_name(Window w, char *buf, size_t len)
{
    char *name;

    XFetchName(dpy, w, &name);
    if (!name) { buf[0] = '\0'; return; }

    if (get_wm_state(w) == NormalState) {
        if (strlen(name) >= len) strcpy(name+len-4, "...");
        strcpy(buf, name);
    } else {
        if (strlen(name) >= len-2) strcpy(name+len-6, "...");
        sprintf(buf, "[%s]", name);
    }

    XFree(name);
}

/* root here means the root of the subtree we're currently looking at.
 * we'll sometimes get passed a WM frame so we have to search
 * downwards until we find a client window. If we don't want to check
 * for existing but changed windows then pass in NULL for
 * update_client_button(). */

void update_clients(menu_t menu, Window root,
    add_client_func make_client_button,
    add_client_func update_client_button)
{
    Window dummy_w, *wins;
    unsigned int i, nwins;
    XPointer dummy_p;

    if (update_client_button
            && XFindContext(dpy, root, client_tab, &dummy_p) == Success) {
        update_client_button(menu, root);
    } else if (get_wm_state(root) != WithdrawnState) {
        make_client_button(menu, root);
    } else if (XQueryTree(dpy, root, &dummy_w, &dummy_w, &wins, &nwins)) {
        for (i = 0; i < nwins; i++)
            update_clients(menu, wins[i], make_client_button, update_client_button);
        XFree(wins);
    }
}

void raise_win(Window w)
{
    XMapRaised(dpy, w);
    XSync(dpy, False);
}
