/* ply-text-display.c - APIs for displaying text
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
#include "ply-text-display.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wchar.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-terminal.h"
#include "ply-utils.h"

#ifndef CLEAR_SCREEN_SEQUENCE
#define CLEAR_SCREEN_SEQUENCE "\033[2J"
#endif

#ifndef CLEAR_LINE_SEQUENCE
#define CLEAR_LINE_SEQUENCE "\033[2K\r\n"
#endif

#ifndef BACKSPACE
#define BACKSPACE "\b\033[0K"
#endif

#ifndef MOVE_CURSOR_SEQUENCE
#define MOVE_CURSOR_SEQUENCE "\033[%d;%df"
#endif

#ifndef HIDE_CURSOR_SEQUENCE
#define HIDE_CURSOR_SEQUENCE "\033[?25l"
#endif

#ifndef SHOW_CURSOR_SEQUENCE
#define SHOW_CURSOR_SEQUENCE "\033[?25h"
#endif

#ifndef COLOR_SEQUENCE_FORMAT
#define COLOR_SEQUENCE_FORMAT "\033[%dm"
#endif

#ifndef PAUSE_SEQUENCE
#define PAUSE_SEQUENCE "\023"
#endif

#ifndef UNPAUSE_SEQUENCE
#define UNPAUSE_SEQUENCE "\021"
#endif

#ifndef FOREGROUND_COLOR_BASE
#define FOREGROUND_COLOR_BASE 30
#endif

#ifndef BACKGROUND_COLOR_BASE
#define BACKGROUND_COLOR_BASE 40
#endif

#ifndef TEXT_PALETTE_SIZE
#define TEXT_PALETTE_SIZE 48
#endif

struct _ply_text_display
{
  ply_event_loop_t *loop;

  ply_terminal_t *terminal;

  ply_terminal_color_t foreground_color;
  ply_terminal_color_t background_color;

  ply_text_display_draw_handler_t draw_handler;
  void *draw_handler_user_data;
};

ply_text_display_t *
ply_text_display_new (ply_terminal_t *terminal)
{
  ply_text_display_t *display;

  display = calloc (1, sizeof (ply_text_display_t));

  display->loop = NULL;
  display->terminal = terminal;

  return display;
}

int
ply_text_display_get_number_of_columns (ply_text_display_t *display)
{
  return ply_terminal_get_number_of_columns (display->terminal);
}

int
ply_text_display_get_number_of_rows (ply_text_display_t *display)
{
  return ply_terminal_get_number_of_rows (display->terminal);
}

void
ply_text_display_set_cursor_position (ply_text_display_t *display,
                                      int                 column,
                                      int                 row)
{
  int number_of_columns;
  int number_of_rows;

  number_of_columns = ply_text_display_get_number_of_columns (display);
  number_of_rows = ply_text_display_get_number_of_rows (display);

  column = CLAMP (column, 0, number_of_columns - 1);
  row = CLAMP (row, 0, number_of_rows - 1);

  ply_terminal_write (display->terminal,
                      MOVE_CURSOR_SEQUENCE,
                      row, column);
}

void
ply_text_display_clear_screen (ply_text_display_t *display)
{
  if (ply_is_tracing ())
    return;

  ply_terminal_write (display->terminal,
                      CLEAR_SCREEN_SEQUENCE);

  ply_text_display_set_cursor_position (display, 0, 0);
}

void
ply_text_display_clear_line (ply_text_display_t *display)
{

  ply_terminal_write (display->terminal,
                      CLEAR_LINE_SEQUENCE);
}

void
ply_text_display_remove_character (ply_text_display_t *display)
{
  ply_terminal_write (display->terminal,
                      BACKSPACE);
}

void
ply_text_display_set_background_color (ply_text_display_t   *display,
                                       ply_terminal_color_t  color)
{

  ply_terminal_write (display->terminal,
                      COLOR_SEQUENCE_FORMAT,
                      BACKGROUND_COLOR_BASE + color);

  display->background_color = color;
}

void
ply_text_display_set_foreground_color (ply_text_display_t       *display,
                                       ply_terminal_color_t  color)
{
  ply_terminal_write (display->terminal,
                      COLOR_SEQUENCE_FORMAT,
                      FOREGROUND_COLOR_BASE + color);

  display->foreground_color = color;
}

ply_terminal_color_t
ply_text_display_get_background_color (ply_text_display_t *display)
{
  return display->background_color;
}

ply_terminal_color_t
ply_text_display_get_foreground_color (ply_text_display_t *display)
{
  return display->foreground_color;
}

void
ply_text_display_draw_area (ply_text_display_t *display,
                            int                 x,
                            int                 y,
                            int                 width,
                            int                 height)
{
  if (display->draw_handler != NULL)
    display->draw_handler (display->draw_handler_user_data,
                           display->terminal,
                           x, y, width, height);
}

void
ply_text_display_hide_cursor (ply_text_display_t *display)
{
  ply_terminal_write (display->terminal,
                      HIDE_CURSOR_SEQUENCE);
}

void
ply_text_display_write (ply_text_display_t *display,
                        const char         *format,
                        ...)
{
  int fd;

  va_list args;
  char *string;

  assert (display != NULL);
  assert (format != NULL);

  fd = ply_terminal_get_fd (display->terminal);

  string = NULL;
  va_start (args, format);
  vasprintf (&string, format, args);
  va_end (args);

  write (fd, string, strlen (string));
  free (string);
}

void
ply_text_display_show_cursor (ply_text_display_t *display)
{
  ply_terminal_write (display->terminal,
                      SHOW_CURSOR_SEQUENCE);
}

bool
ply_text_display_supports_color (ply_text_display_t *display)
{
  return ply_terminal_supports_color (display->terminal);
}

static void
ply_text_display_detach_from_event_loop (ply_text_display_t *display)
{
  assert (display != NULL);
  display->loop = NULL;
}

void
ply_text_display_free (ply_text_display_t *display)
{
  if (display == NULL)
    return;

  if (display->loop != NULL)
    ply_event_loop_stop_watching_for_exit (display->loop,
                                           (ply_event_loop_exit_handler_t)
                                           ply_text_display_detach_from_event_loop,
                                           display);

  free (display);
}

void
ply_text_display_set_draw_handler (ply_text_display_t *display,
                                   ply_text_display_draw_handler_t draw_handler,
                                   void               *user_data)
{
  assert (display != NULL);

  display->draw_handler = draw_handler;
  display->draw_handler_user_data = user_data;
}

void
ply_text_display_pause_updates (ply_text_display_t *display)
{
  ply_terminal_write (display->terminal,
                      PAUSE_SEQUENCE);
}

void
ply_text_display_unpause_updates (ply_text_display_t *display)
{
  ply_terminal_write (display->terminal,
                      UNPAUSE_SEQUENCE);
}

void
ply_text_display_attach_to_event_loop (ply_text_display_t *display,
                                       ply_event_loop_t   *loop)
{
  assert (display != NULL);
  assert (loop != NULL);
  assert (display->loop == NULL);

  display->loop = loop;

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                 ply_text_display_detach_from_event_loop,
                                 display);
}

ply_terminal_t *
ply_text_display_get_terminal (ply_text_display_t *display)
{
  return display->terminal;
}

/* vim: set ts= 4 sw= 4 et ai ci cino= {.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
