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

#include "client_common.h"

/*
   Common functions for use by all client types
*/

void
client_set_state(Client *c, int state)
{
  Wm *w = c->wm;
  CARD32 data[2];

  data[0] = state;
  data[1] = None;
   
  XChangeProperty(w->dpy, c->window, 
		  w->atoms[WM_STATE], w->atoms[WM_STATE],
		  32, PropModeReplace, (unsigned char *)data, 2);
}

long
client_get_state(Client *c)
{
  Wm *w = c->wm;

  Atom          real_type; 
  int           real_format;
  unsigned long items_read, items_left;
  long         *data = NULL, state = WithdrawnState;

  misc_trap_xerrors(); 

  if (XGetWindowProperty(w->dpy, c->window,
			 w->atoms[WM_STATE], 0L, 2L, False,
			 w->atoms[WM_STATE], &real_type, &real_format,
			 &items_read, &items_left,
			 (unsigned char **) &data) == Success
      && items_read)
    state = *data;

  if (misc_untrap_xerrors()) 	/* Just in case */
    state = WithdrawnState; 

  if (data)
    XFree(data);

  return state;
}


void
client_deliver_config(Client *c)
{
  Wm *w = c->wm;
  XConfigureEvent ce;
   
  ce.type = ConfigureNotify;
  ce.event = c->window;
  ce.window = c->window;
  ce.x = c->x;
  ce.y = c->y;
  ce.width = c->width;
  ce.height = c->height;
  ce.border_width = 0;
  ce.above = None;
  ce.override_redirect = 0;
   
  dbg("%s() to %s  x: %i , y: %i w: %i h: %i \n", 
      __func__, c->name, ce.x, ce.y, ce.width, ce.height);

  XSendEvent(w->dpy, c->window, False,
	     StructureNotifyMask, (XEvent *)&ce);
}

void
client_deliver_wm_protocol(Client *c, Atom delivery)
{
  Wm *w = c->wm;

  client_deliver_message(c, w->atoms[WM_PROTOCOLS], 
			 delivery, CurrentTime, 0, 0, 0);
}

void
client_get_wm_protocols(Client *c)
{
  Atom  *protocols = NULL;
  int    n = 0, i = 0;
  Status status;
  
   /* Check for 'special' extra button/ping protocols */

  misc_trap_xerrors();

  status = XGetWMProtocols(c->wm->dpy, c->window, &protocols, &n);

  if (status && n && !misc_untrap_xerrors()) 
    {
      dbg("%s() checking wm protocols ( %i found )\n", __func__, n);

      c->flags &= ~(CLIENT_HELP_BUTTON_FLAG|CLIENT_ACCEPT_BUTTON_FLAG|CLIENT_CUSTOM_BUTTON_FLAG);

      for (i=0; i<n; i++)
	{
	  if (protocols[i] == c->wm->atoms[_NET_WM_CONTEXT_HELP])
	    {
	      dbg("%s() got _NET_WM_CONTEXT_HELP protocol\n", __func__ );
	      c->flags |= CLIENT_HELP_BUTTON_FLAG;
	    }
	  else if (protocols[i] == c->wm->atoms[_NET_WM_CONTEXT_ACCEPT])
	    {
	      dbg("%s() got _NET_WM_CONTEXT_ACCEPT protocol\n", __func__ );
	      c->flags |= CLIENT_ACCEPT_BUTTON_FLAG;
	    }
	  else if (protocols[i] == c->wm->atoms[_NET_WM_CONTEXT_CUSTOM])
	    {
	      dbg("%s() got _NET_WM_CONTEXT_CUSTOM protocol\n", __func__ );
	      c->flags |= CLIENT_CUSTOM_BUTTON_FLAG;
	    }
#ifndef NO_PNG
	  else if (protocols[i] == c->wm->atoms[_NET_WM_PING]
		   && c->host_machine && c->pid)
	    {
	      dbg("%s() has PING ewmh\n", __func__);
	      c->has_ping_protocol = True;
	    }
#endif
#ifdef USE_XSYNC
	  else if (protocols[i] == c->wm->atoms[_NET_WM_SYNC_REQUEST])
	    {
	      c->has_ewmh_sync = True;
	      dbg("%s() client has _NET_WM_SYNC_REQUEST\n", __func__);
	    }
#endif
	}
    }

  if (protocols)
    XFree(protocols);
}

void
client_deliver_message(Client       *c, 
		       Atom          delivery_atom,
		       unsigned long data0,
		       unsigned long data1,
		       unsigned long data2,
		       unsigned long data3,
		       unsigned long data4)
{
  Wm *w = c->wm;

  XEvent ev;

  memset(&ev, 0, sizeof(ev));

  ev.xclient.type = ClientMessage;
  ev.xclient.window = c->window;
  ev.xclient.message_type = delivery_atom;
  ev.xclient.format = 32;
  ev.xclient.data.l[0] = data0;
  ev.xclient.data.l[1] = data1;
  ev.xclient.data.l[2] = data2;
  ev.xclient.data.l[3] = data3;
  ev.xclient.data.l[4] = data4;

  XSendEvent(w->dpy, c->window, False, NoEventMask, &ev);

  XSync(w->dpy, False);
}


/* 'Really' kill an app if it gives us enough info */
Bool
client_obliterate(Client *c)
{
  char buf[257];
  int  sig  = 9;

  if (c->host_machine == NULL || !c->pid)
    return False;

  if (gethostname (buf, sizeof(buf)-1) == 0)
    {
      if (!strcmp (buf, c->host_machine))
	{
	  if (kill (c->pid, sig) < 0)
	    {
	      fprintf(stderr, "matchbox: kill %i on %s failed.\n",
		      sig, c->name);
	      return False;
	    }
	}
      else return False; 	/* on a different host */
    }
  else 
    {
      fprintf(stderr, "matchbox: gethostname failed\n");
      return False;
    }

  return True;
}

void
client_deliver_delete(Client *c)
{
  Wm *w = c->wm;

  int   i, n, found = 0;
  Atom *protocols;
    
  if (XGetWMProtocols(w->dpy, c->window, &protocols, &n)) {
    for (i=0; i<n; i++)
      if (protocols[i] == w->atoms[WM_DELETE_WINDOW]) found++;
    XFree(protocols);
  }

  /* Initiate pinging the app - to really kill hung applications */

  if (c->has_ping_protocol && c->pings_pending == -1) 
    {
      ewmh_ping_client_start (c);
    }

  if (found)
    client_deliver_wm_protocol(c, w->atoms[WM_DELETE_WINDOW]);
  else 
    {
      if (!client_obliterate(c))
	XKillClient(w->dpy, c->window);
    }
}

int
client_want_focus(Client *c)
{
  Wm *w = c->wm;

  int       ret = 1;
  XWMHints *hints = NULL;

  misc_trap_xerrors(); 

  hints = XGetWMHints(w->dpy, c->window);

  /* TODO: Oddly the above will sometimes fire an X Error, yet hints get set. 
   *       Check this.   
  */
  if (misc_untrap_xerrors())
    {
      dbg("%s() called, gave X Error for %s but going to focus it anyway\n", 
	  __func__, c->name);
    }

  if (hints != NULL)
    {
      if ((hints->flags & InputHint) && (hints->input == False)) 
	ret = 0;
      
      XFree(hints);
    }
  
  return ret;
}

Bool
client_set_focus(Client *c)
{
  Wm *w;

  if (!c) return False;

  w = c->wm;

  dbg("%s() called, checking if %s wants focus\n", __func__, c->name);

  if (client_want_focus(c))
    {
      if (w->focused_client == c)
	return True; 

      misc_trap_xerrors(); 

      XSetInputFocus(w->dpy, c->window, RevertToPointerRoot, CurrentTime);

      /* TODO: - Should we handle WM_TAKE_FOCUS ? 
       *       - Handle focs in/out message on root ?
       */

      /* Rememeber what was focused last */
      if (w->focused_client)
	{
	  Client *trans_old = w->focused_client;
	  Client *trans_new = c;

	  while (trans_old->trans != NULL)
	    trans_old = trans_old->trans;

	  while (trans_new->trans != NULL)
	    trans_new = trans_new->trans;

	  /* Are we both transient for the same thing */
	  if (trans_new && trans_old && trans_new == trans_old)
	    c->next_focused_client = w->focused_client;

	  /* From regular dialog to transient for root dialogs */
	  if (w->focused_client->type == MBCLIENT_TYPE_DIALOG
	      && c->type == MBCLIENT_TYPE_DIALOG
	      && c->trans == NULL)
	    c->next_focused_client = w->focused_client;
	    
	}

      if (misc_untrap_xerrors())
	{
	  w->focused_client = NULL;
	}
      else
	w->focused_client = c;

      dbg("%s() called, setting focus to %s\n", 
	  __func__, c->name);

      ewmh_set_active(w);

      return True;
    }
  
  return False;
}


void
client_get_transient_list(Wm *w, MBList **list, Client *c)
{
  Client *p = NULL;

  stack_enumerate(w,p)
    {
      if (p != c && p->type == MBCLIENT_TYPE_DIALOG)
	{
	  Client *trans = p->trans;

	  dbg("%s() checking <%s> trans for <%s>\n",
	      __func__, p->name, trans ? trans->name : "nothing");

	  if (c == NULL) 
	    { 			
	      /* Transient for root dialogs */
	      if (trans == NULL)
		{
		  list_add(list, NULL, 0, p);
		}
	      else
		{
		  /* The dialog isn't transient for root directly 
                   * but possibly one of its transient parents are
		   */

		  while (trans->trans != NULL)
		    trans = trans->trans;

		  /* trans is now transient for nothing
                   * so if its a dialog add it.
		   */
		  if (trans->type == MBCLIENT_TYPE_DIALOG)
		    list_add(list, NULL, 0, p);
		}
	    }
	  else
	    {
	      /* Follow transients 'down', for a match. */

	      while (trans != NULL && trans != c)
		trans = trans->trans;
	      
	      if (trans == c)
		{
		  list_add(list, NULL, 0, p);
		}
	      else if (p->trans 
		       && c->win_group 
		       && (c->type == MBCLIENT_TYPE_APP
			   || c->type == MBCLIENT_TYPE_DESKTOP))
		{
		  /* Handle window groups and transiency. 
		   * App windows with matchbox window groups
                   * 'share' transients  
		  */
		  trans = p->trans;

		  while (trans->trans != NULL)
		    trans = trans->trans;

		  if ((trans->type == MBCLIENT_TYPE_APP
		       || trans->type == MBCLIENT_TYPE_DESKTOP)
		      && trans->win_group == c->win_group) 
		    list_add(list, NULL, 0, p);
		}

	    }
	}
    }
}


static Client*
client_get_highest_transient_recurse (Client *c, 
				      int     client_flags, 
				      Client *ignore,
				      int    *depth)
{
  Wm     *w = c->wm;
  Client *p = NULL;
  Client *highest = c, *tmp;
  int     this_depth = 0, max_depth = 0;

  /* FIXME: its likely this can be combined into
   * client_get_highest_transient() somehow.. 
  */
  stack_enumerate(w,p)
    {
      if (p != c && p->trans && p->trans == c && p != ignore)
	{
	  if (client_flags && !(p->flags & client_flags))
	    continue;

	  this_depth++;

	  tmp = client_get_highest_transient_recurse(p, 
						     client_flags, 
						     ignore, 
						     &this_depth);
	  if (this_depth > max_depth)
	    {
	      max_depth = this_depth;
	      highest = tmp;
	    }
	}
    }

  *depth += max_depth;

  return highest;
}

Client*
client_get_highest_transient(Client *c, int client_flags, Client *ignore)
{
  Wm     *w = c->wm;
  Client *p = NULL;
  Client *highest = c, *tmp;
  int     depth = 0, depth_max = 0;

  stack_enumerate(w,p)
    {
      if (p != c && p->trans && p->trans == c && p != ignore)
	{
	  depth = 0;

	  if (client_flags && !(p->flags & client_flags))
	    continue;

	  /* Recurse for each directly transient client, getting the depth 
	   * for any extra matchbox children.
	  */
	  tmp = client_get_highest_transient_recurse (p, 
						      client_flags,
						      ignore,
						      &depth);
	  if (depth >= depth_max)
	    {
	      highest = tmp;
	      depth_max = depth;
	    }
	}
    }

  return highest;
}

void
client_decor_frames_move_resize(Client *c, 
			       int     width_west, 
			       int     width_east, 
			       int     height_north,
			       int     height_south)
{
  Wm *w = c->wm;

  if (c->frames_decor[NORTH] && height_north > 0)
      XMoveResizeWindow(w->dpy, c->frames_decor[NORTH], 
			0, 0, 
			c->width + width_east + width_west, height_north);

  if (c->frames_decor[SOUTH])
      XMoveResizeWindow(w->dpy, c->frames_decor[SOUTH],
			0, height_north + c->height, 
			c->width + width_east + width_west, height_south);
 
  if (c->frames_decor[EAST])
      XMoveResizeWindow(w->dpy, c->frames_decor[EAST], 
			c->width + width_west, height_north, 
			width_east, c->height);

  if (c->frames_decor[WEST])
      XMoveResizeWindow(w->dpy, c->frames_decor[WEST], 
			0, 
			height_north, 
			width_west, 
			c->height);
}

void
client_decor_frames_init(Client *c, 
			 int     width_west, 
			 int     width_east, 
			 int     height_north,
			 int     height_south)
{
  Wm *w = c->wm;

  XSetWindowAttributes attr;
  int                  i;

  for(i=0; i<N_DECOR_FRAMES; i++)
    if (c->frames_decor[i] != None)
      {
	XDestroyWindow(w->dpy, c->frames_decor[i]);
	c->frames_decor[i] = None;
      }

  attr.override_redirect = True;
  attr.background_pixel  = w->grey_col.pixel;  
  attr.event_mask = ButtonMask;

  if (height_north > 0)
    c->frames_decor[NORTH] =
      XCreateWindow(w->dpy, 
		    c->frame, 0, 0, 
		    c->width + width_east + width_west, 
		    height_north, 0,
		    CopyFromParent, 
		    CopyFromParent, CopyFromParent,
		    CWBackPixel|CWEventMask, 
		    &attr);

  if (width_east > 0)
    c->frames_decor[EAST] = 
      XCreateWindow(w->dpy, c->frame,
		    c->width + width_west, 
		    height_north, 
		    width_east, 
		    c->height, 
		    0, CopyFromParent, CopyFromParent, CopyFromParent,
		    CWBackPixel, &attr);

  if (width_west > 0)
    c->frames_decor[WEST] = 
      XCreateWindow(w->dpy, c->frame,
		    0, 
		    height_north, 
		    width_west, 
		    c->height, 
		    0, CopyFromParent, CopyFromParent, CopyFromParent,
		    CWBackPixel, &attr);
  
  if (height_south > 0)
    c->frames_decor[SOUTH] = 
      XCreateWindow(w->dpy, c->frame,
		    0, 
		    c->height + height_north, 
		    c->width + width_east + width_west, 
		    height_south, 
		    0, CopyFromParent, CopyFromParent, CopyFromParent,
		    CWBackPixel, &attr);

}

/* Create masks used for shaped decorations */
void 
client_init_backing_mask (Client *c, 
			  int     width, 
			  int     height, 
			  int     height_north, 
			  int     height_south,
			  int     width_east, 
			  int     width_west )
{
  Wm *w = c->wm;
  GC  shape_gc;
  int i = 0;

   for (i=0; i<MSK_COUNT; i++)
     if (c->backing_masks[i] != None)
       XFreePixmap(w->dpy, c->backing_masks[i]);

  c->backing_masks[MSK_NORTH] 
    = XCreatePixmap(w->dpy, w->root, width, height_north, 1);

  shape_gc = XCreateGC( w->dpy, c->backing_masks[MSK_NORTH], 0, 0 );

  XSetForeground(w->dpy, shape_gc, 
		 WhitePixel( w->dpy, w->screen ));

  XFillRectangle(w->dpy, c->backing_masks[MSK_NORTH],
		 shape_gc, 0, 0, width, height_north);

  if (height_south)
    {
      c->backing_masks[MSK_SOUTH] 
	= XCreatePixmap(w->dpy, w->root, width, height_south, 1);

      XFillRectangle(w->dpy, c->backing_masks[MSK_SOUTH],
		     shape_gc, 0, 0, width, height_south);
    }

  if (width_east)
    {
      c->backing_masks[MSK_EAST] 
	= XCreatePixmap(w->dpy, w->root, width_east, height, 1);

      XFillRectangle(w->dpy, c->backing_masks[MSK_EAST],
		     shape_gc, 0, 0, width_east, height);
    }

  if (width_west)
    {
      c->backing_masks[MSK_WEST] 
	= XCreatePixmap(w->dpy, w->root, width_west, height, 1);

      XFillRectangle(w->dpy, c->backing_masks[MSK_WEST],
		     shape_gc, 0, 0, width_west, height);
    }

  XFreeGC(w->dpy, shape_gc);
}

void
client_button_init(Client         *c, 
		   Window          win_parent, 
		   MBClientButton *b,
		   int             x, 
		   int             y, 
		   int             width, 
		   int             height,
		   Bool            want_inputonly,
		   void           *data)
{
  Wm *w = c->wm;

  int                  class = CopyFromParent;
  XSetWindowAttributes attr;

  attr.override_redirect = True; 
  attr.event_mask        = ExposureMask;
  
  if (want_inputonly ) class = InputOnly;	      
  
  b->x = x; b->y = y; b->w = width; b->h = height; b->data = data;
  
  b->win = XCreateWindow(w->dpy, win_parent, 
			 x, y, width, height, 0,
			 CopyFromParent, 
			 class, 
			 CopyFromParent,
			 CWOverrideRedirect|CWEventMask, 
			 &attr);

  XMapWindow(w->dpy, b->win);
}

MBClientButton*
client_button_new(Client *c, 
		  Window  win_parent, 
		  int     x, 
		  int     y, 
                  int     width, 
		  int     height,
		  Bool    want_inputonly, 
		  void   *data )
{
  MBClientButton      *b = malloc(sizeof(MBClientButton));

  memset(b, 0, sizeof(MBClientButton));

  client_button_init(c, win_parent, b, 
		     x, y, width, height, 
		     want_inputonly, data);
  return b;
}

void
client_button_remove(Client *c, int button_action)
{
  Wm *w = c->wm;

  struct list_item *l = c->buttons;
  MBClientButton   *b = NULL;

  while (l != NULL)
    {
      if (l->id == button_action)
	{
	  b = (MBClientButton *)l->data;
	  dbg("%s() destroying a button ( %li ) for %s\n", __func__, 
	      b->win, c->name); 
	  XDestroyWindow(w->dpy, b->win);
	  b->win = None;
	}
      l = l->next;
    }
}

void
client_buttons_delete_all(Client *c)
{
  Wm               *w = c->wm;
  struct list_item *l = c->buttons, *p = NULL;
  MBClientButton   *b = NULL;
  
  while (l != NULL)
    {
      b = (MBClientButton *)l->data;
      dbg("%s() destroying a button\n", __func__); 
      if (b->win != None)
	XDestroyWindow(w->dpy, b->win);
      free(b);
      p = l->next;
      free(l);
      l = p;
    }

  c->buttons = NULL;
}

MBClientButton*
client_get_button_from_event(Client *c, XButtonEvent *e)
{
  struct list_item *l = c->buttons;
  MBClientButton   *b = NULL;

  while (l != NULL)
    {
      b = (MBClientButton *)l->data;
      if (b->win == e->subwindow)
	{
	  return b;
	}
      l = l->next;
    }

  return NULL;
}

struct list_item*
client_get_button_list_item_from_event(Client *c, XButtonEvent *e)
{
  struct list_item *l = c->buttons;
  MBClientButton   *b = NULL;

  while (l != NULL)
    {
      b = (MBClientButton *)l->data;
      if (b->win == e->subwindow)
	{
	  return l;
	}
      l = l->next;
    }

  return NULL;
}

int
client_button_do_ops(Client       *c, 
		     XButtonEvent *e, 
		     int           frame_type, 
		     int           width, 
		     int           height)
{
  Wm *w = c->wm;

  int               button_action;
  struct list_item *button_item = NULL;
  MBClientButton   *b = NULL;
  XEvent            ev;

  if ((button_item = client_get_button_list_item_from_event(c, e)) != NULL
       && button_item->id != -1 )
   {
     /* XXX hack hack hack - stop dubious 'invisible' text menu button 
	working when it shouldn't.....            */
     if (frame_type == FRAME_MAIN && w->flags & SINGLE_FLAG
	 && button_item->id == BUTTON_ACTION_MENU
	 && ( !wm_get_desktop(c->wm) || w->flags & DESKTOP_DECOR_FLAG))
       return -1;

     b = (MBClientButton *)button_item->data;

     if (b->press_activates)
       {
	 XUngrabPointer(w->dpy, CurrentTime); 

	 client_deliver_message(c, w->atoms[MB_GRAB_TRANSFER],
				CurrentTime, e->subwindow, 0, 0, 0);
	 return button_item->id;
       }

     if (XGrabPointer(w->dpy, e->subwindow, False,
		      ButtonPressMask|ButtonReleaseMask|
		      PointerMotionMask|EnterWindowMask|LeaveWindowMask,
		      GrabModeAsync,
		      GrabModeAsync, 
		      None, w->curs, CurrentTime) == GrabSuccess)
       {
	 Bool canceled = False;
	
	 button_action = button_item->id;

	 theme_frame_button_paint(w->mbtheme, c, button_action,
				  ACTIVE, frame_type, width, height);

	 comp_engine_client_repair (c->wm, c);
	 comp_engine_render(c->wm, w->all_damage);

	 for (;;) 
	 {
	    XMaskEvent(w->dpy,
		       ButtonPressMask|ButtonReleaseMask|
		       PointerMotionMask|EnterWindowMask|LeaveWindowMask
		       ,
		    &ev);
	    switch (ev.type)
	    {
	       case MotionNotify:
		  break;
	       case EnterNotify:
		  theme_frame_button_paint(w->mbtheme, c, button_action,
					   ACTIVE, frame_type, width, height );
		  comp_engine_client_repair (c->wm, c);
		  comp_engine_render(c->wm, w->all_damage);
		  canceled = False;
		  break;
	       case LeaveNotify:
		  theme_frame_button_paint(w->mbtheme, c, button_action,
					   INACTIVE,frame_type, width, height);
		  comp_engine_client_repair (c->wm, c);
		  comp_engine_render(c->wm, w->all_damage);
		  canceled = True;
		  break;
	       case ButtonRelease:
		  theme_frame_button_paint(w->mbtheme, c, button_action,
					   INACTIVE,frame_type, width, height);
		  XUngrabPointer(w->dpy, CurrentTime);
		  if (!canceled)
		  {
		    return button_action;
		  }
		  else return -1;  /* cancelled  */
	    }

#ifdef USE_COMPOSITE
      if (w->all_damage)
      	{
	  dbg("%s() adding damage\n", __func__);
	  comp_engine_render(c->wm, w->all_damage);
	  XFixesDestroyRegion (w->dpy, w->all_damage);
	  w->all_damage = None;
	}
#endif

	 }
       }
   }
  return 0;
}


