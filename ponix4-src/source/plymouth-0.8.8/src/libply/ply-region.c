/* ply-region.c
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
#include "ply-region.h"

#include <assert.h>
#include <stdlib.h>

#include "ply-list.h"
#include "ply-rectangle.h"

struct _ply_region
{
  ply_list_t *rectangle_list;
};

ply_region_t *
ply_region_new (void)
{
  ply_region_t *region;

  region = calloc (1, sizeof (ply_region_t));

  region->rectangle_list = ply_list_new ();

  return region;
}

void
ply_region_clear (ply_region_t *region)
{
  ply_list_node_t *node;

  node = ply_list_get_first_node (region->rectangle_list);
  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_rectangle_t *rectangle;

      rectangle = (ply_rectangle_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (region->rectangle_list, node);

      free (rectangle);
      ply_list_remove_node (region->rectangle_list, node);

      node = next_node;
    }
}

void
ply_region_free (ply_region_t *region)
{

  ply_region_clear (region);
  ply_list_free (region->rectangle_list);
  free (region);
}

static ply_rectangle_t *
copy_rectangle (ply_rectangle_t *rectangle)
{
  ply_rectangle_t *new_rectangle;

  new_rectangle = malloc (sizeof (*rectangle));
  *new_rectangle = *rectangle;

  return new_rectangle;
}

static void
merge_rectangle_with_sub_list (ply_region_t    *region,
                               ply_rectangle_t *new_area,
                               ply_list_node_t *node)
{

  if (ply_rectangle_is_empty (new_area))
    {
      free (new_area);
      return;
    }

  while (node != NULL)
    {
      ply_list_node_t *next_node;
      ply_rectangle_t *old_area;
      ply_rectangle_overlap_t overlap;

      old_area = (ply_rectangle_t *) ply_list_node_get_data (node);

      next_node = ply_list_get_next_node (region->rectangle_list, node);

      if (ply_rectangle_is_empty (new_area))
        overlap = PLY_RECTANGLE_OVERLAP_NO_EDGES;
      else if (ply_rectangle_is_empty (old_area))
        overlap = PLY_RECTANGLE_OVERLAP_ALL_EDGES;
      else
        overlap = ply_rectangle_find_overlap (old_area, new_area);

      switch (overlap)
        {
          /* NNNN      The new rectangle and node rectangle don't touch,
           * NNNN OOOO so let's move on to the next one.
           *      OOOO
           */
          case PLY_RECTANGLE_OVERLAP_NONE:
          break;

          /* NNNNN   We need to split the new rectangle into
           * NNOOOOO two rectangles:  The top row of Ns and
           * NNOOOOO the left side of Ns.
           *   OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_TOP_AND_LEFT_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);
              rectangle->y = old_area->y;
              rectangle->width = old_area->x - new_area->x;
              rectangle->height = (new_area->y + new_area->height) - old_area->y;

              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->height = old_area->y - new_area->y;
            }
          break;

          /*   NNNNN We need to split the new rectangle into
           * OOOOONN two rectangles:  The top row of Ns and
           * OOOOONN the right side of Ns.
           * OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_TOP_AND_RIGHT_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);
              rectangle->x = old_area->x + old_area->width;
              rectangle->y = old_area->y;
              rectangle->width = (new_area->x + new_area->width) - (old_area->x + old_area->width);
              rectangle->height = (new_area->y + new_area->height) - old_area->y;

              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->height = old_area->y - new_area->y;
            }
          break;

          /* NNNNNNN We need to trim out the part of
           * NOOOOON old rectangle that overlaps the new
           * NOOOOON rectangle by shrinking and moving it
           *  OOOOO  and then we need to add the new rectangle.
           */
          case PLY_RECTANGLE_OVERLAP_TOP_AND_SIDE_EDGES:
            {
              old_area->height = (old_area->y + old_area->height)
                                 - (new_area->y + new_area->height);
              old_area->y = new_area->y + new_area->height;
            }
          break;

          /*   NNN  We only care about the top row of Ns,
           *  ONNNO everything below that is already handled by
           *  ONNNO the old rectangle.
           *  OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_TOP_EDGE:
            new_area->height = old_area->y - new_area->y;
          break;

          /*   OOOOO We need to split the new rectangle into
           * NNOOOOO two rectangles:  The left side of Ns and
           * NNOOOOO the bottom row of Ns.
           * NNOOOOO
           * NNNNN
           */
          case PLY_RECTANGLE_OVERLAP_BOTTOM_AND_LEFT_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);

              rectangle->width = old_area->x - new_area->x;
              rectangle->height = (old_area->y + old_area->height) - new_area->y;

              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->height = (new_area->y + new_area->height) - (old_area->y + old_area->height);
              new_area->y = old_area->y + old_area->height;
            }
          break;

          /*   OOOOO   We need to split the new rectangle into
           *   OOOOONN two rectangles:  The right side of Ns and
           *   OOOOONN the bottom row of Ns.
           *   OOOOONN
           *     NNNNN
           */
          case PLY_RECTANGLE_OVERLAP_BOTTOM_AND_RIGHT_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);

              rectangle->x = old_area->x + old_area->width;
              rectangle->width = (new_area->x + new_area->width) - (old_area->x + old_area->width);
              rectangle->height = (old_area->y + old_area->height) - new_area->y;

              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->height = (new_area->y + new_area->height) - (old_area->y + old_area->height);
              new_area->y = old_area->y + old_area->height;
            }
          break;

          /*  OOOOO  We need to trim out the part of
           * NOOOOON old rectangle that overlaps the new
           * NOOOOON rectangle by shrinking it
           * NNNNNNN and then we need to add the new rectangle.
           */
          case PLY_RECTANGLE_OVERLAP_BOTTOM_AND_SIDE_EDGES:
            {
              old_area->height = new_area->y - old_area->y;
            }
          break;

          /*  OOOOO We only care about the bottom row of Ns,
           *  ONNNO everything above that is already handled by
           *  ONNNO the old rectangle.
           *   NNN
           */
          case PLY_RECTANGLE_OVERLAP_BOTTOM_EDGE:
            {
              new_area->height = (new_area->y + new_area->height) - (old_area->y + old_area->height);
              new_area->y = old_area->y + old_area->height;
            }
          break;

          /*  NNNN   We need to trim out the part of
           *  NNNNO  old rectangle that overlaps the new
           *  NNNNO  rectangle by shrinking it and moving it
           *  NNNN   and then we need to add the new rectangle.
           */
          case PLY_RECTANGLE_OVERLAP_TOP_LEFT_AND_BOTTOM_EDGES:
            {
              old_area->width = (old_area->x + old_area->width)
                                 - (new_area->x + new_area->width);
              old_area->x = new_area->x + new_area->width;
            }
          break;

          /*  NNNN  We need to trim out the part of
           * ONNNN  old rectangle that overlaps the new
           * ONNNN  rectangle by shrinking it and then we
           *  NNNN  need to add the new rectangle.
           */
          case PLY_RECTANGLE_OVERLAP_TOP_RIGHT_AND_BOTTOM_EDGES:
            old_area->width = new_area->x - old_area->x;
          break;

          /* NNNNNNN The old rectangle is completely inside the new rectangle
           * NOOOOON so replace the old rectangle with the new rectangle.
           * NOOOOON
           * NNNNNNN
           */
          case PLY_RECTANGLE_OVERLAP_ALL_EDGES:
            merge_rectangle_with_sub_list (region, new_area, next_node);
            free (old_area);
            ply_list_remove_node (region->rectangle_list, node);
          return;

          /*  NNN  We need to split the new rectangle into
           * ONNNO two rectangles: the top and bottom row of Ns
           * ONNNO
           *  NNN
           */
          case PLY_RECTANGLE_OVERLAP_TOP_AND_BOTTOM_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);
              rectangle->y = old_area->y + old_area->height;
              rectangle->width = new_area->width;
              rectangle->height = (new_area->y + new_area->height) - (old_area->y + old_area->height);
              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->height = old_area->y - new_area->y;
            }
          break;

          /*  OOOOO We only care about the side row of Ns,
           * NNNNOO everything rigth of that is already handled by
           * NNNNOO the old rectangle.
           *  OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_LEFT_EDGE:
            new_area->width = old_area->x - new_area->x;
          break;

          /* OOOOO  We only care about the side row of Ns,
           * NNNNNN everything left of that is already handled by
           * NNNNNN the old rectangle.
           * OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_RIGHT_EDGE:
            {
              long temp = new_area->x;
              new_area->x = old_area->x + old_area->width;
              new_area->width = (temp + new_area->width) - (old_area->x + old_area->width);
            }
          break;

          /*  OOOOO  We need to split the new rectangle into
           * NNNNNNN two rectangles: the side columns of Ns
           * NNNNNNN
           *  OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_SIDE_EDGES:
            {
              ply_rectangle_t *rectangle;

              rectangle = copy_rectangle (new_area);

              rectangle->x = old_area->x + old_area->width;
              rectangle->width = (new_area->x + new_area->width) - (old_area->x + old_area->width);

              merge_rectangle_with_sub_list (region, rectangle, next_node);

              new_area->width = old_area->x - new_area->x;
            }
          break;

          /* OOOOOOO The new rectangle is completely inside an old rectangle
           * ONNNNNO so return early without adding the new rectangle.
           * ONNNNNO
           * OOOOOOO
           */
          case PLY_RECTANGLE_OVERLAP_NO_EDGES:
            free (new_area);
          return;

          /*  NNNNN We expand the old rectangle up and throw away the new.
           *  NNNNN We must merge it because the new region may have overlapped
           *  NNNNN something further down the list.
           *  OOOOO
           */
          case PLY_RECTANGLE_OVERLAP_EXACT_TOP_EDGE:
            {
              old_area->height = (old_area->y + old_area->height) - new_area->y;
              old_area->y = new_area->y;
              free (new_area);
              merge_rectangle_with_sub_list (region, old_area, next_node);
              ply_list_remove_node (region->rectangle_list, node);
            }
          return;

          /*  OOOOO We expand the old rectangle down and throw away the new.
           *  NNNNN We must merge it because the new region may have overlapped
           *  NNNNN something further down the list.
           *  NNNNN
           */
          case PLY_RECTANGLE_OVERLAP_EXACT_BOTTOM_EDGE:
            {
              old_area->height = (new_area->y + new_area->height) - old_area->y;
              free (new_area);
              merge_rectangle_with_sub_list (region, old_area, next_node);
              ply_list_remove_node (region->rectangle_list, node);
            }
          return;

          /*  NNNNNO We expand the old rectangle left and throw away the new.
           *  NNNNNO We must merge it because the new region may have overlapped
           *  NNNNNO something further down the list.
           */
          case PLY_RECTANGLE_OVERLAP_EXACT_LEFT_EDGE:
            {
              old_area->width = (old_area->x + old_area->width) - new_area->x;
              old_area->x = new_area->x;
              free (new_area);
              merge_rectangle_with_sub_list (region, old_area, next_node);
              ply_list_remove_node (region->rectangle_list, node);
            }
          return;

          /*  ONNNNN We expand the old rectangle right and throw away the new.
           *  ONNNNN We must merge it because the new region may have overlapped
           *  ONNNNN something further down the list.
           */
          case PLY_RECTANGLE_OVERLAP_EXACT_RIGHT_EDGE:
            {
              old_area->width = (new_area->x + new_area->width) - old_area->x;
              free (new_area);
              merge_rectangle_with_sub_list (region, old_area, next_node);
              ply_list_remove_node (region->rectangle_list, node);
            }
          return;


        }

      node = ply_list_get_next_node (region->rectangle_list, node);
    }

  ply_list_append_data (region->rectangle_list, new_area);
}

void
ply_region_add_rectangle (ply_region_t    *region,
                          ply_rectangle_t *rectangle)
{
  ply_list_node_t *first_node;
  ply_rectangle_t *rectangle_copy;

  assert (region != NULL);
  assert (rectangle != NULL);

  first_node = ply_list_get_first_node (region->rectangle_list);

  rectangle_copy = copy_rectangle (rectangle);
  merge_rectangle_with_sub_list (region,
                                 rectangle_copy,
                                 first_node);
}

ply_list_t *
ply_region_get_rectangle_list (ply_region_t *region)
{
  return region->rectangle_list;
}

static int
rectangle_compare_y (void *element_a, void *element_b)
{
  ply_rectangle_t *rectangle_a = element_a;
  ply_rectangle_t *rectangle_b = element_b;
  return rectangle_a->y - rectangle_b->y;
}

ply_list_t *
ply_region_get_sorted_rectangle_list (ply_region_t *region)
{
  ply_list_sort (region->rectangle_list, &rectangle_compare_y);
  return region->rectangle_list;
}

#ifdef PLY_REGION_ENABLE_TEST
#include <stdio.h>

#define COVER_SIZE 100
#define RECTANGLE_COUNT 1000

static void
cover_with_rect(char             cover[COVER_SIZE][COVER_SIZE],
                ply_rectangle_t *rectangle,
                char             value)
{      /* is value is not zero, the entry will be set to the value,
          otherwise entry is incremented*/
  unsigned long x, y;
  for (y=0; y<rectangle->height; y++)
    {
      for (x=0; x<rectangle->width; x++)
        {
          if (rectangle->x + x < COVER_SIZE &&
              rectangle->y + y < COVER_SIZE)
            {
              if (value)
                cover[rectangle->y + y][rectangle->x + x] = value;
              else
                cover[rectangle->y + y][rectangle->x + x]++;
            }
        }
    }
}

static int
do_test (void)
{
  ply_rectangle_t rectangle;
  char cover[COVER_SIZE][COVER_SIZE];
  int i;
  unsigned long x, y;
  ply_region_t *region;
  ply_list_node_t *node;

  region = ply_region_new ();

  for (y = 0; y < COVER_SIZE; y++)
    {
      for (x = 0; x < COVER_SIZE; x++)
        {
          cover[y][x] = 0;
        }
    }

  for (i = 0; i < RECTANGLE_COUNT; i++)
    {
      rectangle.x = random() % COVER_SIZE-5;
      rectangle.y = random() % COVER_SIZE-5;
      rectangle.width = 1 + random() % 20;
      rectangle.height = 1 + random() % 20;
      printf("Adding X=%ld Y=%ld W=%ld H=%ld\n",
              rectangle.x,
              rectangle.y,
              rectangle.width,
              rectangle.height);
      cover_with_rect(cover, &rectangle, 100); /* 100 means covered by origial squares */
      ply_region_add_rectangle (region, &rectangle);
    }

  printf("Converted to:\n");
  int count = 0;

  ply_list_t *rectangle_list = ply_region_get_rectangle_list (region);
  for (node = ply_list_get_first_node (rectangle_list);
       node;
       node = ply_list_get_next_node (rectangle_list, node))
    {
      ply_rectangle_t *small_rectangle = ply_list_node_get_data (node);
      printf("Processed X=%ld Y=%ld W=%ld H=%ld\n",
             small_rectangle->x,
             small_rectangle->y, 
             small_rectangle->width, 
             small_rectangle->height);
      cover_with_rect(cover, small_rectangle, 0);
      count++;
    }
  printf("Rectangles in:%d out:%d\n", RECTANGLE_COUNT, count);

  count=0;

  for (y = 0; y < COVER_SIZE; y++)
    {
      printf("%03ld ", y);
      for (x = 0; x < COVER_SIZE; x++)
        {
          if (cover[y][x] >= 100)
            {
              if (cover[y][x] == 100)
                {
                  printf("-");  /* "-" means should have been covered but wasn't */
                  count++;
                }
              else
                {
                  if (cover[y][x] == 101)
                    printf("O");  /* "O" means correctly covered */
                  else
                    {
                      printf("%d", cover[y][x] - 101);
                      count++;  /* 1+ means covered multiple times*/
                    }
                }
            }
          else
            {
              if (cover[y][x] == 0)
                printf("o");  /* "o" means not involved*/
              else
                {
                  printf("%c", 'A' - 1 + cover[y][x]);
                  count++;  /* A+ means covered despite being not involved*/
                }
            }
        }
      printf("\n");
    }
  printf("errors:%d\n", count);
  return count;
}
int
main (int    argc,
      char **argv)
{
  int i;
  srandom(312);
  for (i=0; i<100; i++)
    {
      if (do_test ()) return 1;
    }
  return 0;
}
#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
