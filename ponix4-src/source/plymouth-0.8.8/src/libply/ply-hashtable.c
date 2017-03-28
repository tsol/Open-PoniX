/* ply-hashtable.c - hash table implementation
 *
 * Copyright (C) 2009 Charlie Brej <cbrej@cs.man.ac.uk>
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
 */
#include "config.h"
#include "ply-hashtable.h"
#include "ply-utils.h"
#include "ply-bitarray.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MASKGEN(x) {x |= x >> 16; x |= x >> 8; x |= x >> 4; x |= x >> 2;  x |= x >> 1;}

struct _ply_hashtable_node
{
  void   *data;
  void   *key;
};

struct _ply_hashtable
{
  struct _ply_hashtable_node   *nodes;
  unsigned int                  total_node_count;   /* must be a 2^X */
  ply_bitarray_t               *dirty_node_bitmap;
  unsigned int                  dirty_node_count;   /* live + dead nodes */
  ply_bitarray_t               *live_node_bitmap;
  unsigned int                  live_node_count;
  ply_hashtable_compare_func_t *compare_func;
  ply_hashtable_hash_func_t    *hash_func;
};

unsigned int
ply_hashtable_direct_hash (void *element)
{
  return (unsigned int) (intptr_t) element;
}

int
ply_hashtable_direct_compare (void *elementa,
                              void *elementb)
{
  return (int) ((intptr_t) elementa - (intptr_t) elementb);
}

unsigned int
ply_hashtable_string_hash (void *element)
{
  char* strptr;
  unsigned int hash = 0;
  for (strptr = element; *strptr; strptr++)
    {
      hash ^= *strptr;
      hash ^= hash << 1;
    }
  return hash;
}

int
ply_hashtable_string_compare (void *elementa,
                              void *elementb)
{
  return strcmp (elementa, elementb);
}

ply_hashtable_t *
ply_hashtable_new (ply_hashtable_hash_func_t    *hash_func,
                   ply_hashtable_compare_func_t *compare_func)
{
  ply_hashtable_t *hashtable;

  hashtable = malloc (sizeof (ply_hashtable_t));
  hashtable->total_node_count = 0;
  hashtable->dirty_node_count = 0;
  hashtable->live_node_count = 0;
  hashtable->nodes = NULL;
  hashtable->dirty_node_bitmap = NULL;
  hashtable->live_node_bitmap = NULL;
  hashtable->compare_func = compare_func;
  hashtable->hash_func = hash_func;

  if (hashtable->compare_func == NULL)
    {
      hashtable->compare_func = ply_hashtable_direct_compare;
    }
  if (hashtable->hash_func == NULL)
    {
      hashtable->hash_func = ply_hashtable_direct_hash;
    }
  ply_hashtable_resize (hashtable);
  return hashtable;
}

void
ply_hashtable_free (ply_hashtable_t *hashtable)
{
  if (hashtable == NULL) return;
  ply_bitarray_free (hashtable->dirty_node_bitmap);
  ply_bitarray_free (hashtable->live_node_bitmap);
  free(hashtable->nodes);
  free(hashtable);
}


static void
ply_hashtable_insert_internal (ply_hashtable_t *hashtable,
                               void            *key,
                               void            *data)
{
  unsigned int hash_index;
  int step = 0;
  
#ifdef PLY_HASHTABLE_ENABLE_TEST
    /* Make sure the counts are synchronised with bitmap */
  assert (ply_bitarray_count (hashtable->dirty_node_bitmap, hashtable->total_node_count) ==
          (int) hashtable->dirty_node_count);
  assert (ply_bitarray_count (hashtable->live_node_bitmap, hashtable->total_node_count) ==
          (int) hashtable->live_node_count);
#endif /* PLY_HASHTABLE_ENABLE_TEST */

  hash_index = hashtable->hash_func (key);
  hash_index &= hashtable->total_node_count - 1;

  while (ply_bitarray_lookup (hashtable->dirty_node_bitmap, hash_index))
    {
      step++;
      hash_index += step;
      hash_index &= hashtable->total_node_count - 1;
    }
  ply_bitarray_set (hashtable->dirty_node_bitmap, hash_index);
  ply_bitarray_set (hashtable->live_node_bitmap, hash_index);
  hashtable->nodes[hash_index].key = key;
  hashtable->nodes[hash_index].data = data;

  hashtable->live_node_count++;
  hashtable->dirty_node_count++;
}


void
ply_hashtable_resize (ply_hashtable_t *hashtable)
{
  unsigned int newsize, oldsize;
  unsigned int i;
  struct _ply_hashtable_node *oldnodes;
  ply_bitarray_t *old_live_node_bitmap;

  newsize = (hashtable->live_node_count + 1) * 4; /* make table 4x to 8x the number of live elements (at least 8) */
  MASKGEN (newsize);
  newsize++;
  oldsize = hashtable->total_node_count;
  oldnodes = hashtable->nodes;

  hashtable->total_node_count = newsize;
  hashtable->nodes = malloc (newsize * sizeof (struct _ply_hashtable_node));
  ply_bitarray_free (hashtable->dirty_node_bitmap);
  hashtable->dirty_node_bitmap = ply_bitarray_new(newsize);
  old_live_node_bitmap = hashtable->live_node_bitmap;
  hashtable->live_node_bitmap = ply_bitarray_new(newsize);
  hashtable->dirty_node_count = 0;
  hashtable->live_node_count = 0;

  for (i=0; i<oldsize; i++)
    {
      if (ply_bitarray_lookup (old_live_node_bitmap, i))
        ply_hashtable_insert_internal (hashtable, oldnodes[i].key, oldnodes[i].data);
    } 
  ply_bitarray_free (old_live_node_bitmap);
  free (oldnodes);
}

static inline void
ply_hashtable_resize_check (ply_hashtable_t *hashtable)
{
  if (hashtable->total_node_count < (hashtable->dirty_node_count * 2))
    ply_hashtable_resize (hashtable);   /* hash tables work best below 50% occupancy */
}

void
ply_hashtable_insert (ply_hashtable_t *hashtable,
                      void            *key,
                      void            *data)
{
  ply_hashtable_resize_check (hashtable);
  ply_hashtable_insert_internal (hashtable, key, data);
}

static int
ply_hashtable_lookup_index (ply_hashtable_t *hashtable,
                            void            *key)
{
  unsigned int hash_index;
  int step = 0;

  hash_index = hashtable->hash_func (key);
  while (1)
    {
      hash_index &= hashtable->total_node_count - 1;
      if (!ply_bitarray_lookup (hashtable->dirty_node_bitmap, hash_index))
        break;
      if (ply_bitarray_lookup (hashtable->live_node_bitmap, hash_index))
        if (!hashtable->compare_func (hashtable->nodes[hash_index].key, key))
          return hash_index;
      hash_index += step;
      step++;
    }
  return -1;
}

void *
ply_hashtable_remove (ply_hashtable_t *hashtable,
                      void            *key)
{
  int index;
  index = ply_hashtable_lookup_index (hashtable, key);
  if (index < 0)
    return NULL;
    
  ply_bitarray_clear (hashtable->live_node_bitmap, index);
  hashtable->live_node_count--;
  return hashtable->nodes[index].data;
}

void *
ply_hashtable_lookup (ply_hashtable_t *hashtable,
                      void            *key)
{
  int index;
  index = ply_hashtable_lookup_index (hashtable, key);
  if (index < 0)
    return NULL;
  return hashtable->nodes[index].data;
}

int
ply_hashtable_lookup_full (ply_hashtable_t *hashtable,
                           void            *key,
                           void           **reply_key,
                           void           **reply_data)
{
  int index;
  index = ply_hashtable_lookup_index (hashtable, key);
  if (index < 0)
    return false;
  *reply_key = hashtable->nodes[index].key;
  *reply_data = hashtable->nodes[index].data;
  return true;
}

void
ply_hashtable_foreach (ply_hashtable_t              *hashtable,
					   ply_hashtable_foreach_func_t  func,
					   void                         *user_data)
{
  unsigned int i;
  for (i = 0; i < hashtable->total_node_count; i++)
    {
      if (ply_bitarray_lookup (hashtable->live_node_bitmap, i))
        func(hashtable->nodes[i].key, hashtable->nodes[i].data, user_data);
    }
}


#ifdef PLY_HASHTABLE_ENABLE_TEST
#include <stdio.h>

static void
foreach_func (void *key,
              void *data,
              void *user_data)
{
  printf ("foreach key:%s data:%s\n", (char*) key, (char*) data);
}


int
main (int    argc,
      char **argv)
{
  ply_hashtable_t *hashtable;
  int i;
  const char* key[10] =  {"k1", "k2", "k3", "k4", "k5", "k6", "k7", "k8", "k9", "k10"};
  const char* data[10] = {"d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9", "d10"};
  char* reply_key = NULL;
  char* reply_data = NULL;
  
  printf ("hashtable test start\n");
  hashtable = ply_hashtable_new (ply_hashtable_string_hash, ply_hashtable_string_compare);
  for (i=0; i<10; i++)
    {
      ply_hashtable_insert (hashtable, (void *) key[i], (void *) data[9-i]);
    }
  for (i=0; i<10; i++)
    {
      reply_data = ply_hashtable_lookup (hashtable, (void *) key[i]);
      printf ("got:%s\n", reply_data);
    }
  for (i=0; i<10; i++)
    {
      ply_hashtable_remove (hashtable, (void *) key[i]);
      ply_hashtable_insert (hashtable, (void *) key[i], (void *) data[i]);
    }
  for (i=0; i<10; i++)
    {
      if (ply_hashtable_lookup_full (hashtable, (void *) key[i], (void**) &reply_key, (void**) &reply_data))
        printf ("got key:%s data:%s\n", reply_key, reply_data);
    }
  ply_hashtable_foreach (hashtable, foreach_func, NULL);
  ply_hashtable_free(hashtable);
  printf ("hashtable test end\n");
  return 0;
}

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
