/* animation.h - simple animation
 *
 * Copyright (C) 2009 Red Hat, Inc.
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
 * Written By: William Jon McCann <jmccann@redhat.com>
 */
#ifndef ANIMATION_H
#define ANIMATION_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-pixel-display.h"
#include "ply-trigger.h"

typedef struct _ply_animation ply_animation_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_animation_t *ply_animation_new (const char *image_dir,
                                    const char *frames_prefix);
void ply_animation_free (ply_animation_t *animation);

bool ply_animation_load (ply_animation_t *animation);
bool ply_animation_start (ply_animation_t    *animation,
                          ply_pixel_display_t *display,
                          ply_trigger_t      *stop_trigger,
                          long                x,
                          long                y);
void ply_animation_stop (ply_animation_t *animation);
bool ply_animation_is_stopped (ply_animation_t *animation);

void ply_animation_draw_area (ply_animation_t    *animation,
                              ply_pixel_buffer_t *buffer,
                              long                x,
                              long                y,
                              unsigned long       width,
                              unsigned long       height);

long ply_animation_get_width (ply_animation_t *animation);
long ply_animation_get_height (ply_animation_t *animation);
#endif

#endif /* ANIMATION_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
