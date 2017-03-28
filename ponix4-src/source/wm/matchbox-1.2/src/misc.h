/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#ifndef _MISC_H_
#define _MISC_H_

#include "structs.h"
#include "wm.h"
#include "main_client.h"
#include "config.h"

void err(const char *fmt, ...);

#ifndef HAVE_STRSEP
char *strsep(char **stringp, char *delim);
#endif

void 
fork_exec(char *cmd);

void 
sig_handler(int signal);

int 
handle_xerror(Display *dpy, XErrorEvent *e);

int 
ignore_xerror(Display *dpy, XErrorEvent *e);

void 
misc_trap_xerrors(void);

int 
misc_untrap_xerrors(void);

int 
mwm_get_decoration_flags(Wm *w, Window win);

void
misc_scale_wm_app_icon(Wm *w);

#endif

