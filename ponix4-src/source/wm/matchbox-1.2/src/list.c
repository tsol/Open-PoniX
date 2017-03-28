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

#include "list.h"

struct list_item*
list_new(int id, char *name, void *data)
{
  struct list_item* list;
  list = malloc(sizeof(struct list_item));
  memset(list, 0, sizeof(struct list_item));

  if (name) list->name = strdup(name);
  if (id)   list->id   = id;
  list->data = data;
  list->next = NULL;

  return list;
}

void*
list_add(struct list_item** head, char *name, int id, void *data)
{
  struct list_item* tmp = *head;

  if (tmp == NULL) 
    {
      *head = list_new(id, name, data);
      return NULL;
    }
  
  while (tmp->next != NULL) tmp = tmp->next;
  tmp->next = list_new(id, name, data);

  return tmp->next->data;
}


void*
list_find_by_id(struct list_item* head, int needed_id)
{
  struct list_item* tmp = head;
  
  while (tmp != NULL) 
    {
      if (tmp->id && tmp->id == needed_id) return tmp->data;
      tmp = tmp->next;
    }


  return NULL;
}

void*
list_find_by_name(struct list_item* head, char *name)
{
  struct list_item* tmp = head;
  
  while (tmp != NULL) 
    {
      if (tmp->name && name && !strcmp(tmp->name, name)) return tmp->data;
      tmp = tmp->next;
    }

  return NULL;
}

void
list_remove(struct list_item** head, void *data)
{
  struct list_item* cur = *head, *prev = NULL;

  if (cur && cur->next == NULL && cur->data == data)
    {
      list_destroy(head);
      return;
    }
  
  while (cur != NULL) 
    {
      if (cur->data == data)
	{
	  if (prev)
	    prev->next = cur->next;
	  else
	    *head = cur->next;
	    
	  if (cur->name) free (cur->name);
	  free(cur);
	  return;
	}
      prev = cur;
      cur = cur->next;
    }

}

void
list_destroy(struct list_item** head)
{
  struct list_item* next = NULL, *cur = *head;
  while (cur != NULL)
    {
      next = cur->next;
      if (cur->name) free (cur->name);
      free(cur);
      cur = next;
    }
  *head = NULL;
}
  
