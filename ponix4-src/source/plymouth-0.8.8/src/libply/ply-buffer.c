/* ply-buffer.c - facilities for buffering data
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
#include "ply-buffer.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ply-utils.h"

#ifndef PLY_BUFFER_MAX_APPEND_SIZE
#define PLY_BUFFER_MAX_APPEND_SIZE 1024
#endif

#ifndef PLY_BUFFER_MAX_BUFFER_CAPACITY
#define PLY_BUFFER_MAX_BUFFER_CAPACITY (255 * 4096)
#endif

struct _ply_buffer
{
  char   *data;
  size_t  size;
  size_t  capacity;
};

static bool
ply_buffer_increase_capacity (ply_buffer_t *buffer)
{
  assert (buffer != NULL);

  if ((buffer->capacity * 2) > PLY_BUFFER_MAX_BUFFER_CAPACITY)
    return false;

  buffer->capacity *= 2;

  buffer->data = realloc (buffer->data, buffer->capacity);
  return true;
}

void
ply_buffer_remove_bytes (ply_buffer_t *buffer,
                         size_t        bytes_to_remove)
{
  assert (buffer != NULL);

  bytes_to_remove = MIN (buffer->size, bytes_to_remove);

  if (bytes_to_remove == buffer->size)
    buffer->size = 0;
  else
    {
      memmove (buffer->data, buffer->data + bytes_to_remove,
               buffer->size - bytes_to_remove);
      buffer->size -= bytes_to_remove;
    }
  buffer->data[buffer->size] = '\0';
}

void
ply_buffer_remove_bytes_at_end (ply_buffer_t *buffer,
                                size_t        bytes_to_remove)
{
  assert (buffer != NULL);

  bytes_to_remove = MIN (buffer->size, bytes_to_remove);

  buffer->size -= bytes_to_remove;
  buffer->data[buffer->size] = '\0';
}

ply_buffer_t *
ply_buffer_new (void)
{
  ply_buffer_t *buffer;

  buffer = calloc (1, sizeof (ply_buffer_t));

  buffer->capacity = 4096;
  buffer->data = calloc (1, buffer->capacity);
  buffer->size = 0;

  return buffer;
}

void
ply_buffer_free (ply_buffer_t *buffer)
{
  if (buffer == NULL)
    return;

  free (buffer->data);
  free (buffer);
}

static bool
ply_buffer_validate_format_string (ply_buffer_t   *buffer,
                                   const char     *format)
{
  char *n, *p;

  p = (char *) format;

  /* lame checks to limit the damage
   * of some potential exploits.
   */
  while ((n = strstr (p, "%n")) != NULL)
    {
      if (n == format)
          return false;

      if (n[-1] != '%')
          return false;

      p = n + 1;
    }

  return true;
}

void
ply_buffer_append_with_non_literal_format_string (ply_buffer_t   *buffer,
                                                  const char     *format,
                                                  ...)
{
  va_list args;
  size_t string_size;
  char write_buffer[PLY_BUFFER_MAX_APPEND_SIZE] = "";

  assert (buffer != NULL);

  if (!ply_buffer_validate_format_string (buffer, format))
    return;

  va_start (args, format);
  string_size = vsnprintf (write_buffer, 0, format, args) + 1;
  va_end (args);

  if (string_size > PLY_BUFFER_MAX_APPEND_SIZE)
    return;

  va_start (args, format);
  vsnprintf (write_buffer, PLY_BUFFER_MAX_APPEND_SIZE,
             format, args);
  va_end (args);

  ply_buffer_append_bytes (buffer, write_buffer, string_size - 1);
}

void
ply_buffer_append_bytes (ply_buffer_t *buffer,
                         const void   *bytes_in,
                         size_t        length)
{
  assert (buffer != NULL);
  assert (bytes_in != NULL);
  assert (length != 0);
  
  const uint8_t *bytes = bytes_in;
  
  if (length >PLY_BUFFER_MAX_BUFFER_CAPACITY){
    bytes += length - (PLY_BUFFER_MAX_BUFFER_CAPACITY-1);
    length = (PLY_BUFFER_MAX_BUFFER_CAPACITY-1);
    }
  
  while ((buffer->size + length) >= buffer->capacity)
    {
      if (!ply_buffer_increase_capacity (buffer))
        {
          ply_buffer_remove_bytes (buffer, length);
        }
    }

  assert (buffer->size + length < buffer->capacity);

  memcpy (buffer->data + buffer->size,
          bytes, length);

  buffer->size += length;
  buffer->data[buffer->size] = '\0';
}

void
ply_buffer_append_from_fd (ply_buffer_t *buffer,
                           int           fd)
{

  char bytes[PLY_BUFFER_MAX_APPEND_SIZE] = "";
  ssize_t bytes_read;

  assert (buffer != NULL);
  assert (fd >= 0);

  if (!ply_fd_has_data (fd))
    return;

  bytes_read = read (fd, bytes, sizeof (bytes));

  if (bytes_read > 0)
    ply_buffer_append_bytes (buffer, bytes, bytes_read);
}

const char *
ply_buffer_get_bytes (ply_buffer_t *buffer)
{
  assert (buffer != NULL);
  return buffer->data;
}

char *
ply_buffer_steal_bytes (ply_buffer_t *buffer)
{
  char *bytes;
  assert (buffer != NULL);

  bytes = buffer->data;

  buffer->data = calloc (1, buffer->capacity);
  buffer->size = 0;

  return bytes;
}

size_t
ply_buffer_get_size (ply_buffer_t *buffer)
{
  return buffer->size;
}

void
ply_buffer_clear (ply_buffer_t *buffer)
{
  memset (buffer->data, '\0', buffer->capacity);
  buffer->size = 0;
}

#ifdef PLY_BUFFER_ENABLE_TEST
int
main (int    argc,
      char **argv)
{
  int exit_code;
  ply_buffer_t *buffer;

  exit_code = 0;
  buffer = ply_buffer_new ();

  ply_buffer_append (buffer, "yo yo yo\n");
  ply_buffer_free (buffer);

  return exit_code;
}

#endif /* PLY_BUFFER_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
