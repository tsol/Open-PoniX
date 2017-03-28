/* script-lib-math.c - math script functions library
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
#include "script.h"
#include "script-parse.h"
#include "script-execute.h"
#include "script-object.h"
#include "script-lib-math.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "config.h"

#include "script-lib-math.script.h"

static script_return_t script_lib_math_double_from_double_function (script_state_t *state,
                                                                    void           *user_data)
{
  double (*function)(double) = user_data;
  double value = script_obj_hash_get_number (state->local, "value");
  double reply_double = function (value);
  return script_return_obj (script_obj_new_number (reply_double));
}

static script_return_t script_lib_math_double_from_double_double_function (script_state_t *state,
                                                                           void           *user_data)
{
  double (*function)(double, double) = user_data;
  double value1 = script_obj_hash_get_number (state->local, "value_a");
  double value2 = script_obj_hash_get_number (state->local, "value_b");
  double reply_double = function (value1, value2);
  return script_return_obj (script_obj_new_number (reply_double));
}

static script_return_t script_lib_math_random (script_state_t *state,
                                               void           *user_data)
{
  double reply_double = random() / ((double)RAND_MAX + 1);
  return script_return_obj (script_obj_new_number (reply_double));
}

script_lib_math_data_t *script_lib_math_setup (script_state_t *state)
{
  script_lib_math_data_t *data = malloc (sizeof (script_lib_math_data_t));

  srand ((int) ply_get_timestamp ());

  script_obj_t *math_hash = script_obj_hash_get_element (state->global, "Math");
  script_add_native_function (math_hash,
                              "Cos",
                              script_lib_math_double_from_double_function,
                              cos,
                              "value",
                              NULL);
  script_add_native_function (math_hash,
                              "Sin",
                              script_lib_math_double_from_double_function,
                              sin,
                              "value",
                              NULL);
  script_add_native_function (math_hash,
                              "Tan",
                              script_lib_math_double_from_double_function,
                              tan,
                              "value",
                              NULL);
  script_add_native_function (math_hash,
                              "ATan2",
                              script_lib_math_double_from_double_double_function,
                              atan2,
                              "value_a",
                              "value_b",
                              NULL);
  script_add_native_function (math_hash,
                              "Sqrt",
                              script_lib_math_double_from_double_function,
                              sqrt,
                              "value",
                              NULL);
  script_add_native_function (math_hash,
                              "Int",
                              script_lib_math_double_from_double_function,
                              floor,
                              "value",
                              NULL);
  script_add_native_function (math_hash,
                              "Random",
                              script_lib_math_random,
                              NULL,
                              NULL);
  script_obj_unref (math_hash);

  data->script_main_op = script_parse_string (script_lib_math_string, "script-lib-math.script");
  script_return_t ret = script_execute (state, data->script_main_op);
  script_obj_unref (ret.object);

  return data;
}

void script_lib_math_destroy (script_lib_math_data_t *data)
{
  script_parse_op_free (data->script_main_op);
  free (data);
}

