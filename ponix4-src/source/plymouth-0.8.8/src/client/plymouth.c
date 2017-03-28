/* plymouth.c - updates boot status
 *
 * Copyright (C) 2007 Red Hat, Inc
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#include "config.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "ply-boot-client.h"
#include "ply-command-parser.h"
#include "ply-event-loop.h"
#include "ply-logger.h"
#include "ply-utils.h"

#ifndef PLY_MAX_COMMAND_LINE_SIZE
#define PLY_MAX_COMMAND_LINE_SIZE 512
#endif

#define KEY_CTRL_C ('\100' ^'C')

typedef struct
{
  ply_event_loop_t     *loop;
  ply_boot_client_t    *client;
  ply_command_parser_t *command_parser;
  char kernel_command_line[PLY_MAX_COMMAND_LINE_SIZE];
} state_t;

typedef struct
{
  state_t *state;
  char    *command;
  char    *prompt;
  int      number_of_tries_left;
  uint32_t pause : 1;
} password_answer_state_t;

typedef struct
{
  state_t *state;
  char    *command;
  char    *prompt;
  uint32_t pause : 1;
} question_answer_state_t;

typedef struct
{
  state_t *state;
  char    *command;
  char    *keys;
} key_answer_state_t;

static char **
split_string (const char *command,
              const char  delimiter)
{
  const char *p, *q;
  int i, number_of_delimiters;
  char **args;

  number_of_delimiters = 0;
  for (p = command; *p != '\0'; p++)
    {
      if (*p == delimiter &&
          *(p + 1) != delimiter)
      number_of_delimiters++;
    }

  /* there is one more arg that delimiters between args
   * and a trailing NULL arg
   */
  args = calloc (number_of_delimiters + 2, sizeof (char *));
  q = command;
  i = 0;
  for (p = command; *p != '\0'; p++)
    {
      if (*p == delimiter)
        {
          args[i++] = strndup (q, p - q);

          while (*p == delimiter)
              p++;

          q = p;
        }

      assert (*q != delimiter);
      assert (i <= number_of_delimiters);
    }

  args[i++] = strndup (q, p - q);

  return args;
}

static bool
answer_via_command (char           *command,
                    const char     *answer,
                    int            *exit_status)
{
  bool gave_answer;
  pid_t pid;
  int command_input_sender_fd, command_input_receiver_fd;


  ply_trace ("running command '%s'", command);

  /* answer may be NULL which means,
   * "The daemon can't ask the user questions,
   *   do all the prompting from the client"
   */

  gave_answer = false;
  if (answer != NULL &&
    !ply_open_unidirectional_pipe (&command_input_sender_fd,
                                   &command_input_receiver_fd))
    return false;

  pid = fork (); 

  if (pid < 0)
    return false;

  if (pid == 0)
    {
      char **args;
      args = split_string (command, ' ');
      if (answer != NULL)
        {
          close (command_input_sender_fd);
          dup2 (command_input_receiver_fd, STDIN_FILENO);
        }

      execvp (args[0], args); 
      ply_trace ("could not run command: %m");
      _exit (127);
    }

  if (answer != NULL)
    {
      close (command_input_receiver_fd);

      if (write (command_input_sender_fd, answer, strlen (answer)) < 0)
        goto out;
    }

  gave_answer = true;
out:
  if (answer != NULL)
    close (command_input_sender_fd);
  waitpid (pid, exit_status, 0); 

  return gave_answer;
}

static void
on_failure (state_t *state)
{
  ply_event_loop_exit (state->loop, 1);
}

static void
on_success (state_t *state)
{
  ply_event_loop_exit (state->loop, 0);
}

static void
on_password_answer_failure (password_answer_state_t *answer_state,
                            ply_boot_client_t       *client)
{
  ply_trace ("password answer failure");

  /* plymouthd isn't running for some reason.  If there is a command
   * to run, we'll run it anyway, because it might be important for
   * boot up to continue (to decrypt the root partition or whatever)
   */
  if (answer_state->command != NULL)
    {
      int exit_status;
      bool command_started;

      ply_trace ("daemon not available, running command on our own");

      exit_status = 127;
      command_started = false;
      while (answer_state->number_of_tries_left > 0)
        {
          command_started = answer_via_command (answer_state->command, NULL,
                                                &exit_status);

          if (command_started && WIFEXITED (exit_status) &&
              WEXITSTATUS (exit_status) == 0)
            {
              ply_trace ("command was successful");
              break;
            }

          ply_trace ("command failed");
          answer_state->number_of_tries_left--;
        }

      if (command_started && WIFSIGNALED (exit_status))
        {
          ply_trace ("command died with signal %s", strsignal (WTERMSIG (exit_status)));
          raise (WTERMSIG (exit_status));
        }
      else
        {
          ply_event_loop_exit (answer_state->state->loop,
                               WEXITSTATUS (exit_status));
        }
    }
  else
    {
      ply_event_loop_exit (answer_state->state->loop, 1);
    }
}

static void
on_password_answer (password_answer_state_t   *answer_state,
                    const char                *answer,
                    ply_boot_client_t         *client)
{
  int exit_status;

  exit_status = 127;
  if (answer != NULL && answer[0] != KEY_CTRL_C)  /* a CTRL-C answer means the user canceled */
    {
      if (answer_state->command != NULL)
        {
          bool command_started = false;

          command_started = answer_via_command (answer_state->command, answer,
                                                &exit_status);

          if (command_started && (!WIFEXITED (exit_status) ||
              WEXITSTATUS (exit_status) != 0))
            {
              answer_state->number_of_tries_left--;

              if (answer_state->number_of_tries_left > 0)
                {
                  ply_boot_client_ask_daemon_for_password (answer_state->state->client,
                                                           answer_state->prompt,
                                                           (ply_boot_client_answer_handler_t)
                                                           on_password_answer,
                                                           (ply_boot_client_response_handler_t)
                                                           on_password_answer_failure,
                                                           answer_state);
                  return;
                }
            }
        }
      else
        {
          write (STDOUT_FILENO, answer, strlen (answer));
          exit_status = 0;
        }
    }
  else if (answer == NULL)
    {
      on_password_answer_failure (answer_state, answer_state->state->client);
    }

  if (WIFSIGNALED (exit_status))
    raise (WTERMSIG (exit_status));
  
  if (answer_state->pause)
    {
      ply_boot_client_tell_daemon_to_progress_unpause (client,
                                                       (ply_boot_client_response_handler_t)
                                                       (WEXITSTATUS (exit_status) ? on_failure : on_success),
                                                       (ply_boot_client_response_handler_t)
                                                       on_failure,
                                                       answer_state->state);
    }
  else
    ply_event_loop_exit (answer_state->state->loop, WEXITSTATUS (exit_status));
}

static void
on_question_answer_failure (question_answer_state_t *answer_state,
                            ply_boot_client_t       *client)
{
  ply_event_loop_exit (answer_state->state->loop, 1);
}

static void
on_question_answer (question_answer_state_t   *answer_state,
                   const char                 *answer,
                   ply_boot_client_t          *client)
{
  if (answer != NULL)
    {
      if (answer_state->command != NULL)
        {
          answer_via_command (answer_state->command, answer, NULL);
        }
      else
        {
          write (STDOUT_FILENO, answer, strlen (answer));
        }
      if (answer_state->pause)
        ply_boot_client_tell_daemon_to_progress_unpause (client,
                                                         (ply_boot_client_response_handler_t)
                                                         on_success,
                                                         (ply_boot_client_response_handler_t)
                                                         on_failure,
                                                         answer_state->state);
      else
        ply_event_loop_exit (answer_state->state->loop, 0);
    }
  else
    {
      if (answer_state->pause)
        ply_boot_client_tell_daemon_to_progress_unpause (client,
                                                         (ply_boot_client_response_handler_t)
                                                         on_failure,
                                                         (ply_boot_client_response_handler_t)
                                                         on_failure,
                                                         answer_state->state);
      else
        ply_event_loop_exit (answer_state->state->loop, 1);
    }
}

static void
on_key_answer_failure (key_answer_state_t *answer_state,
                       ply_boot_client_t  *client)
{
  ply_event_loop_exit (answer_state->state->loop, 1);
}

static void
on_key_answer (key_answer_state_t *answer_state,
               const char         *answer,
               ply_boot_client_t  *client)
{

  if (answer == NULL)
    {
      ply_event_loop_exit (answer_state->state->loop, 1);
      return;
    }

  if (answer_state->command != NULL)
    {
      answer_via_command (answer_state->command, answer, NULL);
    }
  else
    {
      if (answer != NULL)
        write (STDOUT_FILENO, answer, strlen (answer));
    }

  ply_event_loop_exit (answer_state->state->loop, 0);
}

static void
on_multiple_password_answers (password_answer_state_t     *answer_state,
                              const char * const          *answers)
{
  bool need_to_ask_user;
  int i;
  int exit_status;

  assert (answer_state->command != NULL);

  ply_trace ("on multiple password answers");

  need_to_ask_user = true;

  if (answers != NULL)
    {
      ply_trace ("daemon has a few candidate passwords for us to try");
      for (i = 0; answers[i] != NULL; i++)
        {
          bool command_started;
          exit_status = 127;
          command_started = answer_via_command (answer_state->command, answers[i],
                                                &exit_status);
          if (command_started && WIFEXITED (exit_status) &&
              WEXITSTATUS (exit_status) == 0)
            {
              need_to_ask_user = false;
              break;
            }
        }
    }
  else
    {
      ply_trace ("daemon has no candidate passwords for us to try");
    }

  if (need_to_ask_user)
    {
      ply_boot_client_ask_daemon_for_password (answer_state->state->client,
                                               answer_state->prompt,
                                               (ply_boot_client_answer_handler_t)
                                               on_password_answer,
                                               (ply_boot_client_response_handler_t)
                                               on_password_answer_failure, answer_state);
      return;
    }

  ply_event_loop_exit (answer_state->state->loop, 0);
}

static void
on_disconnect (state_t *state)
{
  bool wait;
  int status = 0;

  wait = false;
  ply_command_parser_get_options (state->command_parser,
                                   "wait", &wait,
                                   NULL
                                  );

  if (! wait) {
      ply_error ("error: unexpectedly disconnected from boot status daemon");
      status = 2;
  }

  ply_trace ("disconnect");
  ply_event_loop_exit (state->loop, status);
}

static void
on_password_request_execute (password_answer_state_t *password_answer_state,
                             ply_boot_client_t       *client)
{
  ply_trace ("executing password request (command %s)",
             password_answer_state->command);

  if (password_answer_state->command != NULL)
    {
      ply_boot_client_ask_daemon_for_cached_passwords (client,
                                                       (ply_boot_client_multiple_answers_handler_t)
                                                       on_multiple_password_answers,
                                                       (ply_boot_client_response_handler_t)
                                                       on_password_answer_failure, password_answer_state);
    }
  else
    {
      ply_boot_client_ask_daemon_for_password (client,
                                               password_answer_state->prompt,
                                               (ply_boot_client_answer_handler_t)
                                               on_password_answer,
                                               (ply_boot_client_response_handler_t)
                                               on_password_answer_failure, password_answer_state);
    }
}

static void
on_password_request (state_t    *state,
                     const char *command)
{
  char *prompt;
  char *program;
  int number_of_tries;
  bool dont_pause;
  password_answer_state_t *password_answer_state;

  prompt = NULL;
  program = NULL;
  number_of_tries = 0;
  dont_pause = false;

  ply_trace ("Password request");
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "command", &program,
                                          "prompt", &prompt,
                                          "number-of-tries", &number_of_tries,
                                          "dont-pause-progress", &dont_pause,
                                          NULL);

  if (number_of_tries <= 0)
    number_of_tries = INT_MAX;

  password_answer_state = calloc (1, sizeof (password_answer_state_t));
  password_answer_state->state = state;
  password_answer_state->command = program;
  password_answer_state->prompt = prompt;
  password_answer_state->number_of_tries_left = number_of_tries;
  password_answer_state->pause = !dont_pause;
  if (password_answer_state->pause)
    {
      ply_boot_client_tell_daemon_to_progress_pause (state->client,
                                                     (ply_boot_client_response_handler_t)
                                                     on_password_request_execute,
                                                     (ply_boot_client_response_handler_t)
                                                     on_password_answer_failure,
                                                     password_answer_state);
    }
  else
    {
      on_password_request_execute (password_answer_state, state->client);
    }
}

static void
on_question_request_execute (question_answer_state_t *question_answer_state,
                             ply_boot_client_t       *client)
{
  ply_boot_client_ask_daemon_question (client,
                                       question_answer_state->prompt,
                                       (ply_boot_client_answer_handler_t)
                                       on_question_answer,
                                       (ply_boot_client_response_handler_t)
                                       on_question_answer_failure,
                                       question_answer_state);
}


static void
on_question_request (state_t    *state,
                     const char *command)
{
  char *prompt;
  char *program;
  bool dont_pause;
  question_answer_state_t *question_answer_state;

  prompt = NULL;
  program = NULL;
  dont_pause = false;
  
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "command", &program,
                                          "prompt", &prompt,
                                          "dont-pause-progress", &dont_pause,
                                          NULL);

  question_answer_state = calloc (1, sizeof (question_answer_state_t));
  question_answer_state->state = state;
  question_answer_state->command = program;
  question_answer_state->prompt = prompt;
  question_answer_state->pause = !dont_pause;
  if (question_answer_state->pause)
    {
      ply_boot_client_tell_daemon_to_progress_pause (state->client,
                                                     (ply_boot_client_response_handler_t)
                                                     on_question_request_execute,
                                                     (ply_boot_client_response_handler_t)
                                                     on_question_answer_failure,
                                                     question_answer_state);
    }
  else
    {
      on_question_request_execute (question_answer_state, state->client);
    }
}

static void
on_display_message_request (state_t    *state,
                            const char *command)
{
  char *text;

  text = NULL;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "text", &text,
                                          NULL);
  if (text != NULL)
    {
      ply_boot_client_tell_daemon_to_display_message (state->client,
                                                      text,
                                                      (ply_boot_client_response_handler_t)
                                                      on_success,
                                                      (ply_boot_client_response_handler_t)
                                                      on_failure, state);
      free (text);
    }
}

static void
on_hide_message_request (state_t    *state,
                         const char *command)
{
  char *text;

  text = NULL;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "text", &text,
                                          NULL);
  if (text != NULL)
    {
      ply_boot_client_tell_daemon_to_hide_message (state->client,
                                                   text,
                                                   (ply_boot_client_response_handler_t)
                                                   on_success,
                                                   (ply_boot_client_response_handler_t)
                                                   on_failure, state);
      free (text);
    }
}

static void
on_keystroke_request (state_t    *state,
                     const char *command)
{
  char *keys;
  char *program;

  keys = NULL;
  program = NULL;
  
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "command", &program,
                                          "keys", &keys,
                                          NULL);
  key_answer_state_t *key_answer_state;
  key_answer_state = calloc (1, sizeof (key_answer_state_t));
  key_answer_state->state = state;
  key_answer_state->keys = keys;
  key_answer_state->command = program;
  ply_boot_client_ask_daemon_to_watch_for_keystroke (state->client,
                                                     keys,
                                                     (ply_boot_client_answer_handler_t)
                                                     on_key_answer,
                                                     (ply_boot_client_response_handler_t)
                                                     on_key_answer_failure,
                                                     key_answer_state);
}

static void
on_keystroke_ignore (state_t    *state,
                     const char *command)
{
  char *keys;

  keys = NULL;
  
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "keys", &keys,
                                          NULL);

  ply_boot_client_ask_daemon_to_ignore_keystroke (state->client,
                                                  keys,
                                                  (ply_boot_client_answer_handler_t)
                                                  on_success,
                                                  (ply_boot_client_response_handler_t)
                                                  on_failure, state);
}

static void
on_progress_pause_request (state_t    *state,
                           const char *command)
{
  ply_boot_client_tell_daemon_to_progress_pause (state->client,
                                                (ply_boot_client_response_handler_t)
                                                on_success,
                                                (ply_boot_client_response_handler_t)
                                                on_failure, state);
}


static void
on_progress_unpause_request (state_t    *state,
                             const char *command)
{
  ply_boot_client_tell_daemon_to_progress_unpause (state->client,
                                                  (ply_boot_client_response_handler_t)
                                                  on_success,
                                                  (ply_boot_client_response_handler_t)
                                                  on_failure, state);
}

static void
on_report_error_request (state_t    *state,
                         const char *command)
{
  ply_boot_client_tell_daemon_about_error (state->client,
                                           (ply_boot_client_response_handler_t)
                                           on_success,
                                           (ply_boot_client_response_handler_t)
                                           on_failure, state);

}

static void
on_deactivate_request (state_t    *state,
                       const char *command)
{
  ply_boot_client_tell_daemon_to_deactivate (state->client,
                                             (ply_boot_client_response_handler_t)
                                             on_success,
                                             (ply_boot_client_response_handler_t)
                                             on_failure, state);
}

static void
on_reactivate_request (state_t    *state,
                       const char *command)
{
  ply_boot_client_tell_daemon_to_reactivate (state->client,
                                             (ply_boot_client_response_handler_t)
                                             on_success,
                                             (ply_boot_client_response_handler_t)
                                             on_failure, state);
}

static void
on_quit_request (state_t    *state,
                 const char *command)
{
  bool should_retain_splash;

  should_retain_splash = false;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "retain-splash", &should_retain_splash,
                                          NULL);

  ply_boot_client_tell_daemon_to_quit (state->client,
                                       should_retain_splash,
                                       (ply_boot_client_response_handler_t)
                                       on_success,
                                       (ply_boot_client_response_handler_t)
                                       on_failure, state);
}

static bool
get_kernel_command_line (state_t *state)
{
  int fd;

  ply_trace ("opening /proc/cmdline");
  fd = open ("/proc/cmdline", O_RDONLY);

  if (fd < 0)
    {
      ply_trace ("couldn't open it: %m");
      return false;
    }

  ply_trace ("reading kernel command line");
  if (read (fd, state->kernel_command_line, sizeof (state->kernel_command_line)) < 0)
    {
      ply_trace ("couldn't read it: %m");
      close (fd);
      return false;
    }

  ply_trace ("Kernel command line is: '%s'", state->kernel_command_line);
  close (fd);
  return true;
}

static void
on_update_root_fs_request (state_t    *state,
                           const char *command)
{

  char *root_dir;
  bool is_read_write;

  root_dir = NULL;
  is_read_write = false;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "new-root-dir", &root_dir,
                                          "read-write", &is_read_write,
                                          NULL);

  if (root_dir != NULL)
    {
      ply_boot_client_tell_daemon_to_change_root (state->client, root_dir,
                                                  (ply_boot_client_response_handler_t)
                                                  on_success,
                                                  (ply_boot_client_response_handler_t)
                                                  on_failure, state);

    }

  if (is_read_write)
    {
      ply_boot_client_tell_daemon_system_is_initialized (state->client,
                                                         (ply_boot_client_response_handler_t)
                                                         on_success,
                                                         (ply_boot_client_response_handler_t)
                                                         on_failure, state);
    }
}

static void
on_show_splash_request (state_t    *state,
                        const char *command)
{
    ply_boot_client_tell_daemon_to_show_splash (state->client,
                                               (ply_boot_client_response_handler_t)
                                               on_success,
                                               (ply_boot_client_response_handler_t)
                                               on_failure, state);
}

static void
on_hide_splash_request (state_t    *state,
                        const char *command)
{
    ply_boot_client_tell_daemon_to_hide_splash (state->client,
                                               (ply_boot_client_response_handler_t)
                                               on_success,
                                               (ply_boot_client_response_handler_t)
                                               on_failure, state);
}

static void
on_update_request (state_t    *state,
                   const char *command)
{
  char *status;

  status = NULL;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "status", &status,
                                          NULL);

  if (status != NULL)
    {
      ply_boot_client_update_daemon (state->client, status,
                                     (ply_boot_client_response_handler_t)
                                     on_success,
                                     (ply_boot_client_response_handler_t)
                                     on_failure, state);

    }
}

static void
on_change_mode_request (state_t    *state,
                        const char *command)
{
  bool boot_up;
  bool shutdown;
  bool updates;

  boot_up = false;
  shutdown = false;
  updates = false;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "boot-up", &boot_up,
                                          "shutdown", &shutdown,
                                          "updates", &updates,
                                          NULL);

  if (boot_up)
    {
      ply_boot_client_change_mode (state->client, "boot-up",
                                   (ply_boot_client_response_handler_t)
                                   on_success,
                                   (ply_boot_client_response_handler_t)
                                   on_failure, state);

    }
  else if (shutdown)
    {
      ply_boot_client_change_mode (state->client, "shutdown",
                                   (ply_boot_client_response_handler_t)
                                   on_success,
                                   (ply_boot_client_response_handler_t)
                                   on_failure, state);

    }
  else if (updates)
    {
      ply_boot_client_change_mode (state->client, "updates",
                                   (ply_boot_client_response_handler_t)
                                   on_success,
                                   (ply_boot_client_response_handler_t)
                                   on_failure, state);

    }
}

static void
on_system_update_request (state_t    *state,
                          const char *command)
{
  int progress;

  progress = 0;
  ply_command_parser_get_command_options (state->command_parser,
                                          command,
                                          "progress", &progress,
                                          NULL);

  if (progress >= 0 && progress <= 100)
    {
      char *progress_string = NULL;

      asprintf (&progress_string, "%d", progress);

      ply_boot_client_system_update (state->client,
                                     progress_string,
                                     (ply_boot_client_response_handler_t)
                                     on_success,
                                     (ply_boot_client_response_handler_t)
                                     on_failure, state);
      free (progress_string);
    }
  else
    {
      ply_error ("couldn't set invalid percentage: %i", progress);
      ply_event_loop_exit (state->loop, 1);
    }
}

int
main (int    argc,
      char **argv)
{
  state_t state = { 0 };
  bool should_help, should_quit, should_ping, should_check_for_active_vt, should_sysinit, should_ask_for_password, should_show_splash, should_hide_splash, should_wait, should_be_verbose, report_error, should_get_plugin_path;
  bool is_connected;
  char *status, *chroot_dir, *ignore_keystroke;
  int exit_code;

  exit_code = 0;

  signal (SIGPIPE, SIG_IGN);

  state.loop = ply_event_loop_new ();
  state.client = ply_boot_client_new ();
  state.command_parser = ply_command_parser_new ("plymouth", "Splash control client");

  ply_command_parser_add_options (state.command_parser,
                                  "help", "This help message", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "debug", "Enable verbose debug logging", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "get-splash-plugin-path", "Get directory where splash plugins are installed", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "newroot", "Tell boot daemon that new root filesystem is mounted", PLY_COMMAND_OPTION_TYPE_STRING,
                                  "quit", "Tell boot daemon to quit", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "ping", "Check of boot daemon is running", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "has-active-vt", "Check if boot daemon has an active vt", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "sysinit", "Tell boot daemon root filesystem is mounted read-write", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "show-splash", "Show splash screen", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "hide-splash", "Hide splash screen", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "ask-for-password", "Ask user for password", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "ignore-keystroke", "Remove sensitivity to a keystroke", PLY_COMMAND_OPTION_TYPE_STRING,
                                  "update", "Tell boot daemon an update about boot progress", PLY_COMMAND_OPTION_TYPE_STRING,
                                  "details", "Tell boot daemon there were errors during boot", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "wait", "Wait for boot daemon to quit", PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "change-mode", "Change the operation mode",
                                  (ply_command_handler_t)
                                  on_change_mode_request, &state,
                                  "boot-up", "Starting the system up",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "shutdown", "Shutting the system down",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  "updates", "Applying updates",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "system-update", "Tell the daemon about updates progress",
                                  (ply_command_handler_t)
                                  on_system_update_request, &state,
                                  "progress", "The percentage progress of the updates",
                                  PLY_COMMAND_OPTION_TYPE_INTEGER,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "update", "Tell daemon about boot status changes",
                                  (ply_command_handler_t)
                                  on_update_request, &state,
                                  "status", "Tell daemon the current boot status",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "update-root-fs", "Tell daemon about root filesystem changes",
                                  (ply_command_handler_t)
                                  on_update_root_fs_request, &state,
                                  "new-root-dir", "Root filesystem is about to change",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "read-write", "Root filesystem is no longer read-only",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "show-splash", "Tell daemon to show splash screen",
                                  (ply_command_handler_t)
                                  on_show_splash_request, &state,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "hide-splash", "Tell daemon to hide splash screen",
                                  (ply_command_handler_t)
                                  on_hide_splash_request, &state,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "ask-for-password", "Ask user for password",
                                  (ply_command_handler_t)
                                  on_password_request, &state,
                                  "command", "Command to send password to via standard input",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "prompt", "Message to display when asking for password",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "number-of-tries", "Number of times to ask before giving up (requires --command)",
                                  PLY_COMMAND_OPTION_TYPE_INTEGER,
                                  "dont-pause-progress", "Don't pause boot progress bar while asking",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "ask-question", "Ask user a question",
                                  (ply_command_handler_t)
                                  on_question_request, &state,
                                  "command", "Command to send the answer to via standard input",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "prompt", "Message to display when asking the question",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "dont-pause-progress", "Don't pause boot progress bar while asking",
                                  PLY_COMMAND_OPTION_TYPE_FLAG,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "display-message", "Display a message",
                                  (ply_command_handler_t)
                                  on_display_message_request, &state,
                                  "text", "The message text",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);
  ply_command_parser_add_command_alias (state.command_parser,
                                        "display-message",
                                        "message");
  ply_command_parser_add_command (state.command_parser,
                                  "hide-message", "Hide a message",
                                  (ply_command_handler_t)
                                  on_hide_message_request, &state,
                                  "text", "The message text",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "watch-keystroke", "Become sensitive to a keystroke",
                                  (ply_command_handler_t)
                                  on_keystroke_request, &state,
                                  "command", "Command to send keystroke to via standard input",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  "keys", "Keys to become sensitive to",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "ignore-keystroke", "Remove sensitivity to a keystroke",
                                  (ply_command_handler_t)
                                  on_keystroke_ignore, &state,
                                  "keys", "Keys to remove sensitivity to",
                                  PLY_COMMAND_OPTION_TYPE_STRING,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "pause-progress", "Pause boot progress bar",
                                  (ply_command_handler_t)
                                  on_progress_pause_request, &state,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "unpause-progress", "Unpause boot progress bar",
                                  (ply_command_handler_t)
                                  on_progress_unpause_request, &state,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "report-error", "Tell boot daemon there were errors during boot",
                                  (ply_command_handler_t)
                                  on_report_error_request, &state,
                                  NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "deactivate", "Tell boot daemon to deactivate",
                                  (ply_command_handler_t)
                                  on_deactivate_request, &state, NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "reactivate", "Tell boot daemon to reactivate",
                                  (ply_command_handler_t)
                                  on_reactivate_request, &state, NULL);

  ply_command_parser_add_command (state.command_parser,
                                  "quit", "Tell boot daemon to quit",
                                  (ply_command_handler_t)
                                  on_quit_request, &state,
                                  "retain-splash", "Don't explicitly hide boot splash on exit",
                                  PLY_COMMAND_OPTION_TYPE_FLAG, NULL);

  if (!ply_command_parser_parse_arguments (state.command_parser, state.loop, argv, argc))
    {
      char *help_string;

      help_string = ply_command_parser_get_help_string (state.command_parser);

      ply_error ("%s", help_string);

      free (help_string);
      return 1;
    }

  ply_command_parser_get_options (state.command_parser,
                                  "help", &should_help,
                                  "debug", &should_be_verbose,
                                  "get-splash-plugin-path", &should_get_plugin_path,
                                  "newroot", &chroot_dir,
                                  "quit", &should_quit,
                                  "ping", &should_ping,
                                  "has-active-vt", &should_check_for_active_vt,
                                  "sysinit", &should_sysinit,
                                  "show-splash", &should_show_splash,
                                  "hide-splash", &should_hide_splash,
                                  "ask-for-password", &should_ask_for_password,
                                  "ignore-keystroke", &ignore_keystroke,
                                  "update", &status,
                                  "wait", &should_wait,
                                  "details", &report_error,
                                  NULL);

  if (should_help || argc < 2)
    {
      char *help_string;

      help_string = ply_command_parser_get_help_string (state.command_parser);

      if (argc < 2)
        fprintf (stderr, "%s", help_string);
      else
        printf ("%s", help_string);

      free (help_string);
      return 0;
    }

  if (get_kernel_command_line (&state))
    {
      if (strstr (state.kernel_command_line, "plymouth:debug") != NULL
          && !ply_is_tracing ())
        ply_toggle_tracing ();
    }

  if (should_be_verbose && !ply_is_tracing ())
    ply_toggle_tracing ();

  if (should_get_plugin_path)
    {
      printf ("%s\n", PLYMOUTH_PLUGIN_PATH);
      return 0;
    }

  is_connected = ply_boot_client_connect (state.client,
                                          (ply_boot_client_disconnect_handler_t)
                                          on_disconnect, &state);
  if (!is_connected)
    {
      ply_trace ("daemon not running");

      if (should_ping)
        {
          ply_trace ("ping failed");
          return 1;
        }
      if (should_check_for_active_vt)
        {
          ply_trace ("has active vt? failed");
          return 1;
        }
      if (should_wait)
        {
          ply_trace("no need to wait");
          return 0;
        }
    }

  ply_boot_client_attach_to_event_loop (state.client, state.loop);

  if (should_show_splash)
    ply_boot_client_tell_daemon_to_show_splash (state.client,
                                               (ply_boot_client_response_handler_t)
                                               on_success,
                                               (ply_boot_client_response_handler_t)
                                               on_failure, &state);
  else if (should_hide_splash)
    ply_boot_client_tell_daemon_to_hide_splash (state.client,
                                               (ply_boot_client_response_handler_t)
                                               on_success,
                                               (ply_boot_client_response_handler_t)
                                               on_failure, &state);
  else if (should_quit)
    ply_boot_client_tell_daemon_to_quit (state.client,
                                         false,
                                         (ply_boot_client_response_handler_t)
                                         on_success,
                                         (ply_boot_client_response_handler_t)
                                         on_failure, &state);
  else if (should_ping)
    ply_boot_client_ping_daemon (state.client,
                                 (ply_boot_client_response_handler_t)
                                 on_success, 
                                 (ply_boot_client_response_handler_t)
                                 on_failure, &state);
  else if (should_check_for_active_vt)
    ply_boot_client_ask_daemon_has_active_vt (state.client,
                                              (ply_boot_client_response_handler_t)
                                              on_success,
                                              (ply_boot_client_response_handler_t)
                                              on_failure, &state);
  else if (status != NULL)
    ply_boot_client_update_daemon (state.client, status,
                                   (ply_boot_client_response_handler_t)
                                   on_success, 
                                   (ply_boot_client_response_handler_t)
                                   on_failure, &state);
  else if (should_ask_for_password)
    {
      password_answer_state_t answer_state = { 0 };

      answer_state.state = &state;
      answer_state.number_of_tries_left = 1;
      ply_boot_client_ask_daemon_for_password (state.client,
                                               NULL,
                                               (ply_boot_client_answer_handler_t)
                                               on_password_answer,
                                               (ply_boot_client_response_handler_t)
                                               on_password_answer_failure, &answer_state);
    }
  else if (ignore_keystroke)
    {
      ply_boot_client_ask_daemon_to_ignore_keystroke (state.client,
                                           ignore_keystroke,
                                           (ply_boot_client_answer_handler_t)
                                           on_success,
                                           (ply_boot_client_response_handler_t)
                                           on_failure, &state);
    }
  else if (should_sysinit)
    ply_boot_client_tell_daemon_system_is_initialized (state.client,
                                   (ply_boot_client_response_handler_t)
                                   on_success, 
                                   (ply_boot_client_response_handler_t)
                                   on_failure, &state);
  else if (chroot_dir)
    ply_boot_client_tell_daemon_to_change_root (state.client, chroot_dir,
                                   (ply_boot_client_response_handler_t)
                                   on_success,
                                   (ply_boot_client_response_handler_t)
                                   on_failure, &state);

  else if (should_wait)
    {} // Do nothing
  else if (report_error)
    ply_boot_client_tell_daemon_about_error (state.client,
                                             (ply_boot_client_response_handler_t)
                                             on_success,
                                             (ply_boot_client_response_handler_t)
                                             on_failure, &state);

  exit_code = ply_event_loop_run (state.loop);

  ply_boot_client_free (state.client);

  ply_event_loop_free (state.loop);

  return exit_code;
}
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
