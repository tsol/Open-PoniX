/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
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
#ifndef _GROUPS_H_
#define _GROUPS_H_

#define GROUP_SELECT_ALL             0
#define GROUP_SELECT_EWIN_ONLY       1
#define GROUP_SELECT_ALL_EXCEPT_EWIN 2

/* For window group listing */
#define GROUP_ACTION_ANY                     0
#define GROUP_ACTION_MOVE                    1
#define GROUP_ACTION_STACKING                2
#define GROUP_ACTION_KILL                    3
#define GROUP_ACTION_STICK                   4
#define GROUP_ACTION_ICONIFY                 5
#define GROUP_ACTION_SHADE                   6
#define GROUP_ACTION_SET_WINDOW_BORDER       7

typedef struct _groupconfig {
   char                iconify;
   char                kill;
   char                move;
   char                raise;
   char                set_border;
   char                shade;
   char                stick;
} GroupConfig;

struct _group {
   int                 index;
   EWin              **members;
   int                 num_members;
   GroupConfig         cfg;
   char                save;	/* Used in snapshot - must save */
};

/* finders.c */
EWin              **ListWinGroupMembersForEwin(const EWin * ewin, int action,
					       char nogroup, int *num);

/* groups.c */
Group              *const *EwinGetGroups(const EWin * ewin, int *num);
Group              *EwinsInGroup(const EWin * ewin1, const EWin * ewin2);
void                GroupsEwinAdd(EWin * ewin, const int *pgid, int ngid);
void                GroupsEwinRemove(EWin * ewin);
void                GroupsLoad(void);
void                GroupsSave(void);
Group             **GroupsGetList(int *pnum);
int                 GroupsGetSwapmove(void);
void                GroupRemember(int gid);

#endif /* _GROUPS_H_ */
