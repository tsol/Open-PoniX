/* ply-pixel-display.h - APIs for displaying pixels
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_PIXEL_DISPLAY_H
#define PLY_PIXEL_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-pixel-buffer.h"
#include "ply-renderer.h"

typedef struct _ply_pixel_display ply_pixel_display_t;

typedef void (* ply_pixel_display_draw_handler_t) (void               *user_data,
                                                   ply_pixel_buffer_t *pixel_buffer,
                                                   int                 x,
                                                   int                 y,
                                                   int                 width,
                                                   int                 height,
                                                   ply_pixel_display_t *pixel_display);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_pixel_display_t *ply_pixel_display_new (ply_renderer_t      *renderer,
                                            ply_renderer_head_t *head);

void ply_pixel_display_free (ply_pixel_display_t *display);

unsigned long ply_pixel_display_get_width  (ply_pixel_display_t *display);
unsigned long ply_pixel_display_get_height (ply_pixel_display_t *display);

void ply_pixel_display_set_draw_handler (ply_pixel_display_t              *display,
                                         ply_pixel_display_draw_handler_t  draw_handler,
                                         void                             *user_data);

void ply_pixel_display_draw_area (ply_pixel_display_t *display,
                                  int                  x,
                                  int                  y,
                                  int                  width,
                                  int                  height);

void ply_pixel_display_pause_updates (ply_pixel_display_t *display);
void ply_pixel_display_unpause_updates (ply_pixel_display_t *display);

#endif

#endif /* PLY_PIXEL_DISPLAY_H */
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
