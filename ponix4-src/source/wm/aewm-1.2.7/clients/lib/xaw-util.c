/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include "switch.h"
#include "xaw-util.h"
#include <X11/StringDefs.h>

static void get_mouse_position(int *x, int *y)
{
    Window mouse_root, mouse_win;
    int win_x, win_y;
    unsigned int mask;

    XQueryPointer(dpy, root, &mouse_root, &mouse_win,
        x, y, &win_x, &win_y, &mask);
}

void popup_cb(Widget w, XtPointer data, XtPointer call)
{
    int x, y;

    get_mouse_position(&x, &y);
    XtVaSetValues((Widget)data, XtNx, x, XtNy, y, NULL);
    XtPopup((Widget)data, XtGrabExclusive);
}
