/* fade-throbber.c - boot splash plugin
 *
 * Copyright (C) 2007, 2008, 2009 Red Hat, Inc.
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
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-trigger.h"
#include "ply-utils.h"

#include <linux/kd.h>

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 30
#endif


typedef enum {
   PLY_BOOT_SPLASH_DISPLAY_NORMAL,
   PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY,
   PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY
} ply_boot_splash_display_type_t;

typedef struct
{
  unsigned int x;
  unsigned int y;
  double start_time;
  double speed;
  double opacity;
} star_t;

typedef struct
{
  ply_boot_splash_plugin_t *plugin;
  ply_pixel_display_t *display;
  ply_list_t *stars;
  ply_entry_t *entry;
  ply_label_t *label;
  ply_label_t *message_label;
  ply_rectangle_t lock_area;
  double logo_opacity;
} view_t;

struct _ply_boot_splash_plugin
{
  ply_event_loop_t *loop;
  ply_boot_splash_mode_t mode;
  ply_image_t *logo_image;
  ply_image_t *star_image;
  ply_image_t *lock_image;
  char *image_dir;
  ply_list_t *views;

  ply_boot_splash_display_type_t state;

  double start_time;
  double now;

  uint32_t is_animating : 1;
};

ply_boot_splash_plugin_interface_t *ply_boot_splash_plugin_get_interface (void);

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

      x = view->lock_area.x;
      y = view->lock_area.y + view->lock_area.height;

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

  srand ((int) ply_get_timestamp ());
  plugin = calloc (1, sizeof (ply_boot_splash_plugin_t));
  plugin->start_time = 0.0;

  plugin->logo_image = ply_image_new (PLYMOUTH_LOGO_FILE);
  image_dir = ply_key_file_get_value (key_file, "fade-throbber", "ImageDir");

  asprintf (&image_path, "%s/star.png", image_dir);
  plugin->star_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/lock.png", image_dir);
  plugin->lock_image = ply_image_new (image_path);
  free (image_path);

  plugin->image_dir = image_dir;

  plugin->state = PLY_BOOT_SPLASH_DISPLAY_NORMAL;
  plugin->views = ply_list_new ();

  return plugin;
}

static star_t *
star_new (int    x,
          int    y,
          double speed)
{
  star_t *star;

  star = calloc (1, sizeof (star_t));
  star->x = x;
  star->y = y;
  star->speed = speed;
  star->start_time = ply_get_timestamp ();

  return star;
}

static void
star_free (star_t *star)
{
  free (star);
}

static void
free_stars (view_t *view)
{
  ply_list_node_t *node;

  assert (view != NULL);

  node = ply_list_get_first_node (view->stars);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      star_t *star;

      star = (star_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (view->stars, node);

      star_free (star);
      node = next_node;
    }

  ply_list_free (view->stars);
  view->stars = NULL;
}

static void detach_from_event_loop (ply_boot_splash_plugin_t *plugin);

static view_t *
view_new (ply_boot_splash_plugin_t *plugin,
          ply_pixel_display_t      *display)
{
  view_t *view;

  view = calloc (1, sizeof (view_t));
  view->plugin = plugin;
  view->display = display;

  view->entry = ply_entry_new (plugin->image_dir);
  view->stars = ply_list_new ();
  view->label = ply_label_new ();

  view->message_label = ply_label_new ();

  return view;
}

static void
view_free (view_t *view)
{

  ply_entry_free (view->entry);
  ply_label_free (view->message_label);
  free_stars (view);

  free (view);
}

static bool
view_load (view_t *view)
{
  ply_trace ("loading entry");
  if (!ply_entry_load (view->entry))
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

  free_views (plugin);
  ply_image_free (plugin->logo_image);
  ply_image_free (plugin->star_image);
  ply_image_free (plugin->lock_image);
  free (plugin);
}

static void
view_animate_at_time (view_t  *view,
                      double   time)
{
  ply_boot_splash_plugin_t *plugin;
  ply_list_node_t *node;
  double logo_opacity;
  long logo_x, logo_y;
  long logo_width, logo_height;
  unsigned long screen_width, screen_height;
  unsigned long star_width, star_height;

  plugin = view->plugin;

  logo_width = ply_image_get_width (plugin->logo_image);
  logo_height = ply_image_get_height (plugin->logo_image);

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  logo_x = (screen_width / 2) - (logo_width / 2);
  logo_y = (screen_height / 2) - (logo_height / 2);

  star_width = ply_image_get_width (plugin->star_image);
  star_height = ply_image_get_height (plugin->star_image);

  node = ply_list_get_first_node (view->stars);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      star_t *star;

      star = (star_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (view->stars, node);

      star->opacity = .5 * sin (((plugin->now - star->start_time) / star->speed) * (2 * M_PI)) + .5;
      star->opacity = CLAMP (star->opacity, 0, 1.0);

      ply_pixel_display_draw_area (view->display,
                                   star->x, star->y,
                                   star_width, star_height);
      node = next_node;
    }

  logo_opacity = .5 * sin ((time / 5) * (2 * M_PI)) + .8;
  logo_opacity = CLAMP (logo_opacity, 0, 1.0);

  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    logo_opacity = 1.0;

  if (fabs (logo_opacity - view->logo_opacity) <= DBL_MIN)
    return;

  view->logo_opacity = logo_opacity;

  ply_pixel_display_draw_area (view->display,
                               logo_x, logo_y,
                               logo_width, logo_height);
}

static void
animate_at_time (ply_boot_splash_plugin_t *plugin,
                 double                    time)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_animate_at_time (view, time);

      node = next_node;
    }
}

static void
on_timeout (ply_boot_splash_plugin_t *plugin)
{
  double sleep_time;

  plugin->now = ply_get_timestamp ();

  /* The choice below is between
   *
   * 1) keeping a constant animation speed, and dropping
   * frames when necessary
   * 2) showing every frame, but slowing down the animation
   * when a frame would be otherwise dropped.
   *
   * It turns out there are parts of boot up where the animation
   * can get sort of choppy.  By default we choose 2, since the
   * nature of this animation means it looks natural even when it
   * is slowed down
   */
#ifdef REAL_TIME_ANIMATION
  animate_at_time (plugin,
                   plugin->now - plugin->start_time);
#else
  static double time = 0.0;
  time += 1.0 / FRAMES_PER_SECOND;
  animate_at_time (plugin, time);
#endif

  sleep_time = 1.0 / FRAMES_PER_SECOND;
  sleep_time = MAX (sleep_time - (ply_get_timestamp () - plugin->now),
                    0.005);

  ply_event_loop_watch_for_timeout (plugin->loop, 
                                    sleep_time,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, plugin);
}

static void
view_start_animation (view_t *view)
{
  unsigned long screen_width, screen_height;

  assert (view != NULL);

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  ply_pixel_display_draw_area (view->display, 0, 0,
                               screen_width, screen_height);

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
  
  plugin->start_time = ply_get_timestamp ();
  animate_at_time (plugin, plugin->start_time);
  
  if (plugin->mode == PLY_BOOT_SPLASH_MODE_SHUTDOWN)
    return;

  ply_event_loop_watch_for_timeout (plugin->loop, 
                                    1.0 / FRAMES_PER_SECOND,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, plugin);
}

static void
stop_animation (ply_boot_splash_plugin_t *plugin)
{
  assert (plugin != NULL);
  assert (plugin->loop != NULL);

  if (!plugin->is_animating)
     return;

  plugin->is_animating = false;

  if (plugin->loop != NULL)
    {
      ply_event_loop_stop_watching_for_timeout (plugin->loop,
                                                (ply_event_loop_timeout_handler_t)
                                                on_timeout, plugin);
    }
  redraw_views (plugin);
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
  ply_rectangle_t area;

  area.x = x;
  area.y = y;
  area.width = width;
  area.height = height;

  ply_pixel_buffer_fill_with_gradient (pixel_buffer, &area,
                                       PLYMOUTH_BACKGROUND_START_COLOR,
                                       PLYMOUTH_BACKGROUND_END_COLOR);
}

static void
draw_normal_view (view_t             *view,
                  ply_pixel_buffer_t *pixel_buffer,
                  int                 x,
                  int                 y,
                  int                 width,
                  int                 height)
{
  ply_boot_splash_plugin_t *plugin;
  ply_list_node_t *node;
  ply_rectangle_t logo_area;
  ply_rectangle_t star_area;
  uint32_t *logo_data, *star_data;
  unsigned long screen_width, screen_height;

  plugin = view->plugin;

  if (!plugin->is_animating)
    return;

  logo_area.width = ply_image_get_width (plugin->logo_image);
  logo_area.height = ply_image_get_height (plugin->logo_image);
  logo_data = ply_image_get_data (plugin->logo_image);

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  logo_area.x = (screen_width / 2) - (logo_area.width / 2);
  logo_area.y = (screen_height / 2) - (logo_area.height / 2);

  star_data = ply_image_get_data (plugin->star_image);
  star_area.width = ply_image_get_width (plugin->star_image);
  star_area.height = ply_image_get_height (plugin->star_image);

  node = ply_list_get_first_node (view->stars);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      star_t *star;

      star = (star_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (view->stars, node);

      star_area.x = star->x;
      star_area.y = star->y;
      ply_pixel_buffer_fill_with_argb32_data_at_opacity (pixel_buffer,
                                                         &star_area,
                                                         star_data,
                                                         star->opacity);
      node = next_node;
    }

  ply_pixel_buffer_fill_with_argb32_data_at_opacity (pixel_buffer,
                                                     &logo_area,
                                                     logo_data,
                                                     view->logo_opacity);
}

static void
draw_prompt_view (view_t             *view,
                  ply_pixel_buffer_t *pixel_buffer,
                  int                 x,
                  int                 y,
                  int                 width,
                  int                 height)
{
  ply_boot_splash_plugin_t *plugin;
  uint32_t *lock_data;

  plugin = view->plugin;

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

  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_NORMAL)
    draw_normal_view (view, pixel_buffer, x, y, width, height);
  else
    draw_prompt_view (view, pixel_buffer, x, y, width, height);

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

  ply_trace ("loading star image");
  if (!ply_image_load (plugin->star_image))
    return false;

  ply_trace ("loading lock image");
  if (!ply_image_load (plugin->lock_image))
    return false;

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                 detach_from_event_loop,
                                 plugin);

  if (!load_views (plugin))
    {
      ply_trace ("couldn't load views");
      return false;
    }

  ply_trace ("starting boot animation");
  start_animation (plugin);

  return true;
}

static void
view_add_star (view_t *view)
{
  ply_boot_splash_plugin_t *plugin;
  ply_rectangle_t logo_area;
  star_t *star;
  unsigned int x, y;
  unsigned int width, height;
  unsigned long screen_width, screen_height;
  ply_list_node_t *node;

  assert (view != NULL);

  plugin = view->plugin;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);
  width = ply_image_get_width (plugin->logo_image);
  height = ply_image_get_height (plugin->logo_image);
  logo_area.x = (screen_width / 2) - (width / 2);
  logo_area.y = (screen_height / 2) - (height / 2);
  logo_area.width = width;
  logo_area.height = height;

  width = ply_image_get_width (plugin->star_image);
  height = ply_image_get_height (plugin->star_image);

  node = NULL;
  do
    {
      x = rand () % screen_width;
      y = rand () % screen_height;

      if ((x <= logo_area.x + logo_area.width)
           && (x >= logo_area.x)
          && (y >= logo_area.y)
           && (y <= logo_area.y + logo_area.height))
          continue;

      if ((x + width >= logo_area.x)
           && (x + width <= logo_area.x + logo_area.width)
          && (y + height >= logo_area.y)
           && (y + height <= logo_area.y + logo_area.height))
          continue;

      node = ply_list_get_first_node (view->stars);
      while (node != NULL)
        {
          ply_list_node_t *next_node;

          star = (star_t *) ply_list_node_get_data (node);
          next_node = ply_list_get_next_node (view->stars, node);

          if ((x <= star->x + width)
               && (x >= star->x)
              && (y >= star->y)
               && (y <= star->y + height))
              break;

          if ((x + width >= star->x)
               && (x + width <= star->x + width)
              && (y + height >= star->y)
               && (y + height <= star->y + height))
              break;

          node = next_node;
        }

    } while (node != NULL);

  star = star_new (x, y, (double) ((rand () % 50) + 1));
  ply_list_append_data (view->stars, star);
}

static void
add_stars (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (plugin->views);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_add_star (view);

      node = next_node;
    }
}

static void
update_status (ply_boot_splash_plugin_t *plugin,
               const char               *status)
{
  assert (plugin != NULL);

  add_stars (plugin);
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
hide_splash_screen (ply_boot_splash_plugin_t *plugin,
                    ply_event_loop_t         *loop)
{
  assert (plugin != NULL);

  if (plugin->loop != NULL)
    {
      stop_animation (plugin);

      ply_event_loop_stop_watching_for_exit (plugin->loop, (ply_event_loop_exit_handler_t)
                                             detach_from_event_loop,
                                             plugin);
      detach_from_event_loop (plugin);
    }
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
    stop_animation (plugin);

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
    stop_animation (plugin);

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
      .hide_splash_screen = hide_splash_screen,
      .display_normal = display_normal,
      .display_password = display_password,
      .display_question = display_question,
      .display_message = display_message,
    };

  return &plugin_interface;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
