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

#ifndef _MBLIST_H_
#define _MBLIST_H_

#include "structs.h"



struct list_item
{
  char* name;
  int   id;
  void* data;
  struct list_item* next;
};

#define list_enumerate(l,i) for((i)=(l);(i);(i)=(i)->next)

#define list_get_tail(l) while ((l)->next != NULL) (l)=(l)->next; 

struct list_item* list_new(int id, char *name, void *data);

void* list_add(struct list_item** head, char *name, int id, void *data);

void* list_find_by_id(struct list_item* head, int needed_id);

void* list_find_by_name(struct list_item* head, char *name);

void list_remove(struct list_item** head, void *data);

void list_destroy(struct list_item** head);

#endif
