/* ply-array.c - linked array implementation
 *
 * Copyright (C) 2008 Red Hat, Inc.
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
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-array.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "ply-buffer.h"

static const void *pointer_terminator = NULL;
static const uint32_t uint32_terminator = 0;

struct _ply_array
{
  ply_buffer_t *buffer;
  ply_array_element_type_t element_type;
};

ply_array_t *
ply_array_new (ply_array_element_type_t element_type)
{
  ply_array_t *array;

  array = calloc (1, sizeof (ply_array_t));

  array->buffer = ply_buffer_new ();
  array->element_type = element_type;

  switch (array->element_type)
    {
      case PLY_ARRAY_ELEMENT_TYPE_POINTER:
        ply_buffer_append_bytes (array->buffer, &pointer_terminator, sizeof (pointer_terminator));
      break;

      case PLY_ARRAY_ELEMENT_TYPE_UINT32:
        ply_buffer_append_bytes (array->buffer, &uint32_terminator, sizeof (uint32_terminator));
      break;
    }

  return array;
}

void
ply_array_free (ply_array_t *array)
{
  if (array == NULL)
    return;

  ply_buffer_free (array->buffer);

  free (array);
}

int
ply_array_get_size (ply_array_t *array)
{
  int size;

  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_POINTER ||
          array->element_type == PLY_ARRAY_ELEMENT_TYPE_UINT32);

  switch (array->element_type)
    {
      case PLY_ARRAY_ELEMENT_TYPE_POINTER:
        size = (ply_buffer_get_size (array->buffer) / sizeof (const void *)) - 1;
      break;

      case PLY_ARRAY_ELEMENT_TYPE_UINT32:
        size = (ply_buffer_get_size (array->buffer) / sizeof (const uint32_t)) - 1;
      break;
    }

  return size;
}

void
ply_array_add_pointer_element (ply_array_t *array,
                               const void  *data)
{
  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_POINTER);

  /* Temporarily remove NULL terminator
   */
  ply_buffer_remove_bytes_at_end (array->buffer, sizeof (pointer_terminator));

  ply_buffer_append_bytes (array->buffer, &data, sizeof (const void *));

  /* Add NULL terminator back
   */
  ply_buffer_append_bytes (array->buffer, &pointer_terminator, sizeof (pointer_terminator));
}

void
ply_array_add_uint32_element (ply_array_t    *array,
                              const uint32_t  data)
{
  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_UINT32);

  /* Temporarily remove 0 terminator
   */
  ply_buffer_remove_bytes_at_end (array->buffer, sizeof (uint32_terminator));

  ply_buffer_append_bytes (array->buffer, &data, sizeof (const uint32_t));

  /* Add 0 terminator back
   */
  ply_buffer_append_bytes (array->buffer, &uint32_terminator, sizeof (uint32_terminator));
}

void * const *
ply_array_get_pointer_elements (ply_array_t *array)
{
  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_POINTER);
  return (void * const *) ply_buffer_get_bytes (array->buffer);
}

uint32_t const *
ply_array_get_uint32_elements (ply_array_t *array)
{
  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_UINT32);
  return (uint32_t const *) ply_buffer_get_bytes (array->buffer);
}

void **
ply_array_steal_pointer_elements (ply_array_t *array)
{
  void **data;

  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_POINTER);

  data = (void **) ply_buffer_steal_bytes (array->buffer);

  ply_buffer_append_bytes (array->buffer, &pointer_terminator, sizeof (const void *));

  return data;
}

uint32_t *
ply_array_steal_uint32_elements (ply_array_t *array)
{
  uint32_t *data;

  assert (array->element_type == PLY_ARRAY_ELEMENT_TYPE_UINT32);

  data = (uint32_t *) ply_buffer_steal_bytes (array->buffer);

  ply_buffer_append_bytes (array->buffer, &uint32_terminator, sizeof (const uint32_t));

  return data;
}

#ifdef PLY_ARRAY_ENABLE_TEST
#include <stdio.h>

int
main (int    argc,
      char **argv)
{
  ply_array_t *array;
  int i;
  char **data;

  array = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_POINTER);

  ply_array_add_pointer_element (array, "foo");
  ply_array_add_pointer_element (array, "bar");
  ply_array_add_pointer_element (array, "baz");
  ply_array_add_pointer_element (array, "qux");

  data = (char **) ply_array_get_pointer_elements (array);
  for (i = 0; data[i] != NULL; i++)
    {
      printf ("element '%d' has data '%s'\n", i, data[i]);
      i++;
    }

  ply_array_free (array);
  return 0;
}

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
