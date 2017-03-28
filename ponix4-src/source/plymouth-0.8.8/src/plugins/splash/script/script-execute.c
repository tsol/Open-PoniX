/* script-execute.c - execution of scripts
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
#include "ply-logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#include "script.h"
#include "script-debug.h"
#include "script-execute.h"
#include "script-object.h"

static script_obj_t *script_evaluate (script_state_t *state,
                                      script_exp_t   *exp);
static script_return_t script_execute_function_with_parlist (script_state_t    *state,
                                                             script_function_t *function,
                                                             script_obj_t      *this,
                                                             ply_list_t        *parameter_data);


static void script_execute_error (void       *element,
                                  const char *message)
{
  script_debug_location_t *location = script_debug_lookup_element (element);
  if (location)
    ply_error ("Execution error \"%s\" L:%d C:%d : %s\n",
               location->name,
               location->line_index,
               location->column_index,
               message);
  else
    ply_error ("Execution error: %s\n", message);
}


static script_obj_t *script_evaluate_apply_function (script_state_t *state,
                                                     script_exp_t   *exp,
                                                     script_obj_t   *(*function)(script_obj_t *, script_obj_t *))
{
  script_obj_t *script_obj_a = script_evaluate (state, exp->data.dual.sub_a);
  script_obj_t *script_obj_b = script_evaluate (state, exp->data.dual.sub_b);
  script_obj_t *obj = function (script_obj_a, script_obj_b);

  script_obj_unref (script_obj_a);
  script_obj_unref (script_obj_b);
  return obj;
}

static script_obj_t *script_evaluate_apply_function_and_assign (script_state_t *state,
                                                                script_exp_t   *exp,
                                                                script_obj_t   *(*function)(script_obj_t *, script_obj_t *))
{
  script_obj_t *script_obj_a = script_evaluate (state, exp->data.dual.sub_a);
  script_obj_t *script_obj_b = script_evaluate (state, exp->data.dual.sub_b);
  script_obj_t *obj = function (script_obj_a, script_obj_b);

  script_obj_assign (script_obj_a, obj);
  script_obj_unref (script_obj_a);
  script_obj_unref (script_obj_b);
  return obj;
}

static script_obj_t *script_evaluate_hash (script_state_t *state,
                                           script_exp_t   *exp)
{
  script_obj_t *hash = script_evaluate (state, exp->data.dual.sub_a);
  script_obj_t *key  = script_evaluate (state, exp->data.dual.sub_b);
  script_obj_t *obj;
  char *name = script_obj_as_string (key);

  if (!script_obj_is_hash(hash))
    {
      script_obj_t *newhash  = script_obj_new_hash ();
      script_obj_assign (hash, newhash);
      script_obj_unref (newhash);
    }
  
  obj = script_obj_hash_get_element (hash, name);
  free(name);
  
  script_obj_unref (hash);
  script_obj_unref (key);
  return obj;
}

static script_obj_t *script_evaluate_var (script_state_t *state,
                                          script_exp_t   *exp)
{
  char *name = exp->data.string;
  script_obj_t *obj = script_obj_hash_peek_element (state->local, name);
  if (obj) return obj;
  obj = script_obj_hash_peek_element (state->this, name);
  if (obj) return obj;
  obj = script_obj_hash_peek_element (state->global, name);
  if (obj) return obj;
  obj = script_obj_hash_get_element (state->local, name);
  return obj;
}

static script_obj_t *script_evaluate_set (script_state_t *state,
                                          script_exp_t   *exp)
{

  ply_list_t *parameter_data = exp->data.parameters;
  ply_list_node_t *node_data = ply_list_get_first_node (parameter_data);
  int index = 0;
  script_obj_t *obj = script_obj_new_hash ();
  while (node_data)
    {
      script_exp_t *data_exp = ply_list_node_get_data (node_data);
      script_obj_t *data_obj = script_evaluate (state, data_exp);
      char *name;
      asprintf (&name, "%d", index);
      index++;
      script_obj_hash_add_element (obj, data_obj, name);
      free(name);
      
      node_data = ply_list_get_next_node (parameter_data, node_data);
    }
  return obj;
}

static script_obj_t *script_evaluate_assign (script_state_t *state,
                                             script_exp_t   *exp)
{
  script_obj_t *script_obj_a = script_evaluate (state, exp->data.dual.sub_a);
  script_obj_t *script_obj_b = script_evaluate (state, exp->data.dual.sub_b);

  script_obj_assign (script_obj_a, script_obj_b);

  script_obj_unref (script_obj_b);
  return script_obj_a;
}

static script_obj_t *script_evaluate_cmp (script_state_t           *state,
                                          script_exp_t             *exp,
                                          script_obj_cmp_result_t   condition)
{
  script_obj_t *script_obj_a = script_evaluate (state, exp->data.dual.sub_a);
  script_obj_t *script_obj_b = script_evaluate (state, exp->data.dual.sub_b);
  script_obj_cmp_result_t cmp_result = script_obj_cmp(script_obj_a, script_obj_b);
  script_obj_unref (script_obj_a);
  script_obj_unref (script_obj_b);
  
  if (cmp_result & condition)
    return script_obj_new_number (1);
  return script_obj_new_number (0);
}

static script_obj_t *script_evaluate_logic (script_state_t *state,
                                            script_exp_t   *exp)
{
  script_obj_t *obj = script_evaluate (state, exp->data.dual.sub_a);

  if ((exp->type == SCRIPT_EXP_TYPE_AND) && !script_obj_as_bool (obj))
    return obj;
  else if (exp->type == SCRIPT_EXP_TYPE_OR && script_obj_as_bool (obj))
    return obj;
  script_obj_unref (obj);
  obj = script_evaluate (state, exp->data.dual.sub_b);
  return obj;
}

static script_obj_t *script_evaluate_unary (script_state_t *state,
                                            script_exp_t   *exp)
{
  script_obj_t *obj = script_evaluate (state, exp->data.sub);
  script_obj_t *new_obj;

  if (exp->type == SCRIPT_EXP_TYPE_NOT)
    {
      new_obj = script_obj_new_number (!script_obj_as_bool (obj));
      script_obj_unref (obj);
      return new_obj;
    }
  if (exp->type == SCRIPT_EXP_TYPE_POS)     /* FIXME what should happen on non number operands? */
    return obj;                             /* Does nothing, maybe just remove at parse stage */
  if (exp->type == SCRIPT_EXP_TYPE_NEG)
    {
      if (script_obj_is_number(obj))
        new_obj = script_obj_new_number (-script_obj_as_number (obj));
      else
        {
          script_execute_error(exp, "Cannot negate non number objects");
          new_obj = script_obj_new_null ();
        }
      script_obj_unref (obj);
      return new_obj;
    }
  int change_pre = 0;
  int change = -1;

  if ((exp->type == SCRIPT_EXP_TYPE_PRE_INC) ||
      (exp->type == SCRIPT_EXP_TYPE_POST_INC))
    change = 1;
  if ((exp->type == SCRIPT_EXP_TYPE_PRE_INC) ||
      (exp->type == SCRIPT_EXP_TYPE_PRE_DEC))
    change_pre = 1;

  if (script_obj_is_number(obj))
    {
      if (change_pre)
        {
          new_obj = script_obj_new_number (script_obj_as_number(obj) + change);
          script_obj_assign (obj, new_obj);
        }
      else
        {
          new_obj = script_obj_deref_direct (obj);
          script_obj_ref (new_obj);
          script_obj_t *new_obj2 = script_obj_new_number (script_obj_as_number(obj) + change);
          script_obj_assign (obj, new_obj2);
          script_obj_unref (new_obj2);
        }
    }
  else
    {
      script_execute_error(exp, "Cannot increment/decrement non number objects");
      new_obj = script_obj_new_null (); /* If performeing something like a=hash++; a and hash become NULL */
      script_obj_reset (obj);
    }
  script_obj_unref (obj);
  return new_obj;
}
typedef struct 
{
  script_state_t    *state;
  script_obj_t      *this;
  ply_list_t        *parameter_data;
} script_obj_execute_data_t;

static void *script_obj_execute (script_obj_t *obj,
                                 void         *user_data)
{
  script_obj_execute_data_t *execute_data = user_data;
  if (obj->type == SCRIPT_OBJ_TYPE_FUNCTION){
      script_function_t *function = obj->data.function;
      script_return_t reply = script_execute_function_with_parlist (execute_data->state,
                                                                    function,
                                                                    execute_data->this,
                                                                    execute_data->parameter_data);
      if (reply.type != SCRIPT_RETURN_TYPE_FAIL)
        return reply.object ? reply.object : script_obj_new_null ();
    }
  return NULL;
}

static script_return_t script_execute_object_with_parlist (script_state_t    *state,
                                                           script_obj_t      *obj,
                                                           script_obj_t      *this,
                                                           ply_list_t        *parameter_data)
{
  script_obj_execute_data_t execute_data;
  execute_data.state = state;
  execute_data.this = this;
  execute_data.parameter_data = parameter_data;

  obj = script_obj_as_custom (obj, script_obj_execute, &execute_data);

  if (obj) return script_return_obj(obj);
  return script_return_fail();
}

static script_obj_t *script_evaluate_func (script_state_t *state,
                                           script_exp_t   *exp)
{
  script_obj_t *this_obj = NULL; ;
  script_obj_t *func_obj;
  script_exp_t *name_exp = exp->data.function_exe.name;
  
  if (name_exp->type == SCRIPT_EXP_TYPE_HASH)
    {
      script_obj_t *this_key  = script_evaluate (state, name_exp->data.dual.sub_b);
      this_obj = script_evaluate (state, name_exp->data.dual.sub_a);
      char *this_key_name = script_obj_as_string (this_key);
      script_obj_unref (this_key);
      func_obj = script_obj_hash_peek_element (this_obj, this_key_name);

      if (!func_obj && script_obj_is_string (this_obj))
        {
          script_obj_t *string_hash = script_obj_hash_peek_element (state->global, "String");
          func_obj = script_obj_hash_peek_element (string_hash, this_key_name);
          script_obj_unref (string_hash);
        }

      if (!func_obj)
        func_obj = script_obj_hash_get_element (this_obj, this_key_name);

      free(this_key_name);
    }
  else if (name_exp->type == SCRIPT_EXP_TYPE_TERM_VAR)
    {
      char *name = name_exp->data.string;
      func_obj = script_obj_hash_peek_element (state->local, name);
      if (!func_obj)
        {
          func_obj = script_obj_hash_peek_element (state->this, name);
          if (func_obj)
            {
              this_obj = state->this;
              script_obj_ref(this_obj);
            }
          else
            {
              func_obj = script_obj_hash_peek_element (state->global, name);
              if (!func_obj) func_obj = script_obj_new_null ();
            }
        }
    }
  else
    {
      func_obj = script_evaluate (state, name_exp);
    }
  
  ply_list_t *parameter_expressions = exp->data.function_exe.parameters;
  ply_list_t *parameter_data = ply_list_new ();

  ply_list_node_t *node_expression = ply_list_get_first_node (parameter_expressions);
  while (node_expression)
    {
      script_exp_t *data_exp = ply_list_node_get_data (node_expression);
      script_obj_t *data_obj = script_evaluate (state, data_exp);
      ply_list_append_data (parameter_data, data_obj);
      node_expression = ply_list_get_next_node (parameter_expressions,
                                                node_expression);
    }

  script_return_t reply = script_execute_object_with_parlist (state, func_obj, this_obj, parameter_data);
    
  ply_list_node_t *node_data = ply_list_get_first_node (parameter_data);
  while (node_data)
    {
      script_obj_t *data_obj = ply_list_node_get_data (node_data);
      script_obj_unref (data_obj);
      node_data = ply_list_get_next_node (parameter_data, node_data);
    }
  ply_list_free (parameter_data);

  script_obj_unref (func_obj);
  if (this_obj) script_obj_unref (this_obj);

  return reply.object ? reply.object : script_obj_new_null ();
}

static script_obj_t *script_evaluate (script_state_t *state,
                                      script_exp_t   *exp)
{
  switch (exp->type)
    {
      case SCRIPT_EXP_TYPE_PLUS:
        {
          return script_evaluate_apply_function (state, exp, script_obj_plus);
        }
      case SCRIPT_EXP_TYPE_MINUS:
        {
          return script_evaluate_apply_function (state, exp, script_obj_minus);
        }

      case SCRIPT_EXP_TYPE_MUL:
        {
          return script_evaluate_apply_function (state, exp, script_obj_mul);
        }
      case SCRIPT_EXP_TYPE_DIV:
        {
          return script_evaluate_apply_function (state, exp, script_obj_div);
        }
      case SCRIPT_EXP_TYPE_MOD:
        {
          return script_evaluate_apply_function (state, exp, script_obj_mod);
        }

      case SCRIPT_EXP_TYPE_EQ:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_EQ);
        }
      case SCRIPT_EXP_TYPE_NE:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_NE |
                                                  SCRIPT_OBJ_CMP_RESULT_LT |
                                                  SCRIPT_OBJ_CMP_RESULT_GT);
        }
      case SCRIPT_EXP_TYPE_GT:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_GT);
        }
      case SCRIPT_EXP_TYPE_GE:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_GT |
                                                  SCRIPT_OBJ_CMP_RESULT_EQ);
        }
      case SCRIPT_EXP_TYPE_LT:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_LT);
        }
      case SCRIPT_EXP_TYPE_LE:
        {
          return script_evaluate_cmp (state, exp, SCRIPT_OBJ_CMP_RESULT_LT |
                                                  SCRIPT_OBJ_CMP_RESULT_EQ);
        }

      case SCRIPT_EXP_TYPE_AND:
      case SCRIPT_EXP_TYPE_OR:
        {
          return script_evaluate_logic (state, exp);
        }

      case SCRIPT_EXP_TYPE_EXTEND:
        {
          return script_evaluate_apply_function (state, exp, script_obj_new_extend);
        }

      case SCRIPT_EXP_TYPE_NOT:
      case SCRIPT_EXP_TYPE_POS:
      case SCRIPT_EXP_TYPE_NEG:
      case SCRIPT_EXP_TYPE_PRE_INC:
      case SCRIPT_EXP_TYPE_PRE_DEC:
      case SCRIPT_EXP_TYPE_POST_INC:
      case SCRIPT_EXP_TYPE_POST_DEC:
        {
          return script_evaluate_unary (state, exp);
        }

      case SCRIPT_EXP_TYPE_TERM_NUMBER:
        {
          return script_obj_new_number (exp->data.number);
        }

      case SCRIPT_EXP_TYPE_TERM_STRING:
        {
          return script_obj_new_string (exp->data.string);
        }

      case SCRIPT_EXP_TYPE_TERM_NULL:
        {
          return script_obj_new_null ();
        }

      case SCRIPT_EXP_TYPE_TERM_LOCAL:
        {
          script_obj_ref (state->local);
          return state->local;
        }

      case SCRIPT_EXP_TYPE_TERM_GLOBAL:
        {
          script_obj_ref (state->global);
          return state->global;
        }

      case SCRIPT_EXP_TYPE_TERM_THIS:
        {
          script_obj_ref (state->this);
          return state->this;
        }

      case SCRIPT_EXP_TYPE_TERM_SET:
        {
          return script_evaluate_set (state, exp);
        }

      case SCRIPT_EXP_TYPE_TERM_VAR:
        {
          return script_evaluate_var (state, exp);
        }

      case SCRIPT_EXP_TYPE_ASSIGN:
        {
          return script_evaluate_assign (state, exp);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_PLUS:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_plus);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_MINUS:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_minus);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_MUL:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_mul);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_DIV:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_div);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_MOD:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_mod);
        }

      case SCRIPT_EXP_TYPE_ASSIGN_EXTEND:
        {
          return script_evaluate_apply_function_and_assign (state,
                                                            exp,
                                                            script_obj_new_extend);
        }

      case SCRIPT_EXP_TYPE_HASH:
        {
          return script_evaluate_hash (state, exp);
        }

      case SCRIPT_EXP_TYPE_FUNCTION_EXE:
        {
          return script_evaluate_func (state, exp);
        }
      case SCRIPT_EXP_TYPE_FUNCTION_DEF:
        {
          return script_obj_new_function (exp->data.function_def);
        }
    }
  return script_obj_new_null ();
}

static script_return_t script_execute_list (script_state_t *state,
                                            ply_list_t   *op_list)                        /* FIXME script_execute returns the return obj */
{
  script_return_t reply = script_return_normal ();
  ply_list_node_t *node = ply_list_get_first_node (op_list);

  for (node = ply_list_get_first_node (op_list);
       node;
       node = ply_list_get_next_node (op_list, node))
    {
      script_op_t *op = ply_list_node_get_data (node);
      script_obj_unref (reply.object);
      reply = script_execute (state, op);
      switch (reply.type)
        {
          case SCRIPT_RETURN_TYPE_NORMAL:
            break;
          case SCRIPT_RETURN_TYPE_RETURN:
          case SCRIPT_RETURN_TYPE_FAIL:
          case SCRIPT_RETURN_TYPE_BREAK:
          case SCRIPT_RETURN_TYPE_CONTINUE:
            return reply;
        }
    }
  return reply;
}

/* parameter_data list should be freed by caller */
static script_return_t script_execute_function_with_parlist (script_state_t    *state,
                                                             script_function_t *function,
                                                             script_obj_t      *this,
                                                             ply_list_t        *parameter_data)
{
  script_state_t *sub_state = script_state_init_sub (state, this);
  ply_list_t *parameter_names = function->parameters;
  ply_list_node_t *node_name = ply_list_get_first_node (parameter_names);
  ply_list_node_t *node_data = ply_list_get_first_node (parameter_data);
  int index = 0;;
  script_obj_t *arg_obj = script_obj_new_hash ();
  
  while (node_data)
    {
      script_obj_t *data_obj = ply_list_node_get_data (node_data);
      char *name;
      asprintf (&name, "%d", index);
      index++;
      script_obj_hash_add_element (arg_obj, data_obj, name);
      free(name);
      
      if (node_name)
        {
          name = ply_list_node_get_data (node_name);
          script_obj_hash_add_element (sub_state->local, data_obj, name);
          node_name = ply_list_get_next_node (parameter_names, node_name);
        }
      node_data = ply_list_get_next_node (parameter_data, node_data);
    }

  script_obj_t *count_obj = script_obj_new_number (index);
  script_obj_hash_add_element (arg_obj, count_obj, "count");
  script_obj_hash_add_element (sub_state->local, arg_obj, "_args");
  script_obj_unref (count_obj);
  script_obj_unref (arg_obj);

  if (this)
    script_obj_hash_add_element (sub_state->local, this, "this");

  script_return_t reply;
  switch (function->type)
    {
      case SCRIPT_FUNCTION_TYPE_SCRIPT:
        {
          script_op_t *op = function->data.script;
          reply = script_execute (sub_state, op);
          break;
        }

      case SCRIPT_FUNCTION_TYPE_NATIVE:
        {
          reply = function->data.native (sub_state, function->user_data);
          break;
        }
    }
  script_state_destroy (sub_state);
  if (reply.type != SCRIPT_RETURN_TYPE_FAIL)
    reply.type = SCRIPT_RETURN_TYPE_RETURN;
  return reply;
}

script_return_t script_execute_object (script_state_t    *state,
                                       script_obj_t      *function,
                                       script_obj_t      *this,
                                       script_obj_t      *first_arg,
                                       ...)
{
  script_return_t reply;
  va_list args;
  script_obj_t *arg;
  ply_list_t *parameter_data = ply_list_new ();

  arg = first_arg;
  va_start (args, first_arg);
  while (arg)
    {
      ply_list_append_data (parameter_data, arg);
      arg = va_arg (args, script_obj_t *);
    }
  va_end (args);

  reply = script_execute_object_with_parlist (state, function, this, parameter_data);
  ply_list_free (parameter_data);

  return reply;
}

script_return_t script_execute (script_state_t *state,
                                script_op_t    *op)
{
  script_return_t reply = script_return_normal ();
  if (!op) return reply;
  switch (op->type)
    {
      case SCRIPT_OP_TYPE_EXPRESSION:
        {
          reply.object = script_evaluate (state, op->data.exp);
          break;
        }

      case SCRIPT_OP_TYPE_OP_BLOCK:
        {
          reply = script_execute_list (state, op->data.list);
          break;
        }

      case SCRIPT_OP_TYPE_IF:
        {
          script_obj_t *obj = script_evaluate (state, op->data.cond_op.cond);
          if (script_obj_as_bool (obj))
            reply = script_execute (state, op->data.cond_op.op1);
          else
            reply = script_execute (state, op->data.cond_op.op2);
          script_obj_unref (obj);
          break;
        }

      case SCRIPT_OP_TYPE_DO_WHILE:
      case SCRIPT_OP_TYPE_WHILE:
      case SCRIPT_OP_TYPE_FOR:
        {
          script_obj_t *obj = NULL;
          bool cond = false;
          if (op->type == SCRIPT_OP_TYPE_DO_WHILE) cond = true;
          while (1)
            {
              if (!cond)
                {
                  obj = script_evaluate (state, op->data.cond_op.cond);
                  cond = script_obj_as_bool (obj);
                  script_obj_unref (obj);
                }
               
              if (cond)
                {
                  script_obj_unref (reply.object);
                  reply = script_execute (state, op->data.cond_op.op1);
                  switch (reply.type)
                    {
                      case SCRIPT_RETURN_TYPE_NORMAL:
                        break;

                      case SCRIPT_RETURN_TYPE_RETURN:
                      case SCRIPT_RETURN_TYPE_FAIL:
                        return reply;

                      case SCRIPT_RETURN_TYPE_BREAK:
                        return script_return_normal();

                      case SCRIPT_RETURN_TYPE_CONTINUE:
                        break;
                    }
                  if (op->data.cond_op.op2)
                    {
                      script_obj_unref (reply.object);
                      reply = script_execute (state, op->data.cond_op.op2);
                    }
                }
              else
                {
                  break;
                }
              cond = false;
            }
          break;
        }

      case SCRIPT_OP_TYPE_RETURN:
        {
          script_obj_t *obj;
          if (op->data.exp) obj = script_evaluate (state, op->data.exp);
          else obj = script_obj_new_null ();
          reply = script_return_obj (obj);
          break;
        }

      case SCRIPT_OP_TYPE_FAIL:
        {
          reply = script_return_fail ();
          break;
        }

      case SCRIPT_OP_TYPE_BREAK:
        {
          reply = script_return_break ();
          break;
        }

      case SCRIPT_OP_TYPE_CONTINUE:
        {
          reply = script_return_continue ();
          break;
        }
    }
  return reply;
}

