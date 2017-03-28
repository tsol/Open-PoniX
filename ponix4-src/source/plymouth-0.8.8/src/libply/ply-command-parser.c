/* ply-command-parser.c - command line parser
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
#include "config.h"
#include "ply-command-parser.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "ply-buffer.h"
#include "ply-list.h"
#include "ply-utils.h"

typedef union
{
  bool as_boolean;
  char *as_string;
  int as_integer;
} ply_command_option_result_t;

typedef struct
{
  char *name;
  char *description;
  ply_command_option_type_t type;
  uint32_t was_set : 1;
  ply_command_option_result_t result;
} ply_command_option_t;

typedef struct
{
  char *name;
  ply_list_t *aliases;
  char *description;
  ply_list_t *options;

  size_t longest_option_length;

  ply_command_handler_t handler;
  void       *handler_data;
} ply_command_t;

struct _ply_command_parser
{
  ply_event_loop_t *loop;
  ply_command_t *main_command;

  ply_list_t *available_subcommands;
  ply_list_t *read_subcommands;

  ply_list_t *arguments;
  size_t longest_command_length;

  uint32_t dispatch_is_queued : 1;
};

static ply_command_option_t *
ply_command_option_new (const char                *name,
                        const char                *description,
                        ply_command_option_type_t  type)
{
  ply_command_option_t *option;

  option = calloc (1, sizeof (ply_command_option_t));

  option->name = strdup (name);
  option->description = strdup (description);
  option->type = type;

  return option;
}

static void
ply_command_option_free (ply_command_option_t *option)
{
  if (option == NULL)
    return;

  free (option->name);
  free (option->description);
  free (option);
}

static ply_command_t *
ply_command_new (const char *name,
                 const char *description,
                 ply_command_handler_t handler,
                 void       *data)
{
  ply_command_t *command;

  command = calloc (1, sizeof (ply_command_t));

  command->options = ply_list_new ();
  command->name = strdup (name);
  command->aliases = ply_list_new ();
  command->description = strdup (description);
  command->handler = handler;
  command->handler_data = data;

  return command;
}

static void
ply_command_free (ply_command_t *command)
{
  ply_list_node_t *option_node;
  ply_list_node_t *alias_node;

  if (command == NULL)
    return;

  while ((alias_node = ply_list_get_first_node (command->aliases)) != NULL)
    {
      free (ply_list_node_get_data (alias_node));
      ply_list_remove_node (command->aliases, alias_node);
    }

  option_node = ply_list_get_first_node (command->options);
  while (option_node != NULL)
    {
      ply_command_option_t *option;

      option = (ply_command_option_t *) ply_list_node_get_data (option_node);
      ply_command_option_free (option);

      option_node = ply_list_get_next_node (command->options, option_node);
    }
  ply_list_free (command->options);
  free (command);
}

static void
append_usage_line_to_buffer (ply_command_parser_t *parser,
                             ply_buffer_t *buffer)
{
  ply_buffer_append (buffer, "%s\n",
                     parser->main_command->description);
  ply_buffer_append (buffer, "USAGE: %s [OPTION...]", parser->main_command->name);

  if (ply_list_get_length (parser->available_subcommands) > 0)
    ply_buffer_append (buffer, " [COMMAND [OPTION...]...]\n");
}

static const char *
get_type_string (int type)
{
  const char *option_type_string;

  switch (type)
    {
    case PLY_COMMAND_OPTION_TYPE_FLAG:
      option_type_string = "";
      break;
    case PLY_COMMAND_OPTION_TYPE_BOOLEAN:
      option_type_string = "={true|false}";
      break;
    case PLY_COMMAND_OPTION_TYPE_STRING:
      option_type_string = "=<string>";
      break;
    case PLY_COMMAND_OPTION_TYPE_INTEGER:
      option_type_string = "=<integer>";
      break;
    default:
      option_type_string = "";
      break;
    }

  return option_type_string;
}

static void
append_command_options_to_buffer (ply_command_parser_t *parser,
                               ply_command_t *command,
                               ply_buffer_t *buffer)
{
  ply_list_node_t *option_node;

  option_node = ply_list_get_first_node (command->options);

  while (option_node != NULL)
    {
      ply_command_option_t *option;
      int option_width;
      const char *option_type_string;

      option = (ply_command_option_t *) ply_list_node_get_data (option_node);

      option_type_string = get_type_string (option->type);

      option_width = command->longest_option_length + 2 -
        (strlen(option->name) + strlen(option_type_string));

      ply_buffer_append (buffer,
                         "  --%s%s",
                         option->name,
                         option_type_string);

      ply_buffer_append (buffer, "%*s %s\n",
                         option_width,
                         "",
                         option->description ? option->description : "");

      option_node = ply_list_get_next_node (command->options, option_node);
    }
}

char *
ply_command_parser_get_help_string (ply_command_parser_t *parser)
{
  ply_buffer_t *buffer;
  ply_list_node_t *command_node;
  char *help_string;
  int longest_subcommand;

  buffer = ply_buffer_new ();

  append_usage_line_to_buffer (parser, buffer);
  ply_buffer_append (buffer, "\n");
  ply_buffer_append (buffer, "Options:\n");
  append_command_options_to_buffer (parser, parser->main_command, buffer);
  ply_buffer_append (buffer, "\n");

  if (ply_list_get_length (parser->available_subcommands) > 0)
    ply_buffer_append (buffer, "Available commands:\n");

  /* get longest subcommand */
  longest_subcommand = -1;
  command_node = ply_list_get_first_node (parser->available_subcommands);
  while (command_node != NULL)
    {
      ply_command_t *command;
      command = (ply_command_t *) ply_list_node_get_data (command_node);
      longest_subcommand = MAX (longest_subcommand, (int)strlen (command->name));
      command_node = ply_list_get_next_node (parser->available_subcommands,
                                             command_node);
    }
  command_node = ply_list_get_first_node (parser->available_subcommands);
  while (command_node != NULL)
    {
      ply_command_t *command;

      command = (ply_command_t *) ply_list_node_get_data (command_node);

      ply_buffer_append (buffer, "  %s%*s %s\n",
                         command->name,
                         (int) (longest_subcommand + 2 - strlen (command->name)),
                         "",
                         command->description);

      command_node = ply_list_get_next_node (parser->available_subcommands,
                                             command_node);
    }

  command_node = ply_list_get_first_node (parser->available_subcommands);
  while (command_node != NULL)
    {
      ply_command_t *command;

      command = (ply_command_t *) ply_list_node_get_data (command_node);

      if (ply_list_get_first_node (command->options) != NULL)
        {
          ply_buffer_append (buffer, "\nOptions for %s command:\n", command->name);

          append_command_options_to_buffer (parser, command, buffer);
        }

      command_node = ply_list_get_next_node (parser->available_subcommands,
                                             command_node);
    }

  help_string = ply_buffer_steal_bytes (buffer);
  ply_buffer_free (buffer);

  return help_string;
}

static void
ply_command_add_option (ply_command_t *command,
                        const char    *name,
                        const char    *description,
                        ply_command_option_type_t type)
{
  ply_command_option_t *option;

  option = ply_command_option_new (name, description, type);

  ply_list_append_data (command->options, option);

  command->longest_option_length = MAX (command->longest_option_length,
                                        strlen (name)
                                        + 1
                                        + strlen (get_type_string (type)));
}

static ply_command_option_t *
ply_command_get_option (ply_command_t *command,
                        const char    *option_name)
{
  ply_command_option_t *option;
  ply_list_node_t *node;

  option = NULL;
  node = ply_list_get_first_node (command->options);
  while (node != NULL)
    {
      ply_list_node_t *next_node;

      option = (ply_command_option_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (command->options, node);

      if (strcmp (option_name, option->name) == 0)
        break;

      node = next_node;
    }

  if (node == NULL)
      return NULL;

  return option;
}

ply_command_parser_t *
ply_command_parser_new (const char *name,
                        const char *description)
{
  ply_command_parser_t *command_parser;

  command_parser = calloc (1, sizeof (ply_command_parser_t));

  command_parser->main_command = ply_command_new (name, description, NULL, NULL);
  command_parser->available_subcommands = ply_list_new ();
  command_parser->read_subcommands = ply_list_new ();

  return command_parser;
}

void
ply_command_parser_free (ply_command_parser_t *command_parser)
{
  ply_list_node_t *command_node;

  if (command_parser == NULL)
    return;

  command_node = ply_list_get_first_node (command_parser->available_subcommands);
  while (command_node != NULL)
    {
      ply_command_t *command;

      command = (ply_command_t *) ply_list_node_get_data (command_node);
      ply_command_free (command);

      command_node = ply_list_get_next_node (command_parser->available_subcommands, command_node);
    }
  ply_list_free (command_parser->available_subcommands);
  ply_list_free (command_parser->read_subcommands);

  ply_command_free (command_parser->main_command);

  free (command_parser);
}

void
ply_command_parser_add_options (ply_command_parser_t *parser,
                                const char *first_variadic_argument, /*
                                const char *option_description,
                                ply_option_argument_type_t option_type */
                                ...)
{
  va_list args;
  const char *option_name;
  const char *option_description;
  ply_command_option_type_t option_type;

  assert (parser != NULL);

  option_name = first_variadic_argument;
  va_start (args, first_variadic_argument);
  while (option_name != NULL)
    {
      option_description = va_arg (args, const char *);
      option_type = va_arg (args, ply_command_option_type_t);

      ply_command_add_option (parser->main_command,
                              option_name, option_description, option_type);

      option_name = va_arg (args, const char *);
    }
  va_end (args);
}

void
ply_command_parser_add_command (ply_command_parser_t *parser,
                                const char *name, const char *description,
                                ply_command_handler_t handler,
                                void *handler_data,
                                const char *first_variadic_argument, /*
                                const char *option_description,
                                ply_command_option_type_t option_type */
                                ...)
{
  ply_command_t *command;
  va_list args;
  const char *option_name;
  const char *option_description;
  ply_command_option_type_t option_type;

  assert (parser != NULL);
  assert (name != NULL);

  command = ply_command_new (name, description, handler, handler_data);

  option_name = first_variadic_argument;
  va_start (args, first_variadic_argument);
  while (option_name != NULL)
    {
      option_description = va_arg (args, const char *);
      option_type = va_arg (args, ply_command_option_type_t);

      ply_command_add_option (command, option_name, option_description, option_type);

      option_name = va_arg (args, const char *);
    }
  va_end (args);

  ply_list_append_data (parser->available_subcommands, command);

  parser->longest_command_length = MAX (parser->longest_command_length, strlen (name));
}

static void
ply_command_parser_set_arguments (ply_command_parser_t *parser,
                                  char * const         *arguments,
                                  int                   number_of_arguments)
{
  int i;

  assert (parser != NULL);
  assert (arguments != NULL);

  ply_list_free (parser->arguments);
  parser->arguments = ply_list_new ();

  for (i = 0; arguments[i] != NULL; i++)
    ply_list_append_data (parser->arguments, arguments[i]);
}

static ply_command_t *
ply_command_parser_get_command (ply_command_parser_t *parser,
                                const char           *command_name)
{
  ply_command_t *command;
  ply_list_node_t *node;

  command = NULL;
  node = ply_list_get_first_node (parser->available_subcommands);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_list_node_t *alias_node;
      char *alias_name;

      command = (ply_command_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (parser->available_subcommands, node);

      if (strcmp (command_name, command->name) == 0)
        return command;
      alias_node = ply_list_get_first_node (command->aliases);
      while (alias_node)
        {
          alias_name = (char *) ply_list_node_get_data (alias_node);
          if (strcmp (command_name, alias_name) == 0)
            return command;
          
          alias_node = ply_list_get_next_node (command->aliases, alias_node);
        }

      node = next_node;
    }

  return NULL;
}

void ply_command_parser_add_command_alias (ply_command_parser_t *parser,
                                           const char           *name,
                                           const char           *alias)
{
  ply_command_t *command;

  assert (parser != NULL);
  assert (name != NULL);
  assert (alias != NULL);

  command = ply_command_parser_get_command (parser, name);
  ply_list_append_data (command->aliases, strdup(alias));
}

static void
ply_command_parser_get_option_for_command (ply_command_parser_t *parser,
                                           ply_command_t        *command,
                                           const char           *option_name,
                                           void                 *option_result,
                                           bool                 *option_is_set)
{
  ply_command_option_t *option;

  option = ply_command_get_option (command, option_name);

  if (option == NULL)
    return;

  if (option_result != NULL)
    {
      switch (option->type)
        {
        case PLY_COMMAND_OPTION_TYPE_FLAG:
        case PLY_COMMAND_OPTION_TYPE_BOOLEAN:
          {
            *(bool *)option_result = option->result.as_boolean;
          }
          break;

        case PLY_COMMAND_OPTION_TYPE_STRING:
          {
            if (option->result.as_string != NULL)
              *(char **)option_result = strdup (option->result.as_string);
            else
              *(char **)option_result = NULL;
          }
          break;

        case PLY_COMMAND_OPTION_TYPE_INTEGER:
          {
            *(int *)option_result = option->result.as_integer;
          }
          break;
        }
    }

  if (option_is_set != NULL)
    *option_is_set = option->was_set;
}

static void
ply_command_parser_get_options_for_command (ply_command_parser_t *parser,
                                            ply_command_t *command,
                                            const char *option_name,
                                            va_list args)
{

  assert (parser != NULL);
  assert (command != NULL);
  assert (option_name != NULL);

  while (option_name != NULL)
    {
      void *option_result;

      option_result = va_arg (args, void *);

      ply_command_parser_get_option_for_command (parser,
                                                 command,
                                                 option_name,
                                                 option_result,
                                                 NULL);

      option_name = va_arg (args, const char *);
    }
}

void
ply_command_parser_get_option (ply_command_parser_t *parser,
                               const char           *option_name,
                               void                 *option_result,
                               bool                 *option_is_set)
{
  assert (parser != NULL);
  assert (option_name != NULL);

  ply_command_parser_get_option_for_command (parser,
                                             parser->main_command,
                                             option_name,
                                             option_result,
                                             option_is_set);
}

void
ply_command_parser_get_options (ply_command_parser_t *parser,
                                const char *option_name, /*
                                void *      option_result,
                                bool *      option_was_set */
                                ...)
{
  va_list args;

  assert (parser != NULL);
  assert (option_name != NULL);

  va_start (args, option_name);
  ply_command_parser_get_options_for_command (parser, parser->main_command, option_name, args);
  va_end (args);
}

void
ply_command_parser_get_command_option (ply_command_parser_t *parser,
                                       const char           *command_name,
                                       const char           *option_name,
                                       void                 *option_result,
                                       bool                 *option_is_set)
{
  ply_command_t *command;

  assert (parser != NULL);
  assert (command_name != NULL);
  assert (option_name != NULL);

  command = ply_command_parser_get_command (parser, command_name);

  if (command == NULL)
    return;

  ply_command_parser_get_option_for_command (parser,
                                             parser->main_command,
                                             option_name,
                                             option_result,
                                             option_is_set);
}


void
ply_command_parser_get_command_options (ply_command_parser_t *parser,
                                        const char *command_name,
                                        const char *option_name, /*
                                        void *      option_result,
                                        bool *      option_was_set */
                                        ...)
{
  ply_command_t *command;
  va_list args;

  assert (parser != NULL);
  assert (command_name != NULL);
  assert (option_name != NULL);

  command = ply_command_parser_get_command (parser, command_name);

  if (command == NULL)
    return;

  va_start (args, option_name);
  ply_command_parser_get_options_for_command (parser, command, option_name, args);
  va_end (args);

  ply_list_append_data (parser->available_subcommands, command);
}

static void
ply_command_parser_detach_from_event_loop (ply_command_parser_t *parser)
{
  parser->loop = NULL;
}

void
ply_command_parser_stop_parsing_arguments (ply_command_parser_t *parser)
{
  assert (parser != NULL);

  if (parser->loop == NULL)
    return;

  ply_event_loop_stop_watching_for_exit (parser->loop,
                                         (ply_event_loop_exit_handler_t)
                                         ply_command_parser_detach_from_event_loop,
                                         parser);
}

static bool
argument_is_option (const char *argument)
{
  return argument[0] == '-' && argument[1] == '-';
}

static bool
ply_command_option_read_arguments (ply_command_option_t *option,
                                   ply_list_t           *arguments)

{
  ply_list_node_t *node;
  const char *argument;

  assert (option != NULL);

  node = ply_list_get_first_node (arguments);

  if (node == NULL)
    {
      if (option->type == PLY_COMMAND_OPTION_TYPE_BOOLEAN ||
          option->type == PLY_COMMAND_OPTION_TYPE_FLAG)
        {
          option->result.as_boolean = true;
          return true;
        }

      return false;
    }

  argument = (const char *) ply_list_node_get_data (node);

  assert (argument != NULL);

  if (argument_is_option (argument))
    {
      if (option->type == PLY_COMMAND_OPTION_TYPE_BOOLEAN ||
          option->type == PLY_COMMAND_OPTION_TYPE_FLAG)
        {
          option->result.as_boolean = true;
          return true;
        }

      return false;
    }

  switch (option->type)
    {
      case PLY_COMMAND_OPTION_TYPE_FLAG:
      case PLY_COMMAND_OPTION_TYPE_BOOLEAN:

          /* next argument isn't ours, so treat it like an unqualified
           * flag
           */
          if (rpmatch (argument) < 0)
            {
              option->result.as_boolean = true;
              return true;
            }

          option->result.as_boolean = (bool) rpmatch (argument);
          ply_list_remove_node (arguments, node);
          return true;

      case PLY_COMMAND_OPTION_TYPE_STRING:
        option->result.as_string = strdup (argument);
        ply_list_remove_node (arguments, node);
        return true;

      case PLY_COMMAND_OPTION_TYPE_INTEGER:
        {
         char *end;
         long argument_as_long;

         if (argument[0] == '\0')
           return false;

         argument_as_long = strtol (argument, &end, 0);

         if (*end != '\0')
           return false;

         if (argument_as_long == LONG_MIN &&
             errno == ERANGE)
           return false; 

         if (argument_as_long > INT_MAX)
           return false;

         option->result.as_integer = (int) argument_as_long;
         ply_list_remove_node (arguments, node);
         return true;
        }
    }

  return option;
}

static ply_command_option_t *
ply_command_read_option (ply_command_t *command,
                         ply_list_t    *arguments)
{
  ply_list_node_t *node;
  const char *argument;
  char *option_name;
  char *option_separator;
  ply_command_option_t *option;

  node = ply_list_get_first_node (arguments);

  if (node == NULL)
    return NULL;

  argument = (const char *) ply_list_node_get_data (node);

  assert (argument != NULL);

  if (!argument_is_option (argument))
    return NULL;

  option_name = strdup (argument + strlen ("--"));
  option_separator = strchr (option_name, '=');

  if (option_separator != NULL)
    {
      *option_separator = '\0';
      ply_list_insert_data (arguments,
                            (void *) (argument +
                            ((ptrdiff_t) (option_separator - option_name + 1)) +
                            strlen ("=") + 1),
                            node);
    }

  option = ply_command_get_option (command, option_name);
  free (option_name);

  if (option == NULL)
    return NULL;

  ply_list_remove_node (arguments, node);

  if (ply_command_option_read_arguments (option, arguments))
    option->was_set = true;

  return option;
}

static bool
argument_is_command (const char *argument)
{
  return !argument_is_option (argument);
}

static ply_command_t *
ply_command_parser_read_command (ply_command_parser_t *parser)
{
  ply_list_node_t *node;
  const char *argument;
  ply_command_t *command;

  node = ply_list_get_first_node (parser->arguments);

  if (node == NULL)
    return NULL;

  argument = (const char *) ply_list_node_get_data (node);

  assert (argument != NULL);

  if (!argument_is_command (argument))
    return NULL;

  command = ply_command_parser_get_command (parser, argument);

  if (command == NULL)
    return NULL;

  ply_list_remove_node (parser->arguments, node);

  while (ply_command_read_option (command, parser->arguments) != NULL);

  return command;
}

static void
on_command_dispatch_timeout (ply_command_parser_t *parser)
{

  ply_command_t *command;
  ply_list_node_t *node;

  command = NULL;
  node = ply_list_get_first_node (parser->read_subcommands);

  if (node == NULL)
    {
      parser->dispatch_is_queued = false;
      return;
    }

  command = (ply_command_t *) ply_list_node_get_data (node);

  assert (command != NULL);

  ply_event_loop_watch_for_timeout (parser->loop, 0.01,
                                    (ply_event_loop_timeout_handler_t)
                                    on_command_dispatch_timeout,
                                    parser);

  if (command->handler != NULL)
    command->handler (command->handler_data, command->name);

  ply_list_remove_node (parser->read_subcommands, node);
}

static void
ply_command_parser_queue_command_dispatch (ply_command_parser_t *parser)
{
  if (parser->dispatch_is_queued)
    return;

  ply_event_loop_watch_for_timeout (parser->loop, 0.01,
                                    (ply_event_loop_timeout_handler_t)
                                    on_command_dispatch_timeout,
                                    parser);
}

bool
ply_command_parser_parse_arguments (ply_command_parser_t *parser,
                                    ply_event_loop_t     *loop,
                                    char * const         *arguments,
                                    int                   number_of_arguments)
{
  ply_command_t *command;
  bool parsed_arguments;

  assert (parser != NULL);
  assert (loop != NULL);
  assert (parser->loop == NULL);
  assert (number_of_arguments >= 1);

  parsed_arguments = false;

  if (number_of_arguments == 1)
    return true;

  ply_command_parser_set_arguments (parser, arguments + 1, number_of_arguments - 1);

  while (ply_command_read_option (parser->main_command, parser->arguments) != NULL)
    parsed_arguments = true;

  parser->loop = loop;

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t)
                                 ply_command_parser_detach_from_event_loop,
                                 parser);

  while ((command = ply_command_parser_read_command (parser)) != NULL)
    ply_list_append_data (parser->read_subcommands, command);

  if (ply_list_get_length (parser->read_subcommands) > 0)
    {
      ply_command_parser_queue_command_dispatch (parser);
      parsed_arguments = true;
    }

  return parsed_arguments;
}

#ifdef PLY_COMMAND_PARSER_ENABLE_TEST

#include <stdio.h>

#include "ply-command-parser.h"
#include "ply-event-loop.h"
#include "ply-logger.h"

static void
on_ask_for_password (ply_command_parser_t *parser,
                     const char *command)
{
  char *prompt;
  char *program;

  prompt = NULL;
  program = NULL;
  ply_command_parser_get_command_options (parser, command, "prompt", &prompt, "command", &program, NULL);

  printf ("ask for password with prompt '%s' feed result to '%s'\n", prompt, program);
  free (prompt);
  free (program);
}

static void
on_show_splash (ply_command_parser_t *parser,
                const char *command)
{
  char *plugin_name;

  plugin_name = NULL;
  ply_command_parser_get_command_options (parser, command, "plugin-name", &plugin_name, NULL);

  printf ("show splash plugin '%s'\n", plugin_name);
  free (plugin_name);
}

int
main (int    argc,
      char **argv)
{
  ply_event_loop_t *loop;
  ply_command_parser_t *parser;
  bool should_help;

  loop = ply_event_loop_new ();
  parser = ply_command_parser_new (argv[0], "Test Program");

  ply_command_parser_add_options (parser,
                                  "help", "This help message", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (parser,
                                  "ask-for-password",
                                  "Ask user for password",
                                  (ply_command_handler_t)
                                  on_ask_for_password, parser,
                                  "command", "command to pipe result to", PLY_COMMAND_OPTION_TYPE_STRING,
                                  "prompt", "string to present to user", PLY_COMMAND_OPTION_TYPE_STRING,
                                  "output-result", "print result", PLY_COMMAND_OPTION_TYPE_BOOLEAN,
                                  NULL);

  ply_command_parser_add_command (parser,
                                  "show-splash",
                                  "Show splash to user",
                                  (ply_command_handler_t)
                                  on_show_splash, parser,
                                  "plugin-name", "name of the plugin to run", PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);

  if (!ply_command_parser_parse_arguments (parser, loop, argv, argc))
    {
      ply_error ("couldn't parse arguments");
      return 1;
    }


  ply_command_parser_get_options (parser, "help", &should_help, NULL);

  if (should_help)
    {
      char *usage;
      usage = ply_command_parser_get_help_string (parser);
      printf ("%s\n", usage);
      free (usage);
      return 0;
    }

  ply_event_loop_run (loop);

  ply_command_parser_free (parser);

  return 0;
}

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
