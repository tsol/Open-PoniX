/* script-lib-plymouth.c - script library for interacting with plymouth
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
#define _GNU_SOURCE
#include "ply-boot-splash-plugin.h"
#include "ply-utils.h"
#include "script.h"
#include "script-parse.h"
#include "script-execute.h"
#include "script-object.h"
#include "script-lib-plymouth.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "script-lib-plymouth.script.h"

static script_return_t plymouth_set_function (script_state_t *state,
                                              void           *user_data)
{
  script_obj_t **script_func = user_data;
  script_obj_t *obj = script_obj_hash_get_element (state->local, "function");

  script_obj_deref (&obj);
  script_obj_unref (*script_func);
  *script_func = obj;
  return script_return_obj_null ();
}

static script_return_t plymouth_get_mode (script_state_t *state,
                                          void           *user_data)
{
  script_lib_plymouth_data_t *data = user_data;
  script_obj_t *obj;
  switch (data->mode)
    {
      case PLY_BOOT_SPLASH_MODE_BOOT_UP:
        obj = script_obj_new_string ("boot");
        break;
      case PLY_BOOT_SPLASH_MODE_SHUTDOWN:
        obj = script_obj_new_string ("shutdown");
        break;
      case PLY_BOOT_SPLASH_MODE_UPDATES:
        obj = script_obj_new_string ("updates");
        break;
      default:
        obj = script_obj_new_string ("unknown");
        break;
    }
  return script_return_obj (obj);
}

script_lib_plymouth_data_t *script_lib_plymouth_setup (script_state_t         *state,
                                                       ply_boot_splash_mode_t  mode)
{
  script_lib_plymouth_data_t *data = malloc (sizeof (script_lib_plymouth_data_t));

  data->script_refresh_func = script_obj_new_null ();
  data->script_boot_progress_func = script_obj_new_null ();
  data->script_root_mounted_func = script_obj_new_null ();
  data->script_keyboard_input_func = script_obj_new_null ();
  data->script_update_status_func = script_obj_new_null ();
  data->script_display_normal_func = script_obj_new_null ();
  data->script_display_password_func = script_obj_new_null ();
  data->script_display_question_func = script_obj_new_null ();
  data->script_display_message_func = script_obj_new_null ();
  data->script_hide_message_func = script_obj_new_null ();
  data->script_quit_func = script_obj_new_null ();
  data->mode = mode;
  
  script_obj_t *plymouth_hash = script_obj_hash_get_element (state->global, "Plymouth");
  script_add_native_function (plymouth_hash,
                              "SetRefreshFunction",
                              plymouth_set_function,
                              &data->script_refresh_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetBootProgressFunction",
                              plymouth_set_function,
                              &data->script_boot_progress_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetRootMountedFunction",
                              plymouth_set_function,
                              &data->script_root_mounted_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetKeyboardInputFunction",
                              plymouth_set_function,
                              &data->script_keyboard_input_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetUpdateStatusFunction",
                              plymouth_set_function,
                              &data->script_update_status_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetDisplayNormalFunction",
                              plymouth_set_function,
                              &data->script_display_normal_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetDisplayPasswordFunction",
                              plymouth_set_function,
                              &data->script_display_password_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetDisplayQuestionFunction",
                              plymouth_set_function,
                              &data->script_display_question_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetDisplayMessageFunction",
                              plymouth_set_function,
                              &data->script_display_message_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetHideMessageFunction",
                              plymouth_set_function,
                              &data->script_hide_message_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "SetQuitFunction",
                              plymouth_set_function,
                              &data->script_quit_func,
                              "function",
                              NULL);
  script_add_native_function (plymouth_hash,
                              "GetMode",
                              plymouth_get_mode,
                              data,
                              NULL);
  script_obj_unref (plymouth_hash);

  data->script_main_op = script_parse_string (script_lib_plymouth_string, "script-lib-plymouth.script");
  script_return_t ret = script_execute (state, data->script_main_op);
  script_obj_unref (ret.object);                /* Throw anything sent back away */

  return data;
}

void script_lib_plymouth_destroy (script_lib_plymouth_data_t *data)
{
  script_parse_op_free (data->script_main_op);
  script_obj_unref (data->script_refresh_func);
  script_obj_unref (data->script_boot_progress_func);
  script_obj_unref (data->script_root_mounted_func);
  script_obj_unref (data->script_keyboard_input_func);
  script_obj_unref (data->script_update_status_func);
  script_obj_unref (data->script_display_normal_func);
  script_obj_unref (data->script_display_password_func);
  script_obj_unref (data->script_display_question_func);
  script_obj_unref (data->script_display_message_func);
  script_obj_unref (data->script_hide_message_func);
  script_obj_unref (data->script_quit_func);
  free (data);
}

void script_lib_plymouth_on_refresh (script_state_t             *state,
                                     script_lib_plymouth_data_t *data)
{
    script_return_t ret = script_execute_object (state,
                                                 data->script_refresh_func,
                                                 NULL,
                                                 NULL);
    script_obj_unref (ret.object);
}

void script_lib_plymouth_on_boot_progress (script_state_t             *state,
                                           script_lib_plymouth_data_t *data,
                                           double                     duration,
                                           double                     progress)
{
  script_obj_t *duration_obj = script_obj_new_number (duration);
  script_obj_t *progress_obj = script_obj_new_number (progress);
  script_return_t ret = script_execute_object (state,
                                               data->script_boot_progress_func,
                                               NULL,
                                               duration_obj,
                                               progress_obj,
                                               NULL);
  script_obj_unref (ret.object);
  script_obj_unref (duration_obj);
  script_obj_unref (progress_obj);
}

void script_lib_plymouth_on_root_mounted (script_state_t             *state,
                                          script_lib_plymouth_data_t *data)
{
  script_return_t ret = script_execute_object (state,
                                               data->script_root_mounted_func,
                                               NULL,
                                               NULL);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_keyboard_input (script_state_t             *state,
                                            script_lib_plymouth_data_t *data,
                                            const char                 *keyboard_input)
{
  script_obj_t *keyboard_input_obj = script_obj_new_string (keyboard_input);
  script_return_t ret = script_execute_object (state,
                                                 data->script_keyboard_input_func,
                                                 NULL,
                                                 keyboard_input_obj,
                                                 NULL);
  script_obj_unref (keyboard_input_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_update_status (script_state_t             *state,
                                           script_lib_plymouth_data_t *data,
                                           const char                 *new_status)
{
  script_obj_t *new_status_obj = script_obj_new_string (new_status);
  script_return_t ret = script_execute_object (state,
                                               data->script_update_status_func,
                                               NULL,
                                               new_status_obj,
                                               NULL);
  script_obj_unref (new_status_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_display_normal (script_state_t             *state,
                                            script_lib_plymouth_data_t *data)
{
  script_return_t ret = script_execute_object (state,
                                               data->script_display_normal_func,
                                               NULL,
                                               NULL);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_display_password (script_state_t             *state,
                                              script_lib_plymouth_data_t *data,
                                              const char                 *prompt,
                                              int                         bullets)
{
  script_obj_t *prompt_obj = script_obj_new_string (prompt);
  script_obj_t *bullets_obj = script_obj_new_number (bullets);
  script_return_t ret = script_execute_object (state,
                                               data->script_display_password_func,
                                               NULL,
                                               prompt_obj,
                                               bullets_obj,
                                               NULL);
  script_obj_unref (prompt_obj);
  script_obj_unref (bullets_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_display_question (script_state_t             *state,
                                              script_lib_plymouth_data_t *data,
                                              const char                 *prompt,
                                              const char                 *entry_text)
{
  script_obj_t *prompt_obj = script_obj_new_string (prompt);
  script_obj_t *entry_text_obj = script_obj_new_string (entry_text);
  script_return_t ret = script_execute_object (state,
                                               data->script_display_question_func,
                                               NULL,
                                               prompt_obj,
                                               entry_text_obj,
                                               NULL);
  script_obj_unref (prompt_obj);
  script_obj_unref (entry_text_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_display_message (script_state_t             *state,
                                             script_lib_plymouth_data_t *data,
                                             const char                 *message)
{
  script_obj_t *new_message_obj = script_obj_new_string (message);
  script_return_t ret = script_execute_object (state,
                                               data->script_display_message_func,
                                               NULL,
                                               new_message_obj,
                                               NULL);
  script_obj_unref (new_message_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_hide_message (script_state_t             *state,
                                          script_lib_plymouth_data_t *data,
                                          const char                 *message)
{
  script_obj_t *new_message_obj = script_obj_new_string (message);
  script_return_t ret = script_execute_object (state,
                                               data->script_hide_message_func,
                                               NULL,
                                               new_message_obj,
                                               NULL);
  script_obj_unref (new_message_obj);
  script_obj_unref (ret.object);
}

void script_lib_plymouth_on_quit (script_state_t             *state,
                                  script_lib_plymouth_data_t *data)
{
  script_return_t ret = script_execute_object (state,
                                               data->script_quit_func,
                                               NULL,
                                               NULL);
  script_obj_unref (ret.object);
}
