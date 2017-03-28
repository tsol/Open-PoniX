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

#include "desktop_client.h"

Client*
desktop_client_new(Wm *w, Window win)
{
   Client *c = NULL; 

   if (w->flags & DESKTOP_DECOR_FLAG)
     {
       c = main_client_new(w, win);
       c->flags  |= CLIENT_IS_DESKTOP_FLAG;
       w->client_desktop = c;
       return c;
     }

   c = base_client_new(w, win); 
   c->type         = MBCLIENT_TYPE_DESKTOP;
   c->configure    = &desktop_client_configure;
   c->reparent     = &desktop_client_reparent;
   c->move_resize  = &desktop_client_move_resize;
   c->show         = &desktop_client_show;
   c->destroy      = &desktop_client_destroy;

   c->hide         = &main_client_hide;

   if (w->stack_top_app && (w->flags & SINGLE_FLAG))
     main_client_redraw(w->stack_top_app, False);

   w->client_desktop = c;

   return c;
}

void
desktop_client_reparent(Client *c)
{
  c->frame = c->window;
}

void
desktop_client_move_resize(Client *c)
{
   XMoveResizeWindow(c->wm->dpy, c->window, c->x, c->y, c->width, c->height );
}

void
desktop_client_configure(Client *c)
{   
   c->width  = c->wm->dpy_width;
   c->height = c->wm->dpy_height;

   c->x = 0;
   c->y = 0;

}

void
desktop_client_show(Client *c)
{
  Wm *w = c->wm;

  stack_move_top(c);

  /* All clients use the above move to base */
  
  if (!c->mapped)
    {
      XMapSubwindows(w->dpy, c->frame);
      XMapWindow(w->dpy, c->frame);
    }
  
  c->mapped = True;
}

void
desktop_client_unmap(Client *c)
{
   Wm     *w = c->wm;

   dbg("%s called for %s\n", __func__, c->name);

   c->mapped = False;

   if (w->stack_top_app)
     {
       /* Needed to make sure app window task menu button gets updated */
       if (w->stack_top_app == stack_get_below(w->stack_top_app, MBCLIENT_TYPE_APP))
	 main_client_redraw(w->stack_top_app, False);

       wm_activate_client(w->stack_top_app);   
     }

   w->flags &= ~DESKTOP_RAISED_FLAG;
   ewmh_update_desktop_hint(w);
}

void
desktop_client_destroy(Client *c)
{
   Wm     *w = c->wm;

  desktop_client_unmap(c);

  base_client_destroy(c); 

  w->client_desktop = NULL;
}

