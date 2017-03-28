/* plugin.c - boot splash plugin
 *
 * Copyright (C) 2007, 2008 Red Hat, Inc.
 *                     2008 Charlie Brej <cbrej@cs.man.ac.uk>
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
 * Written by: Charlie Brej <cbrej@cs.man.ac.uk>
 *             Ray Strode <rstrode@redhat.com>
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
#include "ply-logger.h"
#include "ply-image.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-trigger.h"
#include "ply-utils.h"

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 40
#endif

#define FLARE_FRAMES_PER_SECOND 20
#define BG_STARS_FRAMES_PER_SECOND 10
#define FLARE_COUNT 30
#define FLARE_LINE_COUNT 20
#define HALO_BLUR 4
#define STAR_HZ 0.08

/*you can comment one or both of these out*/
/*#define SHOW_PLANETS */
/*#define SHOW_COMETS */
#define SHOW_PROGRESS_BAR
/*#define SHOW_LOGO_HALO */

typedef enum {
  PLY_BOOT_SPLASH_DISPLAY_NORMAL,
  PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY,
  PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY
} ply_boot_splash_display_type_t;

typedef enum
{
  SPRITE_TYPE_STATIC,
  SPRITE_TYPE_FLARE,
  SPRITE_TYPE_SATELLITE,
  SPRITE_TYPE_PROGRESS,
  SPRITE_TYPE_STAR_BG,
} sprite_type_t;

typedef struct
{
  int x; 
  int y;
  int z;
  int oldx; 
  int oldy;
  int oldz;
  int refresh_me;
  float opacity;
  ply_image_t *image;
  sprite_type_t type;
  void* data;
} sprite_t;


typedef struct
{
  float stretch[FLARE_COUNT];
  float rotate_yz[FLARE_COUNT];
  float rotate_xy[FLARE_COUNT];
  float rotate_xz[FLARE_COUNT];
  float increase_speed[FLARE_COUNT];
  float z_offset_strength[FLARE_COUNT];
  float y_size[FLARE_COUNT];
  ply_image_t *image_a;
  ply_image_t *image_b;
  int frame_count;
} flare_t;

typedef enum
{
  SATELLITE_TYPE_PLANET,
  SATELLITE_TYPE_COMET,
} satellite_type_t;

typedef struct
{
  satellite_type_t type;
  int start_x;
  int start_y;
  int end_x;
  int end_y;
  int distance;
  double theta;
  ply_image_t *image;
  ply_image_t *image_altered;
} satellite_t;



typedef struct
{
  int start_width;
  int end_width;
  int current_width;
  ply_image_t *image;
  ply_image_t *image_altered;
} progress_t;


typedef struct
{
  int star_count;
  int *star_x;
  int *star_y;
  int *star_refresh;
  int frame_count;
} star_bg_t;

typedef struct
{
  ply_boot_splash_plugin_t *plugin;
  ply_pixel_display_t *display;
  ply_entry_t *entry;
  ply_label_t *label;
  ply_label_t *message_label;
  ply_list_t *sprites;
  ply_rectangle_t box_area, lock_area, logo_area;
  ply_image_t *scaled_background_image;
} view_t;

struct _ply_boot_splash_plugin
{
  ply_event_loop_t *loop;
  ply_boot_splash_mode_t mode;
  ply_image_t *logo_image;
  ply_image_t *lock_image;
  ply_image_t *box_image;
  ply_image_t *star_image;
  
#ifdef  SHOW_PLANETS
  ply_image_t *planet_image[5];
#endif
#ifdef  SHOW_PROGRESS_BAR
  ply_image_t *progress_barimage;
#endif

#ifdef SHOW_LOGO_HALO
  ply_image_t *highlight_logo_image;
#endif

  char *image_dir;
  ply_boot_splash_display_type_t state;
  ply_list_t *views;

  double now;
  
  double progress;
  double progress_target;

  uint32_t root_is_mounted : 1;
  uint32_t is_visible : 1;
  uint32_t is_animating : 1;
};

ply_boot_splash_plugin_interface_t *ply_boot_splash_plugin_get_interface (void);
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
  view->label = ply_label_new ();
  view->message_label = ply_label_new ();

  view->sprites = ply_list_new ();

  return view;
}

static void view_free_sprites (view_t *view);

static void
view_free (view_t *view)
{

  ply_entry_free (view->entry);
  ply_label_free (view->label);
  ply_label_free (view->message_label);
  view_free_sprites (view);
  ply_list_free (view->sprites);

  ply_image_free (view->scaled_background_image);

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

static sprite_t*
add_sprite (view_t      *view,
            ply_image_t *image,
            int          type,
            void        *data)
{
 sprite_t *new_sprite = calloc (1, sizeof (sprite_t));

 new_sprite->x = 0;
 new_sprite->y = 0;
 new_sprite->z = 0;
 new_sprite->oldx = 0;
 new_sprite->oldy = 0;
 new_sprite->oldz = 0;
 new_sprite->opacity = 1;
 new_sprite->refresh_me = 0;
 new_sprite->image = image;
 new_sprite->type = type;
 new_sprite->data = data;

 ply_list_append_data (view->sprites, new_sprite);

 return new_sprite;
}

static void view_setup_scene (view_t *view);

static void
view_start_animation (view_t *view)
{
  unsigned long screen_width, screen_height;

  assert (view != NULL);

  view_setup_scene (view);

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  ply_pixel_display_draw_area (view->display, 0, 0,
                               screen_width, screen_height);
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

static ply_boot_splash_plugin_t *
create_plugin (ply_key_file_t *key_file)
{
  ply_boot_splash_plugin_t *plugin;
  char *image_dir, *image_path;

  srand ((int) ply_get_timestamp ());
  plugin = calloc (1, sizeof (ply_boot_splash_plugin_t));

  plugin->logo_image = ply_image_new (PLYMOUTH_LOGO_FILE);

  image_dir = ply_key_file_get_value (key_file, "space-flares", "ImageDir");

  asprintf (&image_path, "%s/lock.png", image_dir);
  plugin->lock_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/box.png", image_dir);
  plugin->box_image = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/star.png", image_dir);
  plugin->star_image = ply_image_new (image_path);
  free (image_path);

#ifdef  SHOW_PLANETS

  asprintf (&image_path, "%s/plant1.png", image_dir);
  plugin->planet_image[0] = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/plant2.png", image_dir);
  plugin->planet_image[1] = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/plant3.png", image_dir);
  plugin->planet_image[2] = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/plant4.png", image_dir);
  plugin->planet_image[3] = ply_image_new (image_path);
  free (image_path);

  asprintf (&image_path, "%s/plant5.png", image_dir);
  plugin->planet_image[4] = ply_image_new (image_path);
  free (image_path);

#endif
#ifdef  SHOW_PROGRESS_BAR

  asprintf (&image_path, "%s/progress_bar.png", image_dir);
  plugin->progress_barimage = ply_image_new (image_path);
  free (image_path);
#endif
  plugin->state = PLY_BOOT_SPLASH_DISPLAY_NORMAL;
  plugin->progress = 0;
  plugin->progress_target = -1;

  plugin->image_dir = image_dir;

  plugin->views = ply_list_new ();

  return plugin;
}

static void
destroy_plugin (ply_boot_splash_plugin_t *plugin)
{
  if (plugin == NULL)
    return;

  free (plugin->image_dir);

  if (plugin->loop != NULL)
    {
      ply_event_loop_stop_watching_for_exit (plugin->loop, (ply_event_loop_exit_handler_t)
                                             detach_from_event_loop,
                                             plugin);
      detach_from_event_loop (plugin);
    }

  ply_image_free (plugin->logo_image);
  ply_image_free (plugin->lock_image);
  ply_image_free (plugin->box_image);

  ply_image_free (plugin->star_image);
#ifdef  SHOW_PLANETS
  ply_image_free (plugin->planet_image[0]);
  ply_image_free (plugin->planet_image[1]);
  ply_image_free (plugin->planet_image[2]);
  ply_image_free (plugin->planet_image[3]);
  ply_image_free (plugin->planet_image[4]);
#endif
#ifdef  SHOW_PROGRESS_BAR
  ply_image_free (plugin->progress_barimage);
#endif

  free_views (plugin);

  free (plugin);
}

static void
free_sprite (sprite_t* sprite)
{
  if (sprite)
    {
      switch (sprite->type){
          case SPRITE_TYPE_STATIC:
              break;
          case SPRITE_TYPE_SATELLITE:
            {
              satellite_t *satellite = sprite->data;
              ply_image_free (satellite->image_altered);
              break;
            }
              break;
          case SPRITE_TYPE_PROGRESS:
            {
              progress_t *progress = sprite->data;
              ply_image_free (progress->image_altered);
              break;
            }
              break;
          case SPRITE_TYPE_FLARE:
            {
              flare_t *flare = sprite->data;
              ply_image_free (flare->image_a);
              ply_image_free (flare->image_b);
              break;
            }
          case SPRITE_TYPE_STAR_BG:
            {
              star_bg_t *star_bg = sprite->data;
              free (star_bg->star_x);
              free (star_bg->star_y);
              free (star_bg->star_refresh);
              break;
            }
        }

      if (sprite->data) free(sprite->data);
      free(sprite);
    }
 return;
}

static int
sprite_compare_z (void *data_a, void *data_b)
{
 sprite_t *sprite_a = data_a;
 sprite_t *sprite_b = data_b;
 return sprite_a->z - sprite_b->z;
}

static void 
stretch_image(ply_image_t *scaled_image, ply_image_t *orig_image, int width)
{
  int x, y;
  int stretched_width = ply_image_get_width (scaled_image);
  int stretched_height = ply_image_get_height (scaled_image);
  int orig_width = ply_image_get_width (orig_image);
  uint32_t * scaled_image_data = ply_image_get_data (scaled_image);
  uint32_t * orig_image_data = ply_image_get_data (orig_image);
  
  
  for (y=0; y<stretched_height; y++)
    {
      float my_width = y+0.5;
      my_width /= (stretched_height);
      my_width *= 2;
      my_width -= 1;
      my_width *= my_width;
      my_width = sqrt(1-my_width)-1;
      my_width *= stretched_height;
      my_width /= 2;
      my_width = width+my_width;
      for (x=0; x<stretched_width;  x++)
        {
          if(x<my_width)
            {
              uint32_t value = 0x0;
              int new_x = (x * orig_width) / width;
              value = orig_image_data[new_x + y * orig_width];
              scaled_image_data[x + y * stretched_width] = value;
            }
          else
            {
              scaled_image_data[x + y * stretched_width] = 0;
            }
        }
    }
}

static void
progress_update (view_t *view, sprite_t* sprite, double time)
{
  progress_t *progress = sprite->data;
  ply_boot_splash_plugin_t *plugin = view->plugin;
  int newwidth = plugin->progress * (progress->end_width - progress->start_width) + progress->start_width;

  if (progress->current_width >newwidth) return;
  progress->current_width = newwidth;
  stretch_image(progress->image_altered, progress->image, newwidth);
  sprite->opacity = plugin->progress;
  sprite->refresh_me=1;
}


static inline uint32_t 
star_bg_gradient_colour (int x, int y, int width, int height, bool star, float time)
{
  int full_dist =  sqrt(width*width+height*height);
  int my_dist = sqrt(x*x+y*y);
  float val;
  
  uint16_t r0 = 0x0000;  /* start colour:033c73 */
  uint16_t g0 = 0x3c00;
  uint16_t b0 = 0x7300;
  
  uint16_t r1 = 0x0000;  /* end colour:00193a */
  uint16_t g1 = 0x1900;
  uint16_t b1 = 0x3a00;
  
  uint16_t r = r0+((r1-r0)*my_dist)/full_dist;
  uint16_t g = g0+((g1-g0)*my_dist)/full_dist;
  uint16_t b = b0+((b1-b0)*my_dist)/full_dist;

  static uint16_t r_err = 0; 
  static uint16_t g_err = 0; 
  static uint16_t b_err = 0; 
  
  r += r_err;
  g += g_err;
  b += b_err;
  r_err = ((r>>8) | ((r>>8)<<8)) - r;
  g_err = ((g>>8) | ((g>>8)<<8)) - g;
  b_err = ((b>>8) | ((b>>8)<<8)) - b;
  r >>= 8;
  g >>= 8;
  b >>= 8;
  
  if (!star) {
    
    return 0xff000000 | r<<16 | g<<8 | b;
    }
  
  x -= width+720-800;
  y -= height+300-480;
  val = sqrt(x*x+y*y)/100;
  val = (sin(val-time*(2*M_PI)*STAR_HZ+atan2(y, x)*2)+1)/2;
  
  val= val*0.3;
  
  r = r*(1-val) + val*0xff;
  g = g*(1-val) + val*0xff;
  b = b*(1-val) + val*0xff;
  
  return 0xff000000 | r<<16 | g<<8 | b;
}



static void
star_bg_update (view_t *view, sprite_t* sprite, double time)
{
  star_bg_t *star_bg = sprite->data;
  int width = ply_image_get_width (sprite->image);
  int height = ply_image_get_height (sprite->image);
  uint32_t* image_data = ply_image_get_data (sprite->image);
  int i, x, y;
  star_bg->frame_count++;
  star_bg->frame_count%=FRAMES_PER_SECOND/BG_STARS_FRAMES_PER_SECOND;

  for (i = star_bg->frame_count; i < star_bg->star_count; i+=FRAMES_PER_SECOND/BG_STARS_FRAMES_PER_SECOND){
    x = star_bg->star_x[i];
    y = star_bg->star_y[i];
    uint32_t pixel_colour = star_bg_gradient_colour (x, y, width, height, true, time);
    if (abs(((image_data[x + y * width]>>16)&0xff) - ((pixel_colour>>16)&0xff))>8){
        image_data[x + y * width] = pixel_colour;
        star_bg->star_refresh[i]=1;
        }
    }
  
  
  sprite->refresh_me=1;
}

static void
satellite_move (view_t *view, sprite_t* sprite, double time)
{
  ply_boot_splash_plugin_t *plugin = view->plugin;
  satellite_t *satellite = sprite->data;

  int width = ply_image_get_width (sprite->image);
  int height = ply_image_get_height (sprite->image);
  unsigned long screen_width, screen_height;

  sprite->x=cos(satellite->theta+(1-plugin->progress)*2000/(satellite->distance))*satellite->distance;
  sprite->y=sin(satellite->theta+(1-plugin->progress)*2000/(satellite->distance))*satellite->distance;
  sprite->z=0;

  float distance = sqrt(sprite->z*sprite->z+sprite->y*sprite->y);
  float angle_zy = atan2 (sprite->y, sprite->z)-M_PI*0.4;
  sprite->z = distance* cos(angle_zy);
  sprite->y = distance* sin(angle_zy);

  float angle_offset = atan2 (sprite->x, sprite->y);
  float cresent_angle = atan2 (sqrt(sprite->x*sprite->x+sprite->y*sprite->y), sprite->z);
  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

  sprite->x+=(float)satellite->end_x*plugin->progress+(float)satellite->start_x*(1-plugin->progress)-width/2;
  sprite->y+=(float)satellite->end_y*plugin->progress+(float)satellite->start_y*(1-plugin->progress)-height/2;
  
  if (sprite->x > (signed int) screen_width) return;
  if (sprite->y > (signed int) screen_height) return;
  
  if (satellite->type == SATELLITE_TYPE_PLANET)
    {
      int x, y;

      uint32_t *image_data = ply_image_get_data (satellite->image);
      uint32_t *cresent_data = ply_image_get_data (satellite->image_altered);

      for (y=0; y<height; y++) for (x=0; x<width; x++)
        {
      
          float fx = x-(float)width/2;
          float fy = y-(float)height/2;
          float angle = atan2 (fy, fx)+angle_offset;
          float distance = sqrt(fy*fy+fx*fx);
          fx = cos(angle)*(distance/(width/2));
          fy = sin(angle)*(distance/(height/2));
          float want_y = sqrt(1-fx*fx);
          want_y *= -cos(cresent_angle);
          if (fy<want_y)
            {
              cresent_data[x+y*width] = image_data[x+y*width];
            }
          else
            {
              int strength =(fy-want_y)*16+2;
              uint32_t val =0;
              int alpha = ((image_data[x+y*width]>>24) & 0xFF);
              if (strength<=0)strength=1;
              if (strength>=8)strength=8;
              val |= (((image_data[x+y*width]>>24) & 0xFF)/1)<<24;
              val |= (((image_data[x+y*width]>>16) & 0xFF)/strength)<<16;
              val |= (((image_data[x+y*width]>>8 ) & 0xFF)/strength)<<8;
              val |= (((image_data[x+y*width]>>0 ) & 0xFF)/strength+(alpha-alpha/(strength))/8)<<0;
              cresent_data[x+y*width] =val;
            }
        }
    }
  
  if (satellite->type == SATELLITE_TYPE_COMET)
    {
      int x, y;

      uint32_t *image_data = ply_image_get_data (satellite->image);
      uint32_t *comet_data = ply_image_get_data (satellite->image_altered);
      x = width/2;
      image_data[x] = 0xFFFFFFFF;
      x = 2*sin(plugin->progress*62)+width/2;
      image_data[x] = 0xFFFFFFFF;
      x = 2*sin(plugin->progress*163)+width/2;
      image_data[x] = 0xFFFFFFFF;
      x = 2*sin(plugin->progress*275)+width/2;
      image_data[x] = 0xFFFFFFFF;
      for (y=height-1; y>0; y--)
        {
          for (x=1; x<width-1; x++)
            {
               uint32_t pixel;
               pixel = 2 * (image_data[x + (y - 1) * width] >> 24)
                       + (image_data[(x - 1) + (y - 1) * width] >> 24)
                       + (image_data[(x + 1) + (y - 1) * width] >> 24);
               pixel /= 4.05;
               pixel |= pixel<<8;
               pixel |= pixel<<16;
               image_data[x+y*width] = pixel;
            }
        }
      for (x=1; x<width-1; x++) image_data[x] = 0x0;
      for (y=0; y<height; y++) for (x=0; x<width; x++)
        {
          float scale= cos(M_PI*0.4);
          float fx=x;
          float fy=y;
          fx -= (float) width/2;
          fy -= (float) height/2;
          fy /= scale;
          float angle = atan2 (fy, fx)-(satellite->theta+(1-plugin->progress)*2000/(satellite->distance))+M_PI/2*0.0;
          float distance = sqrt(fy*fy+fx*fx);
          fx = cos(angle)*distance;
          fy = sin(angle)*distance;
          fx += (fy*fy*2)/(satellite->distance);
          fx += (float) width/2;
          fy += (float) height/2;
          int ix=fx;
          int iy=fy;
          if (ix<0 || iy<0 || ix>=width || iy>=height){
          comet_data[x+y*width] = 0;
            }
          else
            {
              comet_data[x+y*width] = image_data[ix+iy*width];
            }
        }
    }
  return;
}


static void
sprite_list_sort (view_t *view)
{
  ply_list_sort_stable (view->sprites, &sprite_compare_z);
}

static void
flare_reset (flare_t *flare, int index)
{
  flare->rotate_yz[index]=((float)(rand()%1000)/1000)*2*M_PI;
  flare->rotate_xy[index]=((float)(rand()%1000)/1000)*2*M_PI;
  flare->rotate_xz[index]=((float)(rand()%1000)/1000)*2*M_PI;
  flare->y_size[index]=((float)(rand()%1000)/1000)*0.8+0.2;
  flare->increase_speed[index] = ((float)(rand()%1000)/1000)*0.08+0.08;
  flare->stretch[index]=(((float)(rand()%1000)/1000)*0.1+0.3)*flare->y_size[index];
  flare->z_offset_strength[index]=0.1;
}

static void
flare_update (sprite_t* sprite, double time)
{
  int width;
  int height;
  flare_t *flare = sprite->data;
  ply_image_t *old_image;
  ply_image_t *new_image;
  uint32_t * old_image_data;
  uint32_t * new_image_data;

  flare->frame_count++;
  if (flare->frame_count%(FRAMES_PER_SECOND/FLARE_FRAMES_PER_SECOND)){
    return;
    }

  old_image = flare->image_a;
  new_image = flare->image_b;

  old_image_data = ply_image_get_data (old_image);
  new_image_data = ply_image_get_data (new_image);

  width = ply_image_get_width (new_image);
  height = ply_image_get_height (new_image);


  int b;
  for (b=0; b<FLARE_COUNT; b++)
    {
      int flare_line;
      flare->stretch[b] += (flare->stretch[b] * flare->increase_speed[b]) * (1-(1/(3.01-flare->stretch[b])));
      flare->increase_speed[b]-=0.003;
      flare->z_offset_strength[b]+=0.01;

      if (flare->stretch[b]>2 || flare->stretch[b]<0.2)
        {
          flare_reset (flare, b);
        }
      for (flare_line=0; flare_line<FLARE_LINE_COUNT; flare_line++)
        {
          double x, y, z;
          double angle, distance;
          float theta;
          for (theta = -M_PI+(0.05*cos(flare->increase_speed[b]*1000+flare_line)); theta < M_PI; theta+=0.05)
            {
              int ix;
              int iy;

              x=(cos(theta)+0.5)*flare->stretch[b]*0.8;
              y=(sin(theta))*flare->y_size[b];
              z=x*(sin(b+flare_line*flare_line))*flare->z_offset_strength[b];
              
              float strength = 1.1-(x/2)+flare->increase_speed[b]*3;
              x+=4.5;
              if ((x*x+y*y+z*z)<25) continue;

              strength = CLAMP(strength, 0, 1);
              strength*=32;

              x+=0.05*sin(4*theta*(sin(b+flare_line*5)));
              y+=0.05*cos(4*theta*(sin(b+flare_line*5)));
              z+=0.05*sin(4*theta*(sin(b+flare_line*5)));

              distance = sqrt(x*x+y*y);
              angle = atan2 (y, x) + flare->rotate_xy[b]+0.02*sin(b*flare_line) ;
              x = distance* cos(angle);
              y = distance* sin(angle);

              distance = sqrt(z*z+y*y);
              angle = atan2 (y, z) + flare->rotate_yz[b]+0.02*sin(3*b*flare_line);
              z = distance* cos(angle);
              y = distance* sin(angle);

              distance = sqrt(x*x+z*z);
              angle = atan2 (z, x) + flare->rotate_xz[b]+0.02*sin(8*b*flare_line);
              x = distance* cos(angle);
              z = distance* sin(angle);


              x*=41;
              y*=41;

              x+=720-800+width;
              y+=300-480+height;

              ix=x;
              iy=y;
              if (ix>=(width-1) || iy>=(height-1) || ix<=0 || iy<=0) continue;

              uint32_t colour = MIN (strength + (old_image_data[ix + iy * width]>>24), 255);
              colour <<= 24;
              old_image_data[ix + iy * width] = colour;

            }
        }
    }

  {
  int  x, y;
  for (y = 1; y < (height-1); y++)
    {
      for (x = 1; x < (width-1); x++)
        {
          uint32_t value = 0;
          value += (old_image_data[(x-1) + (y-1) * width]>>24)*1;
          value += (old_image_data[ x +    (y-1) * width]>>24)*2;
          value += (old_image_data[(x+1) + (y-1) * width]>>24)*1;
          value += (old_image_data[(x-1) +  y    * width]>>24)*2;
          value += (old_image_data[ x +     y    * width]>>24)*8;
          value += (old_image_data[(x+1) +  y    * width]>>24)*2;
          value += (old_image_data[(x-1) + (y+1) * width]>>24)*1;
          value += (old_image_data[ x +    (y+1) * width]>>24)*2;
          value += (old_image_data[(x+1) + (y+1) * width]>>24)*1;
          value /=21;
          value = (value<<24) | ((int)(value*0.7)<<16) | (value<<8) | (value<<0);
          new_image_data[x +y * width] = value;
        }
    }
  }
  flare->image_a = new_image;
  flare->image_b = old_image;
  sprite->image = new_image;
  sprite->refresh_me = 1;
  return;
}

static void
sprite_move (view_t *view, sprite_t* sprite, double time)
{
  sprite->oldx = sprite->x;
  sprite->oldy = sprite->y;
  sprite->oldz = sprite->z;
  switch (sprite->type){
      case SPRITE_TYPE_STATIC:
          break;
      case SPRITE_TYPE_PROGRESS:
          progress_update (view, sprite, time);
          break;
      case SPRITE_TYPE_FLARE:
          flare_update (sprite, time);
          break;
      case SPRITE_TYPE_SATELLITE:
          satellite_move (view, sprite, time);
          break;
      case SPRITE_TYPE_STAR_BG:
          star_bg_update (view, sprite, time);
          break;
    }
}

static void
view_animate_attime (view_t *view, double time)
{
  ply_list_node_t *node;
  ply_boot_splash_plugin_t *plugin;

  plugin = view->plugin;

  if (plugin->progress_target>=0)
      plugin->progress = (plugin->progress*10 + plugin->progress_target) /11;
    
  node = ply_list_get_first_node (view->sprites);
  while(node)
    {
      sprite_t* sprite = ply_list_node_get_data (node);
      sprite_move (view, sprite, time);
      node = ply_list_get_next_node (view->sprites, node);
    }

  sprite_list_sort (view);

  for(node = ply_list_get_first_node (view->sprites); node; node = ply_list_get_next_node (view->sprites, node))
    {
      sprite_t* sprite = ply_list_node_get_data (node);
      if (sprite->x != sprite->oldx ||
          sprite->y != sprite->oldy ||
          sprite->z != sprite->oldz ||
          sprite->refresh_me)
        {
          sprite->refresh_me=0;

          int width = ply_image_get_width (sprite->image);
          int height= ply_image_get_height (sprite->image);
          
          if (sprite->type == SPRITE_TYPE_STAR_BG){
            star_bg_t *star_bg = sprite->data;
            int i;
            for (i=0; i<star_bg->star_count; i++){
                if (star_bg->star_refresh[i]){
                    ply_pixel_display_draw_area (view->display,
                                                 sprite->x+star_bg->star_x[i], sprite->y+star_bg->star_y[i], 1, 1);
                    star_bg->star_refresh[i]=0;
                    }
              }
            continue;
            }
          
          
          
          int x = sprite->x - sprite->oldx;
          int y = sprite->y - sprite->oldy;

          if (x < width && x > -width && y < height && y > -height)
            {
              x=MIN(sprite->x, sprite->oldx);
              y=MIN(sprite->y, sprite->oldy);
              width =(MAX(sprite->x, sprite->oldx)-x)+ply_image_get_width (sprite->image);
              height=(MAX(sprite->y, sprite->oldy)-y)+ply_image_get_height (sprite->image);
              ply_pixel_display_draw_area (view->display,
                                           x, y, width, height);
            }
          else
            {
              ply_pixel_display_draw_area (view->display,
                                           sprite->x, sprite->y, width, height);
              ply_pixel_display_draw_area (view->display,
                                           sprite->oldx, sprite->oldy, width, height);
            }
        }
    }
}

static void
on_timeout (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;
  double sleep_time;
  double now;

  now = ply_get_timestamp ();

  node = ply_list_get_first_node (plugin->views);

  while (node != NULL)
    {
      ply_list_node_t *next_node;
      view_t *view;

      view = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (plugin->views, node);

      view_animate_attime (view, now);

      node = next_node;
    }
  plugin->now = now;

  sleep_time = 1.0 / FRAMES_PER_SECOND;

  ply_event_loop_watch_for_timeout (plugin->loop, 
                                    sleep_time,
                                    (ply_event_loop_timeout_handler_t)
                                    on_timeout, plugin);
}

static void
on_boot_progress (ply_boot_splash_plugin_t *plugin,
                  double                    duration,
                  double                    percent_done)
{
  if (plugin->progress_target<0)
    plugin->progress = percent_done;
  plugin->progress_target = percent_done;
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

  on_timeout (plugin);

  plugin->is_animating = true;
}

static void
view_free_sprites (view_t *view)
{
  ply_list_node_t *node;

  for (node = ply_list_get_first_node (view->sprites);
       node != NULL;
       node = ply_list_get_next_node (view->sprites, node))
    {
      sprite_t* sprite = ply_list_node_get_data (node);
      free_sprite (sprite);
    }
  ply_list_remove_all_nodes (view->sprites);
}

static void
stop_animation (ply_boot_splash_plugin_t *plugin)
{
  ply_list_node_t *node;

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

#ifdef  SHOW_LOGO_HALO
  ply_image_free(plugin->highlight_logo_image);
#endif

  for(node = ply_list_get_first_node (plugin->views); node; node = ply_list_get_next_node (plugin->views, node))
    {
      view_t *view = ply_list_node_get_data (node);
      view_free_sprites (view);
    }
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
                 int                 height);
static void
on_draw (view_t                   *view,
         ply_pixel_buffer_t       *pixel_buffer,
         int                       x,
         int                       y,
         int                       width,
         int                       height)
{
  ply_boot_splash_plugin_t *plugin;
  ply_rectangle_t clip_area;
  clip_area.x = x;
  clip_area.y = y;
  clip_area.width = width;
  clip_area.height = height;

  bool single_pixel = 0;
  float pixel_r=0;
  float pixel_g=0;
  float pixel_b=0;

  plugin = view->plugin;

  if (width==1 && height==1)
      single_pixel = true;

  if (plugin->state == PLY_BOOT_SPLASH_DISPLAY_QUESTION_ENTRY || 
      plugin->state == PLY_BOOT_SPLASH_DISPLAY_PASSWORD_ENTRY  )
    {
      uint32_t *box_data, *lock_data;

      draw_background (view, pixel_buffer, x, y, width, height);

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
      ply_list_node_t *node;

      draw_background (view, pixel_buffer, x, y, width, height);

      for(node = ply_list_get_first_node (view->sprites); node; node = ply_list_get_next_node (view->sprites, node))
        {
          sprite_t* sprite = ply_list_node_get_data (node);
          ply_rectangle_t sprite_area;


          sprite_area.x = sprite->x;
          sprite_area.y = sprite->y;

          if (sprite_area.x>=(x+width)) continue;
          if (sprite_area.y>=(y+height)) continue;

          sprite_area.width =  ply_image_get_width (sprite->image);
          sprite_area.height = ply_image_get_height (sprite->image);

          if ((int) (sprite_area.x+sprite_area.width)<=x) continue;
          if ((int) (sprite_area.y+sprite_area.height)<=y) continue;

          if (single_pixel)
            {
              uint32_t* image_data = ply_image_get_data (sprite->image);
              uint32_t overlay_pixel = image_data[(x-sprite_area.x)+(y-sprite_area.y)*sprite_area.width];
              float  alpha = (float)((overlay_pixel>>24)&0xff)/255 * sprite->opacity;
              float  red =   (float)((overlay_pixel>>16)&0xff)/255 * sprite->opacity;
              float  green = (float)((overlay_pixel>>8) &0xff)/255 * sprite->opacity;
              float  blue =  (float)((overlay_pixel>>0) &0xff)/255 * sprite->opacity;
              pixel_r = pixel_r*(1-alpha) + red;
              pixel_g = pixel_g*(1-alpha) + green;
              pixel_b = pixel_b*(1-alpha) + blue;
            }
          else
            {
              ply_pixel_buffer_fill_with_argb32_data_at_opacity_with_clip (pixel_buffer,
                                                                           &sprite_area, &clip_area,
                                                                           ply_image_get_data (sprite->image), sprite->opacity);
            }
        }
    }
  if (single_pixel){
      ply_pixel_buffer_fill_with_color (pixel_buffer, &clip_area, pixel_r, pixel_g, pixel_b, 1.0);
      }
  ply_label_draw_area (view->message_label,
                       pixel_buffer,
                       x, y, width, height);
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
  ply_rectangle_t image_area;

  plugin = view->plugin;

  area.x = x;
  area.y = y;
  area.width = width;
  area.height = height;

  image_area.x = 0;
  image_area.y = 0;
  image_area.width = ply_image_get_width (view->scaled_background_image);
  image_area.height = ply_image_get_height (view->scaled_background_image);

  ply_pixel_buffer_fill_with_argb32_data_with_clip (pixel_buffer,
                                                    &image_area, &area,
                                                    ply_image_get_data (view->scaled_background_image));
  
  image_area.x = image_area.width-ply_image_get_width(plugin->star_image);
  image_area.y = image_area.height-ply_image_get_height(plugin->star_image);
  image_area.width = ply_image_get_width(plugin->star_image);
  image_area.height = ply_image_get_height(plugin->star_image);
  
  
  ply_pixel_buffer_fill_with_argb32_data_with_clip (pixel_buffer,
                                                     &image_area, &area,
                                                     ply_image_get_data (plugin->star_image));
                                                     
  image_area.x = 20;
  image_area.y = 20;
  image_area.width = ply_image_get_width(plugin->logo_image);
  image_area.height = ply_image_get_height(plugin->logo_image);
  
  
  ply_pixel_buffer_fill_with_argb32_data_with_clip (pixel_buffer,
                                                     &image_area, &area,
                                                     ply_image_get_data (plugin->logo_image));
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

static void
view_setup_scene (view_t *view)
{
  ply_boot_splash_plugin_t *plugin;
  sprite_t *sprite;
  int i;
  int x, y;
  int width = 360;
  int height = 460;
  unsigned long screen_width, screen_height;

  plugin = view->plugin;

  screen_width = ply_pixel_display_get_width (view->display);
  screen_height = ply_pixel_display_get_height (view->display);

    {
      star_bg_t* star_bg;
      if (view->scaled_background_image)
        ply_image_free (view->scaled_background_image);
      view->scaled_background_image = ply_image_resize (plugin->logo_image, screen_width, screen_height);
      star_bg = malloc(sizeof(star_bg_t));
      star_bg->star_count = (screen_width * screen_height)/400;
      star_bg->star_x = malloc(sizeof(int)*star_bg->star_count);
      star_bg->star_y = malloc(sizeof(int)*star_bg->star_count);
      star_bg->star_refresh = malloc(sizeof(int)*star_bg->star_count);
      star_bg->frame_count=0;
      sprite = add_sprite (view, view->scaled_background_image, SPRITE_TYPE_STAR_BG, star_bg);
      sprite->z = -10000;
      
      uint32_t* image_data = ply_image_get_data (view->scaled_background_image);
      for (y=0; y< (int) screen_height; y++) for (x=0; x< (int) screen_width; x++){
          image_data[x + y * screen_width] = star_bg_gradient_colour(x, y, screen_width, screen_height, false, 0);
        }
      
      for (i=0; i<star_bg->star_count; i++){
          do
            {
              x = rand()%screen_width;
              y = rand()%screen_height;
            }
          while (image_data[x + y * screen_width] == 0xFFFFFFFF);
          star_bg->star_refresh[i] = 0;
          star_bg->star_x[i] = x;
          star_bg->star_y[i] = y;
          image_data[x + y * screen_width] = 0xFFFFFFFF;
        }
      for (i=0; i<(int) (screen_width * screen_height)/400; i++){
        x = rand()%screen_width;
        y = rand()%screen_height;
        image_data[x + y * screen_width] = star_bg_gradient_colour(x, y, screen_width, screen_height, true, ((float)x*y*13/10000));
        }
      
      for (i=0; i<star_bg->star_count; i++){
        image_data[star_bg->star_x[i]  + star_bg->star_y[i] * screen_width] = 
            star_bg_gradient_colour(star_bg->star_x[i], star_bg->star_y[i], screen_width, screen_height, true, 0.0);
        }
    }

  sprite = add_sprite (view, plugin->logo_image, SPRITE_TYPE_STATIC, NULL);
  sprite->x=screen_width/2-ply_image_get_width(plugin->logo_image)/2;
  sprite->y=screen_height/2-ply_image_get_height(plugin->logo_image)/2;
  sprite->z=1000;

#ifdef SHOW_LOGO_HALO
  plugin->highlight_logo_image = ply_image_resize (plugin->logo_image, ply_image_get_width(plugin->logo_image)+HALO_BLUR*2, ply_image_get_height(plugin->logo_image)+HALO_BLUR*2);
  highlight_image (plugin->highlight_logo_image, plugin->logo_image, HALO_BLUR);
  sprite = add_sprite (view, plugin->highlight_logo_image, SPRITE_TYPE_STATIC, NULL);
  sprite->x=10-HALO_BLUR;
  sprite->y=10-HALO_BLUR;
  sprite->z=-910;
#endif

  sprite = add_sprite (view, plugin->star_image, SPRITE_TYPE_STATIC, NULL);
  sprite->x=screen_width-ply_image_get_width(plugin->star_image);
  sprite->y=screen_height-ply_image_get_height(plugin->star_image);
  sprite->z=0;
#ifdef  SHOW_PLANETS
  for (i=0; i<5; i++)
    {
       satellite_t* satellite = malloc(sizeof(satellite_t));
       satellite->type=SATELLITE_TYPE_PLANET;
       satellite->end_x=satellite->start_x=720-800+screen_width;
       satellite->end_y=satellite->start_y=300-480+screen_height;

       satellite->distance=i*100+280;
       satellite->theta=M_PI*0.8;
       satellite->image=plugin->planet_image[i];
       satellite->image_altered=ply_image_resize (satellite->image, ply_image_get_width(satellite->image), ply_image_get_height(satellite->image));
       sprite = add_sprite (view, satellite->image_altered, SPRITE_TYPE_SATELLITE, satellite);
       satellite_move (plugin, sprite, 0);

    }
#endif
#ifdef  SHOW_COMETS
  for (i=0; i<1; i++)
    {
      satellite_t* satellite = malloc(sizeof(satellite_t));
      satellite->type=SATELLITE_TYPE_COMET;
      satellite->end_x=satellite->start_x=720-800+screen_width;
      satellite->end_y=satellite->start_y=300-480+screen_height;
      satellite->distance=550+i*50;
      satellite->theta=M_PI*0.8;
#define COMET_SIZE 64
      satellite->image=ply_image_resize (plugin->progress_barimage, COMET_SIZE, COMET_SIZE);
      satellite->image_altered=ply_image_resize (satellite->image, COMET_SIZE, COMET_SIZE);
      uint32_t * image_data = ply_image_get_data (satellite->image);
      uint32_t * image_altered_data = ply_image_get_data (satellite->image_altered);


      for (y=0; y<COMET_SIZE; y++)for (x=0; x<COMET_SIZE; x++){
          image_data[x + y * COMET_SIZE] = 0x0;
          image_altered_data[x + y * COMET_SIZE] = 0x0;
        }
            
      sprite = add_sprite (view, satellite->image_altered, SPRITE_TYPE_SATELLITE, satellite);
      for (x=0; x<COMET_SIZE; x++) satellite_move (plugin, sprite, 0);
     }
#endif

#ifdef  SHOW_PROGRESS_BAR
  progress_t* progress = malloc(sizeof(progress_t));
  
  progress->image = plugin->progress_barimage;
  
  x = screen_width/2-ply_image_get_width(plugin->logo_image)/2;;
  y = screen_height/2+ply_image_get_height(plugin->logo_image)/2+20;
  progress->image_altered = ply_image_resize (plugin->progress_barimage, ply_image_get_width(plugin->logo_image), ply_image_get_height(plugin->progress_barimage));
  progress->start_width = 1;
  progress->end_width = ply_image_get_width(plugin->logo_image);
  progress->current_width = 0;
  
  sprite = add_sprite (view, progress->image_altered, SPRITE_TYPE_PROGRESS, progress);
  sprite->x=x;
  sprite->y=y;
  sprite->z=10011;
  progress_update (view, sprite, 0);
  
  
  
#endif

  flare_t *flare = malloc(sizeof(flare_t));

  flare->image_a = ply_image_resize (plugin->star_image, width, height);
  flare->image_b = ply_image_resize (plugin->star_image, width, height);

  sprite = add_sprite (view, flare->image_a, SPRITE_TYPE_FLARE, flare);
  sprite->x=screen_width-width;
  sprite->y=screen_height-height;
  sprite->z=1;

  sprite_list_sort (view);

  uint32_t * old_image_data = ply_image_get_data (flare->image_a);
  uint32_t * new_image_data = ply_image_get_data (flare->image_b);


  for (y = 0; y < height; y++)
    {
      for (x = 0; x < width; x++)
        {
          new_image_data[x + y * width] = 0x0;
          old_image_data[x + y * width] = 0x0;
        }
    }

  for (i=0; i<FLARE_COUNT; i++)
    {
      flare_reset (flare, i);
    }
  flare->frame_count=0;
  flare_update(sprite, i);
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

  ply_trace ("loading planet images");
#ifdef  SHOW_PLANETS
  if (!ply_image_load (plugin->planet_image[0]))
    return false;
  if (!ply_image_load (plugin->planet_image[1]))
    return false;
  if (!ply_image_load (plugin->planet_image[2]))
    return false;
  if (!ply_image_load (plugin->planet_image[3]))
    return false;
  if (!ply_image_load (plugin->planet_image[4]))
    return false;
#endif
#ifdef  SHOW_PROGRESS_BAR
  if (!ply_image_load (plugin->progress_barimage))
    return false;
#endif

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
  stop_animation (plugin);
  ply_trigger_pull (idle_trigger, NULL);
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
      .on_boot_progress = on_boot_progress,
      .hide_splash_screen = hide_splash_screen,
      .on_root_mounted = on_root_mounted,
      .become_idle = become_idle,
      .display_normal = display_normal,
      .display_password = display_password,
      .display_question = display_question,      
      .display_message = display_message,
    };

  return &plugin_interface;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
