/* script-lib-plymouth.h - script library for interacting with plymouth
 *
 * Copyright (C) 2009 Charlie Brej <cbrej@cs.man.ac.uk>
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
 */
#ifndef SCRIPT_LIB_PLYMOUTH_H
#define SCRIPT_LIB_PLYMOUTH_H

#include "ply-boot-splash-plugin.h"
#include "script.h"

typedef struct
{
  script_op_t            *script_main_op;
  script_obj_t           *script_refresh_func;
  script_obj_t           *script_boot_progress_func;
  script_obj_t           *script_root_mounted_func;
  script_obj_t           *script_keyboard_input_func;
  script_obj_t           *script_update_status_func;
  script_obj_t           *script_display_normal_func;
  script_obj_t           *script_display_password_func;
  script_obj_t           *script_display_question_func;
  script_obj_t           *script_display_message_func;
  script_obj_t           *script_hide_message_func;
  script_obj_t           *script_quit_func;
  ply_boot_splash_mode_t  mode;
} script_lib_plymouth_data_t;

script_lib_plymouth_data_t *script_lib_plymouth_setup (script_state_t         *state,
                                                       ply_boot_splash_mode_t  mode);
void script_lib_plymouth_destroy (script_lib_plymouth_data_t *data);

void script_lib_plymouth_on_refresh (script_state_t             *state,
                                     script_lib_plymouth_data_t *data);
void script_lib_plymouth_on_boot_progress (script_state_t             *state,
                                           script_lib_plymouth_data_t *data,
                                           double                     duration,
                                           double                     progress);
void script_lib_plymouth_on_root_mounted (script_state_t             *state,
                                          script_lib_plymouth_data_t *data);
void script_lib_plymouth_on_keyboard_input (script_state_t             *state,
                                            script_lib_plymouth_data_t *data,
                                            const char                 *keyboard_input);
void script_lib_plymouth_on_update_status (script_state_t             *state,
                                           script_lib_plymouth_data_t *data,
                                           const char                 *new_status);
void script_lib_plymouth_on_display_normal (script_state_t             *state,
                                            script_lib_plymouth_data_t *data);
void script_lib_plymouth_on_display_password (script_state_t             *state,
                                              script_lib_plymouth_data_t *data,
                                              const char                 *prompt,
                                              int                         bullets);
void script_lib_plymouth_on_display_question (script_state_t             *state,
                                              script_lib_plymouth_data_t *data,
                                              const char                 *prompt,
                                              const char                 *entry_text);
void script_lib_plymouth_on_display_message (script_state_t             *state,
                                             script_lib_plymouth_data_t *data,
                                             const char                 *new_message);
void script_lib_plymouth_on_hide_message (script_state_t             *state,
                                          script_lib_plymouth_data_t *data,
                                          const char                 *new_message);
void script_lib_plymouth_on_quit (script_state_t             *state,
                                  script_lib_plymouth_data_t *data);

#endif /* SCRIPT_LIB_PLYMOUTH_H */
