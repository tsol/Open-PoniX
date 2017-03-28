/* progress-animation.c - boot progress animation
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
 *
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

#include "ply-progress-animation.h"
#include "ply-array.h"
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-utils.h"

#include <linux/kd.h>

struct _ply_progress_animation
{
  ply_array_t *frames;
  char *image_dir;
  char *frames_prefix;

  ply_progress_animation_transition_t transition;
  double transition_duration;

  ply_pixel_display_t *display;
  ply_rectangle_t  area;
  ply_rectangle_t  frame_area;

  double percent_done;
  int previous_frame_number;

  double transition_start_time;

  ply_pixel_buffer_t *last_rendered_frame;

  uint32_t is_hidden : 1;
  uint32_t is_transitioning : 1;
};

ply_progress_animation_t *
ply_progress_animation_new (const char *image_dir,
                            const char *frames_prefix)
{
  ply_progress_animation_t *progress_animation;

  assert (image_dir != NULL);
  assert (frames_prefix != NULL);

  progress_animation = calloc (1, sizeof (ply_progress_animation_t));

  progress_animation->frames = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_POINTER);
  progress_animation->frames_prefix = strdup (frames_prefix);
  progress_animation->image_dir = strdup (image_dir);
  progress_animation->is_hidden = true;
  progress_animation->percent_done = 0.0;
  progress_animation->area.x = 0;
  progress_animation->area.y = 0;
  progress_animation->area.width = 0;
  progress_animation->area.height = 0;
  progress_animation->frame_area.x = 0;
  progress_animation->frame_area.y = 0;
  progress_animation->frame_area.width = 0;
  progress_animation->frame_area.height = 0;
  progress_animation->previous_frame_number = 0;
  progress_animation->last_rendered_frame = NULL;

  return progress_animation;
}

void
ply_progress_animation_set_transition (ply_progress_animation_t *progress_animation,
                                       ply_progress_animation_transition_t transition,
                                       double                    duration)
{
  progress_animation->transition = transition;
  progress_animation->transition_duration = duration;
}

static void
ply_progress_animation_remove_frames (ply_progress_animation_t *progress_animation)
{
  int i;
  ply_image_t **frames;

  frames = (ply_image_t **) ply_array_steal_pointer_elements (progress_animation->frames);
  for (i = 0; frames[i] != NULL; i++)
    ply_image_free (frames[i]);
  free (frames);
}

void
ply_progress_animation_free (ply_progress_animation_t *progress_animation)
{
  if (progress_animation == NULL)
    return;

  ply_progress_animation_remove_frames (progress_animation);
  ply_array_free (progress_animation->frames);

  free (progress_animation->frames_prefix);
  free (progress_animation->image_dir);
  free (progress_animation);
}

static void
image_fade_merge (ply_image_t* frame0,
                  ply_image_t* frame1,
                  float        fade,
                  int          width,
                  int          height,
                  uint32_t    *reply_data)
{
  int frame0_width = ply_image_get_width (frame0);
  int frame0_height = ply_image_get_height (frame0);
  int frame1_width = ply_image_get_width (frame1);
  int frame1_height = ply_image_get_height (frame1);

  uint32_t *frame0_data = ply_image_get_data (frame0);
  uint32_t *frame1_data = ply_image_get_data (frame1);

  int x, y, i;

  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          uint32_t pixel0, pixel1, pixelout;
          
          if (y < frame0_height && x < frame0_width)
            pixel0 = frame0_data[y*frame0_width+x];
          else 
            pixel0 = 0;
          
          if (y < frame1_height && x < frame1_width)
            pixel1 = frame1_data[y*frame1_width+x];
          else 
            pixel1 = 0;
            
          pixelout = 0;
          for (i = 0; i < 4; i++)
            {
              int subval0 = (pixel0 >> (i * 8)) & 0xFF;
              int subval1 = (pixel1 >> (i * 8)) & 0xFF;
              int subvalout = subval0 * (1-fade) + subval1 * fade;
              pixelout |= (subvalout & 0xFF) << (i * 8);
            }
          reply_data[y*width+x] = pixelout;
        }
    }
}

void
ply_progress_animation_draw_area (ply_progress_animation_t *progress_animation,
                                  ply_pixel_buffer_t       *buffer,
                                  long                      x,
                                  long                      y,
                                  unsigned long             width,
                                  unsigned long             height)
{
  if (progress_animation->is_hidden)
    return;

  ply_pixel_buffer_fill_with_buffer (buffer,
                                     progress_animation->last_rendered_frame,
                                     progress_animation->frame_area.x,
                                     progress_animation->frame_area.y);
}

void
ply_progress_animation_draw (ply_progress_animation_t *progress_animation)
{
  int number_of_frames;
  int frame_number;
  ply_image_t * const * frames;
  ply_pixel_buffer_t *previous_frame_buffer, *current_frame_buffer;

  if (progress_animation->is_hidden)
    return;

  number_of_frames = ply_array_get_size (progress_animation->frames);

  if (number_of_frames == 0)
    return;

  frame_number = progress_animation->percent_done * (number_of_frames - 1);

  if (progress_animation->previous_frame_number != frame_number &&
      progress_animation->transition != PLY_PROGRESS_ANIMATION_TRANSITION_NONE &&
      progress_animation->transition_duration > 0.0)
    {
      progress_animation->is_transitioning = true;
      progress_animation->transition_start_time = ply_get_timestamp ();
    }

  frames = (ply_image_t * const *) ply_array_get_pointer_elements (progress_animation->frames);

  progress_animation->frame_area.x = progress_animation->area.x;
  progress_animation->frame_area.y = progress_animation->area.y;
  current_frame_buffer = ply_image_get_buffer (frames[frame_number]);

  if (progress_animation->is_transitioning)
    {
      double now;
      double fade_percentage;
      double fade_out_opacity;
      int width, height;
      uint32_t* faded_data;
      now = ply_get_timestamp ();

      fade_percentage = (now - progress_animation->transition_start_time) / progress_animation->transition_duration;

      if (fade_percentage >= 1.0)
        progress_animation->is_transitioning = false;
      fade_percentage = CLAMP (fade_percentage, 0.0, 1.0);

      if (progress_animation->transition == PLY_PROGRESS_ANIMATION_TRANSITION_MERGE_FADE)
        {
          width = MAX(ply_image_get_width (frames[frame_number]), ply_image_get_width (frames[frame_number - 1]));
          height = MAX(ply_image_get_height (frames[frame_number]), ply_image_get_height (frames[frame_number - 1]));
          progress_animation->frame_area.width = width;
          progress_animation->frame_area.height = height;

          ply_pixel_buffer_free (progress_animation->last_rendered_frame);
          progress_animation->last_rendered_frame = ply_pixel_buffer_new (width, height);
          faded_data = ply_pixel_buffer_get_argb32_data (progress_animation->last_rendered_frame);

          image_fade_merge (frames[frame_number - 1], frames[frame_number], fade_percentage, width, height, faded_data);
        }
      else
        {
          previous_frame_buffer = ply_image_get_buffer (frames[frame_number - 1]);
          if (progress_animation->transition == PLY_PROGRESS_ANIMATION_TRANSITION_FADE_OVER)
            {
              ply_pixel_buffer_free (progress_animation->last_rendered_frame);
              progress_animation->last_rendered_frame = ply_pixel_buffer_new (ply_image_get_width (frames[frame_number - 1]),
                                                                              ply_image_get_height (frames[frame_number - 1]));
              ply_pixel_buffer_fill_with_buffer (progress_animation->last_rendered_frame,
                                                 previous_frame_buffer,
                                                 0,
                                                 0);
            }
          else
            {
              fade_out_opacity = 1.0 - fade_percentage;
              ply_pixel_buffer_fill_with_buffer_at_opacity (progress_animation->last_rendered_frame,
                                                            previous_frame_buffer,
                                                            0,
                                                            0,
                                                            fade_out_opacity);
            }

          ply_pixel_buffer_fill_with_buffer_at_opacity (progress_animation->last_rendered_frame,
                                                        current_frame_buffer,
                                                        0,
                                                        0,
                                                        fade_percentage);

          width = MAX(ply_image_get_width (frames[frame_number]), ply_image_get_width (frames[frame_number - 1]));
          height = MAX(ply_image_get_height (frames[frame_number]), ply_image_get_height (frames[frame_number - 1]));
          progress_animation->frame_area.width = width;
          progress_animation->frame_area.height = height;
        }
    }
  else
    {
      ply_pixel_buffer_free (progress_animation->last_rendered_frame);
      progress_animation->frame_area.width = ply_image_get_width (frames[frame_number]);
      progress_animation->frame_area.height = ply_image_get_height (frames[frame_number]);
      progress_animation->last_rendered_frame = ply_pixel_buffer_new (progress_animation->frame_area.width,
                                                                      progress_animation->frame_area.height);

      ply_pixel_buffer_fill_with_buffer (progress_animation->last_rendered_frame,
                                         current_frame_buffer,
                                         0,
                                         0);
    }

  progress_animation->previous_frame_number = frame_number;

  ply_pixel_display_draw_area (progress_animation->display,
                               progress_animation->frame_area.x,
                               progress_animation->frame_area.y,
                               progress_animation->frame_area.width,
                               progress_animation->frame_area.height);
}

static bool
ply_progress_animation_add_frame (ply_progress_animation_t *progress_animation,
                                  const char               *filename)
{
  ply_image_t *image;

  image = ply_image_new (filename);

  if (!ply_image_load (image))
    {
      ply_image_free (image);
      return false;
    }

  ply_array_add_pointer_element (progress_animation->frames, image);

  progress_animation->area.width = MAX (progress_animation->area.width, (size_t) ply_image_get_width (image));
  progress_animation->area.height = MAX (progress_animation->area.height, (size_t) ply_image_get_height (image));

  return true;
}

static bool
ply_progress_animation_add_frames (ply_progress_animation_t *progress_animation)
{
  struct dirent **entries;
  int number_of_entries;
  int number_of_frames;
  int i;
  bool load_finished;

  entries = NULL;

  number_of_entries = scandir (progress_animation->image_dir, &entries, NULL, versionsort);

  if (number_of_entries < 0)
    return false;

  load_finished = false;
  for (i = 0; i < number_of_entries; i++)
    {
      if (strncmp (entries[i]->d_name,
                   progress_animation->frames_prefix,
                   strlen (progress_animation->frames_prefix)) == 0
          && (strlen (entries[i]->d_name) > 4)
          && strcmp (entries[i]->d_name + strlen (entries[i]->d_name) - 4, ".png") == 0)
        {
          char *filename;
          bool r;

          filename = NULL;
          asprintf (&filename, "%s/%s", progress_animation->image_dir, entries[i]->d_name);

          r = ply_progress_animation_add_frame (progress_animation, filename);
          free (filename);
          if (!r)
            goto out;
        }

      free (entries[i]);
      entries[i] = NULL;
    }

  number_of_frames = ply_array_get_size (progress_animation->frames);
  if (number_of_frames == 0)
    {
      ply_trace ("could not find any progress animation frames");
      load_finished = false;
    }
  else
    {
      ply_trace ("found %d progress animation frames", number_of_frames);
      load_finished = true;
    }

out:
  if (!load_finished)
    {
      ply_progress_animation_remove_frames (progress_animation);

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
ply_progress_animation_load (ply_progress_animation_t *progress_animation)
{
  if (ply_array_get_size (progress_animation->frames) != 0)
    ply_progress_animation_remove_frames (progress_animation);

  if (!ply_progress_animation_add_frames (progress_animation))
    return false;

  return true;
}

void
ply_progress_animation_show (ply_progress_animation_t *progress_animation,
                             ply_pixel_display_t      *display,
                             long                      x,
                             long                      y)
{
  assert (progress_animation != NULL);

  progress_animation->display = display;

  progress_animation->area.x = x;
  progress_animation->area.y = y;

  progress_animation->is_hidden = false;
  ply_progress_animation_draw (progress_animation);
}

void
ply_progress_animation_hide (ply_progress_animation_t *progress_animation)
{
  if (progress_animation->is_hidden)
    return;

  progress_animation->is_hidden = true;
  if (progress_animation->frame_area.width > 0)
    {
      ply_pixel_display_draw_area (progress_animation->display,
                                   progress_animation->area.x, progress_animation->area.y,
                                   progress_animation->frame_area.width,
                                   progress_animation->frame_area.height);
    }

  progress_animation->display = NULL;
}

bool
ply_progress_animation_is_hidden (ply_progress_animation_t *progress_animation)
{
  return progress_animation->is_hidden;
}

long
ply_progress_animation_get_width (ply_progress_animation_t *progress_animation)
{
  return progress_animation->area.width;
}

long
ply_progress_animation_get_height (ply_progress_animation_t *progress_animation)
{
  return progress_animation->area.height;
}

void
ply_progress_animation_set_percent_done (ply_progress_animation_t *progress_animation,
                                         double            percent_done)
{
  progress_animation->percent_done = percent_done;
  ply_progress_animation_draw (progress_animation);
}

double
ply_progress_animation_get_percent_done (ply_progress_animation_t *progress_animation)
{
  return progress_animation->percent_done;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
