/* ply-renderer-generic-driver.c - interface to generic drm kms api
 *
 * Copyright (C) 2012 Red Hat, Inc.
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
 * Written by: Dave Airlie
 * Based on other renderer drivers written by Ray Strode
 */
#include "config.h"

#include "ply-renderer-generic-driver.h"

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

#include <drm.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "ply-hashtable.h"
#include "ply-logger.h"
#include "ply-renderer-driver.h"

typedef struct _ply_renderer_buffer ply_renderer_buffer_t;

struct _ply_renderer_buffer
{
  uint32_t id;

  uint32_t handle;
  uint32_t width;
  uint32_t height;
  uint32_t row_stride;

  void *map_address;
  uint32_t map_size;
  int map_count;

  uint32_t added_fb : 1;
};

struct _ply_renderer_driver
{
  int device_fd;
  ply_hashtable_t *buffers;
};

static bool
ply_renderer_buffer_map (ply_renderer_driver_t *driver,
                         ply_renderer_buffer_t *buffer)
{
  struct drm_mode_map_dumb map_dumb_buffer_request;
  void *map_address;

  if (buffer->map_address != MAP_FAILED)
    {
      buffer->map_count++;
      return true;
    }

  memset (&map_dumb_buffer_request, 0, sizeof (struct drm_mode_map_dumb));
  map_dumb_buffer_request.handle = buffer->handle;
  if (drmIoctl (driver->device_fd, DRM_IOCTL_MODE_MAP_DUMB, &map_dumb_buffer_request) < 0)
    {
      ply_trace ("Could not map GEM object %u: %m", buffer->handle);
      return false;
    }

  map_address = mmap (0, buffer->map_size,
                      PROT_READ | PROT_WRITE, MAP_SHARED,
                      driver->device_fd, map_dumb_buffer_request.offset);

  if (map_address == MAP_FAILED)
    return false;

  buffer->map_address = map_address;
  buffer->map_count++;

  return true;
}

static void
ply_renderer_buffer_unmap (ply_renderer_driver_t *driver,
                           ply_renderer_buffer_t *buffer)
{
  buffer->map_count--;

  assert (buffer->map_count >= 0);
}

static ply_renderer_driver_t *
create_driver (int device_fd)
{
  ply_renderer_driver_t *driver;

  driver = calloc (1, sizeof (ply_renderer_driver_t));
  driver->device_fd = device_fd;

  driver->buffers = ply_hashtable_new (ply_hashtable_direct_hash,
                                       ply_hashtable_direct_compare);

  return driver;
}

static void
destroy_driver (ply_renderer_driver_t *driver)
{
  ply_hashtable_free (driver->buffers);

  free (driver);
}

static ply_renderer_buffer_t *
ply_renderer_buffer_new (ply_renderer_driver_t *driver,
                         uint32_t               width,
                         uint32_t               height)
{
  ply_renderer_buffer_t *buffer;
  struct drm_mode_create_dumb create_dumb_buffer_request;

  buffer = calloc (1, sizeof (ply_renderer_buffer_t));
  buffer->width = width;
  buffer->height = height;
  buffer->map_address = MAP_FAILED;

  memset (&create_dumb_buffer_request, 0, sizeof (struct drm_mode_create_dumb));

  create_dumb_buffer_request.width = width;
  create_dumb_buffer_request.height = height;
  create_dumb_buffer_request.bpp = 32;
  create_dumb_buffer_request.flags = 0;

  if (drmIoctl (driver->device_fd,
                DRM_IOCTL_MODE_CREATE_DUMB,
                &create_dumb_buffer_request) < 0)
    {
      free (buffer);
      ply_trace ("Could not allocate GEM object for frame buffer: %m");
      return NULL;
    }

  buffer->handle = create_dumb_buffer_request.handle;
  buffer->row_stride = create_dumb_buffer_request.pitch;
  buffer->map_size = create_dumb_buffer_request.size;

  ply_trace ("returning %ux%u buffer with stride %u",
             width, height, buffer->row_stride);

  return buffer;
}

static void
ply_renderer_buffer_free (ply_renderer_driver_t *driver,
                          ply_renderer_buffer_t *buffer)
{
  struct drm_mode_destroy_dumb destroy_dumb_buffer_request;

  if (buffer->added_fb)
    drmModeRmFB (driver->device_fd, buffer->id);

  if (buffer->map_address != MAP_FAILED)
    {
      munmap (buffer->map_address, buffer->map_size);
      buffer->map_address = MAP_FAILED;
    }

  memset (&destroy_dumb_buffer_request, 0, sizeof (struct drm_mode_destroy_dumb));
  destroy_dumb_buffer_request.handle = buffer->handle;

  if (drmIoctl (driver->device_fd,
                DRM_IOCTL_MODE_DESTROY_DUMB,
                &destroy_dumb_buffer_request) < 0)
    {
      ply_trace ("Could not deallocate GEM object %u: %m", buffer->handle);
    }

  free (buffer);
}

static ply_renderer_buffer_t *
get_buffer_from_id (ply_renderer_driver_t *driver,
                    uint32_t               id)
{
  static ply_renderer_buffer_t *buffer;

  buffer = ply_hashtable_lookup (driver->buffers, (void *) (uintptr_t) id);

  return buffer;
}

static bool
fetch_buffer (ply_renderer_driver_t *driver,
              uint32_t               buffer_id,
              unsigned long         *width,
              unsigned long         *height,
              unsigned long         *row_stride)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  if (buffer == NULL)
    {
      ply_trace ("could not fetch buffer %u", buffer_id);
      return false;
    }

  if (width != NULL)
    *width = buffer->width;

  if (height != NULL)
    *height = buffer->height;

  if (row_stride != NULL)
    *row_stride = buffer->row_stride;

  ply_trace ("fetched %ux%u buffer with stride %u",
             buffer->width, buffer->height, buffer->row_stride);
  return true;
}

static uint32_t
create_buffer (ply_renderer_driver_t *driver,
               unsigned long          width,
               unsigned long          height,
               unsigned long         *row_stride)
{
  ply_renderer_buffer_t *buffer;

  buffer = ply_renderer_buffer_new (driver, width, height);

  if (buffer == NULL)
    {
      ply_trace ("Could not allocate GEM object for frame buffer: %m");
      return 0;
    }

  if (drmModeAddFB (driver->device_fd, width, height,
                    24, 32, buffer->row_stride, buffer->handle,
                    &buffer->id) != 0)
    {
      ply_trace ("Could not set up GEM object as frame buffer: %m");
      ply_renderer_buffer_free (driver, buffer);
      return 0;
    }

  *row_stride = buffer->row_stride;

  buffer->added_fb = true;
  ply_hashtable_insert (driver->buffers,
                        (void *) (uintptr_t) buffer->id,
                        buffer);

  return buffer->id;
}

static bool
map_buffer (ply_renderer_driver_t *driver,
            uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  return ply_renderer_buffer_map (driver, buffer);
}

static void
unmap_buffer (ply_renderer_driver_t *driver,
              uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  ply_renderer_buffer_unmap (driver, buffer);
}

static char *
begin_flush (ply_renderer_driver_t *driver,
             uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  return buffer->map_address;
}

static void
end_flush (ply_renderer_driver_t *driver,
           uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);
}

static void
destroy_buffer (ply_renderer_driver_t *driver,
                uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = ply_hashtable_remove (driver->buffers,
                                 (void *) (uintptr_t) buffer_id);

  assert (buffer != NULL);

  ply_renderer_buffer_free (driver, buffer);
}

ply_renderer_driver_interface_t *
ply_renderer_generic_driver_get_interface (int device_fd)
{
  uint64_t supports_dumb_buffers;

  static ply_renderer_driver_interface_t driver_interface =
    {
      .create_driver = create_driver,
      .destroy_driver = destroy_driver,
      .create_buffer = create_buffer,
      .fetch_buffer = fetch_buffer,
      .map_buffer = map_buffer,
      .unmap_buffer = unmap_buffer,
      .begin_flush = begin_flush,
      .end_flush = end_flush,
      .destroy_buffer = destroy_buffer,
    };


  if (drmGetCap (device_fd, DRM_CAP_DUMB_BUFFER, &supports_dumb_buffers) < 0)
    return NULL;

  if (!supports_dumb_buffers)
    return NULL;

  return &driver_interface;
}

/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s, (0,=.5s,:.5s */
