/* ply-event-loop.c - small epoll based event loop
 *
 * Copyright (C) 2007 Red Hat, Inc.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
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
#include "ply-event-loop.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <sys/termios.h>
#include <unistd.h>

#include "ply-logger.h"
#include "ply-list.h"
#include "ply-utils.h"

#ifndef PLY_EVENT_LOOP_NUM_EVENT_HANDLERS
#define PLY_EVENT_LOOP_NUM_EVENT_HANDLERS 64
#endif

#ifndef PLY_EVENT_LOOP_NO_TIMED_WAKEUP
#define PLY_EVENT_LOOP_NO_TIMED_WAKEUP 0.0
#endif

typedef struct
{
  int fd;
  ply_list_t *destinations;
  ply_list_t *fd_watches;
  uint32_t is_getting_polled : 1;
  uint32_t is_disconnected : 1;
  int reference_count;
} ply_event_source_t;

typedef struct
{
  ply_event_source_t *source;

  ply_event_loop_fd_status_t status;
  ply_event_handler_t status_met_handler;
  ply_event_handler_t disconnected_handler;
  void *user_data;
} ply_event_destination_t;

struct _ply_fd_watch
{
  ply_event_destination_t *destination;
};

typedef struct
{
  int signal_number;
  ply_event_handler_t handler;
  void *user_data;

  sighandler_t old_posix_signal_handler;
} ply_signal_source_t;

static int ply_signal_dispatcher_sender_fd = -1,
           ply_signal_dispatcher_receiver_fd = -1;

typedef struct
{
  ply_list_t *sources;
} ply_signal_dispatcher_t;

typedef struct
{
  ply_event_loop_exit_handler_t  handler;
  void                          *user_data;
} ply_event_loop_exit_closure_t;

typedef struct
{
  double timeout;
  ply_event_loop_timeout_handler_t  handler;
  void                             *user_data;
} ply_event_loop_timeout_watch_t;

struct _ply_event_loop
{
  int epoll_fd;
  int exit_code;
  double wakeup_time;

  ply_list_t *sources;
  ply_list_t *exit_closures;
  ply_list_t *timeout_watches;

  ply_signal_dispatcher_t *signal_dispatcher;

  uint32_t should_exit : 1;
};

static void ply_event_loop_remove_source (ply_event_loop_t    *loop,
                                          ply_event_source_t *source);
static ply_list_node_t *ply_event_loop_find_source_node (ply_event_loop_t *loop,
                                                         int               fd);

static ply_list_node_t *
ply_signal_dispatcher_find_source_node (ply_signal_dispatcher_t *dispatcher,
                                        int                      signal_number);


static ply_signal_source_t *
ply_signal_source_new (int                  signal_number,
                       ply_event_handler_t  signal_handler,
                       void                *user_data)
{
  ply_signal_source_t *source;

  source = calloc (1, sizeof (ply_signal_source_t));
  source->signal_number = signal_number;
  source->handler = signal_handler;
  source->user_data = user_data;
  source->old_posix_signal_handler = NULL;

  return source;
}

static void
ply_signal_source_free (ply_signal_source_t *handler)
{
  if (handler == NULL)
    return;

  free (handler);
}

static ply_signal_dispatcher_t *
ply_signal_dispatcher_new (void)
{
  ply_signal_dispatcher_t *dispatcher;

  if (!ply_open_unidirectional_pipe (&ply_signal_dispatcher_sender_fd,
                                     &ply_signal_dispatcher_receiver_fd))
    return NULL;

  dispatcher = calloc (1, sizeof (ply_signal_dispatcher_t));

  dispatcher->sources = ply_list_new ();

  return dispatcher;
}

static void
ply_signal_dispatcher_free (ply_signal_dispatcher_t *dispatcher)
{
  ply_list_node_t *node;

  if (dispatcher == NULL)
    return;

  close (ply_signal_dispatcher_receiver_fd);
  ply_signal_dispatcher_receiver_fd = -1;
  close (ply_signal_dispatcher_sender_fd);
  ply_signal_dispatcher_sender_fd = -1;

  node = ply_list_get_first_node (dispatcher->sources);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_signal_source_t *source;

      source = (ply_signal_source_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (dispatcher->sources, node);

      ply_signal_source_free (source);

      node = next_node;
    }

  ply_list_free (dispatcher->sources);

  free (dispatcher);
}

static void
ply_signal_dispatcher_posix_signal_handler (int signal_number)
{
  if (ply_signal_dispatcher_sender_fd < 0)
    return;

  ply_write (ply_signal_dispatcher_sender_fd, &signal_number,
             sizeof (signal_number));
}

static int
ply_signal_dispatcher_get_next_signal_from_pipe (ply_signal_dispatcher_t *dispatcher)
{
  int signal_number;

  if (!ply_read (ply_signal_dispatcher_receiver_fd, &signal_number,
                 sizeof (signal_number)))
    signal_number = 0;

  return signal_number;
}

static void
ply_signal_dispatcher_dispatch_signal (ply_signal_dispatcher_t *dispatcher,
                                       int                      fd)
{
  ply_list_node_t *node;
  int signal_number;

  assert (fd == ply_signal_dispatcher_receiver_fd);

  signal_number = ply_signal_dispatcher_get_next_signal_from_pipe (dispatcher);

  node = ply_list_get_first_node (dispatcher->sources);
  while (node != NULL)
    {
      ply_signal_source_t *source;

      source = (ply_signal_source_t *) ply_list_node_get_data (node);

      if (source->signal_number == signal_number)
        {
          if (source->handler != NULL)
            source->handler (source->user_data, signal_number);
        }

      node = ply_list_get_next_node (dispatcher->sources, node);
    }
}

static void
ply_signal_dispatcher_reset_signal_sources (ply_signal_dispatcher_t *dispatcher,
                                            int                      fd)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (dispatcher->sources);
  while (node != NULL)
    {
      ply_signal_source_t *handler;

      handler = (ply_signal_source_t *) ply_list_node_get_data (node);

      signal (handler->signal_number,
              handler->old_posix_signal_handler != NULL?
              handler->old_posix_signal_handler : SIG_DFL);

      node = ply_list_get_next_node (dispatcher->sources, node);
    }
}

static ply_event_destination_t *
ply_event_destination_new (ply_event_loop_fd_status_t     status,
                           ply_event_handler_t            status_met_handler,
                           ply_event_handler_t            disconnected_handler,
                           void                          *user_data)
{
  ply_event_destination_t *destination;

  destination = calloc (1, sizeof (ply_event_destination_t));

  destination->source = NULL;
  destination->status = status;
  destination->status_met_handler = status_met_handler;
  destination->disconnected_handler = disconnected_handler;
  destination->user_data = user_data;

  return destination;
}

static void
ply_event_destination_free (ply_event_destination_t *destination)
{
  if (destination == NULL)
    return;

  free (destination);
}

static ply_fd_watch_t *
ply_fd_watch_new (ply_event_destination_t *destination)
{
  ply_fd_watch_t *watch;

  watch = calloc (1, sizeof (ply_fd_watch_t));
  watch->destination = destination;

  return watch;
}

static void
ply_fd_watch_free (ply_fd_watch_t *watch)
{
  watch->destination = NULL;
  free (watch);
}

static void
ply_event_source_take_reference (ply_event_source_t *source)
{
  source->reference_count++;
}

static ply_event_source_t *
ply_event_source_new (int fd)
{
  ply_event_source_t *source;

  source = calloc (1, sizeof (ply_event_source_t));

  source->fd = fd;
  source->destinations = ply_list_new ();
  source->fd_watches = ply_list_new ();
  source->is_getting_polled = false;
  source->is_disconnected = false;
  source->reference_count = 0;

  return source;
}

static void
ply_event_source_free (ply_event_source_t *source)
{
  if (source == NULL)
    return;

  assert (ply_list_get_length (source->destinations) == 0);

  ply_list_free (source->destinations);
  ply_list_free (source->fd_watches);
  free (source);
}

static void
ply_event_source_drop_reference (ply_event_source_t *source)
{
  if (source == NULL)
    return;

  source->reference_count--;

  assert (source->reference_count >= 0);

  if (source->reference_count == 0)
    {
      ply_event_source_free (source);
    }
}

static void
ply_event_loop_update_source_event_mask (ply_event_loop_t   *loop,
                                         ply_event_source_t *source)
{
  ply_list_node_t *node;
  struct epoll_event event = { 0 };

  assert (loop != NULL);
  assert (source != NULL);
  assert (source->destinations != NULL);

  event.events = EPOLLERR | EPOLLHUP;

  node = ply_list_get_first_node (source->destinations);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_destination_t *destination;

      destination = (ply_event_destination_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (source->destinations, node);

      if (destination->status & PLY_EVENT_LOOP_FD_STATUS_HAS_DATA)
          event.events |= EPOLLIN;

      if (destination->status & PLY_EVENT_LOOP_FD_STATUS_HAS_CONTROL_DATA)
          event.events |= EPOLLPRI;

      if (destination->status & PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA)
          event.events |= EPOLLOUT;

      node = next_node;
    }
  event.data.ptr = source;

  if (source->is_getting_polled)
    {
      int status;

      status = epoll_ctl (loop->epoll_fd, EPOLL_CTL_MOD, source->fd, &event);

      if (status < 0)
         ply_trace ("failed to modify epoll event mask for fd %d: %m", source->fd);
    }
}

static ply_fd_watch_t *
ply_event_loop_add_destination_for_source (ply_event_loop_t        *loop,
                                           ply_event_destination_t *destination,
                                           ply_event_source_t      *source)
{
  ply_list_node_t *destination_node;
  ply_fd_watch_t *watch;

  assert (loop != NULL);
  assert (destination != NULL);
  assert (destination->source == NULL);
  assert (source != NULL);

  destination->source = source;
  ply_event_source_take_reference (source);
  destination_node = ply_list_append_data (source->destinations, destination);
  assert (destination_node != NULL);
  assert (destination->source == source);

  ply_event_loop_update_source_event_mask (loop, source);

  watch = ply_fd_watch_new (destination);

  ply_event_source_take_reference (source);
  ply_list_append_data (source->fd_watches, watch);

  return watch;
}

static ply_event_destination_t *
ply_event_loop_get_destination_from_fd_watch (ply_event_loop_t *loop,
                                              ply_fd_watch_t   *watch)
{
   ply_event_destination_t *destination;

   assert (loop != NULL);
   assert (watch != NULL);
   assert (watch->destination != NULL);

   destination = watch->destination;

   return destination;
}

static void
ply_event_loop_remove_destination_by_fd_watch (ply_event_loop_t *loop,
                                               ply_fd_watch_t   *watch)
{
  ply_event_destination_t *destination;
  ply_event_source_t *source;

  assert (loop != NULL);
  assert (watch != NULL);

  destination = ply_event_loop_get_destination_from_fd_watch (loop, watch);
  assert (destination != NULL);

  source = destination->source;
  assert (source != NULL);

  ply_list_remove_data (source->destinations, destination);
  ply_event_source_drop_reference (source);
  assert (ply_list_find_node (source->destinations, destination) == NULL);
  ply_event_loop_update_source_event_mask (loop, source);
}

ply_event_loop_t *
ply_event_loop_new (void)
{
  ply_event_loop_t *loop;

  loop = calloc (1, sizeof (ply_event_loop_t));

  loop->epoll_fd = epoll_create (PLY_EVENT_LOOP_NUM_EVENT_HANDLERS);
  loop->wakeup_time = PLY_EVENT_LOOP_NO_TIMED_WAKEUP;

  assert (loop->epoll_fd >= 0);

  loop->should_exit = false;
  loop->exit_code = 0;

  loop->sources = ply_list_new ();
  loop->exit_closures = ply_list_new ();
  loop->timeout_watches = ply_list_new ();

  loop->signal_dispatcher = ply_signal_dispatcher_new ();

  if (loop->signal_dispatcher == NULL)
    return NULL;

  ply_event_loop_watch_fd (loop,
                           ply_signal_dispatcher_receiver_fd,
                           PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                           (ply_event_handler_t)
                           ply_signal_dispatcher_dispatch_signal,
                           (ply_event_handler_t)
                           ply_signal_dispatcher_reset_signal_sources,
                           loop->signal_dispatcher);

  return loop;
}

ply_event_loop_t *
ply_event_loop_get_default (void)
{
  static ply_event_loop_t *loop = NULL;

  if (loop == NULL)
    loop = ply_event_loop_new ();

  return loop;
}

static void
ply_event_loop_free_exit_closures (ply_event_loop_t *loop)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (loop->exit_closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_exit_closure_t *closure;

      closure = (ply_event_loop_exit_closure_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (loop->exit_closures, node);
      free (closure);

      node = next_node;
    }
  ply_list_free (loop->exit_closures);
}

static void
ply_event_loop_run_exit_closures (ply_event_loop_t *loop)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (loop->exit_closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_exit_closure_t *closure;

      closure = (ply_event_loop_exit_closure_t *) ply_list_node_get_data (node);

      assert (closure->handler != NULL);
      next_node = ply_list_get_next_node (loop->exit_closures, node);

      closure->handler (closure->user_data, loop->exit_code, loop);

      node = next_node;
    }
}

void
ply_event_loop_free (ply_event_loop_t *loop)
{
  if (loop == NULL)
    return;

  assert (ply_list_get_length (loop->sources) == 0);
  assert (ply_list_get_length (loop->timeout_watches) == 0);

  ply_signal_dispatcher_free (loop->signal_dispatcher);
  ply_event_loop_free_exit_closures (loop);

  ply_list_free (loop->sources);
  ply_list_free (loop->timeout_watches);

  close (loop->epoll_fd);
  free (loop);
}

static ply_list_node_t *
ply_event_loop_find_source_node (ply_event_loop_t *loop,
                                 int               fd)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (loop->sources);
  while (node != NULL)
    {
      ply_event_source_t *source;

      source = (ply_event_source_t *) ply_list_node_get_data (node);

      if (source->fd == fd)
        break;

      node = ply_list_get_next_node (loop->sources, node);
    }

  return node;
}

static void
ply_event_loop_add_source (ply_event_loop_t    *loop,
                           ply_event_source_t  *source)
{
  struct epoll_event event = { 0 };
  int status;

  assert (ply_event_loop_find_source_node (loop, source->fd) == NULL);
  assert (source->is_getting_polled == false);

  event.events = EPOLLERR | EPOLLHUP;
  event.data.ptr = source;

  status = epoll_ctl (loop->epoll_fd, EPOLL_CTL_ADD, source->fd, &event);
  assert (status == 0);

  source->is_getting_polled = true;

  ply_event_source_take_reference (source);
  ply_list_append_data (loop->sources, source);
}

static void
ply_event_loop_remove_source_node (ply_event_loop_t *loop,
                                   ply_list_node_t  *source_node)
{
  ply_event_source_t *source;

  source = (ply_event_source_t *) ply_list_node_get_data (source_node);

  assert (source != NULL);

  if (source->is_getting_polled)
    {
      int status;

      status = epoll_ctl (loop->epoll_fd, EPOLL_CTL_DEL, source->fd, NULL);

      if (status < 0)
        ply_trace ("failed to delete fd %d from epoll watch list: %m", source->fd);
      source->is_getting_polled = false;
    }

  ply_list_remove_node (loop->sources, source_node);
  ply_event_source_drop_reference (source);
}

static void
ply_event_loop_remove_source (ply_event_loop_t   *loop,
                              ply_event_source_t *source)
{
  ply_list_node_t *source_node;
  assert (ply_list_get_length (source->destinations) == 0);

  source_node = ply_list_find_node (loop->sources, source);

  assert (source_node != NULL);

  ply_event_loop_remove_source_node (loop, source_node);
}

static void
ply_event_loop_free_sources (ply_event_loop_t *loop)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (loop->sources);
  while (node != NULL)
    {
      ply_list_node_t *next_node;

      next_node = ply_list_get_next_node (loop->sources, node);
      ply_event_loop_remove_source_node (loop, node);
      node = next_node;
    }
}

static bool
ply_event_loop_fd_status_is_valid (ply_event_loop_fd_status_t status)
{
  return (status & ~(PLY_EVENT_LOOP_FD_STATUS_NONE
                     | PLY_EVENT_LOOP_FD_STATUS_HAS_DATA
                     | PLY_EVENT_LOOP_FD_STATUS_HAS_CONTROL_DATA
                     | PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA)) == 0;

}

static ply_event_source_t *
ply_event_loop_get_source_from_fd (ply_event_loop_t *loop,
                                   int               fd)
{
  ply_list_node_t *source_node;
  ply_event_source_t *source;

  source_node = ply_event_loop_find_source_node (loop, fd);

  if (source_node == NULL)
    {
      source = ply_event_source_new (fd);
      ply_event_loop_add_source (loop, source);

      source_node = ply_list_get_last_node (loop->sources);
      assert (source_node != NULL);
    }

  source = (ply_event_source_t *) ply_list_node_get_data (source_node);
  assert (source->fd == fd);

  return source;
}

ply_fd_watch_t *
ply_event_loop_watch_fd (ply_event_loop_t           *loop,
                         int                         fd,
                         ply_event_loop_fd_status_t  status,
                         ply_event_handler_t         status_met_handler,
                         ply_event_handler_t         disconnected_handler,
                         void                       *user_data)
{

  ply_event_source_t *source;
  ply_event_destination_t *destination;
  ply_fd_watch_t *watch;

  assert (loop != NULL);
  assert (fd >= 0);
  assert (ply_event_loop_fd_status_is_valid (status));
  assert (status != PLY_EVENT_LOOP_FD_STATUS_NONE || status_met_handler == NULL);

  source = ply_event_loop_get_source_from_fd (loop, fd);
  assert (source != NULL);

  destination = ply_event_destination_new (status, status_met_handler,
                                           disconnected_handler, user_data);
  watch = ply_event_loop_add_destination_for_source (loop, destination, source);

  return watch;
}

void
ply_event_loop_stop_watching_fd (ply_event_loop_t *loop,
                                 ply_fd_watch_t   *watch)
{
  ply_event_destination_t *destination;
  ply_event_source_t *source;

  assert (loop != NULL);
  assert (watch != NULL);

  destination = ply_event_loop_get_destination_from_fd_watch (loop, watch);

  assert (destination != NULL);

  source = destination->source;

  if (source == NULL) 
    ply_trace ("NULL source when stopping watching fd");
  else
    ply_trace ("stopping watching fd %d", source->fd);

  assert (source != NULL);
  assert (source->fd >= 0);

  /* if we're already disconnected then the watch is already scheduled
   * to be removed by ply_event_loop_disconnect_source
   */
  if (source->is_disconnected)
    {
      ply_trace ("source for fd %d is already disconnected", source->fd);
      ply_list_remove_data (source->fd_watches, watch);
      ply_event_source_drop_reference (source);
      ply_fd_watch_free (watch);
      return;
    }

  ply_trace ("removing destination for fd %d", source->fd);
  ply_event_loop_remove_destination_by_fd_watch (loop, watch);

  ply_list_remove_data (source->fd_watches, watch);
  ply_event_source_drop_reference (source);
  ply_fd_watch_free (watch);
  ply_event_destination_free (destination);

  if (ply_list_get_length (source->destinations) == 0)
    {
      ply_trace ("no more destinations remaing for fd %d, removing source", source->fd);
      ply_event_loop_remove_source (loop, source);
    }
}

static ply_list_node_t *
ply_signal_dispatcher_find_source_node (ply_signal_dispatcher_t *dispatcher,
                                        int                 signal_number)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (dispatcher->sources);
  while (node != NULL)
    {
      ply_signal_source_t *handler;

      handler = (ply_signal_source_t *) ply_list_node_get_data (node);

      assert (handler != NULL);

      if (handler->signal_number == signal_number)
        break;

      node = ply_list_get_next_node (dispatcher->sources, node);
    }

  return node;
}

void
ply_event_loop_watch_signal (ply_event_loop_t   *loop,
                            int                  signal_number,
                            ply_event_handler_t  signal_handler,
                            void                *user_data)
{
  ply_signal_source_t *source;

  source = ply_signal_source_new (signal_number,
                                  signal_handler,
                                  user_data);

  source->old_posix_signal_handler =
      signal (signal_number, ply_signal_dispatcher_posix_signal_handler);
  ply_list_append_data (loop->signal_dispatcher->sources, source);
}

static void
ply_signal_dispatcher_remove_source_node (ply_signal_dispatcher_t  *dispatcher,
                                          ply_list_node_t          *node)
{
  ply_signal_source_t *source;

  source = (ply_signal_source_t *) ply_list_node_get_data (node);

  signal (source->signal_number,
          source->old_posix_signal_handler != NULL?
          source->old_posix_signal_handler : SIG_DFL);

  ply_list_remove_node (dispatcher->sources, node);
}

void
ply_event_loop_stop_watching_signal (ply_event_loop_t *loop,
                                     int               signal_number)
{
  ply_list_node_t *node;

  node = ply_signal_dispatcher_find_source_node (loop->signal_dispatcher,
                                                 signal_number);

  if (node == NULL)
    return;

  ply_signal_dispatcher_remove_source_node (loop->signal_dispatcher, node);
}

void
ply_event_loop_watch_for_exit (ply_event_loop_t              *loop,
                               ply_event_loop_exit_handler_t  exit_handler,
                               void                          *user_data)
{
  ply_event_loop_exit_closure_t *closure;

  assert (loop != NULL);
  assert (exit_handler != NULL);

  closure = calloc (1, sizeof (ply_event_loop_exit_closure_t));
  closure->handler = exit_handler;
  closure->user_data = user_data;

  ply_list_append_data (loop->exit_closures, closure);
}

void
ply_event_loop_stop_watching_for_exit (ply_event_loop_t *loop,
                                       ply_event_loop_exit_handler_t exit_handler,
                                       void             *user_data)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (loop->exit_closures);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_exit_closure_t *closure;

      closure = (ply_event_loop_exit_closure_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (loop->exit_closures, node);

      if (closure->handler == exit_handler &&
          closure->user_data == user_data) {
              ply_list_remove_node (loop->exit_closures, node);
              free (closure);
      }

      node = next_node;
    }
}

void
ply_event_loop_watch_for_timeout (ply_event_loop_t    *loop,
                                  double               seconds,
                                  ply_event_loop_timeout_handler_t timeout_handler,
                                  void                *user_data)
{
  ply_event_loop_timeout_watch_t *timeout_watch;

  assert (loop != NULL);
  assert (timeout_handler != NULL);
  assert (seconds > 0.0);

  timeout_watch = calloc (1, sizeof (ply_event_loop_timeout_watch_t));
  timeout_watch->timeout = ply_get_timestamp () + seconds;
  timeout_watch->handler = timeout_handler;
  timeout_watch->user_data = user_data;

  if (fabs (loop->wakeup_time - PLY_EVENT_LOOP_NO_TIMED_WAKEUP) <= 0)
    loop->wakeup_time = timeout_watch->timeout;
  else
    loop->wakeup_time = MIN (loop->wakeup_time, timeout_watch->timeout);

  ply_list_append_data (loop->timeout_watches, timeout_watch);
}

void
ply_event_loop_stop_watching_for_timeout (ply_event_loop_t *loop,
                                          ply_event_loop_timeout_handler_t timeout_handler,
                                          void             *user_data)
{
  ply_list_node_t *node;
  bool timeout_removed;

  loop->wakeup_time = PLY_EVENT_LOOP_NO_TIMED_WAKEUP;

  timeout_removed = false;
  node = ply_list_get_first_node (loop->timeout_watches);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_timeout_watch_t *timeout_watch;

      timeout_watch = (ply_event_loop_timeout_watch_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (loop->timeout_watches, node);

      if (timeout_watch->handler == timeout_handler &&
          timeout_watch->user_data == user_data)
        {
          ply_list_remove_node (loop->timeout_watches, node);
          free (timeout_watch);

          if (timeout_removed)
            ply_trace ("multiple matching timeouts found for removal");

          timeout_removed = true;
        }
      else
        {
          if (fabs (loop->wakeup_time - PLY_EVENT_LOOP_NO_TIMED_WAKEUP) <= 0)
            loop->wakeup_time = timeout_watch->timeout;
          else
            loop->wakeup_time = MIN (loop->wakeup_time, timeout_watch->timeout);
        }

      node = next_node;
    }

  if (!timeout_removed)
    ply_trace ("no matching timeout found for removal");
}

static ply_event_loop_fd_status_t
ply_event_loop_get_fd_status_from_poll_mask (uint32_t mask)
{
  ply_event_loop_fd_status_t status;

  status = PLY_EVENT_LOOP_FD_STATUS_NONE;

  if (mask & EPOLLIN)
    status |= PLY_EVENT_LOOP_FD_STATUS_HAS_DATA;

  if (mask & EPOLLPRI)
    status |= PLY_EVENT_LOOP_FD_STATUS_HAS_CONTROL_DATA;

  if (mask & EPOLLOUT)
    status |= PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA;

  return status;
}

static bool
ply_event_loop_source_has_met_status (ply_event_source_t         *source,
                                      ply_event_loop_fd_status_t  status)
{
  ply_list_node_t *node;

  assert (source != NULL);
  assert (ply_event_loop_fd_status_is_valid (status));

  node = ply_list_get_first_node (source->destinations);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_destination_t *destination;

      destination = (ply_event_destination_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (source->destinations, node);

      if (((destination->status & status) != 0)
          && (destination->status_met_handler != NULL))
        return true;

      node = next_node;
    }
  return false;
}

static void
ply_event_loop_handle_met_status_for_source (ply_event_loop_t           *loop,
                                             ply_event_source_t         *source,
                                             ply_event_loop_fd_status_t  status)
{
  ply_list_node_t *node;

  assert (loop != NULL);
  assert (source != NULL);
  assert (ply_event_loop_fd_status_is_valid (status));

  node = ply_list_get_first_node (source->destinations);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_destination_t *destination;

      destination = (ply_event_destination_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (source->destinations, node);

      if (((destination->status & status) != 0)
          && (destination->status_met_handler != NULL))
        destination->status_met_handler (destination->user_data, source->fd);

      node = next_node;
    }
}

static void
ply_event_loop_handle_disconnect_for_source (ply_event_loop_t   *loop,
                                             ply_event_source_t *source)
{
  ply_list_node_t *node;

  assert (loop != NULL);
  assert (source != NULL);

  source->is_disconnected = true;
  node = ply_list_get_first_node (source->destinations);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_destination_t *destination;

      destination = (ply_event_destination_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (source->destinations, node);

      if (destination->disconnected_handler != NULL)
        {
          ply_trace ("calling disconnected_handler %p for fd %d",
                     destination->disconnected_handler, source->fd);
          destination->disconnected_handler (destination->user_data, source->fd);

          ply_trace ("done calling disconnected_handler %p for fd %d",
                     destination->disconnected_handler, source->fd);
        }

      node = next_node;
    }
}

static void
ply_event_loop_free_watches_for_source (ply_event_loop_t   *loop,
                                        ply_event_source_t *source)
{
  ply_list_node_t *node;

  assert (loop != NULL);
  assert (source != NULL);

  node = ply_list_get_first_node (source->fd_watches);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_fd_watch_t *watch;

      next_node = ply_list_get_next_node (source->fd_watches, node);

      watch = (ply_fd_watch_t *) ply_list_node_get_data (node);

      assert (watch != NULL);
      ply_fd_watch_free (watch);
      ply_list_remove_node (source->fd_watches, node);
      ply_event_source_drop_reference (source);
      node = next_node;
    }
}

static void
ply_event_loop_free_timeout_watches (ply_event_loop_t *loop)
{
  ply_list_node_t *node;

  assert (loop != NULL);

  node = ply_list_get_first_node (loop->timeout_watches);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_timeout_watch_t *watch;

      watch = (ply_event_loop_timeout_watch_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (loop->timeout_watches, node);

      free (watch);
      ply_list_remove_node (loop->timeout_watches, node);

      node = next_node;
    }

  assert (ply_list_get_length (loop->timeout_watches) == 0);
  loop->wakeup_time = PLY_EVENT_LOOP_NO_TIMED_WAKEUP;
}

static void
ply_event_loop_free_destinations_for_source (ply_event_loop_t   *loop,
                                             ply_event_source_t *source)
{
  ply_list_node_t *node;

  assert (loop != NULL);
  assert (source != NULL);

  node = ply_list_get_first_node (source->destinations);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_destination_t *destination;

      next_node = ply_list_get_next_node (source->destinations, node);

      destination =
          (ply_event_destination_t *) ply_list_node_get_data (node);

      assert (destination != NULL);
      ply_trace ("freeing destination (%u, %p, %p) of fd %d",
                 destination->status, destination->status_met_handler,
                 destination->disconnected_handler, source->fd);
      ply_event_destination_free (destination);

      ply_list_remove_node (source->destinations, node);
      ply_event_source_drop_reference (source);
      node = next_node;
    }
}

static void
ply_event_loop_disconnect_source (ply_event_loop_t           *loop,
                                  ply_event_source_t         *source)
{
  ply_trace ("disconnecting source with fd %d", source->fd);
  ply_event_loop_handle_disconnect_for_source (loop, source);
  ply_trace ("done disconnecting source with fd %d", source->fd);

  /* at this point, we've told the event loop users about the
   * fd disconnection, so we can invalidate any outstanding
   * watches and free the destinations.
   */
  ply_trace ("freeing watches for source with fd %d", source->fd);
  ply_event_loop_free_watches_for_source (loop, source);
  ply_trace ("done freeing watches for source with fd %d", source->fd);
  ply_trace ("freeing destinations for source with fd %d", source->fd);
  ply_event_loop_free_destinations_for_source (loop, source);
  ply_trace ("done freeing destinations for source with fd %d", source->fd);
  assert (ply_list_get_length (source->destinations) == 0);

  ply_trace ("removing source with fd %d from event loop", source->fd);
  ply_event_loop_remove_source (loop, source);
  ply_trace ("done removing source with fd %d from event loop", source->fd);
}

static void
ply_event_loop_handle_timeouts (ply_event_loop_t *loop)
{
  ply_list_node_t *node;
  double now;

  assert (loop != NULL);

  now = ply_get_timestamp ();
  node = ply_list_get_first_node (loop->timeout_watches);
  loop->wakeup_time = PLY_EVENT_LOOP_NO_TIMED_WAKEUP;
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_event_loop_timeout_watch_t *watch;

      watch = (ply_event_loop_timeout_watch_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (loop->timeout_watches, node);

      if (watch->timeout <= now)
        {
          assert (watch->handler != NULL);

          ply_list_remove_node (loop->timeout_watches, node);

          watch->handler (watch->user_data, loop);
          free (watch);

          /* start over in case the handler invalidated the list
           */
          next_node = ply_list_get_first_node (loop->timeout_watches);
        }
      else
        {
          if (fabs (loop->wakeup_time - PLY_EVENT_LOOP_NO_TIMED_WAKEUP) <= 0)
            loop->wakeup_time = watch->timeout;
          else
            loop->wakeup_time = MIN (loop->wakeup_time, watch->timeout);
        }

      node = next_node;
    }

}

void
ply_event_loop_process_pending_events (ply_event_loop_t *loop)
{
  int number_of_received_events, i;
  static struct epoll_event events[PLY_EVENT_LOOP_NUM_EVENT_HANDLERS];

  assert (loop != NULL);

  memset (events, -1,
          PLY_EVENT_LOOP_NUM_EVENT_HANDLERS * sizeof (struct epoll_event));

  do
   {
     int timeout;

     if (fabs (loop->wakeup_time - PLY_EVENT_LOOP_NO_TIMED_WAKEUP) <= 0)
       timeout = -1;
     else
       {
         timeout = (int) ((loop->wakeup_time - ply_get_timestamp ()) * 1000);
         timeout = MAX (timeout, 0);
       }

     number_of_received_events = epoll_wait (loop->epoll_fd, events,
                                             PLY_EVENT_LOOP_NUM_EVENT_HANDLERS,
                                             timeout);
     if (number_of_received_events < 0)
       {
         if (errno != EINTR && errno != EAGAIN)
           {
             ply_event_loop_exit (loop, 255);
             return;
           }
       }
     else
       {
         /* Reference all sources, so they stay alive for the duration of this
          * iteration of the loop.
          */
         for (i = 0; i < number_of_received_events; i++)
           {
             ply_event_source_t *source;
             source = (ply_event_source_t *) (events[i].data.ptr);

             ply_event_source_take_reference (source);
           }
       }

     /* First handle timeouts */
     ply_event_loop_handle_timeouts (loop);
    }
  while (number_of_received_events < 0);

  /* Then process the incoming events
   */
  for (i = 0; i < number_of_received_events; i++)
    {
      ply_event_source_t *source;
      ply_event_loop_fd_status_t status;
      bool is_disconnected;

      source = (ply_event_source_t *) (events[i].data.ptr);
      status = ply_event_loop_get_fd_status_from_poll_mask (events[i].events);

      is_disconnected = false;
      if ((events[i].events & EPOLLHUP) || (events[i].events & EPOLLERR))
        {
          int bytes_ready;

          bytes_ready = 0;
          if (ioctl (source->fd, FIONREAD, &bytes_ready) < 0)
            bytes_ready = 0;

          if (bytes_ready <= 0)
            is_disconnected = true;
        }

      if (is_disconnected)
        {
          ply_event_loop_disconnect_source (loop, source);
        }
      else if (ply_event_loop_source_has_met_status (source, status))
        ply_event_loop_handle_met_status_for_source (loop, source, status);

      if (loop->should_exit)
        break;
    }

  /* Finally, kill off any unused sources
   */
  for (i = 0; i < number_of_received_events; i++)
    {
      ply_event_source_t *source;

      source = (ply_event_source_t *) (events[i].data.ptr);

      ply_event_source_drop_reference (source);
    }
}

void
ply_event_loop_exit (ply_event_loop_t *loop,
                     int               exit_code)
{
  assert (loop != NULL);

  loop->should_exit = true;
  loop->exit_code = exit_code;
}

int
ply_event_loop_run (ply_event_loop_t *loop)
{
  while (!loop->should_exit)
    ply_event_loop_process_pending_events (loop);

  ply_event_loop_run_exit_closures (loop);
  ply_event_loop_free_sources (loop);
  ply_event_loop_free_timeout_watches (loop);

  loop->should_exit = false;

  return loop->exit_code;
}

#ifdef PLY_EVENT_LOOP_ENABLE_TEST

static ply_event_loop_t *loop;

static void
alrm_signal_handler (void)
{
  write (1, "times up!\n", sizeof ("times up!\n") - 1);
  ply_event_loop_exit (loop, 0);
}

static void
usr1_signal_handler (void)
{
  write (1, "got sigusr1\n", sizeof ("got sigusr1\n") - 1);
}

static void
hangup_signal_handler (void)
{
  write (1, "got hangup\n", sizeof ("got hangup\n") - 1);
}

static void
terminate_signal_handler (void)
{
  write (1, "got terminate\n", sizeof ("got terminate\n") - 1);
  ply_event_loop_exit (loop, 0);
}

static void
line_received_handler (void)
{
  char line[512] = { 0 };
  printf ("Received line: ");
  fflush (stdout);

  fgets (line, sizeof (line), stdin);
  printf ("%s", line);
}

static void
on_timeout (ply_event_loop_t *loop)
{
  printf ("timeout elapsed\n");
}

int
main (int    argc,
      char **argv)
{
  int exit_code;

  loop = ply_event_loop_new ();

  ply_event_loop_watch_signal (loop, SIGHUP,
                             (ply_event_handler_t) hangup_signal_handler,
                             NULL);
  ply_event_loop_watch_signal (loop, SIGTERM,
                             (ply_event_handler_t)
                             terminate_signal_handler, NULL);
  ply_event_loop_watch_signal (loop, SIGUSR1,
                             (ply_event_handler_t)
                             usr1_signal_handler, NULL);
  ply_event_loop_watch_signal (loop, SIGALRM,
                             (ply_event_handler_t)
                             alrm_signal_handler, NULL);

  ply_event_loop_watch_for_timeout (loop, 2.0,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, loop);
  ply_event_loop_watch_fd (loop, 0, PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                          (ply_event_handler_t) line_received_handler,
                          (ply_event_handler_t) line_received_handler,
                          NULL);

  alarm (5);
  exit_code = ply_event_loop_run (loop);

  ply_event_loop_free (loop);

  return exit_code;
}
#endif /* PLY_EVENT_LOOP_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
