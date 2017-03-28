/* ply-bitarray.h - bitarray implementation
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
#ifndef PLY_ARRAY_H
#define PLY_ARRAY_H

#include <stdint.h>
#include <stdbool.h>

#define PLY_BITARRAY_BASE_SIZE 32
#define PLY_BITARRAY_BASE_MASK (PLY_BITARRAY_BASE_SIZE-1)

typedef uint32_t ply_bitarray_t;

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
#define ply_bitarray_new(size) calloc ((size + PLY_BITARRAY_BASE_SIZE - 1) / PLY_BITARRAY_BASE_SIZE, sizeof (ply_bitarray_t))
#define ply_bitarray_free(bitarray) free (bitarray)
#define ply_bitarray_lookup(bitarray, index) ((bitarray[index / PLY_BITARRAY_BASE_SIZE] >> (index & PLY_BITARRAY_BASE_MASK)) & 1)
#define ply_bitarray_set(bitarray, index) (bitarray[index / PLY_BITARRAY_BASE_SIZE] |= 1 << (index & PLY_BITARRAY_BASE_MASK))
#define ply_bitarray_clear(bitarray, index) (bitarray[index / PLY_BITARRAY_BASE_SIZE] &= ~(1 << (index & PLY_BITARRAY_BASE_MASK)))
#define ply_bitarray_toggle(bitarray, index) (bitarray[index / PLY_BITARRAY_BASE_SIZE] ^= 1 << (index & PLY_BITARRAY_BASE_MASK))
int ply_bitarray_count (ply_bitarray_t *bitarray,
                        int             size);

#endif

#endif /* PLY_ARRAY_H */
