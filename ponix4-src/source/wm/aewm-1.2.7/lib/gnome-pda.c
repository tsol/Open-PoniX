/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include "gnome-pda.h"

/* Reads the GNOME_PANEL_DESKTOP_AREA hint into the args, if it
 * exists. Each number is the margin in pixels on that side of the
 * display where we don't want to place clients. If there is no hint,
 * we act as if it was all zeros (no margin). */

int get_gnome_pda(deskarea_t *da)
{
    Atom real_type; int real_format;
    unsigned long items_read, bytes_left;
    unsigned char *data;

    if (XGetWindowProperty(dpy, root, gnome_pda, 0L, 4L, False,
            XA_CARDINAL, &real_type, &real_format, &items_read, &bytes_left,
            &data) == Success && items_read) {
        int *di = (int *) data;
        da->left = di[0];
        da->right = di[1];
        da->top = di[2];
        da->bottom = di[3];
        XFree(data);
        return 1;
    } else {
        da->left = 0;
        da->right = 0;
        da->top = 0;
        da->bottom = 0;
        return 0;
    }
}
