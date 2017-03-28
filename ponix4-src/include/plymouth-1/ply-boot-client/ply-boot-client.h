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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_BOOT_CLIENT_H
#define PLY_BOOT_CLIENT_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-boot-protocol.h"
#include "ply-event-loop.h"

typedef struct _ply_boot_client ply_boot_client_t;
typedef void (* ply_boot_client_response_handler_t) (void              *user_data,
                                                     ply_boot_client_t *client);
typedef void (* ply_boot_client_answer_handler_t) (void              *user_data,
                                                   const char        *answer,
                                                   ply_boot_client_t *client);

typedef void (* ply_boot_client_multiple_answers_handler_t) (void    *user_data,
                                                             const char * const *answers,
                                                             ply_boot_client_t *client);
typedef void (* ply_boot_client_disconnect_handler_t) (void              *user_data,
                                                       ply_boot_client_t *client);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_boot_client_t *ply_boot_client_new (void);

void ply_boot_client_free (ply_boot_client_t *client);
bool ply_boot_client_connect (ply_boot_client_t                    *client,
                              ply_boot_client_disconnect_handler_t  disconnect_handler,
                              void                                 *user_data);
void ply_boot_client_ping_daemon (ply_boot_client_t                  *client,
                                  ply_boot_client_response_handler_t  handler,
                                  ply_boot_client_response_handler_t  failed_handler,
                                  void                               *user_data);
void ply_boot_client_update_daemon (ply_boot_client_t                  *client,
                                    const char                         *new_status,
                                    ply_boot_client_response_handler_t  handler,
                                    ply_boot_client_response_handler_t  failed_handler,
                                    void                               *user_data);
void ply_boot_client_change_mode (ply_boot_client_t                  *client,
                                  const char                         *new_mode,
                                  ply_boot_client_response_handler_t  handler,
                                  ply_boot_client_response_handler_t  failed_handler,
                                  void                               *user_data);
void ply_boot_client_system_update (ply_boot_client_t                  *client,
                                    const char                         *progress,
                                    ply_boot_client_response_handler_t  handler,
                                    ply_boot_client_response_handler_t  failed_handler,
                                    void                               *user_data);
void ply_boot_client_tell_daemon_to_change_root (ply_boot_client_t                  *client,
                                                 const char                         *chroot_dir,
                                                 ply_boot_client_response_handler_t  handler,
                                                 ply_boot_client_response_handler_t  failed_handler,
                                                 void                               *user_data);
void ply_boot_client_tell_daemon_to_display_message (ply_boot_client_t                  *client,
                                                     const char                         *message,
                                                     ply_boot_client_response_handler_t  handler,
                                                     ply_boot_client_response_handler_t  failed_handler,
                                                     void                               *user_data);
void ply_boot_client_tell_daemon_to_hide_message (ply_boot_client_t                  *client,
                                                  const char                         *message,
                                                  ply_boot_client_response_handler_t  handler,
                                                  ply_boot_client_response_handler_t  failed_handler,
                                                  void                               *user_data);
void ply_boot_client_ask_daemon_for_password (ply_boot_client_t                  *client,
                                              const char                         *prompt,
                                              ply_boot_client_answer_handler_t    handler,
                                              ply_boot_client_response_handler_t  failed_handler,
                                              void                               *user_data);
void ply_boot_client_ask_daemon_for_cached_passwords (ply_boot_client_t                  *client,
                                                      ply_boot_client_multiple_answers_handler_t    handler,
                                                      ply_boot_client_response_handler_t  failed_handler,
                                                      void                               *user_data);
void ply_boot_client_ask_daemon_question     (ply_boot_client_t                    *client,
                                              const char                           *prompt,
                                              ply_boot_client_answer_handler_t      handler,
                                              ply_boot_client_response_handler_t    failed_handler,
                                              void                                 *user_data);
void ply_boot_client_ask_daemon_to_watch_for_keystroke (ply_boot_client_t          *client,
                                         const char                                *keys,
                                         ply_boot_client_answer_handler_t           handler,
                                         ply_boot_client_response_handler_t         failed_handler,
                                         void                                      *user_data);
void ply_boot_client_ask_daemon_to_ignore_keystroke (ply_boot_client_t             *client,
                                         const char                                *keys,
                                         ply_boot_client_answer_handler_t           handler,
                                         ply_boot_client_response_handler_t         failed_handler,
                                         void                                      *user_data);
void ply_boot_client_tell_daemon_system_is_initialized (ply_boot_client_t                  *client,
                                                        ply_boot_client_response_handler_t  handler,
                                                        ply_boot_client_response_handler_t  failed_handler,
                                                        void                               *user_data);
void ply_boot_client_tell_daemon_to_show_splash (ply_boot_client_t                  *client,
                                                 ply_boot_client_response_handler_t  handler,
                                                 ply_boot_client_response_handler_t  failed_handler,
                                                 void                               *user_data);
void ply_boot_client_tell_daemon_to_hide_splash (ply_boot_client_t                  *client,
                                                 ply_boot_client_response_handler_t  handler,
                                                 ply_boot_client_response_handler_t  failed_handler,
                                                 void                               *user_data);
void ply_boot_client_tell_daemon_to_deactivate (ply_boot_client_t                  *client,
                                                ply_boot_client_response_handler_t  handler,
                                                ply_boot_client_response_handler_t  failed_handler,
                                                void                               *user_data);
void ply_boot_client_tell_daemon_to_reactivate (ply_boot_client_t                  *client,
                                                ply_boot_client_response_handler_t  handler,
                                                ply_boot_client_response_handler_t  failed_handler,
                                                void                               *user_data);
void ply_boot_client_tell_daemon_to_quit (ply_boot_client_t                  *client,
                                          bool                                retain_splash,
                                          ply_boot_client_response_handler_t  handler,
                                          ply_boot_client_response_handler_t  failed_handler,
                                          void                               *user_data);
void ply_boot_client_tell_daemon_to_progress_pause (ply_boot_client_t                  *client,
                                                    ply_boot_client_response_handler_t  handler,
                                                    ply_boot_client_response_handler_t  failed_handler,
                                                    void                               *user_data);
void ply_boot_client_tell_daemon_to_progress_unpause (ply_boot_client_t                  *client,
                                                      ply_boot_client_response_handler_t  handler,
                                                      ply_boot_client_response_handler_t  failed_handler,
                                                      void                               *user_data);
void ply_boot_client_ask_daemon_has_active_vt (ply_boot_client_t                  *client,
                                               ply_boot_client_response_handler_t  handler,
                                               ply_boot_client_response_handler_t  failed_handler,
                                               void                               *user_data);
void ply_boot_client_flush (ply_boot_client_t *client);
void ply_boot_client_disconnect (ply_boot_client_t *client);
void ply_boot_client_attach_to_event_loop (ply_boot_client_t *client,
                                           ply_event_loop_t  *loop);
void ply_boot_client_tell_daemon_about_error (ply_boot_client_t                  *client,
                                              ply_boot_client_response_handler_t  handler,
                                              ply_boot_client_response_handler_t  failed_handler,
                                              void                               *user_data);

#endif

#endif /* PLY_BOOT_CLIENT_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
