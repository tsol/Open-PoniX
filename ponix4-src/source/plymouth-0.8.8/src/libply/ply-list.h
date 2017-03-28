/* ply-list.h - linked list implementation
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
 * Written by: Ray Strode <rstrode@redhat.com>
 */
#ifndef PLY_LIST_H
#define PLY_LIST_H

typedef struct _ply_list_node ply_list_node_t;
typedef struct _ply_list ply_list_t;
typedef int (ply_list_compare_func_t) (void *elementa, void *elementb);

#ifndef PLY_HIDE_FUNCTION_DECLARATIONS
ply_list_t *ply_list_new (void);
void ply_list_free (ply_list_t *list);
int ply_list_get_length (ply_list_t *list);
ply_list_node_t *ply_list_find_node (ply_list_t *list,
                                     void       *data);
ply_list_node_t *ply_list_insert_data (ply_list_t *list,
		                       void       *data,
		                       ply_list_node_t *node_before);
ply_list_node_t *ply_list_append_data (ply_list_t *list,
                                       void       *data);
ply_list_node_t *ply_list_prepend_data (ply_list_t *list,
                                        void       *data);
void ply_list_remove_data (ply_list_t *list,
                          void        *data);
void ply_list_remove_node (ply_list_t      *list,
                           ply_list_node_t *node);
void ply_list_remove_all_nodes (ply_list_t *list);
void ply_list_sort (ply_list_t              *list,
                    ply_list_compare_func_t *compare);
void ply_list_sort_stable (ply_list_t              *list,
                           ply_list_compare_func_t *compare);
ply_list_node_t *ply_list_get_first_node (ply_list_t *list);
ply_list_node_t *ply_list_get_last_node (ply_list_t *list);
ply_list_node_t *ply_list_get_nth_node (ply_list_t *list,
                                        int         index);
ply_list_node_t *ply_list_get_next_node (ply_list_t      *list,
                                         ply_list_node_t *node);
void *ply_list_node_get_data (ply_list_node_t *node);
#endif

#endif /* PLY_LIST_H */
