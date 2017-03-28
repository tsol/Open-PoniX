/* script-lib-sprite.c - script library controling sprites
 *
 * Copyright (C) 2009 Charlie Brej <cbrej@cs.man.ac.uk>
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
 */
#include "config.h"
#include "ply-image.h"
#include "ply-utils.h"
#include "ply-logger.h"
#include "ply-key-file.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "script.h"
#include "script-parse.h"
#include "script-execute.h"
#include "script-object.h"
#include "script-lib-image.h"
#include "script-lib-sprite.h"
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "script-lib-sprite.script.h"

static void sprite_free (script_obj_t *obj)
{
  sprite_t *sprite = obj->data.native.object_data;
  sprite->remove_me = true;
}

static script_return_t sprite_new (script_state_t *state,
                                   void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  script_obj_t *reply;

  sprite_t *sprite = calloc (1, sizeof (sprite_t));

  sprite->x = 0;
  sprite->y = 0;
  sprite->z = 0;
  sprite->opacity = 1.0;
  sprite->old_x = 0;
  sprite->old_y = 0;
  sprite->old_z = 0;
  sprite->old_width = 0;
  sprite->old_height = 0;
  sprite->old_opacity = 1.0;
  sprite->refresh_me = false;
  sprite->remove_me = false;
  sprite->image = NULL;
  sprite->image_obj = NULL;
  ply_list_append_data (data->sprite_list, sprite);

  reply = script_obj_new_native (sprite, data->class);
  return script_return_obj (reply);
}

static script_return_t sprite_get_image (script_state_t *state,
                                         void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);
  
  if (sprite && sprite->image_obj)
    {
      script_obj_ref (sprite->image_obj);
      return script_return_obj (sprite->image_obj);
    }
  return script_return_obj_null ();
}

static script_return_t sprite_set_image (script_state_t *state,
                                         void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);
  script_obj_t *script_obj_image = script_obj_hash_get_element (state->local,
                                                                "image");
  script_obj_deref (&script_obj_image);
  ply_pixel_buffer_t *image = script_obj_as_native_of_class_name (script_obj_image,
                                                                  "image");

  if (image && sprite)
    {
      script_obj_unref (sprite->image_obj);
      script_obj_ref (script_obj_image);
      sprite->image = image;
      sprite->image_obj = script_obj_image;
      sprite->refresh_me = true;
    }
  script_obj_unref (script_obj_image);

  return script_return_obj_null ();
}

static script_return_t sprite_get_x (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    return script_return_obj (script_obj_new_number (sprite->x));
  return script_return_obj_null ();
}

static script_return_t sprite_get_y (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    return script_return_obj (script_obj_new_number (sprite->y));
  return script_return_obj_null ();
}

static script_return_t sprite_get_z (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    return script_return_obj (script_obj_new_number (sprite->z));
  return script_return_obj_null ();
}

static script_return_t sprite_get_opacity (script_state_t *state,
                                           void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    return script_return_obj (script_obj_new_number (sprite->opacity));
  return script_return_obj_null ();
}

static script_return_t sprite_set_x (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    sprite->x = script_obj_hash_get_number (state->local, "value");
  return script_return_obj_null ();
}

static script_return_t sprite_set_y (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    sprite->y = script_obj_hash_get_number (state->local, "value");
  return script_return_obj_null ();
}

static script_return_t sprite_set_z (script_state_t *state,
                                     void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    sprite->z = script_obj_hash_get_number (state->local, "value");
  return script_return_obj_null ();
}

static script_return_t sprite_set_opacity (script_state_t *state,
                                           void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  sprite_t *sprite = script_obj_as_native_of_class (state->this, data->class);

  if (sprite)
    sprite->opacity = script_obj_hash_get_number (state->local, "value");
  return script_return_obj_null ();
}

static script_return_t sprite_window_get_width (script_state_t *state,
                                                void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  int index;
  script_obj_t *index_obj;
  script_lib_display_t *display;
  unsigned int width;

  index_obj = script_obj_hash_peek_element (state->local, "window");

  if (index_obj)
    {
      index = script_obj_as_number (index_obj);
      script_obj_unref(index_obj);
      if (index < 0)
        return script_return_obj_null ();
      node = ply_list_get_nth_node (data->displays, index);
      if (node == NULL)
        return script_return_obj_null ();
      display = ply_list_node_get_data (node);
      width = ply_pixel_display_get_width (display->pixel_display);
      return script_return_obj (script_obj_new_number (width));
    }

  width = 0;
  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      display = ply_list_node_get_data (node);
      if (width == 0)
        width = ply_pixel_display_get_width (display->pixel_display);
      else
        width = MIN (width, ply_pixel_display_get_width (display->pixel_display));
    }
  return script_return_obj (script_obj_new_number (width));
}

static script_return_t sprite_window_get_height (script_state_t *state,
                                                 void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  int index;
  script_obj_t *index_obj;
  script_lib_display_t *display;
  unsigned int height;

  index_obj = script_obj_hash_peek_element (state->local, "window");

  if (index_obj)
    {
      index = script_obj_as_number (index_obj);
      script_obj_unref(index_obj);
      if (index < 0)
        return script_return_obj_null ();
      node = ply_list_get_nth_node (data->displays, index);
      if (node == NULL)
        return script_return_obj_null ();
      display = ply_list_node_get_data (node);
      height = ply_pixel_display_get_height (display->pixel_display);
      return script_return_obj (script_obj_new_number (height));
    }

  height = 0;
  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      display = ply_list_node_get_data (node);
      if (height == 0)
        height = ply_pixel_display_get_height (display->pixel_display);
      else
        height = MIN (height, ply_pixel_display_get_height (display->pixel_display));
    }
  return script_return_obj (script_obj_new_number (height));
}

static script_return_t sprite_window_get_x (script_state_t *state,
                                            void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  int index;
  script_obj_t *index_obj;
  script_lib_display_t *display;
  int x;

  index_obj = script_obj_hash_peek_element (state->local, "window");

  if (index_obj)
    {
      index = script_obj_as_number (index_obj);
      script_obj_unref(index_obj);
      if (index < 0)
        return script_return_obj_null ();
      node = ply_list_get_nth_node (data->displays, index);
      if (node == NULL)
        return script_return_obj_null ();
      display = ply_list_node_get_data (node);
      return script_return_obj (script_obj_new_number (display->x));
    }

  x = 0;
  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      display = ply_list_node_get_data (node);
      x = MAX (x, display->x);
    }
  return script_return_obj (script_obj_new_number (x));
}


static script_return_t sprite_window_get_y (script_state_t *state,
                                            void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  int index;
  script_obj_t *index_obj;
  script_lib_display_t *display;
  int y;

  index_obj = script_obj_hash_peek_element (state->local, "window");

  if (index_obj)
    {
      index = script_obj_as_number (index_obj);
      script_obj_unref(index_obj);
      if (index < 0)
        return script_return_obj_null ();
      node = ply_list_get_nth_node (data->displays, index);
      if (node == NULL)
        return script_return_obj_null ();
      display = ply_list_node_get_data (node);
      return script_return_obj (script_obj_new_number (display->y));
    }

  y = 0;
  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      display = ply_list_node_get_data (node);
      y = MAX (y, display->y);
    }
  return script_return_obj (script_obj_new_number (y));
}

static script_return_t sprite_window_set_x (script_state_t *state,
                                            void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  script_lib_display_t *display;
  int index;
  int x;

  index = script_obj_hash_get_number (state->local, "window");
  x = script_obj_hash_get_number (state->local, "value");
  node = ply_list_get_nth_node (data->displays, index);
  if (node)
    {
      display = ply_list_node_get_data (node);
      if (display->x != x)
        {
          display->x = x;
          data->full_refresh = true;
        }
    }
  return script_return_obj_null ();
}

static script_return_t sprite_window_set_y (script_state_t *state,
                                            void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;
  ply_list_node_t *node;
  script_lib_display_t *display;
  int index;
  int y;

  index = script_obj_hash_get_number (state->local, "window");
  y = script_obj_hash_get_number (state->local, "value");
  node = ply_list_get_nth_node (data->displays, index);
  if (node)
    {
      display = ply_list_node_get_data (node);
      if (display->y != y)
        {
          display->y = y;
          data->full_refresh = true;
        }
    }
  return script_return_obj_null ();
}

static uint32_t extract_rgb_color (script_state_t *state)
{
  uint8_t red =   CLAMP (255 * script_obj_hash_get_number (state->local, "red"),   0, 255);
  uint8_t green = CLAMP (255 * script_obj_hash_get_number (state->local, "green"), 0, 255);
  uint8_t blue =  CLAMP (255 * script_obj_hash_get_number (state->local, "blue"),  0, 255);

  return (uint32_t) red << 16 | green << 8 | blue;
}

static script_return_t sprite_window_set_background_top_color (script_state_t *state,
                                                               void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;

  data->background_color_start = extract_rgb_color (state);
  data->full_refresh = true;
  return script_return_obj_null ();
}

static script_return_t sprite_window_set_background_bottom_color (script_state_t *state,
                                                                  void           *user_data)
{
  script_lib_sprite_data_t *data = user_data;

  data->background_color_end = extract_rgb_color (state);
  data->full_refresh = true;
  return script_return_obj_null ();
}

static void script_lib_sprite_draw_area (script_lib_display_t *display,
                                         ply_pixel_buffer_t   *pixel_buffer,
                                         int                   x,
                                         int                   y,
                                         int                   width,
                                         int                   height)
{
  ply_rectangle_t clip_area;
  ply_list_node_t *node;
  script_lib_sprite_data_t *data = display->data;

  clip_area.x = x;
  clip_area.y = y;
  clip_area.width = width;
  clip_area.height = height;

  if (data->background_color_start == data->background_color_end)
    ply_pixel_buffer_fill_with_hex_color (pixel_buffer,
                                          &clip_area,
                                          data->background_color_start);
  else
    ply_pixel_buffer_fill_with_gradient (pixel_buffer,
                                         &clip_area,
                                         data->background_color_start,
                                         data->background_color_end);
  for (node = ply_list_get_first_node (data->sprite_list);
       node;
       node = ply_list_get_next_node (data->sprite_list, node))
    {
      sprite_t *sprite = ply_list_node_get_data (node);
      int position_x, position_y;

      if (!sprite->image) continue;
      if (sprite->remove_me) continue;
      if (sprite->opacity < 0.011) continue;

      position_x = sprite->x - display->x;
      position_y = sprite->y - display->y;

      if (position_x >= (x + width)) continue;
      if (position_y >= (y + height)) continue;

      if ((position_x + (int) ply_pixel_buffer_get_width (sprite->image)) <= x) continue;
      if ((position_y + (int) ply_pixel_buffer_get_height (sprite->image)) <= y) continue;
      ply_pixel_buffer_fill_with_buffer_at_opacity_with_clip (pixel_buffer,
                                                              sprite->image,
                                                              position_x,
                                                              position_y,
                                                              &clip_area,
                                                              sprite->opacity);
    }
}

static void 
draw_area (script_lib_sprite_data_t *data,
           int                       x,
           int                       y,
           int                       width,
           int                       height)
{
  ply_list_node_t *node;
  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      script_lib_display_t* display = ply_list_node_get_data (node);

      ply_pixel_display_draw_area (display->pixel_display,
                                   x - display->x,
                                   y - display->y,
                                   width,
                                   height);
    }
}

script_lib_sprite_data_t *script_lib_sprite_setup (script_state_t *state,
                                                   ply_list_t     *pixel_displays)
{
  ply_list_node_t *node;
  unsigned int max_width, max_height;
  script_lib_sprite_data_t *data = malloc (sizeof (script_lib_sprite_data_t));

  data->class = script_obj_native_class_new (sprite_free, "sprite", data);
  data->sprite_list = ply_list_new ();
  data->displays = ply_list_new ();

  max_width = 0;
  max_height = 0;

  for (node = ply_list_get_first_node (pixel_displays);
       node;
       node = ply_list_get_next_node (pixel_displays, node))
    {
      ply_pixel_display_t *pixel_display = ply_list_node_get_data (node);
      max_width = MAX(max_width, ply_pixel_display_get_width (pixel_display));
      max_height = MAX(max_height, ply_pixel_display_get_height (pixel_display));
    }

  for (node = ply_list_get_first_node (pixel_displays);
       node;
       node = ply_list_get_next_node (pixel_displays, node))
    {
      ply_pixel_display_t *pixel_display = ply_list_node_get_data (node);
      script_lib_display_t *script_display = malloc (sizeof(script_lib_display_t));
      script_display->pixel_display = pixel_display;
      
      script_display->x = (max_width - ply_pixel_display_get_width (pixel_display)) / 2;
      script_display->y = (max_height - ply_pixel_display_get_height (pixel_display)) / 2;
      
      script_display->data = data;
      ply_pixel_display_set_draw_handler (pixel_display,
                                          (ply_pixel_display_draw_handler_t)
                                          script_lib_sprite_draw_area, script_display);

      ply_list_append_data (data->displays, script_display);
    }

  script_obj_t *sprite_hash = script_obj_hash_get_element (state->global, "Sprite");
  script_add_native_function (sprite_hash,
                              "_New",
                              sprite_new,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "GetImage",
                              sprite_get_image,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "SetImage",
                              sprite_set_image,
                              data,
                              "image",
                              NULL);
  script_add_native_function (sprite_hash,
                              "GetX",
                              sprite_get_x,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "SetX",
                              sprite_set_x,
                              data,
                              "value",
                              NULL);
  script_add_native_function (sprite_hash,
                              "GetY",
                              sprite_get_y,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "SetY",
                              sprite_set_y,
                              data,
                              "value",
                              NULL);
  script_add_native_function (sprite_hash,
                              "GetZ",
                              sprite_get_z,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "SetZ",
                              sprite_set_z,
                              data,
                              "value",
                              NULL);
  script_add_native_function (sprite_hash,
                              "GetOpacity",
                              sprite_get_opacity,
                              data,
                              NULL);
  script_add_native_function (sprite_hash,
                              "SetOpacity",
                              sprite_set_opacity,
                              data,
                              "value",
                              NULL);
  script_obj_unref (sprite_hash);


  script_obj_t *window_hash = script_obj_hash_get_element (state->global, "Window");
  script_add_native_function (window_hash,
                              "GetWidth",
                              sprite_window_get_width,
                              data,
                              "window",
                              NULL);
  script_add_native_function (window_hash,
                              "GetHeight",
                              sprite_window_get_height,
                              data,
                              "window",
                              NULL);
  script_add_native_function (window_hash,
                              "GetX",
                              sprite_window_get_x,
                              data,
                              "window",
                              NULL);
  script_add_native_function (window_hash,
                              "GetY",
                              sprite_window_get_y,
                              data,
                              "window",
                              NULL);
  script_add_native_function (window_hash,
                              "SetX",
                              sprite_window_set_x,
                              data,
                              "window",
                              "value",
                              NULL);
  script_add_native_function (window_hash,
                              "SetY",
                              sprite_window_set_y,
                              data,
                              "window",
                              "value",
                              NULL);
  script_add_native_function (window_hash,
                              "SetBackgroundTopColor",
                              sprite_window_set_background_top_color,
                              data,
                              "red",
                              "green",
                              "blue",
                              NULL);
  script_add_native_function (window_hash,
                              "SetBackgroundBottomColor",
                              sprite_window_set_background_bottom_color,
                              data,
                              "red",
                              "green",
                              "blue",
                              NULL);
  script_obj_unref (window_hash);

  data->script_main_op = script_parse_string (script_lib_sprite_string, "script-lib-sprite.script");
  data->background_color_start = 0x000000;
  data->background_color_end   = 0x000000;
  data->full_refresh = true;
  script_return_t ret = script_execute (state, data->script_main_op);
  script_obj_unref (ret.object);
  return data;
}

static int
sprite_compare_z (void *data_a, void *data_b)
{
 sprite_t *sprite_a = data_a;
 sprite_t *sprite_b = data_b;
 return sprite_a->z - sprite_b->z;
}

static void
region_add_area      (ply_region_t *region,
                      long          x,
                      long          y,
                      unsigned long width,
                      unsigned long height)
{
  ply_rectangle_t rectangle;
  rectangle.x = x;
  rectangle.y = y;
  rectangle.width = width;
  rectangle.height = height;
  ply_region_add_rectangle (region, &rectangle);
}

void
script_lib_sprite_refresh (script_lib_sprite_data_t *data)
{
  ply_list_node_t *node;
  ply_region_t *region = ply_region_new ();
  ply_list_t *rectable_list;

  ply_list_sort_stable (data->sprite_list, &sprite_compare_z);

  node = ply_list_get_first_node (data->sprite_list);


  if (data->full_refresh)
    {
      for (node = ply_list_get_first_node (data->displays);
           node;
           node = ply_list_get_next_node (data->displays, node))
        {
          script_lib_display_t* display = ply_list_node_get_data (node);
          region_add_area (region,
                           display->x,
                           display->y,
                           ply_pixel_display_get_width (display->pixel_display),
                           ply_pixel_display_get_height (display->pixel_display));
        }

      data->full_refresh = false;
    }

  while (node)
    {
      sprite_t *sprite = ply_list_node_get_data (node);
      ply_list_node_t *next_node = ply_list_get_next_node (data->sprite_list,
                                                           node);
      if (sprite->remove_me)
        {
          if (sprite->image)
            region_add_area (region,
                             sprite->old_x,
                             sprite->old_y,
                             sprite->old_width,
                             sprite->old_height);
          ply_list_remove_node (data->sprite_list, node);
          script_obj_unref (sprite->image_obj);
          free (sprite);
        }
      node = next_node;
    }

  for (node = ply_list_get_first_node (data->sprite_list);
       node;
       node = ply_list_get_next_node (data->sprite_list, node))
    {
      sprite_t *sprite = ply_list_node_get_data (node);
      if (!sprite->image) continue;
      if ((sprite->x != sprite->old_x)
          || (sprite->y != sprite->old_y)
          || (sprite->z != sprite->old_z)
          || (fabs (sprite->old_opacity - sprite->opacity) > 0.01)      /* People can't see the difference between */
          || sprite->refresh_me)
        {
          ply_rectangle_t size;
          ply_pixel_buffer_get_size (sprite->image, &size);
          region_add_area (region,
                           sprite->x,
                           sprite->y,
                           size.width,
                           size.height);
          region_add_area (region,
                           sprite->old_x,
                           sprite->old_y,
                           sprite->old_width,
                           sprite->old_height);

          sprite->old_x = sprite->x;
          sprite->old_y = sprite->y;
          sprite->old_z = sprite->z;
          sprite->old_width = size.width;
          sprite->old_height = size.height;
          sprite->old_opacity = sprite->opacity;
          sprite->refresh_me = false;
        }
    }

  rectable_list = ply_region_get_rectangle_list (region);

  for (node = ply_list_get_first_node (rectable_list);
       node;
       node = ply_list_get_next_node (rectable_list, node))
    {
      ply_rectangle_t *rectangle = ply_list_node_get_data (node);
      draw_area (data,
                 rectangle->x,
                 rectangle->y,
                 rectangle->width,
                 rectangle->height);
    }

  ply_region_free (region);
}

void script_lib_sprite_destroy (script_lib_sprite_data_t *data)
{
  ply_list_node_t *node;

  for (node = ply_list_get_first_node (data->displays);
       node;
       node = ply_list_get_next_node (data->displays, node))
    {
      script_lib_display_t* display = ply_list_node_get_data (node);
      ply_pixel_display_set_draw_handler (display->pixel_display, NULL, NULL);
    }

  node = ply_list_get_first_node (data->sprite_list);

  while (node)
    {
      sprite_t *sprite = ply_list_node_get_data (node);
      ply_list_node_t *next_node = ply_list_get_next_node (data->sprite_list,
                                                           node);
      ply_list_remove_node (data->sprite_list, node);
      script_obj_unref (sprite->image_obj);
      free (sprite);
      node = next_node;
    }

  ply_list_free (data->sprite_list);
  script_parse_op_free (data->script_main_op);
  script_obj_native_class_destroy (data->class);
  free (data);
}

