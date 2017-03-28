/* ply-boot-splash.h - APIs for putting up a splash screen
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
 * Written by: Ray Strode <rstrode@redhat.com>
 *             Soeren Sandmann <sandmann@redhat.com>
 */
#include "config.h"
#include "ply-boot-splash.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

#include "ply-boot-splash-plugin.h"
#include "ply-terminal.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-trigger.h"
#include "ply-utils.h"
#include "ply-progress.h"
#include "ply-keyboard.h"
#include "ply-key-file.h"

#ifndef UPDATES_PER_SECOND
#define UPDATES_PER_SECOND 30
#endif

#define KEY_CTRL_L ('\100' ^'L')
#define KEY_CTRL_T ('\100' ^'T')
#define KEY_CTRL_V ('\100' ^'V')

struct _ply_boot_splash
{
  ply_event_loop_t *loop;
  ply_module_handle_t *module_handle;
  const ply_boot_splash_plugin_interface_t *plugin_interface;
  ply_boot_splash_plugin_t *plugin;
  ply_boot_splash_mode_t mode;
  ply_keyboard_t *keyboard;
  ply_buffer_t *boot_buffer;
  ply_trigger_t *idle_trigger;
  ply_list_t *pixel_displays;
  ply_list_t *text_displays;

  char *theme_path;
  char *plugin_dir;
  char *status;

  ply_progress_t *progress;
  ply_boot_splash_on_idle_handler_t idle_handler;
  void *idle_handler_user_data;

  uint32_t is_loaded : 1;
  uint32_t should_force_text_mode : 1;
};

typedef const ply_boot_splash_plugin_interface_t *
        (* get_plugin_interface_function_t) (void);

static void ply_boot_splash_update_progress (ply_boot_splash_t *splash);
static void ply_boot_splash_detach_from_event_loop (ply_boot_splash_t *splash);

ply_boot_splash_t *
ply_boot_splash_new (const char     *theme_path,
                     const char     *plugin_dir,
                     ply_buffer_t   *boot_buffer)
{
  ply_boot_splash_t *splash;

  assert (theme_path != NULL);

  splash = calloc (1, sizeof (ply_boot_splash_t));
  splash->loop = NULL;
  splash->theme_path = strdup (theme_path);
  splash->plugin_dir = strdup (plugin_dir);
  splash->module_handle = NULL;
  splash->mode = PLY_BOOT_SPLASH_MODE_INVALID;

  splash->boot_buffer = boot_buffer;
  splash->pixel_displays = ply_list_new ();
  splash->text_displays = ply_list_new ();

  return splash;
}

static void
refresh_displays (ply_boot_splash_t *splash)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (splash->pixel_displays);
  while (node != NULL)
    {
      ply_pixel_display_t *display;
      ply_list_node_t *next_node;
      unsigned long width, height;

      display = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (splash->pixel_displays, node);

      width = ply_pixel_display_get_width (display);
      height = ply_pixel_display_get_height (display);

      ply_pixel_display_draw_area (display, 0, 0, width, height);
      node = next_node;
    }

  node = ply_list_get_first_node (splash->text_displays);
  while (node != NULL)
    {
      ply_text_display_t *display;
      ply_list_node_t *next_node;
      int number_of_columns, number_of_rows;

      display = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (splash->text_displays, node);

      number_of_columns = ply_text_display_get_number_of_columns (display);
      number_of_rows = ply_text_display_get_number_of_rows (display);

      ply_text_display_draw_area (display, 0, 0,
                                  number_of_columns,
                                  number_of_rows);
      node = next_node;
    }
}

static ply_terminal_t *
find_local_console_terminal (ply_boot_splash_t *splash)
{
  ply_list_node_t *node;
  node = ply_list_get_first_node (splash->text_displays);

  while (node != NULL)
    {
      ply_text_display_t *display;
      ply_terminal_t *terminal;
      ply_list_node_t *next_node;

      display = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (splash->text_displays, node);

      terminal = ply_text_display_get_terminal (display);

      if (terminal != NULL && ply_terminal_is_vt (terminal))
        return terminal;

      node = next_node;
    }

  return NULL;
}

static void
on_keyboard_input (ply_boot_splash_t *splash,
                   const char        *keyboard_input,
                   size_t             character_size)
{
  wchar_t key;

  if ((ssize_t) mbrtowc (&key, keyboard_input, character_size, NULL) > 0)
    {
      switch (key)
        {
          case KEY_CTRL_L:
            refresh_displays (splash);
          return;

          case KEY_CTRL_T:
            ply_trace ("toggle text mode!");
            splash->should_force_text_mode = !splash->should_force_text_mode;

            if (ply_list_get_length (splash->pixel_displays) >= 1)
              {
                ply_terminal_t *terminal;

                terminal = find_local_console_terminal (splash);

                if (terminal != NULL)
                  {
                    if (splash->should_force_text_mode)
                      {
                        ply_terminal_set_mode (terminal, PLY_TERMINAL_MODE_TEXT);
                        ply_terminal_ignore_mode_changes (terminal, true);
                      }
                    else
                      ply_terminal_ignore_mode_changes (terminal, false);
                  }
              }
            ply_trace ("text mode toggled!");
          return;

          case KEY_CTRL_V:
            ply_trace ("toggle verbose mode!");
            ply_toggle_tracing ();
            ply_trace ("verbose mode toggled!");
          return;
        }
    }
}

void
ply_boot_splash_set_keyboard (ply_boot_splash_t *splash,
                              ply_keyboard_t    *keyboard)
{
  splash->keyboard = keyboard;

  ply_keyboard_add_input_handler (keyboard,
                                  (ply_keyboard_input_handler_t)
                                  on_keyboard_input, splash);

  if (splash->plugin_interface->set_keyboard == NULL)
    return;

  splash->plugin_interface->set_keyboard (splash->plugin, keyboard);
}

void
ply_boot_splash_unset_keyboard (ply_boot_splash_t *splash)
{
  ply_keyboard_remove_input_handler (splash->keyboard,
                                     (ply_keyboard_input_handler_t)
                                     on_keyboard_input);

  if (splash->plugin_interface->set_keyboard == NULL)
    return;

  splash->plugin_interface->unset_keyboard (splash->plugin, splash->keyboard);
}

void
ply_boot_splash_add_pixel_display (ply_boot_splash_t   *splash,
                                   ply_pixel_display_t *display)
{
  ply_list_append_data (splash->pixel_displays, display);

  if (splash->plugin_interface->add_pixel_display == NULL)
    return;

  splash->plugin_interface->add_pixel_display (splash->plugin, display);
}

void
ply_boot_splash_remove_pixel_display (ply_boot_splash_t   *splash,
                                      ply_pixel_display_t *display)
{
  ply_list_remove_data (splash->pixel_displays, display);

  if (splash->plugin_interface->remove_pixel_display == NULL)
    return;

  splash->plugin_interface->remove_pixel_display (splash->plugin, display);
}

void
ply_boot_splash_add_text_display (ply_boot_splash_t   *splash,
                                  ply_text_display_t *display)
{
  ply_list_append_data (splash->text_displays, display);

  if (splash->plugin_interface->add_text_display == NULL)
    return;

  splash->plugin_interface->add_text_display (splash->plugin, display);
}

void
ply_boot_splash_remove_text_display (ply_boot_splash_t   *splash,
                                     ply_text_display_t *display)
{
  ply_list_remove_data (splash->text_displays, display);

  if (splash->plugin_interface->remove_pixel_display == NULL)
    return;

  splash->plugin_interface->remove_text_display (splash->plugin, display);
}

bool
ply_boot_splash_load (ply_boot_splash_t *splash)
{
  ply_key_file_t *key_file;
  char *module_name;
  char *module_path;

  assert (splash != NULL);

  get_plugin_interface_function_t get_boot_splash_plugin_interface;

  key_file = ply_key_file_new (splash->theme_path);

  if (!ply_key_file_load (key_file))
    return false;

  module_name = ply_key_file_get_value (key_file, "Plymouth Theme", "ModuleName");

  asprintf (&module_path, "%s%s.so",
            splash->plugin_dir, module_name);
  free (module_name);

  splash->module_handle = ply_open_module (module_path);

  free (module_path);

  if (splash->module_handle == NULL)
    {
      ply_key_file_free (key_file);
      return false;
    }

  get_boot_splash_plugin_interface = (get_plugin_interface_function_t)
      ply_module_look_up_function (splash->module_handle,
                                   "ply_boot_splash_plugin_get_interface");

  if (get_boot_splash_plugin_interface == NULL)
    {
      ply_save_errno ();
      ply_close_module (splash->module_handle);
      splash->module_handle = NULL;
      ply_key_file_free (key_file);
      ply_restore_errno ();
      return false;
    }

  splash->plugin_interface = get_boot_splash_plugin_interface ();

  if (splash->plugin_interface == NULL)
    {
      ply_save_errno ();
      ply_close_module (splash->module_handle);
      splash->module_handle = NULL;
      ply_key_file_free (key_file);
      ply_restore_errno ();
      return false;
    }

  splash->plugin = splash->plugin_interface->create_plugin (key_file);

  ply_key_file_free (key_file);

  assert (splash->plugin != NULL);

  splash->is_loaded = true;

  return true;
}

bool
ply_boot_splash_load_built_in (ply_boot_splash_t *splash)
{
  get_plugin_interface_function_t get_boot_splash_plugin_interface;

  assert (splash != NULL);

  splash->module_handle = ply_open_built_in_module ();

  if (splash->module_handle == NULL)
    return false;

  get_boot_splash_plugin_interface = (get_plugin_interface_function_t)
      ply_module_look_up_function (splash->module_handle,
                                   "ply_boot_splash_plugin_get_interface");

  if (get_boot_splash_plugin_interface == NULL)
    {
      ply_save_errno ();
      ply_close_module (splash->module_handle);
      splash->module_handle = NULL;
      ply_restore_errno ();
      return false;
    }

  splash->plugin_interface = get_boot_splash_plugin_interface ();

  if (splash->plugin_interface == NULL)
    {
      ply_save_errno ();
      ply_close_module (splash->module_handle);
      splash->module_handle = NULL;
      ply_restore_errno ();
      return false;
    }

  splash->plugin = splash->plugin_interface->create_plugin (NULL);

  assert (splash->plugin != NULL);

  splash->is_loaded = true;

  return true;
}

void
ply_boot_splash_unload (ply_boot_splash_t *splash)
{
  assert (splash != NULL);
  assert (splash->plugin != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->module_handle != NULL);

  splash->plugin_interface->destroy_plugin (splash->plugin);
  splash->plugin = NULL;

  ply_close_module (splash->module_handle);
  splash->plugin_interface = NULL;
  splash->module_handle = NULL;

  splash->is_loaded = false;
}

static void
remove_displays (ply_boot_splash_t *splash)
{
  ply_list_node_t *node, *next_node;

  ply_trace ("removing pixel displays");

  node = ply_list_get_first_node (splash->pixel_displays);
  while (node != NULL)
    {
      ply_pixel_display_t *display;
      ply_list_node_t *next_node;
      unsigned long width, height;

      display = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (splash->pixel_displays, node);

      width = ply_pixel_display_get_width (display);
      height = ply_pixel_display_get_height (display);

      ply_trace ("Removing %lux%lu pixel display", width, height);

      if (splash->plugin_interface->remove_pixel_display != NULL)
        splash->plugin_interface->remove_pixel_display (splash->plugin, display);

      ply_trace ("Removing node");
      ply_list_remove_node (splash->pixel_displays, node);

      node = next_node;
    }

  ply_trace ("removing text displays");
  node = ply_list_get_first_node (splash->text_displays);
  while (node != NULL)
    {
      ply_text_display_t *display;
      int number_of_columns, number_of_rows;

      display = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (splash->text_displays, node);

      number_of_columns = ply_text_display_get_number_of_columns (display);
      number_of_rows = ply_text_display_get_number_of_rows (display);

      ply_trace ("Removing %dx%d text display", number_of_columns, number_of_rows);

      if (splash->plugin_interface->remove_text_display != NULL)
        splash->plugin_interface->remove_text_display (splash->plugin, display);

      ply_trace ("Removing node");
      ply_list_remove_node (splash->text_displays, node);

      node = next_node;
    }
}

void
ply_boot_splash_free (ply_boot_splash_t *splash)
{
  ply_trace ("freeing splash");
  if (splash == NULL)
    return;

  if (splash->loop != NULL)
    {
      if (splash->plugin_interface->on_boot_progress != NULL)
        {
          ply_event_loop_stop_watching_for_timeout (splash->loop,
                                                    (ply_event_loop_timeout_handler_t)
                                                    ply_boot_splash_update_progress, splash);
        }

      ply_event_loop_stop_watching_for_exit (splash->loop, (ply_event_loop_exit_handler_t)
                                             ply_boot_splash_detach_from_event_loop,
                                             splash);
    }

  remove_displays (splash);
  ply_list_free (splash->pixel_displays);
  ply_list_free (splash->text_displays);

  if (splash->module_handle != NULL)
    ply_boot_splash_unload (splash);

  if (splash->idle_trigger != NULL)
    ply_trigger_free (splash->idle_trigger);

  free (splash->theme_path);
  free (splash->plugin_dir);
  free (splash);
}

static void
ply_boot_splash_update_progress (ply_boot_splash_t *splash)
{
  double percentage=0.0;
  double time=0.0;

  assert (splash != NULL);

  if (splash->progress)
    {
      percentage = ply_progress_get_percentage(splash->progress);
      time = ply_progress_get_time(splash->progress);
    }

  if (splash->plugin_interface->on_boot_progress != NULL)
    splash->plugin_interface->on_boot_progress (splash->plugin,
                                                time,
                                                percentage);

  ply_event_loop_watch_for_timeout (splash->loop,
                                   1.0 / UPDATES_PER_SECOND,
                                   (ply_event_loop_timeout_handler_t)
                                   ply_boot_splash_update_progress, splash);
}

void
ply_boot_splash_attach_progress (ply_boot_splash_t *splash,
                                      ply_progress_t    *progress)
{
  assert (splash != NULL);
  assert (progress != NULL);
  assert (splash->progress == NULL);
  splash->progress = progress;
}


bool
ply_boot_splash_show (ply_boot_splash_t *splash,
                      ply_boot_splash_mode_t mode)
{
  assert (splash != NULL);
  assert (mode != PLY_BOOT_SPLASH_MODE_INVALID);
  assert (splash->module_handle != NULL);
  assert (splash->loop != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  assert (splash->plugin_interface->show_splash_screen != NULL);

  if (splash->mode == mode)
    {
      ply_trace ("already set same splash screen mode");
      return true;
    }
  else if (splash->mode != PLY_BOOT_SPLASH_MODE_INVALID)
    {
      splash->plugin_interface->hide_splash_screen (splash->plugin,
                                                    splash->loop);
    }

  ply_trace ("showing splash screen");
  if (!splash->plugin_interface->show_splash_screen (splash->plugin,
                                                     splash->loop,
                                                     splash->boot_buffer,
                                                     mode))
    {

      ply_save_errno ();
      ply_trace ("can't show splash: %m");
      ply_restore_errno ();
      return false;
    }

  if (splash->plugin_interface->on_boot_progress != NULL)
    {
      ply_boot_splash_update_progress (splash);
    }

  splash->mode = mode;
  return true;
}

bool
ply_boot_splash_system_update (ply_boot_splash_t *splash,
                               int                progress)
{
  assert (splash != NULL);
  assert (splash->module_handle != NULL);
  assert (splash->loop != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);

  if (splash->plugin_interface->system_update == NULL)
    return false;

  ply_trace ("updating system %i%%", progress);
  splash->plugin_interface->system_update (splash->plugin,
                                           progress);
  return true;
}

void
ply_boot_splash_update_status (ply_boot_splash_t *splash,
                               const char        *status)
{
  assert (splash != NULL);
  assert (status != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  assert (splash->plugin_interface->update_status != NULL);
  assert (splash->mode != PLY_BOOT_SPLASH_MODE_INVALID);

  splash->plugin_interface->update_status (splash->plugin, status);
}

void
ply_boot_splash_update_output (ply_boot_splash_t *splash,
                               const char        *output,
                               size_t             size)
{
  assert (splash != NULL);
  assert (output != NULL);

  if (splash->plugin_interface->on_boot_output != NULL)
    splash->plugin_interface->on_boot_output (splash->plugin, output, size);
}

void
ply_boot_splash_root_mounted (ply_boot_splash_t *splash)
{
  assert (splash != NULL);

  if (splash->plugin_interface->on_root_mounted != NULL)
    splash->plugin_interface->on_root_mounted (splash->plugin);
}

static void
ply_boot_splash_detach_from_event_loop (ply_boot_splash_t *splash)
{
  assert (splash != NULL);
  splash->loop = NULL;
}

void
ply_boot_splash_hide (ply_boot_splash_t *splash)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  assert (splash->plugin_interface->hide_splash_screen != NULL);

  splash->plugin_interface->hide_splash_screen (splash->plugin,
                                                splash->loop);

  if (ply_list_get_length (splash->pixel_displays) >= 1)
    {
      ply_terminal_t *terminal;

      terminal = find_local_console_terminal (splash);

      if (terminal != NULL)
        ply_terminal_set_mode (terminal, PLY_TERMINAL_MODE_TEXT);
    }

  splash->mode = PLY_BOOT_SPLASH_MODE_INVALID;

  if (splash->loop != NULL)
    {
      if (splash->plugin_interface->on_boot_progress != NULL)
        {
          ply_event_loop_stop_watching_for_timeout (splash->loop,
                                                    (ply_event_loop_timeout_handler_t)
                                                    ply_boot_splash_update_progress, splash);
        }

      ply_event_loop_stop_watching_for_exit (splash->loop, (ply_event_loop_exit_handler_t)
                                             ply_boot_splash_detach_from_event_loop,
                                             splash);
    }
}

void ply_boot_splash_display_message (ply_boot_splash_t             *splash,
                                      const char                    *message)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);

  if (splash->plugin_interface->display_message != NULL)
    splash->plugin_interface->display_message (splash->plugin, message);
}

void ply_boot_splash_hide_message (ply_boot_splash_t             *splash,
                                      const char                 *message)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  if (splash->plugin_interface->hide_message != NULL)
    splash->plugin_interface->hide_message (splash->plugin, message);
}

void ply_boot_splash_display_normal  (ply_boot_splash_t              *splash)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  if (splash->plugin_interface->display_normal != NULL)
    splash->plugin_interface->display_normal (splash->plugin);
}

void ply_boot_splash_display_password (ply_boot_splash_t             *splash,
                                       const char                    *prompt,
                                       int                            bullets)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  if (splash->plugin_interface->display_password != NULL)
    splash->plugin_interface->display_password (splash->plugin, prompt, bullets);
}

void ply_boot_splash_display_question (ply_boot_splash_t             *splash,
                                       const char                    *prompt,
                                       const char                    *entry_text)
{
  assert (splash != NULL);
  assert (splash->plugin_interface != NULL);
  assert (splash->plugin != NULL);
  if (splash->plugin_interface->display_question != NULL)
    splash->plugin_interface->display_question (splash->plugin, prompt, entry_text);
}



void
ply_boot_splash_attach_to_event_loop (ply_boot_splash_t *splash,
                                      ply_event_loop_t  *loop)
{
  assert (splash != NULL);
  assert (loop != NULL);
  assert (splash->loop == NULL);

  splash->loop = loop;

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t) 
                                 ply_boot_splash_detach_from_event_loop,
                                 splash); 
}

static void
on_idle (ply_boot_splash_t *splash)
{

  ply_trace ("splash now idle");
  ply_event_loop_watch_for_timeout (splash->loop, 0.01,
                                    (ply_event_loop_timeout_handler_t)
                                    splash->idle_handler,
                                    splash->idle_handler_user_data);
  splash->idle_handler = NULL;
  splash->idle_handler_user_data = NULL;
}

void
ply_boot_splash_become_idle (ply_boot_splash_t                  *splash,
                             ply_boot_splash_on_idle_handler_t  idle_handler,
                             void                              *user_data)
{
  assert (splash->idle_trigger == NULL);

  if (splash->progress != NULL)
    {
      ply_progress_set_percentage (splash->progress, 1.0);
      ply_boot_splash_update_progress (splash);
    }

  ply_trace ("telling splash to become idle");
  if (splash->plugin_interface->become_idle == NULL)
    {
      ply_event_loop_watch_for_timeout (splash->loop, 0.01,
                                        (ply_event_loop_timeout_handler_t)
                                        idle_handler,
                                        user_data);

      return;
    }

  splash->idle_handler = idle_handler;
  splash->idle_handler_user_data = user_data;

  splash->idle_trigger = ply_trigger_new (&splash->idle_trigger);
  ply_trigger_add_handler (splash->idle_trigger,
                           (ply_trigger_handler_t) on_idle,
                           splash);

  splash->plugin_interface->become_idle (splash->plugin, splash->idle_trigger);
}

#ifdef PLY_BOOT_SPLASH_ENABLE_TEST

#include <stdio.h>

#include "ply-event-loop.h"
#include "ply-boot-splash.h"

typedef struct test_state test_state_t;
struct test_state {
  ply_event_loop_t *loop;
  ply_boot_splash_t *splash;
  ply_buffer_t *buffer;
};

static void
on_timeout (ply_boot_splash_t *splash)
{
  ply_boot_splash_update_status (splash, "foo");
  ply_event_loop_watch_for_timeout (splash->loop, 
                                    5.0,
                                   (ply_event_loop_timeout_handler_t)
                                   on_timeout,
                                   splash);
}

static void
on_quit (test_state_t *state)
{
    ply_boot_splash_hide (state->splash);
    ply_event_loop_exit (state->loop, 0);
}

static void
add_displays_to_splash_from_renderer (test_state_t   *state,
                                      ply_renderer_t *renderer)
{
  ply_list_t *heads;
  ply_list_node_t *node;

  heads = ply_renderer_get_heads (renderer);

  node = ply_list_get_first_node (heads);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_renderer_head_t *head;
      ply_pixel_display_t *display;

      head = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (heads, node);

      display = ply_pixel_display_new (renderer, head);

      ply_boot_splash_add_pixel_display (state->splash, display);

      node = next_node;
    }
}

int
main (int    argc,
      char **argv)
{
  int exit_code;
  test_state_t state;
  char *tty_name;
  const char *theme_path;
  ply_text_display_t *text_display;
  ply_renderer_t *renderer;
  ply_terminal_t *terminal;
  ply_keyboard_t *keyboard;

  exit_code = 0;

  state.loop = ply_event_loop_new ();

  if (argc > 1)
    theme_path = argv[1];
  else
    theme_path = PLYMOUTH_THEME_PATH "/fade-in/fade-in.plymouth";

  if (argc > 2)
    asprintf(&tty_name, "tty%s", argv[2]);
  else
    tty_name = strdup("tty0");

  terminal = ply_terminal_new (tty_name);

  if (!ply_terminal_open (terminal))
    {
      perror ("could not open tty");
      return errno;
    }

  renderer = ply_renderer_new (NULL, terminal);
  free(tty_name);

  if (!ply_renderer_open (renderer))
    {
      perror ("could not open renderer /dev/fb");
      ply_renderer_free (renderer);
      return errno;
    }

  keyboard = ply_keyboard_new_for_renderer (renderer);
  ply_keyboard_add_escape_handler (keyboard,
                                   (ply_keyboard_escape_handler_t) on_quit, &state);

  state.buffer = ply_buffer_new ();
  state.splash = ply_boot_splash_new (theme_path, PLYMOUTH_PLUGIN_PATH, state.buffer);

  if (!ply_boot_splash_load (state.splash))
    {
      perror ("could not load splash screen");
      return errno;
    }

  ply_boot_splash_set_keyboard (state.splash, keyboard);
  add_displays_to_splash_from_renderer (&state, renderer);

  text_display = ply_text_display_new (terminal);
  ply_boot_splash_add_text_display (state.splash, text_display);

  ply_boot_splash_attach_to_event_loop (state.splash, state.loop);

  if (!ply_boot_splash_show (state.splash, PLY_BOOT_SPLASH_MODE_BOOT_UP))
    {
      perror ("could not show splash screen");
      return errno;
    }

  ply_event_loop_watch_for_timeout (state.loop, 
                                    1.0,
                                   (ply_event_loop_timeout_handler_t)
                                   on_timeout,
                                   state.splash);
  exit_code = ply_event_loop_run (state.loop);
  ply_boot_splash_free (state.splash);
  ply_buffer_free (state.buffer);

  return exit_code;
}

#endif /* PLY_BOOT_SPLASH_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
