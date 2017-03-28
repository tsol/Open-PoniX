/* ply-renderer-plugin.h - plugin interface for ply_renderer_t
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_RENDERER_PLUGIN_H
#define PLY_RENDERER_PLUGIN_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-terminal.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-region.h"
#include "ply-renderer.h"

typedef struct _ply_renderer_plugin ply_renderer_plugin_t;
typedef struct _ply_renderer_backend ply_renderer_backend_t;

typedef struct
{
  ply_renderer_backend_t * (* create_backend) (const char *device_name,
                                               ply_terminal_t *terminal);
  void (* destroy_backend) (ply_renderer_backend_t *backend);
  bool (* open_device) (ply_renderer_backend_t *backend);
  void (* close_device) (ply_renderer_backend_t *backend);
  bool (* query_device) (ply_renderer_backend_t *backend);
  bool (* map_to_device) (ply_renderer_backend_t *backend);
  void (* unmap_from_device) (ply_renderer_backend_t *backend);
  void (* activate) (ply_renderer_backend_t *backend);
  void (* deactivate) (ply_renderer_backend_t *backend);
  void (* flush_head) (ply_renderer_backend_t *backend,
                       ply_renderer_head_t    *head);

  ply_list_t * (* get_heads) (ply_renderer_backend_t *backend);

  ply_pixel_buffer_t * (* get_buffer_for_head) (ply_renderer_backend_t *backend,
                                                ply_renderer_head_t    *head);

  ply_renderer_input_source_t * (* get_input_source) (ply_renderer_backend_t *backend);
  bool                 (* open_input_source) (ply_renderer_backend_t *backend,
                                              ply_renderer_input_source_t *input_source);

  void                 (* set_handler_for_input_source) (ply_renderer_backend_t    *backend,
                                                         ply_renderer_input_source_t *input_source,
                                                         ply_renderer_input_source_handler_t handler,
                                                         void                      *user_data);

  void                 (* close_input_source) (ply_renderer_backend_t *backend,
                                               ply_renderer_input_source_t *input_source);
} ply_renderer_plugin_interface_t;

#endif /* PLY_RENDERER_PLUGIN_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
