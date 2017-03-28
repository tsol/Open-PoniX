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
#include "main_client.h"

Client*
main_client_new(Wm *w, Window win)
{
   Client *c = base_client_new(w, win); 

   if (!c) return NULL;
   
   c->type = MBCLIENT_TYPE_APP;
   c->reparent     = &main_client_reparent;
   c->redraw       = &main_client_redraw;
   c->button_press = &main_client_button_press;
   c->move_resize  = &main_client_move_resize;
   c->get_coverage = &main_client_get_coverage;
   c->hide         = &main_client_hide;
   c->show         = &main_client_show;
   c->configure    = &main_client_configure;
   c->destroy      = &main_client_destroy;
   c->iconize      = &main_client_iconize;

   main_client_check_for_state_hints(c);

   main_client_check_for_single(c);
   
   return c;
}

void
main_client_check_for_state_hints(Client *c)
{
  dbg("%s() checking for fullscreen hint\n", __func__);

  if (ewmh_state_check(c, c->wm->atoms[WINDOW_STATE_FULLSCREEN]))
    {
      c->flags ^= CLIENT_FULLSCREEN_FLAG;
      dbg("%s() client is fullscreen\n", __func__);
    }
}

void
main_client_check_for_single(Client *c)
{
  Wm     *w = c->wm;

  if (w->flags & SINGLE_FLAG)
    {
      /* There was only main client till this came along */
      w->flags ^= SINGLE_FLAG; /* turn off single flag */
      if (w->stack_top_app)
	main_client_redraw(w->stack_top_app, False); /* update menu button */
    } else if (!w->stack_top_app) /* This must be the only client*/
      c->wm->flags |= SINGLE_FLAG; /* so turn on single flag */      
}

/* Handle the case for showing input methods ( toolbars ) 
 * for fullscreen 
 */
int
main_client_manage_toolbars_for_fullscreen(Client *c, Bool main_client_showing)
{
#ifdef USE_ALT_INPUT_WIN
  Wm     *w = c->wm;
  Client *p  = NULL;

  dbg("%s() called\n", __func__);

  stack_enumerate(w, p)
    {
      if (p->type == MBCLIENT_TYPE_DIALOG)
	{
	  if (p->flags & CLIENT_TB_ALT_TRANS_FOR_APP 
	      && p->trans == c)
	    {
	      toolbar_client_configure(p);
	      toolbar_client_move_resize(p);

	      return p->height;
	    }
	  else if (p->flags & CLIENT_TB_ALT_TRANS_FOR_DIALOG)
	    {
	      dbg("%s() IM trans for dialog found resizing..\n", __func__);

	      toolbar_client_configure(p);
	      toolbar_client_move_resize(p);
	      return 0;
	    }
	}
    }

  return 0;

#else
  Wm     *w = c->wm;
  Client *p  = NULL;
  int     south_panel_size = 0, south_total_size = 0;

  if (main_client_showing 
      && (c->flags & CLIENT_TOOLBARS_MOVED_FOR_FULLSCREEN))
    return 0;

  if (!main_client_showing 
      && !(c->flags & CLIENT_TOOLBARS_MOVED_FOR_FULLSCREEN))
    return 0;

  south_panel_size = wm_get_offsets_size(w, SOUTH, NULL, False); 
  south_total_size = wm_get_offsets_size(w, SOUTH, NULL, True); 

  c->flags ^= CLIENT_TOOLBARS_MOVED_FOR_FULLSCREEN;    

  if (south_total_size > south_panel_size) /* there are toolbars */
    {
      stack_enumerate(w, p)
	{
	  /* move toolbar wins up/down over panels */
	  if (p->type == MBCLIENT_TYPE_TOOLBAR && p->mapped) 
	    {
	      if (main_client_showing)
		{
		  p->y += south_panel_size; 

		  /* cover vertical panels */
		  p->x = toolbar_win_offset(p);
		  p->width = w->dpy_width - toolbar_win_offset(p);
		}
	      else
		{
		  /* uncover any vertical panels */
		  p->x = toolbar_win_offset(p) 
		    + wm_get_offsets_size(w, WEST,  NULL, False);
		  p->width = w->dpy_width - toolbar_win_offset(p)
		    - wm_get_offsets_size(w, WEST,  NULL, False)
		    - wm_get_offsets_size(w, EAST,  NULL, False);


		  p->y -= south_panel_size; 

		}

	      p->move_resize(p);
	      XMapRaised(w->dpy, p->frame);
	    }
	  else if (p->type == MBCLIENT_TYPE_PANEL && main_client_showing)
	    {
	      XLowerWindow(w->dpy, p->frame);
	    }
	}

      return (south_total_size - south_panel_size);
    }

  return 0;
#endif
}

void
main_client_configure(Client *c)
{
  Wm *w = c->wm;
  int frm_size = main_client_title_height(c);
  int offset_south = theme_frame_defined_height_get(c->wm->mbtheme, 
						    FRAME_MAIN_SOUTH);
  int offset_east  = theme_frame_defined_width_get(c->wm->mbtheme, 
						   FRAME_MAIN_EAST );
  int offset_west  = theme_frame_defined_width_get(c->wm->mbtheme, 
						   FRAME_MAIN_WEST );

  int h = wm_get_offsets_size(w, SOUTH, NULL, True);

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) /* Decorations */
    frm_size = offset_south = offset_east = offset_west = 0;

   if ( c->flags & CLIENT_FULLSCREEN_FLAG )
     { 
       c->y = 0;  
       c->x = 0;
       c->width  = w->dpy_width;
       c->height = w->dpy_height - main_client_manage_toolbars_for_fullscreen(c, True);
     }
   else
     {
       c->y = wm_get_offsets_size(c->wm, NORTH, NULL, False) + frm_size;
       c->x = wm_get_offsets_size(c->wm, WEST,  NULL, False) + offset_west;
       c->width  = c->wm->dpy_width - ( offset_east + offset_west ) 
	 - wm_get_offsets_size(c->wm, EAST,  NULL, False)
	 - wm_get_offsets_size(c->wm, WEST,  NULL, False);

#ifdef USE_ALT_INPUT_WIN
       c->height = c->wm->dpy_height - c->y - h - offset_south - main_client_manage_toolbars_for_fullscreen(c, False);
#else
       c->height = c->wm->dpy_height - c->y - h - offset_south;
       main_client_manage_toolbars_for_fullscreen(c, False);
#endif
     }

   dbg("%s() configured as %i*%i+%i+%i, frame size is %i\n", 
       __func__, c->width, c->height, c->x, c->y, frm_size);

}

int
main_client_title_height(Client *c)
{
  if (c == NULL || c->type != MBCLIENT_TYPE_APP)
    return 0;

  if ( (!c->wm->config->use_title)
       || c->flags & CLIENT_FULLSCREEN_FLAG
       || c->flags & CLIENT_TITLE_HIDDEN_FLAG) 
    return 0;

  if ((c->wm->flags & TITLE_HIDDEN_FLAG) && c->type == MBCLIENT_TYPE_APP)
    return TITLE_HIDDEN_SZ;

  return theme_frame_defined_height_get(c->wm->mbtheme, FRAME_MAIN);
}

void
main_client_get_coverage(Client *c, int *x, int *y, int *w, int *h)
{
  int offset_south = theme_frame_defined_height_get(c->wm->mbtheme, 
						    FRAME_MAIN_SOUTH);
  int offset_east  = theme_frame_defined_width_get(c->wm->mbtheme, 
						   FRAME_MAIN_EAST );
  int offset_west  = theme_frame_defined_width_get(c->wm->mbtheme, 
						   FRAME_MAIN_WEST );

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    offset_south = offset_east = offset_west = 0; 

   *x = c->x - offset_west; 
   *y = c->y - main_client_title_height(c);
   *w = c->width + offset_east + offset_west;
   *h = c->height + main_client_title_height(c) + offset_south;

   dbg("%s() +%i+%i, %ix%i\n", __func__, *x, *y, *w, *h);
}

void
main_client_reparent(Client *c)
{
  Wm *w = c->wm;
  XSetWindowAttributes attr;
  int   frame_north_height;

  int offset_north = main_client_title_height(c);
  int offset_south = theme_frame_defined_height_get(w->mbtheme, 
						    FRAME_MAIN_SOUTH);
  int offset_east  = theme_frame_defined_width_get(w->mbtheme, 
						   FRAME_MAIN_EAST );
  int offset_west  = theme_frame_defined_width_get(w->mbtheme, 
						     FRAME_MAIN_WEST );
  attr.override_redirect = True;
  attr.background_pixel  = w->grey_col.pixel; /* BlackPixel(w->dpy, w->screen); */
  attr.event_mask         = ChildMask|ButtonMask|ExposureMask;

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    offset_south = offset_east = offset_west = 0; 

  c->frame =
    XCreateWindow(w->dpy, w->root, 
		  c->x - offset_west, 
		  c->y - offset_north,
		  c->width + offset_east + offset_west, 
		  c->height + offset_north + offset_south, 
		  0,
		  CopyFromParent, CopyFromParent, CopyFromParent,
		  CWOverrideRedirect|CWEventMask|CWBackPixel,
		  &attr);

  dbg("%s frame created : %i*%i+%i+%i\n",
      __func__, c->width, c->height + offset_north, c->x, c->y);

  frame_north_height = offset_north;

  if (!c->wm->config->use_title && c->flags & CLIENT_FULLSCREEN_FLAG) 
  {
    /* make sure the top frame is not 0 pixels high if fullscreen
     * else toggling will break. 
    */
    frame_north_height = theme_frame_defined_height_get(c->wm->mbtheme, 
							FRAME_MAIN);
  }

  if (!(c->flags & CLIENT_TITLE_HIDDEN_FLAG))
    client_decor_frames_init(c, 
			     offset_west, 
			     offset_east, 
			     ( c->flags & CLIENT_FULLSCREEN_FLAG) ?
			     : offset_north, 
			     offset_south);

  XClearWindow(w->dpy, c->frame);

  XSetWindowBorderWidth(w->dpy, c->window, 0);
  XAddToSaveSet(w->dpy, c->window); 
  XSelectInput(w->dpy, c->window, ColormapChangeMask|PropertyChangeMask);
  XReparentWindow(w->dpy, c->window, c->frame, offset_west, offset_north);
}


void
main_client_move_resize(Client *c)
{
  Wm *w = c->wm;

  int offset_south = theme_frame_defined_height_get(w->mbtheme, 
						    FRAME_MAIN_SOUTH);
  int offset_east  = theme_frame_defined_width_get(w->mbtheme, 
						   FRAME_MAIN_EAST );
  int offset_west  = theme_frame_defined_width_get(w->mbtheme, 
						   FRAME_MAIN_WEST );

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    offset_south = offset_east = offset_west = 0; 

  base_client_move_resize(c);

  XMoveResizeWindow(w->dpy, c->window, 
		    offset_west, main_client_title_height(c), 
		    c->width, c->height);

  XMoveResizeWindow(w->dpy, c->frame, 
		    c->x - offset_west,
		    c->y - main_client_title_height(c), 
		    c->width + ( offset_east + offset_west),
		    c->height + main_client_title_height(c) + offset_south);

  client_decor_frames_move_resize(c, 
				  offset_west, offset_east, 
				  main_client_title_height(c), offset_south);


/*
 * Disabled _NET_WM_SYNC code ( Alternate to above XMove*'s )
 * Is unfinished and initial testing did not seem to improve things.
 * Maybe re-try again at a later date.
 *
 * if (!c->ewmh_sync_is_waiting)
 *  {
 *    XMoveResizeWindow(w->dpy, c->window, 
 *			offset_west, main_client_title_height(c), 
 *			c->width, c->height);
 *   }
 * 
 * if (ewmh_sync_client_move_resize(c))
 *   {
 *     dbg("%s() not resizing frame yet\n", __func__);
 *     return; 
 *   }
 *
 * XMoveWindow(w->dpy, c->window, offset_west, main_client_title_height(c));
 * 
 * XResizeWindow(w->dpy, c->title_frame, 
 *		c->width + (offset_east + offset_west),
 *		c->height + main_client_title_height(c) + offset_south);
 * 
 * XMoveResizeWindow(w->dpy, c->frame, 
 *		    c->x - offset_west,
 *		    c->y - main_client_title_height(c), 
 *		    c->width + ( offset_east + offset_west),
 *		    c->height + main_client_title_height(c) + offset_south);
 *
 *  dbg("%s() resizing frame\n", __func__);
 *
 * c->ewmh_sync_is_waiting = False;
 */

}


void
main_client_toggle_fullscreen(Client *c)
{
  c->flags ^= CLIENT_FULLSCREEN_FLAG;

  ewmh_state_set(c); /* Let win know it fullscreen state has changed, it
		        could be waiting on this to adjust ui */

  main_client_configure(c);
  main_client_move_resize(c);

  if (!(c->flags & CLIENT_FULLSCREEN_FLAG))
    {
      /* Client was fullscreen - to be safe we redraw decoration buttons */
      client_buttons_delete_all(c);    
      c->redraw(c, False);
    }

  wm_activate_client(c); /* Reactivate, stacking order slightly different  */

  /* No need to ping here anymore
   * if (c->wm->config->ping_aggressive)
   *   ewmh_ping_client_start (c);
   */ 
}


/* redraws the frame */
void
main_client_redraw(Client *c, Bool use_cache)
{
  Wm  *w = c->wm;
  Bool is_shaped = False;
  int  width = 0, height = 0;
  int  offset_south, offset_east, offset_west;

  dbg("%s() called on %s\n", __func__, c->name);

   if (!w->config->use_title || c->flags & CLIENT_TITLE_HIDDEN_FLAG)
     return;
   
   if (w->flags & TITLE_HIDDEN_FLAG)
   {
     XUnmapWindow(w->dpy, client_title_frame(c));
     return;
   }

   if (use_cache && c->have_set_bg)  return ;

   offset_south = theme_frame_defined_height_get(w->mbtheme, 
						 FRAME_MAIN_SOUTH);
   offset_east  = theme_frame_defined_width_get(w->mbtheme, 
						FRAME_MAIN_EAST );
   offset_west  = theme_frame_defined_width_get(w->mbtheme, 
						FRAME_MAIN_WEST );
 
   width = c->width + offset_east + offset_west;
   height = theme_frame_defined_height_get(w->mbtheme, FRAME_MAIN);
   
   is_shaped = theme_frame_wants_shaped_window( w->mbtheme, FRAME_MAIN);

   dbg("%s() cache failed, actual redraw on %s\n", __func__, c->name);

   if (is_shaped) 
     client_init_backing_mask(c, c->width + offset_east + offset_west, 
			      c->height, height , offset_south,
			      width - offset_east, offset_west);

   dbg("%s() calling theme_frame_paint()\n", __func__); 

   theme_frame_paint(w->mbtheme, c, FRAME_MAIN, width, height); 

   theme_frame_paint(w->mbtheme, c, FRAME_MAIN_WEST, offset_west, c->height); 
  
   theme_frame_paint(w->mbtheme, c, FRAME_MAIN_EAST, offset_east, c->height); 

   theme_frame_paint(w->mbtheme, c, FRAME_MAIN_SOUTH, 
		     c->width + offset_east + offset_west, offset_south); 

   if (!(c->flags & CLIENT_IS_DESKTOP_FLAG))
     theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_CLOSE, 
			      INACTIVE, FRAME_MAIN, width, height);

   theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_HIDE, 
			    INACTIVE, FRAME_MAIN, width, height);

   theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_MIN, 
			    INACTIVE, FRAME_MAIN, width, height);

   if (!(w->flags & SINGLE_FLAG))
   {
      dbg("%s() painting next / prev buttons\n", __func__);
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_MENU, 
			       INACTIVE, FRAME_MAIN, width, height);
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_NEXT, 
			       INACTIVE, FRAME_MAIN, width, height);
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_PREV, 
			       INACTIVE, FRAME_MAIN, width, height);
   } else {
     client_button_remove(c, BUTTON_ACTION_NEXT);
     client_button_remove(c, BUTTON_ACTION_PREV);

     if (!(w->flags & DESKTOP_DECOR_FLAG)
	   && wm_get_desktop(c->wm)) /* Paint the dropdown for the desktop */
       {
	 dbg("%s() have desktop\n", __func__);
	 theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_MENU, 
				  INACTIVE, FRAME_MAIN, width, height);

	 theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_DESKTOP, 
				  INACTIVE, FRAME_MAIN, width, height);
       }
     else 
       {
	 dbg("%s() removing menu button\n", __func__ );
	 client_button_remove(c, BUTTON_ACTION_MENU);
       }
   }

   if (c->flags & CLIENT_ACCEPT_BUTTON_FLAG)
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_ACCEPT, 
			       INACTIVE, FRAME_MAIN, width, height);

   if (c->flags & CLIENT_HELP_BUTTON_FLAG)
     {
       dbg("%s() painting help button\n", __func__);
       theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_HELP, 
				INACTIVE, FRAME_MAIN, width, height);
     }

   if (c->flags & CLIENT_CUSTOM_BUTTON_FLAG)
     {
       dbg("%s() painting help button\n", __func__);
       theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_CUSTOM, 
				INACTIVE, FRAME_MAIN, width, height);
     }


  if (is_shaped)   /* XXX do we really need titleframe here ? */
    {
      XRectangle rects[1];

      rects[0].x = 0;
      rects[0].y = height;
      rects[0].width = width;
      rects[0].height = c->height;

      XShapeCombineRectangles ( w->dpy, c->frame, 
				ShapeBounding,
				0, 0, rects, 1, ShapeSet, 0 );

      XShapeCombineMask( c->wm->dpy, c->frames_decor[NORTH], 
			 ShapeBounding, 0, 0, 
			 c->backing_masks[MSK_NORTH], ShapeSet);

      XShapeCombineMask( c->wm->dpy, c->frames_decor[SOUTH], 
			 ShapeBounding, 0, 0,
			 c->backing_masks[MSK_SOUTH], ShapeSet);


      XShapeCombineShape ( c->wm->dpy, 
			   c->frame,
			   ShapeBounding, 0, 0, 
			   c->frames_decor[NORTH],
			   ShapeBounding, ShapeUnion);

      XShapeCombineShape ( c->wm->dpy, 
			   c->frame,
			   ShapeBounding, 0, c->height + height, 
			   c->frames_decor[SOUTH],
			   ShapeBounding, ShapeUnion);
    }

#if 0

#ifdef STANDALONE
   XSetWindowBackgroundPixmap(w->dpy, c->title_frame, c->backing);
#else
   XSetWindowBackgroundPixmap(w->dpy, c->title_frame, 
			      mb_drawable_pixmap(c->backing));
#endif

   XClearWindow(w->dpy, c->title_frame);

#ifdef STANDALONE
   XFreePixmap(w->dpy, c->backing);
   c->backing = None;
#else
   mb_drawable_unref(c->backing);
   c->backing = NULL;
#endif

#endif /* if 0 */

   c->have_set_bg = True;
}


void main_client_button_press(Client *c, XButtonEvent *e)
{
  Wm *w = c->wm;

  int     ch = 0, offset_east = 0, offset_west = 0, total_w = 0;
  Client *p  = NULL;

   if (!w->config->use_title) return;

   if (w->flags & TITLE_HIDDEN_FLAG)
   {
      main_client_toggle_title_bar(c);
      /* XXX What is this doing ?
      XMapWindow(w->dpy, c->title_frame);
      XMapSubwindows(w->dpy, c->title_frame);
      */
      return;
   }

   if (w->config->super_modal)
     {
       stack_enumerate(w, p)
	 {
	   if (p->trans == c && (p->flags & CLIENT_IS_MODAL_FLAG))
	     {
	       MBList *button_item = client_get_button_list_item_from_event(c, e);
	       /* In the precense of a modal transient dialog ignore 
		* certain buttons. 
		*
		*/
	       if ((button_item && button_item->id == BUTTON_ACTION_CUSTOM)
		   || (button_item && button_item->id == BUTTON_ACTION_MIN)
		   || (button_item && button_item->id == BUTTON_ACTION_CLOSE))
		 {
		   if (button_item->id == BUTTON_ACTION_CLOSE)
		     {
		       /* initiate pinging the app anyway for close button */
		       if (c->has_ping_protocol && c->pings_pending == -1) 
			 {
			   c->pings_pending = 0;
			   w->n_active_ping_clients++;
			 }
		     }
		   return;
		 }
	     }
	 }
     }

   offset_east  = theme_frame_defined_width_get(w->mbtheme, 
						FRAME_MAIN_EAST );
   offset_west  = theme_frame_defined_width_get(w->mbtheme, 
						FRAME_MAIN_WEST );


   ch = theme_frame_defined_height_get(w->mbtheme, FRAME_MAIN);

   total_w = c->width + offset_east + offset_west;

   switch (client_button_do_ops(c, e, FRAME_MAIN, total_w, ch))
     {
      case BUTTON_ACTION_DESKTOP:
	 wm_toggle_desktop(w);
	 break;
      case BUTTON_ACTION_CLOSE:
	 client_deliver_delete(c);
	 break;
      case BUTTON_ACTION_NEXT:
	wm_activate_client(stack_cycle_backward(w, MBCLIENT_TYPE_APP));
	 break;
      case BUTTON_ACTION_PREV:
	wm_activate_client(stack_cycle_forward(w, MBCLIENT_TYPE_APP));
	 break;
      case BUTTON_ACTION_MENU:
	 select_client_new(w);
	 break;
      case BUTTON_ACTION_HIDE:
	 main_client_toggle_title_bar(c);
	 break;
      case BUTTON_ACTION_MIN:
	 main_client_iconize(c);
	 break;
      case BUTTON_ACTION_HELP:
	client_deliver_wm_protocol(c, w->atoms[_NET_WM_CONTEXT_HELP]);
	 break;
      case BUTTON_ACTION_ACCEPT:
	client_deliver_wm_protocol(c, w->atoms[_NET_WM_CONTEXT_ACCEPT]);
	 break;
      case BUTTON_ACTION_CUSTOM:
	client_deliver_wm_protocol(c, w->atoms[_NET_WM_CONTEXT_CUSTOM]);
	 break;
      case -1: 		 
	/* Cancelled  */
	 break;
      case 0:
	 /* Not on button */
	 break;
   }
}

void
main_client_toggle_title_bar(Client *c)
{
  Wm *w = c->wm;

  Client *p = NULL;
  int prev_height = main_client_title_height(c);
  int y_offset = wm_get_offsets_size(c->wm, NORTH, NULL, False);

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    return;
  
  w->flags ^= TITLE_HIDDEN_FLAG;
  
  dbg("%s() called\n", __func__);
  
  XGrabServer(w->dpy);
  
  theme_img_cache_clear( w->mbtheme,  FRAME_MAIN );
  
  stack_enumerate(c->wm, p)
    if (p->type == MBCLIENT_TYPE_APP)
      {
	if (w->flags & TITLE_HIDDEN_FLAG)
	  {  /* hide */
	    p->height += (prev_height - TITLE_HIDDEN_SZ );
	    p->y = y_offset + TITLE_HIDDEN_SZ;
	    
	    if (w->have_titlebar_panel 
		&& mbtheme_has_titlebar_panel(w->mbtheme))
	      {
		w->have_titlebar_panel->ignore_unmap++;
		XUnmapWindow(w->dpy, w->have_titlebar_panel->frame);
	      }
	    
	  } else {
	    /* show */
	    p->y = main_client_title_height(p) + y_offset;
	    p->height -= ( main_client_title_height(p) - TITLE_HIDDEN_SZ );
	    XMapWindow(w->dpy, client_title_frame(p)); /* prev will have unmapped */
	    
	    if (w->have_titlebar_panel
		&& mbtheme_has_titlebar_panel(w->mbtheme))
	      XMapRaised(w->dpy, w->have_titlebar_panel->frame);
	    
	  }
	p->move_resize(p);
	p->redraw(p, False);
      }
  
  XUngrabServer(w->dpy);
}

/* This is called when a main client is not visible anymore - 
   i.e. another one is activated */
void
main_client_hide(Client *c)
{
  Wm *w = c->wm;

  /* If we dont have focus atm, and one of our dialogs likely does,
     then remember it for next time we come active
  */
  if (w->focused_client != c 
      && w->focused_client       
      && w->focused_client->type == MBCLIENT_TYPE_DIALOG
      && w->focused_client->trans != NULL)
    {
      c->next_focused_client = w->focused_client;
    }
  else 
    {
      c->next_focused_client = NULL;

      /* If were focused, unset focus for something better to be found */
      if (w->focused_client == c)
	w->focused_client = NULL;
    }
}

void
main_client_iconize(Client *c)
{
  Wm *w = c->wm;
  Client *p = NULL;

  dbg("%s() called on %s\n", __func__, c->name);

  client_set_state(c, IconicState);
  c->flags |= CLIENT_IS_MINIMIZED;

  /* Make sure any transients get iconized too */
  stack_enumerate(w, p)
    if (p->trans == c)
      p->iconize(p);

  main_client_unmap(c);
}

void
main_client_show(Client *c)
{
  Wm     *w = c->wm;

  Client *visible_app_client = NULL;

  dbg("%s() called on %s\n", __func__, c->name);
   
  if (c->flags & CLIENT_NO_FOCUS_ON_MAP)
    visible_app_client = wm_get_visible_main_client(w);

   if (w->flags & DESKTOP_RAISED_FLAG) 
     {
       c->flags |= CLIENT_NEW_FOR_DESKTOP;
     }
   else
     {
       /* Only reset this flag if were not the main client. 
        * - ie we've paged to another app and come back. 
        * Avoid problem of open app *and* the dialog from desktop, close 
        * app then does not go back to desktop. 
       */
       if (wm_get_visible_main_client(w) != c)
	 c->flags &= ~CLIENT_NEW_FOR_DESKTOP;
     }

   /* Move this client and any transients to the very top of the stack.
      wm_activate_client() ( call it sync_display ? ) will then take
      care of painels etc as it can use active client as a 'watermark' 
   */
   stack_move_top(c);

   stack_dump(w);

   if (!c->mapped)
     {
       if (c->flags & CLIENT_IS_MINIMIZED)
	 {
	   Client *p = NULL;

	   /* Reset state from Iconized */
	   client_set_state(c, NormalState);
	   c->flags &= ~CLIENT_IS_MINIMIZED;

	   /* Make sure any dialogs are shown too */
	   stack_enumerate(w, p)
	     if (p->trans == c)
	       {
		 p->show(p);
		 /* To handle delay mapping.. */
		 XMapSubwindows(w->dpy, p->frame);
		 XMapWindow(w->dpy, p->frame);
	       }
	 }

       if (c->flags & CLIENT_NO_FOCUS_ON_MAP)
	 {
	   if (visible_app_client)
	     {
	       stack_move_above_client (c, visible_app_client->below);
	       c->flags |= CLIENT_DELAY_MAPPING;
	     }
	   c->flags &= ~CLIENT_NO_FOCUS_ON_MAP;
	 }

       if (!(c->flags & CLIENT_DELAY_MAPPING))
	 {
	   XMapSubwindows(w->dpy, c->frame);
	   XMapWindow(w->dpy, c->frame);
	 }
     }

   c->mapped = True;
}

void
main_client_unmap(Client *c)
{
   Wm     *w = c->wm;
   Client *next_client = NULL; 

   dbg("%s called for %s\n", __func__, c->name);

   if ( c->flags & CLIENT_FULLSCREEN_FLAG )
     main_client_manage_toolbars_for_fullscreen(c, False);

   /* Are we at the top of the stack ? */
   if (c == w->stack_top_app)
     {
       next_client = stack_get_below(c, MBCLIENT_TYPE_APP);
       
       dbg("%s() at stack top\n", __func__ );

       /* Is this the only main client left? */
       if(next_client == c) 
	 {
	   dbg("%s() only client left\n", __func__ );

	   if (w->flags & SINGLE_FLAG)
	     w->flags ^= SINGLE_FLAG; /* single flag off ( for menu button ) */
	   
	   /* is there a desktop ? */
	   next_client = wm_get_desktop(w);
	 }
       else
	 {
	   /* There are more main clients left, but we may have been 
            * opened from the desktop and it therefor makes sense to
            * go back there. 
	    */
	   if (c->flags & CLIENT_NEW_FOR_DESKTOP)
	     {
	       /* Make sure we set stack_top_app so desktop 
                * toggling still works.
                */
	       w->stack_top_app = next_client; 
	       next_client = wm_get_desktop(w);
	     }
	 }

       /* if we havn't set stack_top_app to something else let 
	* wm_activate_client(next_client) below update this */
       if (c == w->stack_top_app)
	 w->stack_top_app = NULL;
     }

   c->mapped = False;

   if (next_client /* only 1 main_client left ? */
       && next_client->type == MBCLIENT_TYPE_APP
       && (next_client == stack_get_below(next_client, MBCLIENT_TYPE_APP)))
     {
       dbg("%s() turning on single flag\n", __func__);
       w->flags |= SINGLE_FLAG; /* turn on single flag for menu button */
       main_client_redraw(next_client, False);
     }

   XUnmapWindow(w->dpy, c->frame); 

   if (c == w->focused_client)
     w->focused_client = NULL;

   if (next_client)
     wm_activate_client(next_client);   
}

void
main_client_destroy(Client *c)
{
   dbg("%s called for %s\n", __func__, c->name);
  
   main_client_unmap(c);

   base_client_destroy(c); 
}




