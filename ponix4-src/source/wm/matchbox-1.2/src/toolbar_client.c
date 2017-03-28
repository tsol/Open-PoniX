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

#ifndef USE_ALT_INPUT_WIN

Client*
toolbar_client_new(Wm *w, Window win)
{
   Client *c = base_client_new(w, win); 

   if (!c) return NULL;

   /* If theres not room for the toolbar, just make it an app window */
   /* In practise one would expect this to never happen              */
   if ((c->height) > w->dpy_height - (theme_frame_defined_height_get(w->mbtheme, FRAME_MAIN)*2) )
     {
       dbg("%s() not enough room for toolbar client\n", __func__);
       base_client_destroy(c);
       return main_client_new(w, win);
     }

   c->type = MBCLIENT_TYPE_TOOLBAR;
   
   c->configure    = &toolbar_client_configure;
   c->reparent     = &toolbar_client_reparent;
   c->button_press = &toolbar_client_button_press;
   c->redraw       = &toolbar_client_redraw;
   c->hide         = &toolbar_client_hide;
   c->iconize      = &toolbar_client_hide;
   c->show         = &toolbar_client_show;
   c->move_resize  = &toolbar_client_move_resize;
   c->get_coverage = &toolbar_client_get_coverage;
   c->destroy      = &toolbar_client_destroy;

   client_set_state(c,WithdrawnState); 	/* set initially to signal show() */
   
   return c;
}

void
toolbar_client_configure(Client *c)
{
  Wm *w = c->wm;

  if (c->flags & CLIENT_IS_MINIMIZED)
    return;

  c->y = w->dpy_height - wm_get_offsets_size(w, SOUTH, c, True)
    - c->height;
  c->x = toolbar_win_offset(c) 
    + wm_get_offsets_size(w, WEST,  NULL, False);
  c->width = w->dpy_width - toolbar_win_offset(c)
    - wm_get_offsets_size(w, WEST,  NULL, False)
    - wm_get_offsets_size(w, EAST,  NULL, False);
}

void
toolbar_client_move_resize(Client *c)
{
  Wm *w = c->wm;

  int max_offset = theme_frame_defined_width_get(w->mbtheme, 
						 FRAME_UTILITY_MAX);
  int min_offset = theme_frame_defined_height_get(w->mbtheme, 
						  FRAME_UTILITY_MIN);
  int offset = toolbar_win_offset(c);
   
  base_client_move_resize(c);

  if (!(c->flags & CLIENT_IS_MINIMIZED))
    {
      if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) max_offset = 0;
      
      XResizeWindow(w->dpy, c->window, c->width, c->height);
      XMoveResizeWindow(w->dpy, c->frame, c->x - max_offset,
		  c->y, c->width + max_offset, c->height );
      if (client_title_frame(c))
	XMoveResizeWindow(w->dpy, client_title_frame(c), 0,
			  0, max_offset , c->height );
   } else {

     if (min_offset)
       {
	 XMoveResizeWindow(w->dpy, c->frame, c->x,
			   c->y, c->width + max_offset, offset );
	 if (client_title_frame(c))
	   XMoveResizeWindow(w->dpy, client_title_frame(c), 0,
			     0, c->width + max_offset , min_offset );
       }
   }      
}

void
toolbar_client_reparent(Client *c)
{
  Wm *w = c->wm;

  XSetWindowAttributes attr;

  int frm_size = theme_frame_defined_width_get(w->mbtheme, 
					       FRAME_UTILITY_MAX );   
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

  if (frm_size && !(c->flags & CLIENT_TITLE_HIDDEN_FLAG))
    c->frames_decor[NORTH] =
      XCreateWindow(w->dpy, c->frame, 0, 0, frm_size, c->height, 0,
		    CopyFromParent, CopyFromParent, CopyFromParent,
		    CWOverrideRedirect|CWBackPixel|CWEventMask, &attr);
  
  XSetWindowBorderWidth(w->dpy, c->window, 0);

  XAddToSaveSet(w->dpy, c->window);

  XSelectInput(w->dpy, c->window, PropertyChangeMask);

  XReparentWindow(w->dpy, c->window, c->frame, toolbar_win_offset(c), 0);
}


void
toolbar_client_show(Client *c)
{
  Wm   *w = c->wm;
  long win_state; 

  c->mapped = True;

  if (w->stack_top_app 
      && (w->stack_top_app->flags & CLIENT_FULLSCREEN_FLAG))
    main_client_manage_toolbars_for_fullscreen(c, True);

   win_state = client_get_state(c);

   if (win_state == WithdrawnState)    /* initial show() state */
     {
       client_set_state(c,NormalState);
       wm_update_layout(c->wm, c, - c->height);
     } 
   else if (win_state == IconicState) /* minimised, set maximised */
     {
       client_set_state(c,NormalState);

       /* Make sure desktop flag is unset */
       c->flags &= ~CLIENT_IS_MINIMIZED;

       wm_update_layout(c->wm, c, -(c->height - toolbar_win_offset(c)));

       if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
	 {
	   c->x = wm_get_offsets_size(c->wm, WEST,  NULL, False);
	 }
       else
	 {
	   c->x = theme_frame_defined_width_get(w->mbtheme,
						FRAME_UTILITY_MAX )
	     + wm_get_offsets_size(c->wm, WEST,  NULL, False);
	   c->y = c->y - ( c->height - toolbar_win_offset(c));
	 }

       /* destroy buttons so they get recreated ok */   
       client_buttons_delete_all(c);   

       toolbar_client_move_resize(c);

       toolbar_client_redraw(c, False);
     } 

   stack_move_client_above_type(c, MBCLIENT_TYPE_APP|MBCLIENT_TYPE_DESKTOP);

   XMapSubwindows(w->dpy, c->frame);
   XMapWindow(w->dpy, c->frame);

   comp_engine_client_show(w, c);
}

void
toolbar_client_hide(Client *c)
{
  Wm   *w = c->wm;

  if (c->flags & CLIENT_IS_MINIMIZED || client_get_state(c) == IconicState) 
    return;
   
  client_set_state(c,IconicState);
  c->flags |= CLIENT_IS_MINIMIZED; 

  c->ignore_unmap++;
  XUnmapWindow(w->dpy, c->window);
  
  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG)
    {
      XUnmapWindow(w->dpy, c->frame);
    }
  else
    {
      client_buttons_delete_all(c);   
  
      c->x = wm_get_offsets_size(c->wm, WEST,  NULL, False);
  
      c->y = c->y + c->height 
               - theme_frame_defined_height_get(c->wm->mbtheme, FRAME_UTILITY_MIN);
    }

  toolbar_client_move_resize(c);

  toolbar_client_redraw(c, False);
  
  dbg("hiding toolbar y is now %i", c->y);

  wm_update_layout(c->wm, c, c->height - toolbar_win_offset(c));
}

void
toolbar_client_destroy(Client *c)
{
  Wm *w = c->wm;

  dbg("%s() called\n", __func__);
   
  c->mapped = False; /* Setting mapped to false will allow the 
                        dialog resizing/repositioning via restack
                        to ignore use  */

  if (c->x == theme_frame_defined_width_get(w->mbtheme, FRAME_UTILITY_MAX )
      || (c->flags & CLIENT_TITLE_HIDDEN_FLAG) )
    {
      wm_update_layout(w, c, c->height);
    } 
  else 
    {
      wm_update_layout(w, c, theme_frame_defined_height_get(w->mbtheme,
							   FRAME_UTILITY_MIN));
    }
  
  base_client_destroy(c);     
}

void
toolbar_client_get_coverage(Client *c, int *x, int *y, int *w, int *h)
{
   *x = c->x; *y = c->y;
   *w = c->width + toolbar_win_offset(c);

   if (!(c->flags & CLIENT_IS_MINIMIZED))
   {
      *x = c->x - toolbar_win_offset(c);
      *h = c->height;
   } else {
     *h = toolbar_win_offset(c);
   }
}

void
toolbar_client_button_press(Client *c, XButtonEvent *e)
{
  Wm *w = c->wm;
  int frame_id, cw, ch;
  int max_offset = theme_frame_defined_width_get(w->mbtheme,
						 FRAME_UTILITY_MAX);
  int min_offset = theme_frame_defined_height_get(w->mbtheme, 
						  FRAME_UTILITY_MIN);

   if (!(c->flags & CLIENT_IS_MINIMIZED))
     {
       frame_id = FRAME_UTILITY_MAX;
       cw = max_offset;
       ch = c->height;
     }
   else
     {
       frame_id = FRAME_UTILITY_MIN;
       cw = c->width + max_offset; 
       ch = min_offset;
     }
   
   switch (client_button_do_ops(c, e, frame_id, cw, ch))
     {
     case BUTTON_ACTION_CLOSE:
       client_deliver_delete(c);
       break;
     case BUTTON_ACTION_MIN:
       toolbar_client_hide(c);
       break;
     case BUTTON_ACTION_MAX:
       toolbar_client_show(c);
       break;
     case -1: 		 /* Cancelled  */
       break;
     case 0:
       /* Not on button */
       break;
     }
}

int
toolbar_win_offset(Client *c)
{
  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) 
    {
      dbg("return 0\n");
      return 0;
    }

   if (c->flags & CLIENT_IS_MINIMIZED)
     {
       return theme_frame_defined_height_get(c->wm->mbtheme, 
					     FRAME_UTILITY_MIN);
     } else {
       return theme_frame_defined_width_get(c->wm->mbtheme, 
					    FRAME_UTILITY_MAX);
     }
}

void
toolbar_client_redraw(Client *c, Bool use_cache)
{
  Wm *w = c->wm;

  int max_offset = theme_frame_defined_width_get(w->mbtheme, 
						 FRAME_UTILITY_MAX);
  int min_offset = theme_frame_defined_height_get(w->mbtheme, 
						  FRAME_UTILITY_MIN);

  comp_engine_client_show(w, c);

  if (c->flags & CLIENT_TITLE_HIDDEN_FLAG) return;

  if (use_cache) return;
  
  client_buttons_delete_all(c);
  
  if (c->flags & CLIENT_IS_MINIMIZED)
    {
      if (!min_offset) return;
      
      theme_frame_paint(w->mbtheme, c, FRAME_UTILITY_MIN, 
			c->width + max_offset, min_offset); 
      
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_CLOSE, 
			       INACTIVE, FRAME_UTILITY_MIN, 
			       c->width + max_offset, min_offset);
      
      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_MAX, 
			       INACTIVE, FRAME_UTILITY_MIN, 
			       c->width + max_offset, min_offset);
      
    } else {
      
      if (!max_offset) return;
      
      theme_frame_paint( w->mbtheme, c, FRAME_UTILITY_MAX, 
			 max_offset, c->height);

      dbg("%s() painting close button\n", __func__);
      

      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_CLOSE, 
			       INACTIVE, FRAME_UTILITY_MAX, 
			       max_offset, c->height);

      dbg("%s() painting min button\n", __func__);

      theme_frame_button_paint(w->mbtheme, c, BUTTON_ACTION_MIN, 
			       INACTIVE, FRAME_UTILITY_MAX, 
			       max_offset, c->height);
    }
}


#endif /* USE_ALT_INPUT_WIN */


