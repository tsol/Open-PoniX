/* ply-bitarray.c - bitarray implementation
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
#include "ply-bitarray.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>



int
ply_bitarray_count (ply_bitarray_t *bitarray,
                    int             size)
{
  int count = 0;
  int i;
  for (i = 0; i < size; i++){
    count += ply_bitarray_lookup(bitarray, i);
    }
  return count;
}




#ifdef PLY_BITARRAY_ENABLE_TEST
#include <stdio.h>

int
main (int    argc,
      char **argv)
{
  ply_bitarray_t *bitarray;
  int i, i2;
  printf ("bitarray test start\n");
  bitarray = ply_bitarray_new (134);
  
  for (i=0; i<64; i++)
    {
      if (ply_bitarray_lookup (bitarray, i))
        printf ("1");
      else
        printf ("0");
    }
  printf ("\n");
  
  for (i=0; i<64; i++)
    if ((6654654654654654654ll >> i) & 1)
      ply_bitarray_set (bitarray, i);
  
  for (i=0; i<64; i++)
    {
      if (ply_bitarray_lookup (bitarray, i))
        printf ("1");
      else
        printf ("0");
    }
  printf ("\n");
    
  for (i = 63; i > 0; i--)
    {
      if ((6654654654654654654ll >> i) & 1)
        {
          ply_bitarray_clear (bitarray, i);
          for (i2 = 0; i2 < 64; i2++)
            {
              if (ply_bitarray_lookup (bitarray, i2))
                printf ("1");
              else
                printf ("0"); 
            }
          printf ("\n");
        }
    }
    
  ply_bitarray_free (bitarray);
  
  printf ("bitarray test end\n");
  return 0;
}

#endif
/* vim: set ts=4 sw=4 expandtab autoindent cindent cino={.5s,(0: */
