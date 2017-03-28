/* ply-progress.c - calculats boot progress 
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
 *             Soeren Sandmann <sandmann@redhat.com>
 *             Charlie Brej <cbrej@cs.man.ac.uk>
 */

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#include "ply-list.h"
#include "ply-logger.h"
#include "ply-progress.h"
#include "ply-utils.h"

#ifndef UPDATES_PER_SECOND
#define UPDATES_PER_SECOND 30
#endif

#ifndef DEFAULT_BOOT_DURATION
#define DEFAULT_BOOT_DURATION 60.0
#endif


struct _ply_progress
{
  double start_time;
  double pause_time;
  double scalar;
  double last_percentage;
  double last_percentage_time;
  double dead_time;
  double next_message_percentage;
  ply_list_t *current_message_list;
  ply_list_t *previous_message_list;
  uint32_t paused : 1;
};

typedef struct 
{
  double time;
  char* string;
  uint32_t disabled : 1;
} ply_progress_message_t;

ply_progress_t*
ply_progress_new (void)
{
  ply_progress_t *progress = calloc (1, sizeof (ply_progress_t));
  
  progress->start_time = ply_get_timestamp();
  progress->pause_time=0;
  progress->scalar=1.0/DEFAULT_BOOT_DURATION;
  progress->pause_time=0.0;
  progress->last_percentage=0.0;
  progress->last_percentage_time=0.0;
  progress->dead_time=0.0;
  progress->next_message_percentage=0.25;
  progress->current_message_list = ply_list_new ();
  progress->previous_message_list = ply_list_new ();
  progress->paused = false;
  return progress;
}

void
ply_progress_free (ply_progress_t* progress)
{
  ply_list_node_t *node;
  node = ply_list_get_first_node (progress->current_message_list);

  while (node)
   {
      ply_list_node_t *next_node;
      ply_progress_message_t *message = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (progress->current_message_list, node);

      free (message->string);
      free (message);
      node = next_node;
    }
  ply_list_free (progress->current_message_list);

  node = ply_list_get_first_node (progress->previous_message_list);

  while (node)
   {
      ply_list_node_t *next_node;
      ply_progress_message_t *message = ply_list_node_get_data (node);
      next_node = ply_list_get_next_node (progress->previous_message_list, node);

      free (message->string);
      free (message);
      node = next_node;
    }
  ply_list_free (progress->previous_message_list);
  free(progress);
  return;
}


static ply_progress_message_t*
ply_progress_message_search (ply_list_t *message_list, const char* string)
{
  ply_list_node_t *node;
  node = ply_list_get_first_node (message_list);

  while (node)
    {
      ply_progress_message_t *message = ply_list_node_get_data (node);
      if (strcmp(string, message->string)==0)
          return message;
      node = ply_list_get_next_node (message_list, node);
    }
  return NULL;
}


static ply_progress_message_t*
ply_progress_message_search_next (ply_list_t *message_list, double time)
{
  ply_list_node_t *node;
  node = ply_list_get_first_node (message_list);
  ply_progress_message_t *best=NULL;
  while (node)
    {
      ply_progress_message_t *message = ply_list_node_get_data (node);
      if (message->time > time && (!best || message->time < best->time))
          best = message;
      node = ply_list_get_next_node (message_list, node);
    }
  return best;
}

void
ply_progress_load_cache (ply_progress_t* progress,
                         const char *filename)
{
  FILE *fp;

  fp = fopen (filename,"r");
  if (fp == NULL)
    return;

  while (1)
    {
      int items_matched;
      double time;
      int string_size=81;
      char *string;
      char colon;
      int i=0;

      items_matched = fscanf (fp, "%lf", &time);
      if (items_matched<1) break;
      items_matched = fscanf (fp, "%c", &colon);
      if (items_matched<1 || colon != ':') break;

      string = malloc(sizeof(char)*string_size);
      while (1)
        {
          if (i>=string_size)
            {
              string_size*=2;
              string = realloc(string, sizeof(char)*string_size);
            }
          items_matched = fscanf (fp, "%c", &string[i]);
          if (items_matched<1 || string[i] == '\n')
            {
              string[i] = '\0';
              break;
            }
          i++;
        }
      ply_progress_message_t* message = malloc(sizeof(ply_progress_message_t));
      message->time = time;
      message->string = string;
      ply_list_append_data(progress->previous_message_list, message);
    }
  fclose (fp);
}

void
ply_progress_save_cache (ply_progress_t* progress,
                         const char *filename)
{
  FILE *fp;
  ply_list_node_t *node;
  double cur_time = ply_progress_get_time(progress);

  fp = fopen (filename,"w");
  if (fp == NULL)
    return;

  node = ply_list_get_first_node (progress->current_message_list);

  while (node)
    {
      ply_progress_message_t *message = ply_list_node_get_data (node);
      double percentage = message->time / cur_time;
      if (!message->disabled)
          fprintf (fp, "%.3lf:%s\n", percentage, message->string);
      node = ply_list_get_next_node (progress->current_message_list, node);
    }
  fclose (fp);
}


double
ply_progress_get_percentage (ply_progress_t* progress)
{
  double percentage;
  double cur_time = ply_progress_get_time (progress);
  
  if ((progress->last_percentage_time-progress->dead_time)*progress->scalar<0.999)
    {
      percentage = progress->last_percentage
                + (((cur_time - progress->last_percentage_time)*progress->scalar)
                / (1 - (progress->last_percentage_time-progress->dead_time)*progress->scalar))
                * (1 - progress->last_percentage);
      
      if ((percentage - progress->next_message_percentage)/progress->scalar > 1){
          percentage = progress->last_percentage
                     + (cur_time - progress->last_percentage_time) / (DEFAULT_BOOT_DURATION * 10);
          progress->dead_time += cur_time - progress->last_percentage_time;
        }
      percentage = CLAMP(percentage, 0.0, 1.0);
    }
  else 
    percentage = 1.0;
  
  progress->last_percentage_time = cur_time;
  progress->last_percentage = percentage;
  return percentage;
}

void
ply_progress_set_percentage (ply_progress_t* progress, double percentage)
{
  progress->next_message_percentage = 1;
  progress->scalar += percentage / (ply_progress_get_time(progress)-progress->dead_time);
  progress->scalar /= 2;
  return;
}

double
ply_progress_get_time (ply_progress_t* progress)
{
  if (progress->paused)
    {
      return progress->pause_time - progress->start_time;
    }
  return ply_get_timestamp() - progress->start_time;
}

void
ply_progress_pause (ply_progress_t* progress)
{
  progress->pause_time = ply_get_timestamp ();
  progress->paused = true;
  return;
}


void
ply_progress_unpause (ply_progress_t* progress)
{
  progress->start_time += ply_get_timestamp() - progress->pause_time;
  progress->paused = false;
  return;
}

void
ply_progress_status_update (ply_progress_t* progress,
                             const char  *status)
{
  ply_progress_message_t *message, *message_next;
  message = ply_progress_message_search(progress->current_message_list, status);
  if (message)
    {
      message->disabled = true;
    }                                                   /* Remove duplicates as they confuse things*/
  else
    {
      message = ply_progress_message_search(progress->previous_message_list, status);
      if (message)
        {
          message_next = ply_progress_message_search_next(progress->previous_message_list, message->time);
          if (message_next)
              progress->next_message_percentage = message_next->time;
          else
              progress->next_message_percentage = 1;
              
          progress->scalar += message->time / (ply_progress_get_time(progress)-progress->dead_time);
          progress->scalar /= 2;
        }
      message = malloc(sizeof(ply_progress_message_t));
      message->time = ply_progress_get_time (progress);
      message->string = strdup(status);
      message->disabled = false;
      ply_list_append_data(progress->current_message_list, message);
    }
}

#ifdef PLY_PROGRESS_ENABLE_TEST

#include <stdio.h>

int
main (int    argc,
      char **argv)
{
  double percent;
  int slowness;
  double time;
  int i;
  const char* strings[10]={"foobar", "barfoo", "barbar", "foo", "foo", "bar", "foo", "more", "even more", "even even more"};
  ply_progress_t* progress = ply_progress_new ();
  
  progress->scalar = 1.0/5;  /* Original time estimate is 5 sec*/

  percent = ply_progress_get_percentage (progress);
  time = ply_progress_get_time (progress);
  printf("Time:%f   \t Percentage: %f%%\n", time, percent*100);
  srand ((int) ply_get_timestamp ());
  
  slowness = rand () % 500000 + 50000;

  for (i=0; i<2; i++)
    {
      usleep ((rand () % slowness+slowness));
      percent = ply_progress_get_percentage (progress);
      time = ply_progress_get_time (progress);
      printf("Time:%f   \t Percentage: %f%%\n", time, percent*100);
    }
  printf("Load cache\n");
  ply_progress_load_cache (progress, PLYMOUTH_TIME_DIRECTORY "/boot-duration");

  for (i=0; i<10; i++)
    {
      ply_progress_status_update (progress, strings[i]);
      usleep ((rand () % slowness+slowness));
      percent = ply_progress_get_percentage (progress);
      time = ply_progress_get_time (progress);
      printf("Time:%f   \t Percentage: %f%% \tScalar:%f\n", time, percent*100, progress->scalar);
    }
  printf("Save and free cache\n");
  ply_progress_save_cache (progress, PLYMOUTH_TIME_DIRECTORY "/boot-duration");
  ply_progress_free(progress);

  printf("\nManual set percentage run\n\n");

  progress = ply_progress_new ();
  progress->scalar = 1.0/5;  /* Original time estimate is 5 sec*/

  percent = ply_progress_get_percentage (progress);
  time = ply_progress_get_time (progress);
  printf("Time:%f   \t Percentage: %f%%\n", time, percent*100);
  srand ((int) ply_get_timestamp ());

  for (i=0; i<12; i++)
    {
      ply_progress_set_percentage (progress, (double)i/12);
      usleep ((rand () % slowness+slowness));
      percent = ply_progress_get_percentage (progress);
      time = ply_progress_get_time (progress);
      printf("Time:%f   \t Percentage: %f%% (%f%%)\tScalar:%f\n", time, percent*100, (double)i/12*100, progress->scalar);
    }
  ply_progress_free(progress);

  return 0;
}

#endif /* PLY_PROGRESS_ENABLE_TEST */
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
    
