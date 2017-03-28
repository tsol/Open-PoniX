/* progress_bar.c - boot progress_bar
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
 *             Will Woods <wwoods@redhat.com>
 */
#include "config.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <values.h>
#include <unistd.h>
#include <wchar.h>

#include "ply-progress-bar.h"
#include "ply-event-loop.h"
#include "ply-array.h"
#include "ply-logger.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-image.h"
#include "ply-utils.h"

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif

#ifndef BAR_HEIGHT
#define BAR_HEIGHT 16
#endif

struct _ply_progress_bar
{
  ply_pixel_display_t    *display;
  ply_rectangle_t  area;

  double percent_done;

  uint32_t is_hidden : 1;
};

ply_progress_bar_t *
ply_progress_bar_new (void)
{
  ply_progress_bar_t *progress_bar;

  progress_bar = calloc (1, sizeof (ply_progress_bar_t));

  progress_bar->is_hidden = true;
  progress_bar->percent_done = 0.0;
  progress_bar->area.x = 0;
  progress_bar->area.y = 0;
  progress_bar->area.width = 0;
  progress_bar->area.height = BAR_HEIGHT;

  return progress_bar;
}

void
ply_progress_bar_free (ply_progress_bar_t *progress_bar)
{
  if (progress_bar == NULL)
    return;
  free (progress_bar);
}

static void
ply_progress_bar_update_area (ply_progress_bar_t *progress_bar,
                              long                x,
                              long                y)
{
  unsigned long display_width;

  progress_bar->area.x = x;
  progress_bar->area.y = y;
  progress_bar->area.height = BAR_HEIGHT;

  display_width = ply_pixel_display_get_width (progress_bar->display);
  progress_bar->area.width = (long) (display_width * progress_bar->percent_done);
}

void
ply_progress_bar_draw_area (ply_progress_bar_t *progress_bar,
                            ply_pixel_buffer_t *buffer,
                            long                x,
                            long                y,
                            unsigned long       width,
                            unsigned long       height)
{
  ply_rectangle_t paint_area;

  if (progress_bar->is_hidden)
    return;

  paint_area.x = x;
  paint_area.y = y;
  paint_area.width = width;
  paint_area.height = height;

  ply_rectangle_intersect (&progress_bar->area, &paint_area, &paint_area);
  ply_pixel_buffer_fill_with_hex_color (buffer,
                                        &paint_area,
                                        0xffffff); /* white */
}

void
ply_progress_bar_draw (ply_progress_bar_t *progress_bar)
{
  if (progress_bar->is_hidden)
    return;

  ply_progress_bar_update_area (progress_bar, progress_bar->area.x, progress_bar->area.y);
  ply_pixel_display_draw_area (progress_bar->display,
                               progress_bar->area.x,
                               progress_bar->area.y,
                               progress_bar->area.width,
                               progress_bar->area.height);
}

void
ply_progress_bar_show (ply_progress_bar_t  *progress_bar,
                       ply_pixel_display_t *display,
                       long                 x,
                       long                 y)
{
  assert (progress_bar != NULL);

  progress_bar->display = display;

  ply_progress_bar_update_area (progress_bar, x, y);

  progress_bar->is_hidden = false;
  ply_progress_bar_draw (progress_bar);
}

void
ply_progress_bar_hide (ply_progress_bar_t *progress_bar)
{
  if (progress_bar->is_hidden)
    return;

  progress_bar->is_hidden = true;
  ply_pixel_display_draw_area (progress_bar->display,
                               progress_bar->area.x, progress_bar->area.y,
                               progress_bar->area.width, progress_bar->area.height);

  progress_bar->display = NULL;

}

bool
ply_progress_bar_is_hidden (ply_progress_bar_t *progress_bar)
{
  return progress_bar->is_hidden;
}

long
ply_progress_bar_get_width (ply_progress_bar_t *progress_bar)
{
  return progress_bar->area.width;
}

long
ply_progress_bar_get_height (ply_progress_bar_t *progress_bar)
{
  return progress_bar->area.height;
}

void
ply_progress_bar_set_percent_done (ply_progress_bar_t *progress_bar,
                                   double              percent_done)
{
  progress_bar->percent_done = percent_done;
}

double
ply_progress_bar_get_percent_done (ply_progress_bar_t *progress_bar)
{
  return progress_bar->percent_done;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
