/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2011 Kim Woelders
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
 * Author: Merlin Hughes
 *  - merlin@merlin.org
 *
 *  This code is free software.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */
#include "E.h"
#include "desktops.h"
#include "emodule.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "iclass.h"
#include "icons.h"
#include "screen.h"
#include "tclass.h"
#include "tooltips.h"
#include "xwin.h"
#include <X11/keysym.h>

static void
WarpShapeDraw(EWin * ewin)
{
   static ShapeWin    *shape_win = NULL;
   int                 bl, br, bt, bb;

   if (!ewin)
     {
	ShapewinDestroy(shape_win);
	shape_win = NULL;
	return;
     }

   if (!shape_win)
      shape_win = ShapewinCreate(MR_BOX);
   if (!shape_win)
      return;

   EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
   ShapewinShapeSet(shape_win, MR_BOX, EoGetX(ewin), EoGetY(ewin),
		    ewin->client.w, ewin->client.h, bl, br, bt, bb, 0);
   EoMap(shape_win, 0);
}

typedef struct {
   EWin               *ewin;
   Win                 win;
   char               *txt;
} WarplistItem;

typedef struct {
   EObj                o;
   TextClass          *tc;
   ImageClass         *ic;
   int                 mw, mh, tw, th;
} WarpFocusWin;

static void         WarpFocusHandleEvent(Win win, XEvent * ev, void *prm);

static WarpFocusWin *warpFocusWindow = NULL;

static int          warpFocusIndex = 0;
static unsigned int warpFocusKey = 0;
static unsigned int warpFocusState = 0;
static int          warplist_num = 0;
static WarplistItem *warplist;

#define ICON_PAD 2

static WarpFocusWin *
WarpFocusWinCreate(void)
{
   WarpFocusWin       *fw;

   fw = ECALLOC(WarpFocusWin, 1);
   if (!fw)
      return fw;

   EoInit(fw, EOBJ_TYPE_MISC, None, 0, 0, 1, 1, 1, "Warp");
   EoSetFloating(fw, 1);
   EoSetLayer(fw, 20);
   EoSetFade(fw, 1);
   EoSetShadow(fw, 1);

   EventCallbackRegister(EoGetWin(fw), WarpFocusHandleEvent, NULL);
   ESelectInput(EoGetWin(fw), ButtonReleaseMask);

   fw->tc = TextclassFind("WARPFOCUS", 0);
   if (!fw->tc)
      fw->tc = TextclassFind("COORDS", 1);

   fw->ic = ImageclassFind("WARPFOCUS", 0);
   if (!fw->ic)
      fw->ic = ImageclassFind("COORDS", 1);

   return fw;
}

#if 0
static void
WarpFocusWinDestroy(WarpFocusWin * fw)
{
   EventCallbackUnregister(EoGetWin(fw), WarpFocusHandleEvent, NULL);
   EoFini(fw);
   Efree(fw);
}
#endif

static void
WarpFocusWinShow(WarpFocusWin * fw)
{
   WarplistItem       *wi;
   EImageBorder       *pad;
   EWin               *ewin;
   int                 i, x, y, w, h, ww, hh;
   char                s[1024], ss[32];
   const char         *fmt;

   w = 0;
   h = 0;
   pad = ImageclassGetPadding(fw->ic);

   for (i = 0; i < warplist_num; i++)
     {
	wi = warplist + i;
	wi->win = ECreateWindow(EoGetWin(fw), 0, 0, 1, 1, 0);
	EMapWindow(wi->win);

	ewin = wi->ewin;
	if (ewin->state.iconified)
	   fmt = "%s[%s]";
	else if (ewin->state.shaded)
	   fmt = "%s=%s=";
	else
	   fmt = "%s%s";
	ss[0] = '\0';
	if (Conf.warplist.showalldesks)
	  {
	     if (EoIsSticky(ewin) || ewin->state.iconified)
		strcpy(ss, "[-] ");
	     else
		Esnprintf(ss, sizeof(ss), "[%d] ", EoGetDeskNum(ewin));
	  }
	Esnprintf(s, sizeof(s), fmt, ss, EwinGetTitle(ewin));
	wi->txt = strdup(s);
	TextSize(fw->tc, 0, 0, 0, wi->txt, &ww, &hh, 17);
	if (ww > w)
	   w = ww;
	if (hh > h)
	   h = hh;
     }

   fw->tw = w;			/* Text size */
   fw->th = h;
   w += pad->left + pad->right;
   h += pad->top + pad->bottom;
   if (Conf.warplist.icon_mode != 0)
     {
	if (Conf.warplist.icon_mode != EWIN_ICON_MODE_APP_IMG &&
	    Conf.warplist.icon_mode != EWIN_ICON_MODE_IMG_APP)
	   Conf.warplist.icon_mode = EWIN_ICON_MODE_APP_IMG;
	w += h;
     }
   fw->mw = w;			/* Focus list item size */
   fw->mh = h;

   /* Reset shape */
   EShapeSetMask(EoGetWin(fw), 0, 0, None);

   ScreenGetAvailableAreaByPointer(&x, &y, &ww, &hh, Conf.place.ignore_struts);
   x += (ww - w) / 2;
   y += (hh - h * warplist_num) / 2;
   EoMoveResize(fw, x, y, w, h * warplist_num);

   for (i = 0; i < warplist_num; i++)
      EMoveResizeWindow(warplist[i].win, 0, (h * i), fw->mw, fw->mh);

   EoMap(fw, 0);

   /*
    * Grab the keyboard. The grab is automatically released when
    * WarpFocusHide unmaps warpFocusWindow.
    */
   GrabKeyboardSet(EoGetWin(fw));
   GrabPointerSet(EoGetWin(fw), None, 0);

   TooltipsEnable(0);
}

static void
WarpFocusWinHide(WarpFocusWin * fw)
{
   int                 i;

   EoUnmap(fw);
   for (i = 0; i < warplist_num; i++)
     {
	EDestroyWindow(warplist[i].win);
	Efree(warplist[i].txt);
     }
#if 0				/* We might as well keep it around */
   WarpFocusWinDestroy(fw);
   warpFocusWindow = NULL;
#endif

   TooltipsEnable(1);
}

static void
WarpFocusWinPaint(WarpFocusWin * fw)
{
   int                 i, state, iw;
   WarplistItem       *wi;
   EImageBorder       *pad;

   pad = ImageclassGetPadding(fw->ic);

   for (i = 0; i < warplist_num; i++)
     {
	wi = warplist + i;

	if (!EwinFindByPtr(wi->ewin))
	   wi->ewin = NULL;
	if (!wi->ewin)
	   continue;

	state = (i == warpFocusIndex) ? STATE_CLICKED : STATE_NORMAL;

	ImageclassApply(fw->ic, wi->win, 0, 0, state, ST_WARPLIST);

	iw = 0;
	if (Conf.warplist.icon_mode != 0)
	  {
	     int                 icon_size = fw->mh - 2 * ICON_PAD;
	     EImage             *im;

	     im = EwinIconImageGet(wi->ewin, icon_size,
				   Conf.warplist.icon_mode);
	     if (im)
	       {
		  EImageRenderOnDrawable(im, wi->win, None,
					 EIMAGE_BLEND | EIMAGE_ANTI_ALIAS,
					 pad->left + ICON_PAD, ICON_PAD,
					 icon_size, icon_size);
		  EImageFree(im);
	       }
	     iw = fw->mh;
	  }

	TextDraw(fw->tc, wi->win, None, 0, 0, state, wi->txt,
		 pad->left + iw, pad->top, fw->tw, fw->th, 0, 0);
     }

   /* FIXME - Check shape */
   EoShapeUpdate(fw, 1);
}

static void
WarpFocusShow(void)
{
   WarpFocusWin       *fw = warpFocusWindow;

   if (!warplist)
      return;

   if (!fw)
     {
	warpFocusWindow = fw = WarpFocusWinCreate();
	if (!fw)
	   return;
     }

   if (!EoIsShown(fw))
      WarpFocusWinShow(fw);

   WarpFocusWinPaint(fw);
}

static void
WarpFocusHide(void)
{
   WarpFocusWin       *fw = warpFocusWindow;

   if (fw && EoIsShown(fw))
      WarpFocusWinHide(fw);

   Efree(warplist);
   warplist = NULL;
   warplist_num = 0;
}

void
WarpFocus(int delta)
{
   WarpFocusWin       *fw = warpFocusWindow;
   EWin               *const *lst;
   EWin               *ewin;
   int                 i, num;
   WarplistItem       *wl;

   /* Remember invoking keycode (ugly hack) */
   if (!fw || !EoIsShown(fw))
     {
	warpFocusKey = Mode.events.last_keycode;
	warpFocusState = Mode.events.last_keystate;
     }

   if (!warplist)
     {
	warplist_num = 0;	/* Not necessary but silences clang */
	lst = EwinListFocusGet(&num);
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (		/* Either visible or iconified */
		   ((EwinIsOnScreen(ewin)) || (ewin->state.iconified) ||
		    (Conf.warplist.showalldesks)) &&
		   /* Exclude windows that explicitely say so */
		   (!ewin->props.skip_focuslist) &&
		   (!ewin->props.skip_ext_task) &&
		   /* Keep shaded windows if conf say so */
		   ((!ewin->state.shaded) || (Conf.warplist.showshaded)) &&
		   /* Keep sticky windows if conf say so */
		   ((!EoIsSticky(ewin)) || (Conf.warplist.showsticky)) &&
		   /* Keep iconified windows if conf say so */
		   ((!ewin->state.iconified) || (Conf.warplist.showiconified)))
	       {
		  warplist_num++;
		  warplist = EREALLOC(WarplistItem, warplist, warplist_num);
		  wl = warplist + warplist_num - 1;
		  wl->ewin = ewin;
	       }
	  }

	/* Hmmm. Hack... */
	if (warplist_num >= 2 && warplist[1].ewin == GetFocusEwin())
	  {
	     warplist[1].ewin = warplist[0].ewin;
	     warplist[0].ewin = GetFocusEwin();
	  }

	warpFocusIndex = 0;
     }

   if (!warplist)
      return;

   warpFocusIndex = (warpFocusIndex + warplist_num + delta) % warplist_num;
   ewin = warplist[warpFocusIndex].ewin;
   if (!EwinFindByPtr(ewin))
      ewin = NULL;
   if (!ewin)
      return;

   WarpFocusShow();

   if (!EwinIsOnScreen(ewin))
      return;

   if (Conf.warplist.show_shape)
      WarpShapeDraw(ewin);

   if (Conf.focus.raise_on_next)
      EwinRaise(ewin);
   if (Conf.focus.warp_on_next)
      EwinWarpTo(ewin, 0);
   if (Conf.warplist.warpfocused)
      FocusToEWin(ewin, FOCUS_SET);
}

static void
WarpFocusClick(int ix)
{
   EWin               *ewin;

   if (!warplist)
      return;
   if (ix < 0 || ix >= warplist_num)
      return;
   if (ix == warpFocusIndex)
      return;

   warpFocusIndex = ix;
   WarpFocusShow();

   ewin = warplist[ix].ewin;
   if (!EwinFindByPtr(ewin))
      return;

   if (Conf.focus.raise_on_next)
      EwinRaise(ewin);

   FocusToEWin(ewin, FOCUS_SET);
}

static void
WarpFocusFinish(void)
{
   EWin               *ewin;

   ewin = warplist[warpFocusIndex].ewin;

   WarpShapeDraw(NULL);
   WarpFocusHide();

   if (!EwinFindByPtr(ewin))
      return;

   EwinOpActivate(ewin, OPSRC_USER, Conf.warplist.raise_on_select);
   if (Conf.warplist.warp_on_select)
      EwinWarpTo(ewin, 0);
}

static void
WarpFocusHandleEvent(Win win __UNUSED__, XEvent * ev, void *prm __UNUSED__)
{
   WarpFocusWin       *fw = warpFocusWindow;
   KeySym              key;
   unsigned int        mask;

   if (!EoIsShown(fw))
      return;

   switch (ev->type)
     {
     case KeyPress:
	if (ev->xkey.keycode == warpFocusKey)
	  {
	     if (((ev->xkey.state ^ warpFocusState) &
		  Mode.masks.mod_key_mask) == 0)
		key = 0x80000000;
	     else
		key = 0x80000001;
	  }
	else
	   key = XLookupKeysym(&ev->xkey, 0);
	switch (key)
	  {
	  default:
	     break;
	  case 0x80000000:
	  case XK_Down:
	     WarpFocus(1);
	     break;
	  case XK_Up:
	  case 0x80000001:
	     WarpFocus(-1);
	     break;
	  }
	break;

     case KeyRelease:
	mask = 0;
	EQueryPointer(NULL, NULL, NULL, NULL, &mask);
	if ((mask & Mode.masks.mod_key_mask) == 0)
	  {
	     WarpFocusFinish();
	     break;
	  }
	if (ev->xkey.keycode == warpFocusKey)
	   key = 0x80000000;
	else
	   key = XLookupKeysym(&ev->xkey, 0);
	switch (key)
	  {
	  case XK_Escape:
	     WarpFocusHide();
	     break;
	  default:
	  case 0x80000000:
	  case XK_Down:
	  case XK_Up:
	     break;
	  }
	break;

     case ButtonRelease:
	WarpFocusClick((ev->xbutton.y * warplist_num) / EoGetH(fw));
	break;
     }
}

/*
 * Warplist module
 */

static const CfgItem WarplistCfgItems[] = {
   CFG_ITEM_BOOL(Conf.warplist, enable, 1),
   CFG_ITEM_BOOL(Conf.warplist, showsticky, 1),
   CFG_ITEM_BOOL(Conf.warplist, showshaded, 1),
   CFG_ITEM_BOOL(Conf.warplist, showiconified, 1),
   CFG_ITEM_BOOL(Conf.warplist, showalldesks, 0),
   CFG_ITEM_BOOL(Conf.warplist, warpfocused, 1),
   CFG_ITEM_BOOL(Conf.warplist, raise_on_select, 1),
   CFG_ITEM_BOOL(Conf.warplist, warp_on_select, 0),
   CFG_ITEM_BOOL(Conf.warplist, show_shape, 0),
   CFG_ITEM_INT(Conf.warplist, icon_mode, EWIN_ICON_MODE_APP_IMG),
};
#define N_CFG_ITEMS (sizeof(WarplistCfgItems)/sizeof(CfgItem))

extern const EModule ModWarplist;

const EModule       ModWarplist = {
   "warplist", "warp",
   NULL,
   {0, NULL},
   {N_CFG_ITEMS, WarplistCfgItems}
};
