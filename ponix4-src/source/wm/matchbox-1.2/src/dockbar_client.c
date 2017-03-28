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

/* 
 *  a Dockbar is a *Panel*.
 */

#include "dockbar_client.h"

static int dockbar_client_orientation_calc(Client *c);

Client*
dockbar_client_new(Wm *w, Window win)
{
   Client *c = base_client_new(w, win); 

   if (!c) return NULL;

   c->type         = MBCLIENT_TYPE_PANEL;
   c->configure    = &dockbar_client_configure;
   c->show         = &dockbar_client_show;
   c->hide         = &dockbar_client_hide;
   c->move_resize  = &dockbar_client_move_resize;
   c->destroy      = &dockbar_client_destroy;

   c->flags        = dockbar_client_orientation_calc(c);
   
   c->frame = c->window;

   return c;
}

static int
dockbar_client_orientation_calc(Client *c)
{
  Wm *w = c->wm;

  dbg("%s() called, x:%i, y:%i, w:%i h:%i\n", __func__, 
      c->x, c->y, c->width, c->height);

  /*   - Can only have one titlebar panel  
   *   - if theme does not have title bar it wont get mapped. 
   */
  if (!w->have_titlebar_panel) /* && mbtheme_has_titlebar_panel(w->mbtheme)) */
    {
      if (ewmh_state_check(c, c->wm->atoms[MB_WM_STATE_DOCK_TITLEBAR]))
	{
	  w->have_titlebar_panel = c;

	  /* Does the panel still want to be shown when we map the desktop ? */
	  if (ewmh_state_check(c, c->wm->atoms[MB_DOCK_TITLEBAR_SHOW_ON_DESKTOP]))
	    return CLIENT_DOCK_TITLEBAR|CLIENT_DOCK_TITLEBAR_SHOW_ON_DESKTOP;

	  return CLIENT_DOCK_TITLEBAR;
	}
    }

  if (c->width > c->height)	/* Assume Horizonal north/south Dock */
    {
      if (c->y < (w->dpy_height/2))
	return CLIENT_DOCK_NORTH;
      else
	return CLIENT_DOCK_SOUTH;
    }
  else
    {
      if (c->x < (w->dpy_width/2))
	return CLIENT_DOCK_WEST;
      else
	return CLIENT_DOCK_EAST;
    }
}

void
dockbar_client_configure(Client *c)
{
  Wm *w = c->wm;

  int n_offset = wm_get_offsets_size(c->wm, NORTH, c, False);
  int s_offset = wm_get_offsets_size(c->wm, SOUTH, c, False);
  int e_offset = wm_get_offsets_size(c->wm, EAST,  c, True);
  int w_offset = wm_get_offsets_size(c->wm, WEST,  c, True);
   
  /* XXX - we should check for overlapping and if this happens
     change em to normal clients. 
  */

   if (c->flags & CLIENT_DOCK_NORTH)
     {
       c->y = n_offset;
       c->x = w_offset;
       c->width  = w->dpy_width - e_offset - w_offset;
     }
   else if (c->flags & CLIENT_DOCK_SOUTH)
     {
       c->y = w->dpy_height - s_offset - c->height;
       c->x = w_offset;
       c->width  = w->dpy_width - e_offset - w_offset;
     }
   else if (c->flags & CLIENT_DOCK_WEST)
     {
       c->y = 0;
       c->x = w_offset;
       c->height = w->dpy_height;
     }
   else if (c->flags & CLIENT_DOCK_EAST)
     {
       c->y = 0;
       c->x = w->dpy_width - e_offset - c->width;
       c->height = w->dpy_height;
     }
   else if (c->flags & CLIENT_DOCK_TITLEBAR)
     {
       XRectangle rect;

       mbtheme_get_titlebar_panel_rect(w->mbtheme, &rect, NULL);
 
       c->x      = rect.x + w_offset; 
       c->y      = rect.y + n_offset;
       c->width  = rect.width;
       c->height = rect.height;
     }
   else
     {
       dbg("%s() : EEEK no dock type flag set !\n", __func__ );
     }

   dbg("%s() sizing as %i %i %i %i", __func__, c->x, c->y, 
       c->width, c->height);
   
   XSetWindowBorderWidth(w->dpy, c->window, 0);
   XSetWindowBorder(w->dpy, c->window, 0);
   
}

void
dockbar_client_move_resize(Client *c)
{
  Wm *w = c->wm;

  base_client_move_resize(c);

  dbg("%s() to %s  x: %i , y: %i w: %i h: %i \n", 
      __func__, c->name, c->x, c->y, c->width, c->height);

  XResizeWindow(w->dpy, c->window, c->width, c->height);
  XMoveWindow(w->dpy, c->window, c->x, c->y);
}

void
dockbar_client_show(Client *c) /*TODO: show and hide share common static func*/
{
  Wm *w = c->wm;

  if (client_get_state(c) == NormalState) return;

  dbg("%s() called\n", __func__);
  
  XGrabServer(w->dpy);
  
  c->mapped = True;
  
  if (c->flags & CLIENT_DOCK_EAST || c->flags & CLIENT_DOCK_WEST)
    wm_update_layout(c->wm, c, - c->width);
  else if ( c->flags & CLIENT_DOCK_NORTH )
    {
      wm_update_layout(c->wm, c, - c->height);
    }
  else if ( c->flags & CLIENT_DOCK_SOUTH )
    wm_update_layout(c->wm, c, - c->height);
  
  client_set_state(c, NormalState);

  XMapWindow(w->dpy, c->window);

  stack_move_client_above_type(c, MBCLIENT_TYPE_APP|MBCLIENT_TYPE_DESKTOP);

  XUngrabServer(w->dpy);
}

void
dockbar_client_hide(Client *c)
{
  Wm *w = c->wm;

  if (client_get_state(c) == IconicState) return;
  XGrabServer(w->dpy);
  client_set_state(c, IconicState);
  
  c->mapped = False; 		/* Same reasoning as toolbar_destroy */
  
  if (c->flags & CLIENT_DOCK_EAST || c->flags & CLIENT_DOCK_WEST)
    wm_update_layout(c->wm, c, c->width);
  else if (!(c->flags & CLIENT_DOCK_TITLEBAR))
    wm_update_layout(c->wm, c, c->height);
  
  base_client_hide(c);
  
  XUngrabServer(w->dpy);
}

void
dockbar_client_destroy(Client *c)
{
  Wm *w = c->wm;
  if (c == w->have_titlebar_panel)
    w->have_titlebar_panel = NULL;
  
  c->mapped = False;
  
  if (c->flags & CLIENT_DOCK_EAST || c->flags & CLIENT_DOCK_WEST)
    wm_update_layout(c->wm, c, c->width );
  else if (!(c->flags & CLIENT_DOCK_TITLEBAR))
    wm_update_layout(c->wm, c, c->height);
  
  base_client_destroy(c);
}

