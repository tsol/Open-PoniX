/* ply-renderer.h - rendering abstraction
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
#ifndef PLY_RENDERER_H
#define PLY_RENDERER_H

#include <stdbool.h>
#include <stdint.h>

#include "ply-buffer.h"
#include "ply-list.h"
#include "ply-pixel-buffer.h"
#include "ply-terminal.h"
#include "ply-utils.h"

typedef struct _ply_renderer ply_renderer_t;
typedef struct _ply_renderer_head ply_renderer_head_t;
typedef struct _ply_renderer_input_source ply_renderer_input_source_t;

typedef void (* ply_renderer_input_source_handler_t) (void                        *user_data,
                                                      ply_buffer_t                *key_buffer,
                                                      ply_renderer_input_source_t *input_source);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_renderer_t *ply_renderer_new (const char     *plugin_path,
                                  const char     *device_name,
                                  ply_terminal_t *terminal);
void ply_renderer_free (ply_renderer_t *renderer);
bool ply_renderer_open (ply_renderer_t *renderer);
void ply_renderer_close (ply_renderer_t *renderer);
void ply_renderer_activate (ply_renderer_t *renderer);
void ply_renderer_deactivate (ply_renderer_t *renderer);
ply_list_t *ply_renderer_get_heads (ply_renderer_t *renderer);
ply_pixel_buffer_t *ply_renderer_get_buffer_for_head (ply_renderer_t      *renderer,
                                                      ply_renderer_head_t *head);

void ply_renderer_flush_head (ply_renderer_t      *renderer,
                              ply_renderer_head_t *head);

ply_renderer_input_source_t *ply_renderer_get_input_source (ply_renderer_t *renderer);
bool ply_renderer_open_input_source (ply_renderer_t              *renderer,
                                     ply_renderer_input_source_t *input_source);
void ply_renderer_set_handler_for_input_source (ply_renderer_t                      *renderer,
                                                ply_renderer_input_source_t         *input_source,
                                                ply_renderer_input_source_handler_t  handler,
                                                void                                *user_data);

void ply_renderer_close_input_source (ply_renderer_t              *renderer,
                                      ply_renderer_input_source_t *input_source);
#endif

#endif /* PLY_RENDERER_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
