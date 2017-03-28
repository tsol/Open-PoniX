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

#ifndef _DOCKBAR_CLIENT_H_
#define _DOCKBAR_CLIENT_H_

#include "structs.h"
#include "base_client.h"
#include "client_common.h"
#include "toolbar_client.h"
#include "wm.h"
#include "misc.h"

Client* dockbar_client_new (Wm *w, Window win);

void 
dockbar_client_show(Client *c);

void 
dockbar_client_hide(Client *c);

void 
dockbar_client_configure(Client *c);

void 
dockbar_client_get_coverage(Client *c, int *x, int *y, int *w, int *h);

void 
dockbar_client_move_resize(Client *c);

void 
dockbar_client_destroy(Client *c);

#endif



