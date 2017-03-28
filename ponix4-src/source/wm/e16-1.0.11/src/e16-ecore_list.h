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
#ifndef _E16_ECORE_LIST_H_
#define _E16_ECORE_LIST_H_

#undef EAPI
#define EAPI

typedef struct _ecore_list Ecore_List;

typedef int         (*Ecore_Compare_Cb) (const void *data, const void *match);
typedef void        (*Ecore_For_Each) (void *value, void *user_data);
typedef void        (*Ecore_Free_Cb) (void *data);

/* Creating and initializing new list structures */
EAPI Ecore_List    *ecore_list_new(void);

/* Adding items to the list */
EAPI int            ecore_list_append(Ecore_List * list, void *_data);
EAPI int            ecore_list_prepend(Ecore_List * list, void *_data);
EAPI int            ecore_list_insert(Ecore_List * list, void *_data);

/* Removing items from the list */
EAPI int            ecore_list_remove_destroy(Ecore_List * list);
EAPI void          *ecore_list_remove(Ecore_List * list);
EAPI void          *ecore_list_first_remove(Ecore_List * list);
EAPI void          *ecore_list_last_remove(Ecore_List * list);

/* Retrieve the current position in the list */
EAPI void          *ecore_list_current(Ecore_List * list);
EAPI int            ecore_list_index(Ecore_List * list);
EAPI int            ecore_list_count(Ecore_List * list);

/* Traversing the list */
EAPI int            ecore_list_for_each(Ecore_List * list,
					Ecore_For_Each function,
					void *user_data);
EAPI void          *ecore_list_first_goto(Ecore_List * list);
EAPI void          *ecore_list_last_goto(Ecore_List * list);
EAPI void          *ecore_list_index_goto(Ecore_List * list, int index);
EAPI void          *ecore_list_goto(Ecore_List * list, const void *_data);

/* Traversing the list and returning data */
EAPI void          *ecore_list_next(Ecore_List * list);
EAPI void          *ecore_list_find(Ecore_List * list,
				    Ecore_Compare_Cb function,
				    const void *match);

/* Check to see if there is any data in the list */
EAPI int            ecore_list_empty_is(Ecore_List * list);

/* Remove every node in the list without freeing the list itself */
EAPI int            ecore_list_clear(Ecore_List * list);

/* Free the list and it's contents */
EAPI void           ecore_list_destroy(Ecore_List * list);

EAPI int            ecore_list_free_cb_set(Ecore_List * list,
					   Ecore_Free_Cb free_func);

/* e16 additions */
#if __cplusplus
#define ECORE_LIST_FOR_EACH(list, p) \
   for (ecore_list_first_goto(list); (p = (typeof(p))ecore_list_next(list)) != NULL;)
#else
#define ECORE_LIST_FOR_EACH(list, p) \
   for (ecore_list_first_goto(list); (p = ecore_list_next(list)) != NULL;)
#endif

EAPI void          *ecore_list_node_remove(Ecore_List * list, void *_data);
EAPI void         **ecore_list_items_get(Ecore_List * list, int *pnum);

#endif /* _E16_ECORE_LIST_H_ */
