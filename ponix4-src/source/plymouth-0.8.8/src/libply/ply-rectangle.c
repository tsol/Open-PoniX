/* ply-rectangle.c
 *
 * Copyright (C) 2009 Red Hat, Inc.
 *
 * Based in part on some work by:
 *  Copyright (C) 2009 Charlie Brej <cbrej@cs.man.ac.uk>
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
 * Written by: Charlie Brej <cbrej@cs.man.ac.uk>
 *             Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-list.h"
#include "ply-rectangle.h"

bool
ply_rectangle_contains_point (ply_rectangle_t *rectangle,
                              long             x,
                              long             y)
{
  long top_edge;
  long left_edge;
  long right_edge;
  long bottom_edge;

  top_edge = rectangle->y;
  left_edge = rectangle->x;
  right_edge = rectangle->x + rectangle->width - 1;
  bottom_edge = rectangle->y + rectangle->height - 1;

  if (x < left_edge)
    return false;

  if (y < top_edge)
    return false;

  if (x > right_edge)
    return false;

  if (y > bottom_edge)
    return false;

  return true;
}

bool
ply_rectangle_is_empty (ply_rectangle_t *rectangle)
{
  return rectangle->width == 0 || rectangle->height == 0;
}

ply_rectangle_overlap_t
ply_rectangle_find_overlap (ply_rectangle_t *rectangle1,
                            ply_rectangle_t *rectangle2)
{
  enum {H_COLLISION_NONE, H_COLLISION_LEFT, H_COLLISION_RIGHT, H_COLLISION_BOTH, H_COLLISION_CONTAINED, H_COLLISION_EXACT}
        h_collision = H_COLLISION_NONE;
  enum {V_COLLISION_NONE, V_COLLISION_TOP, V_COLLISION_BOTTOM, V_COLLISION_BOTH, V_COLLISION_CONTAINED, V_COLLISION_EXACT}
        v_collision = V_COLLISION_NONE;

  if (rectangle2->x >= rectangle1->x && (rectangle2->x + (int)rectangle2->width) <= (rectangle1->x + (int)rectangle1->width))
    {
      if (rectangle2->x == rectangle1->x && rectangle2->width == rectangle1->width)
        h_collision = H_COLLISION_EXACT;
      else
        h_collision = H_COLLISION_CONTAINED;
    }
  else
    {                                       /* Remember: x+width points to the first pixel outside the rectangle*/
      if (rectangle2->x < rectangle1->x &&
          (rectangle2->x + (int)rectangle2->width) > rectangle1->x)
        h_collision = H_COLLISION_LEFT;         /* rectangle2 colllided with the left edge of rectangle1 */

      if (rectangle2->x < (rectangle1->x + (int)rectangle1->width) &&
          (rectangle2->x + (int)rectangle2->width) >= (rectangle1->x + (int)rectangle1->width))
        {                                       /* rectangle2 colllided with the right edge of rectangle1 */
          if (h_collision == H_COLLISION_LEFT)
            h_collision = H_COLLISION_BOTH;
          else
            h_collision = H_COLLISION_RIGHT;
        }
    }

  if (h_collision != H_COLLISION_NONE)
    {
      if (rectangle2->y >= rectangle1->y && (rectangle2->y + (int)rectangle2->height) <= (rectangle1->y + (int)rectangle1->height))
        {
          if (rectangle2->y == rectangle1->y && rectangle2->height == rectangle1->height)
            v_collision = V_COLLISION_EXACT;
          else
            v_collision = V_COLLISION_CONTAINED;
        }
      else
        {
          if (rectangle2->y < rectangle1->y &&
              (rectangle2->y + (int)rectangle2->height) > rectangle1->y)
            v_collision = V_COLLISION_TOP;
          if (rectangle2->y < (rectangle1->y + (int)rectangle1->height) &&
              (rectangle2->y + (int)rectangle2->height) >= (rectangle1->y + (int)rectangle1->height))
            {
              if (v_collision == V_COLLISION_TOP)
                v_collision = V_COLLISION_BOTH;
              else
                v_collision = V_COLLISION_BOTTOM;
            }
        }
    }

  switch (h_collision)
    {
      case H_COLLISION_NONE:
        return PLY_RECTANGLE_OVERLAP_NONE;
      case H_COLLISION_LEFT:
        switch (v_collision)
          {
            case V_COLLISION_NONE:
                return PLY_RECTANGLE_OVERLAP_NONE;
            case V_COLLISION_TOP:
                return PLY_RECTANGLE_OVERLAP_TOP_AND_LEFT_EDGES;
            case V_COLLISION_BOTTOM:
                return PLY_RECTANGLE_OVERLAP_BOTTOM_AND_LEFT_EDGES;
            case V_COLLISION_BOTH:
                return PLY_RECTANGLE_OVERLAP_TOP_LEFT_AND_BOTTOM_EDGES;
            case V_COLLISION_CONTAINED:
                return PLY_RECTANGLE_OVERLAP_LEFT_EDGE;
            case V_COLLISION_EXACT:
                return PLY_RECTANGLE_OVERLAP_EXACT_LEFT_EDGE;
          }
      case H_COLLISION_RIGHT:
        switch (v_collision)
          {
            case V_COLLISION_NONE:
                return PLY_RECTANGLE_OVERLAP_NONE;
            case V_COLLISION_TOP:
                return PLY_RECTANGLE_OVERLAP_TOP_AND_RIGHT_EDGES;
            case V_COLLISION_BOTTOM:
                return PLY_RECTANGLE_OVERLAP_BOTTOM_AND_RIGHT_EDGES;
            case V_COLLISION_BOTH:
                return PLY_RECTANGLE_OVERLAP_TOP_RIGHT_AND_BOTTOM_EDGES;
            case V_COLLISION_CONTAINED:
                return PLY_RECTANGLE_OVERLAP_RIGHT_EDGE;
            case V_COLLISION_EXACT:
                return PLY_RECTANGLE_OVERLAP_EXACT_RIGHT_EDGE;
          }
      case H_COLLISION_BOTH:
        switch (v_collision)
          {
            case V_COLLISION_NONE:
                return PLY_RECTANGLE_OVERLAP_NONE;
            case V_COLLISION_TOP:
                return PLY_RECTANGLE_OVERLAP_TOP_AND_SIDE_EDGES;
            case V_COLLISION_BOTTOM:
                return PLY_RECTANGLE_OVERLAP_BOTTOM_AND_SIDE_EDGES;
            case V_COLLISION_BOTH:
                return PLY_RECTANGLE_OVERLAP_ALL_EDGES;
            case V_COLLISION_CONTAINED:
                return PLY_RECTANGLE_OVERLAP_SIDE_EDGES;
            case V_COLLISION_EXACT:
                return PLY_RECTANGLE_OVERLAP_ALL_EDGES;
          }
      case H_COLLISION_CONTAINED:
        switch (v_collision)
          {
            case V_COLLISION_NONE:
                return PLY_RECTANGLE_OVERLAP_NONE;
            case V_COLLISION_TOP:
                return PLY_RECTANGLE_OVERLAP_TOP_EDGE;
            case V_COLLISION_BOTTOM:
                return PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE;
            case V_COLLISION_BOTH:
                return PLY_RECTANGLE_OVERLAP_TOP_AND_BOTTOM_EDGES;
            case V_COLLISION_CONTAINED:
                return PLY_RECTANGLE_OVERLAP_NO_EDGES;
            case V_COLLISION_EXACT:
                return PLY_RECTANGLE_OVERLAP_NO_EDGES;
          }
      case H_COLLISION_EXACT:
        switch (v_collision)
          {
            case V_COLLISION_NONE:
                return PLY_RECTANGLE_OVERLAP_NONE;
            case V_COLLISION_TOP:
                return PLY_RECTANGLE_OVERLAP_EXACT_TOP_EDGE;
            case V_COLLISION_BOTTOM:
                return PLY_RECTANGLE_OVERLAP_EXACT_BOTTOM_EDGE;
            case V_COLLISION_BOTH:
                return PLY_RECTANGLE_OVERLAP_ALL_EDGES;
            case V_COLLISION_CONTAINED:
                return PLY_RECTANGLE_OVERLAP_NO_EDGES;
            case V_COLLISION_EXACT:
                return PLY_RECTANGLE_OVERLAP_NO_EDGES;
          }
    }
  return PLY_RECTANGLE_OVERLAP_NONE;
}

void
ply_rectangle_intersect (ply_rectangle_t *rectangle1,
                         ply_rectangle_t *rectangle2,
                         ply_rectangle_t *result)
{

  long rectangle1_top_edge;
  long rectangle1_left_edge;
  long rectangle1_right_edge;
  long rectangle1_bottom_edge;
  long rectangle2_top_edge;
  long rectangle2_left_edge;
  long rectangle2_right_edge;
  long rectangle2_bottom_edge;
  long result_top_edge;
  long result_left_edge;
  long result_right_edge;
  long result_bottom_edge;

  if (ply_rectangle_is_empty (rectangle1))
    {
      *result = *rectangle1;
      return;
    }

  if (ply_rectangle_is_empty (rectangle2))
    {
      *result = *rectangle2;
      return;
    }

  rectangle1_top_edge = rectangle1->y;
  rectangle1_left_edge = rectangle1->x;
  rectangle1_right_edge = rectangle1->x + rectangle1->width - 1;
  rectangle1_bottom_edge = rectangle1->y + rectangle1->height - 1;

  rectangle2_top_edge = rectangle2->y;
  rectangle2_left_edge = rectangle2->x;
  rectangle2_right_edge = rectangle2->x + rectangle2->width - 1;
  rectangle2_bottom_edge = rectangle2->y + rectangle2->height - 1;

  result_top_edge = MAX (rectangle1_top_edge, rectangle2_top_edge);
  result_left_edge = MAX (rectangle1_left_edge, rectangle2_left_edge);
  result_right_edge = MIN (rectangle1_right_edge, rectangle2_right_edge);
  result_bottom_edge = MIN (rectangle1_bottom_edge, rectangle2_bottom_edge);

  result->x = result_left_edge;
  result->y = result_top_edge;

  if (result_right_edge >= result_left_edge)
    result->width = result_right_edge - result_left_edge + 1;
  else
    result->width = 0;

  if (result_bottom_edge >= result_top_edge)
    result->height = result_bottom_edge - result_top_edge + 1;
  else
    result->height = 0;

  if (ply_rectangle_is_empty (result))
    {
      result->width = 0;
      result->height = 0;
    }
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
