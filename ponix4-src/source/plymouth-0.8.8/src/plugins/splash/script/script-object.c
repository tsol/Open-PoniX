/* script-object.c - functions to work with script objects
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
#include <math.h>
#include <values.h>

#include "script.h"
#include "script-object.h"

void script_obj_reset (script_obj_t *obj);

void script_obj_free (script_obj_t *obj)
{
  assert (!obj->refcount);
  script_obj_reset (obj);
  free (obj);
}

void script_obj_ref (script_obj_t *obj)
{
  obj->refcount++;
}

void script_obj_unref (script_obj_t *obj)
{
  if (!obj) return;
  assert (obj->refcount > 0);
  obj->refcount--;
  if (obj->refcount <= 0)
    script_obj_free (obj);
}

static void foreach_free_variable (void *key,
                                   void *data,
                                   void *user_data)
{
  script_variable_t *variable = data;

  script_obj_unref (variable->object);
  free (variable->name);
  free (variable);
}

void script_obj_reset (script_obj_t *obj)
{
  switch (obj->type)
    {
      case SCRIPT_OBJ_TYPE_REF:
        script_obj_unref (obj->data.obj);
        break;

      case SCRIPT_OBJ_TYPE_EXTEND:
        script_obj_unref (obj->data.dual_obj.obj_a);
        script_obj_unref (obj->data.dual_obj.obj_b);
        break;

      case SCRIPT_OBJ_TYPE_NUMBER:
        break;

      case SCRIPT_OBJ_TYPE_STRING:
        free (obj->data.string);
        break;

      case SCRIPT_OBJ_TYPE_HASH:                /* FIXME nightmare */
        ply_hashtable_foreach (obj->data.hash, foreach_free_variable, NULL);
        ply_hashtable_free (obj->data.hash);
        break;

      case SCRIPT_OBJ_TYPE_FUNCTION:
        {
          if (obj->data.function->freeable)
            {
              ply_list_node_t *node;
              for (node =
                     ply_list_get_first_node (obj->data.function->parameters);
                   node;
                   node =
                     ply_list_get_next_node (obj->data.function->parameters,
                                             node))
                {
                  char *operand = ply_list_node_get_data (node);
                  free (operand);
                }
              ply_list_free (obj->data.function->parameters);
              free (obj->data.function);
            }
        }
        break;

      case SCRIPT_OBJ_TYPE_NATIVE:
        if (obj->data.native.class->free_func)
          obj->data.native.class->free_func (obj);
        break;

      case SCRIPT_OBJ_TYPE_NULL:
        break;
    }
  obj->type = SCRIPT_OBJ_TYPE_NULL;
}

script_obj_t *script_obj_deref_direct (script_obj_t *obj)
{
  while (obj->type == SCRIPT_OBJ_TYPE_REF)
    obj = obj->data.obj;
  return obj;
}

void script_obj_deref (script_obj_t **obj_ptr)
{
  script_obj_t *obj = *obj_ptr;

  obj = script_obj_deref_direct (obj);
  script_obj_ref (obj);
  script_obj_unref (*obj_ptr);
  *obj_ptr = obj;
}

script_obj_t *script_obj_new_null (void)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));

  obj->type = SCRIPT_OBJ_TYPE_NULL;
  obj->refcount = 1;
  return obj;
}

script_obj_t *script_obj_new_number (script_number_t number)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));
  obj->type = SCRIPT_OBJ_TYPE_NUMBER;
  obj->refcount = 1;
  obj->data.number = number;
  return obj;
}

script_obj_t *script_obj_new_string (const char *string)
{
  if (!string) return script_obj_new_null ();
  script_obj_t *obj = malloc (sizeof (script_obj_t));
  obj->type = SCRIPT_OBJ_TYPE_STRING;
  obj->refcount = 1;
  obj->data.string = strdup (string);
  return obj;
}

script_obj_t *script_obj_new_hash (void)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));

  obj->type = SCRIPT_OBJ_TYPE_HASH;
  obj->data.hash = ply_hashtable_new (ply_hashtable_string_hash,
                                      ply_hashtable_string_compare);
  obj->refcount = 1;
  return obj;
}

script_obj_t *script_obj_new_function (script_function_t *function)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));

  obj->type = SCRIPT_OBJ_TYPE_FUNCTION;
  obj->data.function = function;
  obj->refcount = 1;
  return obj;
}

script_obj_t *script_obj_new_ref (script_obj_t *sub_obj)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));
  sub_obj = script_obj_deref_direct (sub_obj);
  script_obj_ref (sub_obj);
  obj->type = SCRIPT_OBJ_TYPE_REF;
  obj->data.obj = sub_obj;
  obj->refcount = 1;
  return obj;
}

script_obj_t *script_obj_new_extend (script_obj_t *obj_a, script_obj_t *obj_b)
{
  script_obj_t *obj = malloc (sizeof (script_obj_t));
  obj_a = script_obj_deref_direct (obj_a);
  obj_b = script_obj_deref_direct (obj_b);
  script_obj_ref (obj_a);
  script_obj_ref (obj_b);
  obj->type = SCRIPT_OBJ_TYPE_EXTEND;
  obj->data.dual_obj.obj_a = obj_a;
  obj->data.dual_obj.obj_b = obj_b;
  obj->refcount = 1;
  return obj;
}

script_obj_t *script_obj_new_native (void                      *object_data,
                                     script_obj_native_class_t *class)
{
  if (!object_data) return script_obj_new_null ();
  script_obj_t *obj = malloc (sizeof (script_obj_t));
  obj->type = SCRIPT_OBJ_TYPE_NATIVE;
  obj->data.native.class = class;
  obj->data.native.object_data = object_data;
  obj->refcount = 1;
  return obj;
}

void *script_obj_as_custom (script_obj_t             *obj,
                            script_obj_direct_func_t  user_func,
                            void                     *user_data)
{
  obj = script_obj_deref_direct (obj);
  void *reply = user_func(obj, user_data);
  if (reply) return reply;
  if (obj->type == SCRIPT_OBJ_TYPE_EXTEND)
    {
      reply = script_obj_as_custom(obj->data.dual_obj.obj_a, user_func, user_data);
      if (reply) return reply;
      reply = script_obj_as_custom(obj->data.dual_obj.obj_b, user_func, user_data);
      if (reply) return reply;
    }
  return NULL;
}

script_obj_t *script_obj_as_obj_type (script_obj_t      *obj,
                                      script_obj_type_t  type)
{
  obj = script_obj_deref_direct (obj);
  if (obj->type == type) return obj;
  if (obj->type == SCRIPT_OBJ_TYPE_EXTEND)
    {
      script_obj_t *reply;
      reply = script_obj_as_obj_type(obj->data.dual_obj.obj_a, type);
      if (reply) return reply;
      reply = script_obj_as_obj_type(obj->data.dual_obj.obj_b, type);
      if (reply) return reply;
    }
  return NULL;
}

script_number_t script_obj_as_number (script_obj_t *obj)
{                                                     /* If in then reply contents, otherwise reply NAN */
  obj = script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_NUMBER);
  if (obj) return obj->data.number;
  return NAN;
}

static void *script_obj_direct_as_bool (script_obj_t *obj,
                                        void         *user_data)
{                                                 /* False objects are NULL, 0, NAN, "" */
  switch (obj->type)
    {
      case SCRIPT_OBJ_TYPE_NUMBER:
        {
          int num_type = fpclassify(obj->data.number);
          if (num_type == FP_ZERO || num_type == FP_NAN) return NULL;
          return obj;
        }
      case SCRIPT_OBJ_TYPE_NULL:
      case SCRIPT_OBJ_TYPE_REF:
      case SCRIPT_OBJ_TYPE_EXTEND:
        return NULL;
      case SCRIPT_OBJ_TYPE_HASH:
      case SCRIPT_OBJ_TYPE_FUNCTION:
      case SCRIPT_OBJ_TYPE_NATIVE:
        return obj;
      case SCRIPT_OBJ_TYPE_STRING:
        if (*obj->data.string) return obj;
        return NULL;
    }
  return NULL;
}

bool script_obj_as_bool (script_obj_t *obj)
{
  return script_obj_as_custom(obj, script_obj_direct_as_bool, NULL);
}

char *script_obj_as_string (script_obj_t *obj)              /* reply is strdupped and may be NULL */
{
  char *reply;        
  script_obj_t *string_obj = script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_STRING);
  if (string_obj) return strdup (string_obj->data.string);
  string_obj = script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_NUMBER);
  if (string_obj)
    {
        asprintf (&reply, "%g", string_obj->data.number);
        return reply;
    }
  if (script_obj_is_null (obj))
    return strdup("#NULL");
  asprintf (&reply, "#(0x%p)", obj);
  return reply;
}

static void *script_obj_direct_as_native_of_class (script_obj_t *obj,
                                                   void         *user_data)
{
  script_obj_native_class_t *class = user_data;
  if (obj->type == SCRIPT_OBJ_TYPE_NATIVE && obj->data.native.class == class)
    return obj->data.native.object_data;
  return NULL;
}

void *script_obj_as_native_of_class (script_obj_t              *obj,
                                     script_obj_native_class_t *class)
{
  return script_obj_as_custom(obj, script_obj_direct_as_native_of_class, class);
}

static void *script_obj_direct_as_native_of_class_name (script_obj_t *obj,
                                                        void         *user_data)
{
  const char *class_name = user_data;
  if (obj->type == SCRIPT_OBJ_TYPE_NATIVE &&
      !strcmp (obj->data.native.class->name, class_name))
    return obj->data.native.object_data;
  return NULL;
}

void *script_obj_as_native_of_class_name (script_obj_t *obj,
                                          const char   *class_name)
{
  return script_obj_as_custom(obj, 
                              script_obj_direct_as_native_of_class_name,
                              (void*) class_name);
}

bool script_obj_is_null (script_obj_t *obj)
{
  return script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_NULL);
}

bool script_obj_is_number (script_obj_t *obj)
{
  return script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_NUMBER);
}

bool script_obj_is_string (script_obj_t *obj)
{
  return script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_STRING);
}

bool script_obj_is_hash (script_obj_t *obj)
{
  return script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_HASH);
}

bool script_obj_is_native (script_obj_t *obj)
{
  return script_obj_as_obj_type (obj, SCRIPT_OBJ_TYPE_NATIVE);
}

bool script_obj_is_native_of_class (script_obj_t              *obj,
                                    script_obj_native_class_t *class)
{
  return script_obj_as_custom(obj, 
                              script_obj_direct_as_native_of_class,
                              class);
}

bool script_obj_is_native_of_class_name (script_obj_t *obj,
                                         const char   *class_name)
{
  return script_obj_as_custom(obj, 
                              script_obj_direct_as_native_of_class_name,
                              (void*)class_name);
}

void script_obj_assign (script_obj_t *obj_a,
                        script_obj_t *obj_b)
{
  obj_b = script_obj_deref_direct (obj_b);
  script_obj_ref (obj_b);
  script_obj_reset (obj_a);
  obj_a->type = SCRIPT_OBJ_TYPE_REF;
  obj_a->data.obj = obj_b;
}

static void *script_obj_direct_as_hash_element (script_obj_t *obj,
                                                void         *user_data)
{
  const char *name = user_data;
  if (obj->type == SCRIPT_OBJ_TYPE_HASH)
    {
      script_variable_t *variable = ply_hashtable_lookup (obj->data.hash, (void *) name);
      if (variable)
        return variable->object;
    }
  return NULL;
}

script_obj_t *script_obj_hash_peek_element (script_obj_t *hash,
                                            const char   *name)
{
  script_obj_t *object;
  if (!name) return script_obj_new_null ();
  object = script_obj_as_custom(hash,
                                script_obj_direct_as_hash_element,
                                (void*) name);
  if (object) script_obj_ref (object);
  return object;
}

script_obj_t *script_obj_hash_get_element (script_obj_t *hash,
                                           const char   *name)
{
  script_obj_t *obj = script_obj_hash_peek_element (hash, name);
  if (obj) return obj;
  script_obj_t *realhash = script_obj_as_obj_type (hash, SCRIPT_OBJ_TYPE_HASH);
  if (!realhash)
    {
      realhash = script_obj_new_hash();   /* If it wasn't a hash then make it into one */
      script_obj_assign (hash, realhash);
    }
  script_variable_t *variable = malloc (sizeof (script_variable_t));
  variable->name = strdup (name);
  variable->object = script_obj_new_null ();
  ply_hashtable_insert (realhash->data.hash, variable->name, variable);
  script_obj_ref (variable->object);
  return variable->object;
}

script_number_t script_obj_hash_get_number (script_obj_t *hash,
                                            const char   *name)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  script_number_t reply = script_obj_as_number (obj);

  script_obj_unref (obj);
  return reply;
}

bool script_obj_hash_get_bool (script_obj_t *hash,
                               const char   *name)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  bool reply = script_obj_as_bool (obj);

  script_obj_unref (obj);
  return reply;
}

char *script_obj_hash_get_string (script_obj_t *hash,
                                  const char   *name)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  char *reply = script_obj_as_string (obj);

  script_obj_unref (obj);
  return reply;
}

void *script_obj_hash_get_native_of_class (script_obj_t              *hash,
                                           const char                *name,
                                           script_obj_native_class_t *class)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  void *reply = script_obj_as_native_of_class (obj, class);

  script_obj_unref (obj);
  return reply;
}

void *script_obj_hash_get_native_of_class_name (script_obj_t *hash,
                                                const char   *name,
                                                const char   *class_name)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  void *reply = script_obj_as_native_of_class_name (obj, class_name);

  script_obj_unref (obj);
  return reply;
}

void script_obj_hash_add_element (script_obj_t *hash,
                                  script_obj_t *element,
                                  const char   *name)
{
  script_obj_t *obj = script_obj_hash_get_element (hash, name);
  script_obj_assign (obj, element);
  script_obj_unref (obj);
}

script_obj_t *script_obj_plus (script_obj_t *script_obj_a,
                               script_obj_t *script_obj_b)
{
  if (script_obj_is_number (script_obj_a) && script_obj_is_number (script_obj_b))
    {
      script_number_t value = script_obj_as_number (script_obj_a) + script_obj_as_number (script_obj_b);
      return script_obj_new_number (value);
    }
  if (script_obj_is_string (script_obj_a) || script_obj_is_string (script_obj_b))
    {
      script_obj_t *obj;
      char *string_a = script_obj_as_string (script_obj_a);
      char *string_b = script_obj_as_string (script_obj_b);
      if (string_a && string_b)
        {
          char *newstring;
          asprintf (&newstring, "%s%s", string_a, string_b);
          obj = script_obj_new_string (newstring);
          free (newstring);
        }
      else
        obj = script_obj_new_null ();
      free (string_a);
      free (string_b);
      return obj;
    }
  return script_obj_new_null ();
}

script_obj_t *script_obj_minus (script_obj_t *script_obj_a,
                                script_obj_t *script_obj_b)
{
  if (script_obj_is_number (script_obj_a) && script_obj_is_number (script_obj_b))
    {
      script_number_t value = script_obj_as_number (script_obj_a) - script_obj_as_number (script_obj_b);
      return script_obj_new_number (value);
    }
  return script_obj_new_null ();
}

script_obj_t *script_obj_mul (script_obj_t *script_obj_a,
                              script_obj_t *script_obj_b)
{
  if (script_obj_is_number (script_obj_a) && script_obj_is_number (script_obj_b))
    {
      script_number_t value = script_obj_as_number (script_obj_a) * script_obj_as_number (script_obj_b);
      return script_obj_new_number (value);
    }
  return script_obj_new_null ();
}

script_obj_t *script_obj_div (script_obj_t *script_obj_a,
                              script_obj_t *script_obj_b)
{
  if (script_obj_is_number (script_obj_a) && script_obj_is_number (script_obj_b))
    {
      script_number_t value = script_obj_as_number (script_obj_a) / script_obj_as_number (script_obj_b);
      return script_obj_new_number (value);
    }
  return script_obj_new_null ();
}

script_obj_t *script_obj_mod (script_obj_t *script_obj_a,
                              script_obj_t *script_obj_b)
{
  if (script_obj_is_number (script_obj_a) && script_obj_is_number (script_obj_b))
    {
      script_number_t value = fmodl (script_obj_as_number (script_obj_a), script_obj_as_number (script_obj_b));
      return script_obj_new_number (value);
    }
  return script_obj_new_null ();
}


script_obj_cmp_result_t script_obj_cmp (script_obj_t *script_obj_a,
                                        script_obj_t *script_obj_b)
{
  if (script_obj_is_null (script_obj_a) && script_obj_is_null (script_obj_b))
    {
      return SCRIPT_OBJ_CMP_RESULT_EQ;
    }
  else if (script_obj_is_number (script_obj_a))
    {
      if (script_obj_is_number (script_obj_b))
        {
          script_number_t num_a = script_obj_as_number (script_obj_a);
          script_number_t num_b = script_obj_as_number (script_obj_b);

          if (num_a < num_b) return SCRIPT_OBJ_CMP_RESULT_LT;
          if (num_a > num_b) return SCRIPT_OBJ_CMP_RESULT_GT;
          if (num_a == num_b) return SCRIPT_OBJ_CMP_RESULT_EQ;
          return SCRIPT_OBJ_CMP_RESULT_NE;
        }
    }
  else if (script_obj_is_string (script_obj_a))
    {
      if (script_obj_is_string (script_obj_b))
        {
          char* string_a = script_obj_as_string (script_obj_a);
          char* string_b = script_obj_as_string (script_obj_b);
          int diff = strcmp (string_a, string_b);
          free(string_a);
          free(string_b);
          if (diff < 0) return SCRIPT_OBJ_CMP_RESULT_LT;
          if (diff > 0) return SCRIPT_OBJ_CMP_RESULT_GT;
          return SCRIPT_OBJ_CMP_RESULT_EQ;
        }
    }
  else if (script_obj_deref_direct (script_obj_a) == script_obj_deref_direct (script_obj_b))
        return SCRIPT_OBJ_CMP_RESULT_EQ;
  return SCRIPT_OBJ_CMP_RESULT_NE;
}

