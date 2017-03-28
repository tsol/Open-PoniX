/* ply-image.c - png file loader
 *
 * Copyright (C) 2006, 2007 Red Hat, Inc.
 * Copyright (C) 2003 University of Southern California
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
 * Some implementation taken from the cairo library.
 *
 * Written by: Charlie Brej <cbrej@cs.man.ac.uk>
 *             Kristian Høgsberg <krh@redhat.com>
 *             Ray Strode <rstrode@redhat.com>
 *             Carl D. Worth <cworth@cworth.org>
 */
#include "config.h"
#include "ply-image.h"
#include "ply-pixel-buffer.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

#include <png.h>

#include <linux/fb.h>

#include "ply-utils.h"

struct _ply_image
{
  char  *filename;
  ply_pixel_buffer_t *buffer;
};

ply_image_t *
ply_image_new (const char *filename)
{
  ply_image_t *image;

  assert (filename != NULL);

  image = calloc (1, sizeof (ply_image_t));

  image->filename = strdup (filename);
  image->buffer = NULL;

  return image;
}

void
ply_image_free (ply_image_t *image)
{
  if (image == NULL)
    return;

  assert (image->filename != NULL);
  
  ply_pixel_buffer_free (image->buffer);
  free (image->filename);
  free (image);
}

static void
transform_to_argb32 (png_struct   *png,
                     png_row_info *row_info,
                     png_byte     *data)
{
  unsigned int i;

  for (i = 0; i < row_info->rowbytes; i += 4) 
  {
    uint8_t  red, green, blue, alpha;
    uint32_t pixel_value;

    red = data[i + 0];
    green = data[i + 1];
    blue = data[i + 2];
    alpha = data[i + 3];

    red = (uint8_t) CLAMP (((red / 255.0) * (alpha / 255.0)) * 255.0, 0, 255.0);
    green = (uint8_t) CLAMP (((green / 255.0) * (alpha / 255.0)) * 255.0,
                             0, 255.0);
    blue = (uint8_t) CLAMP (((blue / 255.0) * (alpha / 255.0)) * 255.0, 0, 255.0);

    pixel_value = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
    memcpy (data + i, &pixel_value, sizeof (uint32_t));
  }
}

bool
ply_image_load (ply_image_t *image)
{
  png_struct *png;
  png_info *info;
  png_uint_32 width, height, row;
  int bits_per_pixel, color_type, interlace_method;
  png_byte **rows;
  uint32_t *bytes;
  FILE *fp;
  
  assert (image != NULL);
  
  fp = fopen (image->filename, "r");
  if (fp == NULL)
    return false;
  
  png = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  assert (png != NULL);

  info = png_create_info_struct (png);
  assert (info != NULL);

  png_init_io (png, fp);

  if (setjmp (png_jmpbuf (png)) != 0)
    {
      fclose (fp);
      return false;
    }

  png_read_info (png, info);
  png_get_IHDR (png, info,
                &width, &height, &bits_per_pixel,
                &color_type, &interlace_method, NULL, NULL);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb (png);

  if ((color_type == PNG_COLOR_TYPE_GRAY) && (bits_per_pixel < 8))
    png_set_expand_gray_1_2_4_to_8 (png);

  if (png_get_valid (png, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha (png);

  if (bits_per_pixel == 16)
    png_set_strip_16 (png);

  if (bits_per_pixel < 8)
    png_set_packing (png);

  if ((color_type == PNG_COLOR_TYPE_GRAY)
      || (color_type == PNG_COLOR_TYPE_GRAY_ALPHA))
    png_set_gray_to_rgb (png);

  if (interlace_method != PNG_INTERLACE_NONE)
    png_set_interlace_handling (png);

  png_set_filler (png, 0xff, PNG_FILLER_AFTER);

  png_set_read_user_transform_fn (png, transform_to_argb32);

  png_read_update_info (png, info);

  rows = malloc (height * sizeof (png_byte *));
  image->buffer = ply_pixel_buffer_new (width, height);
  
  bytes = ply_pixel_buffer_get_argb32_data (image->buffer);

  for (row = 0; row < height; row++)
    rows[row] = (png_byte*) &bytes[row * width];

  png_read_image (png, rows);

  free (rows);
  png_read_end (png, info);
  fclose (fp);
  png_destroy_read_struct (&png, &info, NULL);

  return true;
}

uint32_t *
ply_image_get_data (ply_image_t *image)
{
  assert (image != NULL);

  return ply_pixel_buffer_get_argb32_data (image->buffer);
}

long
ply_image_get_width (ply_image_t *image)
{
  ply_rectangle_t size;
  
  assert (image != NULL);
  ply_pixel_buffer_get_size (image->buffer, &size);

  return size.width;
}

long
ply_image_get_height (ply_image_t *image)
{
  ply_rectangle_t size;
  
  assert (image != NULL);
  ply_pixel_buffer_get_size (image->buffer, &size);

  return size.height;
}

ply_image_t *
ply_image_resize (ply_image_t *image,
                  long         width,
                  long         height)
{
  ply_image_t *new_image;
  
  new_image = ply_image_new (image->filename);

  new_image->buffer = ply_pixel_buffer_resize (image->buffer,
                                               width,
                                               height);
  return new_image;
}

ply_image_t *
ply_image_rotate (ply_image_t *image,
                  long         center_x,
                  long         center_y,
                  double       theta_offset)
{
  ply_image_t *new_image;
  
  new_image = ply_image_new (image->filename);
  
  new_image->buffer = ply_pixel_buffer_rotate (image->buffer,
                                               center_x,
                                               center_y,
                                               theta_offset);
  return new_image;
}

ply_pixel_buffer_t *
ply_image_get_buffer (ply_image_t *image)
{
  assert (image != NULL);

  return image->buffer;
}

ply_pixel_buffer_t *
ply_image_convert_to_pixel_buffer (ply_image_t *image)
{
  ply_pixel_buffer_t *buffer;

  assert (image != NULL);

  buffer = image->buffer;
  image->buffer = NULL;
  ply_image_free (image);
  
  return buffer;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
