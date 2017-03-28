/* ply-boot-splash-plugin.h - plugin interface for ply_label_t
 *
 * Copyright (C) 2007 Red Hat, Inc.
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
#ifndef PLY_LABEL_PLUGIN_H
#define PLY_LABEL_PLUGIN_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-label.h"

typedef struct _ply_label_plugin ply_label_plugin_t;
typedef struct _ply_label_plugin_control ply_label_plugin_control_t;

typedef struct
{
  ply_label_plugin_control_t * (* create_control) (void);
  void (* destroy_control) (ply_label_plugin_control_t        *label);
  bool (* show_control) (ply_label_plugin_control_t        *label,
                         ply_pixel_display_t               *display,
                         long                               x,
                         long                               y);
  void (* hide_control) (ply_label_plugin_control_t        *label);
  void (* draw_control) (ply_label_plugin_control_t        *label,
                         ply_pixel_buffer_t                *pixel_buffer,
                         long                               x,
                         long                               y,
                         unsigned long                      width,
                         unsigned long                      height);
  bool (* is_control_hidden) (ply_label_plugin_control_t        *label);

  void (* set_text_for_control) (ply_label_plugin_control_t *label,
                                 const char                 *text);
  void (* set_font_for_control) (ply_label_plugin_control_t *label,
                                 const char                 *fontdesc);
  void (* set_color_for_control) (ply_label_plugin_control_t *label,
                                  float                       red,
                                  float                       green,
                                  float                       blue,
                                  float                       alpha);

  long (* get_width_of_control) (ply_label_plugin_control_t        *label);
  long (* get_height_of_control) (ply_label_plugin_control_t        *label);

  void (* set_alignment_for_control) (ply_label_plugin_control_t  *label,
                                      ply_label_alignment_t alignment);
  void (* set_width_for_control) (ply_label_plugin_control_t  *label,
                                  long                        width);
} ply_label_plugin_interface_t;

#endif /* PLY_LABEL_PLUGIN_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
