/* ply-progress-bar.h - simple text bar field
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
 *             Will Woods <wwoods@redhat.com>
 */
#ifndef PLY_PROGRESS_BAR_H
#define PLY_PROGRESS_BAR_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"

typedef struct _ply_progress_bar ply_progress_bar_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_progress_bar_t *ply_progress_bar_new (void);
void ply_progress_bar_free (ply_progress_bar_t *bar);

void ply_progress_bar_show (ply_progress_bar_t  *bar,
                            ply_pixel_display_t *display,
                            long                 x,
                            long                 y);
void ply_progress_bar_hide (ply_progress_bar_t *bar);
void ply_progress_bar_draw (ply_progress_bar_t *bar);
void ply_progress_bar_draw_area (ply_progress_bar_t *bar,
                                 ply_pixel_buffer_t *buffer,
                                 long                x,
                                 long                y,
                                 unsigned long       width,
                                 unsigned long       height);
bool ply_progress_bar_is_hidden (ply_progress_bar_t *bar);

long ply_progress_bar_get_width (ply_progress_bar_t *bar);
long ply_progress_bar_get_height (ply_progress_bar_t *bar);

void ply_progress_bar_set_percent_done (ply_progress_bar_t *bar,
                                        double              percent_done);
double ply_progress_bar_get_percent_done (ply_progress_bar_t *bar);
#endif

#endif /* PLY_PROGRESS_BAR_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
