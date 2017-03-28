/* plugin.c - frame-backend renderer plugin
 *
 * Copyright (C) 2006-2009 Red Hat, Inc.
 *               2009 Charlie Brej <cbrej@cs.man.ac.uk>
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
#include <sys/ipc.h>
#include <sys/shm.h>

#include <values.h>
#include <unistd.h>

#include <linux/fb.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gdk/gdkx.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-rectangle.h"
#include "ply-region.h"

#include "ply-renderer.h"
#include "ply-renderer-plugin.h"

struct _ply_renderer_head
{
  ply_renderer_backend_t *backend;
  ply_pixel_buffer_t     *pixel_buffer;
  ply_rectangle_t         area;
  GtkWidget              *window;
  GdkPixmap              *pixmap;
  cairo_surface_t        *image;
  uint32_t                is_fullscreen : 1;
};

struct _ply_renderer_input_source
{
  ply_buffer_t                        *key_buffer;
  ply_renderer_input_source_handler_t  handler;
  void                                *user_data;
};

struct _ply_renderer_backend
{
  ply_event_loop_t            *loop;
  ply_renderer_input_source_t  input_source;
  ply_list_t                  *heads;

  ply_fd_watch_t *display_watch;

  uint32_t is_active : 1;
};

ply_renderer_plugin_interface_t *ply_renderer_backend_get_interface (void);
static void ply_renderer_head_redraw (ply_renderer_backend_t *backend,
                                      ply_renderer_head_t    *head);

static gboolean on_motion_notify_event (GtkWidget      *widget,
                                        GdkEventMotion *event,
                                        gpointer        user_data);
static gboolean on_key_event (GtkWidget   *widget,
                              GdkEventKey *event,
                              gpointer     user_data);

static ply_renderer_backend_t *
create_backend (const char     *device_name,
                ply_terminal_t *terminal)
{
  ply_renderer_backend_t *backend;

  backend = calloc (1, sizeof (ply_renderer_backend_t));

  backend->loop = ply_event_loop_get_default ();
  backend->heads = ply_list_new ();
  backend->input_source.key_buffer = ply_buffer_new ();

  return backend;
}

static void
destroy_backend (ply_renderer_backend_t *backend)
{
  ply_list_node_t *node;
  node = ply_list_get_first_node (backend->heads);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_renderer_head_t *head;

      head = (ply_renderer_head_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (backend->heads, node);

      free (head);
      node = next_node;
    }

  ply_list_free (backend->heads);
  ply_buffer_free (backend->input_source.key_buffer);
  free (backend);
}

static void
on_display_event (ply_renderer_backend_t *backend)
{
  while (gtk_events_pending ())
    gtk_main_iteration ();
}

static bool
open_device (ply_renderer_backend_t *backend)
{
  Display *display;
  int display_fd;

  if (!gtk_init_check (0, NULL))
    return false;

  display = GDK_DISPLAY_XDISPLAY (gdk_display_get_default ());
  display_fd = ConnectionNumber (display);
  backend->display_watch = ply_event_loop_watch_fd (backend->loop,
                                                    display_fd,
                                                    PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                                                    (ply_event_handler_t) on_display_event,
                                                    NULL,
                                                    backend);

  return true;
}

static void
close_device (ply_renderer_backend_t *backend)
{
  ply_event_loop_stop_watching_fd (backend->loop, backend->display_watch);
  backend->display_watch = NULL;
  return;
}

static void
create_fake_multi_head_setup (ply_renderer_backend_t *backend)
{
  ply_renderer_head_t *head;

  head = calloc (1, sizeof (ply_renderer_head_t));

  head->backend = backend;
  head->area.x = 0;
  head->area.y = 0;
  head->area.width = 800;         /* FIXME hardcoded */
  head->area.height = 600;
  head->pixmap = gdk_pixmap_new (NULL,
                                 head->area.width,
                                 head->area.height,
                                 24);
  head->pixel_buffer = ply_pixel_buffer_new (head->area.width, head->area.height);

  ply_list_append_data (backend->heads, head);

  head = calloc (1, sizeof (ply_renderer_head_t));

  head->backend = backend;
  head->area.x = 800;
  head->area.y = 0;
  head->area.width = 640;         /* FIXME hardcoded */
  head->area.height = 480;
  head->pixmap = gdk_pixmap_new (NULL,
                                 head->area.width,
                                 head->area.height,
                                 24);
  head->pixel_buffer = ply_pixel_buffer_new (head->area.width, head->area.height);

  ply_list_append_data (backend->heads, head);
}

static void
create_fullscreen_single_head_setup (ply_renderer_backend_t *backend)
{
  ply_renderer_head_t *head;
  GdkRectangle         monitor_geometry;

  gdk_screen_get_monitor_geometry (gdk_screen_get_default (), 0, &monitor_geometry);

  head = calloc (1, sizeof (ply_renderer_head_t));

  head->backend = backend;
  head->area.x = monitor_geometry.x;
  head->area.y = monitor_geometry.y;
  head->area.width = monitor_geometry.width;
  head->area.height = monitor_geometry.height;
  head->is_fullscreen = true;
  head->pixmap = gdk_pixmap_new (NULL,
                                 head->area.width,
                                 head->area.height,
                                 24);
  head->pixel_buffer = ply_pixel_buffer_new (head->area.width, head->area.height);

  ply_list_append_data (backend->heads, head);
}

static bool
query_device (ply_renderer_backend_t *backend)
{
  assert (backend != NULL);

  if (ply_list_get_first_node (backend->heads) == NULL)
    {
      if (getenv ("PLY_CREATE_FAKE_MULTI_HEAD_SETUP") != NULL)
        create_fake_multi_head_setup (backend);
      else
        create_fullscreen_single_head_setup (backend);
    }

  return true;
}

static gboolean
on_window_destroy (GtkWidget *widget,
                   GdkEvent  *event,
                   gpointer   user_data)
{
  return TRUE;
}

static bool
map_to_device (ply_renderer_backend_t *backend)
{
  ply_list_node_t *node;
  assert (backend != NULL);

  node = ply_list_get_first_node (backend->heads);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_renderer_head_t *head;
      uint32_t *shadow_buffer;

      head = (ply_renderer_head_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (backend->heads, node);

      if (head->window == NULL)
        {
          head->window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
          gtk_window_set_resizable (GTK_WINDOW (head->window), FALSE);
          gtk_widget_set_size_request (head->window,
                                       head->area.width,
                                       head->area.height);
          shadow_buffer = ply_pixel_buffer_get_argb32_data (head->pixel_buffer);
          head->image = cairo_image_surface_create_for_data ((unsigned char *) shadow_buffer,
                                                             CAIRO_FORMAT_ARGB32,
                                                             head->area.width, head->area.height,
                                                             head->area.width * 4);
          gtk_widget_set_app_paintable (head->window, TRUE);
          gtk_widget_show_all (head->window);
          gdk_window_set_back_pixmap (head->window->window, head->pixmap, FALSE);
          gdk_window_set_decorations (head->window->window, GDK_DECOR_BORDER);
          gtk_window_move (GTK_WINDOW (head->window), head->area.x, head->area.y);

          gtk_window_set_type_hint (GTK_WINDOW (head->window), GDK_WINDOW_TYPE_HINT_DOCK);

          if (head->is_fullscreen)
            gtk_window_fullscreen (GTK_WINDOW (head->window));

          gtk_widget_add_events (head->window, GDK_BUTTON1_MOTION_MASK);

          g_signal_connect (head->window, "motion-notify-event",
                            G_CALLBACK (on_motion_notify_event),
                            head);
          g_signal_connect (head->window, "key-press-event",
                            G_CALLBACK (on_key_event),
                            &backend->input_source);
          g_signal_connect (head->window, "delete-event",
                            G_CALLBACK (on_window_destroy),
                            NULL);
        }
      ply_renderer_head_redraw (backend, head);
      node = next_node;
    }

  backend->is_active = true;

  return true;
}

static void
unmap_from_device (ply_renderer_backend_t *backend)
{
  ply_list_node_t *node;
  assert (backend != NULL);

  node = ply_list_get_first_node (backend->heads);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_renderer_head_t *head;

      head = (ply_renderer_head_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (backend->heads, node);

      gtk_widget_destroy (head->window);
      head->window = NULL;
      ply_pixel_buffer_free (head->pixel_buffer);
      head->pixel_buffer = NULL;
      cairo_surface_destroy (head->image);
      head->image = NULL;

      node = next_node;
    }
}

static void
activate (ply_renderer_backend_t *backend)
{
  backend->is_active = true;
}

static void
deactivate (ply_renderer_backend_t *backend)
{
  backend->is_active = false;
}

static void
flush_area_to_device (ply_renderer_backend_t *backend,
                      ply_renderer_head_t    *head,
                      ply_rectangle_t        *area_to_flush,
                      cairo_t                *cr)
{
  cairo_save (cr);
  cairo_rectangle (cr,
                   area_to_flush->x,
                   area_to_flush->y,
                   area_to_flush->width,
                   area_to_flush->height);
  cairo_clip (cr);

  cairo_set_source_surface (cr, head->image, 0, 0);
  cairo_paint (cr);
  cairo_restore (cr);
}

static void
flush_head (ply_renderer_backend_t *backend,
            ply_renderer_head_t    *head)
{
  ply_region_t *updated_region;
  ply_list_t *areas_to_flush;
  ply_list_node_t *node;
  ply_pixel_buffer_t *pixel_buffer;
  cairo_t *cr;

  assert (backend != NULL);

  if (!backend->is_active)
    return;

  pixel_buffer = head->pixel_buffer;
  updated_region = ply_pixel_buffer_get_updated_areas (pixel_buffer);
  areas_to_flush = ply_region_get_sorted_rectangle_list (updated_region);

  cr = gdk_cairo_create (head->pixmap);

  node = ply_list_get_first_node (areas_to_flush);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_rectangle_t *area_to_flush;

      area_to_flush = (ply_rectangle_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (areas_to_flush, node);

      flush_area_to_device (backend, head, area_to_flush, cr);
      gdk_window_clear_area (head->window->window,
                             area_to_flush->x,
                             area_to_flush->y,
                             area_to_flush->width,
                             area_to_flush->height);
      node = next_node;
    }
  ply_region_clear (updated_region);

  cairo_destroy (cr);

  /* Force read-back to make sure plymouth isn't saturating the
   * X server with requests
   */
  g_object_unref (gdk_drawable_get_image (GDK_DRAWABLE (head->pixmap),
                  0, 0, 1, 1));
}

static void
ply_renderer_head_redraw (ply_renderer_backend_t *backend,
                          ply_renderer_head_t    *head)
{
  ply_region_t *region;
  ply_rectangle_t area;

  area.x = 0;
  area.y = 0;
  area.width = head->area.width;
  area.height = head->area.height;

  region = ply_pixel_buffer_get_updated_areas (head->pixel_buffer);

  ply_region_add_rectangle (region, &area);

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
  if (head->backend != backend)
    return NULL;

  return head->pixel_buffer;
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

static gboolean
on_motion_notify_event (GtkWidget      *widget,
                        GdkEventMotion *event,
                        gpointer        user_data)
{
  ply_renderer_head_t *head = user_data;

  gtk_window_begin_move_drag (GTK_WINDOW (head->window), 1,
                              event->x_root, event->y_root, event->time);
  return FALSE;
}

static gboolean
on_key_event (GtkWidget   *widget,
              GdkEventKey *event,
              gpointer     user_data)
{
  ply_renderer_input_source_t *input_source = user_data;

  if (event->keyval == GDK_Return)          /* Enter */
    {
      ply_buffer_append_bytes (input_source->key_buffer, "\n", 1);
    }
  else if (event->keyval == GDK_Escape)     /* Esc */
    {
      ply_buffer_append_bytes (input_source->key_buffer, "\033", 1);
    }
  else if (event->keyval == GDK_BackSpace)  /* Backspace */
    {
      ply_buffer_append_bytes (input_source->key_buffer, "\177", 1);
    }
  else
    {
      gchar bytes[7];
      int byte_count;
      guint32 unichar;
      unichar = gdk_keyval_to_unicode (event->keyval);
      byte_count = g_unichar_to_utf8 (unichar, bytes);
      if (bytes[0] != 0)
        ply_buffer_append_bytes (input_source->key_buffer, bytes, byte_count);
      else
        ply_trace ("unknown GDK key: 0x%X \"%s\"",
                   event->keyval,
                   gdk_keyval_name (event->keyval));
    }

  if (input_source->handler != NULL)
    input_source->handler (input_source->user_data, input_source->key_buffer, input_source);
  return FALSE;
}

static bool
open_input_source (ply_renderer_backend_t      *backend,
                   ply_renderer_input_source_t *input_source)
{

  assert (backend != NULL);
  assert (has_input_source (backend, input_source));

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
