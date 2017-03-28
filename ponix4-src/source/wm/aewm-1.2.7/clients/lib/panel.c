/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <X11/Xmd.h>
#include <X11/Xatom.h>
#include "panel.h"
#include "gnome-pda.h"
#include "switch.h"

Atom gnome_pda;

void sig_handler(int signal)
{
    if (signal == SIGCHLD) wait(NULL);
}

void set_gnome_pda(deskarea_t *da)
{
    CARD32 data[4];

    data[0] = da->left;
    data[1] = da->right;
    data[2] = da->top;
    data[3] = da->bottom;

    XChangeProperty(dpy, root, gnome_pda, XA_CARDINAL,
        32, PropModeReplace, (unsigned char *)data, 4);
    XFlush(dpy);
}

void clear_gnome_pda(void)
{
    XDeleteProperty(dpy, root, gnome_pda);
}
