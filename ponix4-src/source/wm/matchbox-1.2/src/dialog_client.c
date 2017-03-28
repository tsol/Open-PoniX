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

#include "dialog_client.h"

static void dialog_client_check_for_state_hints(Client *c);
static void dialog_client_drag(Client *c);
static void _get_mouse_position(Wm *w, int *x, int *y);

Client*
dialog_client_new(Wm *w, Window win, Client *trans)
{

   Client *c = base_client_new(w, win); 

   if (!c) return NULL;

   c->type = MBCLIENT_TYPE_DIALOG;
   
   c->reparent     = &dialog_client_reparent;
   c->move_resize  = &dialog_client_move_resize;
   c->iconize      = &dialog_client_iconize;   
   c->configure    = &dialog_client_configure;
   c->button_press = &dialog_client_button_press;
   c->redraw       = &dialog_client_redraw;
   c->show         = &dialog_client_show;
   c->destroy      = &dialog_client_destroy;
   c->get_coverage = &dialog_client_get_coverage;

   dialog_client_check_for_state_hints(c);

   c->trans = trans;

   return c;
}

static void
dialog_client_get_offsets(Client *c, int *e, int *s, int *w)
{
  /* no decor dialogs */
  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    {
      *s = 0; *e = 0; *w = 0;
      return;
    }

  if (c->flags & CLIENT_BORDERS_ONLY_FLAG
      && theme_has_borders_only_decor(c->wm->mbtheme))
    {
      *s = theme_frame_defined_height_get(c->wm->mbtheme, 
					  FRAME_DIALOG_NT_SOUTH);
      *e = theme_frame_defined_width_get(c->wm->mbtheme, 
					 FRAME_DIALOG_NT_EAST );
      *w = theme_frame_defined_width_get(c->wm->mbtheme, 
					 FRAME_DIALOG_NT_WEST );
      return;
    }

  if (c->flags & CLIENT_HAS_URGENCY_FLAG
      && theme_has_message_decor(c->wm->mbtheme))
    {
      *s = theme_frame_defined_height_get(c->wm->mbtheme, 
				       FRAME_MSG_SOUTH);
      *e = theme_frame_defined_width_get(c->wm->mbtheme, 
					 FRAME_MSG_EAST );
      *w = theme_frame_defined_width_get(c->wm->mbtheme, 
					 FRAME_MSG_WEST );
      return;
    }

   *s = theme_frame_defined_height_get(c->wm->mbtheme, 
				       FRAME_DIALOG_SOUTH);
   *e = theme_frame_defined_width_get(c->wm->mbtheme, 
				      FRAME_DIALOG_EAST );
   *w = theme_frame_defined_width_get(c->wm->mbtheme, 
				      FRAME_DIALOG_WEST );
}

static void
dialog_client_check_for_state_hints(Client *c)
{
  if (ewmh_state_check(c, c->wm->atoms[WINDOW_STATE_MODAL]))
    {
      Client *damaged_client;
      dbg("%s() got modal hint, setting flag\n", __func__);

      c->flags ^= CLIENT_IS_MODAL_FLAG;

      c->wm->n_modals_present++;

      /* Call comp_engine_client_show to add damage to main window
       * so it gets fully lowlighted ok.
       */
      if ((damaged_client = wm_get_visible_main_client(c->wm)) != NULL)
        {
          comp_engine_client_show(c->wm, damaged_client);
        }
    }

  if (ewmh_state_check(c, c->wm->atoms[WINDOW_STATE_ABOVE]))
    c->flags |= CLIENT_HAS_ABOVE_STATE;

}

void
dialog_client_get_coverage(Client *c, int *x, int *y, int *w, int *h)
{
   int frm_size = dialog_client_title_height(c);
   int east,south,west;

   dialog_client_get_offsets(c, &east, &south, &west);

   *x = c->x - west;
   *y = c->y - frm_size;
   *w = c->width + east + west;
   *h = c->height + frm_size + south;
}

void
dialog_client_move_resize(Client *c)
{
  Wm *w = c->wm;
  int frm_size     = dialog_client_title_height(c);
  int offset_south = 0, offset_west = 0, offset_east = 0;

  dialog_client_get_offsets(c, &offset_east, &offset_south, &offset_west);

  base_client_move_resize(c);

  if (c->window != c->frame) 	/* Non decorated */
    XMoveResizeWindow(w->dpy, c->window, 
		      offset_west, frm_size,
		      c->width, c->height);

#ifndef USE_COMPOSITE
   if (w->config->dialog_shade && (c->flags & CLIENT_IS_MODAL_FLAG))
     {
       XMoveWindow(w->dpy, 
		   c->window, 
		   c->x, 
		   c->y);
     }
   else
#endif
     {
       XMoveResizeWindow(w->dpy, 
			 c->frame, 
			 c->x - offset_west, 
			 c->y - frm_size, 
			 c->width + offset_west + offset_east,
			 c->height + frm_size + offset_south
			 );
     }

   if (!(c->flags & CLIENT_TITLE_HIDDEN_FLAG))
     client_decor_frames_move_resize(c, offset_west, offset_east, 
				     frm_size, offset_south);

   if (c->win_modal_blocker) 
     {
       XMoveResizeWindow(w->dpy, c->win_modal_blocker, 
			 0, 0, w->dpy_width, w->dpy_height);
     }

}

int
dialog_client_title_height(Client *c)
{
  Wm *w = c->wm;

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    return 0;
  
  if (c->flags & CLIENT_HAS_URGENCY_FLAG 
      && theme_has_message_decor(w->mbtheme))
    {
      return theme_frame_defined_height_get(c->wm->mbtheme, FRAME_MSG);
    }
  
  if (c->flags & CLIENT_BORDERS_ONLY_FLAG)
    {
      if (theme_has_borders_only_decor (c->wm->mbtheme))
	return theme_frame_defined_height_get(c->wm->mbtheme, FRAME_DIALOG_NT_NORTH);
      else if (theme_has_frame_type_defined(c->wm->mbtheme, FRAME_DIALOG_NORTH))
	return theme_frame_defined_height_get(c->wm->mbtheme, FRAME_DIALOG_NORTH);
    }  

  return theme_frame_defined_height_get(c->wm->mbtheme, FRAME_DIALOG);
}


void
dialog_client_show(Client *c)
{
  Wm *w = c->wm;
  Client *highest_client = NULL;
  MBList *transient_list = NULL, *list_item = NULL;

  dbg("%s() called for %s\n", __func__, c->name);

  if (!c->mapped)
    {
      if (c->flags & CLIENT_IS_MINIMIZED)
	{
	  Client *p = NULL;

	  client_set_state(c, NormalState);
	  c->flags &= ~CLIENT_IS_MINIMIZED;

	  /* Make sure any transients are un minimized too */
	  stack_enumerate(w, p)
	    if (p->trans == c)
	      {
		p->show(p);
		XMapSubwindows(w->dpy, p->frame);
		XMapWindow(w->dpy, p->frame);
	      }

	  if (c->flags & CLIENT_IS_MODAL_FLAG)
	    c->wm->n_modals_present++;
	}

      if (c->win_modal_blocker)
	{
	  XMapWindow(w->dpy, c->win_modal_blocker);
	  dbg("%s() blocker win mapped for '%s'\n", __func__, c->name);
	}

      /* Set flag below to delay actual mapping of window 
       * till *after* we've synced the stack. This hacky  
       * solution avoids problem of dialogs mapping for
       * clients below and causing flicker.
       *
       * Note could still cause issues with transients and remmapping
       * them if minimised.. 
      */
      c->flags |= CLIENT_DELAY_MAPPING;
    }


  /* 
   *  We just need to get the order right in respect 
   *  to other dialogs transient to the same app or 
   *  transient to root.
   *  This order *should* be kept the same by other 
   *  stack operations ( eg wm_activate client ).
   */

  if (c->trans)
    {
      /* Were transient for something 
       * - so recursives find the highest transient for this app
       *   ignoring current client.
       * - raise ourselves above 
       */
      Client *lowest_trans = c->trans;
      int urgent_flag = 0; /* (c->flags & CLIENT_HAS_URGENCY_FLAG) ?
			      CLIENT_HAS_URGENCY_FLAG : 0; */

      while (lowest_trans->trans != NULL) 
	lowest_trans = lowest_trans->trans;

      highest_client = client_get_highest_transient(lowest_trans, 
						    urgent_flag,
						    c);

      if (c->mapped && highest_client == c)
	{
	  /* if were already at the top, logic below will actually
	   *  move us below the transient.
	   */
	  dbg("%s() %s already highest and mapped .. leaving\n", 
	      __func__, c->name);
	}
      else
	{
	  if (highest_client == NULL || highest_client == c)
	    {
	      dbg("%s() raising %s above c->trans: %s\n", __func__, 
		  c->name, c->trans->name);
	      stack_move_above_client(c, c->trans);
	    }
	  else
	    {
	      dbg("%s() raising %s above highest_client: %s\n", 
		  __func__, c->name, highest_client->name);
	      stack_move_above_client(c, highest_client);
	    }
	}

      if (c->trans->type != MBCLIENT_TYPE_DIALOG
	  && lowest_trans->win_group 
	  && (lowest_trans->type == MBCLIENT_TYPE_DESKTOP
	      || lowest_trans->type == MBCLIENT_TYPE_APP))
	{
	  Client *visible;

	  visible = wm_get_visible_main_client(w);

	  if (visible
	      && visible != lowest_trans 
	      && visible->win_group == lowest_trans->win_group)
	    stack_move_above_client(c, visible);
	}
    }
  else
    stack_move_top(c);


  /* Now move any transients for us above us */

  client_get_transient_list(w, &transient_list, c);
 
  highest_client = c;

  list_enumerate(transient_list, list_item)
    {
      stack_move_above_client((Client *)list_item->data, highest_client);
      highest_client = (Client *)list_item->data;
    }

  list_destroy(&transient_list);

  /* Insurance below */

  if (wm_get_visible_main_client(w))
    {
      stack_move_transients_to_top(w, wm_get_visible_main_client(w), 
				   CLIENT_HAS_ABOVE_STATE);
    }

  stack_move_transients_to_top(w, NULL, CLIENT_HAS_ABOVE_STATE);

  stack_move_transients_to_top(w, NULL, CLIENT_HAS_URGENCY_FLAG);

  c->mapped = True;
}

void
dialog_client_reparent(Client *c)
{
  Wm *w = c->wm;
  XSetWindowAttributes attr;
  unsigned long        attr_mask;  

  int offset_north = dialog_client_title_height(c);
  int offset_south = 0, offset_west = 0, offset_east = 0;
  
  dialog_client_get_offsets(c, &offset_east, &offset_south, &offset_west);
  
  attr.override_redirect = True; 
  attr.background_pixel  = w->grey_col.pixel;
  attr.border_pixel      = 0;
  attr.event_mask        = ChildMask|ButtonPressMask|ExposureMask;

  attr_mask              = CWOverrideRedirect|CWEventMask
                           |CWBackPixel|CWBorderPixel ;

#ifdef USE_COMPOSITE
 /* Needed for argb wins, XXX need to figure this out .. */  
  attr.colormap          = c->cmap;

  if (c->cmap && !w->comp_engine_disabled) 
    attr_mask = CWOverrideRedirect|CWEventMask
                |CWBackPixel|CWBorderPixel|CWColormap ;
#endif

  dbg("%s() want lowlight : wm:%i , client:%li\n", __func__,
      c->wm->config->dialog_shade, (c->flags & CLIENT_IS_MODAL_FLAG));
#ifndef USE_COMPOSITE
  if (c->wm->config->dialog_shade && (c->flags & CLIENT_IS_MODAL_FLAG))
    {
      dbg("%s() LOWLIGHTING\n", __func__);
      wm_lowlight(w, c);
    }
  else
#endif
     {
       if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) 
	 {
	   c->frame = c->window;
	 }
       else c->frame = XCreateWindow(w->dpy, 
				     w->root, 
				     0, 0,
				     c->width + offset_east + offset_west, 
				     c->height + offset_north + offset_south, 
				     0,
#ifdef USE_COMPOSITE
				     c->is_argb32 ? 32  : CopyFromParent,
				     InputOutput,  
				     c->is_argb32 ? c->visual : CopyFromParent,
#else
				     CopyFromParent,
				     CopyFromParent, 
				     CopyFromParent,
#endif
				     attr_mask,
				     &attr);
     }

   if (!(c->flags & CLIENT_TITLE_HIDDEN_FLAG))
     {

       client_decor_frames_init(c, 
				offset_west, offset_east, 
				offset_north, offset_south);

     }

   if (c->flags & CLIENT_IS_MODAL_FLAG
       && w->config->super_modal
       && c->trans == NULL )	/* modal for device. XXX check recursive ? */
     {
       /* Create an InputOnly fullscreen window to aid in making 
	* modal dialogs *really* modal to the whole display by
	* block button events. 
       */
       c->win_modal_blocker = XCreateWindow(w->dpy, 
					    w->root, 
					    0, 0,
					    w->dpy_width,
					    w->dpy_height,
					    0,
					    CopyFromParent,
					    InputOnly,  
					    CopyFromParent,
					    CWOverrideRedirect|CWEventMask,
					    &attr);

       /* Map is show() or gets stacked above dialog */
       /* XMapWindow(w->dpy, c->win_modal_blocker); */

       dbg("%s() created and mapped blocker win for '%s'\n", __func__, c->name);
       
       w->n_modal_blocker_wins++;
     }

   XClearWindow(w->dpy, c->frame);
   
   XSetWindowBorderWidth(w->dpy, c->window, 0);

   XAddToSaveSet(w->dpy, c->window);

   XSelectInput(w->dpy, c->window,
		ColormapChangeMask|PropertyChangeMask);

   if (c->frame != c->window)
     XReparentWindow(w->dpy, c->window, c->frame, 
		     offset_west, offset_north);


}

/*  Padding between dialog borders and area available */
#define DIALOG_PADDING 0 

/*
 *  dialog_get_available_area()
 *  Get the 'safe' area ( eg no panels / input windows ) covered. 
 */
void
dialog_get_available_area(Client *c,
			  int    *x,
			  int    *y,
			  int    *width,
			  int    *height)
{
  Wm *w = c->wm;

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG 
      || c->flags & CLIENT_HAS_URGENCY_FLAG)
    {
      /* Decorationless dialogs can position themselves anywhere */
      *y = 0; *x = 0; *height = w->dpy_height; *width = w->dpy_width;
    }
  else
    {
      Client *p = NULL, *main_client = wm_get_visible_main_client(w);
      Bool    have_toolbar = False;

     stack_enumerate(w, p)
      {
	if (p->type == MBCLIENT_TYPE_TOOLBAR && p->mapped 
	    && !(p->flags & CLIENT_IS_MINIMIZED))
	  { have_toolbar = True; break; }
      }

      *y = wm_get_offsets_size(w, NORTH, NULL, True);

      if (main_client && (main_client->flags & CLIENT_FULLSCREEN_FLAG))
	{
	  /* Fullscreen window present, allow dialogs to position themselves
           * move or less anywhere
           * 
           * XXX: should we check is this dialog is trans for it or trans for
           *      root ?
	  */

	  *height = w->dpy_height - *y;
	  *x = 0;
	  *width  = w->dpy_width;

	  /* This mainly for alt toolbars so dialogs get positioned ok
	   * for fullscreen.    
           * Note the above wm_get_offsets calls with NORTH, not SOUTH,
           * this is what below is basically doing. 
	  */
	  if (p != NULL && have_toolbar)
	    *height -=  p->height;

	  dbg("%s() height is %i, y is %i\n", __func__, *height, *y);

	}
      else
	{
	  /* if toolbar ( input window present ) dialogs can cover titlebars 
	   * as can transient for root dialogs. 
	   */
	  if (!have_toolbar)
	    *y  += main_client_title_height(c->trans);
	  
	  *height = w->dpy_height - *y - wm_get_offsets_size(w, SOUTH, 
							     NULL, True);
	  *x      = wm_get_offsets_size(w, WEST, NULL, True);
	  *width  = w->dpy_width - *x - wm_get_offsets_size(w, EAST, 
							    NULL, True);
	}

      dbg("%s() (toolbar) offsets south is %i\n", 
	  __func__, wm_get_offsets_size(w, SOUTH, NULL, True));


    }
}

/* 
 * dialog_constrain_gemoetry()
 *
 * called mainly by wm_restack to suggest better positions for dialogs
 * in relation to panels and toolbar/input wins. 
 *
 * req params are reparented window geometry *without* borders
 *
 *  returns True if geometry supplied fits - is good. 
 *  retruns False if geometry supplyied bad,  supplied geometry is updated
 *  to fit. 
 */
Bool
dialog_constrain_geometry(Client *c,
			  int    *req_x,
			  int    *req_y,
			  int    *req_width,
			  int    *req_height)
{
  Wm  *w = c->wm;
  int avail_x, avail_y, avail_width, avail_height;
  int actual_x, actual_y, actual_width, actual_height;
  int bdr_south = 0, bdr_west = 0, bdr_east = 0, bdr_north = 0;

  Bool res = True;

  if (w->config->dialog_stratergy == WM_DIALOGS_STRATERGY_FREE)
    return True;

  /* Allow urgent dialogs to position themselves anywhere */
  if (c->flags & CLIENT_HAS_URGENCY_FLAG)
    return True;

  /* Decorationless dialogs position anywhere */
  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    return True;

  dialog_get_available_area(c,&avail_x, &avail_y, &avail_width, &avail_height);

  dbg("%s() getting available area (toolbar +%i+%i %ix%i)\n", 
      __func__, avail_x, avail_y, avail_width, avail_height);

  /* Figure out window border offsets */
  dialog_client_get_offsets(c, &bdr_east, &bdr_south, &bdr_west);
  bdr_north = dialog_client_title_height(c);

  dbg("%s() - \n\t avail_x : %d\n\tavail_y : %d\n\tavail_width : %d"
      "\n\tavail_height %d\n\tbdr_south : %d\n\tbdr_west : %d"
      "\n\tbdr_east : %d\n\tbdr_north : %d\n",
      __func__, avail_x, avail_y, avail_width, avail_height,
      bdr_south, bdr_west, bdr_east, bdr_north);

  /* Dialog geometry with decorations */
  actual_x = *req_x - bdr_east - DIALOG_PADDING;
  actual_y = *req_y - bdr_north - DIALOG_PADDING;
  actual_width = *req_width + bdr_east + bdr_west + (2*DIALOG_PADDING);
  actual_height = *req_height + bdr_north + bdr_south + (2*DIALOG_PADDING);

  if (c->init_width && c->init_height 
      && (c->init_width > *req_width || c->init_height > *req_height) )
    {
      actual_width  = c->init_width + bdr_east + bdr_west + (2*DIALOG_PADDING);
      actual_height = c->init_height + bdr_north + bdr_south + (2*DIALOG_PADDING);
    }

  if (actual_width > avail_width)  /* set width to fit  */
    {
      *req_width = avail_width - ( bdr_east + bdr_west + (2*DIALOG_PADDING));
      actual_width = avail_width;
      res = False;
    }

  if (actual_height > avail_height)  /* and height  */
    {
      *req_height = avail_height -(bdr_south + bdr_north + (2*DIALOG_PADDING));
      actual_height = avail_height;
      res = False;
    }

  if (actual_x < avail_x)   /* move dialog right */
    {
      *req_x = avail_x + bdr_west + DIALOG_PADDING;
      res = False;
    }

  if (actual_y < avail_y)    /* move dialog up */
    {
      *req_y = avail_y + bdr_north + DIALOG_PADDING;
      res = False;
    }

  if (actual_x > avail_x    /* move dialog right */
      && (actual_x + actual_width) > (avail_x + avail_width) )
    {
      *req_x = avail_x + bdr_west + DIALOG_PADDING;
      res = False;
    }

   if ( (actual_y + actual_height) > (avail_y + avail_height) )
     {
       *req_y = (avail_y + avail_height) - actual_height + bdr_north + DIALOG_PADDING;
       res = False;
     }

  return res;
}
		
void
dialog_init_geometry(Client *c)      
{
  Wm  *w = c->wm;
  int  avail_x, avail_y, avail_width, avail_height;
  int  bdr_south = 0, bdr_west = 0, bdr_east = 0, bdr_north = 0;

  /* Check if we actually want to perform any sizing intervention */
  if (w->config->dialog_stratergy == WM_DIALOGS_STRATERGY_FREE)
    return;

  /* Allow decorationless dialogs to position themselves anywhere 
   * But centered initially if 0,0 and a splash screen.
  */
  if (c->flags & (CLIENT_TITLE_HIDDEN_FLAG))
    {
      if ((c->flags & CLIENT_IS_SPLASH_WIN) && c->x == 0 && c->y == 0)
	{
	  if (c->height < w->dpy_height)
	    c->y = (w->dpy_height - c->height)/2;
      
	  if (c->width < w->dpy_width)
	    c->x = (w->dpy_width - c->width)/2;
	}
      return;
    }

  dialog_get_available_area(c,&avail_x, &avail_y, &avail_width, &avail_height);

  /* Figure out window border offsets */
  dialog_client_get_offsets(c, &bdr_east, &bdr_south, &bdr_west);
  bdr_north = dialog_client_title_height(c);

  dbg("%s() - \n\t avail_x : %d\n\tavail_y : %d\n\tavail_width : %d"
      "\n\tavail_height %d\n\tbdr_south : %d\n\tbdr_west : %d"
      "\n\tbdr_east : %d\n\tbdr_north : %d\n gravity %i\n",
      __func__, 
      avail_x, avail_y, avail_width, avail_height,
      bdr_south, bdr_west, bdr_east, bdr_north, c->gravity);

  switch (c->gravity)
    {
    case NorthGravity:
      c->y += (bdr_north + avail_y);
      break;
    case NorthEastGravity:
      c->y += (bdr_north + avail_y);
      c->x -= (bdr_east + (w->dpy_width - (avail_x + avail_width)));
      break;
    case WestGravity:
      c->x += (bdr_west + avail_x);
      break;
    case EastGravity:
      c->x -= (bdr_east + (w->dpy_width - (avail_x + avail_width)));
      break;
    case SouthWestGravity:
      c->y -= (bdr_south + (w->dpy_height - (avail_y + avail_height)));
      c->x += (bdr_west + avail_x);
      break;
    case SouthGravity:
      c->y -= (bdr_south + (w->dpy_height - (avail_y + avail_height)));
      break;
    case SouthEastGravity:
      c->x -= (bdr_east + (w->dpy_width - (avail_x + avail_width)));
      c->y -= (bdr_south + (w->dpy_height - (avail_y + avail_height)));
      break;
    case CenterGravity:
    case NorthWestGravity:
    case StaticGravity:
    default:
      break;
    }

  /* Message Dialogs are free to postion/size where ever but can use totally  
   * offscreen request to position to window corners - see below
   */
  if (c->flags & CLIENT_HAS_URGENCY_FLAG)
    {
      int win_width  = c->width + bdr_east;
      int win_height = c->height + bdr_south;

      if (c->x >= w->dpy_width) 
	c->x = w->dpy_width - win_width - (c->x - w->dpy_width );

      if (c->y >= w->dpy_height) 
	c->y = w->dpy_height - win_height - (c->y - w->dpy_height );

      return;
    }

  /* save values for possible resizing later if more space comes available */
  c->init_width  = c->width;
  c->init_height = c->height;

  dbg("%s() set init, %ix%i, wants x:%d y:%d\n", 
      __func__, c->init_width, c->init_height, c->x, c->y); 

  /* Fix width/height  */
  if ((c->width + bdr_east + bdr_west) > avail_width)
    c->width = (avail_width - bdr_east - bdr_west - (2*DIALOG_PADDING));

  if ((c->height + bdr_north + bdr_south) > avail_height)
    c->height = (avail_height - bdr_north - bdr_south - (2*DIALOG_PADDING));

  /* Reposition dialog to center of avialable space if ;
   *   + positioned at 0,0
   *   + positioned offscreen
   */
  if ( (c->x - bdr_west) < avail_x 
       || (c->x + c->width + bdr_east) > (avail_x + avail_width))
    {
      dbg("%s() centering x pos\n", __func__);
      c->x = (avail_width  - (c->width + bdr_east + bdr_west))/2 
	+ bdr_west + avail_x;
    }

  if ( (c->y - bdr_north) < avail_y
       || (c->y + c->height + bdr_south) > (avail_y+avail_height))
    {
      dbg("%s() centering y pos\n", __func__);
      c->y = (avail_height - (c->height + bdr_south + bdr_north))/2 + avail_y + bdr_north;
    }

  /* horiz contarined mode - force dialog to be full width*/
  if (c->wm->config->dialog_stratergy == WM_DIALOGS_STRATERGY_CONSTRAINED_HORIZ
      && !(c->flags & CLIENT_TITLE_HIDDEN_FLAG) )
    {
      c->x     = avail_x + bdr_west;
      c->width = avail_width - (bdr_east + bdr_west);
    }
}

void
dialog_client_configure(Client *c)
{
  dbg("%s() client has border only hint: %s\n",
      __func__, (c->flags & CLIENT_BORDERS_ONLY_FLAG) ? "yes" : "no");

  dbg("%s() client has menu hint: %s\n",
      __func__, (c->flags & CLIENT_IS_MENU_DIALOG) ? "yes" : "no");

  dialog_init_geometry(c);
}

void
dialog_client_redraw(Client *c, Bool use_cache)
{
  Wm *w = c->wm;

  Bool is_shaped = False;

  int offset_north = 0, offset_south = 0, offset_west = 0, offset_east = 0;
  int total_w = 0, total_h = 0;

  int frame_ref_top   = FRAME_DIALOG;
  int frame_ref_east  = FRAME_DIALOG_EAST;
  int frame_ref_west  = FRAME_DIALOG_WEST;
  int frame_ref_south = FRAME_DIALOG_SOUTH;

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) return;

  if (use_cache) return;

  offset_north = dialog_client_title_height(c);

  dialog_client_get_offsets(c, &offset_east, &offset_south, &offset_west);

  total_w = offset_east  + offset_west + c->width;
  total_h = offset_north + offset_south + c->height;


  if (c->flags & CLIENT_BORDERS_ONLY_FLAG 
      && theme_has_frame_type_defined(c->wm->mbtheme, FRAME_DIALOG_NORTH))
    frame_ref_top   = FRAME_DIALOG_NORTH;

  /* 'message dialogs have there own decorations */
  if (c->flags & CLIENT_HAS_URGENCY_FLAG
      && theme_has_message_decor(w->mbtheme))
    {
      frame_ref_top   = FRAME_MSG;
      frame_ref_east  = FRAME_MSG_EAST;
      frame_ref_west  = FRAME_MSG_WEST;
      frame_ref_south = FRAME_MSG_SOUTH;
    }
  else if (c->flags & CLIENT_BORDERS_ONLY_FLAG
	   && theme_has_borders_only_decor(c->wm->mbtheme))
    {
      frame_ref_top   = FRAME_DIALOG_NT_NORTH;
      frame_ref_east  = FRAME_DIALOG_NT_EAST;
      frame_ref_west  = FRAME_DIALOG_NT_WEST;
      frame_ref_south = FRAME_DIALOG_NT_SOUTH;
    }

  dbg("%s() c->width : %i , offset_east : %i, offset_west : %i\n",
      __func__, c->width, offset_east, offset_west );

  is_shaped = theme_frame_wants_shaped_window( c->wm->mbtheme, frame_ref_top);

  if (is_shaped) client_init_backing_mask(c, total_w, c->height, 
					  offset_north, offset_south,
					  offset_east, offset_west);

  theme_frame_paint(c->wm->mbtheme, c, frame_ref_top, total_w, offset_north); 
    
  theme_frame_paint(c->wm->mbtheme, c, frame_ref_west,
		    offset_west, c->height); 
  
  theme_frame_paint(c->wm->mbtheme, c, frame_ref_east, 
		    offset_east, c->height); 

  theme_frame_paint(c->wm->mbtheme, c, frame_ref_south, 
		    total_w, offset_south); 

  /* We dont paint buttons for borderonly and message dialogs */
  if (!(c->flags & CLIENT_BORDERS_ONLY_FLAG
	|| c->flags & CLIENT_HAS_URGENCY_FLAG))
    {
      theme_frame_button_paint(c->wm->mbtheme, c, 
			       BUTTON_ACTION_CLOSE, 
			       INACTIVE, FRAME_DIALOG, 
			       total_w, offset_north);

      if (c->flags & CLIENT_ACCEPT_BUTTON_FLAG)
	theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_ACCEPT, 
				 INACTIVE, FRAME_DIALOG,total_w, offset_north);

      if (c->flags & CLIENT_HELP_BUTTON_FLAG)
	theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_HELP, 
				 INACTIVE, FRAME_DIALOG,total_w, offset_north);

      if (c->flags & CLIENT_CUSTOM_BUTTON_FLAG)
	theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_CUSTOM, 
				 INACTIVE, FRAME_DIALOG, total_w,offset_north);
    }


  if (is_shaped && !c->is_argb32)
    {
      XRectangle rects[1];

      rects[0].x = offset_west;  
      rects[0].y = offset_north;
      rects[0].width = total_w - offset_west - offset_east;
      rects[0].height = total_h - offset_south - offset_north;


      XShapeCombineRectangles ( c->wm->dpy, c->frame, 
				ShapeBounding,
				0, 0, rects, 1, ShapeSet, 0 );

#ifndef USE_COMPOSITE

#if 0
      if (c->wm->config->dialog_shade && (c->flags & CLIENT_IS_MODAL_FLAG)) 
	{
	  /* client->frame is our lowlighted window, so we only shape
           * our decor frames. We dont need to do this for composite. 
	   *
           * TODO: the logic for all this is very messy. fix.
	  */

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[NORTH], 
			     ShapeBounding, 0, 0, 
			     c->backing_masks[MSK_NORTH], ShapeSet);

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[SOUTH], 
			     ShapeBounding, 0, 0,
			     c->backing_masks[MSK_SOUTH], ShapeSet);

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[WEST], 
			     ShapeBounding, 0, 0,
			     c->backing_masks[MSK_WEST], ShapeSet);
	  XShapeCombineMask( c->wm->dpy, c->frames_decor[EAST], 
			     ShapeBounding, 
			     0, 0,
			     c->backing_masks[MSK_EAST], ShapeSet);
	}
      else
#endif

#endif
	{

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[NORTH], 
			     ShapeBounding, 0, 0, 
			     c->backing_masks[MSK_NORTH], ShapeSet);

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[SOUTH], 
			     ShapeBounding, 0, 0,
			     c->backing_masks[MSK_SOUTH], ShapeSet);

	  XShapeCombineMask( c->wm->dpy, c->frames_decor[WEST], 
			     ShapeBounding, 0, 0,
			     c->backing_masks[MSK_WEST], ShapeSet);
	  XShapeCombineMask( c->wm->dpy, c->frames_decor[EAST], 
			     ShapeBounding, 
			     0, 0,
			     c->backing_masks[MSK_EAST], ShapeSet);

	  XShapeCombineShape ( c->wm->dpy, 
			       c->frame,
			       ShapeBounding, 0, 0, 
			       c->frames_decor[NORTH],
			       ShapeBounding, ShapeUnion);
	  
	  XShapeCombineShape ( c->wm->dpy, 
			       c->frame,
			       ShapeBounding, 0, total_h - offset_south, 
			       c->frames_decor[SOUTH],
			       ShapeBounding, ShapeUnion);
	  
	  XShapeCombineShape ( c->wm->dpy, 
			       c->frame,
			       ShapeBounding, 0, offset_north,
			       c->frames_decor[WEST],
			       ShapeBounding, ShapeUnion);
	  
	  XShapeCombineShape ( c->wm->dpy, 
			       c->frame,
			       ShapeBounding, 
			       total_w - offset_east, offset_north,
			       c->frames_decor[EAST],
			       ShapeBounding, ShapeUnion);

	}
    }
}

void
dialog_client_button_press(Client *c, XButtonEvent *e)
{
  Wm *w = c->wm;
  int offset_north = dialog_client_title_height(c);
  int offset_south = 0, offset_west = 0, offset_east = 0;

   if (w->config->super_modal)
     {
       if (w->focused_client != c)
	 {
	   MBList *button_item = client_get_button_list_item_from_event(c, e);
	   /* In the precense of a modal transient dialog ignore 
	    * certain buttons. 
	    */
	   if ((button_item && button_item->id == BUTTON_ACTION_HELP)
	       || (button_item && button_item->id == BUTTON_ACTION_CLOSE))
	     return;
	 }
     }

  dialog_client_get_offsets(c, &offset_east, &offset_south, &offset_west);

  switch (client_button_do_ops(c, e, FRAME_DIALOG, 
			       c->width + offset_east + offset_west, 
			       offset_north))
   {
      case BUTTON_ACTION_CLOSE:
	client_deliver_delete(c);
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
      case -1: 		 /* Cancelled  */
	 break;
      case 0:
	/* Not on button */
	if (w->config->dialog_stratergy == WM_DIALOGS_STRATERGY_STATIC)
	  {
	    /* For static undraggble/stack fixed dialog we simply  
	     * hide the dialog when titlebar is clicked on. 
	     *
	     * TODO: Ideally this code should go in drag loop. but
	     *       it seems the servergrab() stops underlying app
	     *       from repainting itself. Need to look more into 
	     *       this.
	     *       Is it safe to simply remove the server grab ?
	     *
	     */
	    if (c->flags & CLIENT_HAS_URGENCY_FLAG)
	      return; 		/* No Effect for message wins */
	    
	    if (e->window != c->frames_decor[NORTH])
	      return;
	    
	    XUnmapWindow(w->dpy, c->frame);

	    dialog_client_drag(c);

	    misc_trap_xerrors(); 
	    XMapWindow(w->dpy, c->frame);

	    if (w->focused_client == c)
	      {
		w->focused_client = NULL;
		client_set_focus(c);
	      }
	    misc_untrap_xerrors(); 	    

	    XSync(w->dpy, False);
	    return;
	  }

	dialog_client_drag(c); 
	break;
   }
}

static void
dialog_client_drag(Client *c) /* drag box */
{
  Wm                  *w = c->wm;
  XEvent               ev;
  int                  offset_south = 0, offset_west = 0, offset_east = 0;
  int                  x1, y1, old_cx = c->x, old_cy = c->y;
  int                  frm_size     = dialog_client_title_height(c);
  XSetWindowAttributes attr;
  Window               win_outline;
  XRectangle           rects[1];
  Bool                 done = False, client_removed = False;

  dbg("%s called\n", __func__);

  dialog_client_get_offsets(c, &offset_east, &offset_south, &offset_west);

  if (XGrabPointer(c->wm->dpy, c->wm->root, False,
		   (ButtonPressMask|ButtonReleaseMask|PointerMotionMask),
		   GrabModeAsync,
		   GrabModeAsync, None, c->wm->curs_drag, CurrentTime)
      != GrabSuccess)
    return;

  attr.override_redirect = True;
  attr.background_pixel  = BlackPixel(w->dpy, w->screen);  

  win_outline = XCreateWindow(w->dpy, 
			      w->root,
			      c->x - offset_west, c->y - frm_size,
			      c->width + offset_west + offset_east,
			      c->height + frm_size + offset_south,
			      0,
			      CopyFromParent, 
			      CopyFromParent, 
			      CopyFromParent,
			      CWBackPixel|CWOverrideRedirect,
			      &attr);

  rects[0].x      = 2;  
  rects[0].y      = 2;
  rects[0].width  = c->width + offset_west + offset_east - 4;
  rects[0].height = c->height + frm_size + offset_south  - 4;

  XShapeCombineRectangles (w->dpy, win_outline,
			   ShapeBounding,
			   0, 0, rects, 1, ShapeSubtract, 0 );

  XMapWindow (w->dpy, win_outline);
  XSync (w->dpy, False);

  comp_engine_client_show(c->wm, c); 

  _get_mouse_position(c->wm, &x1, &y1);

  XFlush(c->wm->dpy);

  while (!done) 
    {
      int wanted_x = 0, wanted_y = 0;

      XMaskEvent(c->wm->dpy, 
		 ButtonPressMask|ButtonReleaseMask
		 |PointerMotionMask|SubstructureNotifyMask,
		 &ev);

    switch (ev.type) 
      {
      case MapRequest:
	wm_handle_map_request(w, &ev.xmaprequest); 
	break;

      case UnmapNotify:
	{
	  if (wm_find_client(w, ev.xunmap.window, WINDOW) == c)
	    {
	      client_removed = True;
	      done = True;
	    }

	  wm_handle_unmap_event(w, &ev.xunmap); 
	  break;
	}

      case MotionNotify:
	if (w->config->dialog_stratergy == WM_DIALOGS_STRATERGY_STATIC)
	  break;

	wanted_x = (old_cx + (ev.xmotion.x - x1));
	wanted_y = (old_cy + (ev.xmotion.y - y1));

	c->x = wanted_x;
	c->y = wanted_y;

	XMoveWindow(c->wm->dpy, win_outline, c->x-offset_west, c->y-frm_size);
	break;

      case ButtonRelease:
	XMoveWindow(c->wm->dpy, c->frame, c->x - offset_west,
		    c->y - dialog_client_title_height(c));
	done = True;
	break;
      }
    }


  XUngrabPointer(w->dpy, CurrentTime);

  misc_trap_xerrors(); 
  XDestroyWindow (w->dpy, win_outline);

  if (client_removed == False) 
    {
      c->show(c);
      
      if (w->config->dialog_stratergy != WM_DIALOGS_STRATERGY_STATIC)
	{
	  wm_activate_client(c);
	  client_deliver_config(c);
	}
    }

  misc_untrap_xerrors();
}


static void
_get_mouse_position(Wm *w, int *x, int *y)
{
  Window mouse_root, mouse_win;
  int win_x, win_y;
  unsigned int mask;
  
  XQueryPointer(w->dpy, w->root, &mouse_root, &mouse_win,
		x, y, &win_x, &win_y, &mask);
}

static Client*
dialog_client_set_focus_next(Client *c)
{
  Wm *w = c->wm; 

  /* We dont have focus, therefore dont suggest what to focus next */
  if (w->focused_client != c)
    {
      dbg("%s we dont have focus\n", __func__); 
      return NULL;
    }

  if (c->next_focused_client && c != c->next_focused_client)
    {
      dbg("%s setting focus to c->next_focused_client\n", __func__); 
      if (w->focused_client == c)
	w->focused_client = NULL;

      client_set_focus(c->next_focused_client); 
    }
  else
    {
      Client *app_client = NULL, *p = NULL;

      dbg("%s setting focus to visibe_main\n", __func__); 

      if (w->focused_client == c)
	w->focused_client = NULL;

      /* Find highest modal */
      stack_enumerate_reverse (w,p)
	{
	  if (p->type == MBCLIENT_TYPE_DIALOG
	      && (p->flags & CLIENT_IS_MODAL_FLAG)
	      && p != c)
	    return p;
	}

      /* Check if main window has modals and focus that */
      app_client = wm_get_visible_main_client(w);

      if (app_client)
	{
	  Client *modal = NULL;

	  modal = client_get_highest_transient(app_client, 
					       CLIENT_IS_MODAL_FLAG, c);

	  if (modal)
	    return modal;
	}

      return app_client;
    }

  return NULL;
}

void
dialog_client_iconize(Client *c)
{
  Wm     *w = c->wm; 
  Client *d = NULL, *p = NULL;

  client_set_state(c, IconicState);
  c->flags |= CLIENT_IS_MINIMIZED;
  c->mapped = False;
  XUnmapWindow(w->dpy, c->frame); 

  if (c->flags & CLIENT_IS_MODAL_FLAG)
    c->wm->n_modals_present--;

  if (c->win_modal_blocker)
    {
      XUnmapWindow(w->dpy, c->win_modal_blocker);
      dbg("%s() blocker win unmapped for '%s'\n", __func__, c->name);
    }

  /* Make sure any transients get iconized too */  
  stack_enumerate(w, p)
    if (p->trans == c)
      p->iconize(p);

  if ((d = dialog_client_set_focus_next(c)) != NULL)
    wm_activate_client(d);
}
 
void 
dialog_client_destroy(Client *c)
{
  Client *d = NULL;

  /* Focus the saved next or return a likely candidate if none found */
  d = dialog_client_set_focus_next(c);

  if (c->flags & CLIENT_IS_MODAL_FLAG)      
    c->wm->n_modals_present--;

  if (c->win_modal_blocker)
    {
      Wm *w = c->wm;
      XDestroyWindow(w->dpy, c->win_modal_blocker);
       w->n_modal_blocker_wins--;
       dbg("%s() blocker win destroyed for '%s'\n", __func__, c->name);
    }

  base_client_destroy(c);

  /* 
   *  We call activate_client mainly to figure out what to focus next.
   *  This probably only happens in the case of transient for root
   *  dialogs which likely have no real focus history.
   */
  if (d) 
      wm_activate_client(d);
}
