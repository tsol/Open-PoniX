/* plugin.c - frame-backend renderer plugin
 *
 * Copyright (C) 2006-2009 Red Hat, Inc.
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
 *             Peter Jones <pjones@redhat.com>
 *             Ray Strode <rstrode@redhat.com>
 */
#include "config.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <values.h>
#include <unistd.h>

#include <linux/fb.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-rectangle.h"
#include "ply-region.h"
#include "ply-terminal.h"

#include "ply-renderer.h"
#include "ply-renderer-plugin.h"

#ifndef PLY_FRAME_BUFFER_DEFAULT_FB_DEVICE_NAME
#define PLY_FRAME_BUFFER_DEFAULT_FB_DEVICE_NAME "/dev/fb0"
#endif

struct _ply_renderer_head
{
  ply_pixel_buffer_t *pixel_buffer;
  ply_rectangle_t area;
  char *map_address;
  size_t size;

};

struct _ply_renderer_input_source
{
  ply_renderer_backend_t *backend;
  ply_fd_watch_t *terminal_input_watch;

  ply_buffer_t   *key_buffer;

  ply_renderer_input_source_handler_t handler;
  void           *user_data;
};

struct _ply_renderer_backend
{
  ply_event_loop_t *loop;
  ply_terminal_t *terminal;

  char *device_name;
  int   device_fd;

  ply_renderer_input_source_t input_source;
  ply_renderer_head_t head;
  ply_list_t *heads;

  uint32_t red_bit_position;
  uint32_t green_bit_position;
  uint32_t blue_bit_position;
  uint32_t alpha_bit_position;

  uint32_t bits_for_red;
  uint32_t bits_for_green;
  uint32_t bits_for_blue;
  uint32_t bits_for_alpha;

  int32_t dither_red;
  int32_t dither_green;
  int32_t dither_blue;

  unsigned int bytes_per_pixel;
  unsigned int row_stride;

  uint32_t is_active : 1;

  void (* flush_area) (ply_renderer_backend_t *backend,
                       ply_renderer_head_t    *head,
                       ply_rectangle_t        *area_to_flush);
};

ply_renderer_plugin_interface_t *ply_renderer_backend_get_interface (void);
static void ply_renderer_head_redraw (ply_renderer_backend_t *backend,
                                      ply_renderer_head_t    *head);
static bool open_input_source (ply_renderer_backend_t      *backend,
                               ply_renderer_input_source_t *input_source);

static inline uint_fast32_t
argb32_pixel_value_to_device_pixel_value (ply_renderer_backend_t *backend,
                                          uint32_t                pixel_value)
{
  uint8_t r, g, b, a;
  int orig_r, orig_g, orig_b, orig_a;
  uint8_t new_r, new_g, new_b;
  int i;

  orig_a = pixel_value >> 24;
  a = orig_a >> (8 - backend->bits_for_alpha);

  orig_r = ((pixel_value >> 16) & 0xff) - backend->dither_red;
  r = CLAMP (orig_r, 0, 255) >> (8 - backend->bits_for_red);

  orig_g = ((pixel_value >> 8) & 0xff) - backend->dither_green;
  g = CLAMP (orig_g, 0, 255) >> (8 - backend->bits_for_green);

  orig_b = (pixel_value & 0xff) - backend->dither_blue;
  b = CLAMP (orig_b, 0, 255) >> (8 - backend->bits_for_blue);

  new_r = r << (8 - backend->bits_for_red);
  new_g = g << (8 - backend->bits_for_green);
  new_b = b << (8 - backend->bits_for_blue);

  for (i = backend->bits_for_red; i < 8; i <<= 1)
    new_r |= new_r >> i;

  for (i = backend->bits_for_green; i < 8; i <<= 1)
    new_g |= new_g >> i;

  for (i = backend->bits_for_blue; i < 8; i <<= 1)
    new_b |= new_b >> i;

  backend->dither_red = new_r - orig_r;
  backend->dither_green = new_g - orig_g;
  backend->dither_blue = new_b - orig_b;

  return ((a << backend->alpha_bit_position)
          | (r << backend->red_bit_position)
          | (g << backend->green_bit_position)
          | (b << backend->blue_bit_position));
}

static void
flush_area_to_any_device (ply_renderer_backend_t *backend,
                          ply_renderer_head_t    *head,
                          ply_rectangle_t        *area_to_flush)
{
  unsigned long row, column;
  uint32_t *shadow_buffer;
  char *row_backend;
  unsigned long x1, y1, x2, y2;

  x1 = area_to_flush->x;
  y1 = area_to_flush->y;
  x2 = x1 + area_to_flush->width;
  y2 = y1 + area_to_flush->height;

  row_backend = malloc (backend->row_stride);
  shadow_buffer = ply_pixel_buffer_get_argb32_data (backend->head.pixel_buffer);
  for (row = y1; row < y2; row++)
    {
      unsigned long offset;

      for (column = x1; column < x2; column++)
        {
          uint32_t pixel_value;
          uint_fast32_t device_pixel_value;

          pixel_value = shadow_buffer[row * head->area.width + column];

          device_pixel_value = argb32_pixel_value_to_device_pixel_value (backend,
                                                                         pixel_value);

          memcpy (row_backend + column * backend->bytes_per_pixel,
                  &device_pixel_value, backend->bytes_per_pixel);
        }

      offset = row * backend->row_stride + x1 * backend->bytes_per_pixel;
      memcpy (head->map_address + offset, row_backend + x1 * backend->bytes_per_pixel,
              area_to_flush->width * backend->bytes_per_pixel);
    }
  free (row_backend);
}

static void
flush_area_to_xrgb32_device (ply_renderer_backend_t *backend,
                             ply_renderer_head_t    *head,
                             ply_rectangle_t        *area_to_flush)
{
  unsigned long x, y, y1, y2;
  uint32_t *shadow_buffer;
  char *dst, *src;

  x = area_to_flush->x;
  y1 = area_to_flush->y;
  y2 = y1 + area_to_flush->height;

  shadow_buffer = ply_pixel_buffer_get_argb32_data (backend->head.pixel_buffer);

  dst = &head->map_address[y1 * backend->row_stride + x * backend->bytes_per_pixel];
  src = (char *) &shadow_buffer[y1 * head->area.width + x];

  if (area_to_flush->width * 4 == backend->row_stride &&
      head->area.width * 4 == backend->row_stride)
    {
      memcpy (dst, src, area_to_flush->width * area_to_flush->height * 4);
      return;
    }

  for (y = y1; y < y2; y++)
    {
      memcpy (dst, src, area_to_flush->width * 4);
      dst += backend->row_stride;
      src += head->area.width * 4;
    }
}

static ply_renderer_backend_t *
create_backend (const char *device_name,
                ply_terminal_t *terminal)
{
  ply_renderer_backend_t *backend;

  backend = calloc (1, sizeof (ply_renderer_backend_t));

  if (device_name != NULL)
    backend->device_name = strdup (device_name);
  else if (getenv ("FRAMEBUFFER") != NULL)
    backend->device_name = strdup (getenv ("FRAMEBUFFER"));
  else
    backend->device_name =
      strdup (PLY_FRAME_BUFFER_DEFAULT_FB_DEVICE_NAME);

  ply_trace ("creating renderer backend for device %s", backend->device_name);

  backend->loop = ply_event_loop_get_default ();
  backend->head.map_address = MAP_FAILED;
  backend->heads = ply_list_new ();
  backend->input_source.key_buffer = ply_buffer_new ();
  backend->terminal = terminal;

  return backend;
}

static void
initialize_head (ply_renderer_backend_t *backend,
                 ply_renderer_head_t    *head)
{
  ply_trace ("initializing %lux%lu head",
             head->area.width, head->area.height);
  head->pixel_buffer = ply_pixel_buffer_new (head->area.width,
                                             head->area.height);
  ply_pixel_buffer_fill_with_color (backend->head.pixel_buffer, NULL,
                                    0.0, 0.0, 0.0, 1.0);
  ply_list_append_data (backend->heads, head);
}

static void
uninitialize_head (ply_renderer_backend_t *backend,
                   ply_renderer_head_t    *head)
{
  ply_trace ("uninitializing %lux%lu head",
             head->area.width, head->area.height);
  if (head->pixel_buffer != NULL)
    {
      ply_pixel_buffer_free (head->pixel_buffer);
      head->pixel_buffer = NULL;

      ply_list_remove_data (backend->heads, head);
    }
}

static void
destroy_backend (ply_renderer_backend_t *backend)
{

  ply_trace ("destroying renderer backend for device %s",
             backend->device_name);
  free (backend->device_name);
  uninitialize_head (backend, &backend->head);

  ply_list_free (backend->heads);

  free (backend);
}

static void
activate (ply_renderer_backend_t *backend)
{
  ply_trace ("Redrawing screen");
  backend->is_active = true;

  if (backend->head.map_address != MAP_FAILED)
    ply_renderer_head_redraw (backend, &backend->head);
}

static void
deactivate (ply_renderer_backend_t *backend)
{
  backend->is_active = false;
}

static void
on_active_vt_changed (ply_renderer_backend_t *backend)
{
  if (ply_terminal_is_active (backend->terminal))
    {
      ply_trace ("activating on vt change");
      activate (backend);
    }
  else
    {
      ply_trace ("deactivating on vt change");
      deactivate (backend);
    }
}

static bool
open_device (ply_renderer_backend_t *backend)
{
  backend->device_fd = open (backend->device_name, O_RDWR);

  if (backend->device_fd < 0)
    {
      ply_trace ("could not open '%s': %m", backend->device_name);
      return false;
    }

  if (!ply_terminal_open (backend->terminal))
    {
      ply_trace ("could not open terminal: %m");
      return false;
    }

  if (!ply_terminal_is_vt (backend->terminal))
    {
      ply_trace ("terminal is not a VT");
      ply_terminal_close (backend->terminal);
      return false;
    }

  ply_terminal_watch_for_active_vt_change (backend->terminal,
                                           (ply_terminal_active_vt_changed_handler_t)
                                           on_active_vt_changed,
                                           backend);

  return true;
}

static void
close_device (ply_renderer_backend_t *backend)
{

  ply_terminal_stop_watching_for_active_vt_change (backend->terminal,
                                                   (ply_terminal_active_vt_changed_handler_t)
                                                   on_active_vt_changed,
                                                   backend);
  uninitialize_head (backend, &backend->head);

  close (backend->device_fd);
  backend->device_fd = -1;

  backend->bytes_per_pixel = 0;
  backend->head.area.x = 0;
  backend->head.area.y = 0;
  backend->head.area.width = 0;
  backend->head.area.height = 0;
}

static const char const *get_visual_name (int visual)
{
  static const char const *visuals[] =
    {
      [FB_VISUAL_MONO01] = "FB_VISUAL_MONO01",
      [FB_VISUAL_MONO10] = "FB_VISUAL_MONO10",
      [FB_VISUAL_TRUECOLOR] = "FB_VISUAL_TRUECOLOR",
      [FB_VISUAL_PSEUDOCOLOR] = "FB_VISUAL_PSEUDOCOLOR",
      [FB_VISUAL_DIRECTCOLOR] = "FB_VISUAL_DIRECTCOLOR",
      [FB_VISUAL_STATIC_PSEUDOCOLOR] = "FB_VISUAL_STATIC_PSEUDOCOLOR",
      NULL
    };
  static char unknown[] = "invalid visual: -4294967295";

  if (visual < FB_VISUAL_MONO01 || visual > FB_VISUAL_STATIC_PSEUDOCOLOR)
    {
      sprintf (unknown, "invalid visual: %d", visual);
      return unknown;
    }

  return visuals[visual];
}

static bool
query_device (ply_renderer_backend_t *backend)
{
  struct fb_var_screeninfo variable_screen_info;
  struct fb_fix_screeninfo fixed_screen_info;

  assert (backend != NULL);
  assert (backend->device_fd >= 0);

  if (ioctl (backend->device_fd, FBIOGET_VSCREENINFO, &variable_screen_info) < 0)
    return false;

  if (ioctl (backend->device_fd, FBIOGET_FSCREENINFO, &fixed_screen_info) < 0)
    return false;

  /* Normally the pixel is divided into channels between the color components.
   * Each channel directly maps to a color channel on the hardware.
   *
   * There are some odd ball modes that use an indexed palette instead.  In
   * those cases (pseudocolor, direct color, etc), the pixel value is just an
   * index into a lookup table of the real color values.
   *
   * We don't support that.
   */
  if (fixed_screen_info.visual != FB_VISUAL_TRUECOLOR)
    {
      int rc = -1;
      int i;
      static const int depths[] = {32, 24, 16, 0};

      ply_trace ("Visual was %s, trying to find usable mode.\n",
                 get_visual_name (fixed_screen_info.visual));

      for (i = 0; depths[i] != 0; i++)
        {
          variable_screen_info.bits_per_pixel = depths[i];
          variable_screen_info.activate |= FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;

          rc = ioctl (backend->device_fd, FBIOPUT_VSCREENINFO, &variable_screen_info);
          if (rc >= 0)
            {
              if (ioctl (backend->device_fd, FBIOGET_FSCREENINFO, &fixed_screen_info) < 0)
                return false;

              if (fixed_screen_info.visual == FB_VISUAL_TRUECOLOR)
                break;
            }
        }

      if (ioctl (backend->device_fd, FBIOGET_VSCREENINFO, &variable_screen_info) < 0)
        return false;

      if (ioctl (backend->device_fd, FBIOGET_FSCREENINFO, &fixed_screen_info) < 0)
        return false;
    }

  if (fixed_screen_info.visual != FB_VISUAL_TRUECOLOR ||
      variable_screen_info.bits_per_pixel < 16)
    {
      ply_trace ("Visual is %s; not using graphics\n",
                 get_visual_name (fixed_screen_info.visual));
      return false;
    }

  backend->head.area.x = variable_screen_info.xoffset;
  backend->head.area.y = variable_screen_info.yoffset;
  backend->head.area.width = variable_screen_info.xres;
  backend->head.area.height = variable_screen_info.yres;

  backend->red_bit_position = variable_screen_info.red.offset;
  backend->bits_for_red = variable_screen_info.red.length;

  backend->green_bit_position = variable_screen_info.green.offset;
  backend->bits_for_green = variable_screen_info.green.length;

  backend->blue_bit_position = variable_screen_info.blue.offset;
  backend->bits_for_blue = variable_screen_info.blue.length;

  backend->alpha_bit_position = variable_screen_info.transp.offset;
  backend->bits_for_alpha = variable_screen_info.transp.length;

  backend->bytes_per_pixel = variable_screen_info.bits_per_pixel >> 3;
  backend->row_stride = fixed_screen_info.line_length;
  backend->dither_red = 0;
  backend->dither_green = 0;
  backend->dither_blue = 0;

  ply_trace ("%d bpp (%d, %d, %d, %d) with rowstride %d",
             (int) backend->bytes_per_pixel * 8, 
             backend->bits_for_red,
             backend->bits_for_green,
             backend->bits_for_blue,
             backend->bits_for_alpha,
             (int) backend->row_stride);

  backend->head.size = backend->head.area.height * backend->row_stride;

  if (backend->bytes_per_pixel == 4 &&
      backend->red_bit_position == 16 && backend->bits_for_red == 8 &&
      backend->green_bit_position == 8 && backend->bits_for_green == 8 &&
      backend->blue_bit_position == 0 && backend->bits_for_blue == 8)
    backend->flush_area = flush_area_to_xrgb32_device;
  else
    backend->flush_area = flush_area_to_any_device;

  initialize_head (backend, &backend->head);

  return true;

}

static bool
map_to_device (ply_renderer_backend_t *backend)
{
  ply_renderer_head_t *head;

  assert (backend != NULL);
  assert (backend->device_fd >= 0);

  head = &backend->head;
  assert (head->size > 0);

  head->map_address = mmap (NULL, head->size, PROT_WRITE,
                            MAP_SHARED, backend->device_fd, 0);

  if (head->map_address == MAP_FAILED)
    {
      ply_trace ("could not map fb device: %m");
      return false;
    }

  if (ply_terminal_is_active (backend->terminal))
    {
      ply_trace ("already on right vt, activating");
      activate (backend);
    }
  else
    {
      ply_trace ("on wrong vt, changing vts");
      ply_terminal_activate_vt (backend->terminal);
    }

  return true;
}

static void
unmap_from_device (ply_renderer_backend_t *backend)
{
  ply_renderer_head_t *head;

  head = &backend->head;

  ply_trace ("unmapping device");
  if (head->map_address != MAP_FAILED)
    {
      munmap (head->map_address, head->size);
      head->map_address = MAP_FAILED;
    }
}

static void
flush_head (ply_renderer_backend_t *backend,
            ply_renderer_head_t    *head)
{
  ply_region_t *updated_region;
  ply_list_t *areas_to_flush;
  ply_list_node_t *node;
  ply_pixel_buffer_t *pixel_buffer;

  assert (backend != NULL);
  assert (&backend->head == head);

  if (!backend->is_active)
    return;

  ply_terminal_set_mode (backend->terminal, PLY_TERMINAL_MODE_GRAPHICS);
  ply_terminal_set_unbuffered_input (backend->terminal);
  pixel_buffer = head->pixel_buffer;
  updated_region = ply_pixel_buffer_get_updated_areas (pixel_buffer);
  areas_to_flush = ply_region_get_sorted_rectangle_list (updated_region);

  node = ply_list_get_first_node (areas_to_flush);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_rectangle_t *area_to_flush;

      area_to_flush = (ply_rectangle_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (areas_to_flush, node);

      backend->flush_area (backend, head, area_to_flush);

      node = next_node;
    }

  ply_region_clear (updated_region);
}

static void
ply_renderer_head_redraw (ply_renderer_backend_t *backend,
                          ply_renderer_head_t    *head)
{
  ply_region_t *region;

  region = ply_pixel_buffer_get_updated_areas (head->pixel_buffer);

  ply_region_add_rectangle (region, &head->area);

  flush_head (backend, head);
}

static ply_list_t *
get_heads (ply_renderer_backend_t *backend)
{
  return backend->heads;
}

static ply_pixel_buffer_t *
get_buffer_for_head (ply_renderer_backend_t *backend,
                     ply_renderer_head_t    *head)
{

  if (head != &backend->head)
    return NULL;

  return backend->head.pixel_buffer;
}

static bool
has_input_source (ply_renderer_backend_t      *backend,
                  ply_renderer_input_source_t *input_source)
{
  return input_source == &backend->input_source;
}

static ply_renderer_input_source_t *
get_input_source (ply_renderer_backend_t *backend)
{
  return &backend->input_source;
}

static void
on_key_event (ply_renderer_input_source_t *input_source,
              int                          terminal_fd)
{ 
  ply_buffer_append_from_fd (input_source->key_buffer,
                             terminal_fd);

  if (input_source->handler != NULL)
    input_source->handler (input_source->user_data, input_source->key_buffer, input_source);

}

static void
on_input_source_disconnected (ply_renderer_input_source_t *input_source)
{
  ply_trace ("input source disconnected, reopening");
  open_input_source (input_source->backend, input_source);
}

static bool
open_input_source (ply_renderer_backend_t      *backend,
                   ply_renderer_input_source_t *input_source)
{
  int terminal_fd;

  assert (backend != NULL);
  assert (has_input_source (backend, input_source));

  terminal_fd = ply_terminal_get_fd (backend->terminal);

  input_source->backend = backend;
  input_source->terminal_input_watch = ply_event_loop_watch_fd (backend->loop, terminal_fd, PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                                                                (ply_event_handler_t) on_key_event,
                                                                (ply_event_handler_t) on_input_source_disconnected,
                                                                input_source);
  return true;
}

static void
set_handler_for_input_source (ply_renderer_backend_t      *backend,
                              ply_renderer_input_source_t *input_source,
                              ply_renderer_input_source_handler_t handler,
                              void                        *user_data)
{
  assert (backend != NULL);
  assert (has_input_source (backend, input_source));

  input_source->handler = handler;
  input_source->user_data = user_data;
}

static void
close_input_source (ply_renderer_backend_t      *backend,
                    ply_renderer_input_source_t *input_source)
{
  assert (backend != NULL);
  assert (has_input_source (backend, input_source));

  ply_event_loop_stop_watching_fd (backend->loop, input_source->terminal_input_watch);
  input_source->terminal_input_watch = NULL;
  input_source->backend = NULL;
}

ply_renderer_plugin_interface_t *
ply_renderer_backend_get_interface (void)
{
  static ply_renderer_plugin_interface_t plugin_interface =
    {
      .create_backend = create_backend,
      .destroy_backend = destroy_backend,
      .open_device = open_device,
      .close_device = close_device,
      .query_device = query_device,
      .map_to_device = map_to_device,
      .unmap_from_device = unmap_from_device,
      .activate = activate,
      .deactivate = deactivate,
      .flush_head = flush_head,
      .get_heads = get_heads,
      .get_buffer_for_head = get_buffer_for_head,
      .get_input_source = get_input_source,
      .open_input_source = open_input_source,
      .set_handler_for_input_source = set_handler_for_input_source,
      .close_input_source = close_input_source
    };

  return &plugin_interface;
}
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
