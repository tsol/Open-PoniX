/* ply-pixel-display.c - APIs for putting up a pixel
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
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-pixel-display.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-pixel-buffer.h"
#include "ply-renderer.h"
#include "ply-utils.h"

struct _ply_pixel_display
{
  ply_event_loop_t *loop;

  ply_renderer_t *renderer;
  ply_renderer_head_t *head;

  unsigned long width;
  unsigned long height;

  ply_pixel_display_draw_handler_t draw_handler;
  void *draw_handler_user_data;

  int pause_count;

};

ply_pixel_display_t *
ply_pixel_display_new (ply_renderer_t      *renderer,
                       ply_renderer_head_t *head)
{
  ply_pixel_display_t *display;
  ply_pixel_buffer_t  *pixel_buffer;
  ply_rectangle_t      size;

  display = calloc (1, sizeof (ply_pixel_display_t));

  display->loop = ply_event_loop_get_default ();
  display->renderer = renderer;
  display->head = head;

  pixel_buffer = ply_renderer_get_buffer_for_head (renderer, head);
  ply_pixel_buffer_get_size (pixel_buffer, &size);

  display->width = size.width;
  display->height = size.height;

  return display;
}

unsigned long
ply_pixel_display_get_width (ply_pixel_display_t *display)
{
  return display->width;
}

unsigned long
ply_pixel_display_get_height (ply_pixel_display_t *display)
{
  return display->height;
}

static void
ply_pixel_display_flush (ply_pixel_display_t *display)
{
  if (display->pause_count > 0)
    return;

  ply_renderer_flush_head (display->renderer, display->head);
}

void
ply_pixel_display_pause_updates (ply_pixel_display_t *display)
{
  assert (display != NULL);

  display->pause_count++;
}

void
ply_pixel_display_unpause_updates (ply_pixel_display_t *display)
{
  assert (display != NULL);

  display->pause_count--;

  ply_pixel_display_flush (display);
}

void
ply_pixel_display_draw_area (ply_pixel_display_t *display,
                             int                  x,
                             int                  y,
                             int                  width,
                             int                  height)
{

  ply_pixel_buffer_t *pixel_buffer;

  pixel_buffer = ply_renderer_get_buffer_for_head (display->renderer,
                                                   display->head);

  if (display->draw_handler != NULL)
    {
      ply_rectangle_t clip_area;

      clip_area.x = x;
      clip_area.y = y;
      clip_area.width = width;
      clip_area.height = height;
      ply_pixel_buffer_push_clip_area (pixel_buffer, &clip_area);
      display->draw_handler (display->draw_handler_user_data,
                             pixel_buffer,
                             x, y, width, height, display);
      ply_pixel_buffer_pop_clip_area (pixel_buffer);
    }

  ply_pixel_display_flush (display);
}

void
ply_pixel_display_free (ply_pixel_display_t *display)
{
  if (display == NULL)
    return;

  free (display);
}

void
ply_pixel_display_set_draw_handler (ply_pixel_display_t *display,
                                    ply_pixel_display_draw_handler_t draw_handler,
                                    void                *user_data)
{
  assert (display != NULL);

  display->draw_handler = draw_handler;
  display->draw_handler_user_data = user_data;
}

/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
