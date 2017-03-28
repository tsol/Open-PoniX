/* ply-renderer.c - renderer abstraction
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
#include "ply-renderer.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "ply-renderer-plugin.h"
#include "ply-buffer.h"
#include "ply-terminal.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-utils.h"

struct _ply_renderer
{
  ply_event_loop_t *loop;
  ply_module_handle_t *module_handle;
  const ply_renderer_plugin_interface_t *plugin_interface;
  ply_renderer_backend_t *backend;

  char *plugin_path;
  char *device_name;
  ply_terminal_t *terminal;

  uint32_t input_source_is_open : 1;
  uint32_t is_mapped : 1;
};

typedef const ply_renderer_plugin_interface_t *
        (* get_backend_interface_function_t) (void);

static void ply_renderer_unload_plugin (ply_renderer_t *renderer);

ply_renderer_t *
ply_renderer_new (const char     *plugin_path,
                  const char     *device_name,
                  ply_terminal_t *terminal)
{
  ply_renderer_t *renderer;

  renderer = calloc (1, sizeof (struct _ply_renderer));

  if (plugin_path != NULL)
    renderer->plugin_path = strdup (plugin_path);

  if (device_name != NULL)
    renderer->device_name = strdup (device_name);

  renderer->terminal = terminal;

  return renderer;
}

void
ply_renderer_free (ply_renderer_t *renderer)
{
  if (renderer == NULL)
    return;

  if (renderer->plugin_interface != NULL)
    {
      ply_trace ("Unloading renderer backend plugin");
      ply_renderer_unload_plugin (renderer);
    }

  free (renderer->device_name);
  free (renderer->plugin_path);
  free (renderer);
}

static bool
ply_renderer_load_plugin (ply_renderer_t *renderer,
                          const char     *module_path)
{
  assert (renderer != NULL);

  get_backend_interface_function_t get_renderer_backend_interface;

  renderer->module_handle = ply_open_module (module_path);

  if (renderer->module_handle == NULL)
    return false;

  get_renderer_backend_interface = (get_backend_interface_function_t)
      ply_module_look_up_function (renderer->module_handle,
                                   "ply_renderer_backend_get_interface");

  if (get_renderer_backend_interface == NULL)
    {
      ply_save_errno ();
      ply_trace ("module '%s' is not a renderer plugin",
                 module_path);
      ply_close_module (renderer->module_handle);
      renderer->module_handle = NULL;
      ply_restore_errno ();
      return false;
    }

  renderer->plugin_interface = get_renderer_backend_interface ();

  if (renderer->plugin_interface == NULL)
    {
      ply_trace ("module '%s' is not a valid renderer plugin",
                 module_path);
      ply_save_errno ();
      ply_close_module (renderer->module_handle);
      renderer->module_handle = NULL;
      ply_restore_errno ();
      return false;
    }

  renderer->backend = renderer->plugin_interface->create_backend (renderer->device_name,
                                                                  renderer->terminal);

  if (renderer->backend == NULL)
    {
      ply_save_errno ();
      ply_trace ("module '%s' renderer backend could not be created",
                 module_path);
      ply_close_module (renderer->module_handle);
      renderer->module_handle = NULL;
      ply_restore_errno ();
      return false;
    }

  return true;
}

static void
ply_renderer_unload_plugin (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);
  assert (renderer->module_handle != NULL);

  ply_close_module (renderer->module_handle);
  renderer->plugin_interface = NULL;
  renderer->module_handle = NULL;
}

static bool
ply_renderer_open_device (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->open_device (renderer->backend);
}

static void
ply_renderer_close_device (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  renderer->plugin_interface->close_device (renderer->backend);
}

static bool
ply_renderer_query_device (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->query_device (renderer->backend);
}

static bool
ply_renderer_map_to_device (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  if (renderer->is_mapped)
    return true;

  renderer->is_mapped = renderer->plugin_interface->map_to_device (renderer->backend);

  return renderer->is_mapped;
}

static void
ply_renderer_unmap_from_device (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  if (!renderer->is_mapped)
    return;

  renderer->plugin_interface->unmap_from_device (renderer->backend);
  renderer->is_mapped = false;
}

static bool
ply_renderer_open_plugin (ply_renderer_t *renderer,
                          const char     *plugin_path)
{
  ply_trace ("trying to open renderer plugin %s", plugin_path);

  if (!ply_renderer_load_plugin (renderer, plugin_path))
    return false;

  if (!ply_renderer_open_device (renderer))
    {
      ply_trace ("could not open rendering device for plugin %s",
                 plugin_path);
      ply_renderer_unload_plugin (renderer);
      return false;
    }

  if (!ply_renderer_query_device (renderer))
    {
      ply_trace ("could not query rendering device for plugin %s",
                 plugin_path);
      ply_renderer_close_device (renderer);
      ply_renderer_unload_plugin (renderer);
      return false;
    }

  ply_trace ("opened renderer plugin %s", plugin_path);
  return true;
}

bool
ply_renderer_open (ply_renderer_t *renderer)
{
  int i;

  /* FIXME: at some point we may want to make this
   * part more dynamic (so you don't have to edit this
   * list to add a new renderer)
   */
  const char *known_plugins[] =
    {
      PLYMOUTH_PLUGIN_PATH "renderers/x11.so",
      PLYMOUTH_PLUGIN_PATH "renderers/drm.so",
      PLYMOUTH_PLUGIN_PATH "renderers/frame-buffer.so",
      NULL
    };

  if (renderer->plugin_path != NULL)
    {
      return ply_renderer_open_plugin (renderer, renderer->plugin_path);
    }

  for (i = 0; known_plugins[i] != NULL; i++)
    {
      if (ply_renderer_open_plugin (renderer, known_plugins[i]))
        return true;
  }

  ply_trace ("could not find suitable rendering plugin");
  return false;
}

void
ply_renderer_close (ply_renderer_t *renderer)
{
  ply_renderer_unmap_from_device (renderer);
  ply_renderer_close_device (renderer);
}

void
ply_renderer_activate (ply_renderer_t *renderer)
{
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->activate (renderer->backend);
}

void
ply_renderer_deactivate (ply_renderer_t *renderer)
{
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->deactivate (renderer->backend);
}

ply_list_t *
ply_renderer_get_heads (ply_renderer_t *renderer)
{
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->get_heads (renderer->backend);
}

ply_pixel_buffer_t *
ply_renderer_get_buffer_for_head (ply_renderer_t      *renderer,
                                  ply_renderer_head_t *head)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);
  assert (head != NULL);

  return renderer->plugin_interface->get_buffer_for_head (renderer->backend,
                                                          head);
}

void
ply_renderer_flush_head (ply_renderer_t      *renderer,
                         ply_renderer_head_t *head)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);
  assert (head != NULL);

  if (!ply_renderer_map_to_device (renderer))
    return;

  renderer->plugin_interface->flush_head (renderer->backend, head);
}

ply_renderer_input_source_t *
ply_renderer_get_input_source (ply_renderer_t *renderer)
{
  assert (renderer != NULL);
  assert (renderer->plugin_interface != NULL);

  return renderer->plugin_interface->get_input_source (renderer->backend);
}

bool
ply_renderer_open_input_source (ply_renderer_t              *renderer,
                                ply_renderer_input_source_t *input_source)
{
  assert (renderer != NULL);
  assert (input_source != NULL);

  renderer->input_source_is_open = renderer->plugin_interface->open_input_source (renderer->backend,
                                                                                  input_source);

  return renderer->input_source_is_open;
}

void
ply_renderer_set_handler_for_input_source (ply_renderer_t                      *renderer,
                                           ply_renderer_input_source_t         *input_source,
                                           ply_renderer_input_source_handler_t  handler,
                                           void                                *user_data)
{
  assert (renderer != NULL);
  assert (input_source != NULL);

  renderer->plugin_interface->set_handler_for_input_source (renderer->backend,
                                                            input_source,
                                                            handler,
                                                            user_data);
}

void
ply_renderer_close_input_source (ply_renderer_t              *renderer,
                                 ply_renderer_input_source_t *input_source)
{
  assert (renderer != NULL);
  assert (input_source != NULL);

  if (!renderer->input_source_is_open)
    return;

  renderer->plugin_interface->close_input_source (renderer->backend,
                                                  input_source);
  renderer->input_source_is_open = false;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
