/* throbber.c - boot throbber
 *
 * Copyright (C) 2007, 2008 Red Hat, Inc.
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

#include "ply-throbber.h"
#include "ply-event-loop.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-array.h"
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-utils.h"

#include <linux/kd.h>

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif

#ifndef THROBBER_DURATION
#define THROBBER_DURATION 2.0
#endif

struct _ply_throbber
{
  ply_array_t *frames;
  ply_event_loop_t *loop;
  char *image_dir;
  char *frames_prefix;

  ply_pixel_display_t    *display;
  ply_rectangle_t  frame_area;
  ply_trigger_t *stop_trigger;

  long x, y;
  long width, height;
  double start_time, now;

  int frame_number;
  uint32_t is_stopped : 1;
};

static void ply_throbber_stop_now (ply_throbber_t *throbber);

ply_throbber_t *
ply_throbber_new (const char *image_dir,
              const char *frames_prefix)
{
  ply_throbber_t *throbber;

  assert (image_dir != NULL);
  assert (frames_prefix != NULL);

  throbber = calloc (1, sizeof (ply_throbber_t));

  throbber->frames = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_POINTER);
  throbber->frames_prefix = strdup (frames_prefix);
  throbber->image_dir = strdup (image_dir);
  throbber->is_stopped = true;
  throbber->width = 0;
  throbber->height = 0;
  throbber->frame_area.width = 0;
  throbber->frame_area.height = 0;
  throbber->frame_area.x = 0;
  throbber->frame_area.y = 0;
  throbber->frame_number = 0;

  return throbber;
}

static void
ply_throbber_remove_frames (ply_throbber_t *throbber)
{
  int i;
  ply_pixel_buffer_t **frames;

  frames = (ply_pixel_buffer_t **) ply_array_steal_pointer_elements (throbber->frames);
  for (i = 0; frames[i] != NULL; i++)
    ply_pixel_buffer_free (frames[i]);
  free (frames);
}

void
ply_throbber_free (ply_throbber_t *throbber)
{
  if (throbber == NULL)
    return;

  if (!throbber->is_stopped)
    ply_throbber_stop_now (throbber);

  ply_throbber_remove_frames (throbber);
  ply_array_free (throbber->frames);

  free (throbber->frames_prefix);
  free (throbber->image_dir);
  free (throbber);
}

static bool
animate_at_time (ply_throbber_t *throbber,
                 double      time)
{
  int number_of_frames;
  ply_pixel_buffer_t * const * frames;
  bool should_continue;
  double percent_in_sequence;

  number_of_frames = ply_array_get_size (throbber->frames);

  if (number_of_frames == 0)
    return true;

  should_continue = true;
  percent_in_sequence = fmod (time, THROBBER_DURATION) / THROBBER_DURATION;
  throbber->frame_number = (int) (number_of_frames * percent_in_sequence);

  if (throbber->stop_trigger != NULL)
    {
      if (throbber->frame_number == number_of_frames - 1)
        should_continue = false;
    }

  frames = (ply_pixel_buffer_t * const *) ply_array_get_pointer_elements (throbber->frames);
  ply_pixel_buffer_get_size (frames[throbber->frame_number], &throbber->frame_area);
  throbber->frame_area.x = throbber->x;
  throbber->frame_area.y = throbber->y;
  ply_pixel_display_draw_area (throbber->display,
                               throbber->x, throbber->y,
                               throbber->frame_area.width,
                               throbber->frame_area.height);

  return should_continue;
}

static void
on_timeout (ply_throbber_t *throbber)
{
  double sleep_time;
  bool should_continue;
  throbber->now = ply_get_timestamp ();

#ifdef REAL_TIME_ANIMATION
  should_continue = animate_at_time (throbber,
                                 throbber->now - throbber->start_time);
#else
  static double time = 0.0;
  time += 1.0 / FRAMES_PER_SECOND;
  should_continue = animate_at_time (throbber, time);
#endif

  sleep_time = 1.0 / FRAMES_PER_SECOND;
  sleep_time = MAX (sleep_time - (ply_get_timestamp () - throbber->now),
                    0.005);

  if (!should_continue)
    {
      throbber->is_stopped = true;
      if (throbber->stop_trigger != NULL)
        {
          ply_trigger_pull (throbber->stop_trigger, NULL);
          throbber->stop_trigger = NULL;
        }
    }
  else
    {
      ply_event_loop_watch_for_timeout (throbber->loop,
                                        sleep_time,
                                        (ply_event_loop_timeout_handler_t)
                                        on_timeout, throbber);
    }
}

static bool
ply_throbber_add_frame (ply_throbber_t *throbber,
                        const char     *filename)
{
  ply_image_t *image;
  ply_pixel_buffer_t *frame;

  image = ply_image_new (filename);

  if (!ply_image_load (image))
    {
      ply_image_free (image);
      return false;
    }

  frame = ply_image_convert_to_pixel_buffer (image);

  ply_array_add_pointer_element (throbber->frames, frame);

  throbber->width = MAX (throbber->width, (long) ply_pixel_buffer_get_width (frame));
  throbber->height = MAX (throbber->height, (long)ply_pixel_buffer_get_height (frame));

  return true;
}

static bool
ply_throbber_add_frames (ply_throbber_t *throbber)
{
  struct dirent **entries;
  int number_of_entries;
  int i;
  bool load_finished;

  entries = NULL;

  number_of_entries = scandir (throbber->image_dir, &entries, NULL, versionsort);

  if (number_of_entries < 0)
    return false;

  load_finished = false;
  for (i = 0; i < number_of_entries; i++)
    {
      if (strncmp (entries[i]->d_name,
                   throbber->frames_prefix,
                   strlen (throbber->frames_prefix)) == 0
          && (strlen (entries[i]->d_name) > 4)
          && strcmp (entries[i]->d_name + strlen (entries[i]->d_name) - 4, ".png") == 0)
        {
          char *filename;

          filename = NULL;
          asprintf (&filename, "%s/%s", throbber->image_dir, entries[i]->d_name);

          if (!ply_throbber_add_frame (throbber, filename))
            goto out;

          free (filename);
        }

      free (entries[i]);
      entries[i] = NULL;
    }
  load_finished = true;

out:
  if (!load_finished)
    {
      ply_throbber_remove_frames (throbber);

      while (entries[i] != NULL)
        {
          free (entries[i]);
          i++;
        }
    }
  free (entries);

  return load_finished;
}

bool
ply_throbber_load (ply_throbber_t *throbber)
{
  if (ply_array_get_size (throbber->frames) != 0)
    ply_throbber_remove_frames (throbber);

  if (!ply_throbber_add_frames (throbber))
    return false;

  return true;
}

bool
ply_throbber_start (ply_throbber_t       *throbber,
                    ply_event_loop_t     *loop,
                    ply_pixel_display_t  *display,
                    long                  x,
                    long                  y)
{
  assert (throbber != NULL);
  assert (throbber->loop == NULL);

  throbber->loop = loop;
  throbber->display = display;
  throbber->is_stopped = false;

  throbber->x = x;
  throbber->y = y;

  throbber->start_time = ply_get_timestamp ();

  ply_event_loop_watch_for_timeout (throbber->loop,
                                    1.0 / FRAMES_PER_SECOND,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, throbber);

  return true;
}

static void
ply_throbber_stop_now (ply_throbber_t *throbber)
{
  throbber->is_stopped = true;

  ply_pixel_display_draw_area (throbber->display,
                               throbber->x,
                               throbber->y,
                               throbber->frame_area.width,
                               throbber->frame_area.height);
  if (throbber->loop != NULL)
    {
      ply_event_loop_stop_watching_for_timeout (throbber->loop,
                                                (ply_event_loop_timeout_handler_t)
                                                on_timeout, throbber);
      throbber->loop = NULL;
    }
  throbber->display = NULL;
}

void
ply_throbber_stop (ply_throbber_t *throbber,
                   ply_trigger_t  *stop_trigger)
{

  if (stop_trigger == NULL)
    {
      ply_throbber_stop_now (throbber);
      return;
    }

  throbber->stop_trigger = stop_trigger;
}

bool
ply_throbber_is_stopped (ply_throbber_t *throbber)
{
  return throbber->is_stopped;
}

void
ply_throbber_draw_area (ply_throbber_t     *throbber,
                        ply_pixel_buffer_t *buffer,
                        long                x,
                        long                y,
                        unsigned long       width,
                        unsigned long       height)
{
  ply_pixel_buffer_t * const * frames;

  if (throbber->is_stopped)
    return;

  frames = (ply_pixel_buffer_t * const *) ply_array_get_pointer_elements (throbber->frames);
  ply_pixel_buffer_fill_with_buffer (buffer,
                                     frames[throbber->frame_number],
                                     throbber->x,
                                     throbber->y);
}

long
ply_throbber_get_width (ply_throbber_t *throbber)
{
  return throbber->width;
}

long
ply_throbber_get_height (ply_throbber_t *throbber)
{
  return throbber->height;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
