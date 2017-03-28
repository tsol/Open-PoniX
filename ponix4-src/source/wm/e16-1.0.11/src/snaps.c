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
#include "desktops.h"
#include "dialog.h"
#include "e16-ecore_list.h"
#include "ewins.h"
#include "file.h"
#include "groups.h"
#include "ipc.h"
#include "settings.h"
#include "snaps.h"
#include "timers.h"
#include "xwin.h"

struct _snapshot {
   char               *name;
   char               *win_title;
   char               *win_name;
   char               *win_class;
   char               *win_role;
   Window              win;
   EWin               *used;
   unsigned int        startup_id;
   char                track_changes;
   unsigned int        match_flags;
   unsigned int        use_flags;

   char               *border_name;
   int                 desktop;
   int                 area_x, area_y;
   int                 x, y;
   int                 w, h;
   int                 layer;
   char                sticky;
   char                shaded;
   unsigned int        flags[2];
   char               *cmd;
   int                *groups;
   int                 num_groups;
   char                skiptask;
   char                skipfocus;
   char                skipwinlist;
#if USE_COMPOSITE
   int                 opacity;
   int                 focused_opacity;
   char                shadow;
#endif
};

static Ecore_List  *ss_list = NULL;
static Timer       *ss_timer = NULL;

static Snapshot    *
_SnapCreate(const char *name)
{
   Snapshot           *sn;

   sn = ECALLOC(Snapshot, 1);
   if (!sn)
      return NULL;

   if (!ss_list)
      ss_list = ecore_list_new();
   ecore_list_append(ss_list, sn);

   sn->name = Estrdup(name);

   return sn;
}

static void
_SnapDestroy(Snapshot * sn)
{
   /* Just making sure */
   sn = (Snapshot *) ecore_list_node_remove(ss_list, sn);
   if (!sn)
      return;

   if (sn->used)
      sn->used->snap = NULL;

   Efree(sn->name);
   Efree(sn->win_title);
   Efree(sn->win_name);
   Efree(sn->win_class);
   Efree(sn->win_role);
   Efree(sn->border_name);
   Efree(sn->cmd);
   Efree(sn->groups);

   Efree(sn);
}

/*
 * Stupid hack to fix apps that set WM_WINDOW_ROLE to
 * a <name>-<pid>-<something>-<time> like thing.
 * Is this even ICCCM compliant?
 */
static char        *
_ParseRole(const char *role, char *buf, int len)
{
   int                 l1, l2;

   if (!role)
      return NULL;

   l1 = strlen(role);
   if (l1 >= len)
      l1 = len - 1;

   for (l2 = l1; l2 > 0; l2--)
     {
	if (role[l2 - 1] != '-' &&
	    !(role[l2 - 1] >= '0' && role[l2 - 1] <= '9'))
	   break;
     }
   if (l1 - l2 > 8)
      l1 = l2;
   memcpy(buf, role, l1);
   buf[l1] = '\0';

   return buf;
}

#define SEQ(s1, s2) ((s1) && (s2) && !strcmp(s1, s2))

static int
_SnapEwinMatch(const Snapshot * sn, const EWin * ewin)
{
   char                buf[256], *s;

   /* Don't allow matching anything */
   if (!sn->match_flags)
      return 0;

   if (ewin->state.identified)
      return sn->win == EwinGetClientXwin(ewin);

   if (sn->startup_id && !sn->cmd)
      return 0;

   if (sn->match_flags & SNAP_MATCH_TITLE
       && !SEQ(sn->win_title, EwinGetIcccmName(ewin)))
      return 0;

   if (sn->match_flags & SNAP_MATCH_NAME
       && !SEQ(sn->win_name, EwinGetIcccmCName(ewin)))
      return 0;

   if (sn->match_flags & SNAP_MATCH_CLASS
       && !SEQ(sn->win_class, EwinGetIcccmClass(ewin)))
      return 0;

   if (sn->match_flags & SNAP_MATCH_ROLE)
     {
	s = _ParseRole(ewin->icccm.wm_role, buf, sizeof(buf));
	if (!SEQ(sn->win_role, s))
	   return 0;
     }

   /* Match! */
   return 1;
}

static int
_SnapEwinFindMatchCmd(const void *data, const void *match)
{
   const Snapshot     *sn = (Snapshot *) data;
   const EWin         *ewin = (EWin *) match;

   return sn->used ||
      !(sn->startup_id && SEQ(sn->cmd, ewin->icccm.wm_command) &&
	_SnapEwinMatch(sn, ewin));
}

static int
_SnapEwinFindMatch(const void *data, const void *match)
{
   const Snapshot     *sn = (Snapshot *) data;
   const EWin         *ewin = (EWin *) match;

   return sn->used || !_SnapEwinMatch(sn, ewin);
}

/* find a snapshot state that applies to this ewin */
static Snapshot    *
_SnapEwinFind(EWin * ewin)
{
   Snapshot           *sn;

   if (ewin->snap)
      return ewin->snap;

   if (ecore_list_count(ss_list) <= 0)
      return NULL;

   /* If exec'ed by snap try matching command exactly */
   sn = (Snapshot *) ecore_list_find(ss_list, _SnapEwinFindMatchCmd, ewin);

   if (!sn)
      sn = (Snapshot *) ecore_list_find(ss_list, _SnapEwinFindMatch, ewin);

   if (sn && !(sn->match_flags & SNAP_MATCH_MULTIPLE))
     {
	sn->used = ewin;
	ewin->snap = sn;
     }

   return sn;
}

#define ST(s) ((s) ? (s) : "")

/* find a snapshot state that applies to this ewin Or if that doesnt exist */
/* create a new one */
static Snapshot    *
_SnapEwinGet(EWin * ewin, unsigned int match_flags)
{
   Snapshot           *sn;
   char                buf[1024], *s;

   sn = _SnapEwinFind(ewin);
   if (sn)
      return sn;

   if ((match_flags & SNAP_MATCH_TITLE) && !EwinGetIcccmName(ewin))
      match_flags ^= SNAP_MATCH_TITLE;
   if ((match_flags & SNAP_MATCH_NAME) && !EwinGetIcccmCName(ewin))
      match_flags ^= SNAP_MATCH_NAME;
   if ((match_flags & SNAP_MATCH_CLASS) && !EwinGetIcccmClass(ewin))
      match_flags ^= SNAP_MATCH_CLASS;
   if ((match_flags & SNAP_MATCH_ROLE) && !ewin->icccm.wm_role)
      match_flags ^= SNAP_MATCH_ROLE;
   if (match_flags == 0)
     {
	if (!EwinGetIcccmName(ewin))
	   return NULL;
	match_flags = SNAP_MATCH_TITLE;
     }

   sn = _SnapCreate(NULL);
   if (!sn)
      return NULL;

   sn->match_flags = match_flags;
   if (match_flags & SNAP_MATCH_TITLE)
      sn->win_title = Estrdup(EwinGetIcccmName(ewin));
   if (match_flags & SNAP_MATCH_NAME)
      sn->win_name = Estrdup(EwinGetIcccmCName(ewin));
   if (match_flags & SNAP_MATCH_CLASS)
      sn->win_class = Estrdup(EwinGetIcccmClass(ewin));
   if (match_flags & SNAP_MATCH_ROLE)
     {
	s = _ParseRole(ewin->icccm.wm_role, buf, sizeof(buf));
	sn->win_role = Estrdup(s);
     }

   /* Set the snap name. Has no particular significance. */
   if ((sn->win_name || sn->win_class) && sn->win_role)
      Esnprintf(buf, sizeof(buf), "%s.%s:%s", ST(sn->win_name),
		ST(sn->win_class), sn->win_role);
   else if (sn->win_name || sn->win_class)
      Esnprintf(buf, sizeof(buf), "%s.%s", ST(sn->win_name), ST(sn->win_class));
   else if (sn->win_title)
      Esnprintf(buf, sizeof(buf), "TITLE.%s", sn->win_title);
   else				/* We should not go here */
      Esnprintf(buf, sizeof(buf), "TITLE.%s", EwinGetIcccmName(ewin));
   sn->name = Estrdup(buf);

   if (!(sn->match_flags & SNAP_MATCH_MULTIPLE))
     {
	sn->used = ewin;
	ewin->snap = sn;
     }

   return sn;
}

/* record info about this Ewin's attributes */

static void
_SnapUpdateEwinBorder(Snapshot * sn, const EWin * ewin)
{
   Efree(sn->border_name);
   sn->border_name = Estrdup(BorderGetName(ewin->normal_border));
}

static void
_SnapUpdateEwinDesktop(Snapshot * sn, const EWin * ewin)
{
   sn->desktop = EoGetDeskNum(ewin);
}

static void
_SnapUpdateEwinSize(Snapshot * sn, const EWin * ewin)
{
   sn->w = ewin->client.w;
   sn->h = ewin->client.h;
}

static void
_SnapUpdateEwinLocation(Snapshot * sn, const EWin * ewin)
{
   int                 ax, ay;

   sn->x = EoGetX(ewin);
   sn->y = EoGetY(ewin);
   sn->area_x = ewin->area_x;
   sn->area_y = ewin->area_y;
   if (!EoIsSticky(ewin))
     {
	DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	sn->x += ((ax - sn->area_x) * WinGetW(VROOT));
	sn->y += ((ay - sn->area_y) * WinGetH(VROOT));
     }
}

static void
_SnapUpdateEwinLayer(Snapshot * sn, const EWin * ewin)
{
   sn->layer = EoGetLayer(ewin);
}

static void
_SnapUpdateEwinSticky(Snapshot * sn, const EWin * ewin)
{
   sn->sticky = EoIsSticky(ewin);
}

static void
_SnapUpdateEwinShade(Snapshot * sn, const EWin * ewin)
{
   sn->shaded = ewin->state.shaded;
}

static void
_SnapUpdateEwinSkipLists(Snapshot * sn, const EWin * ewin)
{
   sn->skiptask = ewin->props.skip_ext_task;
   sn->skipwinlist = ewin->props.skip_winlist;
   sn->skipfocus = ewin->props.skip_focuslist;
}

static void
_SnapUpdateEwinFlags(Snapshot * sn, const EWin * ewin)
{
   EwinFlagsEncode(ewin, sn->flags);
}

static void
_SnapUpdateEwinCmd(Snapshot * sn, const EWin * ewin)
{
   if (ewin->icccm.wm_machine &&
       strcmp(ewin->icccm.wm_machine, Mode.wm.machine_name))
      return;

   Efree(sn->cmd);
   sn->cmd = Estrdup(ewin->icccm.wm_command);
}

static void
_SnapUpdateEwinGroups(Snapshot * sn, const EWin * ewin, char onoff)
{
   EWin              **gwins = NULL;
   Group              *const *groups;
   int                 i, j, num, num_groups;

   if (!ewin)
      return;

   if (!ewin->groups)
     {
	Efree(sn->groups);
	sn->groups = NULL;
	sn->num_groups = 0;
	return;
     }

   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ANY, Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (onoff)
	  {
	     groups = EwinGetGroups(gwins[i], &num_groups);
	     if (!groups)
		continue;

	     sn = gwins[i]->snap;
	     if (!sn)
		sn = _SnapEwinGet(gwins[i], SNAP_MATCH_DEFAULT);
	     if (!sn)
		continue;

	     sn->num_groups = num_groups;
	     Efree(sn->groups);
	     sn->groups = EMALLOC(int, num_groups);

	     for (j = 0; j < num_groups; j++)
	       {
		  sn->groups[j] = groups[j]->index;
		  groups[j]->save = 1;
	       }
	  }
	else
	  {
	     if (ewin->snap)
	       {
		  sn = gwins[i]->snap;
		  if (sn)
		    {
		       Efree(sn->groups);
		       sn->groups = NULL;
		       sn->num_groups = 0;
		    }
	       }
	  }
     }
   Efree(gwins);
}

#if USE_COMPOSITE

static void
_SnapUpdateEwinOpacity(Snapshot * sn, const EWin * ewin)
{
   sn->opacity = OpacityToPercent(ewin->ewmh.opacity);
   sn->focused_opacity = OpacityToPercent(ewin->props.focused_opacity);
}

static void
_SnapUpdateEwinShadow(Snapshot * sn, const EWin * ewin)
{
   sn->shadow = EoGetShadow(ewin);
}

#endif

static void
_SnapUpdateEwin(Snapshot * sn, const EWin * ewin, unsigned int flags)
{
   /* FIXME - We should check if anything is actually changed */

   if (flags & SNAP_USE_BORDER)
      _SnapUpdateEwinBorder(sn, ewin);
   if (flags & SNAP_USE_COMMAND)
      _SnapUpdateEwinCmd(sn, ewin);
   if (flags & SNAP_USE_DESK)
      _SnapUpdateEwinDesktop(sn, ewin);
   if (flags & SNAP_USE_POS)
      _SnapUpdateEwinLocation(sn, ewin);
   if (flags & SNAP_USE_SIZE)
      _SnapUpdateEwinSize(sn, ewin);
   if (flags & SNAP_USE_LAYER)
      _SnapUpdateEwinLayer(sn, ewin);
   if (flags & SNAP_USE_STICKY)
      _SnapUpdateEwinSticky(sn, ewin);
   if (flags & SNAP_USE_SHADED)
      _SnapUpdateEwinShade(sn, ewin);
   if (flags & SNAP_USE_SKIP_LISTS)
      _SnapUpdateEwinSkipLists(sn, ewin);
   if (flags & SNAP_USE_FLAGS)
      _SnapUpdateEwinFlags(sn, ewin);
#if USE_COMPOSITE
   if (flags & SNAP_USE_OPACITY)
      _SnapUpdateEwinOpacity(sn, ewin);
   if (flags & SNAP_USE_SHADOW)
      _SnapUpdateEwinShadow(sn, ewin);
#endif
   if (flags & SNAP_USE_GROUPS)
      _SnapUpdateEwinGroups(sn, ewin, ewin->num_groups);

   SnapshotsSave();
}

static void
_EwinSnapSet(EWin * ewin, unsigned int match_flags, unsigned int use_flags)
{
   Snapshot           *sn;

   /* Quit if nothing to be saved */
   if (!match_flags || !(use_flags & SNAP_USE_ALL))
      return;

   sn = _SnapEwinGet(ewin, match_flags);
   if (!sn)
      return;

   if (use_flags & SNAP_AUTO)
      sn->track_changes = 1;

   sn->use_flags = use_flags & SNAP_USE_ALL;

   _SnapUpdateEwin(sn, ewin, use_flags);
}

void
SnapshotEwinUpdate(const EWin * ewin, unsigned int flags)
{
   Snapshot           *sn;

   sn = ewin->snap;
   if (!sn || !sn->track_changes)
      return;

#if 0
   Eprintf("SnapshotEwinUpdate %s: %#x\n", EwinGetTitle(ewin), flags);
#endif

   if (flags & sn->use_flags)
      _SnapUpdateEwin(sn, ewin, flags);
}

/* unsnapshot any saved info about this ewin */
static void
_EwinSnapRemove(EWin * ewin)
{
   if (ewin->snap)
      _SnapDestroy(ewin->snap);
   ewin->snap = NULL;
}

#if ENABLE_DIALOGS
/*
 * Snapshot dialogs
 */
typedef struct {
   Window              client;

   struct {
      char                title;
      char                name;
      char                clss;
      char                role;
   } match;

   char                track_changes;
   char                snap_border;
   char                snap_desktop;
   char                snap_size;
   char                snap_location;
   char                snap_layer;
   char                snap_sticky;
   char                snap_shaded;
   char                snap_cmd;
   char                snap_group;
   char                snap_skiplists;
   char                snap_flags;

#if USE_COMPOSITE
   char                snap_opacity;
   char                snap_shadow;
#endif
} SnapDlgData;

static void
CB_ApplySnap(Dialog * d, int val, void *data __UNUSED__)
{
   EWin               *ewin;
   SnapDlgData        *sd = DLG_DATA_GET(d, SnapDlgData);
   unsigned int        match_flags, use_flags;

   if (val >= 2)
      goto done;

   ewin = EwinFindByClient(sd->client);
   if (!ewin)
      goto done;

   _EwinSnapRemove(ewin);

   match_flags = 0;
   if (sd->match.title)
      match_flags |= SNAP_MATCH_TITLE;
   if (sd->match.name)
      match_flags |= SNAP_MATCH_NAME;
   if (sd->match.clss)
      match_flags |= SNAP_MATCH_CLASS;
   if (sd->match.role)
      match_flags |= SNAP_MATCH_ROLE;

   if (!match_flags)
      goto done;

   use_flags = 0;
   if (sd->track_changes)
      use_flags |= SNAP_AUTO;
   if (sd->snap_border)
      use_flags |= SNAP_USE_BORDER;
   if (sd->snap_cmd)
      use_flags |= SNAP_USE_COMMAND;
   if (sd->snap_desktop)
      use_flags |= SNAP_USE_DESK;
   if (sd->snap_location)
      use_flags |= SNAP_USE_POS;
   if (sd->snap_size)
      use_flags |= SNAP_USE_SIZE;
   if (sd->snap_layer)
      use_flags |= SNAP_USE_LAYER;
   if (sd->snap_sticky)
      use_flags |= SNAP_USE_STICKY;
   if (sd->snap_shaded)
      use_flags |= SNAP_USE_SHADED;
   if (sd->snap_skiplists)
      use_flags |= SNAP_USE_SKIP_LISTS;
   if (sd->snap_flags)
      use_flags |= SNAP_USE_FLAGS;
#if USE_COMPOSITE
   if (sd->snap_opacity)
      use_flags |= SNAP_USE_OPACITY;
   if (sd->snap_shadow)
      use_flags |= SNAP_USE_SHADOW;
#endif
   if (sd->snap_group)
      use_flags |= SNAP_USE_GROUPS;

   if (!use_flags)
      goto done;

   _EwinSnapSet(ewin, match_flags, use_flags);

 done:
   SnapshotsSave();
}

static void
_DlgFillSnap(Dialog * d, DItem * table, void *data)
{
   DItem              *di;
   Snapshot           *sn;
   SnapDlgData        *sd;
   char                s[1024];
   const EWin         *ewin = (EWin *) data;

   sd = DLG_DATA_SET(d, SnapDlgData);
   if (!sd)
      return;

   sd->client = EwinGetClientXwin(ewin);

   sn = ewin->snap;
   if (sn)
     {
	sd->match.title = (sn->match_flags & SNAP_MATCH_TITLE) != 0;
	sd->match.name = (sn->match_flags & SNAP_MATCH_NAME) != 0;
	sd->match.clss = (sn->match_flags & SNAP_MATCH_CLASS) != 0;
	sd->match.role = (sn->match_flags & SNAP_MATCH_ROLE) != 0;

	if (sn->track_changes)
	   sd->track_changes = 1;
	if (sn->use_flags & SNAP_USE_BORDER)
	   sd->snap_border = 1;
	if (sn->use_flags & SNAP_USE_COMMAND)
	   sd->snap_cmd = 1;
	if (sn->use_flags & SNAP_USE_DESK)
	   sd->snap_desktop = 1;
	if (sn->use_flags & SNAP_USE_POS)
	   sd->snap_location = 1;
	if (sn->use_flags & SNAP_USE_SIZE)
	   sd->snap_size = 1;
	if (sn->use_flags & SNAP_USE_LAYER)
	   sd->snap_layer = 1;
	if (sn->use_flags & SNAP_USE_STICKY)
	   sd->snap_sticky = 1;
	if (sn->use_flags & SNAP_USE_SHADED)
	   sd->snap_shaded = 1;
	if (sn->use_flags & SNAP_USE_SKIP_LISTS)
	   sd->snap_skiplists = 1;
	if (sn->use_flags & SNAP_USE_FLAGS)
	   sd->snap_flags = 1;
#if USE_COMPOSITE
	if (sn->use_flags & SNAP_USE_OPACITY)
	   sd->snap_opacity = 1;
	if (sn->use_flags & SNAP_USE_SHADOW)
	   sd->snap_shadow = 1;
#endif
	if (sn->use_flags & SNAP_USE_GROUPS)
	   sd->snap_group = 1;
     }
   else
     {
	if (EwinGetIcccmCName(ewin))
	  {
	     sd->match.name = 1;
	     sd->match.clss = 1;
	     sd->match.role = ! !ewin->icccm.wm_role;
	  }
	else
	  {
	     sd->match.title = ! !EwinGetIcccmName(ewin);
	  }
     }

   table = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table, 4, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Title:"));
   DialogItemCheckButtonSetPtr(di, &sd->match.title);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 3);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, EwinGetIcccmName(ewin));

   if (EwinGetIcccmCName(ewin))
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Name:"));
	DialogItemCheckButtonSetPtr(di, &sd->match.name);

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemSetText(di, EwinGetIcccmCName(ewin));
     }

   if (EwinGetIcccmClass(ewin))
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Class:"));
	DialogItemCheckButtonSetPtr(di, &sd->match.clss);

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemSetText(di, EwinGetIcccmClass(ewin));
     }

   if (ewin->icccm.wm_role)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Role:"));
	DialogItemCheckButtonSetPtr(di, &sd->match.role);

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetAlign(di, 1024, 512);
	DialogItemSetText(di, ewin->icccm.wm_role);
     }

   if (ewin->icccm.wm_command)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Command:"));

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetAlign(di, 1024, 512);

	/* if the command is long, cut in into slices of about 80 characters */
	if (strlen(ewin->icccm.wm_command) > 80)
	  {
	     int                 i = 0, slice, last;

	     s[0] = 0;
	     while ((i <= (int)strlen(ewin->icccm.wm_command))
		    && (i < (int)(sizeof(s) / 4)))
	       {
		  last = i;
		  i += 64;
		  slice = 64;
		  /* and make sure that we don't cut in the middle of a word. */
		  while ((ewin->icccm.wm_command[i++] != ' ')
			 && (i < (int)(sizeof(s) / 4)))
		     slice++;
		  strncat(s, ewin->icccm.wm_command + last, slice);
		  if (i < (int)(sizeof(s) / 4))
		     strcat(s, "\n");
		  else
		     strcat(s, "...\n");
	       }
	     DialogItemSetText(di, s);
	  }
	else
	   DialogItemSetText(di, ewin->icccm.wm_command);
     }

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 4);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 4);
   DialogItemSetText(di, _("Track Changes"));
   DialogItemCheckButtonSetPtr(di, &sd->track_changes);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Location"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_location);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Border style"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_border);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Size"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_size);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Desktop"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_desktop);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shaded state"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_shaded);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Sticky state"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_sticky);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Stacking layer"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_layer);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Window List Skip"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_skiplists);

#if USE_COMPOSITE
   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Opacity"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_opacity);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Shadowing"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_shadow);
#endif

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Flags"));
   DialogItemCheckButtonSetPtr(di, &sd->snap_flags);

   di = DialogAddItem(table, DITEM_NONE);
   DialogItemSetColSpan(di, 2);

   if (ewin->icccm.wm_command)
     {
	char                ok = 1;

	if (ewin->icccm.wm_machine)
	  {
	     if (strcmp(ewin->icccm.wm_machine, Mode.wm.machine_name))
		ok = 0;
	  }
	if (ok)
	  {
	     di = DialogAddItem(table, DITEM_CHECKBUTTON);
	     DialogItemSetColSpan(di, 4);
	     DialogItemSetText(di, _("Restart application on login"));
	     DialogItemCheckButtonSetPtr(di, &sd->snap_cmd);
	  }
	else
	  {
	     di = DialogAddItem(table, DITEM_NONE);
	     DialogItemSetColSpan(di, 4);
	  }
     }
   else
     {
	di = DialogAddItem(table, DITEM_NONE);
	DialogItemSetColSpan(di, 4);
     }

   if (ewin->groups)
     {
	di = DialogAddItem(table, DITEM_CHECKBUTTON);
	DialogItemSetColSpan(di, 4);
	DialogItemSetText(di, _("Remember this window's group(s)"));
	DialogItemCheckButtonSetPtr(di, &sd->snap_group);
     }
}

static const DialogDef DlgSnap = {
   NULL,
   NULL,
   N_("Remembered Application Attributes"),
   SOUND_NONE,
   "pix/snapshots.png",
   N_("Select the attributes of this\n"
      "window you wish to Remember\n" "from now on\n"),
   _DlgFillSnap,
   DLG_OAC, CB_ApplySnap,
};

static void
_EwinSnapDialog(const EWin * ewin)
{
   char                s[1024];

   Esnprintf(s, sizeof(s), "SNAPSHOT_WINDOW-%#lx", EwinGetClientXwin(ewin));

   DialogShowSimpleWithName(&DlgSnap, s, (void *)ewin);
}

/* list of remembered items for the remember dialog -- it's either
 * _another_ global var, or a wrapper struct to pass data to the 
 * callback funcs besides the dialog itself -- this is much easier */

typedef struct _remwinlist {
   Snapshot           *snap;
   char                remove;
} RememberWinList;

static RememberWinList *rd_ewin_list;

static void
CB_ApplyRemember(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   int                 i;

   if (val < 2 && rd_ewin_list)
     {
	for (i = 0; rd_ewin_list[i].snap; i++)
	  {
	     if (!rd_ewin_list[i].remove)
		continue;

	     _SnapDestroy(rd_ewin_list[i].snap);
	  }
	/* save snapshot info to disk */
	SnapshotsSave();
     }

   if (((val == 0) || (val == 2)) && rd_ewin_list)
     {
	Efree(rd_ewin_list);
	rd_ewin_list = NULL;
     }
}

static void
CB_RememberWindowSettings(Dialog * d __UNUSED__, int val __UNUSED__, void *data)
{
   RememberWinList    *rd;
   Snapshot           *sn;

   if (!data)
      return;
   rd = (RememberWinList *) data;

   /* Make sure its still there */
   sn = (Snapshot *) ecore_list_goto(ss_list, rd->snap);

   if (!sn || !sn->used)
      return;
   _EwinSnapDialog(sn->used);
}

static void
_DlgFillRemember(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di;
   Snapshot           *sn;
   int                 i, num;
   char                buf[128];
   const char         *s;

   DialogItemTableSetOptions(table, 3, 0, 0, 0);

   num = ecore_list_count(ss_list);
   rd_ewin_list = EMALLOC(RememberWinList, num + 1);

   if (num > 0)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetFill(di, 0, 0);
	DialogItemSetAlign(di, 0, 512);
	DialogItemSetText(di, _("Delete"));
     }

   i = 0;
   ECORE_LIST_FOR_EACH(ss_list, sn)
   {
      rd_ewin_list[i].snap = sn;
      rd_ewin_list[i].remove = 0;

      di = DialogAddItem(table, DITEM_CHECKBUTTON);
      DialogItemSetColSpan(di, 2);
      DialogItemSetAlign(di, 0, 512);
      if (sn->used)
	 s = EwinGetTitle(sn->used);
      else if (sn->win_title)
	 s = sn->win_title;
      else
	{
	   Esnprintf(buf, sizeof(buf), "%s.%s", sn->win_name, sn->win_class);
	   s = buf;
	}
      DialogItemSetText(di, s);
      DialogItemCheckButtonSetPtr(di, &(rd_ewin_list[i].remove));

      if (sn->used)
	{
	   di = DialogAddItem(table, DITEM_BUTTON);
	   DialogItemSetAlign(di, 0, 512);
	   DialogItemSetText(di, _("Remembered Settings..."));
	   DialogItemSetCallback(di, CB_RememberWindowSettings, 0,
				 (char *)(&rd_ewin_list[i]));
	}
      else
	{
	   di = DialogAddItem(table, DITEM_TEXT);
	   DialogItemSetText(di, _("Unused"));
	}
      i++;
   }
   rd_ewin_list[num].snap = NULL;

   /* finish remember window */
   if (!num)
     {
	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetColSpan(di, 3);
	DialogItemSetText(di,
			  _
			  ("There are no active windows with remembered attributes."));
     }
}

const DialogDef     DlgRemember = {
   "CONFIGURE_PAGER",
   N_("Remember"),
   N_("Remembered Windows Settings"),
   SOUND_SETTINGS_PAGER,
   "pix/snapshots.png",
   N_("Enlightenment Remembered\n" "Windows Settings Dialog"),
   _DlgFillRemember,
   DLG_OC, CB_ApplyRemember,
};
#endif /* ENABLE_DIALOGS */

/* ... combine writes, only save after a timeout */
static int
_SnapshotsSaveReal(void *data __UNUSED__)
{
   SnapshotsSaveReal();
   return 0;
}

void
SnapshotsSave(void)
{
   TIMER_DEL(ss_timer);
   TIMER_ADD(ss_timer, 5000, _SnapshotsSaveReal, NULL);
}

/* save out all snapped info to disk */
void
SnapshotsSaveReal(void)
{
   Snapshot           *sn;
   int                 j;
   char                buf[4096], s[4096];
   FILE               *f;

   if (!Mode.wm.save_ok)
      goto done;

   Etmp(s);
   f = fopen(s, "w");
   if (!f)
      goto done;

   ECORE_LIST_FOR_EACH(ss_list, sn)
   {
      fprintf(f, "NEW: %s\n", sn->name);
      if (sn->used)
	 fprintf(f, "WIN: %#lx\n", EwinGetClientXwin(sn->used));
      if ((sn->match_flags & SNAP_MATCH_TITLE) && sn->win_title)
	 fprintf(f, "TITLE: %s\n", sn->win_title);
      if ((sn->match_flags & SNAP_MATCH_NAME) && sn->win_name)
	 fprintf(f, "NAME: %s\n", sn->win_name);
      if ((sn->match_flags & SNAP_MATCH_CLASS) && sn->win_class)
	 fprintf(f, "CLASS: %s\n", sn->win_class);
      if ((sn->match_flags & SNAP_MATCH_ROLE) && sn->win_role)
	 fprintf(f, "ROLE: %s\n", sn->win_role);
      if (sn->track_changes)
	 fprintf(f, "AUTO: yes\n");
      if ((sn->use_flags & SNAP_USE_BORDER) && sn->border_name)
	 fprintf(f, "BORDER: %s\n", sn->border_name);
      if ((sn->use_flags & SNAP_USE_COMMAND) && sn->cmd)
	 fprintf(f, "CMD: %s\n", sn->cmd);
      if (sn->use_flags & SNAP_USE_DESK)
	 fprintf(f, "DESKTOP: %i\n", sn->desktop);
      if (sn->use_flags & SNAP_USE_POS)
	 fprintf(f, "RES: %i %i\n", WinGetW(VROOT), WinGetH(VROOT));
      if (sn->use_flags & SNAP_USE_SIZE)
	 fprintf(f, "WH: %i %i\n", sn->w, sn->h);
      if (sn->use_flags & SNAP_USE_POS)
	 fprintf(f, "XY: %i %i %i %i\n", sn->x, sn->y, sn->area_x, sn->area_y);
      if (sn->use_flags & SNAP_USE_LAYER)
	 fprintf(f, "LAYER: %i\n", sn->layer);
      if (sn->use_flags & SNAP_USE_STICKY)
	 fprintf(f, "STICKY: %i\n", sn->sticky);
      if (sn->use_flags & SNAP_USE_SHADED)
	 fprintf(f, "SHADE: %i\n", sn->shaded);
      if (sn->use_flags & SNAP_USE_SKIP_LISTS)
	{
	   fprintf(f, "SKIPTASK: %i\n", sn->skiptask);
	   fprintf(f, "SKIPWINLIST: %i\n", sn->skipwinlist);
	   fprintf(f, "SKIPFOCUS: %i\n", sn->skipfocus);
	}
      if (sn->use_flags & SNAP_USE_FLAGS)
	 fprintf(f, "FLAGS: %#x %#x\n", sn->flags[0], sn->flags[1]);
#if USE_COMPOSITE
      if (sn->use_flags & SNAP_USE_OPACITY)
	 fprintf(f, "OPACITY: %i %i\n", sn->opacity, sn->focused_opacity);
      if (sn->use_flags & SNAP_USE_SHADOW)
	 fprintf(f, "SHADOW: %i\n", sn->shadow);
#endif
      if (sn->groups)
	{
	   for (j = 0; j < sn->num_groups; j++)
	      fprintf(f, "GROUP: %i\n", sn->groups[j]);
	}
      fprintf(f, "\n");
   }

   fclose(f);

   Esnprintf(buf, sizeof(buf), "%s.snapshots", EGetSavePrefix());

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("SnapshotsSaveReal: %s\n", buf);
   E_mv(s, buf);
   if (!isfile(buf))
      Alert(_("Error saving snaps file"));

   GroupsSave();

 done:
   TIMER_DEL(ss_timer);
}

void
SnapshotsSpawn(void)
{
   Snapshot           *sn;

   ECORE_LIST_FOR_EACH(ss_list, sn)
   {
      if ((sn->use_flags & SNAP_USE_COMMAND) && (sn->cmd) &&
	  !sn->used && !(sn->match_flags & SNAP_MATCH_MULTIPLE))
	{
	   sn->startup_id = ++Mode.apps.startup_id;
	   EspawnCmd(sn->cmd);
	}
   }
}

/* load all snapped info */
static int
_SnapshotsLoad(FILE * fs)
{
   Snapshot           *sn = NULL;
   char                buf[4096], *s;
   int                 res_w, res_h, a, b, c, d;

   res_w = WinGetW(VROOT);
   res_h = WinGetH(VROOT);
   while (fgets(buf, sizeof(buf), fs))
     {
	s = strchr(buf, ':');
	if (!s)
	   continue;
	*s++ = '\0';
	s = Estrtrim(s);
	if (!buf[0] || !s[0])
	   continue;
	if (!strcmp(buf, "NEW"))
	  {
	     res_w = WinGetW(VROOT);
	     res_h = WinGetH(VROOT);
	     sn = _SnapCreate(s);
	  }
	else if (sn)
	  {
	     if (!strcmp(buf, "WIN"))
	       {
		  sn->win = strtoul(s, NULL, 0);
	       }
	     else if (!strcmp(buf, "TITLE"))
	       {
		  sn->win_title = Estrdup(s);
		  sn->match_flags |= SNAP_MATCH_TITLE;
	       }
	     else if (!strcmp(buf, "NAME"))
	       {
		  sn->win_name = Estrdup(s);
		  sn->match_flags |= SNAP_MATCH_NAME;
	       }
	     else if (!strcmp(buf, "CLASS"))
	       {
		  sn->win_class = Estrdup(s);
		  sn->match_flags |= SNAP_MATCH_CLASS;
	       }
	     else if (!strcmp(buf, "ROLE"))
	       {
		  sn->win_role = Estrdup(s);
		  sn->match_flags |= SNAP_MATCH_ROLE;
	       }
	     else if (!strcmp(buf, "AUTO"))
	       {
		  sn->track_changes = 1;
	       }
	     else if (!strcmp(buf, "BORDER"))
	       {
		  sn->use_flags |= SNAP_USE_BORDER;
		  sn->border_name = Estrdup(s);
	       }
	     else if (!strcmp(buf, "CMD"))
	       {
		  sn->use_flags |= SNAP_USE_COMMAND;
		  sn->cmd = Estrdup(s);
	       }
	     else if (!strcmp(buf, "DESKTOP"))
	       {
		  sn->use_flags |= SNAP_USE_DESK;
		  sn->desktop = atoi(s);
	       }
	     else if (!strcmp(buf, "RES"))
	       {
		  if (sscanf(s, "%u %u", &a, &b) < 2)
		     continue;
		  if (a <= 0 || b <= 0)
		     continue;
		  res_w = a;
		  res_h = b;
	       }
	     else if (!strcmp(buf, "WH"))
	       {
		  if (sscanf(s, "%u %u", &a, &b) < 2)
		     continue;
		  if (a <= 0 || b <= 0)
		     continue;
		  sn->use_flags |= SNAP_USE_SIZE;
		  sn->w = a;
		  sn->h = b;
	       }
	     else if (!strcmp(buf, "XY"))
	       {
		  if (sscanf(s, "%d %d %u %u", &a, &b, &c, &d) < 4)
		     continue;
		  sn->use_flags |= SNAP_USE_POS;
		  sn->x = a;
		  sn->y = b;
		  /* we changed reses since we last used this snapshot file */
		  if (res_w != WinGetW(VROOT))
		    {
		       if (sn->use_flags & SNAP_USE_SIZE)
			 {
			    if ((res_w - sn->w) <= 0)
			       sn->x = 0;
			    else
			       sn->x =
				  (sn->x * (WinGetW(VROOT) - sn->w)) /
				  (res_w - sn->w);
			 }
		       else
			 {
			    if (sn->x >= WinGetW(VROOT))
			       sn->x = WinGetW(VROOT) - 32;
			 }
		    }
		  if (res_h != WinGetH(VROOT))
		    {
		       if (sn->use_flags & SNAP_USE_SIZE)
			 {
			    if ((res_h - sn->h) <= 0)
			       sn->y = 0;
			    else
			       sn->y =
				  (sn->y * (WinGetH(VROOT) - sn->h)) /
				  (res_h - sn->h);
			 }
		       else
			 {
			    if (sn->y >= WinGetH(VROOT))
			       sn->y = WinGetH(VROOT) - 32;
			 }
		    }
		  sn->area_x = c;
		  sn->area_y = d;
	       }
	     else if (!strcmp(buf, "LAYER"))
	       {
		  sn->use_flags |= SNAP_USE_LAYER;
		  sn->layer = atoi(s);
	       }
	     else if (!strcmp(buf, "STICKY"))
	       {
		  sn->use_flags |= SNAP_USE_STICKY;
		  sn->sticky = atoi(s);
	       }
	     else if (!strcmp(buf, "SHADE"))
	       {
		  sn->use_flags |= SNAP_USE_SHADED;
		  sn->shaded = atoi(s);
	       }
	     else if (!strcmp(buf, "SKIPFOCUS"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  sn->skipfocus = atoi(s);
	       }
	     else if (!strcmp(buf, "SKIPTASK"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  sn->skiptask = atoi(s);
	       }
	     else if (!strcmp(buf, "SKIPWINLIST"))
	       {
		  sn->use_flags |= SNAP_USE_SKIP_LISTS;
		  sn->skipwinlist = atoi(s);
	       }
	     else if (!strcmp(buf, "FLAGS"))
	       {
		  sn->use_flags |= SNAP_USE_FLAGS;
		  sn->flags[0] = sn->flags[1] = 0;
		  sscanf(s, "%i %i", sn->flags, sn->flags + 1);
	       }
	     else if (!strcmp(buf, "GROUP"))
	       {
		  sn->use_flags |= SNAP_USE_GROUPS;
		  sn->num_groups++;
		  sn->groups = EREALLOC(int, sn->groups, sn->num_groups);

		  sn->groups[sn->num_groups - 1] = atoi(s);
		  GroupRemember(sn->groups[sn->num_groups - 1]);
	       }
#if USE_COMPOSITE
	     else if (!strcmp(buf, "OPACITY"))
	       {
		  sn->use_flags |= SNAP_USE_OPACITY;
		  a = 100;
		  b = 100;
		  sscanf(s, "%i %i", &a, &b);
		  if (b == 1)
		     b = 100;	/* BW compat - focused is opaque */
		  sn->opacity = a;
		  sn->focused_opacity = b;
	       }
	     else if (!strcmp(buf, "SHADOW"))
	       {
		  sn->use_flags |= SNAP_USE_SHADOW;
		  sn->shadow = atoi(s);
	       }
#endif
	  }
     }

   return 0;
}

void
SnapshotsLoad(void)
{
   char                s[4096];

   GroupsLoad();

   Esnprintf(s, sizeof(s), "%s.snapshots", EGetSavePrefix());

   ConfigFileLoad(s, NULL, _SnapshotsLoad, 0);
}

/* make a client window conform to snapshot info */
void
SnapshotEwinApply(EWin * ewin)
{
   Snapshot           *sn;
   int                 ax, ay;
   unsigned int        use_flags;

   _SnapEwinFind(ewin);		/* Find a saved settings match */

   sn = ewin->snap;
   if (!sn)
     {
	if (ewin->props.autosave)
	   _EwinSnapSet(ewin, SNAP_MATCH_DEFAULT, SNAP_USE_ALL | SNAP_AUTO);
	return;
     }

   if (ewin->props.autosave)
      sn->track_changes = 1;

   use_flags = sn->use_flags;
   /* If restarting don't override stuff set in attributes/properties */
   if (ewin->state.identified)
      use_flags &= SNAP_USE_LAYER | SNAP_USE_SHADOW | SNAP_USE_GROUPS |
	 SNAP_USE_OPACITY;

   if (use_flags & SNAP_USE_STICKY)
      EoSetSticky(ewin, sn->sticky);

   if (use_flags & SNAP_USE_DESK)
      EoSetDesk(ewin, DeskGetValid(sn->desktop));

   if (use_flags & SNAP_USE_SIZE)
     {
	ewin->client.w = sn->w;
	ewin->client.h = sn->h;
	ewin->state.maximized_horz = ewin->state.maximized_vert = 0;
     }

   if (use_flags & SNAP_USE_POS)
     {
	ewin->state.placed = 1;
	ewin->client.x = sn->x;
	ewin->client.y = sn->y;
#if 0				/* No, do later in EwinDetermineArea() */
	ewin->area_x = sn->area_x;
	ewin->area_y = sn->area_y;
#endif
	if (!EoIsSticky(ewin))
	  {
	     DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	     ewin->client.x += ((sn->area_x - ax) * WinGetW(VROOT));
	     ewin->client.y += ((sn->area_y - ay) * WinGetH(VROOT));
	  }
     }

   if (use_flags & SNAP_USE_LAYER)
      EoSetLayer(ewin, sn->layer);

   if (use_flags & SNAP_USE_SKIP_LISTS)
     {
	ewin->props.skip_focuslist = sn->skipfocus;
	ewin->props.skip_ext_task = sn->skiptask;
	ewin->props.skip_winlist = sn->skipwinlist;
     }

   if (use_flags & SNAP_USE_FLAGS)
      EwinFlagsDecode(ewin, sn->flags);

   if (use_flags & SNAP_USE_SHADED)
      ewin->state.shaded = sn->shaded;

   if (use_flags & SNAP_USE_BORDER)
      EwinBorderSetInitially(ewin, sn->border_name);

   if (use_flags & SNAP_USE_GROUPS)
      GroupsEwinAdd(ewin, sn->groups, sn->num_groups);

#if USE_COMPOSITE
   if (use_flags & SNAP_USE_OPACITY)
     {
	sn->opacity = OpacityFix(sn->opacity, 0);
	sn->focused_opacity = OpacityFix(sn->focused_opacity, 0);
	ewin->ewmh.opacity = OpacityFromPercent(sn->opacity);
	ewin->props.focused_opacity = OpacityFromPercent(sn->focused_opacity);
     }

   if (use_flags & SNAP_USE_SHADOW)
      EoSetShadow(ewin, sn->shadow);
#endif

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get snap  %#lx: %4d+%4d %4dx%4d: %s\n",
	      EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetTitle(ewin));
}

/* Detach snapshot from ewin */
void
SnapshotEwinUnmatch(EWin * ewin)
{
   Snapshot           *sn;

   sn = ewin->snap;
   if (!sn)
      return;

   ewin->snap = NULL;
   sn->used = NULL;
}

void
SnapshotEwinParse(EWin * ewin, const char *params)
{
   char                param[1024];
   const char         *p;
   int                 len;
   unsigned int        match_flags, use_flags;

   p = params;
   if (!p)
      return;

   match_flags = SNAP_MATCH_DEFAULT;
   use_flags = 0;

   for (;;)
     {
	param[0] = '\0';
	len = 0;
	sscanf(p, "%s %n", param, &len);
	if (len <= 0)
	   break;
	p += len;

	if (!strcmp(param, "all"))
	  {
	     use_flags = SNAP_USE_ALL;
	  }
#if ENABLE_DIALOGS
	else if (!strcmp(param, "dialog"))
	  {
	     _EwinSnapDialog(ewin);
	     break;
	  }
#endif
	else if (!strcmp(param, "none"))
	   _EwinSnapRemove(ewin);
	else if (!strcmp(param, "auto"))
	   use_flags |= SNAP_AUTO;
	else if (!strcmp(param, "border"))
	   use_flags |= SNAP_USE_BORDER;
	else if (!strcmp(param, "command"))
	   use_flags |= SNAP_USE_COMMAND;
	else if (!strcmp(param, "desktop"))
	   use_flags |= SNAP_USE_DESK;
	else if (!strcmp(param, "location"))
	   use_flags |= SNAP_USE_POS;
	else if (!strcmp(param, "size"))
	   use_flags |= SNAP_USE_SIZE;
	else if (!strcmp(param, "layer"))
	   use_flags |= SNAP_USE_LAYER;
	else if (!strcmp(param, "shade"))
	   use_flags |= SNAP_USE_SHADED;
	else if (!strcmp(param, "sticky"))
	   use_flags |= SNAP_USE_STICKY;
#if USE_COMPOSITE
	else if (!strcmp(param, "opacity"))
	   use_flags |= SNAP_USE_OPACITY;
	else if (!strcmp(param, "shadow"))
	   use_flags |= SNAP_USE_SHADOW;
#endif
	else if (!strcmp(param, "group"))
	   use_flags |= SNAP_USE_GROUPS;
     }

   if (ewin->snap)
     {
	match_flags = ewin->snap->match_flags;
	use_flags |= ewin->snap->use_flags;
     }

   _EwinSnapSet(ewin, match_flags, use_flags);

   SnapshotsSave();
}

/*
 * IPC functions
 * A bit ugly...
 */
const char          SnapshotsIpcText[] =
   "usage:\n" "  list_remember [full]\n"
   "  Retrieve a list of remembered windows.  with full, the list\n"
   "  includes the window's remembered attributes\n";

#define SS(s) ((s) ? (s) : NoText)
static const char   NoText[] = "-NONE-";

static void
_SnapShow(void *data, void *prm)
{
   Snapshot           *sn = (Snapshot *) data;
   int                 full = ! !prm;
   char                buf[FILEPATH_LEN_MAX];
   const char         *name;

   name = (sn->name) ? sn->name : "???";

   if (!full)
     {
	if (sn->used)
	   IpcPrintf("%s\n", name);
	else
	   IpcPrintf("%s (unused)\n", name);
	return;
     }

#define SU(sn, item) ((sn->match_flags & item) ? '>' : ':')

   if (sn->used)
      Esnprintf(buf, sizeof(buf), "In use - %#lx", EwinGetClientXwin(sn->used));
   else
      Esnprintf(buf, sizeof(buf), "*** Unused ***");
   IpcPrintf(" Snapshot  Name: %s    %s\n", name, buf);
   if (sn->win_title)
      IpcPrintf("   Window Title%c %s\n", SU(sn, SNAP_MATCH_TITLE),
		sn->win_title);
   if (sn->win_name)
      IpcPrintf("   Window  Name%c %s\n", SU(sn, SNAP_MATCH_NAME),
		sn->win_name);
   if (sn->win_class)
      IpcPrintf("   Window Class%c %s\n", SU(sn, SNAP_MATCH_CLASS),
		sn->win_class);
   if (sn->win_role)
      IpcPrintf("   Window  Role%c %s\n", SU(sn, SNAP_MATCH_ROLE),
		sn->win_role);

   if (sn->track_changes)
      IpcPrintf("      Tracking changes\n");
   if (sn->use_flags & SNAP_USE_BORDER)
      IpcPrintf("      Border Name: %s\n", SS(sn->border_name));
   if (sn->use_flags & SNAP_USE_DESK)
      IpcPrintf("          desktop: %d\n", sn->desktop);
   if (sn->use_flags & SNAP_USE_POS)
      IpcPrintf("           (x, y): %d, %d    area (x, y): %d, %d\n",
		sn->x, sn->y, sn->area_x, sn->area_y);
   if (sn->use_flags & SNAP_USE_SIZE)
      IpcPrintf("           (w, h): %d, %d\n", sn->w, sn->h);
   if (sn->use_flags & SNAP_USE_LAYER)
      IpcPrintf("            layer: %d\n", sn->layer);
   if (sn->use_flags & SNAP_USE_STICKY)
      IpcPrintf("           sticky: %d\n", sn->sticky);
   if (sn->use_flags & SNAP_USE_SHADED)
      IpcPrintf("            shade: %d\n", sn->shaded);
   if (sn->use_flags & SNAP_USE_COMMAND)
      IpcPrintf("          command: %s\n", SS(sn->cmd));
   if (sn->use_flags & SNAP_USE_SKIP_LISTS)
      IpcPrintf
	 ("         skiptask: %d    skipfocus: %d    skipwinlist: %d\n",
	  sn->skiptask, sn->skipfocus, sn->skipwinlist);
   if (sn->use_flags & SNAP_USE_FLAGS)
      IpcPrintf("            flags: %#x %#x\n", sn->flags[0], sn->flags[1]);
   IpcPrintf("\n");
}

void
SnapshotsIpcFunc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (ecore_list_count(ss_list) <= 0)
     {
	IpcPrintf("No remembered windows\n");
	return;
     }

   if (!p || cmd[0] == '?')
     {
	ecore_list_for_each(ss_list, _SnapShow, NULL);
     }
   else
     {
	ecore_list_for_each(ss_list, _SnapShow, (void *)1L);
     }
}
