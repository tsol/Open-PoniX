/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "aeclients.h"
#include "panel.h"

Display *dpy;
Window root;

int main(int argc, char **argv)
{
    int opt_v = 0;
    int i;
    deskarea_t da;

    dpy = XOpenDisplay(NULL);

    if (!dpy) {
        fprintf(stderr, "can't open $DISPLAY: '%s'\n", getenv("DISPLAY"));
        exit(1);
    }

    root = DefaultRootWindow(dpy);
    gnome_pda = XInternAtom(dpy, "GNOME_PANEL_DESKTOP_AREA", False);
    get_gnome_pda(&da);

    for (i = 1; i < argc; i++) {
        if ARG("left", "l", 1) {
            da.left = atoi(argv[++i]);
            continue;
        }
        if ARG("right", "r", 1) {
            da.right = atoi(argv[++i]);
            continue;
        }
        if ARG("top", "t", 1) {
            da.top = atoi(argv[++i]);
            continue;
        }
        if ARG("bottom", "b", 1) {
            da.bottom = atoi(argv[++i]);
            continue;
        }
        if ARG("verbose", "v", 0) {
            opt_v  = 1;
            continue;
        }
        /* nothing matched */
        fprintf(stderr,
            "usage: set-gnome-pda [--left|-l n] [--right|-r n]\n"
            "                     [--top|-t n] [--bottom|-b n]\n"
            "                     [--verbose|-v]\n");
        exit(2);
    }

    set_gnome_pda(&da);

    if (opt_v) {
        printf("Left:\t%ld\n", da.left);
        printf("Right:\t%ld\n", da.right);
        printf("Top:\t%ld\n", da.top);
        printf("Bottom:\t%ld\n", da.bottom);
    }

    XCloseDisplay(dpy);
    return 0;
}
