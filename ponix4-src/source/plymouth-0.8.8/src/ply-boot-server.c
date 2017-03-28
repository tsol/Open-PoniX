/* ply-boot-server.c - listens for and processes boot-status events
 *
 * Copyright (C) 2007 Red Hat, Inc.
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
#include "ply-boot-server.h"

#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ply-buffer.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-trigger.h"
#include "ply-utils.h"

typedef struct
{
  int fd;
  ply_fd_watch_t *watch;
  ply_boot_server_t *server;
  uid_t uid;
  pid_t pid;

  uint32_t credentials_read : 1;
} ply_boot_connection_t;

struct _ply_boot_server
{
  ply_event_loop_t *loop;
  ply_list_t *connections;
  ply_list_t *cached_passwords;
  int socket_fd;

  ply_boot_server_update_handler_t update_handler;
  ply_boot_server_change_mode_handler_t change_mode_handler;
  ply_boot_server_system_update_handler_t system_update_handler;
  ply_boot_server_newroot_handler_t newroot_handler;
  ply_boot_server_system_initialized_handler_t system_initialized_handler;
  ply_boot_server_error_handler_t error_handler;
  ply_boot_server_show_splash_handler_t show_splash_handler;
  ply_boot_server_hide_splash_handler_t hide_splash_handler;
  ply_boot_server_ask_for_password_handler_t ask_for_password_handler;
  ply_boot_server_ask_question_handler_t ask_question_handler;
  ply_boot_server_display_message_handler_t display_message_handler;
  ply_boot_server_hide_message_handler_t hide_message_handler;
  ply_boot_server_watch_for_keystroke_handler_t watch_for_keystroke_handler;
  ply_boot_server_ignore_keystroke_handler_t ignore_keystroke_handler;
  ply_boot_server_progress_pause_handler_t progress_pause_handler;
  ply_boot_server_progress_unpause_handler_t progress_unpause_handler;
  ply_boot_server_deactivate_handler_t deactivate_handler;
  ply_boot_server_reactivate_handler_t reactivate_handler;
  ply_boot_server_quit_handler_t quit_handler;
  ply_boot_server_has_active_vt_handler_t has_active_vt_handler;
  void *user_data;

  uint32_t is_listening : 1;
};

ply_boot_server_t *
ply_boot_server_new (ply_boot_server_update_handler_t  update_handler,
                     ply_boot_server_change_mode_handler_t  change_mode_handler,
                     ply_boot_server_system_update_handler_t  system_update_handler,
                     ply_boot_server_ask_for_password_handler_t ask_for_password_handler,
                     ply_boot_server_ask_question_handler_t ask_question_handler,
                     ply_boot_server_display_message_handler_t display_message_handler,
                     ply_boot_server_hide_message_handler_t hide_message_handler,
                     ply_boot_server_watch_for_keystroke_handler_t watch_for_keystroke_handler,
                     ply_boot_server_ignore_keystroke_handler_t ignore_keystroke_handler,
                     ply_boot_server_progress_pause_handler_t progress_pause_handler,
                     ply_boot_server_progress_unpause_handler_t progress_unpause_handler,
                     ply_boot_server_show_splash_handler_t show_splash_handler,
                     ply_boot_server_hide_splash_handler_t hide_splash_handler,
                     ply_boot_server_newroot_handler_t newroot_handler,
                     ply_boot_server_system_initialized_handler_t initialized_handler,
                     ply_boot_server_error_handler_t error_handler,
                     ply_boot_server_deactivate_handler_t deactivate_handler,
                     ply_boot_server_reactivate_handler_t reactivate_handler,
                     ply_boot_server_quit_handler_t quit_handler,
                     ply_boot_server_has_active_vt_handler_t has_active_vt_handler,
                     void                             *user_data)
{
  ply_boot_server_t *server;

  server = calloc (1, sizeof (ply_boot_server_t));
  server->connections = ply_list_new ();
  server->cached_passwords = ply_list_new ();
  server->loop = NULL;
  server->is_listening = false;
  server->update_handler = update_handler;
  server->change_mode_handler = change_mode_handler;
  server->system_update_handler = system_update_handler;
  server->ask_for_password_handler = ask_for_password_handler;
  server->ask_question_handler = ask_question_handler;
  server->display_message_handler = display_message_handler;
  server->hide_message_handler = hide_message_handler;
  server->watch_for_keystroke_handler = watch_for_keystroke_handler;
  server->ignore_keystroke_handler = ignore_keystroke_handler;
  server->progress_pause_handler = progress_pause_handler;
  server->progress_unpause_handler = progress_unpause_handler;
  server->newroot_handler = newroot_handler;
  server->error_handler = error_handler;
  server->system_initialized_handler = initialized_handler;
  server->show_splash_handler = show_splash_handler;
  server->hide_splash_handler = hide_splash_handler;
  server->deactivate_handler = deactivate_handler;
  server->reactivate_handler = reactivate_handler;
  server->quit_handler = quit_handler;
  server->has_active_vt_handler = has_active_vt_handler;
  server->user_data = user_data;

  return server;
}

static void ply_boot_connection_on_hangup (ply_boot_connection_t *connection);

void
ply_boot_server_free (ply_boot_server_t *server)
{
  ply_list_node_t *node;
  if (server == NULL)
    return;
  while ((node = ply_list_get_first_node(server->connections)))
    {
      ply_boot_connection_t *connection = ply_list_node_get_data (node);
      ply_boot_connection_on_hangup (connection);
    }
  ply_list_free (server->connections);
  ply_list_free (server->cached_passwords);
  free (server);
}

static ply_boot_connection_t *
ply_boot_connection_new (ply_boot_server_t *server,
                         int                fd)
{
  ply_boot_connection_t *connection;

  connection = calloc (1, sizeof (ply_boot_server_t));
  connection->fd = fd;
  connection->server = server;
  connection->watch = NULL;

  return connection;
}

static void
ply_boot_connection_free (ply_boot_connection_t *connection)
{
  if (connection == NULL)
    return;

  close (connection->fd);
  free (connection);
}

bool
ply_boot_server_listen (ply_boot_server_t *server)
{
  assert (server != NULL);

  server->socket_fd =
      ply_listen_to_unix_socket (PLY_BOOT_PROTOCOL_TRIMMED_ABSTRACT_SOCKET_PATH,
                                 PLY_UNIX_SOCKET_TYPE_TRIMMED_ABSTRACT);

  if (server->socket_fd < 0)
    return false;

  return true;
}

void
ply_boot_server_stop_listening (ply_boot_server_t *server)
{
  assert (server != NULL);
}

static bool
ply_boot_connection_read_request (ply_boot_connection_t  *connection,
                                  char                  **command,
                                  char                  **argument)
{
  uint8_t header[2];

  assert (connection != NULL);
  assert (connection->fd >= 0);

  connection->credentials_read = false;

  if (!ply_read (connection->fd, header, sizeof (header)))
    return false;

  *command = calloc (2, sizeof (char));
  *command[0] = header[0];

  *argument = NULL;
  if (header[1] == '\002')
    {
      uint8_t argument_size;

      if (!ply_read (connection->fd, &argument_size, sizeof (uint8_t)))
        {
          free (*command);
          return false;
        }

      *argument = calloc (argument_size, sizeof (char));

      if (!ply_read (connection->fd, *argument, argument_size))
        {
          free (*argument);
          free (*command);
          return false;
        }
    }

  if (!ply_get_credentials_from_fd (connection->fd, &connection->pid, &connection->uid, NULL))
    {
      ply_trace ("couldn't read credentials from connection: %m");
      free (*argument);
      free (*command);
      return false;
    }
  connection->credentials_read = true;

  return true;
}

static bool
ply_boot_connection_is_from_root (ply_boot_connection_t *connection)
{
  if (!connection->credentials_read)
    {
      ply_trace ("Asked if connection is from root, but haven't checked credentials yet");
      return false;
    }

  return connection->uid == 0;
}

static void
ply_boot_connection_send_answer (ply_boot_connection_t *connection,
                                 const char            *answer)
{
  uint32_t size;

  /* splash plugin isn't able to ask for password,
   * punt to client
   */
  if (answer == NULL)
    {
      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER)))
        ply_trace ("could not finish writing no answer reply: %m");
    }
  else
    {
      size = strlen (answer);

      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER)) ||
          !ply_write_uint32 (connection->fd,
                             size) ||
          !ply_write (connection->fd,
                      answer, size))
          ply_trace ("could not finish writing answer: %m");

    }

}

static void
ply_boot_connection_on_password_answer (ply_boot_connection_t *connection,
                                        const char            *password)
{
  ply_trace ("got password answer");

  ply_boot_connection_send_answer (connection, password);
  if (password != NULL)
    ply_list_append_data (connection->server->cached_passwords,
                          strdup (password));

}

static void
ply_boot_connection_on_deactivated (ply_boot_connection_t *connection)
{
  ply_trace ("deactivated");
  if (!ply_write (connection->fd,
                  PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                  strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
    {
      ply_trace ("could not finish writing deactivate reply: %m");
    }
}

static void
ply_boot_connection_on_quit_complete (ply_boot_connection_t *connection)
{
  ply_trace ("quit complete");
  if (!ply_write (connection->fd,
                  PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                  strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
    {
      ply_trace ("could not finish writing quit reply: %m");
    }
}

static void
ply_boot_connection_on_question_answer (ply_boot_connection_t *connection,
                                        const char             *answer)
{
  ply_trace ("got question answer: %s", answer);
  ply_boot_connection_send_answer (connection, answer);
}

static void
ply_boot_connection_on_keystroke_answer (ply_boot_connection_t *connection,
                                        const char            *key)
{
  ply_trace ("got key: %s", key);
  ply_boot_connection_send_answer (connection, key);
}

static void
print_connection_process_identity (ply_boot_connection_t *connection)
{
  char *command_line, *parent_command_line;
  pid_t parent_pid;

  command_line = ply_get_process_command_line (connection->pid);

  if (connection->pid == 1)
    {
      ply_trace ("connection is from toplevel init process (%s)", command_line);
    }
  else
    {
      parent_pid = ply_get_process_parent_pid (connection->pid); parent_command_line = ply_get_process_command_line (parent_pid);

      ply_trace ("connection is from pid %ld (%s) with parent pid %ld (%s)",
                 (long) connection->pid, command_line,
                 (long) parent_pid, parent_command_line);

      free (parent_command_line);
    }

  free (command_line);
}

static void
ply_boot_connection_on_request (ply_boot_connection_t *connection)
{
  ply_boot_server_t *server;
  char *command, *argument;

  assert (connection != NULL);
  assert (connection->fd >= 0);

  server = connection->server;
  assert (server != NULL);

  if (!ply_boot_connection_read_request (connection,
                                         &command, &argument))
    {
      ply_trace ("could not read connection request");
      return;
    }

  if (ply_is_tracing ())
    print_connection_process_identity (connection);

  if (!ply_boot_connection_is_from_root (connection))
    {
      ply_error ("request came from non-root user");

      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK)))
        ply_trace ("could not finish writing is-not-root nak: %m");

      free (command);
      return;
    }

  if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_UPDATE) == 0)
    {

      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
        ply_trace ("could not finish writing update reply: %m");

      ply_trace ("got update request");
      if (server->update_handler != NULL)
        server->update_handler (server->user_data, argument, server);
      free (argument);
      free (command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_CHANGE_MODE) == 0)
    {
      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
        ply_trace ("could not finish writing update reply: %m");

      ply_trace ("got change mode notification");
      if (server->change_mode_handler != NULL)
        server->change_mode_handler (server->user_data, argument, server);
      free (argument);
      free (command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_UPDATE) == 0)
    {
      long int value;
      char *endptr = NULL;

      value = strtol (argument, &endptr, 10);
      if (endptr == NULL || *endptr != '\0' || value < 0 || value > 100)
        {
          ply_error ("failed to parse percentage %s", argument);
          value = 0;
        }

      ply_trace ("got system-update notification %li%%", value);
      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
        ply_trace ("could not finish writing update reply: %m");

      if (server->system_update_handler != NULL)
        server->system_update_handler (server->user_data, value, server);
      free (argument);
      free (command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_INITIALIZED) == 0)
    {
      ply_trace ("got system initialized notification");
      if (server->system_initialized_handler != NULL)
        server->system_initialized_handler (server->user_data, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_ERROR) == 0)
    {
      ply_trace ("got error notification");
      if (server->error_handler != NULL)
        server->error_handler (server->user_data, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_SPLASH) == 0)
    {
      ply_trace ("got show splash request");
      if (server->show_splash_handler != NULL)
        server->show_splash_handler (server->user_data, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_SPLASH) == 0)
    {
      ply_trace ("got hide splash request");
      if (server->hide_splash_handler != NULL)
        server->hide_splash_handler (server->user_data, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_DEACTIVATE) == 0)
    {
      ply_trigger_t *deactivate_trigger;

      ply_trace ("got deactivate request");

      deactivate_trigger = ply_trigger_new (NULL);

      ply_trigger_add_handler (deactivate_trigger,
                               (ply_trigger_handler_t)
                               ply_boot_connection_on_deactivated,
                               connection);

      if (server->deactivate_handler != NULL)
        server->deactivate_handler (server->user_data, deactivate_trigger, server);

      free (argument);
      free (command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_REACTIVATE) == 0)
    {
      ply_trace ("got reactivate request");
      if (server->reactivate_handler != NULL)
        server->reactivate_handler (server->user_data, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT) == 0)
    {
      bool retain_splash;
      ply_trigger_t *quit_trigger;

      retain_splash = (bool) argument[0];

      ply_trace ("got quit %srequest", retain_splash? "--retain-splash " : "");

      quit_trigger = ply_trigger_new (NULL);

      ply_trigger_add_handler (quit_trigger,
                               (ply_trigger_handler_t)
                               ply_boot_connection_on_quit_complete,
                               connection);

      if (server->quit_handler != NULL)
        server->quit_handler (server->user_data, retain_splash, quit_trigger, server);

      free(argument);
      free(command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PASSWORD) == 0)
    {
      ply_trigger_t *answer;

      ply_trace ("got password request");

      answer = ply_trigger_new (NULL);
      ply_trigger_add_handler (answer,
                               (ply_trigger_handler_t)
                               ply_boot_connection_on_password_answer,
                               connection);

      if (server->ask_for_password_handler != NULL)
        server->ask_for_password_handler (server->user_data,
                                          argument,
                                          answer,
                                          server);
      /* will reply later
       */
      free(command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_CACHED_PASSWORD) == 0)
    {
      ply_list_node_t *node;
      ply_buffer_t *buffer;
      size_t buffer_size;
      uint32_t size;

      ply_trace ("got cached password request");

      buffer = ply_buffer_new ();

      node = ply_list_get_first_node (server->cached_passwords);

      ply_trace ("There are %d cached passwords",
                 ply_list_get_length (server->cached_passwords));

      /* Add each answer separated by their NUL terminators into
       * a buffer that we write out to the client
       */
      while (node != NULL)
        {
          ply_list_node_t *next_node;
          const char *password;

          next_node = ply_list_get_next_node (server->cached_passwords, node);
          password = (const char *) ply_list_node_get_data (node);

          ply_buffer_append_bytes (buffer,
                                   password,
                                   strlen (password) + 1);
          node = next_node;
        }

      buffer_size = ply_buffer_get_size (buffer);

      /* splash plugin doesn't have any cached passwords
      */
      if (buffer_size == 0)
        {
          ply_trace ("Responding with 'no answer' reply since there are currently "
                     "no cached answers");
          if (!ply_write (connection->fd,
                          PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER,
                          strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER)))
              ply_trace ("could not finish writing no answer reply: %m");
        }
      else
        {
          size = buffer_size;

          ply_trace ("writing %d cached answers",
                     ply_list_get_length (server->cached_passwords));
          if (!ply_write (connection->fd,
                          PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS,
                          strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS)) ||
              !ply_write_uint32 (connection->fd,
                                 size) ||
              !ply_write (connection->fd,
                          ply_buffer_get_bytes (buffer), size))
              ply_trace ("could not finish writing cached answer reply: %m");
        }

      ply_buffer_free (buffer);
      free(command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUESTION) == 0)
    {
      ply_trigger_t *answer;

      ply_trace ("got question request");

      answer = ply_trigger_new (NULL);
      ply_trigger_add_handler (answer,
                               (ply_trigger_handler_t)
                               ply_boot_connection_on_question_answer,
                               connection);

      if (server->ask_question_handler != NULL)
        server->ask_question_handler (server->user_data,
                                          argument,
                                          answer,
                                          server);
      /* will reply later
       */
      free(command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_MESSAGE) == 0)
    {
      ply_trace ("got show message request");
      if (server->display_message_handler != NULL)
        server->display_message_handler(server->user_data, argument, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_MESSAGE) == 0)
    {
      ply_trace ("got hide message request");
      if (server->hide_message_handler != NULL)
        server->hide_message_handler(server->user_data, argument, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE) == 0)
    {
      ply_trigger_t *answer;

      ply_trace ("got keystroke request");

      answer = ply_trigger_new (NULL);
      ply_trigger_add_handler (answer,
                               (ply_trigger_handler_t)
                               ply_boot_connection_on_keystroke_answer,
                               connection);

      if (server->watch_for_keystroke_handler != NULL)
        server->watch_for_keystroke_handler (server->user_data,
                                          argument,
                                          answer,
                                          server);
      /* will reply later
       */
      free(command);
      return;
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE_REMOVE) == 0)
    {
      ply_trace ("got keystroke remove request");
      if (server->ignore_keystroke_handler != NULL)
        server->ignore_keystroke_handler (server->user_data,
                                          argument,
                                          server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_PAUSE) == 0)
    {
      ply_trace ("got progress pause request");
      if (server->progress_pause_handler != NULL)
        server->progress_pause_handler (server->user_data,
                                        server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_UNPAUSE) == 0)
    {
      ply_trace ("got progress unpause request");
      if (server->progress_unpause_handler != NULL)
        server->progress_unpause_handler (server->user_data,
                                          server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_NEWROOT) == 0)
    {
      ply_trace ("got newroot request");
      if (server->newroot_handler != NULL)
        server->newroot_handler(server->user_data, argument, server);
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HAS_ACTIVE_VT) == 0)
    {
      bool answer = false;

      ply_trace ("got has_active vt? request");
      if (server->has_active_vt_handler != NULL)
        answer = server->has_active_vt_handler(server->user_data, server);

      if (!answer)
        {
          if (!ply_write (connection->fd,
                          PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK,
                          strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK)))
            ply_trace ("could not finish writing nak: %m");

          free(command);
          return;
        }
    }
  else if (strcmp (command, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PING) != 0)
    {
      ply_error ("received unknown command '%s' from client", command);

      if (!ply_write (connection->fd,
                      PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK,
                      strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NAK)))
          ply_trace ("could not finish writing ping reply: %m");

      free(command);
      return;
    }

  if (!ply_write (connection->fd, 
                  PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK,
                  strlen (PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK)))
    {
      ply_trace ("could not finish writing ack: %m");
    }
  free(command);
}

static void
ply_boot_connection_on_hangup (ply_boot_connection_t *connection)
{
  ply_list_node_t *node;
  ply_boot_server_t *server;

  assert (connection != NULL);
  assert (connection->server != NULL);

  server = connection->server;

  node = ply_list_find_node (server->connections, connection);

  assert (node != NULL);

  ply_boot_connection_free (connection);
  ply_list_remove_node (server->connections, node);
}

static void
ply_boot_server_on_new_connection (ply_boot_server_t *server)
{
  ply_boot_connection_t *connection;
  int fd;

  assert (server != NULL);

  fd = accept (server->socket_fd, NULL, NULL);

  if (fd < 0)
    return;

  connection = ply_boot_connection_new (server, fd);

  connection->watch = 
      ply_event_loop_watch_fd (server->loop, fd,
                               PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                               (ply_event_handler_t)
                               ply_boot_connection_on_request,
                               (ply_event_handler_t)
                               ply_boot_connection_on_hangup,
                               connection);

  ply_list_append_data (server->connections, connection);
}

static void
ply_boot_server_on_hangup (ply_boot_server_t *server)
{
  assert (server != NULL);
}

static void
ply_boot_server_detach_from_event_loop (ply_boot_server_t *server)
{
  assert (server != NULL);
  server->loop = NULL;
}

void
ply_boot_server_attach_to_event_loop (ply_boot_server_t *server,
                                      ply_event_loop_t  *loop)
{
  assert (server != NULL);
  assert (loop != NULL);
  assert (server->loop == NULL);
  assert (server->socket_fd >= 0);

  server->loop = loop;

  ply_event_loop_watch_fd (loop, server->socket_fd,
                           PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                           (ply_event_handler_t)
                           ply_boot_server_on_new_connection,
                           (ply_event_handler_t)
                           ply_boot_server_on_hangup,
                           server);
  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t) 
                                 ply_boot_server_detach_from_event_loop,
                                 server); 
}

#ifdef PLY_BOOT_SERVER_ENABLE_TEST

#include <stdio.h>

#include "ply-event-loop.h"
#include "ply-boot-server.h"

static void 
on_update (ply_event_loop_t  *loop,
           const char        *status)
{
  printf ("new status is '%s'\n", status);
}

static void
on_newroot (ply_event_loop_t *loop)
{
  printf ("got newroot request\n");
}

static void
on_system_initialized (ply_event_loop_t *loop)
{
  printf ("got sysinit done request\n");
}

static void
on_show_splash (ply_event_loop_t *loop)
{
  printf ("got show splash request\n");
}

static void
on_hide_splash (ply_event_loop_t *loop)
{
  printf ("got hide splash request\n");
}

static void
on_deactivate (ply_event_loop_t *loop)
{
  printf ("got deactivate request\n");
}

static void
on_reactivate (ply_event_loop_t *loop)
{
  printf ("got reactivate request\n");
}

static void
on_quit (ply_event_loop_t *loop)
{
  printf ("got quit request, quiting...\n");
  ply_event_loop_exit (loop, 0);
}

static void
on_error (ply_event_loop_t *loop)
{
  printf ("got error starting service\n");
}

static char *
on_ask_for_password (ply_event_loop_t *loop)
{
  printf ("got password request, returning 'password'...\n");

  return strdup ("password");
}

static void
on_ask_question (ply_event_loop_t *loop)
{
  printf ("got question request\n");
  return;
}

static void
on_display_message (ply_event_loop_t *loop)
{
  printf ("got display message request\n");
  return;
}

static void
on_hide_message (ply_event_loop_t *loop)
{
  printf ("got hide message request\n");
  return;
}

static void
on_watch_for_keystroke (ply_event_loop_t *loop)
{
  printf ("got keystroke request\n");

  return;
}

static void
on_progress_pause (ply_event_loop_t *loop)
{
  printf ("got progress pause request\n");

  return;
}

static void
on_progress_unpause (ply_event_loop_t *loop)
{
  printf ("got progress unpause request\n");

  return;
}

static void
on_ignore_keystroke (ply_event_loop_t *loop)
{
  printf ("got keystroke ignore request\n");

  return;
}

static bool
on_has_active_vt (ply_event_loop_t *loop)
{
  printf ("got has_active vt? request\n");
  return true;
}

int
main (int    argc,
      char **argv)
{
  ply_event_loop_t *loop;
  ply_boot_server_t *server;
  int exit_code;

  exit_code = 0;

  loop = ply_event_loop_new ();

  server = ply_boot_server_new ((ply_boot_server_update_handler_t) on_update,
                                (ply_boot_server_change_mode_handler_t) on_change_mode,
                                (ply_boot_server_system_update_handler_t) on_system_update,
                                (ply_boot_server_ask_for_password_handler_t) on_ask_for_password,
                                (ply_boot_server_ask_question_handler_t) on_ask_question,
                                (ply_boot_server_display_message_handler_t) on_display_message,
                                (ply_boot_server_hide_message_handler_t) on_hide_message,
                                (ply_boot_server_watch_for_keystroke_handler_t) on_watch_for_keystroke,
                                (ply_boot_server_ignore_keystroke_handler_t) on_ignore_keystroke,
                                (ply_boot_server_progress_pause_handler_t) on_progress_pause,
                                (ply_boot_server_progress_unpause_handler_t) on_progress_unpause,
                                (ply_boot_server_show_splash_handler_t) on_show_splash,
                                (ply_boot_server_hide_splash_handler_t) on_hide_splash,
                                (ply_boot_server_newroot_handler_t) on_newroot,
                                (ply_boot_server_system_initialized_handler_t) on_system_initialized,
                                (ply_boot_server_error_handler_t) on_error,
                                (ply_boot_server_deactivate_handler_t) on_deactivate,
                                (ply_boot_server_reactivate_handler_t) on_reactivate,
                                (ply_boot_server_quit_handler_t) on_quit,
                                (ply_boot_server_has_active_vt_handler_t) on_has_active_vt,
                                loop);

  if (!ply_boot_server_listen (server))
    {
      perror ("could not start boot status daemon");
      return errno;
    }

  ply_boot_server_attach_to_event_loop (server, loop);
  exit_code = ply_event_loop_run (loop);
  ply_boot_server_free (server);

  return exit_code;
}

#endif /* PLY_BOOT_SERVER_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
