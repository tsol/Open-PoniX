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
 *  A 'select client' is the small titilebar task menu dropdown.
 */

#include "select_client.h"


Client*
select_client_new(Wm *w)
{
   Window win;
   XSetWindowAttributes attr;
   int button_x = 0;

   int width, height;
   Client *c;

   dbg("%s() called\n", __func__);

   if (w->flags & SINGLE_FLAG && !wm_get_desktop(w))
     return NULL;

   /* Make sure select menu is not shown on un decored desktop */
   if (!(w->flags & DESKTOP_DECOR_FLAG) && w->flags & DESKTOP_RAISED_FLAG)
     return NULL;

   if (!theme_frame_menu_get_dimentions(w->mbtheme, &width, &height))
     return NULL;

   if (width == 0 || height == 0) return NULL;

   dbg("%s() got valid size\n", __func__);

   button_x = theme_frame_button_get_x_pos(w->mbtheme, FRAME_MAIN, 
					   BUTTON_ACTION_MENU,
					   w->stack_top_app->width);
#ifndef STANDALONE
   {
     /*
      *	A uglyish hack to center the drop down menu on centered labels
      *	which are menu's. Should probably figure out how to get this
      *	into mbtheme.c ( into theme_frame_button_get_x_pos()! )
      */

     MBThemeFrame *frame;
     MBThemeFrame *frame_menu;
     MBThemeButton *button;
     MBThemeLayer  *layer;

     frame = (MBThemeFrame*)list_find_by_id(w->mbtheme->frames, FRAME_MAIN );
     frame_menu = (MBThemeFrame*)list_find_by_id(w->mbtheme->frames, FRAME_MENU );

     if (frame)
       {
	 button = (MBThemeButton *)list_find_by_id(frame->buttons, 
						   BUTTON_ACTION_MENU );
	 layer = (MBThemeLayer*)list_find_by_id(frame->layers, 
						LAYER_LABEL);

	 /* Also handle fixed X positions */
	 if (frame_menu && frame_menu->fixed_x != -1)
	   {
	     button_x = frame_menu->fixed_x;
	   }
	 else if (button && layer && layer->label 
		  && layer->label->justify == ALIGN_CENTER
		  && button->x->unit == textx
		  && button->w->unit == textw)
	   {
	     button_x = button_x 
	       + (( frame->label_w + ( MENU_ENTRY_PADDING + w->config->use_icons ) 
		    - width )/2 );
	   }
	 /* FIXME: should also do for ALIGN_RIGHT */
       }
   }
#endif

   if (XGrabPointer(w->dpy, w->root, True,
		    (ButtonPressMask|ButtonReleaseMask),
		    GrabModeAsync,
		    GrabModeAsync, None, w->curs, CurrentTime)
       != GrabSuccess)
      return NULL;

   XGrabKeyboard(w->dpy, w->root, True, GrabModeAsync, 
		 GrabModeAsync, CurrentTime);
   
   attr.override_redirect = True;
   attr.background_pixel = BlackPixel(w->dpy, w->screen);
   attr.event_mask = ButtonPressMask|ExposureMask|
     EnterWindowMask|LeaveWindowMask|PointerMotionMask;

   win = XCreateWindow(w->dpy, w->root,
		       button_x + wm_get_offsets_size(w, WEST, NULL, True),
		       wm_get_offsets_size(w, NORTH, NULL, True) 
		       + main_client_title_height(w->stack_top_app),
		       width, height, 0,
		       CopyFromParent, CopyFromParent, CopyFromParent,
		       CWOverrideRedirect|CWBackPixel|CWEventMask,
		       &attr);

   c = base_client_new(w, win);
   c->type = MBCLIENT_TYPE_TASK_MENU;
   client_title_frame(c) = c->frame = c->window;

   comp_engine_client_init(w, c); 
   
   c->destroy      = &select_client_destroy;
   c->button_press = &select_client_button_press;
   c->redraw       = &select_client_redraw;

   w->flags ^= MENU_FLAG;

   c->redraw(c, False);
   c->mapped = True;
   client_set_state(c, NormalState);

   c->height = height;
   c->width  = width;
   c->name   = strdup("");

   stack_move_top(c);
   
   XMapWindow(c->wm->dpy, c->frame);

   comp_engine_client_show(w, c);

   return c;
}

void 
select_client_event_loop( Client *c, struct list_item *button_item_cur )
{
  Client *p;
  XEvent ev;
  Bool in_window = True;
  MBClientButton *button;
  struct list_item *button_item_new = NULL;
  KeySym key;

  if (button_item_cur == NULL)
    {
      button_item_cur = c->buttons;
      in_window = False;
    }
  
  button = (MBClientButton *)button_item_cur->data;

  theme_frame_menu_highlight_entry( c, button, ACTIVE); 

  for (;;) 
    {
      XMaskEvent(c->wm->dpy, 
		 EnterWindowMask|LeaveWindowMask|KeyReleaseMask|KeyPressMask
		 |ExposureMask|ButtonReleaseMask|PointerMotionMask, &ev);

      switch (ev.type)
	{
	case KeyRelease:

	  switch (key = XKeycodeToKeysym (c->wm->dpy, ev.xkey.keycode, 0))
	    {
	    case XK_Up:
	      button_item_new = c->buttons;
	      if (button_item_cur == c->buttons)
		{
		  while(button_item_new->next != NULL)
		    button_item_new = button_item_new->next;
		}
	      else
		{
		  while(button_item_new->next != button_item_cur)
		    button_item_new = button_item_new->next;
		}

	      if (button_item_cur)
		theme_frame_menu_highlight_entry( c, button, INACTIVE); 
		
	      button_item_cur = button_item_new;
	      button = (MBClientButton *)button_item_cur->data;
	      theme_frame_menu_highlight_entry( c, button, ACTIVE); 
	      break;
	      
	    case XK_Down:

	      button_item_new = button_item_cur->next;
	      if (button_item_new == NULL)
		button_item_new = c->buttons; 

	      if (button_item_cur)
		theme_frame_menu_highlight_entry( c, button, INACTIVE); 
	      button_item_cur = button_item_new;
	      button = (MBClientButton *)button_item_cur->data;
	      theme_frame_menu_highlight_entry( c, button, ACTIVE); 
	      break;

	    case XK_Return:
	    case XK_KP_Enter:
	      p = (Client *)button->data;
	      theme_frame_menu_highlight_entry( c, button, INACTIVE); 
	      if (p->type == MBCLIENT_TYPE_DESKTOP)
		wm_toggle_desktop(c->wm);
	      else
		{
		  wm_activate_client(p);
		}
	      dbg("%s() returning ....\n", __func__);
	      return;
	    }
	  break;
	case ButtonRelease:
	  if (button && in_window)
	    {
	      p = (Client *)button->data;
	      theme_frame_menu_highlight_entry( c, button, INACTIVE); 
	      if (p->type == MBCLIENT_TYPE_DESKTOP)
		wm_toggle_desktop(c->wm);
	      else
		{
		  wm_activate_client(p);
		}
	    }
	  return;
	case EnterNotify:
	  in_window = True;
	  break;
	case MotionNotify:
	  /* XXX get list_item back, then can get next / prev for keys */
	
	  button_item_new 
	    = client_get_button_list_item_from_event(c, &ev.xbutton);
	  if (button_item_new != NULL && in_window)
	    {
	      if (button_item_new != button_item_cur)
		{
		  dbg("%s() got a button again\n", __func__ );
		  if (button_item_cur)
		    theme_frame_menu_highlight_entry( c, button, INACTIVE); 
		  button_item_cur = button_item_new;
		   button = (MBClientButton *)button_item_cur->data;
		   theme_frame_menu_highlight_entry( c, button, ACTIVE); 
		 }
	     }
	   break;
	 case LeaveNotify:
	   in_window = False;
	   button_item_cur = NULL;
	   /* Clear button highlight */
	   theme_frame_menu_highlight_entry( c, button, INACTIVE); 
	   break;
	 }

     }
}

void
select_client_button_press( Client *c, XButtonEvent *e)
{

  struct list_item *button_item = NULL;

  if ((button_item = client_get_button_list_item_from_event(c, e)) == NULL)
    {
      dbg("%s() failed to get a button\n", __func__ );
      select_client_destroy(c);
      return;
    }

  select_client_event_loop( c, button_item );

  dbg("%s() calling destroy\n", __func__);
  
  select_client_destroy(c);

}

void
select_client_redraw(Client *c, Bool use_cache)
{
   MBTheme *theme = c->wm->mbtheme;
   Bool is_shaped = False;

   dbg("%s() called\n", __func__);

   if (use_cache)
     return;

   is_shaped = theme_frame_wants_shaped_window( theme, FRAME_MENU);

   if (is_shaped) 
     client_init_backing_mask(c, c->width, 0, c->height, 0, 0, 0 );

   theme_frame_menu_paint( theme, c);
  
   if (is_shaped)
     XShapeCombineMask( c->wm->dpy, c->frame, ShapeBounding, 0, 0, 
			c->backing_masks[MSK_NORTH], ShapeSet);

   XClearWindow(c->wm->dpy, c->frame);

}

void
select_client_destroy(Client *c)
{
  dbg("%s() called\n", __func__);

   XUngrabPointer(c->wm->dpy, CurrentTime);
   XUngrabKeyboard(c->wm->dpy, CurrentTime);

   c->wm->flags ^= MENU_FLAG;

   XUnmapWindow(c->wm->dpy, c->frame);

   base_client_destroy(c);
}


