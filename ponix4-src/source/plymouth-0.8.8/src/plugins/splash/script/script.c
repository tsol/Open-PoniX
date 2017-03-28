/* script.c - scripting system
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
#include "ply-hashtable.h"
#include "ply-list.h"
#include "ply-bitarray.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>

#include "script.h"
#include "script-parse.h"
#include "script-object.h"

script_function_t *script_function_script_new (script_op_t *script,
                                               void        *user_data,
                                               ply_list_t  *parameter_list)
{
  script_function_t *function = malloc (sizeof (script_function_t));

  function->type = SCRIPT_FUNCTION_TYPE_SCRIPT;
  function->parameters = parameter_list;
  function->data.script = script;
  function->freeable = false;
  function->user_data = user_data;
  return function;
}

script_function_t *script_function_native_new (script_native_function_t  native_function,
                                               void                     *user_data,
                                               ply_list_t               *parameter_list)
{
  script_function_t *function = malloc (sizeof (script_function_t));

  function->type = SCRIPT_FUNCTION_TYPE_NATIVE;
  function->parameters = parameter_list;
  function->data.native = native_function;
  function->freeable = true;
  function->user_data = user_data;
  return function;
}

void script_add_native_function (script_obj_t            *hash,
                                 const char              *name,
                                 script_native_function_t native_function,
                                 void                    *user_data,
                                 const char              *first_arg,
                                 ...)
{
  va_list args;
  const char *arg;
  ply_list_t *parameter_list = ply_list_new ();

  arg = first_arg;
  va_start (args, first_arg);
  while (arg)
    {
      ply_list_append_data (parameter_list, strdup (arg));
      arg = va_arg (args, const char *);
    }
  va_end (args);

  script_function_t *function = script_function_native_new (native_function,
                                                            user_data,
                                                            parameter_list);
  script_obj_t *obj = script_obj_new_function (function);
  script_obj_hash_add_element (hash, obj, name);
  script_obj_unref (obj);
}

script_obj_native_class_t *script_obj_native_class_new (script_obj_function_t free_func,
                                                        const char           *name,
                                                        void                 *user_data)
{
  script_obj_native_class_t *class = malloc (sizeof (script_obj_native_class_t));

  class->free_func = free_func;
  class->name = strdup (name);
  class->user_data = user_data;
  return class;
}

void script_obj_native_class_destroy (script_obj_native_class_t *class)
{
  free (class->name);
  free (class);
  return;
}

script_state_t *script_state_new (void *user_data)
{
  script_state_t *state = malloc (sizeof (script_state_t));
  script_obj_t *global_hash = script_obj_new_hash ();
  state->global = script_obj_new_ref (global_hash);
  script_obj_unref(global_hash);
  state->local = script_obj_new_ref (global_hash);
  state->this = script_obj_new_null();
  state->user_data = user_data;
  return state;
}

script_state_t *script_state_init_sub (script_state_t *oldstate, script_obj_t *this)
{
  script_state_t *newstate = malloc (sizeof (script_state_t));
  script_obj_t *local_hash = script_obj_new_hash ();
  newstate->local = script_obj_new_ref (local_hash);
  script_obj_unref(local_hash);
  newstate->global = script_obj_new_ref (oldstate->global);
  if (this) newstate->this = script_obj_new_ref (this);
  else newstate->this = script_obj_new_ref (oldstate->this);
  newstate->user_data = oldstate->user_data;
  return newstate;
}

void script_state_destroy (script_state_t *state)
{
  script_obj_unref (state->global);
  script_obj_unref (state->local);
  script_obj_unref (state->this);
  free (state);
}

