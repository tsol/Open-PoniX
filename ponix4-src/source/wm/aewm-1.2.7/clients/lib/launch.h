/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_CLIENTS_LAUNCH_H
#define AEWM_CLIENTS_LAUNCH_H

#include "aeclients.h"

typedef void (*add_item_func)(menu_t, char *, char *);
typedef menu_t (*add_submenu_func)(menu_t, char *);

extern void make_launch_menu(char *, menu_t, add_item_func, add_submenu_func);
extern void fork_exec(char *);

#endif /* AEWM_CLIENTS_LAUNCH_H */
