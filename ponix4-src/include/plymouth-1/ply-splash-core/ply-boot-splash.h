/* ply-boot-splash.h - APIs for putting up a splash screen
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
#ifndef PLY_BOOT_SPLASH_H
#define PLY_BOOT_SPLASH_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"
#include "ply-buffer.h"
#include "ply-terminal.h"
#include "ply-keyboard.h"
#include "ply-pixel-display.h"
#include "ply-text-display.h"
#include "ply-progress.h"

#include "ply-boot-splash-plugin.h"

typedef struct _ply_boot_splash ply_boot_splash_t;

typedef void (* ply_boot_splash_on_idle_handler_t) (void *user_data);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_boot_splash_t *ply_boot_splash_new (const char   *  theme_path,
                                        const char   *  plugin_dir,
                                        ply_buffer_t *  boot_buffer);

bool ply_boot_splash_load (ply_boot_splash_t *splash);
bool ply_boot_splash_load_built_in (ply_boot_splash_t *splash);
void ply_boot_splash_unload (ply_boot_splash_t *splash);
void ply_boot_splash_set_keyboard (ply_boot_splash_t *splash,
                                   ply_keyboard_t    *keyboard);
void ply_boot_splash_unset_keyboard (ply_boot_splash_t *splash);
void ply_boot_splash_add_pixel_display (ply_boot_splash_t   *splash,
                                        ply_pixel_display_t *display);
void ply_boot_splash_remove_pixel_display (ply_boot_splash_t   *splash,
                                           ply_pixel_display_t *display);
void ply_boot_splash_add_text_display (ply_boot_splash_t   *splash,
                                        ply_text_display_t *display);
void ply_boot_splash_remove_text_display (ply_boot_splash_t   *splash,
                                           ply_text_display_t *display);
void ply_boot_splash_free (ply_boot_splash_t *splash);
bool ply_boot_splash_show (ply_boot_splash_t *splash,
                           ply_boot_splash_mode_t mode);
bool ply_boot_splash_system_update (ply_boot_splash_t *splash,
                                    int                progress);
void ply_boot_splash_update_status (ply_boot_splash_t *splash,
                                    const char        *status);
void ply_boot_splash_update_output (ply_boot_splash_t *splash,
                                    const char        *output,
                                    size_t             size);
void ply_boot_splash_root_mounted (ply_boot_splash_t *splash);
void ply_boot_splash_display_message (ply_boot_splash_t *splash,
                                      const char        *message);
void ply_boot_splash_hide_message (ply_boot_splash_t *splash,
                                   const char        *message);
void ply_boot_splash_hide (ply_boot_splash_t *splash);
void ply_boot_splash_display_normal  (ply_boot_splash_t *splash);
void ply_boot_splash_display_password (ply_boot_splash_t *splash,
                                       const char        *prompt,
                                       int                bullets);
void ply_boot_splash_display_question (ply_boot_splash_t *splash,
                                       const char        *prompt,
                                       const char        *entry_text);
void ply_boot_splash_attach_to_event_loop (ply_boot_splash_t *splash,
                                           ply_event_loop_t  *loop);
void ply_boot_splash_attach_progress (ply_boot_splash_t *splash,
                                      ply_progress_t    *progress);
void ply_boot_splash_become_idle (ply_boot_splash_t                 *splash,
                                  ply_boot_splash_on_idle_handler_t  idle_handler,
                                  void                              *user_data);


#endif

#endif /* PLY_BOOT_SPLASH_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
