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

#ifndef _HAVE_STACK_H_
#define _HAVE_STACK_H_

#include "structs.h"
#include "wm.h"
#include "config.h"

#define stack_enumerate(w,c)                               \
 if ((w)->stack_bottom)                                    \
   for ((c)=(w)->stack_bottom; (c) != NULL; (c)=(c)->above) 

#define stack_enumerate_reverse(w,c)                       \
 if ((w)->stack_top)                                       \
   for ((c)=(w)->stack_top; (c) != NULL; (c)=(c)->below) 

#define stack_enumerate_transients(w,c,t)                  \
 if ((w)->stack_bottom)                                    \
   for ((c)=(w)->stack_bottom; (c) != NULL; (c)=(c)->above) \
     if ((c)->trans == (t))

#define stack_move_top(c) \
 stack_move_above_client((c), (c)->wm->stack_top)

#define stack_add_bottom(c) \
 stack_move_below_client((c), (c)->wm->stack_bottom)

#define stack_empty(w) \
 ((w)->stack_bottom == NULL)

#define n_stack_items(w) \
 (w)->stack_n_items


void
stack_add_above_client(Client *client, Client *client_below);

void 
stack_append_top(Client *client);

void 
stack_prepend_bottom(Client *client);

void
stack_remove(Client *client);

void
stack_move_transients_to_top(Wm *w, Client *client_trans_for, int flags);

void
stack_move_client_above_type(Client *client, int type_below);

void
stack_move_type_above_client(Wm               *w, 
			     MBClientTypeEnum  wanted_type, 
			     Client           *client);

void
stack_move_above_client(Client *client, Client *client_below);

Client*
stack_get_above(Client* client_below, MBClientTypeEnum wanted_type);

Client*
stack_get_below(Client* client_above, MBClientTypeEnum wanted_type);

Client*
stack_cycle_forward(Wm *w, MBClientTypeEnum type_to_cycle);

Client*
stack_cycle_backward(Wm *w, MBClientTypeEnum type_to_cycle);

Client*
stack_get_highest(Wm *w, MBClientTypeEnum wanted_type);

Client*
stack_get_lowest(Wm *w, MBClientTypeEnum wanted_type);

Window*
stack_get_window_list(Wm *w);

void
stack_sync_to_display(Wm *w);

void
stack_dump(Wm *w);


#endif
