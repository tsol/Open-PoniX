/* ply-logger.c - logging and tracing facilities
 *
 * Copyright (C) 2007 Red Hat, Inc.
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
#include "ply-logger.h"

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
#include "ply-list.h"

#ifndef PLY_LOGGER_OPEN_FLAGS
#define PLY_LOGGER_OPEN_FLAGS (O_WRONLY | O_TRUNC | O_CREAT | O_NOFOLLOW)
#endif

#ifndef PLY_LOGGER_MAX_INJECTION_SIZE
#define PLY_LOGGER_MAX_INJECTION_SIZE 4096
#endif

#ifndef PLY_LOGGER_MAX_BUFFER_CAPACITY
#define PLY_LOGGER_MAX_BUFFER_CAPACITY (8 * 4096)
#endif

typedef struct
{
  ply_logger_filter_handler_t  handler;
  void                        *user_data;
} ply_logger_filter_t;

struct _ply_logger
{
  int output_fd;
  char *filename;

  char *buffer;
  size_t buffer_size;
  size_t buffer_capacity;

  ply_logger_flush_policy_t flush_policy;
  ply_list_t *filters;

  uint32_t is_enabled : 1;
  uint32_t tracing_is_enabled : 1;
};

static bool ply_text_is_loggable (const char *string,
                                  ssize_t     length);
static void ply_logger_write_exception (ply_logger_t   *logger,
                                        const char     *string);
static bool ply_logger_write (ply_logger_t   *logger,
                              const char     *string,
                              size_t          length,
                              bool            should_report_failures);

static bool ply_logger_buffer (ply_logger_t   *logger,
                               const char     *string,
                               size_t          length);
static bool ply_logger_flush_buffer (ply_logger_t *logger);

static bool
ply_text_is_loggable (const char *string,
                      ssize_t     length)
{
  /* I guess we should let everything through since there
   * isn't really any specified encoding
   */

  return true;
}

static void
ply_logger_write_exception (ply_logger_t   *logger,
                            const char     *string)
{
  char *message;
  int number_of_bytes;

  if (!ply_text_is_loggable (string, -1))
    return;

  message = NULL;
  asprintf (&message,
            "[couldn't write a log entry: %s]\n%n",
            string, &number_of_bytes);

  assert (message != NULL);

  ply_logger_write (logger, message, number_of_bytes, false);
  free (message);
}

static bool
ply_logger_write (ply_logger_t *logger,
                  const char   *string,
                  size_t        length,
                  bool          should_report_failures)
{
  if (!ply_text_is_loggable (string, length))
    {
      if (should_report_failures)
        ply_logger_write_exception (logger,
                                    "log text contains unloggable bytes");
      /* we return true here, because returning false would mean
       * "you aren't allowed to write to the log file anymore"
       */
      return true;
    }

  if (!ply_write (logger->output_fd, string, length))
    {
      if (should_report_failures)
        ply_logger_write_exception (logger, strerror (errno));

      return false;
    }

  return true;
}

static bool
ply_logger_flush_buffer (ply_logger_t *logger)
{
  assert (logger != NULL);

  if (logger->buffer_size == 0)
    return true;

  if (!ply_logger_write (logger, logger->buffer, logger->buffer_size, true))
    return false;

  memset (logger->buffer, '\0', logger->buffer_size);
  logger->buffer_size = 0;

  return true;
}

static bool
ply_logger_increase_buffer_size (ply_logger_t *logger)
{
  assert (logger != NULL);

  if ((logger->buffer_capacity * 2) > PLY_LOGGER_MAX_BUFFER_CAPACITY)
    return false;

  logger->buffer_capacity *= 2;

  logger->buffer = realloc (logger->buffer, logger->buffer_capacity);
  return true;
}

static void
ply_logger_decapitate_buffer (ply_logger_t *logger,
                              size_t        bytes_in_head)
{
  assert (logger != NULL);

  bytes_in_head = MIN (logger->buffer_size, bytes_in_head);

  if (bytes_in_head == logger->buffer_size)
    logger->buffer_size = 0;
  else
    {
      memmove (logger->buffer, logger->buffer + bytes_in_head,
               logger->buffer_size - bytes_in_head);
      logger->buffer_size -= bytes_in_head;
    }
}

static bool
ply_logger_buffer (ply_logger_t *logger,
                   const char   *string,
                   size_t        length)
{
  assert (logger != NULL);

  if ((logger->buffer_size + length) >= logger->buffer_capacity)
    {
      if (!ply_logger_increase_buffer_size (logger))
        {
          ply_logger_decapitate_buffer (logger, length);

          if ((logger->buffer_size + length) >= logger->buffer_capacity)
            if (!ply_logger_increase_buffer_size (logger))
              return false;
        }
    }

  assert (logger->buffer_size + length < logger->buffer_capacity);

  memcpy (logger->buffer + logger->buffer_size,
          string, length);

  logger->buffer_size += length;

  return true;
}

ply_logger_t *
ply_logger_new (void)
{
  ply_logger_t *logger;

  logger = calloc (1, sizeof (ply_logger_t));

  logger->output_fd = -1;
  logger->filename = NULL;
  logger->is_enabled = true;
  logger->tracing_is_enabled = false;

  logger->buffer_capacity = 4096;
  logger->buffer = calloc (1, logger->buffer_capacity);
  logger->buffer_size = 0;

  logger->filters = ply_list_new ();

  return logger;
}

ply_logger_t *
ply_logger_get_default (void)
{
  static ply_logger_t *logger = NULL;

  if (logger == NULL)
    {
      logger = ply_logger_new ();
      ply_logger_set_output_fd (logger, STDOUT_FILENO);
    }

  return logger;
}

ply_logger_t *
ply_logger_get_error_default (void)
{
  static ply_logger_t *logger = NULL;

  if (logger == NULL)
    {
      logger = ply_logger_new ();
      ply_logger_set_output_fd (logger, STDERR_FILENO);
      ply_logger_set_flush_policy (logger,
                                   PLY_LOGGER_FLUSH_POLICY_EVERY_TIME);
    }

  return logger;
}

static void
ply_logger_free_filters (ply_logger_t *logger)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (logger->filters);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_logger_filter_t *filter;

      filter = (ply_logger_filter_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (logger->filters, node);
      free (filter);
      node = next_node;
    }

  ply_list_free (logger->filters);
}

void
ply_logger_free (ply_logger_t *logger)
{
  if (logger == NULL)
    return;

  if (logger->output_fd >= 0)
    {
      if (ply_logger_is_logging (logger))
        ply_logger_flush (logger);
      close (logger->output_fd);
    }

  ply_logger_free_filters (logger);

  free (logger->filename);
  free (logger->buffer);
  free (logger);
}

bool
ply_logger_open_file (ply_logger_t    *logger,
                      const char      *filename,
                      bool             world_readable)
{
  int fd;
  mode_t mode;

  assert (logger != NULL);
  assert (filename != NULL);

  if (world_readable)
    mode = 0644;
  else
    mode = 0600;

  fd = open (filename, PLY_LOGGER_OPEN_FLAGS, mode);

  if (fd < 0)
    return false;

  if (fchmod (fd, mode) < 0) {
    close (fd);
    return false;
  }

  ply_logger_set_output_fd (logger, fd);

  free (logger->filename);

  logger->filename = strdup (filename);

  return true;
}

void
ply_logger_close_file (ply_logger_t *logger)
{
  assert (logger != NULL);

  close (logger->output_fd);
  ply_logger_set_output_fd (logger, -1);
}

void
ply_logger_set_output_fd (ply_logger_t *logger,
                          int           fd)
{
  assert (logger != NULL);

  logger->output_fd = fd;
}

int
ply_logger_get_output_fd (ply_logger_t *logger)
{
  assert (logger != NULL);

  return logger->output_fd;
}

bool
ply_logger_flush (ply_logger_t *logger)
{
  assert (logger != NULL);

  if (!ply_logger_is_logging (logger))
    return false;

  if (logger->output_fd < 0)
    return false;

  if (!ply_logger_flush_buffer (logger))
    return false;

#ifdef SYNC_ON_FLUSH
  if ((fdatasync (logger->output_fd) < 0) &&
      ((errno != EROFS) && (errno != EINVAL)))
    return false;
#endif

  return true;
}

void
ply_logger_set_flush_policy (ply_logger_t              *logger,
                             ply_logger_flush_policy_t  policy)
{
  assert (logger != NULL);

  logger->flush_policy = policy;
}

ply_logger_flush_policy_t
ply_logger_get_flush_policy (ply_logger_t *logger)
{
  assert (logger != NULL);

  return logger->flush_policy;
}

void
ply_logger_toggle_logging (ply_logger_t *logger)
{
  assert (logger != NULL);

  logger->is_enabled = !logger->is_enabled;
}

bool
ply_logger_is_logging (ply_logger_t *logger)
{
  assert (logger != NULL);

  return logger->is_enabled != false;
}

static bool
ply_logger_validate_format_string (ply_logger_t   *logger,
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
ply_logger_inject_with_non_literal_format_string (ply_logger_t   *logger,
                                                  const char     *format,
                                                  ...)
{
  va_list args;
  size_t string_size;
  char write_buffer[PLY_LOGGER_MAX_INJECTION_SIZE] = "";

  assert (logger != NULL);

  if (!ply_logger_is_logging (logger))
    return;

  if (!ply_logger_validate_format_string (logger, format))
    {
      ply_logger_write_exception (logger, "log format string invalid");
      return;
    }

  va_start (args, format);
  string_size = vsnprintf (write_buffer, 0, format, args) + 1;
  va_end (args);

  if (string_size > PLY_LOGGER_MAX_INJECTION_SIZE)
    {
      ply_logger_write_exception (logger, "log text too long");
      return;
    }

  va_start (args, format);
  vsnprintf (write_buffer, PLY_LOGGER_MAX_INJECTION_SIZE,
             format, args);
  va_end (args);

  ply_logger_inject_bytes (logger, write_buffer, string_size - 1);

}

void
ply_logger_inject_bytes (ply_logger_t *logger,
                         const void   *bytes,
                         size_t        number_of_bytes)
{
  ply_list_node_t *node;
  void *filtered_bytes;
  size_t filtered_size;

  assert (logger != NULL);
  assert (bytes != NULL);
  assert (number_of_bytes != 0);

  filtered_bytes = NULL;
  filtered_size = 0;
  node = ply_list_get_first_node (logger->filters);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_logger_filter_t *filter;

      filter = (ply_logger_filter_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (logger->filters, node);

      if (filtered_bytes == NULL)
        filter->handler (filter->user_data, bytes, number_of_bytes,
                         &filtered_bytes, &filtered_size, logger);
      else
        {
          void *new_bytes;
          size_t new_size;

          new_bytes = NULL;
          new_size = 0;
          filter->handler (filter->user_data, filtered_bytes, filtered_size,
                           &new_bytes, &new_size, logger);

          if (new_bytes != NULL)
            {
              free (filtered_bytes);
              filtered_bytes = new_bytes;
              filtered_size = new_size;
            }
        }

      node = next_node;
    }

  if (filtered_bytes == NULL)
    ply_logger_buffer (logger, bytes, number_of_bytes);
  else
    {
      ply_logger_buffer (logger, filtered_bytes, filtered_size);
      free (filtered_bytes);
    }

  assert ((logger->flush_policy == PLY_LOGGER_FLUSH_POLICY_WHEN_ASKED)
          || (logger->flush_policy == PLY_LOGGER_FLUSH_POLICY_EVERY_TIME));

  if (logger->flush_policy == PLY_LOGGER_FLUSH_POLICY_EVERY_TIME)
    ply_logger_flush (logger);
}

void
ply_logger_add_filter (ply_logger_t                *logger,
                       ply_logger_filter_handler_t  filter_handler,
                       void                        *user_data)
{
  ply_logger_filter_t *filter;

  filter = calloc (1, sizeof (ply_logger_filter_t));

  filter->handler = filter_handler;
  filter->user_data = user_data;

  ply_list_append_data (logger->filters, filter);
}

#ifdef PLY_ENABLE_TRACING
void
ply_logger_toggle_tracing (ply_logger_t *logger)
{
  assert (logger != NULL);

  logger->tracing_is_enabled = !logger->tracing_is_enabled;
}

bool
ply_logger_is_tracing_enabled (ply_logger_t *logger)
{
  assert (logger != NULL);

  return logger->tracing_is_enabled != false;
}
#endif /* PLY_ENABLE_TRACING */

#ifdef PLY_LOGGER_ENABLE_TEST

int
main (int    argc,
      char **argv)
{
  int exit_code;
  ply_logger_t *logger;

  exit_code = 0;
  logger = ply_logger_new ();

  ply_logger_inject (logger, "yo yo yo\n");
  ply_logger_set_output_fd (logger, 1);
  ply_logger_inject (logger, "yo yo yo yo\n");
  ply_logger_flush (logger);
  ply_logger_free (logger);

  return exit_code;
}

#endif /* PLY_LOGGER_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
