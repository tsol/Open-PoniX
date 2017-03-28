/* ply-keyboard.h - APIs for putting up a splash screen
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
#ifndef PLY_KEYBOARD_H
#define PLY_KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-renderer.h"

typedef struct _ply_keyboard ply_keyboard_t;

typedef void (* ply_keyboard_input_handler_t) (void       *user_data,
                                               const char *keyboard_input,
                                               size_t      character_size);

typedef void (* ply_keyboard_backspace_handler_t) (void *user_data);

typedef void (* ply_keyboard_escape_handler_t) (void *user_data);

typedef void (* ply_keyboard_enter_handler_t) (void       *user_data,
                                               const char *line);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_keyboard_t *ply_keyboard_new_for_terminal (ply_terminal_t *terminal);
ply_keyboard_t *ply_keyboard_new_for_renderer (ply_renderer_t *renderer);
void ply_keyboard_free (ply_keyboard_t *keyboard);

void ply_keyboard_add_input_handler (ply_keyboard_t               *keyboard,
                                     ply_keyboard_input_handler_t  input_handler,
                                     void                         *user_data);
void ply_keyboard_remove_input_handler (ply_keyboard_t               *keyboard,
                                        ply_keyboard_input_handler_t  input_handler);
void ply_keyboard_add_backspace_handler (ply_keyboard_t                   *keyboard,
                                         ply_keyboard_backspace_handler_t  backspace_handler,
                                         void                             *user_data);
void ply_keyboard_remove_backspace_handler (ply_keyboard_t                   *keyboard,
                                            ply_keyboard_backspace_handler_t  backspace_handler);
void ply_keyboard_add_escape_handler (ply_keyboard_t                *keyboard,
                                      ply_keyboard_escape_handler_t  escape_handler,
                                      void                          *user_data);
void ply_keyboard_remove_escape_handler (ply_keyboard_t                *keyboard,
                                         ply_keyboard_escape_handler_t  escape_handler);
void ply_keyboard_add_enter_handler (ply_keyboard_t               *keyboard,
                                     ply_keyboard_enter_handler_t  enter_handler,
                                     void                         *user_data);
void ply_keyboard_remove_enter_handler (ply_keyboard_t               *keyboard,
                                        ply_keyboard_enter_handler_t  enter_handler);

bool ply_keyboard_watch_for_input (ply_keyboard_t *keyboard);
void ply_keyboard_stop_watching_for_input (ply_keyboard_t *keyboard);

#endif

#endif /* PLY_KEYBOARD_H */
/* vim: set ts=4 sw=4 et ai ci cino={.5s,^-2,+.5s,t0,g0,e-2,n-2,p2s,(0,=.5s,:.5s */
