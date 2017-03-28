/* script-debug.h - Debug handling matching memory loaded data to source locations
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
#include "ply-hashtable.h"
#include <stdlib.h>
#include <string.h>

#include "script-debug.h"

static ply_hashtable_t *script_debug_location_hash = NULL;
static ply_hashtable_t *script_debug_name_hash = NULL;

static void script_debug_setup (void)
{
  if (!script_debug_location_hash)
  {
    script_debug_location_hash = ply_hashtable_new(NULL, NULL);
    script_debug_name_hash = ply_hashtable_new(ply_hashtable_string_hash,
                                               ply_hashtable_string_compare);
  }
}

void script_debug_add_element (void                    *element,
                               script_debug_location_t *location)
{
  script_debug_setup();
  script_debug_location_t *new_location = malloc (sizeof(script_debug_location_t));
  new_location->line_index = location->line_index;
  new_location->column_index = location->column_index;
  new_location->name = ply_hashtable_lookup (script_debug_name_hash, location->name);
  if (!new_location->name)
    {
      new_location->name = strdup(location->name);
      ply_hashtable_insert (script_debug_name_hash, new_location->name, new_location->name);
    }
  ply_hashtable_insert (script_debug_location_hash, element, new_location);
}

void script_debug_remove_element (void *element)
{
  script_debug_setup();
  script_debug_location_t *old_location = ply_hashtable_remove (script_debug_location_hash,
                                                                element);
  free(old_location);
}

script_debug_location_t *script_debug_lookup_element (void *element)
{
  script_debug_setup();
  script_debug_location_t *location = ply_hashtable_lookup (script_debug_location_hash,
                                                            element);
  return location;
}
