/* ply-key-file.c - key file loader
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
 * Some implementation taken from the cairo library.
 *
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#include "config.h"
#include "ply-key-file.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "ply-utils.h"
#include "ply-hashtable.h"
#include "ply-logger.h"

typedef struct
{
  char *key;
  char *value;
} ply_key_file_entry_t;

typedef struct
{
  char *name;
  ply_hashtable_t *entries;
} ply_key_file_group_t;

struct _ply_key_file
{
  char  *filename;
  FILE  *fp;

  ply_hashtable_t *groups;
};

typedef struct
{
  ply_key_file_foreach_func_t *func;
  void                        *user_data;
  char                        *group_name;
} ply_key_file_foreach_func_data_t;

static bool ply_key_file_open_file (ply_key_file_t *key_file);
static void ply_key_file_close_file (ply_key_file_t *key_file);

static bool
ply_key_file_open_file (ply_key_file_t *key_file)
{
  assert (key_file != NULL);

  key_file->fp = fopen (key_file->filename, "r");

  if (key_file->fp == NULL)
    {
      ply_trace ("Failed to open key file %s: %m",
                 key_file->filename);
      return false;
    }
  return true;
}

static void
ply_key_file_close_file (ply_key_file_t *key_file)
{
  assert (key_file != NULL);

  if (key_file->fp == NULL)
    return;
  fclose (key_file->fp);
  key_file->fp = NULL;
}

ply_key_file_t *
ply_key_file_new (const char *filename)
{
  ply_key_file_t *key_file;

  assert (filename != NULL);

  key_file = calloc (1, sizeof (ply_key_file_t));

  key_file->filename = strdup (filename);
  key_file->fp = NULL;
  key_file->groups = ply_hashtable_new (ply_hashtable_string_hash, ply_hashtable_string_compare);

  return key_file;
}

static void
ply_key_file_free_entry_foreach (void *key, 
                                 void *data,
                                 void *user_data)
{
  ply_key_file_entry_t *entry = data;
  free (entry->key);
  free (entry->value);
  free (entry);
}

static void
ply_key_file_free_group (void *key, 
                         void *data,
                         void *user_data)
{
  ply_key_file_group_t *group = data;
  
  ply_hashtable_foreach (group->entries,
					     ply_key_file_free_entry_foreach,
					     NULL);
  ply_hashtable_free (group->entries);
  free (group->name);
  free (group);
}

void
ply_key_file_free (ply_key_file_t *key_file)
{
  if (key_file == NULL)
    return;

  assert (key_file->filename != NULL);
  ply_hashtable_foreach (key_file->groups,
					     ply_key_file_free_group,
					     NULL);
  
  
  ply_hashtable_free (key_file->groups);
  free (key_file->filename);
  free (key_file);
}

static ply_key_file_group_t *
ply_key_file_load_group (ply_key_file_t *key_file,
                         const char     *group_name)
{
  int items_matched;
  ply_key_file_group_t *group;

  group = calloc (1, sizeof (ply_key_file_group_t));
  group->name = strdup (group_name);
  group->entries = ply_hashtable_new (ply_hashtable_string_hash, ply_hashtable_string_compare);

  ply_trace ("trying to load group %s", group_name);
  do
    {
      ply_key_file_entry_t *entry;
      char *key;
      char *value;
      long offset;
      int first_byte;

      key = NULL;
      value = NULL;

      first_byte = fgetc (key_file->fp);
      if (first_byte == '#')
        {
          char *line_to_toss;
          size_t number_of_bytes;

          line_to_toss = NULL;
          number_of_bytes = 0;

          getline (&line_to_toss, &number_of_bytes,
                   key_file->fp);
          free (line_to_toss);
          items_matched = 0;
          continue;
        }
      ungetc (first_byte, key_file->fp);

      offset = ftell (key_file->fp);
      items_matched = fscanf (key_file->fp, " %a[^= \t\n] = %a[^\n] ", &key, &value);

      if (items_matched != 2)
        {
          if (items_matched == 1)
            fseek (key_file->fp, offset, SEEK_SET);

          free (key);
          free (value);
          break;
        }

      entry = calloc (1, sizeof (ply_key_file_entry_t));

      entry->key = key;
      entry->value = value;

      ply_hashtable_insert (group->entries, entry->key, entry);
    }
  while (items_matched != EOF);

  return group;
}

static bool
ply_key_file_load_groups (ply_key_file_t *key_file)
{
  int items_matched;
  bool added_group = false;
  bool has_comments = false;

  do
    {
      char *group_name;
      int first_byte;

      ply_key_file_group_t *group;

      first_byte = fgetc (key_file->fp);
      if (first_byte == '#')
        {
          char *line_to_toss;
          size_t number_of_bytes;

          line_to_toss = NULL;
          number_of_bytes = 0;

          getline (&line_to_toss, &number_of_bytes,
                   key_file->fp);
          free (line_to_toss);
          has_comments = true;
          items_matched = 0;
          continue;
        }
      ungetc (first_byte, key_file->fp);

      group_name = NULL;
      items_matched = fscanf (key_file->fp, " [ %a[^]] ] ", &group_name);

      if (items_matched <= 0)
        {
          ply_trace ("key file has no %sgroups",
                     added_group? "more " : "");
          break;
        }

      assert (group_name != NULL);
      group = ply_key_file_load_group (key_file, group_name);

      free (group_name);

      if (group == NULL)
        break;

      ply_hashtable_insert (key_file->groups, group->name, group);
      added_group = true;
    }
  while (items_matched != EOF);

  if (!added_group && has_comments)
    {
      ply_trace ("key file has comments but no groups");
    }

  return added_group;
}

bool
ply_key_file_load (ply_key_file_t *key_file)
{
  bool was_loaded;

  assert (key_file != NULL);

  if (!ply_key_file_open_file (key_file))
    return false;

  was_loaded = ply_key_file_load_groups (key_file);

  if (!was_loaded)
    {
      ply_trace ("was unable to load any groups");
    }

  ply_key_file_close_file (key_file);

  return was_loaded;
}

static ply_key_file_group_t *
ply_key_file_find_group (ply_key_file_t *key_file,
                         const char     *group_name)
{
  return  ply_hashtable_lookup (key_file->groups, (void *) group_name);
}

static ply_key_file_entry_t *
ply_key_file_find_entry (ply_key_file_t       *key_file,
                         ply_key_file_group_t *group,
                         const char           *key)
{
  return  ply_hashtable_lookup (group->entries, (void *) key);
}

bool
ply_key_file_has_key (ply_key_file_t *key_file,
                      const char     *group_name,
                      const char     *key)
{
  ply_key_file_group_t *group;
  ply_key_file_entry_t *entry;

  group = ply_key_file_find_group (key_file, group_name);

  if (group == NULL)
    return false;

  entry = ply_key_file_find_entry (key_file, group, key);

  return entry != NULL;
}

char *
ply_key_file_get_value (ply_key_file_t *key_file,
                        const char     *group_name,
                        const char     *key)
{
  ply_key_file_group_t *group;
  ply_key_file_entry_t *entry;

  group = ply_key_file_find_group (key_file, group_name);

  if (group == NULL)
    {
      ply_trace ("key file does not have group '%s'", group_name);
      return NULL;
    }

  entry = ply_key_file_find_entry (key_file, group, key);

  if (entry == NULL)
    {
      ply_trace ("key file does not have entry for key '%s'", key);
      return NULL;
    }

  return strdup (entry->value);
}

static void
ply_key_file_foreach_entry_entries (void *key,
                                    void *data,
                                    void *user_data)
{
  ply_key_file_entry_t *entry;
  ply_key_file_foreach_func_data_t *func_data;

  func_data = user_data;
  entry = data;

  func_data->func(func_data->group_name,
                  entry->key,
                  entry->value,
                  func_data->user_data);
}

static void
ply_key_file_foreach_entry_groups (void *key,
                                   void *data,
                                   void *user_data)
{
  ply_key_file_group_t *group;
  ply_key_file_foreach_func_data_t *func_data;

  func_data = user_data;
  group = data;
  func_data->group_name = group->name;

  ply_hashtable_foreach (group->entries,
                         ply_key_file_foreach_entry_entries,
                         func_data);
}

void
ply_key_file_foreach_entry (ply_key_file_t              *key_file,
                            ply_key_file_foreach_func_t  func,
                            void                        *user_data)
{
  ply_key_file_foreach_func_data_t func_data;

  func_data.func = func;
  func_data.user_data = user_data;
  ply_hashtable_foreach (key_file->groups,
                         ply_key_file_foreach_entry_groups,
                         &func_data);
}

/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
