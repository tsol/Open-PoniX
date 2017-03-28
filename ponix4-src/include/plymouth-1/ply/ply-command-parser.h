/* ply-command-parser.h - command line parser
 *
 * Copyright (C) 2008 Red Hat, Inc.
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
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_COMMAND_PARSER_H
#define PLY_COMMAND_PARSER_H

#include <stdarg.h>

#include "ply-event-loop.h"

typedef struct _ply_command_parser ply_command_parser_t;
typedef void (* ply_command_handler_t) (void *data,
                                        const char *command);

typedef enum
{
  PLY_COMMAND_OPTION_TYPE_FLAG = 0,
  PLY_COMMAND_OPTION_TYPE_BOOLEAN,
  PLY_COMMAND_OPTION_TYPE_STRING,
  PLY_COMMAND_OPTION_TYPE_INTEGER
} ply_command_option_type_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_command_parser_t *ply_command_parser_new (const char *name,
                                              const char *description);

void ply_command_parser_add_options (ply_command_parser_t *parser,
                                     const char *option_name, /*
                                     const char *option_description,
                                     ply_option_argument_type_t option_type */
                                     ...);
void ply_command_parser_add_command (ply_command_parser_t *parser,
                                     const char *name, const char *description,
                                     ply_command_handler_t handler,
                                     void *handler_data,
                                     const char *option_name, /*
                                     const char *option_description,
                                     ply_option_argument_type_t option_type */
                                     ...);
void ply_command_parser_add_command_alias (ply_command_parser_t *parser,
                                           const char           *name,
                                           const char           *alias);
void ply_command_parser_get_options (ply_command_parser_t *parser,
                                     const char *option_name, /*
                                     void *      option_result */
                                     ...);
void ply_command_parser_get_option (ply_command_parser_t *parser,
                                    const char           *option_name,
                                    void                 *option_result,
                                    bool                 *option_is_set);
void ply_command_parser_get_command_options (ply_command_parser_t *parser,
                                             const char *command_name,
                                             const char *option_name, /*
                                             void *      option_result,
                                             bool *      option_was_set */
                                             ...);
void ply_command_parser_get_command_option (ply_command_parser_t *parser,
                                            const char           *command_name,
                                            const char           *option_name,
                                            void                 *option_result,
                                            bool                 *option_is_set);
void ply_command_parser_stop_parsing_arguments (ply_command_parser_t *parser);

char *ply_command_parser_get_help_string (ply_command_parser_t *parser);

bool ply_command_parser_parse_arguments (ply_command_parser_t *parser,
                                         ply_event_loop_t     *loop,
                                         char * const         *arguments,
                                         int                   number_of_arguments);
void ply_command_parser_free (ply_command_parser_t *command_parser);
#endif

#endif /* PLY_COMMAND_PARSER_H */
