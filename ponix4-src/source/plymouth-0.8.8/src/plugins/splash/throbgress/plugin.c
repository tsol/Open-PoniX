/* throbgress.c - boot splash plugin
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
 *             Will Woods <wwoods@redhat.com>
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
#include "ply-key-file.h"
#include "ply-label.h"
#include "ply-list.h"
#include "ply-progress-bar.h"
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-trigger.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-utils.h"

#include "ply-throbber.h"

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif

#ifndef BAR_HEIGHT
#define BAR_HEIGHT 16
#endif

typedef enum {
   PLY_BOOT_SPLASH_DISPLAY_NORMAL,
   PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY,
   PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY
} ply_boot_splash_display_type_t;

typedef struct
{
  ply_boot_splash_plugin_t *plugin;
  ply_pixel_display_t *display;
  ply_entry_t *entry;
  ply_throbber_t *throbber;
  ply_progress_bar_t *progress_bar;
  ply_label_t *label;
  ply_label_t *message_label;
  ply_rectangle_t box_area, lock_area, logo_area, bar_area;
} view_t;

struct _ply_boot_splash_plugin
{
  ply_event_loop_t *loop;
  ply_boot_splash_mode_t mode;
  ply_image_t *logo_image;
  ply_image_t *lock_image;
  ply_image_t *box_image;
  ply_list_t *views;

  char *image_dir;
  ply_boot_splash_display_type_t state;

  uint32_t background_start_color;
  uint32_t background_end_color;

  ply_trigger_t *idle_trigger;

  uint32_t root_is_mounted : 1;
  uint32_t is_visible : 1;
  uint32_t is_animating : 1;
  uint32_t is_idle : 1;
};

ply_boot_splash_plugin_interface_t * ply_boot_splash_plugin_get_interface (void);
static void detach_from_event_loop (ply_boot_splash_plugin_t *plugin);

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

  view->entry = ply_entry_new (plugin->image_dir);
  view->throbber = ply_throbber_new (plugin->image_dir,
                                     "throbber-");
  view->progress_bar = ply_progress_bar_new ();
  view->label = ply_label_new ();
  view->message_label = ply_label_new ();

  return view;
}

static void
view_free (view_t *view)
{

  ply_entry_free (view->entry);
  ply_throbber_free (view->throbber);
  ply_progress_bar_free (view->progress_bar);
  ply_label_free (view->label);
  ply_label_free (view->message_label);

  free (view);
}

static void
free_views (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

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

static bool
view_load (view_t *view)
{
  ply_trace ("loading entry");
  if (!ply_entry_load (view->entry))
    return false;

  ply_trace ("loading throbber");
  if (!ply_throbber_load (view->throbber))
    return false;

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
view_start_animation (view_t *view)
{
  ply_boot_splash_plugin_t *plugin;

  unsigned long screen_width, screen_height;
  long width, height;

  assert (view != NULL);

  plugin = view->plugin;

  assert (plugin != NULL);
  assert (plugin->loop != NULL);

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  ply_pixel_display_draw_area (view->display, 0, 0,
                               screen_width, screen_height);

  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    return;

  plugin->is_idle = false;

  width = ply_throbber_get_width (view->throbber);
  height = ply_throbber_get_height (view->throbber);
  ply_throbber_start (view->throbber,
                      plugin->loop,
                      view->display,
                      screen_width / 2.0 - width / 2.0,
                      view->logo_area.y + view->logo_area.height + height / 2);
  ply_progress_bar_show (view->progress_bar,
                         view->display,
                         0, screen_height - ply_progress_bar_get_height (view->progress_bar));
  view_redraw (view);
}

static void
view_show_prompt (view_t     *view,
                  const char *prompt)
{
  ply_boot_splash_plugin_t *plugin;
  int x, y;
  int entry_width, entry_height;

  assert (view != NULL);

  plugin = view->plugin;

  if (ply_entry_is_hidden (view->entry))
    {
      unsigned long screen_width, screen_height;

      screen_width = ply_pixel_display_get_width (view->display);
      screen_height = ply_pixel_display_get_height (view->display);

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

      x = view->box_area.x + view->lock_area.width / 2;
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
  char *color;

  srand ((int) ply_get_timestamp ());
  plugin = calloc (1, sizeof (ply_boot_splash_plugin_t));

  plugin->logo_image = ply_image_new (PLYMOUTH_LOGO_FILE);
  image_dir = ply_key_file_get_value (key_file, "throbgress", "ImageDir");

  asprintf (&image_path, "%s/lock.png", image_dir);
  plugin->lock_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/box.png", image_dir);
  plugin->box_image = ply_image_new (image_path);
  free (image_path);

  plugin->image_dir = image_dir;
  plugin->views = ply_list_new ();

  color = ply_key_file_get_value (key_file, "throbgress", "BackgroundStartColor");

  if (color != NULL)
    plugin->background_start_color = strtol (color, NULL, 0);
  else
    plugin->background_start_color = PLYMOUTH_BACKGROUND_START_COLOR;

  free (color);

  color = ply_key_file_get_value (key_file, "throbgress", "BackgroundEndColor");

  if (color != NULL)
    plugin->background_end_color = strtol (color, NULL, 0);
  else
    plugin->background_end_color = PLYMOUTH_BACKGROUND_END_COLOR;

  free (color);

  return plugin;
}

static void
destroy_plugin (ply_boot_splash_plugin_t *plugin)
{
  if (plugin == NULL)
    return;

  if (plugin->loop != NULL)
    {
      ply_event_loop_stop_watching_for_exit (plugin->loop, (ply_event_loop_exit_handler_t)
                                             detach_from_event_loop,
                                             plugin);
      detach_from_event_loop (plugin);
    }

  ply_image_free (plugin->logo_image);
  ply_image_free (plugin->box_image);
  ply_image_free (plugin->lock_image);

  free_views (plugin);

  free (plugin);
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
draw_logo (view_t                   *view,
           ply_pixel_buffer_t       *pixel_buffer)
{
  ply_boot_splash_plugin_t *plugin;
  uint32_t *logo_data;
  unsigned long screen_width, screen_height;
  long width, height;

  plugin = view->plugin;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  width = ply_image_get_width (plugin->logo_image);
  height = ply_image_get_height (plugin->logo_image);
  logo_data = ply_image_get_data (plugin->logo_image);
  view->logo_area.x = (screen_width / 2) - (width / 2);
  view->logo_area.y = (screen_height / 2) - (height / 2);
  view->logo_area.width = width;
  view->logo_area.height = height;

  ply_pixel_buffer_fill_with_argb32_data (pixel_buffer,
                                          &view->logo_area,
                                          logo_data);
}

static void
start_animation (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  if (plugin->is_animating)
     return;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_start_animation (view);

      node = next_node;
    }

  plugin->is_animating = true;

  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    plugin->is_idle = true;
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

  plugin->is_animating = false;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      ply_progress_bar_hide (view->progress_bar);
      if (trigger != NULL)
        ply_trigger_ignore_next_pull (trigger);
      ply_throbber_stop (view->throbber, trigger);

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
on_draw (view_t                   *view,
         ply_pixel_buffer_t       *pixel_buffer,
         int                       x,
         int                       y,
         int                       width,
         int                       height)
{
  ply_boot_splash_plugin_t *plugin;
  plugin = view->plugin;

  draw_background (view, pixel_buffer, x, y, width, height);

  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY ||
      plugin->state == PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY  )
    {
      uint32_t *box_data, *lock_data;

      box_data = ply_image_get_data (plugin->box_image);
      ply_pixel_buffer_fill_with_argb32_data (pixel_buffer,
                                              &view->box_area,
                                              box_data);
      ply_entry_draw_area (view->entry, pixel_buffer, x, y, width, height);
      ply_label_draw_area (view->label, pixel_buffer, x, y, width, height);
      lock_data = ply_image_get_data (plugin->lock_image);
      ply_pixel_buffer_fill_with_argb32_data (pixel_buffer,
                                              &view->lock_area,
                                              lock_data);
    }
  else
    {
      draw_logo (view, pixel_buffer);
      ply_throbber_draw_area (view->throbber,
                              pixel_buffer, x, y, width, height);
      ply_progress_bar_draw_area (view->progress_bar,
                                  pixel_buffer, x, y, width, height);
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
  assert (plugin->logo_image != NULL);

  plugin->loop = loop;
  plugin->mode = mode;

  ply_trace ("loading logo image");
  if (!ply_image_load (plugin->logo_image))
    return false;

  ply_trace ("loading lock image");
  if (!ply_image_load (plugin->lock_image))
    return false;

  ply_trace ("loading box image");
  if (!ply_image_load (plugin->box_image))
    return false;

  if (!load_views (plugin))
    {
      ply_trace ("couldn't load views");
      return false;
    }

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                 detach_from_event_loop,
                                 plugin);

  ply_trace ("starting boot animation");
  start_animation (plugin);

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
on_boot_progress (ply_boot_splash_plugin_t *plugin,
                  double                    duration,
                  double                    percent_done)
{
  ply_list_node_t *node;
  double total_duration;

  if (plugin->mode == PLY_BOOT_SPLASH_MODE_UPDATES)
    return;

  total_duration = duration / percent_done;

  /* Fun made-up smoothing function to make the growth asymptotic:
   * fraction(time,estimate)=1-2^(-(time^1.45)/estimate) */
  percent_done = 1.0 - pow (2.0, -pow (duration, 1.45) / total_duration) * (1.0 - percent_done);

  node = ply_list_get_first_node (plugin->views);

  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      ply_progress_bar_set_percent_done (view->progress_bar, percent_done);
      ply_progress_bar_draw (view->progress_bar);

      node = next_node;
    }
}

static void
hide_splash_screen (ply_boot_splash_plugin_t *plugin,
                    ply_event_loop_t         *loop)
{
  assert (plugin != NULL);

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
  plugin->root_is_mounted = true;
}

static void
become_idle (ply_boot_splash_plugin_t *plugin,
             ply_trigger_t            *idle_trigger)
{
  if (plugin->is_idle)
    {
      ply_trigger_pull (idle_trigger, NULL);
      return;
    }

  stop_animation (plugin, idle_trigger);
  plugin->is_idle = true;
}

static void
hide_prompt (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

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
display_normal (ply_boot_splash_plugin_t *plugin)
{
  pause_views (plugin);
  if (plugin->state != PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    hide_prompt (plugin);

  plugin->state = PLY_BOOT_SPLASH_DISPLAY_NORMAL;
  start_animation (plugin);
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
      ply_progress_bar_set_percent_done (view->progress_bar, (double) progress / 100.f);
      ply_progress_bar_draw (view->progress_bar);
      node = next_node;
    }
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

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
