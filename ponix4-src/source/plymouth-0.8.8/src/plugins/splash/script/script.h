/* script.h - scripting system structures
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
#ifndef SCRIPT_H
#define SCRIPT_H

#include "ply-hashtable.h"
#include "ply-list.h"
#include <stdbool.h>

typedef enum                        /* FIXME add _t to all types */
{
  SCRIPT_RETURN_TYPE_NORMAL,
  SCRIPT_RETURN_TYPE_RETURN,
  SCRIPT_RETURN_TYPE_FAIL,
  SCRIPT_RETURN_TYPE_BREAK,
  SCRIPT_RETURN_TYPE_CONTINUE,
} script_return_type_t;

struct script_obj_t;

typedef struct
{
  script_return_type_t type;
  struct script_obj_t *object;
} script_return_t;

typedef struct
{
  void *user_data;
  struct script_obj_t *global;
  struct script_obj_t *local;
  struct script_obj_t *this;
} script_state_t;

typedef enum
{
  SCRIPT_FUNCTION_TYPE_SCRIPT,
  SCRIPT_FUNCTION_TYPE_NATIVE,
} script_function_type_t;

typedef script_return_t (*script_native_function_t)(script_state_t *, void *);

typedef struct script_function_t
{
  script_function_type_t type;
  ply_list_t *parameters;           /*  list of char* typedef names */
  void *user_data;
  union
  {
    script_native_function_t native;
    struct script_op_t *script;
  } data;
  bool freeable;
} script_function_t;

typedef void (*script_obj_function_t)(struct script_obj_t *);

typedef struct
{
  script_obj_function_t free_func;
  char *name;
  void *user_data;
} script_obj_native_class_t;

typedef double script_number_t;

typedef struct
{
  void *object_data;
  script_obj_native_class_t *class;
} script_obj_native_t;

typedef enum
{
  SCRIPT_OBJ_TYPE_NULL,
  SCRIPT_OBJ_TYPE_REF,
  SCRIPT_OBJ_TYPE_EXTEND,
  SCRIPT_OBJ_TYPE_NUMBER,
  SCRIPT_OBJ_TYPE_STRING,
  SCRIPT_OBJ_TYPE_HASH,
  SCRIPT_OBJ_TYPE_FUNCTION,
  SCRIPT_OBJ_TYPE_NATIVE,
} script_obj_type_t;

typedef struct script_obj_t
{
  script_obj_type_t type;
  int refcount;
  union
  {
    script_number_t number;
    char *string;
    struct script_obj_t *obj;
    struct
      {
        struct script_obj_t *obj_a;
        struct script_obj_t *obj_b;
      } dual_obj;
    script_function_t *function;
    ply_hashtable_t *hash;
    script_obj_native_t native;
  } data;
} script_obj_t;

typedef enum
{
  SCRIPT_EXP_TYPE_TERM_NULL,
  SCRIPT_EXP_TYPE_TERM_NUMBER,
  SCRIPT_EXP_TYPE_TERM_STRING,
  SCRIPT_EXP_TYPE_TERM_VAR,
  SCRIPT_EXP_TYPE_TERM_LOCAL,
  SCRIPT_EXP_TYPE_TERM_GLOBAL,
  SCRIPT_EXP_TYPE_TERM_THIS,
  SCRIPT_EXP_TYPE_TERM_SET,
  SCRIPT_EXP_TYPE_PLUS,
  SCRIPT_EXP_TYPE_MINUS,
  SCRIPT_EXP_TYPE_MUL,
  SCRIPT_EXP_TYPE_DIV,
  SCRIPT_EXP_TYPE_MOD,
  SCRIPT_EXP_TYPE_GT,
  SCRIPT_EXP_TYPE_GE,
  SCRIPT_EXP_TYPE_LT,
  SCRIPT_EXP_TYPE_LE,
  SCRIPT_EXP_TYPE_EQ,
  SCRIPT_EXP_TYPE_NE,
  SCRIPT_EXP_TYPE_AND,
  SCRIPT_EXP_TYPE_OR,
  SCRIPT_EXP_TYPE_EXTEND,
  SCRIPT_EXP_TYPE_NOT,
  SCRIPT_EXP_TYPE_POS,
  SCRIPT_EXP_TYPE_NEG,
  SCRIPT_EXP_TYPE_PRE_INC,
  SCRIPT_EXP_TYPE_PRE_DEC,
  SCRIPT_EXP_TYPE_POST_INC,
  SCRIPT_EXP_TYPE_POST_DEC,
  SCRIPT_EXP_TYPE_HASH,
  SCRIPT_EXP_TYPE_FUNCTION_EXE,
  SCRIPT_EXP_TYPE_FUNCTION_DEF,
  SCRIPT_EXP_TYPE_ASSIGN,
  SCRIPT_EXP_TYPE_ASSIGN_PLUS,
  SCRIPT_EXP_TYPE_ASSIGN_MINUS,
  SCRIPT_EXP_TYPE_ASSIGN_MUL,
  SCRIPT_EXP_TYPE_ASSIGN_DIV,
  SCRIPT_EXP_TYPE_ASSIGN_MOD,
  SCRIPT_EXP_TYPE_ASSIGN_EXTEND,
} script_exp_type_t;

typedef struct script_exp_t
{
  script_exp_type_t type;
  union
  {
    struct
    {
      struct script_exp_t *sub_a;
      struct script_exp_t *sub_b;
    } dual;
    struct script_exp_t *sub;
    char *string;
    script_number_t number;
    struct
    {
      struct script_exp_t *name;
      ply_list_t *parameters;
    } function_exe;
    ply_list_t *parameters;
    script_function_t *function_def;
  } data;
} script_exp_t;

typedef enum
{
  SCRIPT_OP_TYPE_EXPRESSION,
  SCRIPT_OP_TYPE_OP_BLOCK,
  SCRIPT_OP_TYPE_IF,
  SCRIPT_OP_TYPE_WHILE,
  SCRIPT_OP_TYPE_DO_WHILE,
  SCRIPT_OP_TYPE_FOR,
  SCRIPT_OP_TYPE_RETURN,
  SCRIPT_OP_TYPE_FAIL,
  SCRIPT_OP_TYPE_BREAK,
  SCRIPT_OP_TYPE_CONTINUE,
} script_op_type_t;

typedef struct script_op_t
{
  script_op_type_t type;
  union
  {
    script_exp_t *exp;
    ply_list_t *list;
    struct
    {
      script_exp_t *cond;
      struct script_op_t *op1;
      struct script_op_t *op2;
    } cond_op;
  } data;
} script_op_t;

typedef struct
{
  char *name;
  script_obj_t *object;
} script_variable_t;


#define script_return_obj(_return_object) ((script_return_t) {SCRIPT_RETURN_TYPE_RETURN, _return_object})
#define script_return_obj_null() ((script_return_t) {SCRIPT_RETURN_TYPE_RETURN, script_obj_new_null ()})
#define script_return_fail() ((script_return_t) {SCRIPT_RETURN_TYPE_FAIL, NULL})
#define script_return_normal() ((script_return_t) {SCRIPT_RETURN_TYPE_NORMAL, NULL})
#define script_return_normal_obj(_return_object) ((script_return_t) {SCRIPT_RETURN_TYPE_NORMAL, _return_object})
#define script_return_break() ((script_return_t) {SCRIPT_RETURN_TYPE_BREAK, NULL})
#define script_return_continue() ((script_return_t) {SCRIPT_RETURN_TYPE_CONTINUE, NULL})

script_function_t *script_function_script_new (script_op_t  *script,
                                               void         *user_data,
                                               ply_list_t   *parameter_list);
script_function_t *script_function_native_new (script_native_function_t  native_function,
                                               void                     *user_data,
                                               ply_list_t               *parameter_list);
void script_add_native_function (script_obj_t            *hash,
                                 const char              *name,
                                 script_native_function_t native_function,
                                 void                    *user_data,
                                 const char              *first_arg,
                                 ...);
script_obj_native_class_t *script_obj_native_class_new (script_obj_function_t free_func,
                                                        const char           *name,
                                                        void                 *user_data);

void script_obj_native_class_destroy (script_obj_native_class_t * class);
script_state_t *script_state_new (void *user_data);
script_state_t *script_state_init_sub (script_state_t *oldstate, script_obj_t *this);
void script_state_destroy (script_state_t *state);

#endif /* SCRIPT_H */
