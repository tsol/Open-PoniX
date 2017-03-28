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

/* Toolbar windows are small collapsable 'panel' like windows at bottom 
 * of display. They are mainly to hold input methods like software keyboards
 * stroke recognisers etc. 
 */

#include "toolbar_client.h"

#ifdef USE_ALT_INPUT_WIN

static int dialog_init_height;

Client*
toolbar_client_new(Wm *w, Window win)
{
  Window trans_win;
  Client *c = NULL, *trans_client = NULL; 

  c = base_client_new(w, win);

  if (!c) return NULL;

  c->type = MBCLIENT_TYPE_DIALOG; 
   
  c->configure    = &toolbar_client_configure;
  c->reparent     = &toolbar_client_reparent;
  c->hide         = &toolbar_client_hide;
  c->iconize      = &toolbar_client_hide;
  c->show         = &toolbar_client_show;
  c->move_resize  = &toolbar_client_move_resize;
  c->destroy      = &toolbar_client_destroy;

  dialog_init_height = -1;

  XGetTransientForHint(w->dpy, win, &trans_win);

  dbg("%s() checking trans hint\n", __func__);
   
  if (trans_win && (trans_win != win))
    {
      dbg("%s() got trans hint\n", __func__);

      trans_client = wm_find_client(w, trans_win, WINDOW);

      if (trans_client)
	{

	  if (trans_client->type == MBCLIENT_TYPE_DIALOG)
	    {
	      c->flags |= CLIENT_TB_ALT_TRANS_FOR_DIALOG;
	      dbg("%s() is trans for dialog\n", __func__);
	    }
	  else if (trans_client->type & (MBCLIENT_TYPE_APP|MBCLIENT_TYPE_DESKTOP))
	    {
	      dbg("%s() is trans for app\n", __func__);
	      c->flags |= CLIENT_TB_ALT_TRANS_FOR_APP;
	    }
	  else trans_client = NULL;
	}
      
      if (trans_client == NULL)
	{
	  fprintf(stderr, "matchbox: Alternate toolbar window lacks valid transient parent\n");
	  return NULL; 		/* SledgeHammer for now */
	}
    }
   
   return c;
}

int
toolbar_win_offset(Client *c)
{
  return 0;
}

void
toolbar_client_configure(Client *c)
{
  Wm     *w = c->wm;
  Client *main_client = NULL;

  dbg("%s() called\n", __func__);

  if (c->flags & CLIENT_IS_MINIMIZED)
    return;

  dbg("%s() client is not minimised\n", __func__);

  if (c->flags & CLIENT_TB_ALT_TRANS_FOR_APP)
    {
      /* resize the main client were transient for */
	  
      Client *app_client = c->trans;
	  
      if (app_client && (app_client->flags & CLIENT_FULLSCREEN_FLAG))
	{
	  c->x      = 0;
	  c->y      = w->dpy_height - c->height;
	  c->width  = w->dpy_width;
	  return;
	}
    }

  c->y = w->dpy_height - wm_get_offsets_size(w, SOUTH, c, True) - c->height;
  c->x = wm_get_offsets_size(w, WEST,  NULL, False);
  c->width = w->dpy_width
    - wm_get_offsets_size(w, WEST,  NULL, False)
    - wm_get_offsets_size(w, EAST,  NULL, False);

  /*
   * Though not transient for app, there could be a fullscreened
   * app win below us, in which case the input win needs to cover
   * any vertical panels. 
   */
  if ((main_client = wm_get_visible_main_client(w)) != NULL)
    {
      if (main_client->flags & CLIENT_FULLSCREEN_FLAG)
	{
	  c->x      = 0;
	  c->y      = w->dpy_height - c->height;
	  c->width  = w->dpy_width;
	}
    }

  if (c->flags & CLIENT_TB_ALT_TRANS_FOR_DIALOG)
    {
      Client *dialog_client = c->trans;
      
      dbg("%s() client trans for dialog\n", __func__);

      if (dialog_client)
	{
	  /*
	   *  Move transient dialog out of the way of toolbar.  
	   */
	  Bool tmp_mapped = c->mapped;

	  int req_x = dialog_client->x, req_y = dialog_client->y, 
	    req_w = dialog_client->width, req_h = dialog_client->height;
	  
	  if (dialog_init_height < 0)
	    dialog_init_height = dialog_client->height;

	  c->mapped = True; 	/* Hack Hack */
	  c->type = MBCLIENT_TYPE_TOOLBAR; 

	  dbg("%s() checking for available geom\n", __func__);

	  if (!dialog_constrain_geometry(dialog_client, 
					 &req_x, &req_y, 
					 &req_w, &req_h))
	    {

	      dbg("%s() constraining to %ix%i +%i+%i\n", 
		  __func__,
		  req_w, req_h, req_x, req_y);

	      dialog_client->x = req_x; dialog_client->y = req_y; 
	      dialog_client->width = req_w; dialog_client->height = req_h;
	      dialog_client->move_resize(dialog_client);
	      /* Force a redraw to be safe */
	      dialog_client->redraw(dialog_client, False);
	      client_deliver_config(dialog_client);
	    }

	  c->type = MBCLIENT_TYPE_DIALOG; 
	  c->mapped = tmp_mapped;
	  
	}
    }
}

void
toolbar_client_move_resize(Client *c)
{
  Wm *w = c->wm;

  base_client_move_resize(c);

  dbg("%s() called setting size +%i+%i,%ix%i\n", 
      __func__, c->x, c->y, c->width, c->height);

  XResizeWindow(w->dpy, c->window, c->width, c->height);
  XMoveResizeWindow(w->dpy, c->frame, c->x, c->y, c->width, c->height );
}

void
toolbar_client_reparent(Client *c)
{
  Wm *w = c->wm;

  XSetWindowAttributes attr;

  attr.override_redirect = True; 
  attr.background_pixel  = w->grey_col.pixel;
  attr.event_mask        = ChildMask|ButtonPressMask|ExposureMask;
  
  c->frame =
    XCreateWindow(w->dpy, w->root, 0, c->y,
		  w->dpy_width, c->height, 0,
		  CopyFromParent, CopyFromParent, CopyFromParent,
		  CWOverrideRedirect|CWEventMask|CWBackPixel,
		  &attr);
  
  attr.background_pixel = w->grey_col.pixel;
  
  XSetWindowBorderWidth(w->dpy, c->window, 0);
  XAddToSaveSet(w->dpy, c->window);
  XSelectInput(w->dpy, c->window,
	       ButtonPressMask|ColormapChangeMask|PropertyChangeMask);
  
  dbg("%s() reparenting\n", __func__ );
  
  XReparentWindow(w->dpy, c->window, c->frame, 0, 0);
}


void
toolbar_client_show(Client *c)
{
  Wm   *w = c->wm;

  if (!c->mapped)
    {
      XMapSubwindows(w->dpy, c->frame);
      XMapWindow(w->dpy, c->frame);

      if (c->flags & CLIENT_TB_ALT_TRANS_FOR_APP)
	{
	  /* resize the main client were transient for */
	  
	  Client *app_client = c->trans;

	  /* Call this so, map of toolbar hopefully happens before
           * resize preventing potential flash of desktop win. 
	  */
	  XSync(w->dpy, False);
	  
	  if (app_client)
	    {
	      app_client->height -= c->height;
	      app_client->move_resize(app_client);
	      app_client->redraw(app_client, False);
	    }
	}
    }
      
  dialog_client_show(c);

  c->mapped = True;
}

void
toolbar_client_hide(Client *c)
{
  Wm *w = c->wm;
  
  client_set_state(c, WithdrawnState);
  XReparentWindow(w->dpy, c->window, w->root, c->x, c->y); 
  XUnmapWindow(w->dpy, c->window);
  c->destroy(c);
}

void
toolbar_client_destroy(Client *c)
{
  Wm *w = c->wm;

  dbg("%s() called\n", __func__);
   
  c->mapped = False; /* Setting mapped to false will allow the 
                        dialog resizing/repositioning via restack
                        to ignore use  */

  /* resize the main client were transient for */
  if (c->flags & CLIENT_TB_ALT_TRANS_FOR_APP)
    {
      Client *app_client = c->trans;

      if (app_client)
	{
	  /* app_client could have likely dissapeared with the toolbar
	   * but we havn't been told yet - therefore trap. 
           * XXX - there maybe a better way of handling this ?
	  */
	  misc_trap_xerrors(); 

	  app_client->height += c->height;
	  app_client->move_resize(app_client);
	  app_client->redraw(app_client, False);

	  misc_untrap_xerrors();
	}
    }
  else if (c->flags & CLIENT_TB_ALT_TRANS_FOR_DIALOG)
    {
      Client *dialog_client = c->trans;

      /* Reset dialog to old size - *no* repositioning currently */
      if (dialog_client && (dialog_init_height != dialog_client->height))
	{
	  dialog_client->height = dialog_init_height;
	  dialog_client->move_resize(dialog_client);
	  dialog_client->redraw(dialog_client, False);
	  client_deliver_config(dialog_client);
	}
    }

  if (w->focused_client == c)
    w->focused_client = NULL;

  base_client_destroy(c);     
}


void
toolbar_client_redraw(Client *c, Bool use_cache)
{
  ;
}

#endif /* USE_ALT_INPUT_WIN */



