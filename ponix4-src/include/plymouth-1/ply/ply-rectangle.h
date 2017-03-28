/* ply-rectangle.h
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
#ifndef PLY_RECTANGLE_H
#define PLY_RECTANGLE_H

#include <stdbool.h>
#include <stdint.h>

#include "ply-utils.h"

typedef struct _ply_rectangle ply_rectangle_t;

struct _ply_rectangle
{
  long x;
  long y;
  unsigned long width;
  unsigned long height;
};

typedef enum
{
  PLY_RECTANGLE_OVERLAP_NONE                       = 0,
  PLY_RECTANGLE_OVERLAP_TOP_EDGE                   = 1 << 0,
  PLY_RECTANGLE_OVERLAP_LEFT_EDGE                  = 1 << 1,
  PLY_RECTANGLE_OVERLAP_RIGHT_EDGE                 = 1 << 2,
  PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE                = 1 << 3,
  PLY_RECTANGLE_OVERLAP_TOP_AND_LEFT_EDGES         =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE,
  PLY_RECTANGLE_OVERLAP_TOP_AND_RIGHT_EDGES        =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_TOP_AND_SIDE_EDGES         =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE  |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_BOTTOM_AND_LEFT_EDGES      =
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE,
  PLY_RECTANGLE_OVERLAP_BOTTOM_AND_RIGHT_EDGES     =
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_BOTTOM_AND_SIDE_EDGES      =
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE   |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_SIDE_EDGES                 =
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE   |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_TOP_AND_BOTTOM_EDGES       =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE    |
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE,
  PLY_RECTANGLE_OVERLAP_TOP_LEFT_AND_BOTTOM_EDGES  =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE    |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE   |
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE,
  PLY_RECTANGLE_OVERLAP_TOP_RIGHT_AND_BOTTOM_EDGES =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE    |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE  |
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE,
  PLY_RECTANGLE_OVERLAP_ALL_EDGES                  =
      PLY_RECTANGLE_OVERLAP_TOP_EDGE    |
      PLY_RECTANGLE_OVERLAP_LEFT_EDGE   |
      PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE |
      PLY_RECTANGLE_OVERLAP_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_NO_EDGES                    = 1 << 4,
  PLY_RECTANGLE_OVERLAP_EXACT_TOP_EDGE,
  PLY_RECTANGLE_OVERLAP_EXACT_LEFT_EDGE,
  PLY_RECTANGLE_OVERLAP_EXACT_RIGHT_EDGE,
  PLY_RECTANGLE_OVERLAP_EXACT_BOTTOM_EDGE,
  
} ply_rectangle_overlap_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
bool ply_rectangle_contains_point (ply_rectangle_t *rectangle,
                                   long             x,
                                   long             y);

bool ply_rectangle_is_empty (ply_rectangle_t *rectangle);

ply_rectangle_overlap_t ply_rectangle_find_overlap (ply_rectangle_t *rectangle1,
                                                    ply_rectangle_t *rectangle2);

void ply_rectangle_intersect (ply_rectangle_t *rectangle1,
                              ply_rectangle_t *rectangle2,
                              ply_rectangle_t *result);
#endif

#endif /* PLY_RECTANGLE_H */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
