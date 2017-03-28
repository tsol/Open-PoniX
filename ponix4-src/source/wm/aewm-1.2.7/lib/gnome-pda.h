/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_GNOME_PDA_H
#define AEWM_GNOME_PDA_H

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

struct deskarea {
    long left;
    long right;
    long top;
    long bottom;
};

typedef struct deskarea deskarea_t;

int get_gnome_pda(deskarea_t *);

extern Display *dpy;
extern Window root;
extern Atom gnome_pda;

#endif /* AEWM_GNOME_PDA_H */
