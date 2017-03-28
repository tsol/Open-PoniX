/* ply-renderer-i915-driver.c - interface to i915 drm driver
 *
 * Copyright (C) 2009 Red Hat, Inc.
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

#include "ply-renderer-i915-driver.h"

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
#include <i915_drm.h>
#include <intel_bufmgr.h>
#include <xf86drm.h>
#include <xf86drmMode.h>

#include "ply-hashtable.h"
#include "ply-logger.h"
#include "ply-renderer-driver.h"

typedef struct _ply_renderer_buffer ply_renderer_buffer_t;

struct _ply_renderer_buffer
{
  drm_intel_bo *object;
  uint32_t id;
  unsigned long width;
  unsigned long height;
  unsigned long row_stride;

  uint32_t added_fb : 1;
};

struct _ply_renderer_driver
{
  int device_fd;
  drm_intel_bufmgr *manager;

  ply_hashtable_t *buffers;
};

static ply_renderer_driver_t *
create_driver (int device_fd)
{
  ply_renderer_driver_t *driver;
  int page_size;

  driver = calloc (1, sizeof (ply_renderer_driver_t));
  driver->device_fd = device_fd;

  page_size = (int) sysconf (_SC_PAGE_SIZE);

  driver->manager = drm_intel_bufmgr_gem_init (driver->device_fd, page_size);
  if (driver->manager == NULL)
    {
      ply_trace ("intel buffer manager could not be initialized");
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

  ply_trace ("uninitializing intel buffer manager");
  drm_intel_bufmgr_destroy (driver->manager);
  free (driver);
}

static ply_renderer_buffer_t *
ply_renderer_buffer_new (ply_renderer_driver_t *driver,
                         drm_intel_bo *buffer_object,
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

static drm_intel_bo *
create_intel_bo_from_handle (ply_renderer_driver_t *driver,
                             uint32_t               handle)
{
  struct drm_gem_flink flink_request;
  char *name;
  drm_intel_bo *buffer_object;

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

  asprintf (&name, "buffer %u", handle);

  buffer_object = drm_intel_bo_gem_create_from_name (driver->manager,
                                                     name, flink_request.name);
  free (name);

  return buffer_object;
}

static ply_renderer_buffer_t *
ply_renderer_buffer_new_from_id (ply_renderer_driver_t *driver,
                                 uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;
  drmModeFB *fb;
  drm_intel_bo *buffer_object;

  fb = drmModeGetFB (driver->device_fd, buffer_id);

  if (fb == NULL)
    {
      ply_trace ("could not get FB with buffer id %u", buffer_id);
      return NULL;
    }

  buffer_object = create_intel_bo_from_handle (driver, fb->handle);

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

static ply_renderer_buffer_t *
get_buffer_from_id (ply_renderer_driver_t *driver,
                    uint32_t               buffer_id)
{
  static ply_renderer_buffer_t *buffer;

  buffer = ply_hashtable_lookup (driver->buffers,
                                 (void *) (uintptr_t) buffer_id);

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
  drm_intel_bo *buffer_object;
  ply_renderer_buffer_t *buffer;
  uint32_t buffer_id;

  *row_stride = ply_round_to_multiple (width * 4, 256);

  buffer_object = drm_intel_bo_alloc (driver->manager,
                                      "frame buffer",
                                      height * *row_stride, 0);

  if (buffer_object == NULL)
    {
      ply_trace ("Could not allocate GEM object for frame buffer: %m");
      return 0;
    }

  if (drmModeAddFB (driver->device_fd, width, height,
                    24, 32, *row_stride, buffer_object->handle,
                    &buffer_id) != 0)
    {
      ply_trace ("Could not set up GEM object as frame buffer: %m");
      drm_intel_bo_unreference (buffer_object);
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

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);
  drm_intel_gem_bo_map_gtt (buffer->object);

  return true;
}

static void
unmap_buffer (ply_renderer_driver_t *driver,
              uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);
  drm_intel_gem_bo_unmap_gtt (buffer->object);
}

static char *
begin_flush (ply_renderer_driver_t *driver,
             uint32_t               buffer_id)
{
  ply_renderer_buffer_t *buffer;

  buffer = get_buffer_from_id (driver, buffer_id);

  assert (buffer != NULL);

  return buffer->object->virtual;
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

  drm_intel_bo_unreference (buffer->object);

  ply_hashtable_remove (driver->buffers,
                        (void *) (uintptr_t) buffer_id);
  free (buffer);
}

ply_renderer_driver_interface_t *
ply_renderer_i915_driver_get_interface (void)
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
