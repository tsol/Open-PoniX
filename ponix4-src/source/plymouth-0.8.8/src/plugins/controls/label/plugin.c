/* ply-label.c - label control
 *
 * Copyright (C) 2008 Red Hat, Inc.
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

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <values.h>
#include <unistd.h>
#include <wchar.h>

#include <glib.h>
#include <cairo.h>
#include <pango/pangocairo.h>

#include "ply-pixel-buffer.h"
#include "ply-pixel-display.h"
#include "ply-utils.h"

#include "ply-label-plugin.h"

struct _ply_label_plugin_control
{
  ply_event_loop_t   *loop;
  ply_pixel_display_t       *display;
  ply_rectangle_t     area;

  char               *text;
  char               *fontdesc;

  PangoAlignment      alignment;
  long                width;
  float               red;
  float               green;
  float               blue;
  float               alpha;

  uint32_t is_hidden : 1;
};

ply_label_plugin_interface_t * ply_label_plugin_get_interface (void);

static ply_label_plugin_control_t *
create_control (void)
{
  ply_label_plugin_control_t *label;

  label = calloc (1, sizeof (ply_label_plugin_control_t));

  label->is_hidden = true;
  label->alignment = PANGO_ALIGN_LEFT;
  label->width     = -1;

  return label;
}

static void
destroy_control (ply_label_plugin_control_t *label)
{
  if (label == NULL)
    return;

  free (label);
}

static long
get_width_of_control (ply_label_plugin_control_t *label)
{
  return label->area.width;
}

static long
get_height_of_control (ply_label_plugin_control_t *label)
{
  return label->area.height;
}

static cairo_t *
get_cairo_context_for_pixel_buffer (ply_label_plugin_control_t *label,
                                    ply_pixel_buffer_t         *pixel_buffer)
{
  cairo_surface_t *cairo_surface;
  cairo_t         *cairo_context;
  unsigned char   *data;
  ply_rectangle_t  size;

  data = (unsigned char *) ply_pixel_buffer_get_argb32_data (pixel_buffer);
  ply_pixel_buffer_get_size (pixel_buffer, &size);

  cairo_surface = cairo_image_surface_create_for_data (data,
                                                       CAIRO_FORMAT_ARGB32,
                                                       size.width,
                                                       size.height,
                                                       size.width * 4);
  cairo_context = cairo_create (cairo_surface);
  cairo_surface_destroy (cairo_surface);

  return cairo_context;
}

static cairo_t *
get_cairo_context_for_sizing (ply_label_plugin_control_t *label)
{
  cairo_surface_t *cairo_surface;
  cairo_t         *cairo_context;

  cairo_surface = cairo_image_surface_create_for_data (NULL, CAIRO_FORMAT_ARGB32, 0, 0, 0);
  cairo_context = cairo_create (cairo_surface);
  cairo_surface_destroy (cairo_surface);

  return cairo_context;
}

static PangoLayout*
init_pango_text_layout (cairo_t *cairo_context,
			char *text,
                        char *font_description,
                        PangoAlignment alignment,
                        long width)
{
  PangoLayout          *pango_layout;
  PangoFontDescription *description;

  pango_layout = pango_cairo_create_layout (cairo_context);

  if (!font_description)
    description = pango_font_description_from_string ("Sans 12");
  else
    description = pango_font_description_from_string (font_description);

  pango_layout_set_font_description (pango_layout, description);
  pango_font_description_free (description);

  pango_layout_set_alignment(pango_layout, alignment);
  if (width >= 0)
    pango_layout_set_width(pango_layout, width * PANGO_SCALE);

  pango_layout_set_text (pango_layout, text, -1);
  pango_cairo_update_layout (cairo_context, pango_layout);

  return pango_layout;
}

static void
size_control (ply_label_plugin_control_t *label)
{
  cairo_t              *cairo_context;
  PangoLayout          *pango_layout;
  int                   text_width;
  int                   text_height;

  if (label->is_hidden)
    return;

  cairo_context = get_cairo_context_for_sizing (label);

  pango_layout = init_pango_text_layout(cairo_context, label->text, label->fontdesc, label->alignment, label->width);

  pango_layout_get_size (pango_layout, &text_width, &text_height);
  label->area.width = (long) ((double) text_width / PANGO_SCALE);
  label->area.height = (long) ((double) text_height / PANGO_SCALE);

  g_object_unref (pango_layout);
  cairo_destroy (cairo_context);
}

static void
draw_control (ply_label_plugin_control_t *label,
              ply_pixel_buffer_t         *pixel_buffer,
              long                        x,
              long                        y,
              unsigned long               width,
              unsigned long               height)
{
  cairo_t              *cairo_context;
  PangoLayout          *pango_layout;
  int                   text_width;
  int                   text_height;

  if (label->is_hidden)
    return;

  cairo_context = get_cairo_context_for_pixel_buffer (label, pixel_buffer);

  pango_layout = init_pango_text_layout(cairo_context, label->text, label->fontdesc, label->alignment, label->width);

  pango_layout_get_size (pango_layout, &text_width, &text_height);
  label->area.width = (long) ((double) text_width / PANGO_SCALE);
  label->area.height = (long) ((double) text_height / PANGO_SCALE);

  cairo_rectangle (cairo_context, x, y, width, height);
  cairo_clip (cairo_context);
  cairo_move_to (cairo_context,
                 label->area.x,
                 label->area.y);
  cairo_set_source_rgba (cairo_context,
                         label->red,
                         label->green,
                         label->blue,
                         label->alpha);
  pango_cairo_show_layout (cairo_context,
                           pango_layout);

  g_object_unref (pango_layout);
  cairo_destroy (cairo_context);
}

static void
set_alignment_for_control (ply_label_plugin_control_t *label,
                           ply_label_alignment_t alignment)
{
  ply_rectangle_t dirty_area;
  PangoAlignment pango_alignment;

  switch(alignment)
    {
    case PLY_LABEL_ALIGN_CENTER:
      pango_alignment = PANGO_ALIGN_CENTER;
      break;
    case PLY_LABEL_ALIGN_RIGHT:
      pango_alignment = PANGO_ALIGN_RIGHT;
      break;
    case PLY_LABEL_ALIGN_LEFT:
    default:
      pango_alignment = PANGO_ALIGN_LEFT;
      break;
    }

  if (label->alignment != pango_alignment)
    {
      dirty_area = label->area;
      label->alignment = pango_alignment;
      size_control (label);
      if (!label->is_hidden && label->display != NULL)
        ply_pixel_display_draw_area (label->display,
                                     dirty_area.x, dirty_area.y,
                                     dirty_area.width, dirty_area.height);

    }
}

static void
set_width_for_control (ply_label_plugin_control_t *label,
                       long                        width)
{
  ply_rectangle_t dirty_area;

  if (label->width != width)
    {
      dirty_area = label->area;
      label->width = width;
      size_control (label);
      if (!label->is_hidden && label->display != NULL)
        ply_pixel_display_draw_area (label->display,
                                     dirty_area.x, dirty_area.y,
                                     dirty_area.width, dirty_area.height);

    }
}

static void
set_text_for_control (ply_label_plugin_control_t *label,
                      const char                 *text)
{
  ply_rectangle_t dirty_area;

  if (label->text != text)
    {
      dirty_area = label->area;
      free (label->text);
      label->text = strdup (text);
      size_control (label);
      if (!label->is_hidden && label->display != NULL)
        ply_pixel_display_draw_area (label->display,
                                     dirty_area.x, dirty_area.y,
                                     dirty_area.width, dirty_area.height);

    }
}

static void
set_font_for_control (ply_label_plugin_control_t *label,
                      const char                 *fontdesc)
{
  ply_rectangle_t dirty_area;

  if (label->fontdesc != fontdesc)
    {
      dirty_area = label->area;
      free (label->fontdesc);
      if (fontdesc)
        label->fontdesc = strdup (fontdesc);
      else
        label->fontdesc = NULL;
      size_control (label);
      if (!label->is_hidden && label->display != NULL)
        ply_pixel_display_draw_area (label->display,
                                     dirty_area.x, dirty_area.y,
                                     dirty_area.width, dirty_area.height);

    }
}

static void
set_color_for_control (ply_label_plugin_control_t *label,
                       float                       red,
                       float                       green,
                       float                       blue,
                       float                       alpha)
{
  label->red = red;
  label->green = green;
  label->blue = blue;
  label->alpha = alpha;

  if (!label->is_hidden && label->display != NULL)
    ply_pixel_display_draw_area (label->display,
                                 label->area.x, label->area.y,
                                 label->area.width, label->area.height);
}

static bool
show_control (ply_label_plugin_control_t *label,
              ply_pixel_display_t        *display,
              long                        x,
              long                        y)
{
  ply_rectangle_t dirty_area;

  dirty_area = label->area;
  label->display = display;
  label->area.x = x;
  label->area.y = y;

  label->is_hidden = false;

  size_control (label);

  if (!label->is_hidden && label->display != NULL)
    ply_pixel_display_draw_area (label->display,
                                 dirty_area.x, dirty_area.y,
                                 dirty_area.width, dirty_area.height);

  label->is_hidden = false;

  return true;
}

static void
hide_control (ply_label_plugin_control_t *label)
{
  label->is_hidden = true;
  if (label->display != NULL)
    ply_pixel_display_draw_area (label->display,
                                 label->area.x, label->area.y,
                                 label->area.width, label->area.height);

  label->display = NULL;
  label->loop = NULL;
}

static bool
is_control_hidden (ply_label_plugin_control_t *label)
{
  return label->is_hidden;
}

ply_label_plugin_interface_t *
ply_label_plugin_get_interface (void)
{
  static ply_label_plugin_interface_t plugin_interface =
    {
      .create_control = create_control,
      .destroy_control = destroy_control,
      .show_control = show_control,
      .hide_control = hide_control,
      .draw_control = draw_control,
      .is_control_hidden = is_control_hidden,
      .set_text_for_control = set_text_for_control,
      .set_alignment_for_control = set_alignment_for_control,
      .set_width_for_control = set_width_for_control,
      .set_font_for_control = set_font_for_control,
      .set_color_for_control = set_color_for_control,
      .get_width_of_control = get_width_of_control,
      .get_height_of_control = get_height_of_control
    };

  return &plugin_interface;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
