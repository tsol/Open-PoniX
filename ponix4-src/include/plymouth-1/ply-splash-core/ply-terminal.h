/* ply-terminal.h - APIs for terminaling text
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
#ifndef PLY_TERMINAL_H
#define PLY_TERMINAL_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"

typedef struct _ply_terminal ply_terminal_t;
typedef void (* ply_terminal_active_vt_changed_handler_t) (void           *user_data,
                                                           ply_terminal_t *terminal);
typedef void (* ply_terminal_input_handler_t) (void           *user_data,
                                               ply_terminal_t *terminal);
typedef enum
{
  PLY_TERMINAL_COLOR_BLACK = 0,
  PLY_TERMINAL_COLOR_RED,
  PLY_TERMINAL_COLOR_GREEN,
  PLY_TERMINAL_COLOR_BROWN,
  PLY_TERMINAL_COLOR_BLUE,
  PLY_TERMINAL_COLOR_MAGENTA,
  PLY_TERMINAL_COLOR_CYAN,
  PLY_TERMINAL_COLOR_WHITE,
  PLY_TERMINAL_COLOR_DEFAULT = PLY_TERMINAL_COLOR_WHITE + 2
} ply_terminal_color_t;

typedef enum
{
  PLY_TERMINAL_MODE_TEXT,
  PLY_TERMINAL_MODE_GRAPHICS
} ply_terminal_mode_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_terminal_t *ply_terminal_new (const char *device_name);

void ply_terminal_free (ply_terminal_t *terminal);

bool ply_terminal_open (ply_terminal_t *terminal);
int ply_terminal_get_fd (ply_terminal_t *terminal);
bool ply_terminal_is_vt (ply_terminal_t *terminal);
bool ply_terminal_is_open (ply_terminal_t *terminal);
bool ply_terminal_is_active (ply_terminal_t *terminal);
void ply_terminal_close (ply_terminal_t *terminal);
void ply_terminal_reset_colors (ply_terminal_t *terminal);

bool ply_terminal_set_unbuffered_input (ply_terminal_t *terminal);
bool ply_terminal_set_buffered_input (ply_terminal_t *terminal);

__attribute__((__format__ (__printf__, 2, 3)))
void ply_terminal_write (ply_terminal_t *terminal,
                         const char     *format,
                         ...);
int ply_terminal_get_number_of_columns (ply_terminal_t *terminal);
int ply_terminal_get_number_of_rows (ply_terminal_t *terminal);

bool ply_terminal_supports_color (ply_terminal_t *terminal);
uint32_t ply_terminal_get_color_hex_value (ply_terminal_t       *terminal,
                                           ply_terminal_color_t  color);

void ply_terminal_set_color_hex_value (ply_terminal_t       *terminal,
                                       ply_terminal_color_t  color,
                                       uint32_t              hex_value);

void ply_terminal_set_mode (ply_terminal_t     *terminal,
                            ply_terminal_mode_t mode);

void ply_terminal_ignore_mode_changes (ply_terminal_t *terminal,
                                       bool            should_ignore);

int ply_terminal_get_vt_number (ply_terminal_t *terminal);
bool ply_terminal_activate_vt (ply_terminal_t *terminal);
bool ply_terminal_deactivate_vt (ply_terminal_t *terminal);

void ply_terminal_watch_for_vt_changes (ply_terminal_t *terminal);
void ply_terminal_stop_watching_for_vt_changes (ply_terminal_t *terminal);

void ply_terminal_watch_for_active_vt_change (ply_terminal_t *terminal,
                                              ply_terminal_active_vt_changed_handler_t active_vt_changed_handler,
                                              void *user_data);
void ply_terminal_stop_watching_for_active_vt_change (ply_terminal_t *terminal,
                                                      ply_terminal_active_vt_changed_handler_t active_vt_changed_handler,
                                                      void *user_data);

void ply_terminal_watch_for_input (ply_terminal_t *terminal,
                                   ply_terminal_input_handler_t input_handler,
                                   void *user_data);
void ply_terminal_stop_watching_for_input (ply_terminal_t *terminal,
                                           ply_terminal_input_handler_t input_handler,
                                           void *user_data);

#endif

#endif /* PLY_TERMINAL_H */
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
