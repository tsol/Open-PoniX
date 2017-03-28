/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2008-2010 Kim Woelders
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
#include "borders.h"
#include "ewins.h"
#include "groups.h"
#include "util.h"
#include <ctype.h>
#include <string.h>

EWin               *
EwinFindByPtr(const EWin * ewin)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (ewin == ewins[i])
	   return ewins[i];
     }
   return NULL;
}

EWin               *
EwinFindByClient(Window win)
{
   EWin               *const *ewins;
   int                 i, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (win == EwinGetClientXwin(ewins[i]))
	   return ewins[i];
     }
   return NULL;
}

static EWin        *
EwinFindByChildren(Window win)
{
   EWin               *const *ewins;
   int                 i, j, num;

   ewins = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if ((win == EwinGetClientXwin(ewins[i])) ||
	    (win == EwinGetContainerXwin(ewins[i])))
	  {
	     return ewins[i];
	  }
	else
	  {
	     for (j = 0; j < ewins[i]->border->num_winparts; j++)
		if (win == WinGetXwin(ewins[i]->bits[j].win))
		  {
		     return ewins[i];
		  }
	  }
     }
   return NULL;
}

EWin              **
EwinsFindByExpr(const char *match, int *pnum, int *pflags)
{
   EWin               *ewin, **lst;
   EWin               *const *ewins;
   int                 type;
   int                 i, num, len, nfound, match_one, flags;

   if (pnum)
      *pnum = 0;

   if (!match || !match[0])
      return NULL;

   ewin = NULL;
   flags = 0;

   if (!strcmp(match, "*") || !strcmp(match, "=") || !strcmp(match, "current"))
     {
	ewin = GetContextEwin();
	if (!ewin)
	   ewin = GetFocusEwin();
	if (match[0] == '=')
	   flags = 1;		/* Nogroup */
	goto do_one;
     }

   if (isdigit(match[0]))
     {
	unsigned int        win;

	sscanf(match, "%x", &win);
	ewin = EwinFindByChildren(win);
	goto do_one;
     }

   match_one = 1;
   if (!strcmp(match, "all"))
     {
	type = 'a';
	match_one = 0;
	flags = 1;		/* Nogroup */
     }
   else if (match[0] == '=')
     {
	type = 's';
	match++;
	flags = 1;		/* Nogroup */
     }
   else if (strchr(match, '*'))
     {
	type = 'w';
	match_one = 0;
	flags = 1;		/* Nogroup */
     }
   else
     {
	type = 's';
     }

   len = strlen(match);
   if (len <= 0)
      return NULL;

   ewins = EwinListGetAll(&num);
   if (!ewins)
      return NULL;

   nfound = 0;
   lst = NULL;
   for (i = 0; i < num; i++)
     {
	ewin = ewins[i];

	if (type == 'a')	/* All */
	  {
	  }
	else if (type == 'w')	/* Wildcard */
	  {
	     if (!matchregexp(match, EwinGetIcccmName(ewin)))
		continue;
	  }
	else			/* Match name (substring) */
	  {
	     const char         *name;

	     name = EwinGetIcccmName(ewin);
	     if (!name)
		continue;
	     if (!Estrcasestr(name, match))
		continue;
	  }
	nfound++;
	lst = EREALLOC(EWin *, lst, nfound);
	lst[nfound - 1] = ewin;
	if (match_one)
	   break;
     }
   goto done;

 do_one:
   if (!ewin)
      return NULL;
   nfound = 1;
   lst = EMALLOC(EWin *, 1);
   if (!lst)
      return NULL;
   lst[0] = ewin;

 done:
   if (pnum)
      *pnum = nfound;
   if (pflags)
      *pflags = flags;
   return lst;
}

EWin               *
EwinFindByExpr(const char *match)
{
   EWin               *ewin, **lst;

   lst = EwinsFindByExpr(match, NULL, NULL);
   if (!lst)
      return NULL;
   ewin = lst[0];
   Efree(lst);
   return ewin;
}

EWin              **
ListWinGroupMembersForEwin(const EWin * ewin, int action, char nogroup,
			   int *pnum)
{

   EWin              **gwins, *ew;
   EWin               *const *ewins;
   Group              *grp;
   int                 i, num, gwcnt;

   if (!ewin)
     {
	*pnum = 0;
	return NULL;
     }

   gwcnt = 0;
   gwins = NULL;

   if (nogroup || ewin->num_groups <= 0)
      goto done;

   ewins = EwinListGetAll(&num);
   if (!ewins)			/* Should not be possible */
      goto done;

   /* Loop through window stack, bottom up */
   for (i = num - 1; i >= 0; i--)
     {
	ew = ewins[i];

	if (ew == ewin)
	   goto do_add;

	/* To get consistent behaviour, limit groups to a single desktop for now: */
	if (EoGetDesk(ew) != EoGetDesk(ewin))
	   continue;

	grp = EwinsInGroup(ewin, ew);
	if (!grp)
	   continue;

	switch (action)
	  {
	  case GROUP_ACTION_SET_WINDOW_BORDER:
	     if (!grp->cfg.set_border)
		continue;
	     break;
	  case GROUP_ACTION_ICONIFY:
	     if (!grp->cfg.iconify)
		continue;
	     break;
	  case GROUP_ACTION_MOVE:
	     if (!grp->cfg.move)
		continue;
	     break;
	  case GROUP_ACTION_STACKING:
	     if (!grp->cfg.raise)
		continue;
	     break;
	  case GROUP_ACTION_STICK:
	     if (!grp->cfg.stick)
		continue;
	     break;
	  case GROUP_ACTION_SHADE:
	     if (!grp->cfg.shade)
		continue;
	     break;
	  case GROUP_ACTION_KILL:
	     if (!grp->cfg.kill)
		continue;
	     break;
	  default:
	     break;
	  }

      do_add:
	gwins = EREALLOC(EWin *, gwins, gwcnt + 1);
	gwins[gwcnt] = ew;
	gwcnt++;
     }

 done:
   if (!gwins)
     {
	gwins = EMALLOC(EWin *, 1);
	gwins[0] = (EWin *) ewin;
	gwcnt = 1;
     }
   *pnum = gwcnt;
   return gwins;
}
