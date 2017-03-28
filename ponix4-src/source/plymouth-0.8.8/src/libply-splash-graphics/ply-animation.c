/* animation.c - boot animation
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
 * Written by: William Jon McCann <jmccann@redhat.com>
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

#include "ply-animation.h"
#include "ply-event-loop.h"
#include "ply-array.h"
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-pixel-buffer.h"
#include "ply-utils.h"

#include <linux/kd.h>

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif

struct _ply_animation
{
  ply_array_t *frames;
  ply_event_loop_t *loop;
  char *image_dir;
  char *frames_prefix;

  ply_pixel_display_t     *display;
  ply_rectangle_t          frame_area;
  ply_trigger_t *stop_trigger;

  int frame_number;
  long x, y;
  long width, height;
  double start_time, previous_time, now;
  uint32_t is_stopped : 1;
  uint32_t stop_requested : 1;
};

static void ply_animation_stop_now (ply_animation_t *animation);


ply_animation_t *
ply_animation_new (const char *image_dir,
                   const char *frames_prefix)
{
  ply_animation_t *animation;

  assert (image_dir != NULL);
  assert (frames_prefix != NULL);

  animation = calloc (1, sizeof (ply_animation_t));

  animation->frames = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_POINTER);
  animation->frames_prefix = strdup (frames_prefix);
  animation->image_dir = strdup (image_dir);
  animation->frame_number = 0;
  animation->is_stopped = true;
  animation->stop_requested = false;
  animation->width = 0;
  animation->height = 0;
  animation->frame_area.width = 0;
  animation->frame_area.height = 0;
  animation->frame_area.x = 0;
  animation->frame_area.y = 0;

  return animation;
}

static void
ply_animation_remove_frames (ply_animation_t *animation)
{
  int i;
  ply_pixel_buffer_t **frames;

  frames = (ply_pixel_buffer_t **) ply_array_steal_pointer_elements (animation->frames);
  for (i = 0; frames[i] != NULL; i++)
    ply_pixel_buffer_free (frames[i]);
  free (frames);
}

void
ply_animation_free (ply_animation_t *animation)
{
  if (animation == NULL)
    return;

  if (!animation->is_stopped)
    ply_animation_stop_now (animation);

  ply_animation_remove_frames (animation);
  ply_array_free (animation->frames);

  free (animation->frames_prefix);
  free (animation->image_dir);
  free (animation);
}

static bool
animate_at_time (ply_animation_t *animation,
                 double           time)
{
  int number_of_frames;
  ply_pixel_buffer_t * const * frames;
  bool should_continue;

  number_of_frames = ply_array_get_size (animation->frames);

  if (number_of_frames == 0)
    return false;

  should_continue = true;

  if (animation->frame_number > number_of_frames - 1)
    {
      ply_trace ("reached last frame of animation");
      return false;
    }

  if (animation->stop_requested)
    {
      ply_trace ("stopping animation in the middle of sequence");
      should_continue = false;
    }

  frames = (ply_pixel_buffer_t * const *) ply_array_get_pointer_elements (animation->frames);
  ply_pixel_buffer_get_size (frames[animation->frame_number], &animation->frame_area);
  animation->frame_area.x = animation->x;
  animation->frame_area.y = animation->y;

  ply_pixel_display_draw_area (animation->display,
                               animation->x, animation->y,
                               animation->frame_area.width,
                               animation->frame_area.height);

  animation->frame_number++;

  return should_continue;
}

static void
on_timeout (ply_animation_t *animation)
{
  double sleep_time;
  bool should_continue;
  animation->previous_time = animation->now;
  animation->now = ply_get_timestamp ();

#ifdef REAL_TIME_ANIMATION
  should_continue = animate_at_time (animation,
                                     animation->now - animation->start_time);
#else
  static double time = 0.0;
  time += 1.0 / FRAMES_PER_SECOND;
  should_continue = animate_at_time (animation, time);
#endif

  sleep_time = 1.0 / FRAMES_PER_SECOND;
  sleep_time = MAX (sleep_time - (ply_get_timestamp () - animation->now),
                    0.005);

  if (!should_continue)
    {
      if (animation->stop_trigger != NULL)
        {
          ply_trace ("firing off stop trigger");
          ply_trigger_pull (animation->stop_trigger, NULL);
          animation->stop_trigger = NULL;
        }
    }
  else
    {
      ply_event_loop_watch_for_timeout (animation->loop,
                                        sleep_time,
                                        (ply_event_loop_timeout_handler_t)
                                        on_timeout, animation);
    }
}

static bool
ply_animation_add_frame (ply_animation_t *animation,
                         const char      *filename)
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

  ply_array_add_pointer_element (animation->frames, frame);

  animation->width = MAX (animation->width, (long) ply_pixel_buffer_get_width (frame));
  animation->height = MAX (animation->height,  (long) ply_pixel_buffer_get_height (frame));

  return true;
}

static bool
ply_animation_add_frames (ply_animation_t *animation)
{
  struct dirent **entries;
  int number_of_entries;
  int number_of_frames;
  int i;
  bool load_finished;

  entries = NULL;

  number_of_entries = scandir (animation->image_dir, &entries, NULL, versionsort);

  if (number_of_entries <= 0)
    return false;

  load_finished = false;
  for (i = 0; i < number_of_entries; i++)
    {
      if (strncmp (entries[i]->d_name,
                   animation->frames_prefix,
                   strlen (animation->frames_prefix)) == 0
          && (strlen (entries[i]->d_name) > 4)
          && strcmp (entries[i]->d_name + strlen (entries[i]->d_name) - 4, ".png") == 0)
        {
          char *filename;

          filename = NULL;
          asprintf (&filename, "%s/%s", animation->image_dir, entries[i]->d_name);

          if (!ply_animation_add_frame (animation, filename))
            goto out;

          free (filename);
        }

      free (entries[i]);
      entries[i] = NULL;
    }

  number_of_frames = ply_array_get_size (animation->frames);
  if (number_of_frames == 0)
    {
      ply_trace ("%s directory had no files starting with %s\n",
                 animation->image_dir, animation->frames_prefix);
      goto out;
    }
  else
    {
      ply_trace ("animation has %d frames\n", number_of_frames);
    }

  load_finished = true;

out:
  if (!load_finished)
    {
      ply_animation_remove_frames (animation);

      while (i < number_of_entries)
        {
          free (entries[i]);
          i++;
        }
    }
  free (entries);

  return load_finished;
}

bool
ply_animation_load (ply_animation_t *animation)
{
  if (ply_array_get_size (animation->frames) != 0)
    {
      ply_animation_remove_frames (animation);
      ply_trace ("reloading animation with new set of frames");
    }
  else
    {
      ply_trace ("loading frames for animation");
    }

  if (!ply_animation_add_frames (animation))
    return false;

  return true;
}

bool
ply_animation_start (ply_animation_t    *animation,
                     ply_pixel_display_t *display,
                     ply_trigger_t      *stop_trigger,
                     long                x,
                     long                y)
{
  assert (animation != NULL);

  if (!animation->is_stopped)
    return true;

  ply_trace ("starting animation");

  animation->loop = ply_event_loop_get_default ();
  animation->display = display;
  animation->stop_trigger = stop_trigger;
  animation->is_stopped = false;
  animation->stop_requested = false;

  animation->x = x;
  animation->y = y;

  animation->start_time = ply_get_timestamp ();

  ply_event_loop_watch_for_timeout (animation->loop,
                                    1.0 / FRAMES_PER_SECOND,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, animation);

  return true;
}

static void
ply_animation_stop_now (ply_animation_t *animation)
{
  animation->is_stopped = true;

  ply_trace ("stopping animation now");

  if (animation->loop != NULL)
    {
      ply_event_loop_stop_watching_for_timeout (animation->loop,
                                                (ply_event_loop_timeout_handler_t)
                                                on_timeout, animation);
      animation->loop = NULL;
    }

  animation->display = NULL;
}

void
ply_animation_stop (ply_animation_t *animation)
{
  if (animation->stop_trigger == NULL)
    {
      ply_animation_stop_now (animation);
      return;
    }

  ply_trace ("stopping animation next time through the loop");
  animation->stop_requested = true;
}

bool
ply_animation_is_stopped (ply_animation_t *animation)
{
  return animation->is_stopped;
}

void
ply_animation_draw_area (ply_animation_t    *animation,
                         ply_pixel_buffer_t *buffer,
                         long                x,
                         long                y,
                         unsigned long       width,
                         unsigned long       height)
{
  ply_pixel_buffer_t * const * frames;
  int number_of_frames;
  int frame_index;
  
  if (animation->is_stopped)
    return;

  number_of_frames = ply_array_get_size (animation->frames);
  frame_index = MIN(animation->frame_number, number_of_frames - 1);

  frames = (ply_pixel_buffer_t * const *) ply_array_get_pointer_elements (animation->frames);
  ply_pixel_buffer_fill_with_buffer (buffer,
                                     frames[frame_index],
                                     animation->frame_area.x,
                                     animation->frame_area.y);
}

long
ply_animation_get_width (ply_animation_t *animation)
{
  return animation->width;
}

long
ply_animation_get_height (ply_animation_t *animation)
{
  return animation->height;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
