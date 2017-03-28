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

#ifndef _MAIN_CLIENT_H_
#define _MAIN_CLIENT_H_

#include "structs.h"
#include "base_client.h"
#include "wm.h"
#include "main_client.h"
#include "misc.h"
#include "select_client.h"

#define TITLE_HIDDEN_SZ 4

Client* 
main_client_new (struct _wm *w, Window win);

void 
main_client_check_for_state_hints(Client *c);

void 
main_client_check_for_single(Client *c);

void 
main_client_init(Client *c);

void 
main_client_configure(Client *c);

int
main_client_manage_toolbars_for_fullscreen(Client *c, 
					   Bool    main_client_showing);


void 
main_client_reparent(Client *c);

void 
main_client_redraw(Client *c, Bool use_cache);

void 
main_client_button_press(Client *c, XButtonEvent *e);

void 
main_client_move_resize(Client *c);

void 
main_client_destroy(Client *c);

void 
main_client_iconize(Client *c);

int 
main_client_title_height(Client *c);

void 
main_client_get_coverage(Client *c, int *x, int *y, int *w, int *h);

void
main_client_unmap(Client *c);


void 
main_client_hide(Client *c);

void 
main_client_show(Client *c);

void 
main_client_toggle_title_bar(Client *c);

void 
main_client_toggle_fullscreen(Client *c);

#endif
