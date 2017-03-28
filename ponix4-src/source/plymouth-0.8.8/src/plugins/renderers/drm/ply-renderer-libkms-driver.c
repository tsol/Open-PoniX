/* ply-renderer-libkms-driver.c - interface to libkms abstraction over drm drivers
 *
 * Copyright (C) 2010 Red Hat, Inc.
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

#include "ply-renderer-libkms-driver.h"

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
#include <libkms.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "ply-array.h"
#include "ply-hashtable.h"
#include "ply-logger.h"
#include "ply-renderer-driver.h"

typedef struct _ply_renderer_buffer ply_renderer_buffer_t;

struct _ply_renderer_buffer
{
  struct kms_bo *object;
  uint32_t id;
  unsigned long width;
  unsigned long height;
  unsigned long row_stride;

  void *map_address;

  uint32_t added_fb : 1;
};

struct _ply_renderer_driver
{
  int device_fd;
  struct kms_driver *driver;

  ply_hashtable_t *buffers;
};

static ply_renderer_driver_t *
create_driver (int device_fd)
{
  ply_renderer_driver_t *driver;
  int result;

  driver = calloc (1, sizeof (ply_renderer_driver_t));
  driver->device_fd = device_fd;

  result = kms_create (driver->device_fd, &driver->driver);
  if (result != 0)
    {
      ply_trace ("kms buffer driver could not be initialized: %d", result);
      free (driver);
      return NULL;
    }

  driver->buffers = ply_hashtable_new (ply_hashtable_direct_hash,
                                       ply_hashtable_direct_compare);

  return driver;
}

static void
destroy_driver (ply_renderer_driver_t *driver)
{
  ply_hashtable_free (driver->buffers);

  ply_trace ("uninitializing kms buffer driver");
  kms_destroy (&driver->driver);
  free (driver);
}

static ply_renderer_buffer_t *
ply_renderer_buffer_new (ply_renderer_driver_t *driver,
                         struct kms_bo *buffer_object,
                         uint32_t id,
                         unsigned long width,
                         unsigned long height,
                         unsigned long row_stride)
{
  ply_renderer_buffer_t *buffer;

  buffer = calloc (1, sizeof (ply_renderer_buffer_t));
  buffer->object = buffer_object;
  buffer->id = id;
  buffer->width = width;
  buffer->height = height;
  buffer->row_stride = row_stride;

  ply_trace ("returning %lux%lu buffer with stride %lu",
             width, height, row_stride);

  return buffer;
}

static ply_renderer_buffer_t *
get_buffer_from_id (ply_renderer_driver_t *driver,
                    uint32_t               id)
{
  static ply_renderer_buffer_t *buffer;

  buffer = ply_hashtable_lookup (driver->buffers, (void *) (uintptr_t) id);

  return buffer;
}

static struct kms_bo *
create_kms_bo_from_handle (ply_renderer_driver_t *driver,
                           uint32_t               handle)
{
  struct drm_gem_flink flink_request;
  struct kms_bo *buffer_object;
  ply_array_t *attributes;
  int result;

  /* FIXME: This can't be the right way to do this.
   *
   * 1) It requires skirting around the API and using ioctls
   * 2) It requires taking a local handle, turning it into a
   * a global handle ("name"), just so we can use an api that
   * will open the global name and grab the local handle from it.
   */

  memset (&flink_request, 0, sizeof (struct drm_gem_flink));
  flink_request.handle = handle;

  if (ioctl (driver->device_fd, DRM_IOCTL_GEM_FLINK, &flink_request) < 0)
    {
      ply_trace ("Could not export global name for handle %u", handle);
      return NULL;
    }

  attributes = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_UINT32);
  ply_array_add_uint32_element (attributes, KMS_HANDLE);
  ply_array_add_uint32_element (attributes, flink_request.name);
  ply_array_add_uint32_element (attributes, KMS_TERMINATE_PROP_LIST);
  result = kms_bo_create (driver->driver,
                          (const unsigned *)
                          ply_array_get_uint32_elements (attributes),
                          &buffer_object);
  ply_array_free (attributes);

  if (result != 0)
    {
      ply_trace ("could not create buffer object from global name %u: %d",
                 flink_request.name, result);
      return NULL;
    }

  return buffer_object;
}

static ply_renderer_buffer_t *
ply_renderer_buffer_new_from_id (ply_renderer_driver_t *driver,
                                 uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;
  drmModeFB *fb;
  struct kms_bo *buffer_object;

  fb = drmModeGetFB (driver->device_fd, buffer_id);

  if (fb == NULL)
    {
      ply_trace ("could not get FB with buffer id %u", buffer_id);
      return NULL;
    }

  buffer_object = create_kms_bo_from_handle (driver, fb->handle);

  if (buffer_object == NULL)
    {
      ply_trace ("could not create buffer object from handle %lu",
                 (unsigned long) fb->handle);
      drmModeFreeFB (fb);
      return NULL;
    }

  buffer = ply_renderer_buffer_new (driver, buffer_object, buffer_id,
                                    fb->width, fb->height, fb->pitch);
  drmModeFreeFB (fb);

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
      ply_trace ("could not fetch buffer %u, creating one", buffer_id);
      buffer = ply_renderer_buffer_new_from_id (driver, buffer_id);

      if (buffer == NULL)
        {
          ply_trace ("could not create buffer either %u", buffer_id);
          return false;
        }

      ply_hashtable_insert (driver->buffers,
                            (void *) (uintptr_t) buffer_id,
                            buffer);
    }

  if (width != NULL)
    *width = buffer->width;

  if (height != NULL)
    *height = buffer->height;

  if (row_stride != NULL)
    *row_stride = buffer->row_stride;

  ply_trace ("fetched %lux%lu buffer with stride %lu",
             buffer->width, buffer->height, buffer->row_stride);
  return true;
}

static uint32_t
create_buffer (ply_renderer_driver_t *driver,
               unsigned long          width,
               unsigned long          height,
               unsigned long         *row_stride)
{
  struct kms_bo *buffer_object;
  ply_renderer_buffer_t *buffer;
  uint32_t buffer_id;
  int result;
  unsigned int handle;
  ply_array_t *attributes;

  *row_stride = ply_round_to_multiple (width * 4, 256);

  attributes = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_UINT32);
  ply_array_add_uint32_element (attributes, KMS_BO_TYPE);
  ply_array_add_uint32_element (attributes, KMS_BO_TYPE_SCANOUT_X8R8G8B8);
  ply_array_add_uint32_element (attributes, KMS_WIDTH);
  ply_array_add_uint32_element (attributes, (uint32_t) width);
  ply_array_add_uint32_element (attributes, KMS_HEIGHT);
  ply_array_add_uint32_element (attributes, (uint32_t) height);
  ply_array_add_uint32_element (attributes, KMS_PITCH);
  ply_array_add_uint32_element (attributes, (uint32_t) *row_stride);
  ply_array_add_uint32_element (attributes, KMS_TERMINATE_PROP_LIST);
  result = kms_bo_create (driver->driver,
                          (const unsigned *)
                          ply_array_get_uint32_elements (attributes),
                          &buffer_object);
  ply_array_free (attributes);

  if (result != 0)
    {
      ply_trace ("Could not allocate GEM object for frame buffer: %d", result);
      return 0;
    }

  result = kms_bo_get_prop (buffer_object, KMS_HANDLE, &handle);

  if (result != 0)
    {
      ply_trace ("Could not retrieve handle from GEM object: %d", result);

      kms_bo_destroy (&buffer_object);
      return 0;
    }

  if (drmModeAddFB (driver->device_fd, width, height,
                    24, 32, *row_stride, handle,
                    &buffer_id) != 0)
    {
      ply_trace ("Could not set up GEM object as frame buffer: %m");
      kms_bo_destroy (&buffer_object);
      return 0;
    }

  buffer = ply_renderer_buffer_new (driver,
                                    buffer_object, buffer_id,
                                    width, height, *row_stride);
  buffer->added_fb = true;
  ply_hashtable_insert (driver->buffers,
                        (void *) (uintptr_t) buffer_id,
                        buffer);

  return buffer_id;
}

static bool
map_buffer (ply_renderer_driver_t *driver,
            uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;
  int result;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  result = kms_bo_map (buffer->object, &buffer->map_address);

  if (result != 0)
    {
      ply_trace ("could not map buffer %u: %d", buffer_id, result);
      buffer->map_address = MAP_FAILED;
      return false;
    }

  return true;
}

static void
unmap_buffer (ply_renderer_driver_t *driver,
              uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  kms_bo_unmap (buffer->object);
  buffer->map_address = MAP_FAILED;
}

static char *
begin_flush (ply_renderer_driver_t *driver,
             uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  return (char *) buffer->map_address;
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

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  if (buffer->added_fb)
    drmModeRmFB (driver->device_fd, buffer->id);

  kms_bo_destroy (&buffer->object);

  ply_hashtable_remove (driver->buffers,
                        (void *) (uintptr_t) buffer_id);
  free (buffer);
}

ply_renderer_driver_interface_t *
ply_renderer_libkms_driver_get_interface (void)
{
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

  return &driver_interface;
}

/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
