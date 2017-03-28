/* ply-text-display.h - APIs for displaying text
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
#ifndef PLY_TEXT_DISPLAY_H
#define PLY_TEXT_DISPLAY_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-terminal.h"

typedef struct _ply_text_display ply_text_display_t;

typedef void (* ply_text_display_draw_handler_t) (void *user_data,
                                                  ply_terminal_t *terminal,
                                                  int   column,
                                                  int   row,
                                                  int   number_of_columns,
                                                  int   number_of_rows);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_text_display_t *ply_text_display_new (ply_terminal_t *terminal);

void ply_text_display_free (ply_text_display_t *display);

void ply_text_display_attach_to_event_loop (ply_text_display_t *display,
                                            ply_event_loop_t   *loop);

ply_terminal_t *ply_text_display_get_terminal (ply_text_display_t *display);

int  ply_text_display_get_number_of_rows (ply_text_display_t *display);
int  ply_text_display_get_number_of_columns (ply_text_display_t *display);
void ply_text_display_set_cursor_position (ply_text_display_t *display,
                                           int                 column,
                                           int                 row);
__attribute__((__format__ (__printf__, 2, 3)))
void ply_text_display_write (ply_text_display_t *display,
                             const char         *format,
                             ...);
void ply_text_display_hide_cursor (ply_text_display_t *display);
void ply_text_display_show_cursor (ply_text_display_t *display);
void ply_text_display_clear_screen (ply_text_display_t *display);
void ply_text_display_clear_line (ply_text_display_t *display);
void ply_text_display_remove_character (ply_text_display_t *display);
bool ply_text_display_supports_color (ply_text_display_t *display);
void ply_text_display_set_background_color (ply_text_display_t       *display,
                                            ply_terminal_color_t  color);
void ply_text_display_set_foreground_color (ply_text_display_t       *display,
                                            ply_terminal_color_t  color);
ply_terminal_color_t ply_text_display_get_background_color (ply_text_display_t *display);
ply_terminal_color_t ply_text_display_get_foreground_color (ply_text_display_t *display);

void ply_text_display_draw_area (ply_text_display_t *display,
                                 int           column,
                                 int           row,
                                 int           number_of_columns,
                                 int           number_of_rows);

void ply_text_display_set_draw_handler (ply_text_display_t                *display,
                                        ply_text_display_draw_handler_t    draw_handler,
                                        void                        *user_data);
void ply_text_display_pause_updates (ply_text_display_t *display);
void ply_text_display_unpause_updates (ply_text_display_t *display);

#endif

#endif /* PLY_TEXT_DISPLAY_H */
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
