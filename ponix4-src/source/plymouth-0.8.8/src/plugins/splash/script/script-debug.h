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
#ifndef SCRIPT_DEBUG_H
#define SCRIPT_DEBUG_H


typedef struct
{
  int line_index;
  int column_index;
  char* name;
} script_debug_location_t;


void script_debug_add_element (void                    *element,
                               script_debug_location_t *location);
void script_debug_remove_element (void *element);
script_debug_location_t *script_debug_lookup_element (void *element);

#endif /* SCRIPT_DEBUG_H */
