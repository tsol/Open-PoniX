/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2012 Kim Woelders
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
#include "dialog.h"
#include "e16-ecore_list.h"
#include "emodule.h"
#include "ewins.h"
#include "groups.h"
#include "settings.h"
#include "snaps.h"

#define DEBUG_GROUPS 0
#if DEBUG_GROUPS
#define Dprintf(fmt, ...) Eprintf("%s: " fmt, __func__, __VA_ARGS__)
#else
#define Dprintf(fmt...)
#endif

#define USE_GROUP_SHOWHIDE 1	/* Enable group borders */

#define SET_OFF    0
#define SET_ON     1
#define SET_TOGGLE 2

static Ecore_List  *group_list = NULL;

static struct {
   GroupConfig         dflt;
   char                swapmove;
} Conf_groups;

static struct {
   Group              *current;
} Mode_groups;

static void         AddEwinToGroup(EWin * ewin, Group * g);
static void         RemoveEwinFromGroup(EWin * ewin, Group * g);

int
GroupsGetSwapmove(void)
{
   return Conf_groups.swapmove;
}

static Group       *
GroupCreate(int gid)
{
   Group              *g;

   g = ECALLOC(Group, 1);
   if (!g)
      return NULL;

   if (!group_list)
      group_list = ecore_list_new();
   ecore_list_append(group_list, g);

   if (gid == -1)
     {
	/* Create new group id */
	/* ... using us time. Should really be checked for uniqueness. */
	g->index = (int)GetTimeUs();
     }
   else
     {
	/* Use given group id */
	g->index = gid;
     }
   g->cfg.iconify = Conf_groups.dflt.iconify;
   g->cfg.kill = Conf_groups.dflt.kill;
   g->cfg.move = Conf_groups.dflt.move;
   g->cfg.raise = Conf_groups.dflt.raise;
   g->cfg.set_border = Conf_groups.dflt.set_border;
   g->cfg.stick = Conf_groups.dflt.stick;
   g->cfg.shade = Conf_groups.dflt.shade;

   Dprintf("grp=%p gid=%d\n", g, g->index);
   return g;
}

static void
GroupDestroy(Group * g)
{
   if (!g)
      return;

   Dprintf("grp=%p gid=%d\n", g, g->index);
   ecore_list_node_remove(group_list, g);

   if (g == Mode_groups.current)
      Mode_groups.current = NULL;
   Efree(g->members);

   Efree(g);
}

static int
GroupMatchId(const void *data, const void *match)
{
   return ((const Group *)data)->index != PTR2INT(match);
}

static Group       *
GroupFind(int gid)
{
   return (Group *) ecore_list_find(group_list, GroupMatchId, INT2PTR(gid));
}

void
GroupRemember(int gid)
{
   Group              *g;

   g = GroupFind(gid);
   if (!g)
      return;

   g->save = 1;
}

static Group       *
GroupFind2(const char *groupid)
{
   int                 gid;

   if (groupid[0] == '*' || groupid[0] == '\0')
      return Mode_groups.current;

   gid = -1;
   sscanf(groupid, "%d", &gid);
   if (gid <= 0)
      return NULL;

   return GroupFind(gid);
}

#if ENABLE_DIALOGS
static void
CopyGroupConfig(GroupConfig * src, GroupConfig * dest)
{
   if (!(src && dest))
      return;

   memcpy(dest, src, sizeof(GroupConfig));
}
#endif /* ENABLE_DIALOGS */

static void
BreakWindowGroup(EWin * ewin, Group * g)
{
   int                 i, j;
   EWin               *ewin2;
   Group              *g2;

   Dprintf("ewin=%p group=%p gid=%d\n", ewin, g, g->index);
   if (!ewin || !ewin->groups)
      return;

   for (j = 0; j < ewin->num_groups; j++)
     {
	g2 = ewin->groups[j];
	if (g && g != g2)
	   continue;

	for (i = 0; i < g2->num_members; i++)
	  {
	     ewin2 = g2->members[0];
	     RemoveEwinFromGroup(ewin2, g2);
	     SnapshotEwinUpdate(ewin2, SNAP_USE_GROUPS);
	  }
     }
}

static void
BuildWindowGroup(EWin ** ewins, int num, int gid)
{
   int                 i;
   Group              *group;

   Mode_groups.current = group = GroupCreate(gid);

   for (i = 0; i < num; i++)
      AddEwinToGroup(ewins[i], group);
}

Group             **
GroupsGetList(int *pnum)
{
   return (Group **) ecore_list_items_get(group_list, pnum);
}

Group              *const *
EwinGetGroups(const EWin * ewin, int *num)
{
   *num = ewin->num_groups;
   return ewin->groups;
}

#if ENABLE_DIALOGS
static Group      **
ListWinGroups(const EWin * ewin, char group_select, int *num)
{
   Group             **groups = NULL;
   Group             **groups2 = NULL;
   int                 i, j, killed = 0;

   switch (group_select)
     {
     case GROUP_SELECT_EWIN_ONLY:
	groups = EMALLOC(Group *, ewin->num_groups);
	if (!groups)
	   break;
	memcpy(groups, ewin->groups, sizeof(Group *) * ewin->num_groups);
	*num = ewin->num_groups;
	break;
     case GROUP_SELECT_ALL_EXCEPT_EWIN:
	groups2 = GroupsGetList(num);
	if (!groups2)
	   break;

	for (i = 0; i < (*num); i++)
	  {
	     for (j = 0; j < ewin->num_groups; j++)
	       {
		  if (ewin->groups[j] == groups2[i])
		    {
		       groups2[i] = NULL;
		       killed++;
		    }
	       }
	  }
	groups = EMALLOC(Group *, *num - killed);
	if (groups)
	  {
	     j = 0;
	     for (i = 0; i < (*num); i++)
		if (groups2[i])
		   groups[j++] = groups2[i];
	     (*num) -= killed;
	  }
	Efree(groups2);
	break;
     case GROUP_SELECT_ALL:
     default:
	groups = GroupsGetList(num);
	break;
     }

   return groups;
}
#endif /* ENABLE_DIALOGS */

static void
_GroupAddEwin(Group * g, EWin * ewin)
{
   int                 i;

   for (i = 0; i < ewin->num_groups; i++)
      if (ewin->groups[i] == g)
	 return;

   ewin->num_groups++;
   ewin->groups = EREALLOC(Group *, ewin->groups, ewin->num_groups);
   ewin->groups[ewin->num_groups - 1] = g;
   g->num_members++;
   g->members = EREALLOC(EWin *, g->members, g->num_members);
   g->members[g->num_members - 1] = ewin;
}

static void
AddEwinToGroup(EWin * ewin, Group * g)
{
   if (!ewin || !g)
      return;

   _GroupAddEwin(g, ewin);
   SnapshotEwinUpdate(ewin, SNAP_USE_GROUPS);
}

void
GroupsEwinAdd(EWin * ewin, const int *pgid, int ngid)
{
   Group              *g;
   int                 i, gid;

   for (i = 0; i < ngid; i++)
     {
	gid = pgid[i];
	g = GroupFind(gid);
	Dprintf("ewin=%p gid=%d grp=%p\n", ewin, gid, g);
	if (!g)
	  {
	     /* This should not happen, but may if group/snap configs are corrupted */
	     BuildWindowGroup(&ewin, 1, gid);
	  }
	else
	  {
	     _GroupAddEwin(g, ewin);
	  }
     }
   SnapshotEwinUpdate(ewin, SNAP_USE_GROUPS);
}

static int
EwinInGroup(const EWin * ewin, const Group * g)
{
   int                 i;

   if (ewin && g)
     {
	for (i = 0; i < g->num_members; i++)
	  {
	     if (g->members[i] == ewin)
		return 1;
	  }
     }
   return 0;
}

Group              *
EwinsInGroup(const EWin * ewin1, const EWin * ewin2)
{
   int                 i;

   if (ewin1 && ewin2)
     {
	for (i = 0; i < ewin1->num_groups; i++)
	  {
	     if (EwinInGroup(ewin2, ewin1->groups[i]))
		return ewin1->groups[i];
	  }
     }
   return NULL;
}

void
RemoveEwinFromGroup(EWin * ewin, Group * g)
{
   int                 i, j, k, i2;

   if (!ewin || !g)
      return;

   for (k = 0; k < ewin->num_groups; k++)
     {
	/* is the window actually part of the given group */
	if (ewin->groups[k] != g)
	   continue;

	for (i = 0; i < g->num_members; i++)
	  {
	     if (g->members[i] != ewin)
		continue;

	     /* remove it from the group */
	     for (j = i; j < g->num_members - 1; j++)
		g->members[j] = g->members[j + 1];
	     g->num_members--;
	     if (g->num_members > 0)
		g->members = EREALLOC(EWin *, g->members, g->num_members);
	     else if (g->save)
	       {
		  Efree(g->members);
		  g->members = NULL;
	       }
	     else
	       {
		  GroupDestroy(g);
	       }

	     /* and remove the group from the groups that the window is in */
	     for (i2 = k; i2 < ewin->num_groups - 1; i2++)
		ewin->groups[i2] = ewin->groups[i2 + 1];
	     ewin->num_groups--;
	     if (ewin->num_groups <= 0)
	       {
		  Efree(ewin->groups);
		  ewin->groups = NULL;
		  ewin->num_groups = 0;
	       }
	     else
		ewin->groups =
		   EREALLOC(Group *, ewin->groups, ewin->num_groups);

	     GroupsSave();
	     return;
	  }
     }
}

void
GroupsEwinRemove(EWin * ewin)
{
   int                 num, i;

   num = ewin->num_groups;
   for (i = 0; i < num; i++)
      RemoveEwinFromGroup(ewin, ewin->groups[0]);
}

#if ENABLE_DIALOGS
static char       **
GetWinGroupMemberNames(Group ** groups, int num)
{
   int                 i, j, len;
   char              **group_member_strings;
   const char         *name;

   group_member_strings = ECALLOC(char *, num);

   if (!group_member_strings)
      return NULL;

   for (i = 0; i < num; i++)
     {
	group_member_strings[i] = EMALLOC(char, 1024);

	if (!group_member_strings[i])
	   break;

	len = 0;
	for (j = 0; j < groups[i]->num_members; j++)
	  {
	     name = EwinGetTitle(groups[i]->members[j]);
	     if (!name)		/* Should never happen */
		continue;
	     len += Esnprintf(group_member_strings[i] + len, 1024 - len,
			      "%s\n", name);
	     if (len >= 1024)
		break;
	  }
     }

   return group_member_strings;
}
#endif /* ENABLE_DIALOGS */

#if USE_GROUP_SHOWHIDE
static void
ShowHideWinGroups(EWin * ewin, int group_index, char onoff)
{
   EWin              **gwins;
   int                 i, num;
   const Border       *b = NULL;

   if (!ewin || group_index >= ewin->num_groups)
      return;

   if (group_index < 0)
     {
	gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ANY, 0, &num);
     }
   else
     {
	gwins = ewin->groups[group_index]->members;
	num = ewin->groups[group_index]->num_members;
     }

   if (onoff == SET_TOGGLE)
      onoff = (ewin->border == ewin->normal_border) ? SET_ON : SET_OFF;

   for (i = 0; i < num; i++)
     {
	if (onoff == SET_ON)
	   b = BorderFind(gwins[i]->border->group_border_name);
	else
	   b = gwins[i]->normal_border;

	EwinBorderChange(gwins[i], b, 0);
     }
   if (group_index < 0)
      Efree(gwins);
}
#else

#define ShowHideWinGroups(ewin, group_index, onoff)

#endif /* USE_GROUP_SHOWHIDE */

void
GroupsSave(void)
{
   Group              *g;
   FILE               *f;
   char                s[1024];

   if (ecore_list_count(group_list) <= 0)
      return;

   Esnprintf(s, sizeof(s), "%s.groups", EGetSavePrefix());
   f = fopen(s, "w");
   if (!f)
      return;

   ECORE_LIST_FOR_EACH(group_list, g)
   {
      if (!g->save)
	 continue;

      fprintf(f, "NEW: %i\n", g->index);
      fprintf(f, "ICONIFY: %i\n", g->cfg.iconify);
      fprintf(f, "KILL: %i\n", g->cfg.kill);
      fprintf(f, "MOVE: %i\n", g->cfg.move);
      fprintf(f, "RAISE: %i\n", g->cfg.raise);
      fprintf(f, "SET_BORDER: %i\n", g->cfg.set_border);
      fprintf(f, "STICK: %i\n", g->cfg.stick);
      fprintf(f, "SHADE: %i\n", g->cfg.shade);
   }

   fclose(f);
}

static int
_GroupsLoad(FILE * fs)
{
   char                s[1024];
   Group              *g = NULL;

   while (fgets(s, sizeof(s), fs))
     {
	char                ss[128];
	int                 ii;

	if (strlen(s) > 0)
	   s[strlen(s) - 1] = 0;
	ii = 0;
	sscanf(s, "%100s %d", ss, &ii);

	if (!strcmp(ss, "NEW:"))
	  {
	     g = GroupCreate(ii);
	     continue;
	  }
	if (!g)
	   continue;

	if (!strcmp(ss, "ICONIFY:"))
	  {
	     g->cfg.iconify = ii;
	  }
	else if (!strcmp(ss, "KILL:"))
	  {
	     g->cfg.kill = ii;
	  }
	else if (!strcmp(ss, "MOVE:"))
	  {
	     g->cfg.move = ii;
	  }
	else if (!strcmp(ss, "RAISE:"))
	  {
	     g->cfg.raise = ii;
	  }
	else if (!strcmp(ss, "SET_BORDER:"))
	  {
	     g->cfg.set_border = ii;
	  }
	else if (!strcmp(ss, "STICK:"))
	  {
	     g->cfg.stick = ii;
	  }
	else if (!strcmp(ss, "SHADE:"))
	  {
	     g->cfg.shade = ii;
	  }
     }

   return 0;
}

void
GroupsLoad(void)
{
   char                s[4096];

   Esnprintf(s, sizeof(s), "%s.groups", EGetSavePrefix());

   ConfigFileLoad(s, NULL, _GroupsLoad, 0);
}

#if ENABLE_DIALOGS

#define GROUP_OP_ADD	1
#define GROUP_OP_DEL	2
#define GROUP_OP_BREAK	3

static int          tmp_group_index;
static int          tmp_index;
static EWin        *tmp_ewin;
static Group      **tmp_groups;
static int          tmp_group_num;
static int          tmp_action;

static void
ChooseGroup(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (((val == 0) || (val == 2)) && tmp_groups)
     {
	ShowHideWinGroups(tmp_ewin, tmp_index, SET_OFF);
     }
   if (val == 0)
     {
	if (tmp_groups)
	  {
	     switch (tmp_action)
	       {
	       case GROUP_OP_ADD:
		  AddEwinToGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case GROUP_OP_DEL:
		  RemoveEwinFromGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       case GROUP_OP_BREAK:
		  BreakWindowGroup(tmp_ewin, tmp_groups[tmp_group_index]);
		  break;
	       default:
		  break;
	       }
	  }
     }
   if (((val == 0) || (val == 2)) && tmp_groups)
     {
	Efree(tmp_groups);
	tmp_groups = NULL;

	GroupsSave();
     }
}

static void
GroupCallback(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   ShowHideWinGroups(tmp_ewin, tmp_index, SET_OFF);
   ShowHideWinGroups(tmp_ewin, val, SET_ON);
   tmp_index = val;
}

static void
_DlgFillGroupChoose(Dialog * d, DItem * table, void *data)
{
   DItem              *di, *radio;
   int                 i, num_groups;
   char              **group_member_strings;
   const char         *message = (const char *)data;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, message);

   num_groups = tmp_group_num;
   group_member_strings = GetWinGroupMemberNames(tmp_groups, num_groups);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, GroupCallback, 0, (void *)d);
   DialogItemSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetCallback(di, GroupCallback, i, NULL);
	DialogItemSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_group_index);

   StrlistFree(group_member_strings, num_groups);
}

static const DialogDef DlgGroupChoose = {
   "GROUP_SELECTION",
   NULL,
   N_("Window Group Selection"),
   SOUND_SETTINGS_GROUP,
   "pix/group.png",
   N_("Enlightenment Window Group\n" "Selection Dialog"),
   _DlgFillGroupChoose,
   DLG_OC, ChooseGroup,
};

static void
ChooseGroupDialog(EWin * ewin, const char *message, char group_select,
		  int action)
{
   int                 num_groups;

   if (!ewin)
      return;

   tmp_ewin = ewin;
   tmp_group_index = tmp_index = 0;
   tmp_action = action;
   tmp_groups = ListWinGroups(ewin, group_select, &num_groups);
   tmp_group_num = num_groups;

   if ((num_groups == 0)
       && (action == GROUP_OP_BREAK || action == GROUP_OP_DEL))
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("This window currently does not belong to any groups.\n"
		  "You can only destroy groups or remove windows from groups\n"
		  "through a window that actually belongs to at least one group."));
	return;
     }
   if ((num_groups == 0) && (group_select == GROUP_SELECT_ALL_EXCEPT_EWIN))
     {
	DialogOK(_("Window Group Error"),
		 _("Currently, no groups exist or this window\n"
		   "already belongs to all existing groups.\n"
		   "You have to start other groups first."));
	return;
     }
   if (!tmp_groups)
     {
	DialogOK(_("Window Group Error"),
		 _
		 ("Currently, no groups exist. You have to start a group first."));
	return;
     }

   ShowHideWinGroups(ewin, 0, SET_ON);

   DialogShowSimple(&DlgGroupChoose, (void *)message);
}

typedef struct {
   EWin               *ewin;
   GroupConfig         cfg;	/* Dialog data for current group */
   GroupConfig        *cfgs;	/* Work copy of ewin group cfgs */
   int                 ngrp;
   unsigned int        current;
} EwinGroupDlgData;

static void
CB_ConfigureGroup(Dialog * d, int val, void *data __UNUSED__)
{
   EwinGroupDlgData   *dd = DLG_DATA_GET(d, EwinGroupDlgData);
   EWin               *ewin;
   int                 i;

   /* Check ewin */
   ewin = EwinFindByPtr(dd->ewin);
   if (ewin && ewin->num_groups != dd->ngrp)
      ewin = NULL;

   if (val < 2 && ewin)
     {
	CopyGroupConfig(&(dd->cfg), &(dd->cfgs[dd->current]));
	for (i = 0; i < ewin->num_groups; i++)
	   CopyGroupConfig(dd->cfgs + i, &(ewin->groups[i]->cfg));
     }
   if ((val == 0) || (val == 2))
     {
	ShowHideWinGroups(ewin, dd->current, SET_OFF);
	Efree(dd->cfgs);
     }
   autosave();
}

static void
GroupSelectCallback(Dialog * d, int val, void *data __UNUSED__)
{
   EwinGroupDlgData   *dd = DLG_DATA_GET(d, EwinGroupDlgData);

   CopyGroupConfig(&(dd->cfg), &(dd->cfgs[dd->current]));
   CopyGroupConfig(&(dd->cfgs[val]), &(dd->cfg));
   DialogRedraw(d);
   ShowHideWinGroups(dd->ewin, dd->current, SET_OFF);
   ShowHideWinGroups(dd->ewin, val, SET_ON);
   dd->current = val;
}

static void
_DlgFillGroups(Dialog * d, DItem * table, void *data)
{
   EWin               *ewin = (EWin *) data;
   DItem              *radio, *di;
   int                 i;
   char              **group_member_strings;
   EwinGroupDlgData   *dd;

   dd = DLG_DATA_SET(d, EwinGroupDlgData);
   if (!dd)
      return;

   dd->ewin = ewin;
   dd->cfgs = EMALLOC(GroupConfig, ewin->num_groups);
   dd->ngrp = ewin->num_groups;
   dd->current = 0;
   for (i = 0; i < ewin->num_groups; i++)
      CopyGroupConfig(&(ewin->groups[i]->cfg), dd->cfgs + i);
   CopyGroupConfig(dd->cfgs, &(dd->cfg));

   ShowHideWinGroups(ewin, 0, SET_ON);

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Pick the group to configure:"));

   group_member_strings =
      GetWinGroupMemberNames(ewin->groups, ewin->num_groups);

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetCallback(di, GroupSelectCallback, 0, d);
   DialogItemSetText(di, group_member_strings[0]);
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   for (i = 1; i < ewin->num_groups; i++)
     {
	di = DialogAddItem(table, DITEM_RADIOBUTTON);
	DialogItemSetColSpan(di, 2);
	DialogItemSetCallback(di, GroupSelectCallback, i, d);
	DialogItemSetText(di, group_member_strings[i]);
	DialogItemRadioButtonSetFirst(di, radio);
	DialogItemRadioButtonGroupSetVal(di, i);
     }
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_index);

   StrlistFree(group_member_strings, ewin->num_groups);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("The following actions are\n"
			   "applied to all group members:"));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.set_border));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.iconify));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Killing"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.kill));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Moving"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.move));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.raise));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Sticking"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.stick));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shading"));
   DialogItemCheckButtonSetPtr(di, &(dd->cfg.shade));
}

static const DialogDef DlgGroups = {
   "CONFIGURE_GROUP",
   NULL,
   N_("Window Group Settings"),
   SOUND_SETTINGS_GROUP,
   "pix/group.png",
   N_("Enlightenment Window Group\n" "Settings Dialog"),
   _DlgFillGroups,
   DLG_OAC, CB_ConfigureGroup,
};

static void
SettingsGroups(EWin * ewin)
{
   if (!ewin)
      return;

   if (ewin->num_groups == 0)
     {
	DialogOK(_("Window Group Error"),
		 _("This window currently does not belong to any groups."));
	return;
     }

   DialogShowSimple(&DlgGroups, ewin);
}

static GroupConfig  tmp_group_cfg;
static char         tmp_group_swap;
static void
CB_ConfigureDefaultGroupSettings(Dialog * d __UNUSED__, int val,
				 void *data __UNUSED__)
{
   if (val < 2)
     {
	CopyGroupConfig(&tmp_group_cfg, &(Conf_groups.dflt));
	Conf_groups.swapmove = tmp_group_swap;
     }
   autosave();
}

static void
_DlgFillGroupDefaults(Dialog * d __UNUSED__, DItem * table,
		      void *data __UNUSED__)
{
   DItem              *di;

   CopyGroupConfig(&(Conf_groups.dflt), &tmp_group_cfg);
   tmp_group_swap = Conf_groups.swapmove;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Per-group settings:"));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Changing Border Style"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.set_border));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Iconifying"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.iconify));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Killing"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.kill));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Moving"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.move));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Raising/Lowering"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.raise));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Sticking"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.stick));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shading"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_cfg.shade));

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Global settings:"));

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Swap Window Locations"));
   DialogItemCheckButtonSetPtr(di, &(tmp_group_swap));
}

const DialogDef     DlgGroupDefaults = {
   "CONFIGURE_DEFAULT_GROUP_CONTROL",
   N_("Groups"),
   N_("Default Group Control Settings"),
   SOUND_SETTINGS_GROUP,
   "pix/group.png",
   N_("Enlightenment Default\n" "Group Control Settings Dialog"),
   _DlgFillGroupDefaults,
   DLG_OAC, CB_ConfigureDefaultGroupSettings,
};

static void
GroupsConfigure(const char *params)
{
   char                s[128];
   const char         *p;
   int                 l;
   EWin               *ewin;

   p = params;
   l = 0;
   s[0] = '\0';
   sscanf(p, "%100s %n", s, &l);

   ewin = GetContextEwin();

   if (!strcmp(s, "group"))
     {
	SettingsGroups(ewin);
     }
   else if (!strcmp(s, "add"))
     {
	ChooseGroupDialog(ewin,
			  _("Pick the group the window will belong to:"),
			  GROUP_SELECT_ALL_EXCEPT_EWIN, GROUP_OP_ADD);
     }
   else if (!strcmp(s, "del"))
     {
	ChooseGroupDialog(ewin,
			  _("Select the group to remove the window from:"),
			  GROUP_SELECT_EWIN_ONLY, GROUP_OP_DEL);
     }
   else if (!strcmp(s, "break"))
     {
	ChooseGroupDialog(ewin, _("Select the group to break:"),
			  GROUP_SELECT_EWIN_ONLY, GROUP_OP_BREAK);
     }
}
#endif /* ENABLE_DIALOGS */

/*
 * Groups module
 */

static void
GroupShow(Group * g)
{
   int                 j;

   for (j = 0; j < g->num_members; j++)
      IpcPrintf("%d: %s\n", g->index, EwinGetIcccmName(g->members[j]));

   IpcPrintf("        index: %d\n" "  num_members: %d\n"
	     "      iconify: %d\n" "         kill: %d\n"
	     "         move: %d\n" "        raise: %d\n"
	     "   set_border: %d\n" "        stick: %d\n"
	     "        shade: %d\n",
	     g->index, g->num_members,
	     g->cfg.iconify, g->cfg.kill,
	     g->cfg.move, g->cfg.raise,
	     g->cfg.set_border, g->cfg.stick, g->cfg.shade);
}

static void
IPC_GroupInfo(const char *params)
{
   Group              *group;

   if (params)
     {
	group = GroupFind2(params);
	if (group)
	   GroupShow(group);
	else
	   IpcPrintf("Error: no such group: %s\n", params);
     }
   else
     {
	IpcPrintf("Number of groups: %d\n", ecore_list_count(group_list));
	ECORE_LIST_FOR_EACH(group_list, group) GroupShow(group);
     }
}

static void
IPC_GroupOps(const char *params)
{
   Group              *group;
   char                windowid[128];
   char                operation[128];
   char                groupid[128];
   unsigned int        win;
   EWin               *ewin;

   if (!params)
     {
	IpcPrintf("Error: no window specified\n");
	return;
     }

   windowid[0] = operation[0] = groupid[0] = '\0';
   sscanf(params, "%100s %100s %100s", windowid, operation, groupid);
   win = 0;
   sscanf(windowid, "%x", &win);

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified\n");
	return;
     }

   ewin = EwinFindByExpr(windowid);
   if (!ewin)
     {
	IpcPrintf("Error: no such window: %s\n", windowid);
	return;
     }

   if (!strcmp(operation, "start"))
     {
	BuildWindowGroup(&ewin, 1, -1);
	IpcPrintf("start %8x\n", win);
     }
   else if (!strcmp(operation, "add"))
     {
	group = GroupFind2(groupid);
	AddEwinToGroup(ewin, group);
	IpcPrintf("add %8x\n", win);
     }
   else if (!strcmp(operation, "del"))
     {
	group = GroupFind2(groupid);
	RemoveEwinFromGroup(ewin, group);
	IpcPrintf("del %8x\n", win);
     }
   else if (!strcmp(operation, "break"))
     {
	group = GroupFind2(groupid);
	BreakWindowGroup(ewin, group);
	IpcPrintf("break %8x\n", win);
     }
   else if (!strcmp(operation, "showhide"))
     {
	ShowHideWinGroups(ewin, -1, SET_TOGGLE);
	IpcPrintf("showhide %8x\n", win);
     }
   else
     {
	IpcPrintf("Error: no such operation: %s\n", operation);
	return;
     }
   GroupsSave();
}

static void
IPC_Group(const char *params)
{
   char                groupid[128];
   char                operation[128];
   char                param1[128];
   Group              *group;
   int                 onoff;

   if (!params)
     {
	IpcPrintf("Error: no group specified\n");
	return;
     }

   groupid[0] = operation[0] = param1[0] = '\0';
   sscanf(params, "%100s %100s %100s", groupid, operation, param1);

   if (!operation[0])
     {
	IpcPrintf("Error: no operation specified\n");
	return;
     }

   group = GroupFind2(groupid);
   if (!group)
     {
	IpcPrintf("Error: no such group: %s\n", groupid);
	return;
     }

   if (!param1[0])
     {
	IpcPrintf("Error: no mode specified\n");
	return;
     }

   onoff = -1;
   if (!strcmp(param1, "on"))
      onoff = 1;
   else if (!strcmp(param1, "off"))
      onoff = 0;

   if (onoff == -1 && strcmp(param1, "?"))
     {
	IpcPrintf("Error: unknown mode specified\n");
     }
   else if (!strcmp(operation, "num_members"))
     {
	IpcPrintf("num_members: %d\n", group->num_members);
	onoff = -1;
     }
   else if (!strcmp(operation, "iconify"))
     {
	if (onoff >= 0)
	   group->cfg.iconify = onoff;
	else
	   onoff = group->cfg.iconify;
     }
   else if (!strcmp(operation, "kill"))
     {
	if (onoff >= 0)
	   group->cfg.kill = onoff;
	else
	   onoff = group->cfg.kill;
     }
   else if (!strcmp(operation, "move"))
     {
	if (onoff >= 0)
	   group->cfg.move = onoff;
	else
	   onoff = group->cfg.move;
     }
   else if (!strcmp(operation, "raise"))
     {
	if (onoff >= 0)
	   group->cfg.raise = onoff;
	else
	   onoff = group->cfg.raise;
     }
   else if (!strcmp(operation, "set_border"))
     {
	if (onoff >= 0)
	   group->cfg.set_border = onoff;
	else
	   onoff = group->cfg.set_border;
     }
   else if (!strcmp(operation, "stick"))
     {
	if (onoff >= 0)
	   group->cfg.stick = onoff;
	else
	   onoff = group->cfg.stick;
     }
   else if (!strcmp(operation, "shade"))
     {
	if (onoff >= 0)
	   group->cfg.shade = onoff;
	else
	   onoff = group->cfg.shade;
     }
   else
     {
	IpcPrintf("Error: no such operation: %s\n", operation);
	onoff = -1;
     }

   if (onoff == 1)
      IpcPrintf("%s: on\n", operation);
   else if (onoff == 0)
      IpcPrintf("%s: off\n", operation);
}

#if ENABLE_DIALOGS
static void
GroupsIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	/* Show groups */
     }
   else if (!strncmp(cmd, "cfg", 2))
     {
	GroupsConfigure(prm);
     }
}
#endif /* ENABLE_DIALOGS */

static const IpcItem GroupsIpcArray[] = {
#if ENABLE_DIALOGS
   {
    GroupsIpc,
    "groups", "grp",
    "Configure window groups",
    "  groups cfg           Configure groups\n"}
   ,
#endif /* ENABLE_DIALOGS */
   {
    IPC_GroupInfo,
    "group_info", "gl",
    "Retrieve some info on groups",
    "use \"group_info [group_index]\"\n"}
   ,
   {
    IPC_GroupOps,
    "group_op", "gop",
    "Group operations",
    "use \"group_op <windowid> <property> [<value>]\" to perform "
    "group operations on a window.\n" "Available group_op commands are:\n"
    "  group_op <windowid> start\n"
    "  group_op <windowid> add [<group_index>]\n"
    "  group_op <windowid> del [<group_index>]\n"
    "  group_op <windowid> break [<group_index>]\n"
    "  group_op <windowid> showhide\n"}
   ,
   {
    IPC_Group,
    "group", "gc",
    "Group commands",
    "use \"group <groupid> <property> <value>\" to set group properties.\n"
    "Available group commands are:\n"
    "  group <groupid> num_members <on/off/?>\n"
    "  group <groupid> iconify <on/off/?>\n"
    "  group <groupid> kill <on/off/?>\n" "  group <groupid> move <on/off/?>\n"
    "  group <groupid> raise <on/off/?>\n"
    "  group <groupid> set_border <on/off/?>\n"
    "  group <groupid> stick <on/off/?>\n"
    "  group <groupid> shade <on/off/?>\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(GroupsIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem GroupsCfgItems[] = {
   CFG_ITEM_BOOL(Conf_groups, dflt.iconify, 1),
   CFG_ITEM_BOOL(Conf_groups, dflt.kill, 0),
   CFG_ITEM_BOOL(Conf_groups, dflt.move, 1),
   CFG_ITEM_BOOL(Conf_groups, dflt.raise, 0),
   CFG_ITEM_BOOL(Conf_groups, dflt.set_border, 1),
   CFG_ITEM_BOOL(Conf_groups, dflt.stick, 1),
   CFG_ITEM_BOOL(Conf_groups, dflt.shade, 1),
   CFG_ITEM_BOOL(Conf_groups, swapmove, 1),
};
#define N_CFG_ITEMS (sizeof(GroupsCfgItems)/sizeof(CfgItem))

extern const EModule ModGroups;

const EModule       ModGroups = {
   "groups", "grp",
   NULL,
   {N_IPC_FUNCS, GroupsIpcArray},
   {N_CFG_ITEMS, GroupsCfgItems}
};
