/* script-object.h - functions to work with script objects
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
#ifndef SCRIPT_OBJECT_H
#define SCRIPT_OBJECT_H

#include "script.h"
#include <stdbool.h>


typedef enum
{
  SCRIPT_OBJ_CMP_RESULT_EQ = 1<<1,
  SCRIPT_OBJ_CMP_RESULT_GT = 1<<2,
  SCRIPT_OBJ_CMP_RESULT_LT = 1<<3,
  SCRIPT_OBJ_CMP_RESULT_NE = 1<<4,
} script_obj_cmp_result_t;


typedef void *(*script_obj_direct_func_t)(script_obj_t *, void *);


void script_obj_free (script_obj_t *obj);
void script_obj_ref (script_obj_t *obj);
void script_obj_unref (script_obj_t *obj);
void script_obj_reset (script_obj_t *obj);
script_obj_t *script_obj_deref_direct (script_obj_t *obj);
void script_obj_deref (script_obj_t **obj_ptr);
script_obj_t *script_obj_new_number (script_number_t number);
script_obj_t *script_obj_new_string (const char *string);
script_obj_t *script_obj_new_null (void);
script_obj_t *script_obj_new_hash (void);
script_obj_t *script_obj_new_function (script_function_t *function);
script_obj_t *script_obj_new_ref (script_obj_t *sub_obj);
script_obj_t *script_obj_new_extend (script_obj_t *obj_a, script_obj_t *obj_b);

script_obj_t *script_obj_new_native (void                       *object_data,
                                     script_obj_native_class_t  *class );
void *script_obj_as_custom (script_obj_t             *obj,
                            script_obj_direct_func_t  user_func,
                            void                     *user_data);
script_obj_t *script_obj_as_obj_type (script_obj_t      *obj,
                                      script_obj_type_t  type);
script_number_t script_obj_as_number (script_obj_t *obj);
bool script_obj_as_bool (script_obj_t *obj);
char *script_obj_as_string (script_obj_t *obj);
void *script_obj_as_native_of_class (script_obj_t              *obj,
                                     script_obj_native_class_t *class );
void *script_obj_as_native_of_class_name (script_obj_t *obj,
                                          const char   *class_name);
bool script_obj_is_null (script_obj_t *obj);
bool script_obj_is_number (script_obj_t *obj);
bool script_obj_is_string (script_obj_t *obj);
bool script_obj_is_hash (script_obj_t *obj);
bool script_obj_is_native (script_obj_t *obj);

bool script_obj_is_native_of_class (script_obj_t * obj,
                                    script_obj_native_class_t *class );
bool script_obj_is_native_of_class_name (script_obj_t *obj,
                                         const char   *class_name);
void script_obj_assign (script_obj_t *obj_a,
                        script_obj_t *obj_b);
script_obj_t *script_obj_hash_peek_element (script_obj_t *hash,
                                            const char   *name);
script_obj_t *script_obj_hash_get_element (script_obj_t *hash,
                                           const char   *name);
script_number_t script_obj_hash_get_number (script_obj_t *hash,
                                            const char   *name);
bool script_obj_hash_get_bool (script_obj_t *hash,
                               const char   *name);
char *script_obj_hash_get_string (script_obj_t *hash,
                                  const char   *name);
void *script_obj_hash_get_native_of_class (script_obj_t *hash,
                                           const char   *name,
                                           script_obj_native_class_t *class );
void *script_obj_hash_get_native_of_class_name (script_obj_t *hash,
                                                const char   *name,
                                                const char   *class_name);
void script_obj_hash_add_element (script_obj_t *hash,
                                  script_obj_t *element,
                                  const char   *name);
script_obj_t *script_obj_plus (script_obj_t *script_obj_a_in,
                               script_obj_t *script_obj_b_in);
script_obj_t *script_obj_minus (script_obj_t *script_obj_a_in,
                                script_obj_t *script_obj_b_in);
script_obj_t *script_obj_mul (script_obj_t *script_obj_a_in,
                              script_obj_t *script_obj_b_in);
script_obj_t *script_obj_div (script_obj_t *script_obj_a_in,
                              script_obj_t *script_obj_b_in);
script_obj_t *script_obj_mod (script_obj_t *script_obj_a_in,
                              script_obj_t *script_obj_b_in);
script_obj_cmp_result_t script_obj_cmp (script_obj_t *script_obj_a,
                                        script_obj_t *script_obj_b);
#endif /* SCRIPT_OBJECT_H */
