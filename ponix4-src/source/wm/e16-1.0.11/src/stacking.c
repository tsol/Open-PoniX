/*
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "E.h"
#include "desktops.h"
#include "eobj.h"
#include "ewins.h"

#define ENABLE_DEBUG_STACKING 1

#define EobjGetCwin(p) \
    ((p->type == EOBJ_TYPE_EWIN) ? EwinGetClientXwin(((EWin*)(p))) : None)

typedef struct _eobjlist EobjList;

struct _eobjlist {
   const char         *name;
   int                 nalloc;
   int                 nwins;
   EObj              **list;
   char                layered;
   char                type;
};

static int          EobjListRaise(EobjList * ewl, EObj * eo, int test);
static int          EobjListLower(EobjList * ewl, EObj * eo, int test);

#if ENABLE_DEBUG_STACKING
static void
EobjListShow(const char *txt, EobjList * ewl)
{
   int                 i;
   EObj               *eo;

   if (!EDebug(EDBUG_TYPE_STACKING))
      return;

   Eprintf("%s-%s:\n", ewl->name, txt);
   for (i = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	Eprintf(" %2d: %#10lx %#10lx %d %d %s\n", i, EobjGetXwin(eo),
		EobjGetCwin(eo), eo->desk->num, eo->ilayer, EobjGetName(eo));
     }
}
#else
#define EobjListShow(txt, ewl)
#endif

static int
EobjListGetIndex(EobjList * ewl, EObj * eo)
{
   int                 i;

   for (i = 0; i < ewl->nwins; i++)
      if (ewl->list[i] == eo)
	 return i;

   return -1;
}

static void
EobjListAdd(EobjList * ewl, EObj * eo, int ontop)
{
   int                 i;

   /* Quit if already in list */
   i = EobjListGetIndex(ewl, eo);
   if (i >= 0)
      return;

   if (ewl->nwins >= ewl->nalloc)
     {
	ewl->nalloc += 16;
	ewl->list = EREALLOC(EObj *, ewl->list, ewl->nalloc);
     }

   if (ewl->layered)
     {
	/* The simple way for now (add, raise/lower) */
	if (ontop)
	  {
	     ewl->list[ewl->nwins] = eo;
	     ewl->nwins++;
	     EobjListRaise(ewl, eo, 0);
	  }
	else
	  {
	     memmove(ewl->list + 1, ewl->list, ewl->nwins * sizeof(EObj *));
	     ewl->list[0] = eo;
	     ewl->nwins++;
	     EobjListLower(ewl, eo, 0);
	  }
	if (eo->stacked == 0)
	   DeskSetDirtyStack(eo->desk, eo);
     }
   else
     {
	if (ontop)
	  {
	     memmove(ewl->list + 1, ewl->list, ewl->nwins * sizeof(EObj *));
	     ewl->list[0] = eo;
	  }
	else
	  {
	     ewl->list[ewl->nwins] = eo;
	  }
	ewl->nwins++;
     }

   EobjListShow("EobjListAdd", ewl);
}

static void
EobjListDel(EobjList * ewl, EObj * eo)
{
   int                 i, n;

   /* Quit if not in list */
   i = EobjListGetIndex(ewl, eo);
   if (i < 0)
      return;

   ewl->nwins--;
   n = ewl->nwins - i;
   if (n > 0)
     {
	memmove(ewl->list + i, ewl->list + i + 1, n * sizeof(EObj *));
     }
   else if (ewl->nwins <= 0)
     {
	/* Enables autocleanup at shutdown, if ever implemented */
	Efree(ewl->list);
	ewl->list = NULL;
	ewl->nalloc = 0;
     }

   EobjListShow("EobjListDel", ewl);
}

static int
EobjListLower(EobjList * ewl, EObj * eo, int test)
{
   int                 i, j, n;

   /* Quit if not in list */
   i = EobjListGetIndex(ewl, eo);
   if (i < 0)
      return 0;

   j = ewl->nwins - 1;
   if (ewl->layered)
     {
	/* Take the layer into account */
	for (; j >= 0; j--)
	   if (i != j && eo->ilayer <= ewl->list[j]->ilayer)
	      break;
	if (j < i)
	   j++;
     }

   n = j - i;
   if (test)
      return n;

   if (n > 0)
     {
	memmove(ewl->list + i, ewl->list + i + 1, n * sizeof(EObj *));
	ewl->list[j] = eo;
	if (ewl->layered && eo->stacked > 0)
	   DeskSetDirtyStack(eo->desk, eo);
     }
   else if (n < 0)
     {
	memmove(ewl->list + j + 1, ewl->list + j, -n * sizeof(EObj *));
	ewl->list[j] = eo;
	if (ewl->layered && eo->stacked > 0)
	   DeskSetDirtyStack(eo->desk, eo);
     }

   EobjListShow("EobjListLower", ewl);
   return n;
}

static int
EobjListRaise(EobjList * ewl, EObj * eo, int test)
{
   int                 i, j, n;

   /* Quit if not in list */
   i = EobjListGetIndex(ewl, eo);
   if (i < 0)
      return 0;

   j = 0;
   if (ewl->layered)
     {
	/* Take the layer into account */
	for (; j < ewl->nwins; j++)
	   if (j != i && eo->ilayer >= ewl->list[j]->ilayer)
	      break;
	if (j > i)
	   j--;
     }

   n = j - i;
   if (test)
      return n;

   if (n > 0)
     {
	memmove(ewl->list + i, ewl->list + i + 1, n * sizeof(EObj *));
	ewl->list[j] = eo;
	if (ewl->layered && eo->stacked > 0)
	   DeskSetDirtyStack(eo->desk, eo);
     }
   else if (n < 0)
     {
	memmove(ewl->list + j + 1, ewl->list + j, -n * sizeof(EObj *));
	ewl->list[j] = eo;
	if (ewl->layered && eo->stacked > 0)
	   DeskSetDirtyStack(eo->desk, eo);
     }

   EobjListShow("EobjListRaise", ewl);
   return n;
}

static EObj        *
EobjListFind(const EobjList * ewl, Window win)
{
   int                 i;

   for (i = 0; i < ewl->nwins; i++)
      if (EobjGetXwin(ewl->list[i]) == win)
	 return ewl->list[i];

   return NULL;
}

static int
EobjListTypeCount(const EobjList * ewl, int type)
{
   int                 i, n;

   for (i = n = 0; i < ewl->nwins; i++)
      if (ewl->list[i]->type == type)
	 n++;

   return n;
}

/*
 * The global object/client lists
 */
static EobjList     EwinListStack = { "Stack", 0, 0, NULL, 1, 0 };
static EobjList     EwinListFocus = { "Focus", 0, 0, NULL, 0, 1 };
static EobjList     EwinListOrder = { "Order", 0, 0, NULL, 0, 2 };

static EObj        *const *
EobjListGet(EobjList * ewl, int *num)
{
   *num = ewl->nwins;
   return ewl->list;
}

int
EobjListStackCheck(EObj * eo)
{
   return EobjListGetIndex(&EwinListStack, eo);
}

EObj               *
EobjListStackFind(Window win)
{
   return EobjListFind(&EwinListStack, win);
}

EObj               *const *
EobjListStackGet(int *num)
{
   return EobjListGet(&EwinListStack, num);
}

void
EobjListStackAdd(EObj * eo, int ontop)
{
   EobjListAdd(&EwinListStack, eo, ontop);
}

void
EobjListStackDel(EObj * eo)
{
   EobjListDel(&EwinListStack, eo);
}

int
EobjListStackRaise(EObj * eo, int test)
{
   return EobjListRaise(&EwinListStack, eo, test);
}

int
EobjListStackLower(EObj * eo, int test)
{
   return EobjListLower(&EwinListStack, eo, test);
}

void
EobjListFocusAdd(EObj * eo, int ontop)
{
   EobjListAdd(&EwinListFocus, eo, ontop);
}

void
EobjListFocusDel(EObj * eo)
{
   EobjListDel(&EwinListFocus, eo);
}

int
EobjListFocusRaise(EObj * eo)
{
   return EobjListRaise(&EwinListFocus, eo, 0);
}

EWin               *const *
EwinListStackGet(int *num)
{
   static EWin       **lst = NULL;
   static int          nalloc = 0;
   const EobjList     *ewl;
   int                 i, j, newins;
   EObj               *eo;

   ewl = &EwinListStack;
   newins = EobjListTypeCount(ewl, EOBJ_TYPE_EWIN);
   if (nalloc < newins)
     {
	nalloc = (newins + 16) & ~0xf;	/* 16 at the time */
	lst = EREALLOC(EWin *, lst, nalloc);
     }

   for (i = j = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	if (eo->type != EOBJ_TYPE_EWIN)
	   continue;

	lst[j++] = (EWin *) eo;
     }

   *num = j;
   return lst;
}

EWin               *const *
EwinListFocusGet(int *num)
{
   return (EWin * const *)EobjListGet(&EwinListFocus, num);
}

EWin               *const *
EwinListGetForDesk(int *num, Desk * dsk)
{
   static EWin       **lst = NULL;
   static int          nalloc = 0;
   const EobjList     *ewl;
   int                 i, j, newins;
   EObj               *eo;

   ewl = &EwinListStack;
   newins = EobjListTypeCount(ewl, EOBJ_TYPE_EWIN);
   /* Too many - who cares. */
   if (nalloc < newins)
     {
	nalloc = (newins + 16) & ~0xf;	/* 16 at the time */
	lst = EREALLOC(EWin *, lst, nalloc);
     }

   for (i = j = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	if (eo->type != EOBJ_TYPE_EWIN || eo->desk != dsk)
	   continue;

	lst[j++] = (EWin *) eo;
     }

   *num = j;
   return lst;
}

EObj               *const *
EobjListStackGetForDesk(int *num, Desk * dsk)
{
   static EObj       **lst = NULL;
   static int          nalloc = 0;
   const EobjList     *ewl;
   int                 i, j;
   EObj               *eo;

   ewl = &EwinListStack;

   /* Too many - who cares. */
   if (nalloc < ewl->nwins)
     {
	nalloc = (ewl->nwins + 16) & ~0xf;	/* 16 at the time */
	lst = EREALLOC(EObj *, lst, nalloc);
     }

   for (i = j = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	if (eo->desk != dsk)
	   continue;

	lst[j++] = eo;
     }

   *num = j;
   return lst;
}

#if 0				/* Unused */
EWin               *
EwinListStackGetTop(void)
{
   const EobjList     *ewl;
   int                 i;
   EObj               *eo;

   ewl = &EwinListStack;

   for (i = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	if (eo->type == EOBJ_TYPE_EWIN)
	   return (EWin *) eo;
     }

   return NULL;
}
#endif

int
EwinListStackIsRaised(const EWin * ewin)
{
   const EobjList     *ewl;
   int                 i;
   const EObj         *eo, *eox;

   ewl = &EwinListStack;
   eox = EoObj(ewin);

   for (i = 0; i < ewl->nwins; i++)
     {
	eo = ewl->list[i];
	if (eo->type != EOBJ_TYPE_EWIN)
	   continue;
	if (eo->desk != eox->desk)
	   continue;
	if (eo->ilayer > eox->ilayer)
	   continue;
	if (EwinGetTransientFor((EWin *) eo) == EwinGetClientXwin(ewin))
	   continue;
	return eo == eox;
     }

   return 1;			/* It should be impossible to get here */
}

void
EobjListOrderAdd(EObj * eo)
{
   EobjListAdd(&EwinListOrder, eo, 0);
}

void
EobjListOrderDel(EObj * eo)
{
   EobjListDel(&EwinListOrder, eo);
}

EWin               *const *
EwinListOrderGet(int *num)
{
   return (EWin * const *)EobjListGet(&EwinListOrder, num);
}
