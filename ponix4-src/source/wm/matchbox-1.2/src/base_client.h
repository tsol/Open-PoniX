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

#define _GNU_SOURCE

#ifndef _BASE_CLIENT_H_
#define _BASE_CLIENT_H_

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "structs.h"
#include "wm.h"

Client* 
base_client_new (Wm *w, Window win);

void 
base_client_process_name (Client *c);

/* This will set the window attributes to what _we_ want */
void 
base_client_configure (Client *c);

/* sets inital 'object methods' */
void 
base_client_set_funcs (Client *c);

/* Frame the window if needed */
void 
base_client_reparent (Client *c);

/* redraw the clients frame */
void 
base_client_redraw (Client *c, Bool use_cache);

/* Hide any transients */
void 
base_client_hide_transients (Client *c);

/* button press on frame */
void 
base_client_button_press (Client *c, XButtonEvent *e);

/* move and resize the window */
void 
base_client_move_resize (Client *c);

/* iconize client */
void 
base_client_iconize (Client *c);

/* return the 'area' covered by the window. Including the frame
   Would return 0 for an unmapped window
*/
void 
base_client_get_coverage (Client *c, int *x, int *y, int *w, int *h);

void 
base_client_hide (Client *c);

void 
base_client_show (Client *c);

void 
base_client_destroy (Client *c);

#endif
