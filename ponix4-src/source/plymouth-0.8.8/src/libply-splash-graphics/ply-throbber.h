/* throbber.h - simple throbber animation
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef THROBBER_H
#define THROBBER_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-trigger.h"

typedef struct _ply_throbber ply_throbber_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_throbber_t *ply_throbber_new (const char *image_dir,
                                  const char *frames_prefix);
void ply_throbber_free (ply_throbber_t *throbber);

bool ply_throbber_load (ply_throbber_t *throbber);
bool ply_throbber_start (ply_throbber_t         *throbber,
                         ply_event_loop_t    *loop,
                         ply_pixel_display_t *display,
                         long                 x,
                         long                 y);
void ply_throbber_stop (ply_throbber_t *throbber,
                        ply_trigger_t  *stop_trigger);
bool ply_throbber_is_stopped (ply_throbber_t *throbber);

void ply_throbber_draw_area (ply_throbber_t     *throbber,
                             ply_pixel_buffer_t *buffer,
                             long                x,
                             long                y,
                             unsigned long       width,
                             unsigned long       height);

long ply_throbber_get_width (ply_throbber_t *throbber);
long ply_throbber_get_height (ply_throbber_t *throbber);
#endif

#endif /* THROBBER_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
