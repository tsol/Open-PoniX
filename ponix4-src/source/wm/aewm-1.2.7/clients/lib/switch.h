/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_CLIENTS_SWITCH_H
#define AEWM_CLIENTS_SWITCH_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "aeclients.h"

typedef void (*add_client_func)(menu_t, Window);

extern Display *dpy;
extern Window root;
extern Atom wm_state;
extern XContext client_tab;

extern long get_wm_state(Window);
extern void get_wm_name(Window, char *, size_t);
extern void update_clients(menu_t, Window, add_client_func, add_client_func);
extern void raise_win(Window);

#endif /* AEWM_CLIENTS_SWITCH_H */
