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

#ifndef _CLIENT_COMMON_H_
#define _CLIENT_COMMON_H_

#include "structs.h"
#include "main_client.h"
#include "toolbar_client.h"
#include "dockbar_client.h"
#include "dialog_client.h"
#include "list.h"
#include "misc.h"

#define client_title_frame(c) (c)->frames_decor[NORTH]

void 
client_set_state (Client *c, int state);

long 
client_get_state (Client *c);

void 
client_deliver_config (Client *c);

void 
client_deliver_wm_protocol (Client *c, Atom delivery);

void
client_get_wm_protocols(Client *c);

void
client_deliver_message(Client       *c, 
		       Atom          delivery,
		       unsigned long data0,
		       unsigned long data1,
		       unsigned long data2,
		       unsigned long data3,
		       unsigned long data4);

Bool
client_obliterate(Client *c);

void 
client_deliver_delete (Client *c);

int 
client_want_focus (Client *c);

Bool
client_set_focus(Client *c);

void
client_get_transient_list(Wm *w, MBList **list, Client *c);

Client*
client_get_highest_transient(Client *c, int client_flags, Client *ignore);

Client *
client_get_next (Client* c, MBClientTypeEnum wanted);

Client *
client_get_prev (Client* c, MBClientTypeEnum wanted);


void
client_decor_frames_move_resize(Client *c, 
				int     width_west, 
				int     width_east, 
				int     height_north,
				int     height_south);

void
client_decor_frames_init(Client *c, 
			 int     width_west, 
			 int     width_east, 
			 int     height_north,
			 int     height_south);

void
client_init_backing_mask (Client *c, 
			  int     width, 
			  int     height, 
			  int     height_north, 
			  int     height_south,
			  int     width_east, 
			  int     width_west );


struct list_item* 
client_get_button_list_item_from_event (Client *c, XButtonEvent *e);

int 
client_do_button_ops (Client *c, int frame_id, XButtonEvent *e);

int 
client_button_do_ops (Client *c, XButtonEvent *e, int frame_type,
		     int w, int h);

void
client_button_init(Client         *c, 
		   Window          win_parent, 
		   MBClientButton *b,
		   int             x, 
		   int             y, 
		   int             width, 
		   int             height,
		   Bool            want_inputonly,
		   void           *data);

MBClientButton *
client_button_new (Client *c, 
		   Window  win_parent, 
		   int     x, 
		   int     y, 
		   int     w, 
		   int     h,
		   Bool    want_inputonly, 
		   void   *data );

void 
client_button_remove (Client *c, int button_action);

void 
client_buttons_delete_all (Client *c);


#endif 
