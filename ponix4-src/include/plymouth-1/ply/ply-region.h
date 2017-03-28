/* ply-region.h
 *
 * Copyright (C) 2009 Red Hat, Inc.
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
#ifndef PLY_REGION_H
#define PLY_REGION_H

#include <stdbool.h>
#include <stdint.h>

#include "ply-list.h"
#include "ply-rectangle.h"
#include "ply-utils.h"

typedef struct _ply_region ply_region_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_region_t *ply_region_new (void);
void ply_region_free (ply_region_t *region);
void ply_region_add_rectangle (ply_region_t    *region,
                               ply_rectangle_t *rectangle);
void ply_region_clear (ply_region_t *region);
ply_list_t *ply_region_get_rectangle_list (ply_region_t *region);
ply_list_t *ply_region_get_sorted_rectangle_list (ply_region_t *region);

bool ply_region_is_empty (ply_region_t *region);

#endif

#endif /* PLY_REGION_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
