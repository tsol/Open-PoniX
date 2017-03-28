/* ply-boot-client.h - APIs for talking to the boot status daemon
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
#include "ply-boot-client.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ply-array.h"
#include "ply-event-loop.h"
#include "ply-list.h"
#include "ply-logger.h"
#include "ply-utils.h"

struct _ply_boot_client
{
  ply_event_loop_t *loop;
  ply_fd_watch_t *daemon_can_take_request_watch;
  ply_fd_watch_t *daemon_has_reply_watch;
  ply_list_t *requests_to_send;
  ply_list_t *requests_waiting_for_replies;
  int socket_fd;

  ply_boot_client_disconnect_handler_t disconnect_handler;
  void *disconnect_handler_user_data;

  uint32_t is_connected : 1;
};

typedef struct
{
  ply_boot_client_t *client;
  char *command;
  char *argument;
  ply_boot_client_response_handler_t handler;
  ply_boot_client_response_handler_t failed_handler;
  void *user_data;
} ply_boot_client_request_t;

static void ply_boot_client_cancel_request (ply_boot_client_t         *client,
                                            ply_boot_client_request_t *request);

ply_boot_client_t *
ply_boot_client_new (void)
{
  ply_boot_client_t *client;

  client = calloc (1, sizeof (ply_boot_client_t));
  client->daemon_can_take_request_watch = NULL;
  client->daemon_has_reply_watch = NULL;
  client->requests_to_send = ply_list_new ();
  client->requests_waiting_for_replies = ply_list_new ();
  client->loop = NULL;
  client->is_connected = false;
  client->disconnect_handler = NULL;
  client->disconnect_handler_user_data = NULL;

  return client;
}

static void
ply_boot_client_cancel_unsent_requests (ply_boot_client_t *client)
{
  ply_list_node_t *node;

  if (ply_list_get_length (client->requests_to_send) == 0)
      return;

  node = ply_list_get_first_node (client->requests_to_send);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_boot_client_request_t *request;

      request = (ply_boot_client_request_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (client->requests_to_send, node);

      ply_boot_client_cancel_request (client, request);
      ply_list_remove_node (client->requests_to_send, node);

      node = next_node;
    }

  if (client->daemon_can_take_request_watch != NULL)
    {
      assert (client->loop != NULL);

      ply_event_loop_stop_watching_fd (client->loop, 
                                       client->daemon_can_take_request_watch);
      client->daemon_can_take_request_watch = NULL;
    }
}

static void
ply_boot_client_cancel_requests_waiting_for_replies (ply_boot_client_t *client)
{
  ply_list_node_t *node;

  if (ply_list_get_length (client->requests_waiting_for_replies) == 0)
      return;

  node = ply_list_get_first_node (client->requests_waiting_for_replies);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_boot_client_request_t *request;

      request = (ply_boot_client_request_t *) ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (client->requests_waiting_for_replies, node);

      ply_boot_client_cancel_request (client, request);
      ply_list_remove_node (client->requests_waiting_for_replies, node);

      node = next_node;
    }

  if (client->daemon_has_reply_watch != NULL)
    {
      assert (client->loop != NULL);

      ply_event_loop_stop_watching_fd (client->loop, 
                                       client->daemon_has_reply_watch);
      client->daemon_has_reply_watch = NULL;
    }
}

static void
ply_boot_client_cancel_requests (ply_boot_client_t *client)
{
  ply_boot_client_cancel_unsent_requests (client);
  ply_boot_client_cancel_requests_waiting_for_replies (client);
}

void
ply_boot_client_free (ply_boot_client_t *client)
{
  if (client == NULL)
    return;

  ply_boot_client_cancel_requests (client);

  ply_list_free (client->requests_to_send);
  ply_list_free (client->requests_waiting_for_replies);

  free (client);
}

bool
ply_boot_client_connect (ply_boot_client_t *client,
                         ply_boot_client_disconnect_handler_t  disconnect_handler,
                         void                                 *user_data)
{
  assert (client != NULL);
  assert (!client->is_connected);
  assert (client->disconnect_handler == NULL);
  assert (client->disconnect_handler_user_data == NULL);

  client->socket_fd =
      ply_connect_to_unix_socket (PLY_BOOT_PROTOCOL_TRIMMED_ABSTRACT_SOCKET_PATH,
                                  PLY_UNIX_SOCKET_TYPE_TRIMMED_ABSTRACT);

  if (client->socket_fd < 0)
    {
      ply_trace ("could not connect to " PLY_BOOT_PROTOCOL_TRIMMED_ABSTRACT_SOCKET_PATH ": %m");
      ply_trace ("trying old fallback path " PLY_BOOT_PROTOCOL_OLD_ABSTRACT_SOCKET_PATH);

      client->socket_fd =
          ply_connect_to_unix_socket (PLY_BOOT_PROTOCOL_OLD_ABSTRACT_SOCKET_PATH,
                                      PLY_UNIX_SOCKET_TYPE_ABSTRACT);
      if (client->socket_fd < 0)
        {
          ply_trace ("could not connect to " PLY_BOOT_PROTOCOL_OLD_ABSTRACT_SOCKET_PATH ": %m");
          return false;
        }
    }

  client->disconnect_handler = disconnect_handler;
  client->disconnect_handler_user_data = user_data;

  client->is_connected = true;
  return true;
}

static ply_boot_client_request_t *
ply_boot_client_request_new (ply_boot_client_t                  *client,
                             const char                         *request_command,
                             const char                         *request_argument,
                             ply_boot_client_response_handler_t  handler,
                             ply_boot_client_response_handler_t  failed_handler,
                             void                               *user_data)
{
  ply_boot_client_request_t *request;

  assert (client != NULL);
  assert (request_command != NULL);

  request = calloc (1, sizeof (ply_boot_client_request_t));
  request->client = client;
  request->command = strdup (request_command);
  if (request_argument != NULL)
    request->argument = strdup (request_argument);
  request->handler = handler;
  request->failed_handler = failed_handler;
  request->user_data = user_data;

  return request;
}

static void
ply_boot_client_request_free (ply_boot_client_request_t *request)
{
  if (request == NULL)
    return;
  free (request->command);
  if (request->argument != NULL)
    free (request->argument);
  free (request);
}

static void
ply_boot_client_cancel_request (ply_boot_client_t         *client,
                                ply_boot_client_request_t *request)
{
  if (request->failed_handler != NULL)
    request->failed_handler (request->user_data, request->client);

  ply_boot_client_request_free (request);
}

static void
ply_boot_client_process_incoming_replies (ply_boot_client_t *client)
{
  ply_list_node_t *request_node;
  ply_boot_client_request_t *request;
  bool processed_reply;
  uint8_t byte[2] = "";
  uint32_t size;

  assert (client != NULL);

  processed_reply = false;
  if (ply_list_get_length (client->requests_waiting_for_replies) == 0)
    {
      ply_error ("received unexpected response from boot status daemon");
      return;
    }

  request_node = ply_list_get_first_node (client->requests_waiting_for_replies);
  assert (request_node != NULL);

  request = (ply_boot_client_request_t *) ply_list_node_get_data (request_node);
  assert (request != NULL);

  if (!ply_read (client->socket_fd, byte, sizeof (uint8_t)))
    goto out;

  if (memcmp (byte, PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ACK, sizeof (uint8_t)) == 0)
    {
      if (request->handler != NULL)
        request->handler (request->user_data, client);
    }
  else if (memcmp (byte, PLY_BOOT_PROTOCOL_RESPONSE_TYPE_ANSWER, sizeof (uint8_t)) == 0)
    {
      char *answer;

      if (!ply_read_uint32 (client->socket_fd, &size))
        goto out;
      
      answer = malloc ((size+1) * sizeof(char));
      if (size > 0)
        {
          if (!ply_read (client->socket_fd, answer, size))
            {
              free (answer);
              goto out;
            }
        }

      answer[size] = '\0';
      if (request->handler != NULL)
        ((ply_boot_client_answer_handler_t) request->handler) (request->user_data, answer, client);
      free(answer);
    }
  else if (memcmp (byte, PLY_BOOT_PROTOCOL_RESPONSE_TYPE_MULTIPLE_ANSWERS, sizeof (uint8_t)) == 0)
    {
      ply_array_t *array;
      char **answers;
      char *answer;
      char *p;
      char *q;
      uint8_t i;

      array = NULL;
      answers = NULL;

      if (!ply_read_uint32 (client->socket_fd, &size))
        goto out;

      assert (size > 0);

      answer = malloc (size);

      if (!ply_read (client->socket_fd, answer, size))
        {
          free (answer);
          goto out;
        }

      array = ply_array_new (PLY_ARRAY_ELEMENT_TYPE_POINTER);

      p = answer;
      q = p;
      for (i = 0; i < size; i++, q++)
        {
          if (*q == '\0')
            {
              ply_array_add_pointer_element (array, strdup (p));
              p = q + 1;
            }
        }
      free (answer);

      answers = (char **) ply_array_steal_pointer_elements (array);
      ply_array_free (array);

      if (request->handler != NULL)
        ((ply_boot_client_multiple_answers_handler_t) request->handler) (request->user_data, (const char * const *) answers, client);

      ply_free_string_array (answers);
    }
  else if (memcmp (byte, PLY_BOOT_PROTOCOL_RESPONSE_TYPE_NO_ANSWER, sizeof (uint8_t)) == 0)
    {
      if (request->handler != NULL)
        ((ply_boot_client_answer_handler_t) request->handler) (request->user_data, NULL, client);
    }
  else
    goto out;

  processed_reply = true;

out:
  if (!processed_reply)
    {
      if (request->failed_handler != NULL)
        request->failed_handler (request->user_data, client);
    }

  ply_list_remove_node (client->requests_waiting_for_replies, request_node);

  if (ply_list_get_length (client->requests_waiting_for_replies) == 0)
    {
      if (client->daemon_has_reply_watch != NULL)
        {
          assert (client->loop != NULL);
          ply_event_loop_stop_watching_fd (client->loop,
                                           client->daemon_has_reply_watch);
          client->daemon_has_reply_watch = NULL;
        }
    }
}

static char *
ply_boot_client_get_request_string (ply_boot_client_t         *client,
                                    ply_boot_client_request_t *request,
                                    size_t                    *request_size)
{
  char *request_string;

  assert (client != NULL);
  assert (request != NULL);
  assert (request_size != NULL);

  assert (request->command != NULL);

  if (request->argument == NULL)
    {
      request_string = strdup (request->command);
      *request_size = strlen (request_string) + 1;
      return request_string;
    }

  assert (strlen (request->argument) <= UCHAR_MAX);

  request_string = NULL;
  asprintf (&request_string, "%s\002%c%s", request->command, 
            (char) (strlen (request->argument) + 1), request->argument);
  *request_size = strlen (request_string) + 1;

  return request_string;
}

static bool
ply_boot_client_send_request (ply_boot_client_t         *client,
                              ply_boot_client_request_t *request)
{
  char *request_string;
  size_t request_size;

  assert (client != NULL);
  assert (request != NULL);

  request_string = ply_boot_client_get_request_string (client, request,
                                                       &request_size);
  if (!ply_write (client->socket_fd, request_string, request_size))
    {
      free (request_string);
      ply_boot_client_cancel_request (client, request);
      return false;
    }
  free (request_string);

  if (client->daemon_has_reply_watch == NULL)
    {
      assert (ply_list_get_length (client->requests_waiting_for_replies) == 0);
      client->daemon_has_reply_watch = 
          ply_event_loop_watch_fd (client->loop, client->socket_fd,
                                   PLY_EVENT_LOOP_FD_STATUS_HAS_DATA,
                                   (ply_event_handler_t)
                                   ply_boot_client_process_incoming_replies,
                                   NULL, client);
    }
  return true;
}

static void
ply_boot_client_process_pending_requests (ply_boot_client_t *client)
{
  ply_list_node_t *request_node;
  ply_boot_client_request_t *request;

  assert (ply_list_get_length (client->requests_to_send) != 0);
  assert (client->daemon_can_take_request_watch != NULL);

  request_node = ply_list_get_first_node (client->requests_to_send);
  assert (request_node != NULL);

  request = (ply_boot_client_request_t *) ply_list_node_get_data (request_node);
  assert (request != NULL);

  ply_list_remove_node (client->requests_to_send, request_node);

  if (ply_boot_client_send_request (client, request))
    ply_list_append_data (client->requests_waiting_for_replies, request);

  if (ply_list_get_length (client->requests_to_send) == 0)
    {
      if (client->daemon_has_reply_watch != NULL)
        {
          assert (client->loop != NULL);

          ply_event_loop_stop_watching_fd (client->loop,
                                           client->daemon_can_take_request_watch);
          client->daemon_can_take_request_watch = NULL;
        }
    }
}

static void
ply_boot_client_queue_request (ply_boot_client_t                  *client,
                               const char                         *request_command,
                               const char                         *request_argument,
                               ply_boot_client_response_handler_t  handler,
                               ply_boot_client_response_handler_t  failed_handler,
                               void                               *user_data)
{
  assert (client != NULL);
  assert (client->loop != NULL);
  assert (request_command != NULL);
  assert (request_argument == NULL || strlen (request_argument) <= UCHAR_MAX);

  if (client->daemon_can_take_request_watch == NULL &&
      client->socket_fd >= 0)
    {
      assert (ply_list_get_length (client->requests_to_send) == 0);
      client->daemon_can_take_request_watch = 
          ply_event_loop_watch_fd (client->loop, client->socket_fd,
                                   PLY_EVENT_LOOP_FD_STATUS_CAN_TAKE_DATA,
                                   (ply_event_handler_t)
                                   ply_boot_client_process_pending_requests,
                                   NULL, client);
    }

  if (!client->is_connected)
    {
      if (failed_handler != NULL)
        {
          failed_handler (user_data, client);
        }
    }
  else
    {
      ply_boot_client_request_t *request;

      request = ply_boot_client_request_new (client, request_command,
                                             request_argument,
                                             handler, failed_handler, user_data);
      ply_list_append_data (client->requests_to_send, request);
    }
}

void
ply_boot_client_ping_daemon (ply_boot_client_t                  *client,
                             ply_boot_client_response_handler_t  handler,
                             ply_boot_client_response_handler_t  failed_handler,
                             void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PING,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_update_daemon (ply_boot_client_t                  *client,
                               const char                         *status,
                               ply_boot_client_response_handler_t  handler,
                               ply_boot_client_response_handler_t  failed_handler,
                               void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_UPDATE,
                                 status, handler, failed_handler, user_data);
}

void
ply_boot_client_change_mode (ply_boot_client_t                  *client,
                             const char                         *new_mode,
                             ply_boot_client_response_handler_t  handler,
                             ply_boot_client_response_handler_t  failed_handler,
                             void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_CHANGE_MODE,
                                 new_mode, handler, failed_handler, user_data);
}

void
ply_boot_client_system_update (ply_boot_client_t                  *client,
                               const char                         *progress,
                               ply_boot_client_response_handler_t  handler,
                               ply_boot_client_response_handler_t  failed_handler,
                               void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_UPDATE,
                                 progress, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_change_root (ply_boot_client_t                  *client,
                                            const char                         *root_dir,
                                            ply_boot_client_response_handler_t  handler,
                                            ply_boot_client_response_handler_t  failed_handler,
                                            void                               *user_data)
{
  assert (client != NULL);
  assert (root_dir != NULL);

  ply_boot_client_queue_request(client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_NEWROOT,
                                root_dir, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_display_message (ply_boot_client_t                  *client,
                                                const char                         *message,
                                                ply_boot_client_response_handler_t  handler,
                                                ply_boot_client_response_handler_t  failed_handler,
                                                void                               *user_data)
{
  assert (client != NULL);
  assert (message != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_MESSAGE,
                                 message, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_hide_message (ply_boot_client_t                  *client,
                                             const char                         *message,
                                             ply_boot_client_response_handler_t  handler,
                                             ply_boot_client_response_handler_t  failed_handler,
                                             void                               *user_data)
{
  assert (client != NULL);
  assert (message != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_MESSAGE,
                                 message, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_system_is_initialized (ply_boot_client_t                  *client,
                                                   ply_boot_client_response_handler_t  handler,
                                                   ply_boot_client_response_handler_t  failed_handler,
                                                   void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client,
                                 PLY_BOOT_PROTOCOL_REQUEST_TYPE_SYSTEM_INITIALIZED,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_for_password (ply_boot_client_t                  *client,

                                         const char *prompt,
                                         ply_boot_client_answer_handler_t    handler,
                                         ply_boot_client_response_handler_t  failed_handler,
                                         void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PASSWORD,
                                 prompt, (ply_boot_client_response_handler_t)
                                 handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_for_cached_passwords (ply_boot_client_t                  *client,
                                                 ply_boot_client_multiple_answers_handler_t    handler,
                                                 ply_boot_client_response_handler_t  failed_handler,
                                                 void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_CACHED_PASSWORD,
                                 NULL, (ply_boot_client_response_handler_t)
                                 handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_question     (ply_boot_client_t                    *client,
                                         const char                            *prompt,
                                         ply_boot_client_answer_handler_t       handler,
                                         ply_boot_client_response_handler_t     failed_handler,
                                         void                                  *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUESTION,
                                 prompt, (ply_boot_client_response_handler_t)
                                 handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_to_watch_for_keystroke (ply_boot_client_t           *client,
                                         const char                            *keys,
                                         ply_boot_client_answer_handler_t       handler,
                                         ply_boot_client_response_handler_t     failed_handler,
                                         void                                  *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE,
                                 keys, (ply_boot_client_response_handler_t)
                                 handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_to_ignore_keystroke (ply_boot_client_t                  *client,
                                         const char                                *keys,
                                         ply_boot_client_answer_handler_t           handler,
                                         ply_boot_client_response_handler_t         failed_handler,
                                         void                                      *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_KEYSTROKE_REMOVE,
                                 keys, (ply_boot_client_response_handler_t)
                                 handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_show_splash (ply_boot_client_t                  *client,
                                            ply_boot_client_response_handler_t  handler,
                                            ply_boot_client_response_handler_t  failed_handler,
                                            void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_SHOW_SPLASH,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_hide_splash (ply_boot_client_t                  *client,
                                            ply_boot_client_response_handler_t  handler,
                                            ply_boot_client_response_handler_t  failed_handler,
                                            void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HIDE_SPLASH,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_deactivate (ply_boot_client_t                  *client,
                                           ply_boot_client_response_handler_t  handler,
                                           ply_boot_client_response_handler_t  failed_handler,
                                           void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_DEACTIVATE,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_reactivate (ply_boot_client_t                  *client,
                                           ply_boot_client_response_handler_t  handler,
                                           ply_boot_client_response_handler_t  failed_handler,
                                           void                               *user_data)
{
  assert (client != NULL);

  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_REACTIVATE,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_quit (ply_boot_client_t                  *client,
                                     bool                                retain_splash,
                                     ply_boot_client_response_handler_t  handler,
                                     ply_boot_client_response_handler_t  failed_handler,
                                     void                               *user_data)
{
  char arg[2] = "";

  assert (client != NULL);

  arg[0] = (char) (retain_splash != false);
  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_QUIT,
                                 arg, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_progress_pause (ply_boot_client_t                  *client,
                                               ply_boot_client_response_handler_t  handler,
                                               ply_boot_client_response_handler_t  failed_handler,
                                               void                               *user_data)
{
  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_PAUSE,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_to_progress_unpause (ply_boot_client_t                  *client,
                                                 ply_boot_client_response_handler_t  handler,
                                                 ply_boot_client_response_handler_t  failed_handler,
                                                 void                               *user_data)
{
  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_PROGRESS_UNPAUSE,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_ask_daemon_has_active_vt (ply_boot_client_t                  *client,
                                          ply_boot_client_response_handler_t  handler,
                                          ply_boot_client_response_handler_t  failed_handler,
                                          void                               *user_data)
{
  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_HAS_ACTIVE_VT,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_tell_daemon_about_error (ply_boot_client_t                  *client,
                                         ply_boot_client_response_handler_t  handler,
                                         ply_boot_client_response_handler_t  failed_handler,
                                         void                               *user_data)
{
  ply_boot_client_queue_request (client, PLY_BOOT_PROTOCOL_REQUEST_TYPE_ERROR,
                                 NULL, handler, failed_handler, user_data);
}

void
ply_boot_client_flush (ply_boot_client_t *client)
{
  assert (client != NULL);

  while (ply_list_get_length (client->requests_to_send) > 0)
    {
      ply_event_loop_process_pending_events (client->loop);
    }
}

void
ply_boot_client_disconnect (ply_boot_client_t *client)
{
  assert (client != NULL);

  close (client->socket_fd);
  client->socket_fd = -1;
  client->is_connected = false;
}

static void
ply_boot_client_detach_from_event_loop (ply_boot_client_t *client)
{
  assert (client != NULL);
  ply_trace ("detaching from event loop");
  client->loop = NULL;
  client->daemon_can_take_request_watch = NULL;
  client->daemon_has_reply_watch = NULL;
}

static void
ply_boot_client_on_hangup (ply_boot_client_t *client)
{
  assert (client != NULL);
  ply_boot_client_cancel_requests (client);

  if (client->disconnect_handler != NULL)
    client->disconnect_handler (client->disconnect_handler_user_data,
                                client);
}

void
ply_boot_client_attach_to_event_loop (ply_boot_client_t *client,
                                      ply_event_loop_t  *loop)
{
  assert (client != NULL);
  assert (loop != NULL);
  assert (client->loop == NULL);

  client->loop = loop;

  if (client->socket_fd >= 0)
    {
      ply_event_loop_watch_fd (client->loop, client->socket_fd,
                               PLY_EVENT_LOOP_FD_STATUS_NONE,
                               NULL,
                               (ply_event_handler_t) ply_boot_client_on_hangup,
                               client);
    }

  ply_event_loop_watch_for_exit (loop, (ply_event_loop_exit_handler_t) 
                                 ply_boot_client_detach_from_event_loop,
                                 client); 

}

#ifdef PLY_BOOT_CLIENT_ENABLE_TEST

#include <stdio.h>

#include "ply-event-loop.h"
#include "ply-boot-client.h"

static void
on_pinged (ply_event_loop_t *loop)
{
  printf ("PING!\n");
}

static void
on_ping_failed (ply_event_loop_t *loop)
{
  printf ("PING FAILED! %m\n");
  ply_event_loop_exit (loop, 1);
}

static void
on_update (ply_event_loop_t *loop)
{
  printf ("UPDATE!\n");
}

static void
on_update_failed (ply_event_loop_t *loop)
{
  printf ("UPDATE FAILED! %m\n");
  ply_event_loop_exit (loop, 1);
}

static void
on_newroot (ply_event_loop_t *loop)
{
  printf ("NEWROOT!\n");
}

static void
on_system_initialized (ply_event_loop_t *loop)
{
  printf ("SYSTEM INITIALIZED!\n");
}

static void
on_system_initialized_failed (ply_event_loop_t *loop)
{
  printf ("SYSTEM INITIALIZATION REQUEST FAILED!\n");
  ply_event_loop_exit (loop, 1);
}

static void
on_quit (ply_event_loop_t *loop)
{
  printf ("QUIT!\n");
  ply_event_loop_exit (loop, 0);
}

static void
on_quit_failed (ply_event_loop_t *loop)
{
  printf ("QUIT FAILED! %m\n");
  ply_event_loop_exit (loop, 2);
}

static void
on_disconnect (ply_event_loop_t *loop)
{
  printf ("DISCONNECT!\n");
  ply_event_loop_exit (loop, 1);
}

int
main (int    argc,
      char **argv)
{
  ply_event_loop_t *loop;
  ply_boot_client_t *client;
  int exit_code;

  exit_code = 0;

  loop = ply_event_loop_new ();

  client = ply_boot_client_new ();

  if (!ply_boot_client_connect (client, 
                                (ply_boot_client_disconnect_handler_t) on_disconnect,
                                loop))
    {
      perror ("could not start boot client");
      return errno;
    }

  ply_boot_client_attach_to_event_loop (client, loop);
  ply_boot_client_ping_daemon (client, 
                               (ply_boot_client_response_handler_t) on_pinged,
                               (ply_boot_client_response_handler_t) on_ping_failed,
                               loop);

  ply_boot_client_update_daemon (client, 
                                 "loading",
                                 (ply_boot_client_response_handler_t) on_update,
                                 (ply_boot_client_response_handler_t) on_update_failed,
                                 loop);

  ply_boot_client_update_daemon (client, 
                                 "loading more",
                                 (ply_boot_client_response_handler_t) on_update,
                                 (ply_boot_client_response_handler_t) on_update_failed,
                                 loop);

  ply_boot_client_tell_daemon_system_is_initialized (client, 
                                       (ply_boot_client_response_handler_t) 
                                       on_system_initialized,
                                       (ply_boot_client_response_handler_t) 
                                       on_system_initialized_failed,
                                       loop);

  ply_boot_client_tell_daemon_to_quit (client, 
                                       (ply_boot_client_response_handler_t) on_quit,
                                       (ply_boot_client_response_handler_t) on_quit_failed,
                                       loop);

  exit_code = ply_event_loop_run (loop);

  ply_boot_client_free (client);

  return exit_code;
}

#endif /* PLY_BOOT_CLIENT_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
