/* ply-trigger.c - Calls closure at later time.
 *
 * Copyright (C) 2008 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-trigger.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "ply-logger.h"
#include "ply-list.h"
#include "ply-utils.h"

typedef struct
{
  ply_trigger_handler_t  handler;
  void                  *user_data;
} ply_trigger_closure_t;

struct _ply_trigger
{
  ply_list_t *closures;

  ply_trigger_t **free_address;
  int ignore_count;
};

ply_trigger_t *
ply_trigger_new (ply_trigger_t **free_address)
{
  ply_trigger_t *trigger;

  trigger = calloc (1, sizeof (ply_trigger_t));
  trigger->free_address = free_address;
  trigger->closures = ply_list_new ();
  trigger->ignore_count = 0;

  return trigger;
}

void
ply_trigger_free (ply_trigger_t *trigger)
{
  ply_list_node_t *node;

  if (trigger == NULL)
    return;

  node = ply_list_get_first_node (trigger->closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_trigger_closure_t *closure;

      closure = (ply_trigger_closure_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (trigger->closures, node);

      free (closure);
      ply_list_remove_node (trigger->closures, node);

      node = next_node;
    }
  ply_list_free (trigger->closures);

  if (trigger->free_address != NULL)
    *trigger->free_address = NULL;

  if (trigger->free_address != NULL)
    *trigger->free_address = NULL;

  free (trigger);
}

void
ply_trigger_add_handler (ply_trigger_t         *trigger,
                         ply_trigger_handler_t  handler,
                         void                  *user_data)
{
  ply_trigger_closure_t *closure;

  closure = calloc (1, sizeof (ply_trigger_closure_t));
  closure->handler = handler;
  closure->user_data = user_data;

  ply_list_append_data (trigger->closures, closure);
}

void
ply_trigger_remove_handler (ply_trigger_t         *trigger,
                            ply_trigger_handler_t  handler,
                            void                  *user_data)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (trigger->closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_trigger_closure_t *closure;

      closure = (ply_trigger_closure_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (trigger->closures, node);

      if (closure->handler == handler && closure->user_data == user_data)
        {
          free (closure);
          ply_list_remove_node (trigger->closures, node);
          break;
        }

      node = next_node;
    }
}

void
ply_trigger_ignore_next_pull (ply_trigger_t *trigger)
{
  trigger->ignore_count++;
}

void
ply_trigger_pull (ply_trigger_t *trigger,
                  const void    *data)
{
  ply_list_node_t *node;

  assert (trigger != NULL);
  assert (trigger->ignore_count >= 0);

  if (trigger->ignore_count > 0)
    {
      trigger->ignore_count--;
      return;
    }

  node = ply_list_get_first_node (trigger->closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_trigger_closure_t *closure;

      closure = (ply_trigger_closure_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (trigger->closures, node);

      closure->handler (closure->user_data, data, trigger);

      node = next_node;
    }

  if (trigger->free_address != NULL)
    ply_trigger_free (trigger);
}
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
