/* ply-array.h - dynamic array implementation
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
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_ARRAY_H
#define PLY_ARRAY_H

#include <stdint.h>

typedef struct _ply_array ply_array_t;
typedef enum _ply_array_element_type ply_array_element_type_t;

enum _ply_array_element_type
{
  PLY_ARRAY_ELEMENT_TYPE_POINTER,
  PLY_ARRAY_ELEMENT_TYPE_UINT32
};

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_array_t *ply_array_new (ply_array_element_type_t element_type);
void ply_array_free (ply_array_t *array);
int ply_array_get_size (ply_array_t *array);
void ply_array_add_pointer_element (ply_array_t *array,
                                    const void  *element);
void ply_array_add_uint32_element (ply_array_t    *array,
                                   const uint32_t  element);
void * const *ply_array_get_pointer_elements (ply_array_t *array);
uint32_t const *ply_array_get_uint32_elements (ply_array_t *array);
void **ply_array_steal_pointer_elements (ply_array_t *array);

uint32_t *ply_array_steal_uint32_elements (ply_array_t *array);
#endif

#endif /* PLY_ARRAY_H */
