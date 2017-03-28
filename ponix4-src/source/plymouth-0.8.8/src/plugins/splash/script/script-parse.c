/* script-parse.c - parser for reading in script files
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
#include "ply-logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdbool.h>

#include "script-debug.h"
#include "script-scan.h"
#include "script-parse.h"

#define WITH_SEMIES

typedef struct
{
 const char        *symbol;
 script_exp_type_t  exp_type; 
 int                presedence;
}script_parse_operator_table_entry_t;

static script_op_t *script_parse_op (script_scan_t *scan);
static script_exp_t *script_parse_exp (script_scan_t *scan);
static ply_list_t *script_parse_op_list (script_scan_t *scan);
static void script_parse_op_list_free (ply_list_t *op_list);

static script_exp_t *script_parse_new_exp (script_exp_type_t        type,
                                           script_debug_location_t *location)
{
  script_exp_t *exp = malloc (sizeof (script_exp_t));
  exp->type = type;
  script_debug_add_element (exp, location);
  return exp;
}

static script_exp_t *script_parse_new_exp_single (script_exp_type_t        type,
                                                  script_exp_t            *sub,
                                                  script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(type, location);
  exp->data.sub = sub;
  return exp;
}

static script_exp_t *script_parse_new_exp_dual (script_exp_type_t        type,
                                                script_exp_t            *sub_a,
                                                script_exp_t            *sub_b,
                                                script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(type, location);
  exp->data.dual.sub_a = sub_a;
  exp->data.dual.sub_b = sub_b;
  return exp;
}

static script_exp_t *script_parse_new_exp_number (script_number_t          number,
                                                  script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_NUMBER, location);
  exp->data.number = number;
  return exp;
}

static script_exp_t *script_parse_new_exp_string (char                    *string,
                                                  script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_STRING, location);
  exp->data.string = strdup(string);
  return exp;
}

static script_exp_t *script_parse_new_exp_var (char                    *string,
                                               script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_VAR, location);
  exp->data.string = strdup(string);
  return exp;
}

static script_exp_t *script_parse_new_exp_function_exe (script_exp_t            *name,
                                                        ply_list_t              *parameters,
                                                        script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_FUNCTION_EXE, location);
  exp->data.function_exe.name = name;
  exp->data.function_exe.parameters = parameters;
  return exp;
}

static script_exp_t *script_parse_new_exp_function_def (script_function_t       *function_def,
                                                        script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_FUNCTION_DEF, location);
  exp->data.function_def = function_def;
  return exp;
}

static script_exp_t *script_parse_new_exp_set (ply_list_t              *parameters,
                                               script_debug_location_t *location)
{
  script_exp_t *exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_SET, location);
  exp->data.parameters = parameters;
  return exp;
}

static script_op_t *script_parse_new_op (script_op_type_t         type,
                                         script_debug_location_t *location)
{
  script_op_t *op = malloc (sizeof (script_op_t));
  op->type = type;
  script_debug_add_element (op, location);
  return op;
}

static script_op_t *script_parse_new_op_exp (script_exp_t            *exp,
                                             script_debug_location_t *location)
{
  script_op_t *op = script_parse_new_op (SCRIPT_OP_TYPE_EXPRESSION, location);
  op->data.exp = exp;
  return op;
}

static script_op_t *script_parse_new_op_block (ply_list_t              *list,
                                               script_debug_location_t *location)
{
  script_op_t *op = script_parse_new_op (SCRIPT_OP_TYPE_OP_BLOCK, location);
  op->data.list = list;
  return op;
}

static script_op_t *script_parse_new_op_cond (script_op_type_t         type,
                                              script_exp_t            *cond,
                                              script_op_t             *op1,
                                              script_op_t             *op2,
                                              script_debug_location_t *location)
{
  script_op_t *op = script_parse_new_op (type, location);
  op->data.cond_op.cond = cond;
  op->data.cond_op.op1 = op1;
  op->data.cond_op.op2 = op2;
  return op;
}

static void script_parse_error (script_debug_location_t *location,
                                const char              *message)
{
  ply_error ("Parser error \"%s\" L:%d C:%d : %s\n",
             location->name,
             location->line_index,
             location->column_index,
             message);
}

static const script_parse_operator_table_entry_t*   /* Only allows 1 or 2 character symbols */
script_parse_operator_table_entry_lookup (script_scan_t                             *scan,
                                          const script_parse_operator_table_entry_t *table)
{
  int entry_index;
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_scan_token_t *peektoken = script_scan_peek_next_token (scan);
  for (entry_index = 0; table[entry_index].symbol; entry_index++)
    {
      if (!script_scan_token_is_symbol (curtoken)) continue;
      if (curtoken->data.symbol != table[entry_index].symbol[0]) continue;
      if (table[entry_index].symbol[1])
        {
          if (!script_scan_token_is_symbol (peektoken)) continue;
          if (peektoken->data.symbol != table[entry_index].symbol[1]) continue;
          if (peektoken->whitespace) continue;
        }
      break;
    }
  return &table[entry_index];
}

static void script_parse_advance_scan_by_string (script_scan_t *scan,
                                                 const char *string)
{
  while (*string)
    {
      script_scan_get_next_token(scan);
      string++;
    }
}

static script_function_t *script_parse_function_def (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  ply_list_t *parameter_list;

  if (!script_scan_token_is_symbol_of_value (curtoken, '('))
    {
      script_parse_error (&curtoken->location,
        "Function declaration requires parameters to be declared within '(' brackets");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);
  parameter_list = ply_list_new ();

  while (true)
    {
      if (script_scan_token_is_symbol_of_value (curtoken, ')')) break;
      if (!script_scan_token_is_identifier (curtoken))
        {
          script_parse_error (&curtoken->location,
            "Function declaration parameters must be valid identifiers");
          return NULL;
        }
      char *parameter = strdup (curtoken->data.string);
      ply_list_append_data (parameter_list, parameter);

      curtoken = script_scan_get_next_token (scan);

      if (script_scan_token_is_symbol_of_value (curtoken, ')')) break;
      if (!script_scan_token_is_symbol_of_value (curtoken, ','))
        {
          script_parse_error (&curtoken->location,
            "Function declaration parameters must separated with ',' and terminated with a ')'");
          return NULL;
        }
      curtoken = script_scan_get_next_token (scan);
    }

  curtoken = script_scan_get_next_token (scan);

  script_op_t *func_op = script_parse_op (scan);
  
  script_function_t *function = script_function_script_new (func_op,
                                                            NULL,
                                                            parameter_list);
  return function;
}

static script_exp_t *script_parse_exp_tm (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_exp_t *exp = NULL;

  if (script_scan_token_is_integer (curtoken))
    {
      exp = script_parse_new_exp_number (curtoken->data.integer, &curtoken->location);
      script_scan_get_next_token (scan);
      return exp;
    }
  if (script_scan_token_is_float (curtoken))
    {
      exp = script_parse_new_exp_number (curtoken->data.floatpoint, &curtoken->location);
      script_scan_get_next_token (scan);
      return exp;
    }
  if (script_scan_token_is_identifier (curtoken))
    {
      if (script_scan_token_is_identifier_of_value (curtoken, "NULL"))
        exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_NULL, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "INFINITY"))
        exp = script_parse_new_exp_number (INFINITY, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "NAN"))
        exp = script_parse_new_exp_number (NAN, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "global"))
        exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_GLOBAL, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "local"))
        exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_LOCAL, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "this"))
        exp = script_parse_new_exp(SCRIPT_EXP_TYPE_TERM_THIS, &curtoken->location);
      else if (script_scan_token_is_identifier_of_value (curtoken, "fun"))
        {
          script_debug_location_t location = curtoken->location;
          script_scan_get_next_token (scan);
          exp = script_parse_new_exp_function_def (script_parse_function_def (scan), &location);
          return exp;
        }
      else
        {
          exp = script_parse_new_exp_var (curtoken->data.string, &curtoken->location);
        }
      curtoken = script_scan_get_next_token (scan);
      return exp;
    }
  if (script_scan_token_is_string (curtoken))
    {
      exp = script_parse_new_exp_string (curtoken->data.string, &curtoken->location);
      script_scan_get_next_token (scan);
      return exp;
    }
  
  if (script_scan_token_is_symbol_of_value (curtoken, '['))
    {
      ply_list_t *parameters = ply_list_new ();
      script_debug_location_t location = curtoken->location;
      script_scan_get_next_token (scan);
      while (true)
        {
          if (script_scan_token_is_symbol_of_value (curtoken, ']')) break;
          script_exp_t *parameter = script_parse_exp (scan);

          ply_list_append_data (parameters, parameter);

          curtoken = script_scan_get_current_token (scan);
          if (script_scan_token_is_symbol_of_value (curtoken, ']')) break;
          if (!script_scan_token_is_symbol_of_value (curtoken, ','))
            {
              script_parse_error (&curtoken->location,
                "Set parameters should be separated with a ',' and terminated with a ']'");
              return NULL;
            }
          curtoken = script_scan_get_next_token (scan);
        }
      script_scan_get_next_token (scan);
      exp = script_parse_new_exp_set (parameters, &location);
      return exp;
    }
  if (script_scan_token_is_symbol_of_value (curtoken, '('))
    {
      script_scan_get_next_token (scan);
      exp = script_parse_exp (scan);
      curtoken = script_scan_get_current_token (scan);
      if (!exp)
        {
          script_parse_error (&curtoken->location,
            "Expected valid contents of bracketed expression");
          return NULL;
        }
      if (!script_scan_token_is_symbol_of_value (curtoken, ')'))
        {
          script_parse_error (&curtoken->location,
            "Expected bracketed block to be terminated with a ')'");
          return NULL;
        }
      script_scan_get_next_token (scan);
      return exp;
    }
  return NULL;
}

static script_exp_t *script_parse_exp_pi (script_scan_t *scan)
{
  script_exp_t *exp = script_parse_exp_tm (scan);
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);

  while (true)
    {
      script_debug_location_t location = curtoken->location;
      if (!script_scan_token_is_symbol (curtoken)) break;
      if (script_scan_token_is_symbol_of_value (curtoken, '('))
        {
          ply_list_t *parameters = ply_list_new ();
          script_scan_get_next_token (scan);
          while (true)
            {
              if (script_scan_token_is_symbol_of_value (curtoken, ')')) break;
              script_exp_t *parameter = script_parse_exp (scan);

              ply_list_append_data (parameters, parameter);

              curtoken = script_scan_get_current_token (scan);
              if (script_scan_token_is_symbol_of_value (curtoken, ')')) break;
              if (!script_scan_token_is_symbol_of_value (curtoken, ','))
                {
                  script_parse_error (&curtoken->location,
                    "Function parameters should be separated with a ',' and terminated with a ')'");
                  return NULL;
                }
              curtoken = script_scan_get_next_token (scan);
            }
          script_scan_get_next_token (scan);
          exp = script_parse_new_exp_function_exe (exp, parameters, &location);
          continue;
        }
      script_exp_t *key;

      if (script_scan_token_is_symbol_of_value (curtoken, '.'))
        {
          script_scan_get_next_token (scan);
          if (script_scan_token_is_identifier (curtoken))
            {
              key = script_parse_new_exp_string (curtoken->data.string, &curtoken->location);
            }
          else
            {
              script_parse_error (&curtoken->location,
                "A dot based hash index must be an identifier");
              return NULL;
            }
          curtoken = script_scan_get_next_token (scan);
        }
      else if (script_scan_token_is_symbol_of_value (curtoken, '['))
        {
          script_scan_get_next_token (scan);
          key = script_parse_exp (scan);
          if (!key)
            {
              script_parse_error (&curtoken->location,
                "Expected a valid index expression");
              return NULL;
            }
          curtoken = script_scan_get_current_token (scan);
          if (!script_scan_token_is_symbol_of_value (curtoken, ']'))
            {
              script_parse_error (&curtoken->location,
                "Expected a ']' to terminate the index expression");
              return NULL;
            }
          curtoken = script_scan_get_next_token (scan);
        }
      else break;
      exp = script_parse_new_exp_dual (SCRIPT_EXP_TYPE_HASH, exp, key, &location);
    }
  return exp;
}

static script_exp_t *script_parse_exp_pr (script_scan_t *scan)
{
  static const script_parse_operator_table_entry_t operator_table[] =
    {
      {"++", SCRIPT_EXP_TYPE_PRE_INC,    0},
      {"+",  SCRIPT_EXP_TYPE_POS,        0},
      {"--", SCRIPT_EXP_TYPE_PRE_DEC,    0},
      {"-",  SCRIPT_EXP_TYPE_NEG,        0},
      {"!",  SCRIPT_EXP_TYPE_NOT,        0},
      {NULL, SCRIPT_EXP_TYPE_TERM_NULL, -1},
    };
  const script_parse_operator_table_entry_t* entry;
  entry =  script_parse_operator_table_entry_lookup(scan, operator_table);
  if (entry->presedence < 0) return script_parse_exp_pi (scan);
  script_debug_location_t location = script_scan_get_current_token (scan)->location;
  script_parse_advance_scan_by_string(scan, entry->symbol);
  return script_parse_new_exp_single(entry->exp_type, script_parse_exp_pr (scan), &location);
}

static script_exp_t *script_parse_exp_po (script_scan_t *scan)
{
  static const script_parse_operator_table_entry_t operator_table[] =
    {
      {"++", SCRIPT_EXP_TYPE_POST_INC,   0},
      {"--", SCRIPT_EXP_TYPE_POST_DEC,   0},
      {NULL, SCRIPT_EXP_TYPE_TERM_NULL, -1},
    };
  script_exp_t *exp = script_parse_exp_pr (scan);

  while (true)
    {
      const script_parse_operator_table_entry_t* entry;
      entry =  script_parse_operator_table_entry_lookup(scan, operator_table);
      if (entry->presedence < 0) break;
      exp = script_parse_new_exp_single(entry->exp_type, exp, &script_scan_get_current_token (scan)->location);
      script_parse_advance_scan_by_string(scan, entry->symbol);
    }
  return exp;
}

static script_exp_t *script_parse_exp_ltr (script_scan_t *scan, int presedence)
{
  static const script_parse_operator_table_entry_t operator_table[] =
    {
      {"||", SCRIPT_EXP_TYPE_OR,         1},    /* FIXME Does const imply static? */
      {"&&", SCRIPT_EXP_TYPE_AND,        2},
      {"==", SCRIPT_EXP_TYPE_EQ,         3},
      {"!=", SCRIPT_EXP_TYPE_NE,         3},
      {">=", SCRIPT_EXP_TYPE_GE,         4},
      {"<=", SCRIPT_EXP_TYPE_LE,         4},
      {"+=", SCRIPT_EXP_TYPE_TERM_NULL, -1},    /* A few things it shouldn't consume */
      {"-=", SCRIPT_EXP_TYPE_TERM_NULL, -1},
      {"*=", SCRIPT_EXP_TYPE_TERM_NULL, -1},
      {"/=", SCRIPT_EXP_TYPE_TERM_NULL, -1},
      {"%=", SCRIPT_EXP_TYPE_TERM_NULL, -1},
      {"|=", SCRIPT_EXP_TYPE_TERM_NULL, -1},
      {"|",  SCRIPT_EXP_TYPE_EXTEND,     0},
      {">",  SCRIPT_EXP_TYPE_GT,         4},
      {"<",  SCRIPT_EXP_TYPE_LT,         4},
      {"+",  SCRIPT_EXP_TYPE_PLUS,       5},
      {"-",  SCRIPT_EXP_TYPE_MINUS,      5},
      {"*",  SCRIPT_EXP_TYPE_MUL,        6},
      {"/",  SCRIPT_EXP_TYPE_DIV,        6},
      {"%",  SCRIPT_EXP_TYPE_MOD,        6},    /* Put this number into the "presedence > ?" line below*/
      {NULL, SCRIPT_EXP_TYPE_TERM_NULL, -1},
    };
    
  if (presedence > 6) return script_parse_exp_po (scan);
  script_exp_t *exp = script_parse_exp_ltr (scan, presedence + 1);
  if (!exp) return NULL;
  
  while (true)
    {
      const script_parse_operator_table_entry_t* entry;
      entry =  script_parse_operator_table_entry_lookup(scan, operator_table);
      if (entry->presedence != presedence) break;
      script_debug_location_t location = script_scan_get_current_token (scan)->location;
      script_parse_advance_scan_by_string(scan, entry->symbol);
      exp = script_parse_new_exp_dual(entry->exp_type, exp, script_parse_exp_ltr (scan, presedence + 1), &location);
      if (!exp->data.dual.sub_b)
        {
          script_parse_error (&script_scan_get_current_token (scan)->location,
                              "An invalid RHS of an operation");
          return NULL;
        }
    }
  return exp;
}

static script_exp_t *script_parse_exp_as (script_scan_t *scan)
{
  static const script_parse_operator_table_entry_t operator_table[] =
    {
      {"+=", SCRIPT_EXP_TYPE_ASSIGN_PLUS,  0},
      {"-=", SCRIPT_EXP_TYPE_ASSIGN_MINUS, 0}, 
      {"*=", SCRIPT_EXP_TYPE_ASSIGN_MUL,   0}, 
      {"/=", SCRIPT_EXP_TYPE_ASSIGN_DIV,   0}, 
      {"%=", SCRIPT_EXP_TYPE_ASSIGN_MOD,   0}, 
      {"|=", SCRIPT_EXP_TYPE_ASSIGN_EXTEND,0},
      {"=",  SCRIPT_EXP_TYPE_ASSIGN,       0}, 
      {NULL, SCRIPT_EXP_TYPE_TERM_NULL,   -1}, 
    };
  script_exp_t *lhs = script_parse_exp_ltr (scan, 0);
  if (!lhs) return NULL;

  const script_parse_operator_table_entry_t* entry;
  entry =  script_parse_operator_table_entry_lookup(scan, operator_table);
  if (entry->presedence < 0) return lhs;
  script_debug_location_t location = script_scan_get_current_token (scan)->location;
  script_parse_advance_scan_by_string(scan, entry->symbol);
  script_exp_t *rhs = script_parse_exp_as (scan);
  if (!rhs)
    {
      script_parse_error (&script_scan_get_current_token (scan)->location,
                          "An invalid RHS of an assign");
      return NULL;
    }
  return script_parse_new_exp_dual (entry->exp_type, lhs, rhs, &location);
}

static script_exp_t *script_parse_exp (script_scan_t *scan)
{
  return script_parse_exp_as (scan);
}

static script_op_t *script_parse_op_block (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);

  if (!script_scan_token_is_symbol_of_value (curtoken, '{'))
    return NULL;
  script_debug_location_t location = curtoken->location;

  script_scan_get_next_token (scan);
  ply_list_t *sublist = script_parse_op_list (scan);

  curtoken = script_scan_get_current_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, '}'))
    {
      script_parse_error (&curtoken->location,
                          "Expected a '}' to terminate the operation block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);

  script_op_t *op = script_parse_new_op_block (sublist, &location);
  return op;
}

static script_op_t *script_parse_if_while (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_op_type_t type;

  if       (script_scan_token_is_identifier_of_value (curtoken, "if"))
    type = SCRIPT_OP_TYPE_IF;
  else if  (script_scan_token_is_identifier_of_value (curtoken, "while"))
    type = SCRIPT_OP_TYPE_WHILE;
  else return NULL;

  script_debug_location_t location = curtoken->location;
  curtoken = script_scan_get_next_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, '('))
    {
      script_parse_error (&curtoken->location,
                          "Expected a '(' at the start of a condition block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);

  script_exp_t *cond = script_parse_exp (scan);
  curtoken = script_scan_get_current_token (scan);
  if (!cond)
    {
      script_parse_error (&curtoken->location, "Expected a valid condition expression");
      return NULL;
    }
  if (!script_scan_token_is_symbol_of_value (curtoken, ')'))
    {
      script_parse_error (&curtoken->location,
                          "Expected a ')' at the end of a condition block");
      return NULL;
    }
  script_scan_get_next_token (scan);
  script_op_t *cond_op = script_parse_op (scan);
  script_op_t *else_op = NULL;

  curtoken = script_scan_get_current_token (scan);
  if ((type == SCRIPT_OP_TYPE_IF)
      && (script_scan_token_is_identifier_of_value (curtoken, "else")))
    {
      script_scan_get_next_token (scan);
      else_op = script_parse_op (scan);
    }
  script_op_t *op = script_parse_new_op_cond (type, cond, cond_op, else_op, &location);
  return op;
}

static script_op_t *script_parse_do_while (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);

  if (!script_scan_token_is_identifier_of_value (curtoken, "do"))
    return NULL;
  script_debug_location_t location = curtoken->location;
  curtoken = script_scan_get_next_token (scan);
  script_op_t *cond_op = script_parse_op (scan);
  curtoken = script_scan_get_current_token (scan);

  if (!script_scan_token_is_identifier_of_value (curtoken, "while"))
    {
      script_parse_error (&curtoken->location,
                          "Expected a 'while' after a 'do' block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);

  if (!script_scan_token_is_symbol_of_value (curtoken, '('))
    {
      script_parse_error (&curtoken->location,
                          "Expected a '(' at the start of a do-while condition block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);
  script_exp_t *cond = script_parse_exp (scan);
  curtoken = script_scan_get_current_token (scan);
  if (!cond)
    {
      script_parse_error (&curtoken->location, "Expected a valid condition expression");
      return NULL;
    }
  if (!script_scan_token_is_symbol_of_value (curtoken, ')'))
    {
      script_parse_error (&curtoken->location,
                          "Expected a ')' at the end of a condition block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, ';'))
    {
      script_parse_error (&curtoken->location,
                          "Expected a ';' after a do-while expression");
      return NULL;
    }
  script_scan_get_next_token (scan);
  script_op_t *op = script_parse_new_op_cond (SCRIPT_OP_TYPE_DO_WHILE, cond, cond_op, NULL, &location);
  return op;
}

static script_op_t *script_parse_for (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);

  if (!script_scan_token_is_identifier_of_value (curtoken, "for")) return NULL;
  script_debug_location_t location_for = curtoken->location;
  curtoken = script_scan_get_next_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, '('))
    {
      script_parse_error (&curtoken->location,
                          "Expected a '(' at the start of a condition block");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);
  script_debug_location_t location_first = curtoken->location;

  script_exp_t *first = script_parse_exp (scan);
  if (!first)
    {
      script_parse_error (&curtoken->location, "Expected a valid first expression");
      return NULL;
    }
  curtoken = script_scan_get_current_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, ';'))
    {
      script_parse_error (&curtoken->location,
                          "Expected a ';' after the first 'for' expression");
      return NULL;
    }
  script_scan_get_next_token (scan);

  script_exp_t *cond = script_parse_exp (scan);
  if (!cond)
    {
      script_parse_error (&curtoken->location, "Expected a valid condition expression");
      return NULL;
    }
  curtoken = script_scan_get_current_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, ';'))
    {
      script_parse_error (&curtoken->location, "Expected a ';' after the 'for' condition");
      return NULL;
    }
  script_scan_get_next_token (scan);
  script_debug_location_t location_last = curtoken->location;

  script_exp_t *last = script_parse_exp (scan);
  if (!last)
    {
      script_parse_error (&curtoken->location, "Expected a valid last expression");
      return NULL;
    }
  curtoken = script_scan_get_current_token (scan);
  if (!script_scan_token_is_symbol_of_value (curtoken, ')'))
    {
      script_parse_error (&curtoken->location, "Expected a ')' at the end of a for block");
      return NULL;
    }
  script_scan_get_next_token (scan);
  script_op_t *op_body = script_parse_op (scan);

  script_op_t *op_first = script_parse_new_op_exp (first, &location_first);
  script_op_t *op_last = script_parse_new_op_exp (last, &location_last);
  script_op_t *op_for = script_parse_new_op_cond (SCRIPT_OP_TYPE_FOR, cond, op_body, op_last, &location_for);

  ply_list_t *op_list = ply_list_new ();
  ply_list_append_data (op_list, op_first);
  ply_list_append_data (op_list, op_for);

  script_op_t *op_block = script_parse_new_op_block (op_list, &location_for);

  return op_block;
}

static script_op_t *script_parse_function (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);

  if (!script_scan_token_is_identifier_of_value (curtoken, "fun")) return NULL;
  script_debug_location_t location = curtoken->location;
  curtoken = script_scan_get_next_token (scan);
  if (!script_scan_token_is_identifier (curtoken))
    {
      script_parse_error (&curtoken->location,
                          "A function declaration requires a valid name");
      return NULL;
    }
  script_exp_t *name = script_parse_new_exp_var (curtoken->data.string, &curtoken->location);
  
  curtoken = script_scan_get_next_token (scan);     /* FIXME parse any type of exp as target and do an assign*/

  script_function_t *function = script_parse_function_def (scan);
  if (!function) return NULL;
  script_exp_t *func_exp = script_parse_new_exp_function_def (function, &location);
  script_exp_t *func_def = script_parse_new_exp_dual (SCRIPT_EXP_TYPE_ASSIGN, name, func_exp, &location);
    
  script_op_t *op = script_parse_new_op_exp (func_def, &location);
  return op;
}

static script_op_t *script_parse_return (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_op_type_t type;
  if      (script_scan_token_is_identifier_of_value (curtoken, "return"))
    type = SCRIPT_OP_TYPE_RETURN;
  else if (script_scan_token_is_identifier_of_value (curtoken, "fail"))
    type = SCRIPT_OP_TYPE_FAIL;
  else if (script_scan_token_is_identifier_of_value (curtoken, "break"))
    type = SCRIPT_OP_TYPE_BREAK;
  else if (script_scan_token_is_identifier_of_value (curtoken, "continue"))
    type = SCRIPT_OP_TYPE_CONTINUE;
  else return NULL;
  curtoken = script_scan_get_next_token (scan);

  script_op_t *op = script_parse_new_op (type, &curtoken->location);
  if (type == SCRIPT_OP_TYPE_RETURN)
    {
      op->data.exp = script_parse_exp (scan);                  /* May be NULL */
      curtoken = script_scan_get_current_token (scan);
    }
#ifdef WITH_SEMIES
  if (!script_scan_token_is_symbol_of_value (curtoken, ';'))
    {
      script_parse_error (&curtoken->location, "Expected ';' after an expression");
      return NULL;
    }
  curtoken = script_scan_get_next_token (scan);
#endif

  return op;
}

static script_op_t *script_parse_op (script_scan_t *scan)
{
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_op_t *reply = NULL;

  reply = script_parse_op_block (scan);
  if (reply) return reply;
  reply = script_parse_if_while (scan);
  if (reply) return reply;
  reply = script_parse_do_while (scan);
  if (reply) return reply;
  reply = script_parse_for (scan);
  if (reply) return reply;
  reply = script_parse_return (scan);
  if (reply) return reply;
  reply = script_parse_function (scan);
  if (reply) return reply;
/* if curtoken->data.string == "if/for/while... */

/* default is expression */
  {
    script_debug_location_t location = curtoken->location;
    script_exp_t *exp = script_parse_exp (scan);
    if (!exp) return NULL;
    curtoken = script_scan_get_current_token (scan);
#ifdef WITH_SEMIES
    if (!script_scan_token_is_symbol_of_value (curtoken, ';'))
      {
        script_parse_error (&curtoken->location, "Expected ';' after an expression");
        return NULL;
      }
    curtoken = script_scan_get_next_token (scan);
#endif

    script_op_t *op = script_parse_new_op_exp (exp, &location);
    return op;
  }
  return NULL;
}

static ply_list_t *script_parse_op_list (script_scan_t *scan)
{
  ply_list_t *op_list = ply_list_new ();

  while (true)
    {
      script_op_t *op = script_parse_op (scan);
      if (!op) break;
      ply_list_append_data (op_list, op);
    }

  return op_list;
}

static void script_parse_exp_free (script_exp_t *exp)
{
  if (!exp) return;
  switch (exp->type)
    {
      case SCRIPT_EXP_TYPE_PLUS:
      case SCRIPT_EXP_TYPE_MINUS:
      case SCRIPT_EXP_TYPE_MUL:
      case SCRIPT_EXP_TYPE_DIV:
      case SCRIPT_EXP_TYPE_MOD:
      case SCRIPT_EXP_TYPE_EQ:
      case SCRIPT_EXP_TYPE_NE:
      case SCRIPT_EXP_TYPE_GT:
      case SCRIPT_EXP_TYPE_GE:
      case SCRIPT_EXP_TYPE_LT:
      case SCRIPT_EXP_TYPE_LE:
      case SCRIPT_EXP_TYPE_AND:
      case SCRIPT_EXP_TYPE_OR:
      case SCRIPT_EXP_TYPE_EXTEND:
      case SCRIPT_EXP_TYPE_ASSIGN:
      case SCRIPT_EXP_TYPE_ASSIGN_PLUS:
      case SCRIPT_EXP_TYPE_ASSIGN_MINUS:
      case SCRIPT_EXP_TYPE_ASSIGN_MUL:
      case SCRIPT_EXP_TYPE_ASSIGN_DIV:
      case SCRIPT_EXP_TYPE_ASSIGN_MOD:
      case SCRIPT_EXP_TYPE_ASSIGN_EXTEND:
      case SCRIPT_EXP_TYPE_HASH:
        script_parse_exp_free (exp->data.dual.sub_a);
        script_parse_exp_free (exp->data.dual.sub_b);
        break;

      case SCRIPT_EXP_TYPE_NOT:
      case SCRIPT_EXP_TYPE_POS:
      case SCRIPT_EXP_TYPE_NEG:
      case SCRIPT_EXP_TYPE_PRE_INC:
      case SCRIPT_EXP_TYPE_PRE_DEC:
      case SCRIPT_EXP_TYPE_POST_INC:
      case SCRIPT_EXP_TYPE_POST_DEC:
        script_parse_exp_free (exp->data.sub);
        break;

      case SCRIPT_EXP_TYPE_TERM_NUMBER:
      case SCRIPT_EXP_TYPE_TERM_NULL:
      case SCRIPT_EXP_TYPE_TERM_LOCAL:
      case SCRIPT_EXP_TYPE_TERM_GLOBAL:
      case SCRIPT_EXP_TYPE_TERM_THIS:
        break;
      case SCRIPT_EXP_TYPE_TERM_SET:
        {
          ply_list_node_t *node;
          for (node = ply_list_get_first_node (exp->data.parameters);
               node;
               node = ply_list_get_next_node (exp->data.parameters, node))
            {
              script_exp_t *sub = ply_list_node_get_data (node);
              script_parse_exp_free (sub);
            }
          ply_list_free (exp->data.parameters);
          break;
        }
      case SCRIPT_EXP_TYPE_FUNCTION_EXE:
        {
          ply_list_node_t *node;
          for (node = ply_list_get_first_node (exp->data.function_exe.parameters);
               node;
               node = ply_list_get_next_node (exp->data.function_exe.parameters, node))
            {
              script_exp_t *sub = ply_list_node_get_data (node);
              script_parse_exp_free (sub);
            }
          ply_list_free (exp->data.function_exe.parameters);
          script_parse_exp_free (exp->data.function_exe.name);
          break;
        }
      case SCRIPT_EXP_TYPE_FUNCTION_DEF:   /* FIXME merge the frees with one from op_free */
        {
          if (exp->data.function_def->type == SCRIPT_FUNCTION_TYPE_SCRIPT) 
            script_parse_op_free (exp->data.function_def->data.script);
          ply_list_node_t *node;
          for (node = ply_list_get_first_node (exp->data.function_def->parameters);
               node;
               node = ply_list_get_next_node (exp->data.function_def->parameters, node))
            {
              char *arg = ply_list_node_get_data (node);
              free (arg);
            }
          ply_list_free (exp->data.function_def->parameters);
          free (exp->data.function_def);
          break;
        }

      case SCRIPT_EXP_TYPE_TERM_STRING:
      case SCRIPT_EXP_TYPE_TERM_VAR:
        free (exp->data.string);
        break;
    }
  script_debug_remove_element (exp);
  free (exp);
}

void script_parse_op_free (script_op_t *op)
{
  if (!op) return;
  switch (op->type)
    {
      case SCRIPT_OP_TYPE_EXPRESSION:
        script_parse_exp_free (op->data.exp);
        break;

      case SCRIPT_OP_TYPE_OP_BLOCK:
        script_parse_op_list_free (op->data.list);
        break;

      case SCRIPT_OP_TYPE_IF:
      case SCRIPT_OP_TYPE_WHILE:
      case SCRIPT_OP_TYPE_DO_WHILE:
      case SCRIPT_OP_TYPE_FOR:
        script_parse_exp_free (op->data.cond_op.cond);
        script_parse_op_free  (op->data.cond_op.op1);
        script_parse_op_free  (op->data.cond_op.op2);
        break;

      case SCRIPT_OP_TYPE_RETURN:
        if (op->data.exp) script_parse_exp_free (op->data.exp);
        break;

      case SCRIPT_OP_TYPE_FAIL:
      case SCRIPT_OP_TYPE_BREAK:
      case SCRIPT_OP_TYPE_CONTINUE:
        break;
    }
  script_debug_remove_element (op);
  free (op);
}

static void script_parse_op_list_free (ply_list_t *op_list)
{
  ply_list_node_t *node;

  for (node = ply_list_get_first_node (op_list);
       node;
       node = ply_list_get_next_node (op_list, node))
    {
      script_op_t *op = ply_list_node_get_data (node);
      script_parse_op_free (op);
    }
  ply_list_free (op_list);
  return;
}

script_op_t *script_parse_file (const char *filename)
{
  script_scan_t *scan = script_scan_file (filename);

  if (!scan)
    {
      ply_error ("Parser error : Error opening file %s\n", filename);
      return NULL;
    }
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_debug_location_t location = curtoken->location;
  ply_list_t *list = script_parse_op_list (scan);

  curtoken = script_scan_get_current_token (scan);
  if (curtoken->type != SCRIPT_SCAN_TOKEN_TYPE_EOF)
    {
      script_parse_error (&curtoken->location, "Unparsed characters at end of file");
      return NULL;
    }
  script_op_t *op = script_parse_new_op_block (list, &location);
  script_scan_free (scan);
  return op;
}

script_op_t *script_parse_string (const char *string,
                                  const char *name)
{
  script_scan_t *scan = script_scan_string (string, name);

  if (!scan)
    {
      ply_error ("Parser error : Error creating a parser with a string");
      return NULL;
    }
  script_scan_token_t *curtoken = script_scan_get_current_token (scan);
  script_debug_location_t location = curtoken->location;
  ply_list_t *list = script_parse_op_list (scan);
  
  curtoken = script_scan_get_current_token (scan);
  if (curtoken->type != SCRIPT_SCAN_TOKEN_TYPE_EOF)
    {
      script_parse_error (&curtoken->location, "Unparsed characters at end of file");
      return NULL;
    }
  script_op_t *op = script_parse_new_op_block (list, &location);
  script_scan_free (scan);
  return op;
}
