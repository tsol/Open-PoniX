/* aewm - a minimalist X11 window mananager. vim:sw=4:ts=4:et
 *
 * Copyright 1998-2006 Decklin Foster <decklin@red-bean.com>. This
 * program is free software; please see LICENSE for details. */

#ifndef AEWM_CLIENTS_PANEL_H
#define AEWM_CLIENTS_PANEL_H

#include <signal.h>
#include <X11/X.h>
#include "gnome-pda.h"

extern void sig_handler(int);
extern void set_gnome_pda(deskarea_t *);
extern void clear_gnome_pda(void);

#endif /* AEWM_CLIENTS_PANEL_H */
