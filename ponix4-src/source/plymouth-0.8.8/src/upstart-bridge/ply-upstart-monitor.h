/* ply-upstart-monitor.h - Upstart D-Bus listener
 *
 * Copyright (C) 2010, 2011 Canonical Ltd.
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
 * Written by: Colin Watson <cjwatson@ubuntu.com>
 */
#ifndef PLY_UPSTART_H
#define PLY_UPSTART_H

#include <stdbool.h>

#include "ply-event-loop.h"

typedef struct _ply_upstart_monitor ply_upstart_monitor_t;

typedef struct {
  char *name;
  char *description;
  bool  is_task;
} ply_upstart_monitor_job_properties_t;

typedef struct {
  char     *name;
  char     *goal;
  char     *state;
  uint32_t  failed : 1;
} ply_upstart_monitor_instance_properties_t;

typedef void (* ply_upstart_monitor_state_changed_handler_t) (void                                      *user_data,
                                                              const char                                *old_state,
                                                              ply_upstart_monitor_job_properties_t      *job,
                                                              ply_upstart_monitor_instance_properties_t *instance);

typedef void (* ply_upstart_monitor_failed_handler_t) (void                                      *user_data,
                                                       ply_upstart_monitor_job_properties_t      *job,
                                                       ply_upstart_monitor_instance_properties_t *instance,
                                                       int                                        status);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_upstart_monitor_t *ply_upstart_monitor_new (ply_event_loop_t *loop);
void ply_upstart_monitor_free (ply_upstart_monitor_t *upstart);
bool ply_upstart_monitor_connect_to_event_loop (ply_upstart_monitor_t    *upstart,
                                                ply_event_loop_t         *loop);
void ply_upstart_monitor_add_state_changed_handler (ply_upstart_monitor_t                       *upstart,
                                                    ply_upstart_monitor_state_changed_handler_t  handler,
                                                    void                                        *user_data);
void ply_upstart_monitor_add_failed_handler (ply_upstart_monitor_t                *upstart,
                                             ply_upstart_monitor_failed_handler_t  handler,
                                             void                                 *user_data);
#endif

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
