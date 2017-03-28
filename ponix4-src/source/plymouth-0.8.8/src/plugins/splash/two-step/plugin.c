/*
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
 * Written by: William Jon McCann
 *
 */
#include "config.h"

#include <assert.h>
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

#include "ply-boot-splash-plugin.h"
#include "ply-buffer.h"
#include "ply-entry.h"
#include "ply-event-loop.h"
#include "ply-label.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-key-file.h"
#include "ply-trigger.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-utils.h"

#include "ply-animation.h"
#include "ply-progress-animation.h"
#include "ply-throbber.h"

#include <linux/kd.h>

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif

#ifndef SHOW_ANIMATION_PERCENT
#define SHOW_ANIMATION_PERCENT 0.9
#endif

typedef enum {
   PLY_BOOT_SPLASH_DISPLAY_NORMAL,
   PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY,
   PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY
} ply_boot_splash_display_type_t;

typedef enum {
   PROGRESS_FUNCTION_TYPE_WWOODS,
   PROGRESS_FUNCTION_TYPE_LINEAR,
} progress_function_t;

typedef struct
{
  ply_boot_splash_plugin_t *plugin;
  ply_pixel_display_t *display;
  ply_entry_t *entry;
  ply_animation_t *end_animation;
  ply_progress_animation_t *progress_animation;
  ply_throbber_t *throbber;
  ply_label_t *label;
  ply_label_t *message_label;
  ply_rectangle_t box_area, lock_area;
  ply_trigger_t *end_trigger;
} view_t;

struct _ply_boot_splash_plugin
{
  ply_event_loop_t *loop;
  ply_boot_splash_mode_t mode;
  ply_image_t *lock_image;
  ply_image_t *box_image;
  ply_image_t *corner_image;
  ply_image_t *header_image;
  ply_list_t *views;

  ply_boot_splash_display_type_t state;

  double animation_horizontal_alignment;
  double animation_vertical_alignment;
  char *animation_dir;

  ply_progress_animation_transition_t transition;
  double transition_duration;

  uint32_t background_start_color;
  uint32_t background_end_color;

  progress_function_t progress_function;

  ply_trigger_t *idle_trigger;
  ply_trigger_t *stop_trigger;

  uint32_t root_is_mounted : 1;
  uint32_t is_visible : 1;
  uint32_t is_animating : 1;
  uint32_t is_idle : 1;
};

ply_boot_splash_plugin_interface_t * ply_boot_splash_plugin_get_interface (void);

static void stop_animation (ply_boot_splash_plugin_t *plugin,
                            ply_trigger_t            *idle_trigger);

static void detach_from_event_loop (ply_boot_splash_plugin_t *plugin);
static void display_message (ply_boot_splash_plugin_t *plugin,
                             const char               *message);
static void become_idle (ply_boot_splash_plugin_t *plugin,
                         ply_trigger_t            *idle_trigger);

static view_t *
view_new (ply_boot_splash_plugin_t *plugin,
          ply_pixel_display_t      *display)
{
  view_t *view;

  view = calloc (1, sizeof (view_t));
  view->plugin = plugin;
  view->display = display;

  view->entry = ply_entry_new (plugin->animation_dir);
  view->end_animation = ply_animation_new (plugin->animation_dir,
                                           "animation-");
  view->progress_animation = ply_progress_animation_new (plugin->animation_dir,
                                                         "progress-");

  view->throbber = ply_throbber_new (plugin->animation_dir,
                                     "throbber-");
  ply_progress_animation_set_transition (view->progress_animation,
                                         plugin->transition,
                                         plugin->transition_duration);

  view->label = ply_label_new ();
  view->message_label = ply_label_new ();

  return view;
}

static void
view_free (view_t *view)
{

  ply_entry_free (view->entry);
  ply_animation_free (view->end_animation);
  ply_progress_animation_free (view->progress_animation);
  ply_throbber_free (view->throbber);
  ply_label_free (view->label);
  ply_label_free (view->message_label);

  free (view);
}

static bool
view_load (view_t *view)
{
  ply_trace ("loading entry");
  if (!ply_entry_load (view->entry))
    return false;

  ply_trace ("loading animation");
  if (!ply_animation_load (view->end_animation))
    {
      ply_trace ("Default animation wouldn't load, "
                 "falling back to old naming scheme");

      /* fallback to throbber- for compatibility
       */
      ply_animation_free (view->end_animation);
      view->end_animation = ply_animation_new (view->plugin->animation_dir,
                                               "throbber-");
      if (!ply_animation_load (view->end_animation))
        {
          ply_trace ("old naming scheme didn't work either");
          return false;
        }

      ply_throbber_free (view->throbber);
      view->throbber = NULL;
    }

  ply_trace ("loading progress animation");
  if (!ply_progress_animation_load (view->progress_animation))
    {
      ply_trace ("optional progress animation wouldn't load");
      ply_progress_animation_free (view->progress_animation);
      view->progress_animation = NULL;
    }

  if (view->throbber != NULL)
    {
      ply_trace ("loading throbber");
      if (!ply_throbber_load (view->throbber))
        {
          ply_trace ("optional throbber was not loaded");
          ply_throbber_free (view->throbber);
          view->throbber = NULL;
        }
    }
  else
    {
      ply_trace ("this theme has no throbber\n");
    }

  return true;
}

static bool
load_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;
  bool view_loaded;

  view_loaded = false;
  node = ply_list_get_first_node (plugin->views);

  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      if (view_load (view))
        view_loaded = true;

      node = next_node;
    }

  return view_loaded;
}

static void
view_redraw (view_t *view)
{
  unsigned long screen_width, screen_height;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  ply_pixel_display_draw_area (view->display, 0, 0,
                               screen_width, screen_height);
}

static void
redraw_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_redraw (view);

      node = next_node;
    }
}

static void
pause_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  ply_trace ("pausing views");

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      ply_pixel_display_pause_updates (view->display);

      node = next_node;
    }
}

static void
unpause_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  ply_trace ("unpausing views");

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      ply_pixel_display_unpause_updates (view->display);

      node = next_node;
    }
}

static void
view_start_end_animation (view_t        *view,
                          ply_trigger_t *trigger)
{
  ply_boot_splash_plugin_t *plugin;

  long x, y;
  long width, height;
  unsigned long screen_width, screen_height;

  plugin = view->plugin;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);
  width = ply_animation_get_width (view->end_animation);
  height = ply_animation_get_height (view->end_animation);
  x = plugin->animation_horizontal_alignment * screen_width - width / 2.0;
  y = plugin->animation_vertical_alignment * screen_height - height / 2.0;

  ply_trace ("starting end sequence animation for %ldx%ld view", width, height);
  ply_animation_start (view->end_animation,
                       view->display,
                       trigger, x, y);
}

static void
on_view_throbber_stopped (view_t *view)
{
  ply_trace ("hiding progress animation");
  if (view->progress_animation != NULL)
      ply_progress_animation_hide (view->progress_animation);
  view_start_end_animation (view, view->end_trigger);
  view->end_trigger = NULL;
}

static void
view_start_progress_animation (view_t *view)
{
  ply_boot_splash_plugin_t *plugin;

  long x, y;
  long width, height;
  unsigned long screen_width, screen_height;

  assert (view != NULL);

  plugin = view->plugin;

  plugin->is_idle = false;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  ply_pixel_display_draw_area (view->display, 0, 0,
                               screen_width, screen_height);

  if (view->throbber != NULL)
    {
      width = ply_throbber_get_width (view->throbber);
      height = ply_throbber_get_height (view->throbber);
      x = plugin->animation_horizontal_alignment * screen_width - width / 2.0;
      y = plugin->animation_vertical_alignment * screen_height - height / 2.0;
      ply_throbber_start (view->throbber,
                          plugin->loop,
                          view->display, x, y);
      ply_pixel_display_draw_area (view->display, x, y, width, height);
    }

  /* We don't really know how long shutdown will so
   * don't show the progress animation
   */
  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    return;

  if (view->progress_animation != NULL)
    {
      width = ply_progress_animation_get_width (view->progress_animation);
      height = ply_progress_animation_get_height (view->progress_animation);
      x = plugin->animation_horizontal_alignment * screen_width - width / 2.0;
      y = plugin->animation_vertical_alignment * screen_height - height / 2.0;
      ply_progress_animation_show (view->progress_animation,
                                   view->display, x, y);

      ply_pixel_display_draw_area (view->display, x, y, width, height);
    }
}

static void
view_show_prompt (view_t     *view,
                  const char *prompt)
{
  ply_boot_splash_plugin_t *plugin;
  int x, y;
  int entry_width, entry_height;
  unsigned long screen_width, screen_height;

  assert (view != NULL);

  plugin = view->plugin;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  if (ply_entry_is_hidden (view->entry))
    {
      view->box_area.width = ply_image_get_width (plugin->box_image);
      view->box_area.height = ply_image_get_height (plugin->box_image);
      view->box_area.x = screen_width / 2.0 - view->box_area.width / 2.0;
      view->box_area.y = screen_height / 2.0 - view->box_area.height / 2.0;

      view->lock_area.width = ply_image_get_width (plugin->lock_image);
      view->lock_area.height = ply_image_get_height (plugin->lock_image);

      entry_width = ply_entry_get_width (view->entry);
      entry_height = ply_entry_get_height (view->entry);

      x = screen_width / 2.0 - (view->lock_area.width + entry_width) / 2.0 + view->lock_area.width;
      y = screen_height / 2.0 - entry_height / 2.0;

      view->lock_area.x = screen_width / 2.0 - (view->lock_area.width + entry_width) / 2.0;
      view->lock_area.y = screen_height / 2.0 - view->lock_area.height / 2.0;

      ply_entry_show (view->entry, plugin->loop, view->display, x, y);
    }

  if (prompt != NULL)
    {
      ply_label_set_text (view->label, prompt);

      /* We center the prompt in the middle and use 80% of the horizontal space */
      int label_width = screen_width * 100 / 80;
      ply_label_set_alignment (view->label, PLY_LABEL_ALIGN_CENTER);
      ply_label_set_width (view->label, label_width);

      x = (screen_width - label_width) / 2;
      y = view->box_area.y + view->box_area.height;

      ply_label_show (view->label, view->display, x, y);
    }
}

static void
view_hide_prompt (view_t *view)
{
  assert (view != NULL);

  ply_entry_hide (view->entry);
  ply_label_hide (view->label);
}

static ply_boot_splash_plugin_t *
create_plugin (ply_key_file_t *key_file)
{
  ply_boot_splash_plugin_t *plugin;
  char *image_dir, *image_path;
  char *alignment;
  char *transition;
  char *transition_duration;
  char *color;
  char *progress_function;

  srand ((int) ply_get_timestamp ());
  plugin = calloc (1, sizeof (ply_boot_splash_plugin_t));

  image_dir = ply_key_file_get_value (key_file, "two-step", "ImageDir");

  ply_trace ("Using '%s' as working directory", image_dir);

  asprintf (&image_path, "%s/lock.png", image_dir);
  plugin->lock_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/box.png", image_dir);
  plugin->box_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/corner-image.png", image_dir);
  plugin->corner_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/header-image.png", image_dir);
  plugin->header_image = ply_image_new (image_path);
  free (image_path);

  plugin->animation_dir = image_dir;

  alignment = ply_key_file_get_value (key_file, "two-step", "HorizontalAlignment");
  if (alignment != NULL)
    plugin->animation_horizontal_alignment = strtod (alignment, NULL);
  else
    plugin->animation_horizontal_alignment = .5;
  free (alignment);

  alignment = ply_key_file_get_value (key_file, "two-step", "VerticalAlignment");
  if (alignment != NULL)
    plugin->animation_vertical_alignment = strtod (alignment, NULL);
  else
    plugin->animation_vertical_alignment = .5;
  free (alignment);

  plugin->transition = PLY_PROGRESS_ANIMATION_TRANSITION_NONE;
  transition = ply_key_file_get_value (key_file, "two-step", "Transition");
  if (transition != NULL)
    {
      if (strcmp (transition, "fade-over") == 0)
        plugin->transition = PLY_PROGRESS_ANIMATION_TRANSITION_FADE_OVER;
      else if (strcmp (transition, "cross-fade") == 0)
        plugin->transition = PLY_PROGRESS_ANIMATION_TRANSITION_CROSS_FADE;
      else if (strcmp (transition, "merge-fade") == 0)
        plugin->transition = PLY_PROGRESS_ANIMATION_TRANSITION_MERGE_FADE;
    }
  free (transition);

  transition_duration = ply_key_file_get_value (key_file, "two-step", "TransitionDuration");
  if (transition_duration != NULL)
    plugin->transition_duration = strtod (transition_duration, NULL);
  else
    plugin->transition_duration = 0.0;
  free (transition_duration);

  color = ply_key_file_get_value (key_file, "two-step", "BackgroundStartColor");

  if (color != NULL)
    plugin->background_start_color = strtol (color, NULL, 0);
  else
    plugin->background_start_color = PLYMOUTH_BACKGROUND_START_COLOR;

  free (color);

  color = ply_key_file_get_value (key_file, "two-step", "BackgroundEndColor");

  if (color != NULL)
    plugin->background_end_color = strtol (color, NULL, 0);
  else
    plugin->background_end_color = PLYMOUTH_BACKGROUND_END_COLOR;

  free (color);

  progress_function = ply_key_file_get_value (key_file, "two-step", "ProgressFunction");

  if (progress_function != NULL)
    {
      if (strcmp (progress_function, "wwoods") == 0)
        {
          ply_trace ("Using wwoods progress function");
          plugin->progress_function = PROGRESS_FUNCTION_TYPE_WWOODS;
        }
      else if (strcmp (progress_function, "linear") == 0)
        {
          ply_trace ("Using linear progress function");
          plugin->progress_function = PROGRESS_FUNCTION_TYPE_LINEAR;
        }
      else
        {
          ply_trace ("unknown progress function %s, defaulting to linear", progress_function);
          plugin->progress_function = PROGRESS_FUNCTION_TYPE_LINEAR;
        }
    }

  plugin->views = ply_list_new ();

  return plugin;
}

static void
free_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  ply_trace ("freeing views");

  node = ply_list_get_first_node (plugin->views);

  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_free (view);
      ply_list_remove_node (plugin->views, node);

      node = next_node;
    }

  ply_list_free (plugin->views);
  plugin->views = NULL;
}

static void
destroy_plugin (ply_boot_splash_plugin_t *plugin)
{
  if (plugin == NULL)
    return;

  ply_trace ("destroying plugin");

  if (plugin->loop != NULL)
    {
      stop_animation (plugin, NULL);

      ply_event_loop_stop_watching_for_exit (plugin->loop, (ply_event_loop_exit_handler_t)
                                             detach_from_event_loop,
                                             plugin);
      detach_from_event_loop (plugin);
    }

  ply_image_free (plugin->box_image);
  ply_image_free (plugin->lock_image);

  if (plugin->corner_image != NULL)
    ply_image_free (plugin->corner_image);

  if (plugin->header_image != NULL)
    ply_image_free (plugin->header_image);

  free (plugin->animation_dir);
  free_views (plugin);
  free (plugin);
}

static void
start_end_animation (ply_boot_splash_plugin_t *plugin,
                     ply_trigger_t            *trigger)
{
  ply_trace ("starting end animation");

  ply_list_node_t *node;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;
      ply_trigger_t *throbber_trigger;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      ply_trigger_ignore_next_pull (trigger);

      if (view->throbber != NULL)
        {
          ply_trace ("stopping throbber");
          view->end_trigger = trigger;
          throbber_trigger = ply_trigger_new (NULL);
          ply_trigger_add_handler (throbber_trigger,
                                   (ply_trigger_handler_t)
                                   on_view_throbber_stopped,
                                   view);
          ply_throbber_stop (view->throbber, throbber_trigger);
        }
      else
        {
          if (view->progress_animation != NULL)
            {
              ply_trace ("hiding progress animation");
              ply_progress_animation_hide (view->progress_animation);
            }
          view_start_end_animation (view, trigger);
        }

      node = next_node;
    }
  ply_trigger_pull (trigger, NULL);
}

static void
start_progress_animation (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  if (plugin->is_animating)
     return;

  ply_trace ("starting animation");

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_start_progress_animation (view);

      node = next_node;
    }

  plugin->is_animating = true;

  /* We don't really know how long shutdown will, take
   * but it's normally really fast, so just jump to
   * the end animation
   */
  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    become_idle (plugin, NULL);
}

static void
stop_animation (ply_boot_splash_plugin_t *plugin,
                ply_trigger_t            *trigger)
{
  ply_list_node_t *node;

  assert (plugin != NULL);
  assert (plugin->loop != NULL);

  if (!plugin->is_animating)
     return;

  ply_trace ("stopping animation%s",
             trigger != NULL? " with trigger" : "");

  plugin->is_animating = false;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      if (view->progress_animation != NULL)
        {
          ply_trace ("hiding progress animation");
          ply_progress_animation_hide (view->progress_animation);
        }
      if (trigger != NULL)
        ply_trigger_ignore_next_pull (trigger);
      if (view->throbber != NULL)
        ply_throbber_stop (view->throbber, trigger);
      ply_animation_stop (view->end_animation);

      node = next_node;
    }

  if (trigger != NULL)
    ply_trigger_pull (trigger, NULL);
}

static void
detach_from_event_loop (ply_boot_splash_plugin_t *plugin)
{
  plugin->loop = NULL;
}

static void
draw_background (view_t             *view,
                 ply_pixel_buffer_t *pixel_buffer,
                 int                 x,
                 int                 y,
                 int                 width,
                 int                 height)
{
  ply_boot_splash_plugin_t *plugin;
  ply_rectangle_t area;

  plugin = view->plugin;

  area.x = x;
  area.y = y;
  area.width = width;
  area.height = height;

  if (plugin->background_start_color != plugin->background_end_color)
    ply_pixel_buffer_fill_with_gradient (pixel_buffer, &area,
                                         plugin->background_start_color,
                                         plugin->background_end_color);
  else
    ply_pixel_buffer_fill_with_hex_color (pixel_buffer, &area,
                                          plugin->background_start_color);
}

static void
on_draw (view_t                   *view,
         ply_pixel_buffer_t       *pixel_buffer,
         int                       x,
         int                       y,
         int                       width,
         int                       height)
{
  ply_boot_splash_plugin_t *plugin;
  ply_rectangle_t screen_area;
  ply_rectangle_t image_area;

  plugin = view->plugin;

  draw_background (view, pixel_buffer, x, y, width, height);

  ply_pixel_buffer_get_size (pixel_buffer, &screen_area);

  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY ||
      plugin->state == PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY  )
    {
      uint32_t *box_data, *lock_data;

      box_data = ply_image_get_data (plugin->box_image);
      ply_pixel_buffer_fill_with_argb32_data (pixel_buffer,
                                              &view->box_area,
                                              box_data);

      ply_entry_draw_area (view->entry,
                           pixel_buffer,
                           x, y, width, height);
      ply_label_draw_area (view->label,
                           pixel_buffer,
                           x, y, width, height);

      lock_data = ply_image_get_data (plugin->lock_image);
      ply_pixel_buffer_fill_with_argb32_data (pixel_buffer,
                                              &view->lock_area,
                                              lock_data);
    }
  else
    {
      if (view->throbber != NULL &&
          !ply_throbber_is_stopped (view->throbber))
        {
          ply_throbber_draw_area (view->throbber, pixel_buffer,
                                  x, y, width, height);
        }
      if (view->progress_animation != NULL && !ply_progress_animation_is_hidden (view->progress_animation))
        {
          ply_progress_animation_draw_area (view->progress_animation,
                                            pixel_buffer,
                                            x, y, width, height);
        }
      else if (!ply_animation_is_stopped (view->end_animation))
        ply_animation_draw_area (view->end_animation,
                                 pixel_buffer,
                                 x, y, width, height);

      if (plugin->corner_image != NULL)
        {
          image_area.width = ply_image_get_width (plugin->corner_image);
          image_area.height = ply_image_get_height (plugin->corner_image);
          image_area.x = screen_area.width - image_area.width - 20;
          image_area.y = screen_area.height - image_area.height - 20;

          ply_pixel_buffer_fill_with_argb32_data (pixel_buffer, &image_area, ply_image_get_data (plugin->corner_image));

        }

      if (plugin->header_image != NULL)
        {
          long sprite_height;


          if (view->progress_animation != NULL)
            sprite_height = ply_progress_animation_get_height (view->progress_animation);
          else
            sprite_height = 0;

          if (view->throbber != NULL)
            sprite_height = MAX (ply_throbber_get_height (view->throbber),
                                 sprite_height);

          image_area.width = ply_image_get_width (plugin->header_image);
          image_area.height = ply_image_get_height (plugin->header_image);
          image_area.x = screen_area.width / 2.0 - image_area.width / 2.0;
          image_area.y = plugin->animation_vertical_alignment * screen_area.height - sprite_height / 2.0 - image_area.height;

          ply_pixel_buffer_fill_with_argb32_data (pixel_buffer, &image_area, ply_image_get_data (plugin->header_image));
        }
    }
  ply_label_draw_area (view->message_label,
                       pixel_buffer,
                       x, y, width, height);
}

static void
add_pixel_display (ply_boot_splash_plugin_t *plugin,
                   ply_pixel_display_t      *display)
{
  view_t *view;

  ply_trace ("adding pixel display to plugin");
  view = view_new (plugin, display);

  ply_pixel_display_set_draw_handler (view->display,
                                      (ply_pixel_display_draw_handler_t)
                                      on_draw, view);
  ply_list_append_data (plugin->views, view);
}

static void
remove_pixel_display (ply_boot_splash_plugin_t *plugin,
                      ply_pixel_display_t      *display)
{
  ply_list_node_t *node;

  ply_trace ("removing pixel display from plugin");
  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      view_t *view;
      ply_list_node_t *next_node;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      if (view->display == display)
        {

          ply_pixel_display_set_draw_handler (view->display, NULL, NULL);
          view_free (view);
          ply_list_remove_node (plugin->views, node);
          return;
        }

      node = next_node;
    }
}

static bool
show_splash_screen (ply_boot_splash_plugin_t *plugin,
                    ply_event_loop_t         *loop,
                    ply_buffer_t             *boot_buffer,
                    ply_boot_splash_mode_t    mode)
{
  assert (plugin != NULL);

  plugin->loop = loop;
  plugin->mode = mode;

  ply_trace ("loading lock image");
  if (!ply_image_load (plugin->lock_image))
    return false;

  ply_trace ("loading box image");
  if (!ply_image_load (plugin->box_image))
    return false;

  if (plugin->corner_image != NULL)
    {
      ply_trace ("loading corner image");

      if (!ply_image_load (plugin->corner_image))
        {
          ply_image_free (plugin->corner_image);
          plugin->corner_image = NULL;
        }
    }

  if (plugin->header_image != NULL)
    {
      ply_trace ("loading header image");

      if (!ply_image_load (plugin->header_image))
        {
          ply_image_free (plugin->header_image);
          plugin->header_image = NULL;
        }
    }

  if (!load_views (plugin))
    {
      ply_trace ("couldn't load views");
      return false;
    }

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                 detach_from_event_loop,
                                 plugin);

  ply_trace ("starting boot animations");
  start_progress_animation (plugin);

  plugin->is_visible = true;

  return true;
}

static void
update_status (ply_boot_splash_plugin_t *plugin,
               const char               *status)
{
  assert (plugin != NULL);
}

static void
on_animation_stopped (ply_boot_splash_plugin_t *plugin)
{
  if (plugin->idle_trigger != NULL)
    {
      ply_trigger_pull (plugin->idle_trigger, NULL);
      plugin->idle_trigger = NULL;
    }
  plugin->is_idle = true;
}

static void
update_progress_animation (ply_boot_splash_plugin_t *plugin,
                           double                    percent_done)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      if (view->progress_animation != NULL)
        ply_progress_animation_set_percent_done (view->progress_animation,
                                                 percent_done);

      node = next_node;
    }
}

static void
on_boot_progress (ply_boot_splash_plugin_t *plugin,
                  double                    duration,
                  double                    percent_done)
{
  if (plugin->mode == PLY_BOOT_SPLASH_MODE_UPDATES)
    return;

  if (plugin->state != PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    return;

  if (percent_done >= SHOW_ANIMATION_PERCENT)
    {
      if (plugin->stop_trigger == NULL)
        {
          ply_trace ("boot progressed to end");

          plugin->stop_trigger = ply_trigger_new (&plugin->stop_trigger);
          ply_trigger_add_handler (plugin->stop_trigger,
                                   (ply_trigger_handler_t)
                                   on_animation_stopped,
                                   plugin);
          start_end_animation (plugin, plugin->stop_trigger);
        }
    }
  else
    {
      double total_duration;

      percent_done *= (1 / SHOW_ANIMATION_PERCENT);

      switch (plugin->progress_function)
        {
          /* Fun made-up smoothing function to make the growth asymptotic:
           * fraction(time,estimate)=1-2^(-(time^1.45)/estimate) */
          case PROGRESS_FUNCTION_TYPE_WWOODS:
            total_duration = duration / percent_done;
            percent_done = 1.0 - pow (2.0, -pow (duration, 1.45) / total_duration) * (1.0 - percent_done);
            break;

          case PROGRESS_FUNCTION_TYPE_LINEAR:
            break;
        }

      update_progress_animation (plugin, percent_done);
    }
}

static void
hide_splash_screen (ply_boot_splash_plugin_t *plugin,
                    ply_event_loop_t         *loop)
{
  assert (plugin != NULL);

  ply_trace ("hiding splash");
  if (plugin->loop != NULL)
    {
      stop_animation (plugin, NULL);

      ply_event_loop_stop_watching_for_exit (plugin->loop, (ply_event_loop_exit_handler_t)
                                             detach_from_event_loop,
                                             plugin);
      detach_from_event_loop (plugin);
    }

  plugin->is_visible = false;
}

static void
show_password_prompt (ply_boot_splash_plugin_t *plugin,
                      const char               *text,
                      int                       number_of_bullets)
{
  ply_list_node_t *node;

  ply_trace ("showing password prompt");
  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_show_prompt (view, text);
      ply_entry_set_bullet_count (view->entry, number_of_bullets);

      node = next_node;
    }
}

static void
show_prompt (ply_boot_splash_plugin_t *plugin,
             const char               *prompt,
             const char               *entry_text)
{
  ply_list_node_t *node;

  ply_trace ("showing prompt");
  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_show_prompt (view, prompt);
      ply_entry_set_text (view->entry, entry_text);

      node = next_node;
    }
}

static void
on_root_mounted (ply_boot_splash_plugin_t *plugin)
{
  ply_trace ("root filesystem mounted");
  plugin->root_is_mounted = true;
}

static void
become_idle (ply_boot_splash_plugin_t *plugin,
             ply_trigger_t            *idle_trigger)
{
  ply_trace ("deactivation requested");
  if (plugin->is_idle)
    {
      ply_trace ("plugin is already idle");
      ply_trigger_pull (idle_trigger, NULL);
      return;
    }

  plugin->idle_trigger = idle_trigger;

  if (plugin->stop_trigger == NULL)
    {
      ply_trace ("waiting for plugin to stop");
      plugin->stop_trigger = ply_trigger_new (&plugin->stop_trigger);
      ply_trigger_add_handler (plugin->stop_trigger,
                               (ply_trigger_handler_t)
                               on_animation_stopped,
                               plugin);
      start_end_animation (plugin, plugin->stop_trigger);
    }
  else
    {
      ply_trace ("already waiting for plugin to stop");
    }
}

static void
hide_prompt (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  ply_trace ("hiding prompt");
  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_hide_prompt (view);

      node = next_node;
    }
}


static void
show_message (ply_boot_splash_plugin_t *plugin,
              const char               *message)
{
  ply_trace ("Showing message '%s'", message);
  ply_list_node_t *node;
  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);
      
      ply_label_set_text (view->message_label, message);
      ply_label_show (view->message_label, view->display, 10, 10);
      
      ply_pixel_display_draw_area (view->display, 10, 10,
                                   ply_label_get_width (view->message_label),
                                   ply_label_get_height(view->message_label));
      node = next_node;
    }
}

static void
system_update (ply_boot_splash_plugin_t *plugin,
               int                       progress)
{
  ply_list_node_t *node;

  if (plugin->mode != PLY_BOOT_SPLASH_MODE_UPDATES)
    return;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);
      ply_progress_animation_set_percent_done (view->progress_animation,
                                               (double) progress / 100.f);
      node = next_node;
    }
}

static void
display_normal (ply_boot_splash_plugin_t *plugin)
{
  pause_views (plugin);
  if (plugin->state != PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    hide_prompt (plugin);

  plugin->state = PLY_BOOT_SPLASH_DISPLAY_NORMAL;
  start_progress_animation (plugin);
  redraw_views (plugin);
  unpause_views (plugin);
}

static void
display_password (ply_boot_splash_plugin_t *plugin,
                  const char               *prompt,
                  int                       bullets)
{
  pause_views (plugin);
  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    stop_animation (plugin, NULL);

  plugin->state = PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY;
  show_password_prompt (plugin, prompt, bullets);
  redraw_views (plugin);
  unpause_views (plugin);
}

static void
display_question (ply_boot_splash_plugin_t *plugin,
                  const char               *prompt,
                  const char               *entry_text)
{
  pause_views (plugin);
  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    stop_animation (plugin, NULL);

  plugin->state = PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY;
  show_prompt (plugin, prompt, entry_text);
  redraw_views (plugin);
  unpause_views (plugin);
}

static void
display_message (ply_boot_splash_plugin_t *plugin,
                 const char               *message)
{
  show_message (plugin, message);
}

ply_boot_splash_plugin_interface_t *
ply_boot_splash_plugin_get_interface (void)
{
  static ply_boot_splash_plugin_interface_t plugin_interface =
    {
      .create_plugin = create_plugin,
      .destroy_plugin = destroy_plugin,
      .add_pixel_display = add_pixel_display,
      .remove_pixel_display = remove_pixel_display,
      .show_splash_screen = show_splash_screen,
      .update_status = update_status,
      .on_boot_progress = on_boot_progress,
      .hide_splash_screen = hide_splash_screen,
      .on_root_mounted = on_root_mounted,
      .become_idle = become_idle,
      .display_normal = display_normal,
      .display_password = display_password,
      .display_question = display_question,
      .display_message = display_message,
      .system_update = system_update,
    };

  return &plugin_interface;
}

/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
