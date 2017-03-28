/* ply-event-loop.h - small epoll based event loop
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
#ifndef PLY_EVENT_LOOP_H
#define PLY_EVENT_LOOP_H

#include <stdbool.h>
#include <signal.h>
#include <stdint.h>

typedef struct _ply_event_loop ply_event_loop_t;
typedef struct _ply_fd_watch ply_fd_watch_t;

typedef enum {
  PLY_EVENT_LOOP_FD_STATUS_NONE = 0,
  PLY_EVENT_LOOP_FD_STATUS_HAS_DATA = 0x1,
  PLY_EVENT_LOOP_FD_STATUS_HAS_CONTROL_DATA = 0x2,
  PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA = 0x4,
} ply_event_loop_fd_status_t;

typedef void (* ply_event_handler_t) (void *user_data,
                                      int   source_fd);

typedef void (* ply_event_loop_exit_handler_t) (void *user_data,
                                                int   exit_code,
                                                ply_event_loop_t *loop);
typedef void (* ply_event_loop_timeout_handler_t) (void             *user_data,
                                                   ply_event_loop_t *loop);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_event_loop_t *ply_event_loop_new (void);
void ply_event_loop_free (ply_event_loop_t *loop);
ply_event_loop_t *ply_event_loop_get_default (void);
ply_fd_watch_t *ply_event_loop_watch_fd (ply_event_loop_t *loop,
                                         int               fd,
                                         ply_event_loop_fd_status_t status,
                                         ply_event_handler_t status_met_handler,
                                         ply_event_handler_t disconnected_handler,
                                         void             *user_data);
void ply_event_loop_stop_watching_fd (ply_event_loop_t *loop, 
                                      ply_fd_watch_t   *watch);
void ply_event_loop_watch_signal (ply_event_loop_t     *loop,
                                  int                   signal_number,
                                  ply_event_handler_t   signal_handler,
                                  void                 *user_data);
void ply_event_loop_stop_watching_signal (ply_event_loop_t *loop,
                                          int               signal_number);

void ply_event_loop_watch_for_exit (ply_event_loop_t              *loop,
                                    ply_event_loop_exit_handler_t  exit_handler,
                                    void                          *user_data);
void ply_event_loop_stop_watching_for_exit (ply_event_loop_t              *loop,
                                            ply_event_loop_exit_handler_t  exit_handler,
                                            void                          *user_data);
void ply_event_loop_watch_for_timeout (ply_event_loop_t    *loop,
                                       double               seconds,             
                                       ply_event_loop_timeout_handler_t timeout_handler,
                                       void                 *user_data);

void ply_event_loop_stop_watching_for_timeout (ply_event_loop_t    *loop,
                                               ply_event_loop_timeout_handler_t timeout_handler,
                                               void                 *user_data);

int ply_event_loop_run (ply_event_loop_t *loop);
void ply_event_loop_exit (ply_event_loop_t *loop,
                          int               exit_code);
void
ply_event_loop_process_pending_events (ply_event_loop_t *loop);
#endif

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
