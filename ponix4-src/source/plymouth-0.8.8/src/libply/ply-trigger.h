/* ply-trigger.h - Calls closure at later time.
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
 * Written By: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_TRIGGER_H
#define PLY_TRIGGER_H

#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>

#include "ply-event-loop.h"

typedef struct _ply_trigger ply_trigger_t;

typedef void (* ply_trigger_handler_t) (void          *user_data,
                                        const void    *trigger_data,
                                        ply_trigger_t *trigger);
#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_trigger_t *ply_trigger_new (ply_trigger_t **free_address);

void ply_trigger_add_handler (ply_trigger_t          *trigger,
                              ply_trigger_handler_t   handler,
                              void                   *user_data);
void ply_trigger_remove_handler (ply_trigger_t         *trigger,
                                 ply_trigger_handler_t  handler,
                                 void                  *user_data);
void ply_trigger_free (ply_trigger_t *trigger);

void ply_trigger_ignore_next_pull (ply_trigger_t *trigger);
void ply_trigger_pull (ply_trigger_t *trigger,
                       const void    *data);
#endif

#endif /* PLY_TRIGGER_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
