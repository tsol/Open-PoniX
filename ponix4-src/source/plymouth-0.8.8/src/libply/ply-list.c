/* ply-list.c - linked list implementation
 *
 * Copyright (C) 2007 Red Hat, Inc.
 *               2008 Charlie Brej <cbrej@cs.man.ac.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by: Charlie Brej <cbrej@cs.man.ac.uk>
 *             Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-list.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

struct _ply_list
{
  ply_list_node_t *first_node;
  ply_list_node_t *last_node;

  int number_of_nodes;
};

struct _ply_list_node
{
  void   *data;
  struct _ply_list_node *previous;
  struct _ply_list_node *next;
};

ply_list_t *
ply_list_new (void)
{
  ply_list_t *list;

  list = calloc (1, sizeof (ply_list_t));

  list->first_node = NULL;
  list->last_node = NULL;
  list->number_of_nodes = 0;

  return list;
}

void
ply_list_free (ply_list_t *list)
{
  ply_list_remove_all_nodes (list);
  free (list);
}

static ply_list_node_t *
ply_list_node_new (void *data)
{
  ply_list_node_t *node;

  node = calloc (1, sizeof (ply_list_node_t));
  node->data = data;

  return node;
}

static void
ply_list_node_free (ply_list_node_t *node)
{
  if (node == NULL)
    return;

  assert ((node->previous == NULL) && (node->next == NULL));

  free (node);
}

int
ply_list_get_length (ply_list_t *list)
{
  return list->number_of_nodes;
}

ply_list_node_t *
ply_list_find_node (ply_list_t *list,
                    void       *data)
{
  ply_list_node_t *node;

  node = list->first_node;
  while (node != NULL)
    {
      if (node->data == data)
        break;

      node = node->next;
    }
  return node;
}

static void
ply_list_insert_node (ply_list_t      *list,
                      ply_list_node_t *node_before,
                      ply_list_node_t *new_node)
{

  if (new_node == NULL)
    return;

  if (node_before == NULL)
    {
      if (list->first_node == NULL)
        {
          assert (list->last_node == NULL);

          list->first_node = new_node;
          list->last_node = new_node;
        }
      else
        {
          list->first_node->previous = new_node;
          new_node->next = list->first_node;
          list->first_node = new_node;
        }
    }
  else
    {
      new_node->next = node_before->next;
      if (node_before->next != NULL)
        node_before->next->previous = new_node;
      node_before->next = new_node;
      new_node->previous = node_before;

      if (node_before == list->last_node)
        list->last_node = new_node;
    }

  list->number_of_nodes++;
}

ply_list_node_t *
ply_list_insert_data (ply_list_t      *list,
                      void            *data,
                      ply_list_node_t *node_before)
{
  ply_list_node_t *node;

  node = ply_list_node_new (data);

  ply_list_insert_node (list, node_before, node);

  return node;
}

ply_list_node_t *
ply_list_append_data (ply_list_t *list,
                      void       *data)
{
  return ply_list_insert_data (list, data, list->last_node);
}

ply_list_node_t *
ply_list_prepend_data (ply_list_t *list,
                       void       *data)
{
  return ply_list_insert_data (list, data, NULL);
}

void
ply_list_remove_data (ply_list_t *list,
                      void       *data)
{
  ply_list_node_t *node;

  if (data == NULL)
    return;

  node = ply_list_find_node (list, data);

  if (node != NULL)
    ply_list_remove_node (list, node);
}

static void
ply_list_unlink_node (ply_list_t      *list,
                      ply_list_node_t *node)
{
  ply_list_node_t *node_before, *node_after;

  assert (list != NULL);

  if (node == NULL)
    return;

  node_before = node->previous;
  node_after = node->next;

  if (node_before != NULL)
    node_before->next = node_after;

  if (node_after != NULL)
    node_after->previous = node_before;

  if (list->first_node == node)
    list->first_node = node_after;

  if (list->last_node == node)
    list->last_node = node_before;

  node->previous = NULL;
  node->next = NULL;

  list->number_of_nodes--;
  assert (ply_list_find_node (list, node->data) != node);
}

void
ply_list_remove_node (ply_list_t      *list,
                      ply_list_node_t *node)
{
  ply_list_unlink_node (list, node);
  ply_list_node_free (node);
}

void
ply_list_remove_all_nodes (ply_list_t *list)
{
  ply_list_node_t *node;

  if (list == NULL)
    return;

  node = list->first_node;
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      next_node = node->next;
      ply_list_remove_node (list, node);
      node = next_node;
    }
}

ply_list_node_t *
ply_list_get_first_node (ply_list_t *list)
{
  return list->first_node;
}

ply_list_node_t *
ply_list_get_last_node (ply_list_t *list)
{
  return list->last_node;
}

ply_list_node_t *
ply_list_get_nth_node (ply_list_t *list,
                       int         index)
{
  ply_list_node_t *node;
  node = list->first_node;
  if (index < 0)
    return NULL;
  if (index >= list->number_of_nodes)
    return NULL;
  while (index--)
    {
      node = node->next;
    }
  return node;
}

ply_list_node_t *
ply_list_get_next_node (ply_list_t     *list,
                       ply_list_node_t *node)
{
  return node->next;
}

static void 
ply_list_sort_swap (void **element_a,
                    void **element_b)
{
  void *temp;
  temp = *element_a;
  *element_a = *element_b;
  *element_b = temp;
}

static void 
ply_list_sort_body (ply_list_node_t         *node_start,
                    ply_list_node_t         *node_end,
                    ply_list_compare_func_t *compare)
{
  if (node_start == node_end) return;
  ply_list_node_t *cur_node = node_start;
  ply_list_node_t *top_node = node_end;
  ply_list_node_t *next_node = cur_node->next;
  while (cur_node != top_node)
    {
      int diff = compare(cur_node->data, next_node->data);
      if (diff > 0)
        {
          ply_list_sort_swap (&next_node->data,
                              &cur_node->data);
          cur_node = next_node;
          next_node = cur_node->next;
        }
      else
        {
          ply_list_sort_swap (&next_node->data,
                              &top_node->data);
          top_node = top_node->previous;
        }
    }

  if (cur_node != node_end)
    ply_list_sort_body (cur_node->next,
                        node_end,
                        compare);
  if (cur_node != node_start)
    ply_list_sort_body (node_start,
                        cur_node->previous,
                        compare);
}

void
ply_list_sort (ply_list_t              *list,
               ply_list_compare_func_t *compare)
{
  ply_list_sort_body (ply_list_get_first_node (list),
                      ply_list_get_last_node (list),
                      compare);
}

void
ply_list_sort_stable (ply_list_t              *list,
                      ply_list_compare_func_t *compare)
{
  ply_list_node_t *top_node;
  ply_list_node_t *cur_node;

  top_node = ply_list_get_first_node (list);
  if (top_node == NULL) return;
  top_node = top_node->next;

  while (top_node)
    {
      cur_node = top_node->previous;
      while (cur_node && compare(cur_node->data, cur_node->next->data) > 0)
        {
          ply_list_sort_swap (&cur_node->data,
                              &cur_node->next->data);
          cur_node = cur_node->previous;
        }
      top_node = top_node->next;
    }
  
}

void *
ply_list_node_get_data (ply_list_node_t *node)
{
  return node->data;
}

#ifdef PLY_LIST_ENABLE_TEST
#include <stdio.h>

static int
compare_int_ptr (void *element_a,
                 void *element_b)
{
  int *int_a = element_a;
  int *int_b = element_b;
  return *int_a - *int_b;

}

int
main (int    argc,
      char **argv)
{
  ply_list_t *list;
  ply_list_node_t *node;
  int i, lastval;
  int *value;
  int errors;

  errors = 0;

  list = ply_list_new ();

  ply_list_append_data (list, (void *) "foo");
  ply_list_append_data (list, (void *) "bar");
  ply_list_append_data (list, (void *) "baz");
  ply_list_prepend_data (list, (void *) "qux");
  ply_list_prepend_data (list, (void *) "quux");
  ply_list_remove_data (list, (void *) "baz");
  ply_list_remove_data (list, (void *) "foo");

  node = ply_list_get_first_node (list);
  i = 0;
  while (node != NULL)
    {
      printf ("node '%d' has data '%s'\n", i,
              (char *) ply_list_node_get_data (node));
      node = ply_list_get_next_node (list, node);
      i++;
    }

  printf ("\n");
  ply_list_remove_all_nodes (list);
  srandom(1);

  for (i = 0; i<100; i++)
    {
      value = malloc (sizeof (int));
      *value = random() % 100;
      ply_list_append_data (list, (void *) value);
    }

  ply_list_sort (list, compare_int_ptr);

  node = ply_list_get_first_node (list);
  i = 0;
  lastval = 0;

  while (node != NULL)
    {
      value = (int *) ply_list_node_get_data (node);
      if (*value < lastval)
        {
          printf ("ERROR: incorrect order\n");
          errors = 1;
        }
      lastval = *value;
      printf ("node '%d' has data '%d'\n", i, *value);
      node = ply_list_get_next_node (list, node);
      i++;
    }

  ply_list_free (list);
  return errors;
}

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
