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

#ifndef _WM_H_
#define _WM_H_

#include "structs.h"
#include "stack.h"
#include "base_client.h"
#include "main_client.h"
#include "toolbar_client.h"
#include "dockbar_client.h"
#include "dialog_client.h"
#include "desktop_client.h"
#include "client_common.h"
#include "misc.h"
#include "ewmh.h"
#include "composite-engine.h"
#include "session.h"

#ifdef STANDALONE
#include "mbtheme-standalone.h"
#else
#include "mbtheme.h"
#endif

#include "keys.h"

/* Atoms */

#define WITHDRAW 1

#define FRAME  1
#define WINDOW 2

#define ChildMask (SubstructureRedirectMask|SubstructureNotifyMask)
#define ButtonMask (ButtonPressMask|ButtonReleaseMask)
#define MouseMask (ButtonMask|PointerMotionMask)
#define KeyMask (KeyPressMask|KeyReleaseMask)

Wm 
*wm_new(int argc, char **argv);

void 
wm_usage(char *progname);

void 
wm_load_config(Wm *w, int *argc, char *argv[]);

void 
wm_init_existing(Wm *w);

/* events */
void 
wm_event_loop(Wm* w);

void 
wm_handle_button_event(Wm *w, XButtonEvent *e);

void 
wm_handle_keypress(Wm *w, XKeyEvent *e);

void 
wm_handle_map_request(Wm *w, XMapRequestEvent *e);

void 
wm_handle_unmap_event(Wm *w, XUnmapEvent *e);

void 
wm_handle_expose_event(Wm *w, XExposeEvent *e);

void 
wm_handle_configure_request(Wm *w, XConfigureRequestEvent *e);

void 
wm_handle_configure_notify(Wm *w, XConfigureEvent *e);

void 
wm_handle_destroy_event(Wm *w, XDestroyWindowEvent *e);

void 
wm_handle_client_message(Wm *w, XClientMessageEvent *e);

void 
wm_handle_property_change(Wm *w, XPropertyEvent *e);

void 
wm_handle_enter_notify(Wm *w, XEnterWindowEvent *e);

Client *
wm_find_client(Wm *w, Window win, int mode);

Client *
wm_make_new_client(Wm *w, Window win);

void    
wm_remove_client(Wm *w, Client *c);

void    
wm_activate_client(Client *c);

void    
wm_lowlight(Wm *w, Client *c);

void 
wm_update_layout(Wm *w, Client *c, signed int amount);

int 
wm_get_offsets_size(Wm*     w, 
		    int     wanted_direction,
		    Client* ignore_client, 
		    Bool    include_toolbars
		    );

void 
wm_set_cursor_visibility(Wm *w, Bool visible);

Client * 			/* Returns either desktop or main app client */
wm_get_visible_main_client(Wm *w);

void 
wm_toggle_desktop(Wm *w);

Client 
*wm_get_desktop(Wm *w);

#ifdef USE_LIBSN
void wm_sn_cycle_remove(Wm *w, Window xid);
#endif

#ifdef USE_GCONF
void
gconf_key_changed_callback (GConfClient *client,
			    guint        cnxn_id,
			    GConfEntry  *entry,
			    gpointer    user_data);
#endif

#endif
