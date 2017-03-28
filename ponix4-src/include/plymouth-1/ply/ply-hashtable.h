/* ply-hashtable.h - hash table implementation
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
#ifndef PLY_HASHTABLE_H
#define PLY_HASHTABLE_H

typedef struct _ply_hashtable ply_hashtable_t;

typedef int (ply_hashtable_compare_func_t) (void *elementa,
                                            void *elementb);
typedef unsigned int (ply_hashtable_hash_func_t) (void *element);
typedef void (ply_hashtable_foreach_func_t) (void *key,
                                             void *data,
                                             void *user_data);


#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
int ply_hashtable_direct_compare (void *elementa,
                              	  void *elementb);
unsigned int ply_hashtable_direct_hash (void *element);
unsigned int ply_hashtable_string_hash (void *element);
int ply_hashtable_string_compare (void *elementa,
                                  void *elementb);
ply_hashtable_t *ply_hashtable_new (ply_hashtable_hash_func_t    *hash_func,
                                    ply_hashtable_compare_func_t *compare_func);
void ply_hashtable_free (ply_hashtable_t *hashtable);
void ply_hashtable_resize (ply_hashtable_t *hashtable);
void ply_hashtable_insert (ply_hashtable_t *hashtable,
                    	   void            *key,
                    	   void            *data);
void *ply_hashtable_remove (ply_hashtable_t *hashtable,
                    		void            *key);
void *ply_hashtable_lookup (ply_hashtable_t *hashtable,
                    		void            *key);
int ply_hashtable_lookup_full (ply_hashtable_t *hashtable,
                        	   void            *key,
                        	   void           **reply_key,
                        	   void           **reply_data);
void ply_hashtable_foreach (ply_hashtable_t              *hashtable,
							ply_hashtable_foreach_func_t  func,
							void                         *user_data);
#endif

#endif /* PLY_HASHTABLE_H */
