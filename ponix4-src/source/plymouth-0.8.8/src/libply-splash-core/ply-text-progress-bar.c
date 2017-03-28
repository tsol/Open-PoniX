/* ply-text-progress-bar.c -  simple text based progress bar
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
 * Written by: Adam Jackson <ajax@redhat.com>
 *             Bill Nottingham <notting@redhat.com>
 *             Ray Strode <rstrode@redhat.com>
 *             Soeren Sandmann <sandmann@redhat.com>
 */
#include "config.h"

#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <values.h>
#include <unistd.h>
#include <wchar.h>

#include "ply-text-display.h"
#include "ply-text-progress-bar.h"
#include "ply-array.h"
#include "ply-logger.h"
#include "ply-utils.h"

#include <linux/kd.h>

#ifndef FRAMES_PER_SECOND
#define FRAMES_PER_SECOND 5
#endif

#define NUMBER_OF_INDICATOR_COLUMNS 6

static char *os_string;

struct _ply_text_progress_bar
{
  ply_text_display_t *display;

  int column, row;
  int number_of_rows;
  int number_of_columns;

  double percent_done;
  uint32_t is_hidden : 1;
};

ply_text_progress_bar_t *
ply_text_progress_bar_new (void)
{
  ply_text_progress_bar_t *progress_bar;

  progress_bar = calloc (1, sizeof (ply_text_progress_bar_t));

  progress_bar->row = 0;
  progress_bar->column = 0;
  progress_bar->number_of_columns = 0;
  progress_bar->number_of_rows = 0;

  return progress_bar;
}

void
ply_text_progress_bar_free (ply_text_progress_bar_t *progress_bar)
{
  if (progress_bar == NULL)
    return;

  free (progress_bar);
}

static void
get_os_string (void)
{
  int fd;
  char *buf, *pos, *pos2;
  struct stat sbuf;

  buf = NULL;

  fd = open (RELEASE_FILE, O_RDONLY);
  if (fd == -1)
    goto out;

  if (fstat (fd, &sbuf) == -1) {
    close (fd);
    goto out;
  }

  buf = calloc (sbuf.st_size + 1, sizeof(char));
  read (fd, buf, sbuf.st_size);
  close (fd);

  if (strcmp (RELEASE_FILE, "/etc/os-release") == 0)
    {
      char key[] = "PRETTY_NAME=";

      for (pos = strstr (buf, key);
           pos != NULL;
           pos = strstr (pos, key))
        {
          if (pos == buf || pos[-1] == '\n')
            break;
        }

      if (pos != NULL)
        {
          pos += strlen (key);
          pos2 = strstr (pos, "\n");

          if (pos2 != NULL)
            *pos2 = '\0';

          if ((*pos == '\"' && pos2[-1] == '\"') ||
              (*pos == '\'' && pos2[-1] == '\''))
            {
              pos++;
              pos2--;

              *pos2 = '\0';
            }
          asprintf (&os_string, " %s", pos);
        }
      goto out;
    }

  pos = strstr (buf, " release ");

  if (pos == NULL)
    goto out;

  pos2 = strstr (pos, " (");

  if (pos2 == NULL)
    goto out;

  *pos = '\0';
  pos += strlen (" release ");

  *pos2 = '\0';
  asprintf (&os_string, " %s %s", buf, pos);

out:
  free (buf);

  if (os_string == NULL)
    os_string = strdup ("");
}

void
ply_text_progress_bar_draw (ply_text_progress_bar_t *progress_bar)
{
    int i, width;
    double brown_fraction, blue_fraction, white_fraction;

    if (progress_bar->is_hidden)
      return;

    width = progress_bar->number_of_columns - 2 - strlen (os_string);

    ply_text_display_set_cursor_position (progress_bar->display,
                                          progress_bar->column,
                                          progress_bar->row);

    brown_fraction = - (progress_bar->percent_done * progress_bar->percent_done) + 2 * progress_bar->percent_done;
    blue_fraction  = progress_bar->percent_done;
    white_fraction = progress_bar->percent_done * progress_bar->percent_done;

    for (i = 0; i < width; i++) {
        double f;

        f = (double) i / (double) width;
        if (f < white_fraction)
            ply_text_display_set_background_color (progress_bar->display,
                                                   PLY_TERMINAL_COLOR_WHITE);
        else if (f < blue_fraction)
            ply_text_display_set_background_color (progress_bar->display,
                                             PLY_TERMINAL_COLOR_BLUE);
        else if (f < brown_fraction)
            ply_text_display_set_background_color (progress_bar->display,
                                             PLY_TERMINAL_COLOR_BROWN);
        else
          break;

        ply_text_display_write (progress_bar->display, "%c", ' ');
    }

    ply_text_display_set_background_color (progress_bar->display,
                                           PLY_TERMINAL_COLOR_BLACK);

    if (brown_fraction > 0.5) {
        if (white_fraction > 0.875)
            ply_text_display_set_foreground_color (progress_bar->display,
                                                   PLY_TERMINAL_COLOR_WHITE);
        else if (blue_fraction > 0.66)
            ply_text_display_set_foreground_color (progress_bar->display,
                                                   PLY_TERMINAL_COLOR_BLUE);
        else
            ply_text_display_set_foreground_color (progress_bar->display,
                                                   PLY_TERMINAL_COLOR_BROWN);

        ply_text_display_set_cursor_position (progress_bar->display,
                                              progress_bar->column + width,
                                              progress_bar->row);

        ply_text_display_write (progress_bar->display, "%s", os_string);

        ply_text_display_set_foreground_color (progress_bar->display,
                                               PLY_TERMINAL_COLOR_DEFAULT);
    }
}

void
ply_text_progress_bar_show (ply_text_progress_bar_t  *progress_bar,
                            ply_text_display_t       *display)
{
  assert (progress_bar != NULL);

  progress_bar->display = display;

  progress_bar->number_of_rows = ply_text_display_get_number_of_rows (display);
  progress_bar->row = progress_bar->number_of_rows - 1;
  progress_bar->number_of_columns = ply_text_display_get_number_of_columns (display);
  progress_bar->column = 2;

  get_os_string ();

  progress_bar->is_hidden = false;

  ply_text_progress_bar_draw (progress_bar);
}

void
ply_text_progress_bar_hide (ply_text_progress_bar_t *progress_bar)
{
  progress_bar->display = NULL;
  progress_bar->is_hidden = true;
}

void
ply_text_progress_bar_set_percent_done (ply_text_progress_bar_t  *progress_bar,
                                        double percent_done)
{
  progress_bar->percent_done = percent_done;
}

double
ply_text_progress_bar_get_percent_done (ply_text_progress_bar_t  *progress_bar)
{
  return progress_bar->percent_done;
}

int
ply_text_progress_bar_get_number_of_columns (ply_text_progress_bar_t *progress_bar)
{
  return progress_bar->number_of_columns;
}

int
ply_text_progress_bar_get_number_of_rows (ply_text_progress_bar_t *progress_bar)
{
  return progress_bar->number_of_rows;
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
