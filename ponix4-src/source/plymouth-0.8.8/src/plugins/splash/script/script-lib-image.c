/* script-lib-image.c - scripting system ply-image wrapper
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
#define _GNU_SOURCE
#include "ply-image.h"
#include "ply-label.h"
#include "ply-pixel-buffer.h"
#include "ply-utils.h"
#include "ply-logger.h"
#include "script.h"
#include "script-parse.h"
#include "script-object.h"
#include "script-parse.h"
#include "script-execute.h"
#include "script-lib-image.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "script-lib-image.script.h"

static void image_free (script_obj_t *obj)
{
  ply_pixel_buffer_t *image = obj->data.native.object_data;

  ply_pixel_buffer_free (image);
}

static script_return_t image_new (script_state_t *state,
                                  void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  script_obj_t *reply;
  char *path_filename;
  char *filename = script_obj_hash_get_string (state->local, "filename");
  char *test_string = filename;
  const char *prefix_string = "special://";

  while (*test_string && *prefix_string && *test_string == *prefix_string)
    {
      test_string++;
      prefix_string++;
    }
  if (!*prefix_string)
    {
      if (strcmp (test_string, "logo") == 0)
        path_filename = strdup (PLYMOUTH_LOGO_FILE);
      else
        path_filename = strdup ("");
    }
  else
    asprintf (&path_filename, "%s/%s", data->image_dir, filename);
  ply_image_t *file_image = ply_image_new (path_filename);
  if (ply_image_load (file_image))
    {
      ply_pixel_buffer_t *buffer = ply_image_convert_to_pixel_buffer (file_image);
      reply = script_obj_new_native (buffer, data->class);
    }
  else
    {
      ply_image_free (file_image);
      reply = script_obj_new_null ();
    }
  free (filename);
  free (path_filename);
  return script_return_obj (reply);
}

static script_return_t image_get_width (script_state_t *state,
                                        void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  ply_rectangle_t size;
  ply_pixel_buffer_t *image;
  
  image = script_obj_as_native_of_class (state->this, data->class);
  if (!image) return script_return_obj_null ();
  
  ply_pixel_buffer_get_size (image, &size);
  
  return script_return_obj (script_obj_new_number (size.width));
}

static script_return_t image_get_height (script_state_t *state,
                                         void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  ply_rectangle_t size;
  ply_pixel_buffer_t *image;
  
  image = script_obj_as_native_of_class (state->this, data->class);
  if (!image) return script_return_obj_null ();
  
  ply_pixel_buffer_get_size (image, &size);
  
  return script_return_obj (script_obj_new_number (size.height));
}

static script_return_t image_rotate (script_state_t *state,
                                     void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  ply_pixel_buffer_t *image = script_obj_as_native_of_class (state->this, data->class);
  float angle = script_obj_hash_get_number (state->local, "angle");
  ply_rectangle_t size;
  
  if (image)
    {
      ply_pixel_buffer_get_size (image, &size);
      ply_pixel_buffer_t *new_image = ply_pixel_buffer_rotate (image,
                                                               size.width / 2,
                                                               size.height / 2,
                                                               angle);
      return script_return_obj (script_obj_new_native (new_image, data->class));
    }
  return script_return_obj_null ();
}

static script_return_t image_scale (script_state_t *state,
                                    void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  ply_pixel_buffer_t *image = script_obj_as_native_of_class (state->this, data->class);
  int width = script_obj_hash_get_number (state->local, "width");
  int height = script_obj_hash_get_number (state->local, "height");

  if (image)
    {
      ply_pixel_buffer_t *new_image = ply_pixel_buffer_resize (image, width, height);
      return script_return_obj (script_obj_new_native (new_image, data->class));
    }
  return script_return_obj_null ();
}

static script_return_t image_text (script_state_t *state,
                                   void           *user_data)
{
  script_lib_image_data_t *data = user_data;
  ply_pixel_buffer_t *image;
  ply_label_t *label;
  script_obj_t *alpha_obj, *font_obj, *align_obj;
  int width, height;
  int align = PLY_LABEL_ALIGN_LEFT;
  char *font;
  
  char *text = script_obj_hash_get_string (state->local, "text");
  
  float alpha;
  float red = CLAMP(script_obj_hash_get_number (state->local, "red"), 0, 1);
  float green = CLAMP(script_obj_hash_get_number (state->local, "green"), 0, 1);
  float blue = CLAMP(script_obj_hash_get_number (state->local, "blue"), 0, 1);

  alpha_obj = script_obj_hash_peek_element (state->local, "alpha");

  if (script_obj_is_number (alpha_obj))
    {
      alpha = CLAMP(script_obj_as_number (alpha_obj), 0, 1);
    }
  else
    alpha = 1;
  script_obj_unref(alpha_obj);

  font_obj = script_obj_hash_peek_element (state->local, "font");

  if (script_obj_is_string (font_obj))
    font = script_obj_as_string (font_obj);
  else
    font = NULL;

  script_obj_unref(font_obj);

  align_obj = script_obj_hash_peek_element(state->local, "align");

  if (script_obj_is_string(align_obj)) {
    char *align_str = script_obj_as_string(align_obj);

    if(!strcmp("left", align_str))
      align = PLY_LABEL_ALIGN_LEFT;
    else if(!strcmp("center", align_str))
      align = PLY_LABEL_ALIGN_CENTER;
    else if(!strcmp("right", align_str))
      align = PLY_LABEL_ALIGN_RIGHT;
    else
      ply_error("Unrecognized Image.Text alignment string '%s'. "
	      "Expecting 'left', 'center', or 'right'\n",
		align_str);
    free(align_str);
  }
  script_obj_unref(align_obj);

  if (!text) return script_return_obj_null ();

  label = ply_label_new ();
  ply_label_set_text (label, text);
  if (font)
    ply_label_set_font (label, font);
  ply_label_set_alignment(label, align);
  ply_label_set_color (label, red, green, blue, alpha);
  ply_label_show (label, NULL, 0, 0);
  
  width = ply_label_get_width (label);
  height = ply_label_get_height (label);
  
  image = ply_pixel_buffer_new (width, height);
  ply_label_draw_area (label, image, 0, 0, width, height);
  
  free (text);
  free (font);
  ply_label_free (label);
  
  return script_return_obj (script_obj_new_native (image, data->class));
}

script_lib_image_data_t *script_lib_image_setup (script_state_t *state,
                                                 char         *image_dir)
{
  script_lib_image_data_t *data = malloc (sizeof (script_lib_image_data_t));

  data->class = script_obj_native_class_new (image_free, "image", data);
  data->image_dir = strdup (image_dir);

  script_obj_t *image_hash = script_obj_hash_get_element (state->global, "Image");
  
  script_add_native_function (image_hash,
                              "_New",
                              image_new,
                              data,
                              "filename",
                              NULL);
  script_add_native_function (image_hash,
                              "_Rotate",
                              image_rotate,
                              data,
                              "angle",
                              NULL);
  script_add_native_function (image_hash,
                              "_Scale",
                              image_scale,
                              data,
                              "width",
                              "height",
                              NULL);
  script_add_native_function (image_hash,
                              "GetWidth",
                              image_get_width,
                              data,
                              NULL);
  script_add_native_function (image_hash,
                              "GetHeight",
                              image_get_height,
                              data,
                              NULL);
  script_add_native_function (image_hash,
                              "_Text",
                              image_text,
                              data,
                              "text",
                              "red",
                              "green",
                              "blue",
                              "alpha",
                              "font",
                              "align",
                              NULL);

  script_obj_unref (image_hash);
  data->script_main_op = script_parse_string (script_lib_image_string, "script-lib-image.script");
  script_return_t ret = script_execute (state, data->script_main_op);
  script_obj_unref (ret.object);
  return data;
}

void script_lib_image_destroy (script_lib_image_data_t *data)
{
  script_obj_native_class_destroy (data->class);
  free (data->image_dir);
  script_parse_op_free (data->script_main_op);
  free (data);
}

