/* ply-keyboard.h - APIs for putting up a keyboard screen
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
#include "ply-keyboard.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-renderer.h"
#include "ply-terminal.h"
#include "ply-utils.h"

#define KEY_CTRL_U ('\100' ^'U')
#define KEY_CTRL_W ('\100' ^'W')
#define KEY_CTRL_V ('\100' ^'V')
#define KEY_ESCAPE ('\100' ^'[')
#define KEY_RETURN '\n'
#define KEY_BACKSPACE '\177'

typedef void (* ply_keyboard_handler_t) (void *);

typedef struct
{
  ply_keyboard_handler_t function;
  void *user_data;
} ply_keyboard_closure_t;

typedef enum
{
  PLY_KEYBOARD_PROVIDER_TYPE_TERMINAL,
  PLY_KEYBOARD_PROVIDER_TYPE_RENDERER
} ply_keyboard_provider_type_t;

typedef struct
{
  ply_terminal_t *terminal;
  ply_buffer_t   *key_buffer;
} ply_keyboard_terminal_provider_t;

typedef struct
{
  ply_renderer_t              *renderer;
  ply_renderer_input_source_t *input_source;
} ply_keyboard_renderer_provider_t;

typedef  union {
  ply_keyboard_renderer_provider_t *if_renderer;
  ply_keyboard_terminal_provider_t *if_terminal;
} ply_keyboard_provider_t;

struct _ply_keyboard
{
  ply_event_loop_t *loop;

  ply_keyboard_provider_type_t provider_type;
  ply_keyboard_provider_t provider;

  ply_buffer_t     *line_buffer;

  ply_list_t *keyboard_input_handler_list;
  ply_list_t *backspace_handler_list;
  ply_list_t *escape_handler_list;
  ply_list_t *enter_handler_list;
};

static bool ply_keyboard_watch_for_terminal_input (ply_keyboard_t *keyboard);

ply_keyboard_t *
ply_keyboard_new_for_terminal (ply_terminal_t *terminal)
{
  ply_keyboard_t *keyboard;

  keyboard = calloc (1, sizeof (ply_keyboard_t));
  keyboard->line_buffer = ply_buffer_new ();
  keyboard->keyboard_input_handler_list = ply_list_new ();
  keyboard->backspace_handler_list = ply_list_new ();
  keyboard->escape_handler_list = ply_list_new ();
  keyboard->enter_handler_list = ply_list_new ();
  keyboard->provider_type = PLY_KEYBOARD_PROVIDER_TYPE_TERMINAL;
  keyboard->provider.if_terminal = calloc (1, sizeof (ply_keyboard_terminal_provider_t));
  keyboard->provider.if_terminal->terminal = terminal;
  keyboard->provider.if_terminal->key_buffer = ply_buffer_new ();

  keyboard->loop = ply_event_loop_get_default ();

  return keyboard;
}

ply_keyboard_t *
ply_keyboard_new_for_renderer (ply_renderer_t *renderer)
{
  ply_keyboard_t *keyboard;
  ply_renderer_input_source_t *input_source;

  keyboard = calloc (1, sizeof (ply_keyboard_t));
  keyboard->line_buffer = ply_buffer_new ();
  keyboard->keyboard_input_handler_list = ply_list_new ();
  keyboard->backspace_handler_list = ply_list_new ();
  keyboard->escape_handler_list = ply_list_new ();
  keyboard->enter_handler_list = ply_list_new ();
  keyboard->provider_type = PLY_KEYBOARD_PROVIDER_TYPE_RENDERER;
  keyboard->provider.if_renderer = calloc (1, sizeof (ply_keyboard_renderer_provider_t));
  keyboard->provider.if_renderer->renderer = renderer;

  input_source = ply_renderer_get_input_source (renderer);

  keyboard->provider.if_renderer->input_source = input_source;

  keyboard->loop = ply_event_loop_get_default ();

  return keyboard;
}

static void
process_backspace (ply_keyboard_t *keyboard)
{
  size_t bytes_to_remove;
  ssize_t previous_character_size;
  const char *bytes;
  size_t size;
  ply_list_node_t *node;

  bytes = ply_buffer_get_bytes (keyboard->line_buffer);
  size = ply_buffer_get_size (keyboard->line_buffer);

  bytes_to_remove = MIN (size, PLY_UTF8_CHARACTER_SIZE_MAX);
  while ((previous_character_size = ply_utf8_character_get_size (bytes + size - bytes_to_remove, bytes_to_remove)) < (ssize_t) bytes_to_remove)
    {
      if (previous_character_size > 0)
        bytes_to_remove -= previous_character_size;
      else
        bytes_to_remove--;
    }

  if (bytes_to_remove <= size)
    ply_buffer_remove_bytes_at_end (keyboard->line_buffer, bytes_to_remove);

  for (node = ply_list_get_first_node(keyboard->backspace_handler_list);
       node; node = ply_list_get_next_node(keyboard->backspace_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      ply_keyboard_backspace_handler_t backspace_handler =
        (ply_keyboard_backspace_handler_t) closure->function;
      backspace_handler (closure->user_data);
    }
}

static void
process_line_erase (ply_keyboard_t *keyboard)
{
  size_t size;

  while ((size = ply_buffer_get_size (keyboard->line_buffer)) > 0)
    process_backspace (keyboard);
}

static void
process_keyboard_input (ply_keyboard_t *keyboard,
                        const char   *keyboard_input,
                        size_t        character_size)
{
  wchar_t key;
  ply_list_node_t *node;

  if ((ssize_t) mbrtowc (&key, keyboard_input, character_size, NULL) > 0)
    {
      switch (key)
        {
          case KEY_CTRL_U:
          case KEY_CTRL_W:
            ply_trace ("erase line!");
            process_line_erase (keyboard);
            return;

          case KEY_CTRL_V:
            ply_trace ("toggle verbose mode!");
            ply_toggle_tracing ();
            ply_trace ("verbose mode toggled!");
            return;

          case KEY_ESCAPE:
            ply_trace ("escape key!");
            for (node = ply_list_get_first_node(keyboard->escape_handler_list);
                 node; node = ply_list_get_next_node(keyboard->escape_handler_list, node))
              {
                ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
                ply_keyboard_escape_handler_t escape_handler = (ply_keyboard_escape_handler_t) closure->function;
                escape_handler (closure->user_data);
              }

            ply_trace ("end escape key handler");
            return;

          case KEY_BACKSPACE:
            ply_trace ("backspace key!");
            process_backspace (keyboard);
            return;

          case KEY_RETURN:
            ply_trace ("return key!");

            for (node = ply_list_get_first_node(keyboard->enter_handler_list);
                 node; node = ply_list_get_next_node(keyboard->enter_handler_list, node))
              {
                ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
                ply_keyboard_enter_handler_t enter_handler = (ply_keyboard_enter_handler_t)  closure->function;
                enter_handler (closure->user_data, ply_buffer_get_bytes (keyboard->line_buffer));
              }
            ply_buffer_clear (keyboard->line_buffer);
            return;

          default:
            ply_buffer_append_bytes (keyboard->line_buffer,
                                     keyboard_input, character_size);
            break;
        }
    }

  for (node = ply_list_get_first_node(keyboard->keyboard_input_handler_list);
       node; node = ply_list_get_next_node(keyboard->keyboard_input_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      ply_keyboard_input_handler_t keyboard_input_handler =
        (ply_keyboard_input_handler_t) closure->function;

      keyboard_input_handler (closure->user_data,
                              keyboard_input, character_size);
    }
}

static void
on_key_event (ply_keyboard_t   *keyboard,
              ply_buffer_t     *buffer)
{
  const char *bytes;
  size_t size, i;

  bytes = ply_buffer_get_bytes (buffer);
  size = ply_buffer_get_size (buffer);

  i = 0;
  while (i < size)
    {
      ssize_t character_size;
      char *keyboard_input;

      character_size = (ssize_t) ply_utf8_character_get_size (bytes + i, size - i);

      if (character_size < 0)
        break;

      /* If we're at a NUL character walk through it
      */
      if (character_size == 0)
        {
          i++;
          continue;
        }

      keyboard_input = strndup (bytes + i, character_size);

      process_keyboard_input (keyboard, keyboard_input, character_size);

      i += character_size;

      free (keyboard_input);
    }

  if (i > 0)
    ply_buffer_remove_bytes (buffer, i);
}

static bool
ply_keyboard_watch_for_renderer_input (ply_keyboard_t *keyboard)
{
  assert (keyboard != NULL);

  if (!ply_renderer_open_input_source (keyboard->provider.if_renderer->renderer,
                                       keyboard->provider.if_renderer->input_source))
    return false;

  ply_renderer_set_handler_for_input_source (keyboard->provider.if_renderer->renderer,
                                             keyboard->provider.if_renderer->input_source,
                                             (ply_renderer_input_source_handler_t)
                                             on_key_event,
                                             keyboard);
  return true;
}

static void
ply_keyboard_stop_watching_for_renderer_input (ply_keyboard_t *keyboard)
{
  ply_renderer_set_handler_for_input_source (keyboard->provider.if_renderer->renderer,
                                             keyboard->provider.if_renderer->input_source,
                                             (ply_renderer_input_source_handler_t)
                                             NULL, NULL);

  ply_renderer_close_input_source (keyboard->provider.if_renderer->renderer,
                                   keyboard->provider.if_renderer->input_source);
}

static void
on_terminal_data (ply_keyboard_t *keyboard)
{
  int terminal_fd;

  terminal_fd = ply_terminal_get_fd (keyboard->provider.if_terminal->terminal);
  ply_buffer_append_from_fd (keyboard->provider.if_terminal->key_buffer,
                             terminal_fd);
  on_key_event (keyboard, keyboard->provider.if_terminal->key_buffer);
}

static bool
ply_keyboard_watch_for_terminal_input (ply_keyboard_t *keyboard)
{
  int terminal_fd;

  assert (keyboard != NULL);

  terminal_fd = ply_terminal_get_fd (keyboard->provider.if_terminal->terminal);

  if (terminal_fd < 0 || !ply_terminal_is_open (keyboard->provider.if_terminal->terminal))
    {
      ply_trace ("terminal associated with keyboard isn't open");
      return false;
    }

  ply_terminal_watch_for_input (keyboard->provider.if_terminal->terminal,
                                (ply_terminal_input_handler_t) on_terminal_data,
                                keyboard);

  return true;
}

static void
ply_keyboard_stop_watching_for_terminal_input (ply_keyboard_t *keyboard)
{
  ply_terminal_stop_watching_for_input (keyboard->provider.if_terminal->terminal,
                                        (ply_terminal_input_handler_t)
                                        on_terminal_data,
                                        keyboard);
}

bool
ply_keyboard_watch_for_input (ply_keyboard_t *keyboard)
{
  assert (keyboard != NULL);

  switch (keyboard->provider_type)
    {
      case PLY_KEYBOARD_PROVIDER_TYPE_RENDERER:
        return ply_keyboard_watch_for_renderer_input (keyboard);

      case PLY_KEYBOARD_PROVIDER_TYPE_TERMINAL:
        return ply_keyboard_watch_for_terminal_input (keyboard);
    }

  return false;
}

void
ply_keyboard_stop_watching_for_input (ply_keyboard_t *keyboard)
{
  assert (keyboard != NULL);

  switch (keyboard->provider_type)
    {
      case PLY_KEYBOARD_PROVIDER_TYPE_RENDERER:
        ply_keyboard_stop_watching_for_renderer_input (keyboard);
        break;

      case PLY_KEYBOARD_PROVIDER_TYPE_TERMINAL:
        ply_keyboard_stop_watching_for_terminal_input (keyboard);
        break;
    }

}

void
ply_keyboard_free (ply_keyboard_t *keyboard)
{
  if (keyboard == NULL)
    return;

  ply_keyboard_stop_watching_for_input (keyboard);

  ply_buffer_free (keyboard->line_buffer);

  if (keyboard->provider_type == PLY_KEYBOARD_PROVIDER_TYPE_RENDERER)
    {
      free (keyboard->provider.if_renderer);
    }
  else
    {
      ply_buffer_free (keyboard->provider.if_terminal->key_buffer);
      free (keyboard->provider.if_terminal);
    }

  free (keyboard);
}

static ply_keyboard_closure_t *
ply_keyboard_closure_new (ply_keyboard_handler_t  function,
                          void                   *user_data)
{
  ply_keyboard_closure_t *closure = calloc (1, sizeof (ply_keyboard_closure_t));
  closure->function = function;
  closure->user_data = user_data;
  return closure;
}


static void
ply_keyboard_closure_free (ply_keyboard_closure_t *closure)
{
  free (closure);
}

void
ply_keyboard_add_input_handler (ply_keyboard_t               *keyboard,
                                ply_keyboard_input_handler_t  input_handler,
                                void                         *user_data)
{
  ply_keyboard_closure_t *closure;

  assert (keyboard != NULL);

  closure = ply_keyboard_closure_new ((ply_keyboard_handler_t) input_handler,
                                      user_data);
  ply_list_append_data (keyboard->keyboard_input_handler_list, closure);
}

void
ply_keyboard_remove_input_handler (ply_keyboard_t               *keyboard,
                                   ply_keyboard_input_handler_t  input_handler)
{
  ply_list_node_t *node;

  assert (keyboard != NULL);

  for (node = ply_list_get_first_node(keyboard->keyboard_input_handler_list);
       node; node = ply_list_get_next_node(keyboard->keyboard_input_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      if ((ply_keyboard_input_handler_t) closure->function == input_handler)
        {
          ply_keyboard_closure_free (closure);
          ply_list_remove_node (keyboard->keyboard_input_handler_list, node);
          return;
        }
    }
}

void
ply_keyboard_add_backspace_handler (ply_keyboard_t                   *keyboard,
                                    ply_keyboard_backspace_handler_t  backspace_handler,
                                    void                             *user_data)
{
  ply_keyboard_closure_t *closure;

  assert (keyboard != NULL);

  closure = ply_keyboard_closure_new ((ply_keyboard_handler_t) backspace_handler,
                                      user_data);
  ply_list_append_data (keyboard->backspace_handler_list, closure);
}


void
ply_keyboard_remove_backspace_handler (ply_keyboard_t                   *keyboard,
                                       ply_keyboard_backspace_handler_t  backspace_handler)
{
  ply_list_node_t *node;

  assert (keyboard != NULL);

  for (node = ply_list_get_first_node(keyboard->backspace_handler_list);
       node; node = ply_list_get_next_node(keyboard->backspace_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      if ((ply_keyboard_backspace_handler_t) closure->function == backspace_handler)
        {
          ply_keyboard_closure_free (closure);
          ply_list_remove_node (keyboard->backspace_handler_list, node);
          return;
        }
    }
}

void
ply_keyboard_add_escape_handler (ply_keyboard_t                *keyboard,
                                 ply_keyboard_escape_handler_t  escape_handler,
                                 void                          *user_data)
{
  ply_keyboard_closure_t *closure;

  assert (keyboard != NULL);

  closure = ply_keyboard_closure_new ((ply_keyboard_handler_t)  escape_handler,
                                      user_data);
  ply_list_append_data (keyboard->escape_handler_list, closure);
}


void
ply_keyboard_remove_escape_handler (ply_keyboard_t                *keyboard,
                                    ply_keyboard_escape_handler_t  escape_handler)
{
  ply_list_node_t *node;

  assert (keyboard != NULL);

  for (node = ply_list_get_first_node(keyboard->escape_handler_list);
       node; node = ply_list_get_next_node(keyboard->escape_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      if ((ply_keyboard_escape_handler_t) closure->function == escape_handler)
        {
          ply_keyboard_closure_free (closure);
          ply_list_remove_node (keyboard->escape_handler_list, node);
          return;
        }
    }
}

void
ply_keyboard_add_enter_handler (ply_keyboard_t               *keyboard,
                                ply_keyboard_enter_handler_t  enter_handler,
                                void                         *user_data)
{
  ply_keyboard_closure_t *closure;

  assert (keyboard != NULL);

  closure = ply_keyboard_closure_new ((ply_keyboard_handler_t) enter_handler,
                                      user_data);

  ply_list_append_data (keyboard->enter_handler_list, closure);
}

void
ply_keyboard_remove_enter_handler (ply_keyboard_t               *keyboard,
                                   ply_keyboard_enter_handler_t  enter_handler)
{
  ply_list_node_t *node;

  assert (keyboard != NULL);

  for (node = ply_list_get_first_node(keyboard->enter_handler_list);
       node; node = ply_list_get_next_node(keyboard->enter_handler_list, node))
    {
      ply_keyboard_closure_t *closure = ply_list_node_get_data (node);
      if ((ply_keyboard_enter_handler_t) closure->function == enter_handler)
        {
          ply_keyboard_closure_free (closure);
          ply_list_remove_node (keyboard->enter_handler_list, node);
          return;
        }
    }
}

/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
