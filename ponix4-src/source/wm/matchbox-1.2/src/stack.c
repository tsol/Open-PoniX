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

#include "stack.h"

void
stack_add_above_client(Client *client, Client *client_below)
{
  Wm *w = client->wm;

  if (client_below == NULL)
    {
      /* NULL so nothing below add at bottom */
      if (w->stack_bottom) 
	{
	  client->above = w->stack_bottom;
	  w->stack_bottom->below = client;
	}

      w->stack_bottom = client;
    }
  else
    {
      client->below = client_below;
      client->above = client_below->above;
      if (client->below) client->below->above = client;
      if (client->above) client->above->below = client;


    }

  if (client_below == w->stack_top)
    w->stack_top = client;

  w->stack_n_items++;
}


void   /* can this call above client ? */
stack_append_top(Client *client)
{
  Wm *w = client->wm;

  stack_add_above_client(client, w->stack_top);
}

void   /* can this call above client ? */
stack_prepend_bottom(Client *client)
{
  stack_add_above_client(client, NULL);
}

void
stack_remove(Client *client)
{
  Wm *w = client->wm;

  if (w->stack_top == w->stack_bottom)
    {
      w->stack_top = w->stack_bottom = NULL;
    }
  else
    {
      if (client == w->stack_top)
	w->stack_top = client->below;

      if (client == w->stack_bottom)
	w->stack_bottom = client->above;

      if (client->below != NULL) client->below->above = client->above;
      if (client->above != NULL) client->above->below = client->below;
    }

  client->above = client->below = NULL;

  w->stack_n_items--;
}

void
stack_move_client_above_type(Client *client, int type_below)
{
  Wm     *w = client->wm;
  Client *highest_client = NULL, *c = NULL;

  stack_enumerate(w,c)
    {
      if ((c->type & type_below) && c->mapped)
	highest_client = c;
    }

  if (highest_client)
    stack_move_above_client(client, highest_client);
} 

void
stack_move_above_client(Client *client, Client *client_below)
{
  if (client == client_below) return;

  stack_remove(client);
  stack_add_above_client(client, client_below);
}

void
stack_get_type_list(Wm *w, MBList **list, MBClientTypeEnum  wanted_type)
{
  Client *p = NULL;

  stack_enumerate(w,p)
    if (p->type & wanted_type)
      list_add(list, NULL, 0, p);
}

void
stack_move_type_above_client(Wm               *w, 
			     MBClientTypeEnum  wanted_type, 
			     Client           *client)
{
  MBList *list = NULL, *list_item = NULL;

  stack_get_type_list(w, &list, wanted_type);

  list_enumerate(list, list_item)
    {
      Client *cur = (Client *)list_item->data;
      if (cur->mapped) 
	stack_move_above_client(cur, client);
    }

  list_destroy(&list);
  
}


void
stack_move_transients_to_top(Wm *w, Client *client_trans_for, int flags)
{
  /* This function shifts a clients transients to the top
   * of the stack keeping there respective order.   
   *
   */

  MBList *transient_list = NULL, *list_item = NULL;

  client_get_transient_list(w, &transient_list, client_trans_for);
      
  list_enumerate(transient_list, list_item)
    {
      Client *cur = (Client *)list_item->data;
      
      if (flags && !(cur->flags & flags))
	continue;

      dbg("%s() moving %s to top (trans for %s)\n", 
	  __func__, cur->name, 
	  client_trans_for ? client_trans_for->name : "NULL" );

      stack_move_top(cur);
    }

  list_destroy(&transient_list);
}

/* returns top -> bottom */
Window*
stack_get_window_list(Wm *w)
{
  Window *win_list;
  Client *c;
  int     i = 0;

  if (!w->stack_n_items) return NULL;

  win_list = malloc(sizeof(Window)*(w->stack_n_items+w->n_modal_blocker_wins));

  dbg("%s() called, list is ", __func__);

  stack_enumerate_reverse(w, c)
  {
    dbg("%i:%li, ", i, c->frame);

    win_list[i++] = c->frame;

    if (c->win_modal_blocker)
      {
	dbg("%i: <blocker>,", i );
	win_list[i++] = c->win_modal_blocker;
      }

  }

  dbg("\n");

  return win_list;
}

/* returns the new highest wanted type */
Client*
stack_cycle_forward(Wm *w, MBClientTypeEnum type_to_cycle)
{
  Client *c = NULL;

  c = stack_get_highest(w, type_to_cycle);

  /* Move to stack bottom   */
  if (c) stack_move_above_client(c, NULL);

  /* now return the new highest */
  return stack_get_highest(w, type_to_cycle);
}

/* returns the new highest wanted type */
Client*
stack_cycle_backward(Wm *w, MBClientTypeEnum type_to_cycle)
{
  Client *c = NULL;

  c = stack_get_lowest(w, type_to_cycle);

  if (c) stack_move_client_above_type(c, type_to_cycle);

  return c;
}

Client*
stack_get_highest(Wm *w, MBClientTypeEnum wanted_type)
{
  Client *c = NULL;

  stack_enumerate_reverse(w,c)
    if (c->type == wanted_type && c->mapped)
      return c;

  return NULL;
}



Client*
stack_get_lowest(Wm *w, MBClientTypeEnum wanted_type)
{
  Client *c = NULL;

  stack_enumerate(w,c)
    if (c->type == wanted_type && c->mapped)
      return c;

  return NULL;
}

Client*
stack_get_above(Client* client_below, MBClientTypeEnum wanted_type)
{
  Wm     *w = client_below->wm;
  Client *c = client_below->above;

  if (wanted_type == MBCLIENT_TYPE_ANY)
    return (client_below->above) ? client_below->above : w->stack_bottom; 

  while ( c != client_below )
    {
      if (c == NULL)
	c = w->stack_bottom;

      if (c->type == wanted_type && c->mapped)
	return c;

      c = c->above;
    }

  return client_below;
}

/* Returns client_above if nothing below */
Client*
stack_get_below(Client*          client_above, 
		MBClientTypeEnum wanted_type)
{
  Wm     *w = client_above->wm;
  Client *c = client_above->below;

  while ( c != client_above )
    {
      if (c == NULL)
	c = w->stack_top;

      if (c->type == wanted_type && c->mapped)
	return c;

      c = c->below;
    }

  return client_above;
}




void
stack_dump(Wm *w)
{
#ifdef DEBUG
  Client *c = NULL;

  printf("\n----------------------------------------------------------\n");

  stack_enumerate_reverse(w,c)
    {
      printf("%s:: modal: %s, group: %li, geom: %ix%i+%i+%i%s\n", 
	     c->name ? c->name : "Unkown", 
	     c->flags & CLIENT_IS_MODAL_FLAG ? "yes" : "no",
	     c->win_group,
	     c->width, c->height, c->x, c->y,
	     w->focused_client == c ? " [FOCUSED] " : "");
      if (c->type == MBCLIENT_TYPE_DIALOG)
	printf("   \\ trans for '%s'\n", c->trans ? c->trans->name : "root");
    }

  printf("\n---------------------------------------------------------\n\n");
#endif
}

void
stack_sync_to_display(Wm *w)
{
  /*

    int XRestackWindows(Display *display, Window windows[], int nwindows);

    The XRestackWindows function restacks the windows in the order speci-
    fied, from top to bottom.  The stacking order of the first window in
    the windows array is unaffected, but the other windows in the array are
    stacked underneath the first window, in the order of the array.  The
    stacking order of the other windows is not affected.  For each window
    in the window array that is not a child of the specified window, a Bad-
    Match error results.


  */

  Window *win_list = stack_get_window_list(w);
  
  if (win_list)
    {
      misc_trap_xerrors();
      XRestackWindows(w->dpy, win_list, 
		      w->stack_n_items + w->n_modal_blocker_wins);
      free(win_list);
      misc_untrap_xerrors();
    }

}

#if STACK_STUFF_DEPRECIATED

void
stack_add_below_client(Client *client, Client *client_above);

void
stack_move_type_below_client(MBClientTypeEnum wanted_type, Client *client);

void
stack_move_below_type(Client *client, int type, int flags);

void
stack_move_below_client(Client *client, Client *client_above);

void
stack_update_transients(Client *client, Client *client_below);

void
stack_move_above_extended(Client *client,       
			  Client *client_below,
			  int     type, 
			  int     flags);

void
stack_add_below_client(Client *client, Client *client_above)
{
  Wm *w = client->wm;

  if (client_above == NULL)
    {
      /* nothing above raising to top */
      if (w->stack_top)
	{
	  client->below = w->stack_top;
	  w->stack_top->below = client;
	}

      w->stack_top = client;
    }
  else
    {
      client->above = client_above;
      client->below = client_above->below;

      if (client_above->below) client_above->below->above = client;
      client_above->below = client;
    }

  if (client_above == w->stack_bottom)
    w->stack_bottom = client;

  w->stack_n_items++;
}

void
stack_move_type_below_client(MBClientTypeEnum wanted_type, Client *client)
{
  Wm     *w = client->wm;
  Client *c = NULL;

  stack_enumerate_reverse(w,c)
    if ((c->type & wanted_type) && c->mapped)
      stack_move_below_client(c, client);
}

void
stack_move_below_client(Client *client, Client *client_above)
{
  if (client == client_above) return;

  stack_remove(client);
  stack_add_below_client(client, client_above);

  /* must move any transients too */
}

void 				/* XXX needed ? should sync with above */
stack_move_below_type(Client *client, int type, int flags)
{
  Wm     *w = client->wm;
  Client *client_above = NULL, *client_cur;

  stack_enumerate_reverse(w, client_cur)
    {
      if (client_cur->type == type)
	{
	  if (flags)
	    {
	      if (client_cur->flags & flags)
		client_above = client_cur;
	    }
	  else client_above = client_cur;
	}
    }

  if (client_above)  /* TODO: what if this is NULL ?? */
    stack_move_below_client(client, client_above);


  /* must move any transients too */
}

void
stack_update_transients(Client *client, Client *client_below)
{
  Wm     *w = client->wm;
  Client *client_cur;

  stack_enumerate_transients(w,client_cur,client)
    {
      stack_move_above_client(client_cur, client);
    }

  stack_enumerate_transients(w,client_cur,client_below)
    {
      stack_move_below_client(client_cur, client);
    }


}

void
stack_move_above_extended(Client *client,       
			  Client *client_below,
			  int     type, 
			  int     flags)
{
  Wm     *w = client->wm;
  Client *client_found_below = NULL, *client_cur;

  if (client_below == NULL) 
    client_below = w->stack_bottom;
  else
    client_below = client_below->above;

  for (client_cur = client_below; 
       client_cur; 
       client_cur = client_cur->above)   
    {
      if (client_cur->type & type)
	{
	  if (flags)
	    {
	      if (client_cur->flags & flags)
		client_found_below = client_cur;
	    }
	  else client_found_below = client_cur;
	}
    }


  if (client_found_below)  /* TODO: what if this is NULL ?? */
    {
      dbg("%s() moving %s just above %s\n", 
	  __func__, client->name, client_found_below->name); 
      stack_move_above_client(client, client_found_below);
    }
}

#endif


#if 0

/* Test bits for stack  */

int
main(int argc, char **argv)
{
  Wm *w;
  int i = 0; 
  Client *c, *midc, *midc2; 


  w = malloc(sizeof(Wm));
  memset(w, 0, sizeof(Wm));

  for (i=0; i<10; i++)
    {
      char    buf[64];

      sprintf(buf, "Client-%i", i);
      
      c = client_new(w, buf);

      stack_append_top(c);

      if (i == 5) midc = c;
      if (i == 6) midc2 = c;
    }

  printf("w->stack_top : %s, next %p   w->stack_bottom : %s, prev %p\n", 
	 w->stack_top->name, w->stack_top->above, 
	 w->stack_bottom->name, w->stack_bottom->below);


  stack_move_below_client(w->stack_top, w->stack_bottom);
  stack_move_above_client(w->stack_bottom, w->stack_top);

  printf("w->stack_top : %s, next %p   w->stack_bottom : %s, prev %p\n", 
	 w->stack_top->name, w->stack_top->above, 
	 w->stack_bottom->name, w->stack_bottom->below);


  stack_enumerate(w,c)
    {
      printf("%s\n", c->name);
    }

  printf("\n");

  printf("\n");

  stack_remove(w->stack_top);

  stack_remove(w->stack_bottom);

  stack_enumerate(w,c)
    {
      printf("%s\n", c->name);
    }

  stack_move_below_client(w->stack_top, w->stack_bottom);
  stack_move_above_client(midc, midc2);

  printf("\n");


  stack_enumerate(w,c)
    {
      printf("%s\n", c->name);
    }

}

#endif
