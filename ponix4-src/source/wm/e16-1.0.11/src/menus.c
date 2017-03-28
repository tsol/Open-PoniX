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
#include "eimage.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "file.h"
#include "grabs.h"
#include "hints.h"
#include "iclass.h"
#include "menus.h"
#include "screen.h"
#include "settings.h"
#include "slide.h"
#include "tclass.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <time.h>
#include <X11/keysym.h>

#define DEBUG_MENU_EVENTS 0

#define MENU_UNLOAD_CHECK_INTERVAL 300	/* Seconds */

static struct {
   Menu               *first;
   Menu               *active;
   EWin               *context_ewin;
   EObj               *cover_win;
   char                just_shown;
   char                cover_bpress;
} Mode_menus;

struct _menustyle {
   char               *name;
   TextClass          *tclass;
   ImageClass         *bg_iclass;
   ImageClass         *item_iclass;
   ImageClass         *sub_iclass;
   char                use_item_bg;
   char                iconpos;
   int                 maxx;
   int                 maxy;
   char               *border_name;
   unsigned int        ref_count;
};

struct _menuitem {
   Menu               *menu;
   ImageClass         *icon_iclass;
   char               *text;
   char               *params;
   Menu               *child;
   char                state;
   PmapMask            pmm[3];
   Win                 win;
   Win                 icon_win;
   short               icon_w;
   short               icon_h;
   short               text_w;
   short               text_h;
   short               text_x;
   short               text_y;
};

struct _menu {
   EWin               *ewin;
   Win                 win;
   PmapMask            pmm;
   int                 w, h;
   char               *name;
   char               *title;
   char               *alias;
   MenuStyle          *style;
   MenuLoader         *loader;
   int                 num;
   MenuItem          **items;
   int                 icon_size;
   char                internal;	/* Don't destroy when reloading */
   char                dynamic;	/* Empty when closed */
   char                transient;	/* Destroy when closed */
   char                shown;
   char                redraw;
   char                filled;	/* Has been filled */
   Menu               *parent;
   Menu               *child;
   MenuItem           *sel_item;
   time_t              last_change;
   time_t              last_access;
   void               *data;
   unsigned int        ref_count;
};

#define MENU_ITEM_EVENT_MASK \
	KeyPressMask | KeyReleaseMask | \
	ButtonPressMask | ButtonReleaseMask | \
	EnterWindowMask | LeaveWindowMask

static void         MenuRedraw(Menu * m);
static void         MenuRealize(Menu * m);
static void         MenuActivateItem(Menu * m, MenuItem * mi);
static void         MenuDrawItem(Menu * m, MenuItem * mi, char shape,
				 int state);

static void         MenuHandleEvents(Win win, XEvent * ev, void *m);
static void         MenuItemHandleEvents(Win win, XEvent * ev, void *mi);
static void         MenuMaskerHandleEvents(Win win, XEvent * ev, void *prm);

static void         MenusHide(void);

static Ecore_List  *menu_list = NULL;
static Ecore_List  *menu_style_list = NULL;
static Timer       *menu_timer_submenu = NULL;

static MenuItem    *
MenuFindItemByChild(Menu * m, Menu * mc)
{
   int                 i;

   if (!mc)
      return (m->num) ? m->items[0] : NULL;

   for (i = 0; i < m->num; i++)
     {
	if (mc == m->items[i]->child)
	   return m->items[i];
     }

   return NULL;
}

static void
MenuHide(Menu * m)
{
   EWin               *ewin;

   if (!m || !m->shown)
      return;

   if (m->sel_item)
      MenuDrawItem(m, m->sel_item, 1, STATE_NORMAL);
   m->sel_item = NULL;

   ewin = m->ewin;
   if (ewin)
     {
	EUnmapWindow(m->win);
	EReparentWindow(m->win, VROOT, ewin->client.x, ewin->client.y);
	EwinHide(ewin);
     }
   m->ewin = NULL;

   m->shown = 0;
   m->parent = NULL;
   m->last_access = time(0);
   if (m->child)
     {
	MenuHide(m->child);
	m->child = NULL;
     }
   if (m->dynamic)
      MenuEmpty(m, 0);
   m->ref_count--;
}

static void
_MenuEwinInit(EWin * ewin)
{
   Menu               *m = (Menu *) ewin->data;

   m->ewin = ewin;

   EwinSetTitle(ewin, _(m->title));
   EwinSetClass(ewin, m->name, "Enlightenment_Menu");

   ewin->props.skip_ext_task = 1;
   ewin->props.skip_ext_pager = 1;
   ewin->props.no_actions = 1;
   ewin->props.skip_focuslist = 1;
   ewin->props.skip_winlist = 1;
   ewin->props.ignorearrange = 1;
   ewin->state.active = 1;
   EwinInhSetWM(ewin, focus, 1);

   ICCCM_SetSizeConstraints(ewin, m->w, m->h, m->w, m->h, 0, 0, 1, 1,
			    0.0, 65535.0);
   ewin->icccm.grav = StaticGravity;

   EoSetLayer(ewin, 12);
   ewin->ewmh.opacity = OpacityFromPercent(Conf.opacity.menus);
}

static void
_MenuEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Menu               *m = (Menu *) ewin->data;

   if (!m)
      return;

   if (Mode.mode != MODE_NONE && !m->redraw)
      return;

   if (TransparencyUpdateNeeded())
      m->redraw = 1;

   if ((!m->style->use_item_bg && m->pmm.pmap == 0) || m->redraw)
     {
	MenuRedraw(m);
	EwinUpdateShapeInfo(ewin);
     }
}

static void
_MenuEwinClose(EWin * ewin)
{
   Menu               *m = (Menu *) ewin->data;

   if (m == Mode_menus.active)
     {
	GrabKeyboardRelease();
	Mode_menus.active = NULL;
     }

   ewin->data = NULL;
}

static const EWinOps _MenuEwinOps = {
   _MenuEwinInit,
   NULL,
   _MenuEwinMoveResize,
   _MenuEwinClose,
};

static void         MenuShowMasker(Menu * m);

int
MenuLoad(Menu * m)
{
   if (!m || !m->loader)
      return 0;

   return m->loader(m);
}

static void
_MenuEwinShow(EWin * ewin)
{
   ICCCM_Cmap(NULL);
   EwinOpFloatAt(ewin, OPSRC_NA, EoGetX(ewin), EoGetY(ewin));
   EwinRaise(ewin);
   EwinShow(ewin);
   if (Conf.menus.animate)
      EwinUnShade(ewin);
}

static void
MenuShow(Menu * m, char noshow)
{
   EWin               *ewin;
   int                 x, y, w, h;
   int                 wx, wy, mw, mh;
   int                 head_num = 0;

   if (m->shown)
      return;

   if (MenuLoad(m) || m->redraw)
      MenuRealize(m);

   if (m->num <= 0)
      return;

   if (!m->win || !m->items[0]->win)
      MenuRealize(m);

   if (!m->style)
      return;

   ewin = m->ewin;
   if (ewin)
     {
#if 0				/* ??? */
	EwinRaise(ewin);
	EwinShow(ewin);
	return;
#else
	MenuHide(m);
#endif
     }

   EGetGeometry(m->items[0]->win, NULL, &x, &y, &w, &h, NULL, NULL);
   mw = m->w;
   mh = m->h;

   EQueryPointer(NULL, &wx, &wy, NULL, NULL);
   wx -= EoGetX(DesksGetCurrent()) + x + (w / 2);
   wy -= EoGetY(DesksGetCurrent()) + y + (h / 2);
   if (Conf.menus.onscreen)
     {
	Border             *b;

	b = BorderFind(m->style->border_name);
	if (b)
	  {
	     int                 sx, sy, sw, sh;

	     head_num = ScreenGetGeometryByPointer(&sx, &sy, &sw, &sh);

	     if (wx > sx + sw - mw - b->border.right)
		wx = sx + sw - mw - b->border.right;
	     if (wx < sx + b->border.left)
		wx = sx + b->border.left;

	     if (wy > sy + sh - mh - b->border.bottom)
		wy = sy + sh - mh - b->border.bottom;
	     if (wy < sy + b->border.top)
		wy = sy + b->border.top;
	  }
     }

   EMoveWindow(m->win, wx, wy);

   ewin = AddInternalToFamily(m->win, m->style->border_name, EWIN_TYPE_MENU,
			      &_MenuEwinOps, m);
   if (ewin)
     {
	ewin->client.event_mask |= KeyPressMask;
	ESelectInput(m->win, ewin->client.event_mask);

	ewin->head = head_num;

	EwinResize(ewin, ewin->client.w, ewin->client.h, 0);

	if (Conf.menus.animate)
	   EwinInstantShade(ewin, 0);

	if (!noshow)
	   _MenuEwinShow(ewin);
     }

   m->shown = 1;
   m->last_access = time(0);
   Mode_menus.just_shown = 1;

   if (!Mode_menus.first)
     {
	Mode_menus.context_ewin = GetContextEwin();
#if 0
	Eprintf("Mode_menus.context_ewin set %s\n",
		EwinGetTitle(Mode_menus.context_ewin));
#endif
	ESync(ESYNC_MENUS);
	Mode_menus.first = m;
	MenuShowMasker(m);
	TooltipsEnable(0);
	GrabKeyboardSet(m->win);
     }
   m->ref_count++;
}

static MenuStyle   *
MenuStyleCreate(const char *name)
{
   MenuStyle          *ms;

   ms = ECALLOC(MenuStyle, 1);
   if (!ms)
      return NULL;

   if (!menu_style_list)
      menu_style_list = ecore_list_new();
   ecore_list_prepend(menu_style_list, ms);

   ms->name = Estrdup(name);
   ms->iconpos = ICON_LEFT;

   return ms;
}

MenuItem           *
MenuItemCreate(const char *text, ImageClass * iclass,
	       const char *action_params, Menu * child)
{
   MenuItem           *mi;

   mi = ECALLOC(MenuItem, 1);

   mi->icon_iclass = iclass;
   mi->text = (text) ? Estrdup((text[0]) ? text : "?!?") : NULL;
   mi->params = Estrdup(action_params);
   mi->child = child;
   if (child)
      child->ref_count++;
   mi->state = STATE_NORMAL;

   return mi;
}

static int
_MenuStyleMatchName(const void *data, const void *match)
{
   return strcmp(((const MenuStyle *)data)->name, (const char *)match);
}

MenuStyle          *
MenuStyleFind(const char *name)
{
   MenuStyle          *ms;

   ms = (MenuStyle *) ecore_list_find(menu_style_list, _MenuStyleMatchName,
				      name);
   if (ms)
      return ms;

   ms = (MenuStyle *) ecore_list_find(menu_style_list, _MenuStyleMatchName,
				      "__fb_ms");
   if (ms)
      return ms;

   ms = MenuStyleCreate("__fb_ms");
   if (!ms)
      return ms;

   ms->tclass = TextclassFind(NULL, 1);
   ms->bg_iclass = ImageclassFind(NULL, 1);
   ms->item_iclass = ImageclassFind(NULL, 1);
   ms->sub_iclass = ImageclassFind(NULL, 1);

   return ms;
}

void
MenuSetInternal(Menu * m)
{
   m->internal = 1;
}

void
MenuSetDynamic(Menu * m)
{
   m->dynamic = 1;
}

void
MenuSetTransient(Menu * m)
{
   m->transient = 1;
}

void
MenuSetName(Menu * m, const char *name)
{
   _EFDUP(m->name, name);
}

void
MenuSetAlias(Menu * m, const char *alias)
{
   _EFDUP(m->alias, alias);
}

void
MenuSetTitle(Menu * m, const char *title)
{
   _EFDUP(m->title, title);
}

void
MenuSetIconSize(Menu * m, int size)
{
   if (size > 48)
      size = 48;
   m->icon_size = size;
}

void
MenuSetData(Menu * m, char *data)
{
   Efree(m->data);
   m->data = data;
}

void
MenuSetLoader(Menu * m, MenuLoader * loader)
{
   m->loader = loader;
}

void
MenuSetTimestamp(Menu * m, time_t t)
{
   m->last_change = t;
}

const char         *
MenuGetName(const Menu * m)
{
   return m->name;
}

const char         *
MenuGetData(const Menu * m)
{
   return (const char *)m->data;
}

time_t
MenuGetTimestamp(const Menu * m)
{
   return m->last_change;
}

void
MenuSetStyle(Menu * m, MenuStyle * ms)
{
   if (m->style)
      m->style->ref_count--;
   if (!ms && m->parent)
      ms = m->parent->style;
   if (!ms)
      ms = MenuStyleFind("DEFAULT");
   m->style = ms;
   if (ms)
      ms->ref_count++;
}

Menu               *
MenuCreate(const char *name, const char *title, Menu * parent, MenuStyle * ms)
{
   Menu               *m;

   m = ECALLOC(Menu, 1);
   if (!m)
      return m;

   m->parent = parent;
   MenuSetName(m, name);
   MenuSetTitle(m, title);
   if (ms)
      MenuSetStyle(m, ms);
   m->icon_size = -1;		/* Use image size */

   if (!menu_list)
      menu_list = ecore_list_new();
   ecore_list_append(menu_list, m);

   return m;
}

static void
MenuDestroy(Menu * m)
{
   if (!m)
      return;

   if (!ecore_list_goto(menu_list, m))
      return;

   MenuHide(m);
   MenuEmpty(m, 1);

   if (m->ref_count)
      return;

   ecore_list_node_remove(menu_list, m);

   if (m->win)
      EDestroyWindow(m->win);

   Efree(m->name);
   Efree(m->alias);
   Efree(m->title);
   Efree(m->data);

   Efree(m);
}

/* NB - this doesnt free imageclasses if we created them for the menu
 * FIXME: so it will leak if we create new imageclasses and stop using
 * old ones for menu icons. we need to add some ref counting in menu icon
 * imageclasses to knw to free them when not used
 */
void
MenuEmpty(Menu * m, int destroying)
{
   int                 i, j;
   MenuItem           *mi;

   for (i = 0; i < m->num; i++)
     {
	mi = m->items[i];

	if (mi->child)
	  {
	     mi->child->ref_count--;
	     MenuDestroy(mi->child);
	  }
	Efree(mi->text);
	Efree(mi->params);
	for (j = 0; j < 3; j++)
	   PmapMaskFree(&(mi->pmm[j]));
	if (!destroying && mi->win)
	   EDestroyWindow(mi->win);
	else
	   EventCallbackUnregister(mi->win, MenuItemHandleEvents, mi);
	ImageclassFree(mi->icon_iclass);
	Efree(mi);
     }
   Efree(m->items);
   m->items = NULL;
   m->num = 0;
   m->sel_item = NULL;

   PmapMaskFree(&m->pmm);

   m->last_change = 0;
   m->filled = 0;
}

static void
MenuFreePixmaps(Menu * m)
{
   int                 i, j;
   MenuItem           *mi;

   for (i = 0; i < m->num; i++)
     {
	mi = m->items[i];

	for (j = 0; j < 3; j++)
	   PmapMaskFree(mi->pmm + j);
     }

   PmapMaskFree(&m->pmm);

   m->last_change = 0;
   m->filled = 0;
}

void
MenuAddItem(Menu * m, MenuItem * item)
{
   MenuItem          **items;

   if (!item)
      return;

   items = EREALLOC(MenuItem *, m->items, m->num + 1);
   if (!items)
      return;

   items[m->num] = item;
   m->items = items;
   m->num++;
}

static void
MenuRealize(Menu * m)
{
   int                 i, maxh, maxw, nmaxy;
   int                 maxx1, maxx2, w, h, x, y, r, mmw, mmh;
   EImage             *im;
   EImageBorder       *pad, *pad_item, *pad_sub;
   char                has_i, has_s;

   if (m->num <= 0)
      return;

   if (!m->style)
     {
	MenuSetStyle(m, NULL);
	if (!m->style)
	   return;
     }

   if (!m->win)
     {
	m->win = ECreateClientWindow(VROOT, 0, 0, 1, 1);
	EventCallbackRegister(m->win, MenuHandleEvents, m);
     }

   maxh = maxw = 0;
   maxx1 = 0;
   maxx2 = 0;
   has_i = 0;
   has_s = 0;

   for (i = 0; i < m->num; i++)
     {
	m->items[i]->menu = m;

	if (m->items[i]->child)
	   has_s = 1;
	else
	   has_i = 1;

	m->items[i]->win = ECreateWindow(m->win, 0, 0, 1, 1, 0);
	EventCallbackRegister(m->items[i]->win, MenuItemHandleEvents,
			      m->items[i]);
	ESelectInput(m->items[i]->win, MENU_ITEM_EVENT_MASK);
	EMapWindow(m->items[i]->win);

	if ((m->style->tclass) && (m->items[i]->text))
	  {
	     TextSize(m->style->tclass, 0, 0, 0, _(m->items[i]->text), &w, &h,
		      17);
	     if (h > maxh)
		maxh = h;
	     if (w > maxx1)
		maxx1 = w;
	     m->items[i]->text_w = w;
	     m->items[i]->text_h = h;
	  }
	if (m->items[i]->icon_iclass && Conf.menus.show_icons)
	  {
	     im = ImageclassGetImage(m->items[i]->icon_iclass, 0, 0, 0);
	     if (im)
	       {
		  w = h = 0;
		  if (m->icon_size > 0)
		     w = h = m->icon_size;
		  else if (m->icon_size == 0)
		     w = h = Conf.menus.icon_size;
		  if (w <= 0 || h <= 0)
		     EImageGetSize(im, &w, &h);
		  m->items[i]->icon_w = w;
		  m->items[i]->icon_h = h;
		  m->items[i]->icon_win =
		     ECreateWindow(m->items[i]->win, 0, 0, w, h, 0);
		  EMapWindow(m->items[i]->icon_win);
		  if (h > maxh)
		     maxh = h;
		  if (w > maxx2)
		     maxx2 = w;
		  EImageFree(im);
	       }
	     else
		m->items[i]->icon_iclass = NULL;
	  }
     }

   pad = ImageclassGetPadding(m->style->bg_iclass);
   pad_item = ImageclassGetPadding(m->style->item_iclass);
   pad_sub = ImageclassGetPadding(m->style->sub_iclass);
   if (((has_i) && (has_s)) || ((!has_i) && (!has_s)))
     {
	if (pad_item->top > pad_sub->top)
	   maxh += pad_item->top;
	else
	   maxh += pad_sub->top;
	if (pad_item->bottom > pad_sub->bottom)
	   maxh += pad_item->bottom;
	else
	   maxh += pad_sub->bottom;
	maxw = maxx1 + maxx2;
	if (pad_item->left > pad_sub->left)
	   maxw += pad_item->left;
	else
	   maxw += pad_sub->left;
	if (pad_item->right > pad_sub->right)
	   maxw += pad_item->right;
	else
	   maxw += pad_sub->right;
     }
   else if (has_i)
     {
	maxh += pad_item->top;
	maxh += pad_item->bottom;
	maxw = maxx1 + maxx2;
	maxw += pad_item->left;
	maxw += pad_item->right;
     }
   else if (has_s)
     {
	maxh += pad_sub->top;
	maxh += pad_sub->bottom;
	maxw = maxx1 + maxx2;
	maxw += pad_sub->left;
	maxw += pad_sub->right;
     }

   mmw = 0;
   mmh = 0;

   nmaxy = 3 * WinGetH(VROOT) / (4 * maxh + 1);
   if (m->style->maxy && nmaxy > m->style->maxy)
      nmaxy = m->style->maxy;

   r = 0;
   x = 0;
   y = 0;
   for (i = 0; i < m->num; i++)
     {
	if (r == 0 && (m->style->bg_iclass) && (!m->style->use_item_bg))
	  {
	     x += pad->left;
	     y += pad->top;
	  }
	EMoveResizeWindow(m->items[i]->win, x, y, maxw, maxh);
	if (m->style->iconpos == ICON_LEFT)
	  {
	     m->items[i]->text_x = pad_item->left + maxx2;
	     m->items[i]->text_w = maxx1;
	     m->items[i]->text_y = (maxh - m->items[i]->text_h) / 2;
	     if (m->items[i]->icon_win)
		EMoveWindow(m->items[i]->icon_win,
			    pad_item->left +
			    ((maxx2 - m->items[i]->icon_w) / 2),
			    ((maxh - m->items[i]->icon_h) / 2));
	  }
	else
	  {
	     m->items[i]->text_x = pad_item->left;
	     m->items[i]->text_w = maxx1;
	     m->items[i]->text_y = (maxh - m->items[i]->text_h) / 2;
	     if (m->items[i]->icon_win)
		EMoveWindow(m->items[i]->icon_win,
			    maxw - pad_item->right - maxx2 +
			    ((maxx2 - m->items[i]->icon_w) / 2),
			    ((maxh - m->items[i]->icon_h) / 2));
	  }
	if (m->items[i]->icon_iclass && Conf.menus.show_icons)
	  {
	     ImageclassApply(m->items[i]->icon_iclass, m->items[i]->icon_win,
			     0, 0, STATE_NORMAL, ST_MENU_ITEM);
	  }
	if (x + maxw > mmw)
	   mmw = x + maxw;
	if (y + maxh > mmh)
	   mmh = y + maxh;
	if ((m->style->maxx) || (nmaxy))
	  {
	     if (nmaxy)
	       {
		  y += maxh;
		  r++;
		  if (r >= nmaxy)
		    {
		       r = 0;
		       x += maxw;
		       y = 0;
		    }
	       }
	     else
	       {
		  x += maxw;
		  r++;
		  if (r >= m->style->maxx)
		    {
		       r = 0;
		       y += maxh;
		       x = 0;
		    }
	       }
	  }
	else
	   y += maxh;
     }

   if ((m->style->bg_iclass) && (!m->style->use_item_bg))
     {
	mmw += pad->right;
	mmh += pad->bottom;
     }

   m->redraw = 1;
   m->w = mmw;
   m->h = mmh;
   EResizeWindow(m->win, mmw, mmh);
}

static void
MenuRedraw(Menu * m)
{
   int                 i, j;

   if (m->redraw)
     {
	for (i = 0; i < m->num; i++)
	  {
	     for (j = 0; j < 3; j++)
		PmapMaskFree(&(m->items[i]->pmm[j]));

	  }
	m->redraw = 0;
     }

   if (!m->style->use_item_bg)
     {
	PmapMaskFree(&m->pmm);
	ImageclassApplyCopy(m->style->bg_iclass, m->win, m->w, m->h, 0,
			    0, STATE_NORMAL, &m->pmm, IC_FLAG_MAKE_MASK,
			    ST_MENU);
	EGetWindowBackgroundPixmap(m->win);
	EXCopyAreaTiled(m->pmm.pmap, None, WinGetPmap(m->win),
			0, 0, m->w, m->h, 0, 0);
	EShapeSetMask(m->win, 0, 0, m->pmm.mask);
	EClearWindow(m->win);
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0, -1);
     }
   else
     {
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0, -1);
	EShapePropagate(m->win);
     }

   m->filled = 1;
}

static void
MenuDrawItem(Menu * m, MenuItem * mi, char shape, int state)
{
   PmapMask           *mi_pmm;

   if (state >= 0)
      mi->state = state;
   mi_pmm = &(mi->pmm[(int)(mi->state)]);

   if (!mi_pmm->pmap)
     {
	int                 x, y, w, h;
	int                 item_type;
	ImageClass         *ic;
	GC                  gc;
	PmapMask            pmm;

	EGetGeometry(mi->win, NULL, &x, &y, &w, &h, NULL, NULL);

	mi_pmm->type = 0;
	mi_pmm->pmap = ECreatePixmap(mi->win, w, h, 0);
	mi_pmm->mask = None;

	ic = (mi->child) ? m->style->sub_iclass : m->style->item_iclass;
	item_type = (mi->state != STATE_NORMAL) ? ST_MENU_ITEM : ST_MENU;

	if (!m->style->use_item_bg)
	  {
	     gc = EXCreateGC(m->pmm.pmap, 0, NULL);
	     XCopyArea(disp, WinGetPmap(m->win), mi_pmm->pmap, gc, x, y, w, h,
		       0, 0);
	     if ((mi->state != STATE_NORMAL) || (mi->child))
	       {
		  ImageclassApplyCopy(ic, mi->win, w, h, 0, 0, mi->state, &pmm,
				      IC_FLAG_MAKE_MASK, item_type);
		  EXCopyAreaTiled(pmm.pmap, pmm.mask, mi_pmm->pmap,
				  0, 0, w, h, 0, 0);
		  PmapMaskFree(&pmm);
	       }
	     EXFreeGC(gc);
	  }
	else
	  {
	     ImageclassApplyCopy(ic, mi->win, w, h, 0, 0, mi->state, &pmm,
				 IC_FLAG_MAKE_MASK, item_type);
	     EXCopyAreaTiled(pmm.pmap, pmm.mask, mi_pmm->pmap,
			     0, 0, w, h, 0, 0);
	     PmapMaskFree(&pmm);
	  }

	if (mi->text)
	  {
	     TextDraw(m->style->tclass, mi->win, mi_pmm->pmap, 0, 0, mi->state,
		      _(mi->text), mi->text_x, mi->text_y, mi->text_w,
		      mi->text_h, 17,
		      TextclassGetJustification(m->style->tclass));
	  }
     }

   ESetWindowBackgroundPixmap(mi->win, mi_pmm->pmap);
   EShapeSetMask(mi->win, 0, 0, mi_pmm->mask);
   EClearWindow(mi->win);

   if ((shape) && (m->style->use_item_bg))
      EShapePropagate(m->win);
}

static void
MenuShowMasker(Menu * m __UNUSED__)
{
   EObj               *eo = Mode_menus.cover_win;

   if (!eo)
     {
	eo = EobjWindowCreate(EOBJ_TYPE_EVENT,
			      0, 0, WinGetW(VROOT), WinGetH(VROOT),
			      0, "Masker");
	if (!eo)
	   return;

	EobjReparent(eo, EoObj(DesksGetCurrent()), 0, 0);
	EobjSetLayer(eo, 11);
	ESelectInput(EobjGetWin(eo), ButtonPressMask | ButtonReleaseMask |
		     EnterWindowMask | LeaveWindowMask);
	EventCallbackRegister(EobjGetWin(eo), MenuMaskerHandleEvents, NULL);

	Mode_menus.cover_win = eo;
     }

   Mode_menus.cover_bpress = 0;
   EobjMap(eo, 0);
}

static void
MenuHideMasker(void)
{
   EObj               *eo = Mode_menus.cover_win;

   if (!eo)
      return;

   EventCallbackUnregister(EobjGetWin(eo), MenuMaskerHandleEvents, NULL);
   EobjWindowDestroy(eo);
   Mode_menus.cover_win = NULL;
}

static void
MenusDestroyLoaded(void)
{
   Menu               *m;
   int                 found_one;

   MenusHide();

   /* Free all menustyles first (gulp) */
   do
     {
	found_one = 0;
	ECORE_LIST_FOR_EACH(menu_list, m)
	{
	   if (m->internal)
	      continue;
	   if (m->ref_count)
	      continue;

	   MenuDestroy(m);
	   /* Destroying a menu may result in sub-menus being
	    * destroyed too, so we have to re-find all menus
	    * afterwards. Inefficient yes, but it works...
	    */
	   found_one = 1;
	   break;
	}
     }
   while (found_one);
}

static int
_MenuMatchName(const void *data, const void *match)
{
   const Menu         *m = (const Menu *)data;

   if ((m->name && !strcmp((const char *)match, m->name)) ||
       (m->alias && !strcmp((const char *)match, m->alias)))
      return 0;

   return 1;
}

Menu               *
MenuFind(const char *name, const char *param)
{
   Menu               *m;

   m = (Menu *) ecore_list_find(menu_list, _MenuMatchName, name);
   if (m)
      return (m);

   /* Not in list - try if we can load internal */
   m = MenusCreateInternal(NULL, name, NULL, param);

   return m;
}

/*
 * Aliases for "well-known" menus for backward compatibility.
 */
static const char  *const menu_aliases[] = {
   "APPS_SUBMENU", "file.menu",
   "CONFIG_SUBMENU", "settings.menu",
   "DESKTOP_SUBMENU", "desktop.menu",
   "MAINT_SUBMENU", "maintenance.menu",
   "ROOT_2", "enlightenment.menu",
   "WINOPS_MENU", "winops.menu",
};
#define N_MENU_ALIASES (sizeof(menu_aliases)/sizeof(char*)/2)

static const char  *
_MenuCheckAlias(const char *name)
{
   unsigned int        i;

   for (i = 0; i < N_MENU_ALIASES; i++)
      if (!strcmp(name, menu_aliases[2 * i]))
	 return menu_aliases[2 * i + 1];

   return NULL;
}

static void
MenusShowNamed(const char *name, const char *param)
{
   Menu               *m;
   const char         *name2;

   name2 = _MenuCheckAlias(name);
   if (name2)
      name = name2;

   /* Hide any menus currently up */
   if (MenusActive())
      MenusHide();

   m = MenuFind(name, param);
   if (!m)
      return;

   if (!m->ewin)		/* Don't show if already shown */
      MenuShow(m, 0);
}

int
MenusActive(void)
{
   return ! !Mode_menus.first;
}

static void
MenusHide(void)
{
   Menu               *m;

   TIMER_DEL(menu_timer_submenu);

   m = Mode_menus.first;
   if (m && m->transient)
      MenuDestroy(m);
   else
      MenuHide(m);
   Mode_menus.first = NULL;
   MenuHideMasker();
   TooltipsEnable(1);
}

static void
MenusTouch(void)
{
   Menu               *m;

   ECORE_LIST_FOR_EACH(menu_list, m) m->redraw = 1;
}

/*
 * Menu event handlers
 */

static MenuItem    *
MenuFindNextItem(Menu * m, MenuItem * mi, int inc)
{
   int                 i;

   if (!mi)
     {
	if (m->num > 0)
	   return m->items[0];
	else
	   return NULL;
     }

   for (i = 0; i < m->num; i++)
      if (m->items[i] == mi)
	{
	   i = (i + inc + m->num) % m->num;
	   return m->items[i];
	}

   return NULL;
}

static              KeySym
MenuKeyPressConversion(KeySym key)
{
   if (key == Conf.menus.key.left)
      return XK_Left;
   if (key == Conf.menus.key.right)
      return XK_Right;
   if (key == Conf.menus.key.up)
      return XK_Up;
   if (key == Conf.menus.key.down)
      return XK_Down;
   if (key == Conf.menus.key.escape)
      return XK_Escape;
   if (key == Conf.menus.key.ret)
      return XK_Return;

   /* The key does not correspond to any set, use the default behavior 
    * associated to the key */
   return key;
}

static void
MenuEventKeyPress(Menu * m, XEvent * ev)
{
   KeySym              key;
   MenuItem           *mi;
   EWin               *ewin;

   mi = NULL;
   if (Mode_menus.active)
     {
	m = Mode_menus.active;
	mi = m->sel_item;
     }

   /* NB! m != NULL */

   key = XLookupKeysym(&ev->xkey, 0);
   switch (MenuKeyPressConversion(key))
     {
     case XK_Escape:
	MenusHide();
	break;

     case XK_Down:
      check_next:
	mi = MenuFindNextItem(m, mi, 1);
	goto check_activate;

     case XK_Up:
	mi = MenuFindNextItem(m, mi, -1);
	goto check_activate;

     case XK_Left:
	m = m->parent;
	if (!m)
	   break;
	mi = m->sel_item;
	goto check_activate;

     case XK_Right:
	if (!mi)
	   goto check_next;
	m = mi->child;
	if (!m || m->num <= 0)
	   break;
	ewin = m->ewin;
	if (!ewin || !EoIsMapped(ewin))
	   break;
	mi = MenuFindItemByChild(m, m->child);
	goto check_activate;

      check_activate:
	if (!mi)
	   break;
	MenuActivateItem(m, mi);
	break;

     case XK_Return:
	if (!mi)
	   break;
	if (!mi->params)
	   break;
	EFuncDefer(Mode_menus.context_ewin, mi->params);
	MenusHide();
	EobjsRepaint();
	break;
     }
}

static void
MenuItemEventMouseDown(MenuItem * mi, XEvent * ev __UNUSED__)
{
   Menu               *m;

   Mode_menus.just_shown = 0;

   m = mi->menu;
   MenuDrawItem(m, mi, 1, STATE_CLICKED);
}

static void
MenuItemEventMouseUp(MenuItem * mi, XEvent * ev __UNUSED__)
{
   Menu               *m;

   if (Mode_menus.just_shown)
     {
	Mode_menus.just_shown = 0;
	if (ev->xbutton.time - Mode.events.last_btime < 250)
	   return;
     }

   m = mi->menu;

   if ((m) && (mi->state))
     {
	MenuDrawItem(m, mi, 1, STATE_HILITED);
	if ((mi->params) /* && (!Mode_menus.just_shown) */ )
	  {
	     EFuncDefer(Mode_menus.context_ewin, mi->params);
	     MenusHide();
	     EobjsRepaint();
	  }
     }
}

static void
MenusSetEvents(int on)
{
   int                 i;
   Menu               *m;
   unsigned int        event_mask;

   event_mask = (on) ? MENU_ITEM_EVENT_MASK : 0;

   for (m = Mode_menus.first; m; m = m->child)
     {
	for (i = 0; i < m->num; i++)
	   ESelectInput(m->items[i]->win, event_mask);
     }
}

static void
MenuSelectItem(Menu * m, MenuItem * mi, int focus)
{
   if (mi && focus)
     {
	if (Mode_menus.active != m)
	  {
	     Mode_menus.active = m;
	     GrabKeyboardRelease();
	     GrabKeyboardSet(m->win);
	  }
     }

   if (mi == m->sel_item)
      return;

   if (m->sel_item)
      MenuDrawItem(m, m->sel_item, 1, STATE_NORMAL);

   if (mi)
      MenuDrawItem(m, mi, 1, STATE_HILITED);

   m->sel_item = mi;
}

static void
MenuSelectItemByChild(Menu * m, Menu * mc)
{
   MenuItem           *mi;

   mi = MenuFindItemByChild(m, mc);
   if (!mi)
      return;

   MenuSelectItem(m, mi, 0);
}

static void
_SubmenuGetPlacement(Menu * m, int *xo, int *yo, int *mw, int *mh)
{
   EWin               *ewin, *ewin2;
   MenuItem           *mi = m->sel_item;
   int                 mix, miy, miw, my2;
   int                 bl1, br1, bt1, bb1;
   int                 bl2, br2, bt2, bb2;

   EGetGeometry(mi->win, NULL, &mix, &miy, &miw, NULL, NULL, NULL);
   my2 = 0;
   if (mi->child->num > 0 && mi->child->items[0])
      EGetGeometry(mi->child->items[0]->win, NULL, NULL, &my2, NULL, NULL, NULL,
		   NULL);

   ewin = m->ewin;
   ewin2 = m->child->ewin;
   EwinBorderGetSize(ewin, &bl1, &br1, &bt1, &bb1);
   EwinBorderGetSize(ewin2, &bl2, &br2, &bt2, &bb2);

   /* Sub-menu offsets relative to parent menu origin */
   *xo = bl1 + mix + miw;
   *yo = bt1 + miy - (bt2 + my2);

   /* Size of new submenu (may be shaded atm.) */
   *mw = mi->child->w + bl2 + br2;
   *mh = mi->child->h + bt2 + bb2;
}

static void
_MenusSlideCheck(Menu * m, int xo, int yo, int ww, int hh, int *pdx, int *pdy)
{
   EWin               *ewin;
   int                 sx, sy, sw, sh;
   int                 xdist, ydist;

   xdist = ydist = 0;

   if (!Conf.menus.onscreen)
      goto done;

   ScreenGetGeometryByHead(Mode_menus.first->ewin->head, &sx, &sy, &sw, &sh);

   ewin = m->ewin;

   if (EoGetX(Mode_menus.first->ewin) < sx)
      xdist = sx - EoGetX(Mode_menus.first->ewin);
   if (EoGetX(ewin) + xdist + xo + ww > sx + sw)
      xdist = sx + sw - (EoGetX(ewin) + xo + ww);
   if (EoGetX(ewin) + xdist + xo < sx)
      xdist = sx - (EoGetX(ewin) + xo);

   if (EoGetY(Mode_menus.first->ewin) < sy)
      ydist = sy - EoGetY(Mode_menus.first->ewin);
   if (EoGetY(ewin) + ydist + yo + hh > sy + sh)
      ydist = sy + sh - (EoGetY(ewin) + yo + hh);
   if (EoGetY(ewin) + ydist + yo < sy)
      ydist = sy - (EoGetY(ewin) + yo);

 done:
   *pdx = xdist;
   *pdy = ydist;
}

static void
_MenusSlide(Menu * m, int xdist, int ydist)
{
   EWin               *ewin, *menus[256];
   int                 fx[256], fy[256], tx[256], ty[256];
   int                 i;
   Menu               *mm;
   MenuItem           *mi;

   i = 0;
   for (mm = Mode_menus.first; mm; mm = mm->child)
     {
	ewin = mm->ewin;
	menus[i] = ewin;
	fx[i] = EoGetX(ewin);
	fy[i] = EoGetY(ewin);
	tx[i] = EoGetX(ewin) + xdist;
	ty[i] = EoGetY(ewin) + ydist;
	i++;
	if (mm == m)
	   break;
     }

   MenusSetEvents(0);		/* Disable menu item events while sliding */
   EwinsSlideTo(menus, fx, fy, tx, ty, i, Conf.shading.speed, 0, 0);
   MenusSetEvents(1);

   mi = m->sel_item;
   if (mi && Conf.menus.warp)
      EWarpPointer(mi->win, WinGetW(mi->win) / 2, WinGetH(mi->win) / 2);
}

static int
SubmenuShowTimeout(void *data)
{
   Menu               *m;
   MenuItem           *mi;
   EWin               *ewin, *ewin2;
   int                 xo, yo, mw, mh, xdist, ydist;

   menu_timer_submenu = NULL;

   m = (Menu *) data;
   if (!ecore_list_goto(menu_list, m))
      goto done;
   ewin = m->ewin;
   if (!EwinFindByPtr(ewin))
      goto done;
   if (!EoIsShown(ewin))
      goto done;

   mi = m->sel_item;
   if (!mi)
      goto done;

   if (mi->child != m->child)
      MenuHide(m->child);
   m->child = mi->child;

   xo = yo = mw = mh = 0;
   ewin2 = NULL;
   if (mi->child)
     {
	mi->child->parent = m;
	MenuShow(mi->child, 1);
	ewin2 = mi->child->ewin;
	ewin2 = EwinFindByPtr(ewin2);
	if (ewin2)
	   _SubmenuGetPlacement(m, &xo, &yo, &mw, &mh);
     }

   _MenusSlideCheck(m, xo, yo, mw, mh, &xdist, &ydist);
   if (ewin2)
      EwinMove(ewin2, EoGetX(ewin) + xdist + xo, EoGetY(ewin) + ydist + yo,
	       MRF_NOCHECK_ONSCREEN);
   if (xdist != 0 || ydist != 0)
      _MenusSlide(m, xdist, ydist);
   if (ewin2)
      _MenuEwinShow(ewin2);

 done:
   return 0;
}

static void
MenuActivateItem(Menu * m, MenuItem * mi)
{
   MenuItem           *mi_prev;

   mi_prev = m->sel_item;

   if (m->child)
      MenuSelectItem(m->child, NULL, 0);
   MenuSelectItem(m, mi, 1);
   if (m->parent)
      MenuSelectItemByChild(m->parent, m);

   if (mi == mi_prev)
      return;

   if (mi && !mi->child && mi_prev && !mi_prev->child)
      return;

   TIMER_DEL(menu_timer_submenu);

   if ((mi && mi->child && !mi->child->shown) || (mi && mi->child != m->child))
      TIMER_ADD(menu_timer_submenu, 200, SubmenuShowTimeout, m);
}

static void
MenuItemEventMouseIn(MenuItem * mi, XEvent * ev)
{
   if (ev->xcrossing.detail == NotifyInferior)
      return;

   MenuActivateItem(mi->menu, mi);
}

static void
MenuItemEventMouseOut(MenuItem * mi, XEvent * ev)
{
   if (ev->xcrossing.detail == NotifyInferior)
      return;

   if (!mi->child)
      MenuSelectItem(mi->menu, NULL, 0);
}

static void
MenuHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Menu               *m = (Menu *) prm;

#if DEBUG_MENU_EVENTS
   Eprintf("MenuHandleEvents %d\n", ev->type);
#endif
   switch (ev->type)
     {
     case KeyPress:
	MenuEventKeyPress(m, ev);
	break;
     case ButtonRelease:
	break;
     case EnterNotify:
	GrabKeyboardSet(m->win);
	break;
     }
}

static void
MenuItemHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   MenuItem           *mi = (MenuItem *) prm;

#if DEBUG_MENU_EVENTS
   Eprintf("MenuItemHandleEvents %d\n", ev->type);
#endif
   switch (ev->type)
     {
     case ButtonPress:
	MenuItemEventMouseDown(mi, ev);
	break;
     case ButtonRelease:
	MenuItemEventMouseUp(mi, ev);
	break;
     case EnterNotify:
	MenuItemEventMouseIn(mi, ev);
	break;
     case LeaveNotify:
	MenuItemEventMouseOut(mi, ev);
	break;
     }
}

static void
MenuMaskerHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm __UNUSED__)
{
#if DEBUG_MENU_EVENTS
   Eprintf("MenuMaskerHandleEvents %d\n", ev->type);
#endif

   switch (ev->type)
     {
     case ButtonRelease:
	if (Mode_menus.cover_bpress)
	   MenusHide();
	break;
     case ButtonPress:
	Mode_menus.cover_bpress = 1;
	break;
     }
}

/*
 * Configuration load/save
 */
#include "conf.h"

int
MenuStyleConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   MenuStyle          *ms = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     goto done;
	  case CONFIG_CLASSNAME:
	     ms = MenuStyleCreate(s2);
	     continue;
	  }

	/* ms needed */
	if (!ms)
	   break;

	switch (i1)
	  {
	  case CONFIG_TEXT:
	     ms->tclass = TextclassAlloc(s2, 1);
	     break;
	  case MENU_BG_ICLASS:
	     ms->bg_iclass = ImageclassAlloc(s2, 1);
	     break;
	  case MENU_ITEM_ICLASS:
	     ms->item_iclass = ImageclassAlloc(s2, 1);
	     break;
	  case MENU_SUBMENU_ICLASS:
	     ms->sub_iclass = ImageclassAlloc(s2, 1);
	     break;
	  case MENU_USE_ITEM_BACKGROUND:
	     ms->use_item_bg = atoi(s2);
	     if (ms->use_item_bg)
	       {
		  if (ms->bg_iclass)
		    {
		       ImageclassFree(ms->bg_iclass);
		       ms->bg_iclass = NULL;
		    }
	       }
	     break;
	  case MENU_MAX_COLUMNS:
	     ms->maxx = atoi(s2);
	     break;
	  case MENU_MAX_ROWS:
	     ms->maxy = atoi(s2);
	     break;
	  case CONFIG_BORDER:
	     _EFDUP(ms->border_name, s2);
	     break;
	  default:
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

int
MenuConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char                s3[FILEPATH_LEN_MAX];
   char                s4[FILEPATH_LEN_MAX];
   char                s5[FILEPATH_LEN_MAX];
   char               *p2, *p3;
   char               *txt = NULL;
   const char         *params;
   int                 i1, i2, len;
   Menu               *m = NULL, *mm;
   MenuItem           *mi;
   ImageClass         *ic = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, &p2, &p3);
	switch (i1)
	  {
	  default:
	     break;

	  case CONFIG_VERSION:
	     continue;

	  case CONFIG_MENU:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     m = NULL;
	     ic = NULL;
	     _EFREE(txt);
	     continue;
	  case CONFIG_CLOSE:
	     err = 0;
	     continue;

	  case MENU_PREBUILT:
	     m = MenuFind(s2, NULL);
	     if (m)
		continue;
	     sscanf(p3, "%4000s %4000s %4000s", s3, s4, s5);
	     m = MenusCreateInternal(s4, s2, s3, s5);
	     if (m)
		m->ref_count++;
	     continue;

	  case CONFIG_CLASSNAME:
	     if (!m)
		m = MenuCreate(s2, NULL, NULL, NULL);
	     else
		MenuSetName(m, s2);
	     params = _MenuCheckAlias(s2);
	     if (m && params)
		MenuSetAlias(m, params);
	     continue;

	  case MENU_ITEM:
	     ic = NULL;
	     if (strcmp("NULL", s2))
		ic = ImageclassFind(s2, 0);
	     _EFREE(txt);
	     if (p3 && *p3)
		txt = Estrdup(p3);
	     continue;
	  }

	/* The rest require the menu m to be created */
	if (!m)
	  {
	     ConfigParseError("Menu", s);
	     continue;
	  }

	switch (i1)
	  {
	  case MENU_USE_STYLE:
	     MenuSetStyle(m, MenuStyleFind(s2));
	     break;
	  case MENU_TITLE:
	     MenuSetTitle(m, p2);
	     break;
	  case MENU_ACTION:
	     if ((txt) || (ic))
	       {
		  char                ok = 1;

		  /* if its an execute line then check to see if the exec is 
		   * on your system before adding the menu entry */
		  if (!strcmp(s2, "exec"))
		    {
		       sscanf(p3, "%1000s", s3);
		       ok = path_canexec(s3);
		    }
		  if (ok)
		    {
		       mi = MenuItemCreate(txt, ic, p2, NULL);
		       MenuAddItem(m, mi);
		    }
		  ic = NULL;
		  _EFREE(txt);
	       }
	     break;
	  case MENU_SUBMENU:
	     len = 0;
	     sscanf(p3, "%s %n", s3, &len);
	     ic = NULL;
	     if (strcmp("NULL", s3))
		ic = ImageclassFind(s3, 1);
	     mm = MenuFind(s2, NULL);
	     mi = MenuItemCreate(p3 + len, ic, NULL, mm);
	     MenuAddItem(m, mi);
	     break;
	  default:
	     break;
	  }
     }

 done:
   if (err)
      ConfigAlertLoad("Menu");
   _EFREE(txt);

   return err;
}

static int
MenusTimeout(void *data __UNUSED__)
{
   Menu               *m;
   time_t              ts;

   /* Unload contents if loadable and no access in > 5 min */
   ts = time(0);
   ECORE_LIST_FOR_EACH(menu_list, m)
   {
      if (m->shown || !m->filled ||
	  ts - m->last_access < MENU_UNLOAD_CHECK_INTERVAL)
	 continue;

      if (!m->loader)
	 MenuFreePixmaps(m);
      else if (m->ref_count)
	 MenuEmpty(m, 0);
      else
	 MenuDestroy(m);
   }

   return 1;
}

/*
 * Menus Module
 */

static void
MenusSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_START:
	TIMER_ADD_NP(1000 * MENU_UNLOAD_CHECK_INTERVAL, MenusTimeout, NULL);
	break;

     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	MenusHide();
	break;

     case ESIGNAL_EWIN_UNMAP:
	if ((EWin *) prm == Mode_menus.context_ewin)
	   MenusHide();
	break;

     case ESIGNAL_THEME_TRANS_CHANGE:
	MenusTouch();
	break;
     }
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */

typedef struct {
   char                warp;
   char                animate;
   char                onscreen;
   int                 icon_size;
} MenudDlgData;

static void
CB_ConfigureMenus(Dialog * d, int val, void *data __UNUSED__)
{
   MenudDlgData       *dd = DLG_DATA_GET(d, MenudDlgData);

   if (val >= 2)
      return;

   Conf.menus.warp = dd->warp;
   Conf.menus.animate = dd->animate;
   Conf.menus.onscreen = dd->onscreen;
   Conf.menus.icon_size = dd->icon_size;

   MenusTouch();

   autosave();
}

static void
_DlgCbIconSize(Dialog * d, int val __UNUSED__, void *data)
{
   MenudDlgData       *dd = DLG_DATA_GET(d, MenudDlgData);
   DItem              *di = (DItem *) data;
   char                s[256];

   Esnprintf(s, sizeof(s), _("Icon size: %2d"), dd->icon_size);
   DialogItemSetText(di, s);
}

static void
_DlgFillMenus(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *label;
   MenudDlgData       *dd;

   dd = DLG_DATA_SET(d, MenudDlgData);
   if (!dd)
      return;

   dd->warp = Conf.menus.warp;
   dd->animate = Conf.menus.animate;
   dd->onscreen = Conf.menus.onscreen;
   dd->icon_size = Conf.menus.icon_size;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Animated display of menus"));
   DialogItemCheckButtonSetPtr(di, &dd->animate);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Always pop up menus on screen"));
   DialogItemCheckButtonSetPtr(di, &dd->onscreen);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Warp pointer after moving menus"));
   DialogItemCheckButtonSetPtr(di, &dd->warp);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 12, 48);
   DialogItemSliderSetUnits(di, 2);
   DialogItemSliderSetJump(di, 4);
   DialogItemSliderSetValPtr(di, &dd->icon_size);
   DialogItemSetCallback(di, _DlgCbIconSize, 0, label);
   DialogItemCallCallback(d, di);
}

const DialogDef     DlgMenus = {
   "CONFIGURE_MENUS",
   N_("Menus"),
   N_("Menu Settings"),
   SOUND_SETTINGS_MENUS,
   "pix/place.png",
   N_("Enlightenment Menu\n" "Settings Dialog"),
   _DlgFillMenus,
   DLG_OAC, CB_ConfigureMenus,
};
#endif /* ENABLE_DIALOGS */

static void
MenusIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;
   Menu               *m;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Menus - active=%d\n", MenusActive());
     }
   else if (!strncmp(cmd, "list", 2))
     {
#define SS(s) ((s) ? (s) : "-")
#define ST(s) ((s) ? (s->name) : "-")
	ECORE_LIST_FOR_EACH(menu_list, m)
	   IpcPrintf("%s(%s/%s): %s\n", m->name, SS(m->alias), ST(m->style),
		     SS(m->title));
     }
   else if (!strncmp(cmd, "reload", 2))
     {
	MenusDestroyLoaded();
     }
   else if (!strncmp(cmd, "show", 2))
     {
	if (*p == '\0')
	   p = NULL;
	if (p && !strcmp(prm, "named"))
	  {
	     Esnprintf(prm, sizeof(prm), "%s", p);
	     p = NULL;
	  }
	SoundPlay(SOUND_MENU_SHOW);
	MenusShowNamed(prm, p);
     }
}

static const IpcItem MenusIpcArray[] = {
   {
    MenusIpc,
    "menus", "mnu",
    "Menu functions",
    "  menus list               Show existing menus\n"
    "  menus reload             Force menu refresh\n"
    "  menus show <name>        Show named menu\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(MenusIpcArray)/sizeof(IpcItem))

static const CfgItem MenusCfgItems[] = {
   CFG_ITEM_BOOL(Conf.menus, animate, 0),
   CFG_ITEM_BOOL(Conf.menus, onscreen, 1),
   CFG_ITEM_BOOL(Conf.menus, warp, 1),
   CFG_ITEM_BOOL(Conf.menus, show_icons, 1),
   CFG_ITEM_INT(Conf.menus, icon_size, 16),
   CFG_ITEM_HEX(Conf.menus, key.left, XK_Left),
   CFG_ITEM_HEX(Conf.menus, key.right, XK_Right),
   CFG_ITEM_HEX(Conf.menus, key.up, XK_Up),
   CFG_ITEM_HEX(Conf.menus, key.down, XK_Down),
   CFG_ITEM_HEX(Conf.menus, key.escape, XK_Escape),
   CFG_ITEM_HEX(Conf.menus, key.ret, XK_Return),
};
#define N_CFG_ITEMS (sizeof(MenusCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModMenus;

const EModule       ModMenus = {
   "menus", "menu",
   MenusSighan,
   {N_IPC_FUNCS, MenusIpcArray},
   {N_CFG_ITEMS, MenusCfgItems}
};
