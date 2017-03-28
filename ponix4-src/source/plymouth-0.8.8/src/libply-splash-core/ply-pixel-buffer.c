/* ply-pixel-buffer.c - pixelbuffer abstraction
 *
 * Copyright (C) 2006, 2007, 2008, 2009 Red Hat, Inc.
 *               2008 Charlie Brej <cbrej@cs.man.ac.uk>
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
 *             Kristian Høgsberg <krh@redhat.com>
 *             Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-list.h"
#include "ply-pixel-buffer.h"
#include "ply-logger.h"

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct _ply_pixel_buffer
{
  uint32_t *bytes;

  ply_rectangle_t area;
  ply_list_t *clip_areas;

  ply_region_t *updated_areas;
};

static inline void ply_pixel_buffer_blend_value_at_pixel (ply_pixel_buffer_t *buffer,
                                                          int                 x,
                                                          int                 y,
                                                          uint32_t            pixel_value);

static void ply_pixel_buffer_fill_area_with_pixel_value (ply_pixel_buffer_t *buffer,
                                                         ply_rectangle_t    *fill_area,
                                                         uint32_t            pixel_value);

__attribute__((__pure__))
static inline uint32_t
blend_two_pixel_values (uint32_t pixel_value_1,
                        uint32_t pixel_value_2)
{
  if ((pixel_value_2 & 0xff000000) == 0xff000000)
    {
      uint8_t alpha_1, red_1, green_1, blue_1;
      uint8_t red_2, green_2, blue_2;
      uint_least32_t red, green, blue;

      alpha_1 = (uint8_t) (pixel_value_1 >> 24);
      red_1 = (uint8_t) (pixel_value_1 >> 16);
      green_1 = (uint8_t) (pixel_value_1 >> 8);
      blue_1 = (uint8_t) pixel_value_1;

      red_2 = (uint8_t) (pixel_value_2 >> 16);
      green_2 = (uint8_t) (pixel_value_2 >> 8);
      blue_2 = (uint8_t) pixel_value_2;

      red = red_1 * 255 + red_2 * (255 - alpha_1);
      green = green_1 * 255 + green_2 * (255 - alpha_1);
      blue = blue_1 * 255 + blue_2 * (255 - alpha_1);

      red = (uint8_t) ((red + (red >> 8) + 0x80) >> 8);
      green = (uint8_t) ((green + (green >> 8) + 0x80) >> 8);
      blue = (uint8_t) ((blue + (blue >> 8) + 0x80) >> 8);

      return 0xff000000 | (red << 16) | (green << 8) | blue;
    }
  else
    {
      uint8_t alpha_1, red_1, green_1, blue_1;
      uint8_t alpha_2, red_2, green_2, blue_2;
      uint_least32_t alpha, red, green, blue;

      alpha_1 = (uint8_t) (pixel_value_1 >> 24);
      red_1 = (uint8_t) (pixel_value_1 >> 16);
      green_1 = (uint8_t) (pixel_value_1 >> 8);
      blue_1 = (uint8_t) pixel_value_1;

      alpha_2 = (uint8_t) (pixel_value_2 >> 24);
      red_2 = (uint8_t) (pixel_value_2 >> 16);
      green_2 = (uint8_t) (pixel_value_2 >> 8);
      blue_2 = (uint8_t) pixel_value_2;

      red = red_1 * alpha_1 + red_2 * alpha_2 * (255 - alpha_1);
      green = green_1 * alpha_1 + green_2 * alpha_2 * (255 - alpha_1);
      blue = blue_1 * alpha_1 + blue_2 * alpha_2 * (255 - alpha_1);
      alpha = alpha_1 * 255 + alpha_2 * (255 - alpha_1);

      red = (red + (red >> 8) + 0x80) >> 8;
      red = MIN (red, 0xff);

      green = (green + (green >> 8) + 0x80) >> 8;
      green = MIN (green, 0xff);

      blue = (blue + (blue >> 8) + 0x80) >> 8;
      blue = MIN (blue, 0xff);

      alpha = (alpha + (alpha >> 8) + 0x80) >> 8;
      alpha = MIN (alpha, 0xff);

      return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }
}

__attribute__((__pure__))
static inline uint32_t
make_pixel_value_translucent (uint32_t pixel_value,
                              uint8_t  opacity)
{
  uint_least16_t alpha, red, green, blue;

  if (opacity == 255)
    return pixel_value;

  alpha = (uint8_t) (pixel_value >> 24);
  red = (uint8_t) (pixel_value >> 16);
  green = (uint8_t) (pixel_value >> 8);
  blue = (uint8_t) pixel_value;

  red *= opacity;
  green *= opacity;
  blue *= opacity;
  alpha *= opacity;

  red = (uint8_t) ((red + (red >> 8) + 0x80) >> 8);
  green = (uint8_t) ((green + (green >> 8) + 0x80) >> 8);
  blue = (uint8_t) ((blue + (blue >> 8) + 0x80) >> 8);
  alpha = (uint8_t) ((alpha + (alpha >> 8) + 0x80) >> 8);

  return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

static inline void
ply_pixel_buffer_blend_value_at_pixel (ply_pixel_buffer_t *buffer,
                                       int                 x,
                                       int                 y,
                                       uint32_t            pixel_value)
{
  uint32_t old_pixel_value;

  if ((pixel_value >> 24) != 0xff)
    {
      old_pixel_value = buffer->bytes[y * buffer->area.width + x];

      pixel_value = blend_two_pixel_values (pixel_value, old_pixel_value);
    }

  buffer->bytes[y * buffer->area.width + x] = pixel_value;
}

static void
ply_pixel_buffer_crop_area_to_clip_area (ply_pixel_buffer_t *buffer,
                                         ply_rectangle_t    *area,
                                         ply_rectangle_t    *cropped_area)
{
  ply_list_node_t *node;

  *cropped_area = *area;

  node = ply_list_get_first_node (buffer->clip_areas);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_rectangle_t *clip_rectangle;

      clip_rectangle = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (buffer->clip_areas, node);

      ply_rectangle_intersect (cropped_area, clip_rectangle, cropped_area);

      node = next_node;
    }
}

static void
ply_pixel_buffer_fill_area_with_pixel_value (ply_pixel_buffer_t      *buffer,
                                             ply_rectangle_t         *fill_area,
                                             uint32_t                 pixel_value)
{
  unsigned long row, column;
  ply_rectangle_t cropped_area;

  ply_pixel_buffer_crop_area_to_clip_area (buffer, fill_area, &cropped_area);

  for (row = cropped_area.y; row < cropped_area.y + cropped_area.height; row++)
    {
      for (column = cropped_area.x; column < cropped_area.x + cropped_area.width; column++)
        {
          ply_pixel_buffer_blend_value_at_pixel (buffer,
                                                 column, row,
                                                 pixel_value);
        }
    }
}

void
ply_pixel_buffer_push_clip_area (ply_pixel_buffer_t *buffer,
                                 ply_rectangle_t    *clip_area)
{
  ply_rectangle_t *new_clip_area;

  new_clip_area = malloc (sizeof (*new_clip_area));

  *new_clip_area = *clip_area;
  ply_list_append_data (buffer->clip_areas, new_clip_area);
}

void
ply_pixel_buffer_pop_clip_area (ply_pixel_buffer_t *buffer)
{
  ply_list_node_t *last_node;

  last_node = ply_list_get_last_node (buffer->clip_areas);
  free (ply_list_node_get_data (last_node));
  ply_list_remove_node (buffer->clip_areas, last_node);
}

ply_pixel_buffer_t *
ply_pixel_buffer_new (unsigned long width,
                      unsigned long height)
{
  ply_pixel_buffer_t *buffer;

  buffer = calloc (1, sizeof (ply_pixel_buffer_t));

  buffer->updated_areas = ply_region_new ();
  buffer->bytes = (uint32_t *) calloc (height, width * sizeof (uint32_t));
  buffer->area.width = width;
  buffer->area.height = height;

  buffer->clip_areas = ply_list_new ();
  ply_pixel_buffer_push_clip_area (buffer, &buffer->area);

  return buffer;
}

static void
free_clip_areas (ply_pixel_buffer_t *buffer)
{
  while (ply_list_get_length (buffer->clip_areas) > 0)
    ply_pixel_buffer_pop_clip_area (buffer);

  ply_list_free (buffer->clip_areas);
  buffer->clip_areas = NULL;
}

void
ply_pixel_buffer_free (ply_pixel_buffer_t *buffer)
{
  if (buffer == NULL)
    return;

  free_clip_areas (buffer);
  free (buffer->bytes);
  ply_region_free (buffer->updated_areas);
  free (buffer);
}

void
ply_pixel_buffer_get_size (ply_pixel_buffer_t *buffer,
                           ply_rectangle_t    *size)
{
  assert (buffer != NULL);
  assert (size != NULL);

  *size = buffer->area;
}

unsigned long 
ply_pixel_buffer_get_width (ply_pixel_buffer_t *buffer)
{
  assert (buffer != NULL);
  return buffer->area.width;
}

unsigned long 
ply_pixel_buffer_get_height (ply_pixel_buffer_t *buffer)
{
  assert (buffer != NULL);
  return buffer->area.height;
}

ply_region_t *
ply_pixel_buffer_get_updated_areas (ply_pixel_buffer_t *buffer)
{
  return buffer->updated_areas;
}

void
ply_pixel_buffer_fill_with_gradient (ply_pixel_buffer_t *buffer,
                                     ply_rectangle_t    *fill_area,
                                     uint32_t            start,
                                     uint32_t            end)
{
/* The gradient produced is a linear interpolation of the two passed
 * in color stops: start and end.
 *
 * In order to prevent banding when the color stops are too close
 * together, or are stretched over too large an area, we slightly
 * perturb the intermediate colors as we generate them.
 *
 * Before we do this, we store the interpolated color values in a
 * fixed point number with lots of fractional bits.  This is so
 * we don't add noise after the values have been clamped to 8-bits
 *
 * We add random noise to all of the fractional bits of each color
 * channel and also NOISE_BITS worth of noise to the non-fractional
 * part of the color. By default NOISE_BITS is 1.
 *
 * We incorporate the noise by filling the bottom 24 bits of an
 * integer with random bits and then shifting the color channels
 * to the left such that the top 8 bits of the channel overlap
 * the noise by NOISE_BITS. E.g., if NOISE_BITS is 1, then the top
 * 7 bits of each channel won't overlap with the noise, and the 8th
 * bit + fractional bits will.  When the noise and color channel
 * are properly aligned, we add them together, drop the precision
 * of the resulting channels back to 8 bits and stuff the results
 * into a pixel in the pixel buffer.
 */
#define NOISE_BITS 1
/* In the color stops, red is 8 bits starting at position 24
 * (since they're argb32 pixels).
 * We want to move those 8 bits such that the bottom NOISE_BITS
 * of them overlap the top of the 24 bits of generated noise.
 * Of course, green and blue are 8 bits away from red and each
 * other, respectively.
 */
#define RED_SHIFT (32 - (24 + NOISE_BITS))
#define GREEN_SHIFT (RED_SHIFT + 8)
#define BLUE_SHIFT (GREEN_SHIFT + 8)
#define NOISE_MASK (0x00ffffff)

/* Once, we've lined up the color channel we're interested in with
 * the noise, we need to mask out the other channels.
 */
#define COLOR_MASK (0xff << (24 - NOISE_BITS))

  uint32_t red, green, blue, red_step, green_step, blue_step, t, pixel;
  uint32_t x, y;
  /* we use a fixed seed so that the dithering doesn't change on repaints
   * of the same area.
   */
  uint32_t noise = 0x100001;
  ply_rectangle_t cropped_area;

  if (fill_area == NULL)
    fill_area = &buffer->area;

  ply_pixel_buffer_crop_area_to_clip_area (buffer, fill_area, &cropped_area);

  red   = (start << RED_SHIFT) & COLOR_MASK;
  green = (start << GREEN_SHIFT) & COLOR_MASK;
  blue  = (start << BLUE_SHIFT) & COLOR_MASK;

  t = (end << RED_SHIFT) & COLOR_MASK;
  red_step = (int32_t) (t - red) / (int32_t) buffer->area.height;
  t = (end << GREEN_SHIFT) & COLOR_MASK;
  green_step = (int32_t) (t - green) / (int32_t) buffer->area.height;
  t = (end << BLUE_SHIFT) & COLOR_MASK;
  blue_step = (int32_t) (t - blue) / (int32_t) buffer->area.height;


#define RANDOMIZE(num) (num = (num + (num << 1)) & NOISE_MASK)
#define UNROLLED_PIXEL_COUNT 8

  for (y = buffer->area.y; y < buffer->area.y + buffer->area.height; y++)
    {
      if (cropped_area.y <= y && y < cropped_area.y + cropped_area.height)
        {
          if (cropped_area.width < UNROLLED_PIXEL_COUNT)
            {
              for (x = cropped_area.x; x < cropped_area.x + cropped_area.width; x++)
                {
                  pixel = 0xff000000;
                  RANDOMIZE(noise);
                  pixel |= (((red   + noise) & COLOR_MASK) >> RED_SHIFT);
                  RANDOMIZE(noise);
                  pixel |= (((green + noise) & COLOR_MASK) >> GREEN_SHIFT);
                  RANDOMIZE(noise);
                  pixel |= (((blue  + noise) & COLOR_MASK) >> BLUE_SHIFT);

                  buffer->bytes[y * buffer->area.width + x] = pixel;
                }
            }
          else
            {
              uint32_t shaded_set[UNROLLED_PIXEL_COUNT];
              uint32_t *ptr = &buffer->bytes[y * buffer->area.width + cropped_area.x];
              for (x = 0; x < UNROLLED_PIXEL_COUNT; x++)
                {
                  shaded_set[x] = 0xff000000;
                  RANDOMIZE(noise);
                  shaded_set[x] |= (((red   + noise) & COLOR_MASK) >> RED_SHIFT);
                  RANDOMIZE(noise);
                  shaded_set[x] |= (((green + noise) & COLOR_MASK) >> GREEN_SHIFT);
                  RANDOMIZE(noise);
                  shaded_set[x] |= (((blue  + noise) & COLOR_MASK) >> BLUE_SHIFT);
                }
              for (x = cropped_area.width; x >=UNROLLED_PIXEL_COUNT; x-= UNROLLED_PIXEL_COUNT)
                {
                  memcpy (ptr, (void *) shaded_set, UNROLLED_PIXEL_COUNT * sizeof (uint32_t));
                  ptr += UNROLLED_PIXEL_COUNT;
                }

              memcpy (ptr, (void *) shaded_set, x * sizeof (uint32_t));
            }
        }

      red += red_step;
      green += green_step;
      blue += blue_step;
    }

  ply_region_add_rectangle (buffer->updated_areas, &cropped_area);
}

void
ply_pixel_buffer_fill_with_color (ply_pixel_buffer_t *buffer,
                                  ply_rectangle_t    *fill_area,
                                  double              red,
                                  double              green,
                                  double              blue,
                                  double              alpha)
{
  uint32_t pixel_value;
  ply_rectangle_t cropped_area;

  assert (buffer != NULL);

  if (fill_area == NULL)
    fill_area = &buffer->area;

  ply_pixel_buffer_crop_area_to_clip_area (buffer, fill_area, &cropped_area);

  red *= alpha;
  green *= alpha;
  blue *= alpha;

  pixel_value = PLY_PIXEL_BUFFER_COLOR_TO_PIXEL_VALUE (red, green, blue, alpha);

  ply_pixel_buffer_fill_area_with_pixel_value (buffer, &cropped_area, pixel_value);

  ply_region_add_rectangle (buffer->updated_areas, &cropped_area);
}

void
ply_pixel_buffer_fill_with_hex_color_at_opacity (ply_pixel_buffer_t *buffer,
                                                 ply_rectangle_t    *fill_area,
                                                 uint32_t            hex_color,
                                                 double              opacity)
{
  ply_rectangle_t cropped_area;
  uint32_t pixel_value;
  double red;
  double green;
  double blue;
  double alpha;

  assert (buffer != NULL);

  if (fill_area == NULL)
    fill_area = &buffer->area;

  ply_pixel_buffer_crop_area_to_clip_area (buffer, fill_area, &cropped_area);

  /* if they only gave an rgb hex number, assume an alpha of 0xff
   */
  if ((hex_color & 0xff000000) == 0)
    hex_color = (hex_color << 8) | 0xff;

  red = ((double) (hex_color & 0xff000000) / 0xff000000);
  green = ((double) (hex_color & 0x00ff0000) / 0x00ff0000);
  blue = ((double) (hex_color & 0x0000ff00) / 0x0000ff00);
  alpha = ((double) (hex_color & 0x000000ff) / 0x000000ff);

  alpha *= opacity;

  red *= alpha;
  green *= alpha;
  blue *= alpha;

  pixel_value = PLY_PIXEL_BUFFER_COLOR_TO_PIXEL_VALUE (red, green, blue, alpha);

  ply_pixel_buffer_fill_area_with_pixel_value (buffer, &cropped_area, pixel_value);

  ply_region_add_rectangle (buffer->updated_areas, &cropped_area);
}

void
ply_pixel_buffer_fill_with_hex_color (ply_pixel_buffer_t *buffer,
                                      ply_rectangle_t    *fill_area,
                                      uint32_t            hex_color)
{
  return ply_pixel_buffer_fill_with_hex_color_at_opacity (buffer, fill_area,
                                                          hex_color, 1.0);
}

void
ply_pixel_buffer_fill_with_argb32_data_at_opacity_with_clip (ply_pixel_buffer_t *buffer,
                                                             ply_rectangle_t    *fill_area,
                                                             ply_rectangle_t    *clip_area,
                                                             uint32_t           *data,
                                                             double              opacity)
{
  unsigned long row, column;
  uint8_t opacity_as_byte;
  ply_rectangle_t cropped_area;
  unsigned long x;
  unsigned long y;

  assert (buffer != NULL);

  if (fill_area == NULL)
    fill_area = &buffer->area;

  ply_pixel_buffer_crop_area_to_clip_area (buffer, fill_area, &cropped_area);

  if (clip_area)
    ply_rectangle_intersect (&cropped_area, clip_area, &cropped_area);

  if (cropped_area.width == 0 || cropped_area.height == 0)
    return;

  x = cropped_area.x - fill_area->x;
  y = cropped_area.y - fill_area->y;
  opacity_as_byte = (uint8_t) (opacity * 255.0);

  for (row = y; row < y + cropped_area.height; row++)
    {
      for (column = x; column < x + cropped_area.width; column++)
        {
          uint32_t pixel_value;

          pixel_value = data[fill_area->width * row + column];
          if ((pixel_value >> 24) == 0x00)
            continue;

          pixel_value = make_pixel_value_translucent (pixel_value, opacity_as_byte);
          ply_pixel_buffer_blend_value_at_pixel (buffer,
                                                 cropped_area.x + (column - x),
                                                 cropped_area.y + (row - y),
                                                 pixel_value);

        }
    }

  ply_region_add_rectangle (buffer->updated_areas, &cropped_area);
}

void
ply_pixel_buffer_fill_with_argb32_data_at_opacity (ply_pixel_buffer_t *buffer,
                                                   ply_rectangle_t    *fill_area,
                                                   uint32_t           *data,
                                                   double              opacity)
{
  ply_pixel_buffer_fill_with_argb32_data_at_opacity_with_clip (buffer,
                                                               fill_area,
                                                               NULL,
                                                               data, opacity);
}

void
ply_pixel_buffer_fill_with_argb32_data (ply_pixel_buffer_t *buffer,
                                        ply_rectangle_t    *fill_area,
                                        uint32_t           *data)
{
  ply_pixel_buffer_fill_with_argb32_data_at_opacity_with_clip (buffer,
                                                               fill_area,
                                                               NULL,
                                                               data, 1.0);
}

void
ply_pixel_buffer_fill_with_argb32_data_with_clip (ply_pixel_buffer_t *buffer,
                                                  ply_rectangle_t    *fill_area,
                                                  ply_rectangle_t    *clip_area,
                                                  uint32_t           *data)
{
  ply_pixel_buffer_fill_with_argb32_data_at_opacity_with_clip (buffer,
                                                               fill_area,
                                                               clip_area,
                                                               data, 1.0);
}

void
ply_pixel_buffer_fill_with_buffer_at_opacity_with_clip (ply_pixel_buffer_t *canvas,
                                                        ply_pixel_buffer_t *source,
                                                        int                 x_offset,
                                                        int                 y_offset,
                                                        ply_rectangle_t    *clip_area,
                                                        float               opacity)
{
  unsigned long row, column;
  uint8_t opacity_as_byte;
  ply_rectangle_t cropped_area;
  unsigned long x;
  unsigned long y;

  assert (canvas != NULL);
  assert (source != NULL);

  cropped_area.x = x_offset;
  cropped_area.y = y_offset;
  cropped_area.width = source->area.width;
  cropped_area.height = source->area.height;

  ply_pixel_buffer_crop_area_to_clip_area (canvas, &cropped_area, &cropped_area);

  if (clip_area)
    ply_rectangle_intersect (&cropped_area, clip_area, &cropped_area);

  if (cropped_area.width == 0 || cropped_area.height == 0)
    return;

  x = cropped_area.x - x_offset;
  y = cropped_area.y - y_offset;
  opacity_as_byte = (uint8_t) (opacity * 255.0);

  for (row = y; row < y + cropped_area.height; row++)
    {
      for (column = x; column < x + cropped_area.width; column++)
        {
          uint32_t pixel_value;

          pixel_value = source->bytes[row * source->area.width + column];

          pixel_value = make_pixel_value_translucent (pixel_value, opacity_as_byte);
          
          if ((pixel_value >> 24) == 0x00)
            continue;

          ply_pixel_buffer_blend_value_at_pixel (canvas,
                                                 cropped_area.x + (column - x),
                                                 cropped_area.y + (row - y),
                                                 pixel_value);

        }
    }

  ply_region_add_rectangle (canvas->updated_areas, &cropped_area);
}

void
ply_pixel_buffer_fill_with_buffer_at_opacity (ply_pixel_buffer_t *canvas,
                                              ply_pixel_buffer_t *source,
                                              int                 x_offset,
                                              int                 y_offset,
                                              float               opacity)
{
  ply_pixel_buffer_fill_with_buffer_at_opacity_with_clip (canvas,
                                                          source,
                                                          x_offset,
                                                          y_offset,
                                                          NULL,
                                                          opacity);
}

void
ply_pixel_buffer_fill_with_buffer_with_clip (ply_pixel_buffer_t *canvas,
                                             ply_pixel_buffer_t *source,
                                             int                 x_offset,
                                             int                 y_offset,
                                             ply_rectangle_t    *clip_area)
{
  ply_pixel_buffer_fill_with_buffer_at_opacity_with_clip (canvas,
                                                          source,
                                                          x_offset,
                                                          y_offset,
                                                          clip_area,
                                                          1.0);
}

void
ply_pixel_buffer_fill_with_buffer (ply_pixel_buffer_t *canvas,
                                   ply_pixel_buffer_t *source,
                                   int                 x_offset,
                                   int                 y_offset)
{
  ply_pixel_buffer_fill_with_buffer_at_opacity_with_clip (canvas,
                                                          source,
                                                          x_offset,
                                                          y_offset,
                                                          NULL,
                                                          1.0);
}

uint32_t *
ply_pixel_buffer_get_argb32_data (ply_pixel_buffer_t *buffer)
{
  return buffer->bytes;
}

static inline uint32_t
ply_pixel_buffer_interpolate (ply_pixel_buffer_t *buffer,
                              double              x,
                              double              y)
{
  int ix;
  int iy;
  int width;
  int height;
  
  int i;
  
  int offset_x;
  int offset_y;
  uint32_t pixels[2][2];
  uint32_t reply = 0;
  uint32_t *bytes;
  
  width = buffer->area.width;
  height = buffer->area.height;

  
  bytes = ply_pixel_buffer_get_argb32_data (buffer);
  
  for (offset_y = 0; offset_y < 2; offset_y++)
  for (offset_x = 0; offset_x < 2; offset_x++)
    {
      ix = x + offset_x;
      iy = y + offset_y;
      
      if (ix < 0 || ix >= width || iy < 0 || iy >= height)
        pixels[offset_y][offset_x] = 0x00000000;
      else
        pixels[offset_y][offset_x] = bytes[ix + iy * width];
    }
  if (!pixels[0][0] && !pixels[0][1] && !pixels[1][0] && !pixels[1][1]) return 0;
  
  ix = x;
  iy = y;
  x -= ix;
  y -= iy;
  for (i = 0; i < 4; i++)
    {
      uint32_t value = 0;
      uint32_t mask = 0xFF << (i * 8);
      value += ((pixels[0][0]) & mask) * (1-x) * (1-y);
      value += ((pixels[0][1]) & mask) * x * (1-y);
      value += ((pixels[1][0]) & mask) * (1-x) * y;
      value += ((pixels[1][1]) & mask) * x * y;
      reply |= value & mask;
    }
  return reply;
}

ply_pixel_buffer_t *
ply_pixel_buffer_resize (ply_pixel_buffer_t *old_buffer,
                         long                width,
                         long                height)
{
  ply_pixel_buffer_t *buffer;
  int x, y;
  double old_x, old_y;
  int old_width, old_height;
  float scale_x, scale_y;
  uint32_t *bytes;
  
  buffer = ply_pixel_buffer_new (width, height);

  bytes = ply_pixel_buffer_get_argb32_data (buffer);

  old_width = old_buffer->area.width;
  old_height = old_buffer->area.height;

  scale_x = ((double) old_width - 1) / MAX (width - 1, 1);
  scale_y = ((double) old_height - 1) / MAX (height - 1, 1);

  for (y = 0; y < height; y++)
    {
      old_y = y * scale_y;
      for (x=0; x < width; x++)
        {
          old_x = x * scale_x;
          bytes[x + y * width] =
                    ply_pixel_buffer_interpolate (old_buffer, old_x, old_y);
        }
    }
  return buffer;
}

ply_pixel_buffer_t *
ply_pixel_buffer_rotate (ply_pixel_buffer_t *old_buffer,
                         long                center_x,
                         long                center_y,
                         double              theta_offset)
{
  ply_pixel_buffer_t *buffer;
  int x, y;
  double old_x, old_y;
  int width;
  int height;
  uint32_t *bytes;

  width = old_buffer->area.width;
  height = old_buffer->area.height;

  buffer = ply_pixel_buffer_new (width, height);

  bytes = ply_pixel_buffer_get_argb32_data (buffer);

  double d = sqrt ((center_x * center_x +
                    center_y * center_y));
  double theta = atan2 (-center_y, -center_x) - theta_offset;
  double start_x = center_x + d * cos (theta);
  double start_y = center_y + d * sin (theta);
  double step_x = cos (-theta_offset);
  double step_y = sin (-theta_offset);
  
  for (y = 0; y < height; y++)
    {
      old_y = start_y;
      old_x = start_x;
      start_y += step_x;
      start_x -= step_y;
      for (x = 0; x < width; x++)
        {
          if (old_x < 0 || old_x > width || old_y < 0 || old_y > height)
            bytes[x + y * width] = 0;
          else
            bytes[x + y * width] =
                    ply_pixel_buffer_interpolate (old_buffer, old_x, old_y);
          old_x += step_x;
          old_y += step_y;
        }
    }
  return buffer;
}


/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
