/* script-lib-string.c - string script functions library
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
#include "script-lib-string.h"
#include "ply-utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "script-lib-string.script.h"


static script_return_t script_lib_string_char_at (script_state_t *state,
                                                  void           *user_data)
{
  char *text = script_obj_as_string (state->this);
  int index = script_obj_hash_get_number (state->local, "index");
  int count;
  char charstring [2];
  
  if (!text || index < 0)
    {
      free (text);
      return script_return_obj_null ();
    }
  for (count = 0; count < index; count++)
    {
      if (text[count] == '\0')
        {
          free (text);
          return script_return_obj(script_obj_new_string (""));
        }
    }
  charstring[0] = text[index];
  charstring[1] = '\0';
  free (text);
  return script_return_obj(script_obj_new_string (charstring));
}

static script_return_t script_lib_string_sub_string (script_state_t *state,
                                                     void           *user_data)
{
  char *text = script_obj_as_string (state->this);
  int start = script_obj_hash_get_number (state->local, "start");
  int end = script_obj_hash_get_number (state->local, "end");
  int text_count;
  char* substring;
  script_obj_t *substring_obj;

  if (!text || start < 0 || end < start)
    {
      free (text);
      return script_return_obj_null ();
    }

  for (text_count = 0; text_count < start; text_count++)
    {
      if (text[text_count] == '\0')
        {
          free (text);
          return script_return_obj(script_obj_new_string (""));
        }
    }

  substring = strndup(&text[text_count], end - start);
  substring_obj = script_obj_new_string (substring);
  free (substring);
  free (text);
  return script_return_obj(substring_obj);
}

script_lib_string_data_t *script_lib_string_setup (script_state_t *state)
{
  script_lib_string_data_t *data = malloc (sizeof (script_lib_string_data_t));

  script_obj_t *string_hash = script_obj_hash_get_element (state->global, "String");
  script_add_native_function (string_hash,
                              "CharAt",
                              script_lib_string_char_at,
                              NULL,
                              "index",
                              NULL);
  script_add_native_function (string_hash,
                              "SubString",
                              script_lib_string_sub_string,
                              NULL,
                              "start",
                              "end",
                              NULL);
  script_obj_unref (string_hash);
  data->script_main_op = script_parse_string (script_lib_string_string, "script-lib-string.script");
  script_return_t ret = script_execute (state, data->script_main_op);
  script_obj_unref (ret.object);

  return data;
}

void script_lib_string_destroy (script_lib_string_data_t *data)
{
  script_parse_op_free (data->script_main_op);
  free (data);
}

