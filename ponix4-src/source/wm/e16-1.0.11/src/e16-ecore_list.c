/*
 * Copyright (C) 2000-2007 Carsten Haitzler and various contributors (see AUTHORS)
 * Copyright (C) 2006-2010 Kim Woelders
 *
 * Copyright (C) Nathan Ingersoll (author)
 * Copyright (C) Ibukun Olumuyiwa (ewd -> ecore)
 * Copyright (C) Dan Sinclair     (various cleanups)
 * Copyright (C) Kim Woelders     (e16 port/additions)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*
 * Most of this was snatched from e17/libs/ecore/src/lib/ecore/ecore_list.c
 * revision 1.21, and associated header files.
 * The pertinent AUTHORS list is e17/libs/ecore/AUTHORS.
 */
#include <stdlib.h>
#include <string.h>
#include "e16-ecore_list.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

typedef struct _ecore_list_node Ecore_List_Node;

struct _ecore_list_node {
   void               *data;
   struct _ecore_list_node *next;
};

struct _ecore_list {
   Ecore_List_Node    *first;	/* The first node in the list */
   Ecore_List_Node    *last;	/* The last node in the list */
   Ecore_List_Node    *current;	/* The current node in the list */

   Ecore_Free_Cb       free_func;	/* The callback to free data in nodes */

   int                 nodes;	/* The number of nodes in the list */
   int                 index;	/* The position from the front of the
				 * list of current node */
};

/* Adding functions */
static int          _ecore_list_insert(Ecore_List * list,
				       Ecore_List_Node * node);
static int          _ecore_list_append_0(Ecore_List * list,
					 Ecore_List_Node * node);
static int          _ecore_list_prepend_0(Ecore_List * list,
					  Ecore_List_Node * node);

/* Creating and destroying list nodes */
static Ecore_List_Node *_ecore_list_node_new(void);
static int          _ecore_list_node_destroy(Ecore_List_Node * _e_node,
					     Ecore_Free_Cb free_func);

/**
 * @defgroup Ecore_Data_List_Creation_Group List Creation/Destruction Functions
 *
 * Functions that create, initialize and destroy Ecore_Lists.
 */

/**
 * Create and initialize a new list.
 * @return  A new initialized list on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Creation_Group
 */
EAPI Ecore_List    *
ecore_list_new(void)
{
   Ecore_List         *list;

   list = (Ecore_List *) calloc(1, sizeof(Ecore_List));

   return list;
}

/**
 * Free a list and all of it's nodes.
 * @param   list The list to be freed.
 * @ingroup Ecore_Data_List_Creation_Group
 */
EAPI void
ecore_list_destroy(Ecore_List * list)
{
   void               *data;

   if (!list)
      return;

   while (list->first)
     {
	data = ecore_list_first_remove(list);
	if (list->free_func)
	   list->free_func(data);
     }

   free(list);
}

/**
 * Set the function for freeing data.
 * @param  list      The list that will use this function when nodes are
 *                   destroyed.
 * @param  free_func The function that will free the key data.
 * @return @c TRUE on successful set, @c FALSE otherwise.
 */
EAPI int
ecore_list_free_cb_set(Ecore_List * list, Ecore_Free_Cb free_func)
{
   if (!list)
      return FALSE;

   list->free_func = free_func;

   return TRUE;
}

/**
 * Checks the list for any nodes.
 * @param  list  The list to check for nodes
 * @return @c TRUE if no nodes in list, @c FALSE if the list contains nodes
 */
EAPI int
ecore_list_empty_is(Ecore_List * list)
{
   return (list && list->nodes) ? FALSE : TRUE;
}

/**
 * Returns the number of the current node.
 * @param  list The list to return the number of the current node.
 * @return The number of the current node in the list.
 */
EAPI int
ecore_list_index(Ecore_List * list)
{
   return (list) ? list->index : 0;
}

/**
 * Find the number of nodes in the list.
 * @param  list The list to find the number of nodes
 * @return The number of nodes in the list.
 */
EAPI int
ecore_list_count(Ecore_List * list)
{
   return (list) ? list->nodes : 0;
}

/**
@defgroup Ecore_Data_List_Add_Item_Group List Item Adding Functions

Functions that are used to add nodes to an Ecore_List.
*/

/**
 * Append data to the list.
 * @param   list The list.
 * @param   data The data to append.
 * @return  @c FALSE if an error occurs, @c TRUE if appended successfully
 * @ingroup Ecore_Data_List_Add_Item_Group
 */
EAPI int
ecore_list_append(Ecore_List * list, void *data)
{
   int                 ret;
   Ecore_List_Node    *node;

   node = _ecore_list_node_new();
   node->data = data;

   ret = _ecore_list_append_0(list, node);

   return ret;
}

/* For adding items to the end of the list */
static int
_ecore_list_append_0(Ecore_List * list, Ecore_List_Node * end)
{
   if (list->last)
      list->last->next = end;

   list->last = end;

   if (!list->first)
     {
	list->first = end;
	list->index = 0;
	list->current = NULL;
     }

   if (list->index >= list->nodes)
      list->index++;

   list->nodes++;

   return TRUE;
}

/**
 * Prepend data to the beginning of the list.
 * @param  list The list.
 * @param  data The data to prepend.
 * @return @c FALSE if an error occurs, @c TRUE if prepended successfully.
 * @ingroup Ecore_Data_List_Add_Item_Group
 */
EAPI int
ecore_list_prepend(Ecore_List * list, void *data)
{
   int                 ret;
   Ecore_List_Node    *node;

   node = _ecore_list_node_new();
   node->data = data;

   ret = _ecore_list_prepend_0(list, node);

   return ret;
}

/* For adding items to the beginning of the list */
static int
_ecore_list_prepend_0(Ecore_List * list, Ecore_List_Node * start)
{
   /* Put it at the beginning of the list */
   start->next = list->first;

   list->first = start;

   /* If no last node, then the first node is the last node */
   if (!list->last)
      list->last = list->first;

   list->nodes++;
   list->index++;

   return TRUE;
}

/**
 * Insert data in front of the current point in the list.
 * @param   list The list to hold the inserted @p data.
 * @param   data The data to insert into @p list.
 * @return  @c FALSE if there is an error, @c TRUE on success
 * @ingroup Ecore_Data_List_Add_Item_Group
 */
EAPI int
ecore_list_insert(Ecore_List * list, void *data)
{
   int                 ret;
   Ecore_List_Node    *node;

   node = _ecore_list_node_new();
   node->data = data;

   ret = _ecore_list_insert(list, node);

   return ret;
}

/* For adding items in front of the current position in the list */
static int
_ecore_list_insert(Ecore_List * list, Ecore_List_Node * new_node)
{
   /*
    * If the current point is at the beginning of the list, then it's the
    * same as prepending it to the list.
    */
   if (list->current == list->first)
      return _ecore_list_prepend_0(list, new_node);

   if (!list->current)
     {
	int                 ret_value;

	ret_value = _ecore_list_append_0(list, new_node);
	list->current = list->last;

	return ret_value;
     }

   /* Setup the fields of the new node */
   new_node->next = list->current;

   /* And hook the node into the list */
   ecore_list_index_goto(list, ecore_list_index(list) - 1);

   list->current->next = new_node;

   /* Now move the current item to the inserted item */
   list->current = new_node;
   list->index++;
   list->nodes++;

   return TRUE;
}

/**
@defgroup Ecore_Data_List_Remove_Item_Group List Item Removing Functions

Functions that remove nodes from an Ecore_List.
*/

/**
 * Remove the current item from the list.
 * @param   list The list to remove the current item
 * @return  A pointer to the removed data on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Remove_Item_Group
 */
EAPI void          *
ecore_list_remove(Ecore_List * list)
{
   void               *ret = NULL;
   Ecore_List_Node    *old;

   if (ecore_list_empty_is(list))
      return NULL;

   if (!list->current)
      return NULL;

   if (list->current == list->first)
      return ecore_list_first_remove(list);

   if (list->current == list->last)
      return ecore_list_last_remove(list);

   old = list->current;

   ecore_list_index_goto(list, list->index - 1);

   list->current->next = old->next;
   old->next = NULL;
   ret = old->data;
   old->data = NULL;

   ecore_list_next(list);

   _ecore_list_node_destroy(old, NULL);
   list->nodes--;

   return ret;
}

/**
 * Remove and free the data in lists current position.
 * @param   list The list to remove and free the current item.
 * @return  @c TRUE on success, @c FALSE on error
 * @ingroup Ecore_Data_List_Remove_Item_Group
 */
EAPI int
ecore_list_remove_destroy(Ecore_List * list)
{
   void               *data;

   if (!list)
      return FALSE;

   data = ecore_list_remove(list);
   if (list->free_func)
      list->free_func(data);

   return TRUE;
}

/**
 * Remove the first item from the list.
 * @param   list The list to remove the current item
 * @return  Returns a pointer to the removed data on success, @c NULL on
 *          failure.
 * @ingroup Ecore_Data_List_Remove_Item_Group
 */
EAPI void          *
ecore_list_first_remove(Ecore_List * list)
{
   void               *ret = NULL;
   Ecore_List_Node    *old;

   if (ecore_list_empty_is(list))
      return NULL;

   old = list->first;

   list->first = list->first->next;

   if (list->current == old)
      list->current = list->first;
   else
      list->index--;

   if (list->last == old)
      list->last = list->first;

   ret = old->data;
   old->data = NULL;

   _ecore_list_node_destroy(old, NULL);
   list->nodes--;

   return ret;
}

/**
 * Remove the last item from the list.
 * @param   list The list to remove the last node from
 * @return  A pointer to the removed data on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Remove_Item_Group
 */
EAPI void          *
ecore_list_last_remove(Ecore_List * list)
{
   void               *ret = NULL;
   Ecore_List_Node    *old, *prev;

   if (ecore_list_empty_is(list))
      return NULL;

   old = list->last;
   if (list->current == old)
      list->current = NULL;

   if (list->first == old)
      list->first = NULL;
   for (prev = list->first; prev && prev->next != old; prev = prev->next)
      ;
   list->last = prev;
   if (prev)
      prev->next = NULL;

   old->next = NULL;
   ret = old->data;
   old->data = NULL;

   _ecore_list_node_destroy(old, NULL);
   list->nodes--;

   return ret;
}

/**
@defgroup Ecore_Data_List_Traverse_Group List Traversal Functions

Functions that can be used to traverse an Ecore_List.
*/

/**
 * Make the current item the item with the given index number.
 * @param   list  The list.
 * @param   indx  The position to move the current item.
 * @return  A pointer to new current item on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Traverse_Group
 */
EAPI void          *
ecore_list_index_goto(Ecore_List * list, int indx)
{
   int                 i;

   if (ecore_list_empty_is(list))
      return NULL;

   if (indx > ecore_list_count(list) || indx < 0)
      return NULL;

   ecore_list_first_goto(list);

   for (i = 0; i < indx && ecore_list_next(list); i++)
      ;

   if (i >= list->nodes)
      return NULL;

   list->index = i;

   return list->current->data;
}

/**
 * Make the current item the node that contains @p data.
 * @param   list The list.
 * @param   data The data to find.
 * @return  A pointer to @p data on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Traverse_Group
 */
EAPI void          *
ecore_list_goto(Ecore_List * list, const void *data)
{
   int                 indx;
   Ecore_List_Node    *node;

   if (!list)
      return NULL;

   indx = 0;

   node = list->first;
   while (node && node->data)
     {
	Ecore_List_Node    *next;

	if (node->data == data)
	   break;

	next = node->next;

	node = next;

	indx++;
     }

   if (!node)
      return NULL;

   list->current = node;
   list->index = indx;

   return list->current->data;
}

/**
 * Make the current item the first item in the list
 * @param   list The list.
 * @return  A pointer to the first item on success, @c NULL on failure
 * @ingroup Ecore_Data_List_Traverse_Group
 */
EAPI void          *
ecore_list_first_goto(Ecore_List * list)
{
   if (!list || !list->first)
      return NULL;

   list->current = list->first;
   list->index = 0;

   return list->current->data;
}

/**
 * Make the current item the last item in the list.
 * @param   list The list.
 * @return  A pointer to the last item on success, @c NULL on failure.
 * @ingroup Ecore_Data_List_Traverse_Group
 */
EAPI void          *
ecore_list_last_goto(Ecore_List * list)
{
   if (!list || !list->last)
      return NULL;

   list->current = list->last;
   list->index = (list->nodes - 1);

   return list->current->data;
}

/**
 * Retrieve the data pointed to by the current item in @p list.
 * @param  list The list.
 * @return Returns the data at current position, can be @c NULL.
 */
EAPI void          *
ecore_list_current(Ecore_List * list)
{
   return (list && list->current) ? list->current->data : NULL;
}

/**
 * Retrieve the data pointed to by the current item, and make the next item
 * the current item.
 * @param   list The list to retrieve data from.
 * @return  The current item in the list on success, @c NULL on failure.
 */
EAPI void          *
ecore_list_next(Ecore_List * list)
{
   void               *data;
   Ecore_List_Node    *ret;
   Ecore_List_Node    *next;

   if (!list || !list->current)
      return NULL;

   ret = list->current;
   next = list->current->next;

   list->current = next;
   list->index++;

   data = ret->data;

   return data;
}

/**
 * Remove all nodes from @p list.
 * @param  list The list.
 * @return Returns @c TRUE on success, @c FALSE on error.
 * @note The data for each item on the list is not freed by
 *       @c ecore_list_clear().
 */
EAPI int
ecore_list_clear(Ecore_List * list)
{
   while (!ecore_list_empty_is(list))
      ecore_list_first_remove(list);

   return TRUE;
}

/**
 * Execute function for each node in @p list.
 * @param   list     The list.
 * @param   function The function to pass each node from @p list to.
 * @return  Returns @c TRUE on success, @c FALSE on failure.
 * @ingroup Ecore_Data_List_Traverse_Group
 */
EAPI int
ecore_list_for_each(Ecore_List * list, Ecore_For_Each function, void *user_data)
{
   Ecore_List_Node    *node, *next;

   if (!list || !function)
      return FALSE;

   for (node = list->first; node; node = next)
     {
	next = node->next;
	function(node->data, user_data);
     }

   return TRUE;
}

/**
 * Find data in @p list using the compare function @p func
 * @param list      The list.
 * @param function  The function to test each node of @p list with
 * @param user_data Data to match against (used by @p function)
 * @return the first matching data node, or NULL if none match
 */
EAPI void          *
ecore_list_find(Ecore_List * list, Ecore_Compare_Cb function,
		const void *user_data)
{
   Ecore_List_Node    *node, *next;

   if (!list || !function)
      return NULL;

   for (node = list->first; node; node = next)
     {
	next = node->next;
	if (!function(node->data, user_data))
	   return node->data;
     }

   return NULL;
}

/**
@defgroup Ecore_Data_List_Node_Group List Node Functions

Functions that are used in the creation, maintenance and destruction of
Ecore_List nodes.
*/

/**
 * Allocates and initializes a new list node.
 * @return  A new Ecore_List_Node on success, @c NULL otherwise.
 * @ingroup Ecore_Data_List_Node_Group
 */
static Ecore_List_Node *
_ecore_list_node_new(void)
{
   Ecore_List_Node    *new_node;

   new_node = (Ecore_List_Node *) calloc(1, sizeof(Ecore_List_Node));

   return new_node;
}

/**
 * Calls the function to free the data and the node.
 * @param   node      Node to destroy.
 * @param   free_func Function to call if @p node points to data to free.
 * @return  @c TRUE.
 * @ingroup Ecore_Data_List_Node_Group
 */
EAPI int
_ecore_list_node_destroy(Ecore_List_Node * node, Ecore_Free_Cb free_func)
{
   if (free_func && node->data)
      free_func(node->data);

   free(node);

   return TRUE;
}

/*
 * E16 additions
 */

EAPI void          *
ecore_list_node_remove(Ecore_List * list, void *_data)
{
   void               *data;

   data = ecore_list_goto(list, _data);
   if (data)
      ecore_list_remove(list);

   return data;
}

EAPI void         **
ecore_list_items_get(Ecore_List * list, int *pnum)
{
   void              **lst, *b;
   int                 i, num;

   *pnum = 0;
   num = ecore_list_count(list);
   if (num <= 0)
      return NULL;

   lst = (void **)malloc(num * sizeof(void *));
   if (!lst)
      return NULL;

   for (i = 0, ecore_list_first_goto(list); (b = ecore_list_next(list));)
      lst[i++] = b;

   *pnum = num;
   return lst;
}
