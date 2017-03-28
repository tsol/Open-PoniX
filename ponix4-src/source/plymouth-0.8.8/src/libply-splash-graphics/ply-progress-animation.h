/* progress-animation.h - simple progress animation
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
#ifndef PROGRESS_ANIMATION_H
#define PROGRESS_ANIMATION_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-pixel-display.h"

typedef struct _ply_progress_animation ply_progress_animation_t;

typedef enum
{
  PLY_PROGRESS_ANIMATION_TRANSITION_NONE,
  PLY_PROGRESS_ANIMATION_TRANSITION_FADE_OVER,
  PLY_PROGRESS_ANIMATION_TRANSITION_CROSS_FADE,
  PLY_PROGRESS_ANIMATION_TRANSITION_MERGE_FADE,
} ply_progress_animation_transition_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_progress_animation_t *ply_progress_animation_new (const char *image_dir,
                                                      const char *frames_prefix);
void ply_progress_animation_free (ply_progress_animation_t *progress_animation);

bool ply_progress_animation_load (ply_progress_animation_t *progress_animation);
void ply_progress_animation_set_transition (ply_progress_animation_t *progress_animation,
                                            ply_progress_animation_transition_t transition,
                                            double                    duration);
void ply_progress_animation_show (ply_progress_animation_t *progress_animation,
                                  ply_pixel_display_t      *display,
                                  long                      x,
                                  long                      y);
void ply_progress_animation_hide (ply_progress_animation_t *progress_animation);
void ply_progress_animation_draw (ply_progress_animation_t *progress_animation);
void ply_progress_animation_draw_area (ply_progress_animation_t *progress_animation,
                                       ply_pixel_buffer_t       *buffer,
                                       long                      x,
                                       long                      y,
                                       unsigned long             width,
                                       unsigned long             height);
bool ply_progress_animation_is_hidden (ply_progress_animation_t *progress_animation);

long ply_progress_animation_get_width (ply_progress_animation_t *progress_animation);
long ply_progress_animation_get_height (ply_progress_animation_t *progress_animation);

void ply_progress_animation_set_percent_done (ply_progress_animation_t *progress_animation,
                                              double                    percent_done);
double ply_progress_animation_get_percent_done (ply_progress_animation_t *progress_animation);

#endif

#endif /* PROGRESS_ANIMATION_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
