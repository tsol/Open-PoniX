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

#ifndef _COMPOSITE_ENGINE_H_
#define _COMPOSITE_ENGINE_H_

#include "structs.h"
#include "wm.h"


#ifdef USE_COMPOSITE

Bool
comp_engine_init (Wm *w);

void
comp_engine_set_defualts(Wm *w);

void
comp_engine_theme_init(Wm *w);

void
comp_engine_deinit(Wm *w);

void
comp_engine_reinit(Wm *w);

void
comp_engine_client_init(Wm *w, Client *client);

int
comp_engine_client_get_trans_prop(Wm *w, Client *client);

void
comp_engine_client_show(Wm *w, Client *client);

void
comp_engine_client_hide(Wm *w, Client *client);

void
comp_engine_client_destroy(Wm *w, Client *client);

void
comp_engine_client_repair (Wm *w, Client *client);

void
comp_engine_client_configure(Wm *w, Client *client);

void
comp_engine_handle_events(Wm *w, XEvent *ev);

void
comp_engine_destroy_root_buffer(Wm *w);

void
comp_engine_render(Wm *w, XserverRegion region);

#else

/* All no ops */

#define comp_engine_init(w) ;
#define comp_engine_reinit(w) ;
#define comp_engine_deinit(w) ;
#define comp_engine_theme_init(w);
#define comp_engine_set_defualts(w);
#define comp_engine_client_init(w, c) ;
#define comp_engine_client_get_trans_prop(w, c);
#define comp_engine_client_show(w, c) ;
#define comp_engine_client_hide(w, c) ;
#define comp_engine_client_destroy(w, c) ;
#define comp_engine_client_repair(w, c) ; 
#define comp_engine_client_configure(w, c) ;
#define comp_engine_handle_events(w, c) ;
#define comp_engine_destroy_root_buffer(w) ;
#define comp_engine_render(w, r) ;
#define comp_engine_get_argb32_visual(w) ;


#endif

void
comp_engine_time(Wm *w);


#endif
