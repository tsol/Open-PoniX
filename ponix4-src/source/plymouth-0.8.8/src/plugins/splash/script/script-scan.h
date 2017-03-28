/* ply-scan.h - lexical scanner
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
#ifndef SCRIPT_SCAN_H
#define SCRIPT_SCAN_H

#include "script-debug.h"
#include "ply-bitarray.h"
#include <stdbool.h>

typedef enum
{
  SCRIPT_SCAN_TOKEN_TYPE_EMPTY,
  SCRIPT_SCAN_TOKEN_TYPE_EOF,
  SCRIPT_SCAN_TOKEN_TYPE_INTEGER,
  SCRIPT_SCAN_TOKEN_TYPE_FLOAT,
  SCRIPT_SCAN_TOKEN_TYPE_IDENTIFIER,
  SCRIPT_SCAN_TOKEN_TYPE_STRING,
  SCRIPT_SCAN_TOKEN_TYPE_SYMBOL,
  SCRIPT_SCAN_TOKEN_TYPE_COMMENT,
  SCRIPT_SCAN_TOKEN_TYPE_ERROR,
} script_scan_token_type_t;

typedef struct
{
  script_scan_token_type_t type;
  union
  {
    char *string;
    char symbol;
    long long int integer;
    double floatpoint;
  } data;
  int whitespace;
  script_debug_location_t location;
} script_scan_token_t;

typedef struct
{
  union
  {
    int fd;
    const char *string;
  } source;
  char* name;
  unsigned char cur_char;
  ply_bitarray_t *identifier_1st_char;
  ply_bitarray_t *identifier_nth_char;
  int tokencount;
  script_scan_token_t **tokens;
  int line_index;
  int column_index;
  bool source_is_file;
} script_scan_t;


#define script_scan_token_is_symbol(__token) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_SYMBOL)
#define script_scan_token_is_symbol_of_value(__token,__value) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_SYMBOL \
      && __token->data.symbol == __value)
#define script_scan_token_is_identifier(__token) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_IDENTIFIER)
#define script_scan_token_is_identifier_of_value(__token,__value) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_IDENTIFIER \
      && !strcmp(__token->data.string, __value))
#define script_scan_token_is_integer(__token) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_INTEGER)
#define script_scan_token_is_string(__token) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_STRING)
#define script_scan_token_is_float(__token) \
      (__token->type == SCRIPT_SCAN_TOKEN_TYPE_FLOAT)




script_scan_t *script_scan_file (const char *filename);
script_scan_t *script_scan_string (const char *string,
                                   const char *name);
void script_scan_token_clean (script_scan_token_t *token);
void script_scan_free (script_scan_t *scan);
unsigned char script_scan_get_current_char (script_scan_t *scan);
unsigned char script_scan_get_next_char (script_scan_t *scan);
script_scan_token_t *script_scan_get_current_token (script_scan_t *scan);
script_scan_token_t *script_scan_get_next_token (script_scan_t *scan);
script_scan_token_t *script_scan_peek_next_token (script_scan_t *scan);
void script_scan_read_next_token (script_scan_t       *scan,
                                  script_scan_token_t *token);


#endif /* SCRIPT_SCAN_H */
