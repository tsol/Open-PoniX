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
#include "aclass.h"
#include "borders.h"
#include "cursors.h"
#include "e16-ecore_list.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"
#include "iclass.h"
#include "icons.h"
#include "snaps.h"
#include "tclass.h"
#include "timers.h"
#include "tooltips.h"
#include "windowmatch.h"
#include "xwin.h"

#define EWIN_BORDER_PART_EVENT_MASK \
  (KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | \
   EnterWindowMask | LeaveWindowMask | PointerMotionMask)
#define EWIN_BORDER_TITLE_EVENT_MASK \
  (EWIN_BORDER_PART_EVENT_MASK)

static Ecore_List  *border_list = NULL;

static void         BorderDestroy(Border * b);
static void         BorderWinpartHandleEvents(Win win, XEvent * ev, void *prm);
static void         BorderFrameHandleEvents(Win win, XEvent * ev, void *prm);
static Border      *BorderGetFallback(void);

static void
BorderWinpartRealise(EWin * ewin, int i)
{
   EWinBit            *ewb = &ewin->bits[i];

   if ((ewb->cx != ewb->x) || (ewb->cy != ewb->y) ||
       (ewb->cw != ewb->w) || (ewb->ch != ewb->h))
     {
	if ((ewb->w <= 0) || (ewb->h <= 0))
	  {
	     EUnmapWindow(ewb->win);
	  }
	else
	  {
	     EMapWindow(ewb->win);
	     EMoveResizeWindow(ewb->win, ewb->x, ewb->y, ewb->w, ewb->h);
	  }
     }
}

static void
BorderWinpartITclassApply(EWin * ewin, int i, int force)
{
   EWinBit            *ewb = &ewin->bits[i];
   WinPart            *ebp = &ewin->border->part[i];
   ImageState         *is;
   TextState          *ts;
   const char         *txt;
   int                 flags;

   if (ewb->win == None)
      return;

#if 0				/* Debug */
   Eprintf("BorderWpITApply: %#lx %#lx %2d %d %s\n",
	   EwinGetClientXwin(ewin), EoGetWin(ewin), i, force,
	   EwinGetTitle(ewin));
#endif

   is = ImageclassGetImageState(ebp->iclass, ewb->state,
				ewin->state.active, EoIsSticky(ewin));

   ts = NULL;
   txt = NULL;
   flags = 0;
   if (ebp->flags & FLAG_TITLE)
     {
	txt = EwinGetTitle(ewin);
	if (txt && ebp->tclass)
	  {
	     ts = TextclassGetTextState(ebp->tclass, ewb->state,
					ewin->state.active, EoIsSticky(ewin));
	     flags = ITA_BGPMAP | ITA_JUSTV;
	  }
     }
   if (ebp->flags & FLAG_MINIICON)
     {
	flags |= ITA_BGPMAP;
     }

   if (!force && ewb->is == is && ewb->ts == ts)
      return;
   ewb->is = is;
   ewb->ts = ts;

   ITApply(ewb->win, ebp->iclass, is, ewb->state,
	   ewin->state.active, EoIsSticky(ewin), ST_BORDER,
	   ebp->tclass, ts, txt, flags);

   if (ebp->flags & FLAG_MINIICON)
     {
	EImage             *im;

	im = EwinIconImageGet(ewin, 16, Conf.warplist.icon_mode);
	if (im)
	  {
	     Pixmap              pmap;
	     EImageBorder       *pad;
	     int                 x, y, w, h;

	     x = y = 0;
	     w = WinGetW(ewb->win);
	     h = WinGetH(ewb->win);
	     pad = ImageclassGetPadding(ebp->iclass);
	     if (ts)
	       {
		  /* Occupy square on one side */
		  switch (ts->style.orientation)
		    {
		    default:
		    case FONT_TO_RIGHT:
		    case FONT_TO_LEFT:
		       w = h;
		       break;
		    case FONT_TO_UP:
		       y = h - w;
		       /* FALLTHROUGH */
		    case FONT_TO_DOWN:
		       h = w;
		       break;
		    }
	       }
	     if (pad)
	       {
		  /* Probably not quite right for vertical borders */
		  x += pad->left;
		  y += pad->top;
		  w -= pad->left + pad->right;
		  h -= pad->top + pad->bottom;
	       }
	     if (w < 8 || h < 8)
		goto skip_icon;
	     pmap = EGetWindowBackgroundPixmap(ewb->win);
	     EImageRenderOnDrawable(im, ewb->win, pmap,
				    EIMAGE_BLEND | EIMAGE_ANTI_ALIAS,
				    x, y, w, h);
	     EClearWindow(ewb->win);
	   skip_icon:
	     EImageFree(im);
	  }
     }
}

static int
BorderWinpartDraw(EWin * ewin, int i)
{
   EWinBit            *ewb = &ewin->bits[i];
   int                 resize = 0, ret = 0;

   if ((ewb->x != ewb->cx) || (ewb->y != ewb->cy))
     {
	ewb->cx = ewb->x;
	ewb->cy = ewb->y;
	ret = 1;
     }

   if ((ewb->w != ewb->cw) || (ewb->h != ewb->ch))
     {
	resize = 1;
	ewb->cw = ewb->w;
	ewb->ch = ewb->h;
     }

   if ((resize) || (ewb->expose))
     {
	BorderWinpartITclassApply(ewin, i, 1);
	ewb->expose = 0;
	ret = 1;
     }

   return ret;
}

static void
BorderWinpartChange(EWin * ewin, int i, int force)
{
   BorderWinpartITclassApply(ewin, i, force);

   if (ewin->update.shape || ewin->border->changes_shape)
      EwinPropagateShapes(ewin);
}

void
EwinBorderDraw(EWin * ewin, int do_shape, int do_paint)
{
   int                 i;

#if 0				/* Debug */
   Eprintf("EwinBorderDraw %#lx %s d=%d s=%d p=%d\n",
	   EwinGetClientXwin(ewin), EoGetName(ewin), EoGetDeskNum(ewin),
	   do_shape, do_paint);
#endif

   for (i = 0; i < ewin->border->num_winparts; i++)
      BorderWinpartITclassApply(ewin, i, do_shape || do_paint);

   if (do_shape || ewin->update.shape || ewin->border->changes_shape)
      EwinPropagateShapes(ewin);
}

void
EwinBorderUpdateInfo(EWin * ewin)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (ewin->border->part[i].flags & FLAG_TITLE)
	   BorderWinpartITclassApply(ewin, i, 1);
     }
}

static void
BorderWinpartCalc(const EWin * ewin, int i, int ww, int hh)
{
   int                 x, y, w, h, ox, oy, max, min;
   int                 topleft, bottomright;

   topleft = ewin->border->part[i].geom.topleft.originbox;
   bottomright = ewin->border->part[i].geom.bottomright.originbox;
   if (topleft >= 0)
      BorderWinpartCalc(ewin, topleft, ww, hh);
   if (bottomright >= 0)
      BorderWinpartCalc(ewin, bottomright, ww, hh);

   x = y = 0;
   if (topleft == -1)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent * ww) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute;
	y = ((ewin->border->part[i].geom.topleft.y.percent * hh) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute;
     }
   else if (topleft >= 0)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent *
	      ewin->bits[topleft].w) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute +
	   ewin->bits[topleft].x;
	y = ((ewin->border->part[i].geom.topleft.y.percent *
	      ewin->bits[topleft].h) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute +
	   ewin->bits[topleft].y;
     }

   ox = oy = 0;
   if (bottomright == -1)
     {
	ox = ((ewin->border->part[i].geom.bottomright.x.percent * ww) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute;
	oy = ((ewin->border->part[i].geom.bottomright.y.percent * hh) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute;
     }
   else if (bottomright >= 0)
     {
	ox = ((ewin->border->part[i].geom.bottomright.x.percent *
	       ewin->bits[bottomright].w) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute +
	   ewin->bits[bottomright].x;
	oy = ((ewin->border->part[i].geom.bottomright.y.percent *
	       ewin->bits[bottomright].h) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute +
	   ewin->bits[bottomright].y;
     }

   /*
    * calculate height before width, because we may need it in order to
    * determine the font size. But we might do it the other way around for
    * side borders :-)
    */

   h = (oy - y) + 1;
   max = ewin->border->part[i].geom.height.max;
   min = ewin->border->part[i].geom.height.min;

   /*
    * If the title bar max size is set to zero, then set the title bar size to
    * just a little bit more than the size of the title text.
    */

   if (max == 0 && ewin->border->part[i].flags & FLAG_TITLE)
     {
	int                 dummywidth, wmax, wmin;
	ImageClass         *iclass;
	TextClass          *tclass;
	EImageBorder       *pad;

	/*
	 * calculate width before height, because we need it in order to
	 * determine the font size.
	 */

	w = (ox - x) + 1;
	wmax = ewin->border->part[i].geom.width.max;
	wmin = ewin->border->part[i].geom.width.min;
	if (w > wmax)
	  {
	     w = wmax;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < wmin)
	  {
	     w = wmin;
	  }
	iclass = ewin->border->part[i].iclass;
	tclass = ewin->border->part[i].tclass;
	pad = ImageclassGetPadding(iclass);
	TextSize(tclass, ewin->state.active, EoIsSticky(ewin),
		 ewin->bits[i].state, EwinGetTitle(ewin), &max, &dummywidth,
		 w - (pad->top + pad->bottom));
	max += pad->left + pad->right;
	if (h > max)
	  {
	     y = y + (((h - max) * TextclassGetJustification(tclass)) >> 10);
	     h = max;
	  }
	if (h < min)
	  {
	     h = min;
	  }
     }
   else
     {
	if (h > max)
	  {
	     h = max;
	     y = ((y + oy) - h) >> 1;
	  }
	else if (h < min)
	  {
	     h = min;
	  }
	/*
	 * and now the width.
	 */

	w = (ox - x) + 1;
	max = ewin->border->part[i].geom.width.max;
	min = ewin->border->part[i].geom.width.min;

	/*
	 * If the title bar max size is set to zero, then set the title bar
	 * size to just a little bit more than the size of the title text.
	 */

	if (max == 0 && ewin->border->part[i].flags & FLAG_TITLE)
	  {
	     int                 dummyheight;
	     ImageClass         *iclass;
	     TextClass          *tclass;
	     EImageBorder       *pad;

	     iclass = ewin->border->part[i].iclass;
	     tclass = ewin->border->part[i].tclass;
	     pad = ImageclassGetPadding(iclass);
	     TextSize(tclass, ewin->state.active, EoIsSticky(ewin),
		      ewin->bits[i].state, EwinGetTitle(ewin), &max,
		      &dummyheight, h - (pad->top + pad->bottom));
	     max += pad->left + pad->right;

	     if (w > max)
	       {
		  x = x +
		     (((w - max) * TextclassGetJustification(tclass)) >> 10);
		  w = max;
	       }
	  }
	if (w > max)
	  {
	     w = max;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < min)
	  {
	     w = min;
	  }
     }
   if ((ewin->state.shaded) && (!ewin->border->part[i].keep_for_shade))
     {
	ewin->bits[i].x = -100;
	ewin->bits[i].y = -100;
	ewin->bits[i].w = -1;
	ewin->bits[i].h = -1;
     }
   else
     {
	ewin->bits[i].x = x;
	ewin->bits[i].y = y;
	ewin->bits[i].w = w;
	ewin->bits[i].h = h;
     }
}

void
EwinBorderCalcSizes(EWin * ewin, int propagate)
{
   int                 i, ww, hh;
   char                reshape;

   if (!ewin->border)
      return;

   ww = EoGetW(ewin);
   hh = EoGetH(ewin);

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i, ww, hh);
   for (i = 0; i < ewin->border->num_winparts; i++)
      BorderWinpartRealise(ewin, i);

   reshape = 0;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	reshape |= BorderWinpartDraw(ewin, i);
     }

#if 0				/* Debug */
   Eprintf("EwinBorderCalcSizes prop=%d reshape=%d\n", propagate, reshape);
#endif
   if (reshape)
      ewin->update.shape = 1;
   if (propagate && ewin->update.shape)
      EwinPropagateShapes(ewin);
}

static void
BorderIncRefcount(const Border * b)
{
   ((Border *) b)->ref_count++;
}

static void
BorderDecRefcount(const Border * b)
{
   ((Border *) b)->ref_count--;
}

const char         *
BorderGetName(const Border * b)
{
   return (b) ? b->name : NULL;
}

void
EwinBorderSelect(EWin * ewin)
{
   const Border       *b;

   if (ewin->inh_wm.b.border)
     {
	b = BorderFind("BORDERLESS");
	goto done;
     }

   /* Quit if we already have a border that isn't an internal one */
   b = ewin->border;
   if (b && strncmp(b->name, "__", 2))
      goto done;

   b = NULL;

   if (ewin->props.no_border)
      b = BorderFind("BORDERLESS");

   if (!b)
      b = WindowMatchEwinBorder(ewin);

   if (!b)
      b = BorderFind("DEFAULT");

   if (!b)
      b = BorderGetFallback();

 done:
   ewin->normal_border = ewin->border = b;
}

void
EwinBorderDetach(EWin * ewin)
{
   const Border       *b = ewin->border;
   int                 i;

   if (!b)
      return;

   TooltipsSetPending(0, NULL, NULL);

   EventCallbackUnregister(EoGetWin(ewin), BorderFrameHandleEvents, ewin);
   for (i = 0; i < b->num_winparts; i++)
     {
	EventCallbackUnregister(ewin->bits[i].win,
				BorderWinpartHandleEvents, &ewin->bits[i]);
	if (ewin->bits[i].win)
	   EDestroyWindow(ewin->bits[i].win);
     }
   Efree(ewin->bits);
   ewin->bits = NULL;
   BorderDecRefcount(b);

   ewin->border = NULL;

   if (b->throwaway)
      BorderDestroy((Border *) b);
}

void
EwinBorderSetTo(EWin * ewin, const Border * b)
{
   int                 i;

   if (ewin->border == b)
      return;

   if (!b)
     {
	b = ewin->border;
	ewin->border = NULL;
     }

   if (ewin->border)
      EwinBorderDetach(ewin);

   ewin->border = b;
   BorderIncRefcount(b);
   HintsSetWindowBorder(ewin);

   ewin->state.no_border = b->num_winparts <= 0;

   EventCallbackRegister(EoGetWin(ewin), BorderFrameHandleEvents, ewin);

   if (b->num_winparts > 0)
      ewin->bits = EMALLOC(EWinBit, b->num_winparts);

   for (i = 0; i < b->num_winparts; i++)
     {
	ewin->bits[i].ewin = ewin;	/* Reference to associated Ewin */

	ewin->bits[i].win = ECreateWindow(EoGetWin(ewin), -10, -10, 1, 1, 0);
	ECursorApply(b->part[i].ec, ewin->bits[i].win);
	EMapWindow(ewin->bits[i].win);
	EventCallbackRegister(ewin->bits[i].win,
			      BorderWinpartHandleEvents, &ewin->bits[i]);
	if (b->part[i].flags & FLAG_TITLE)
	   ESelectInput(ewin->bits[i].win, EWIN_BORDER_TITLE_EVENT_MASK);
	else
	   ESelectInput(ewin->bits[i].win, EWIN_BORDER_PART_EVENT_MASK);
	ewin->bits[i].x = -10;
	ewin->bits[i].y = -10;
	ewin->bits[i].w = -10;
	ewin->bits[i].h = -10;
	ewin->bits[i].cx = -99;
	ewin->bits[i].cy = -99;
	ewin->bits[i].cw = -99;
	ewin->bits[i].ch = -99;
	ewin->bits[i].state = 0;
	ewin->bits[i].expose = 0;
	ewin->bits[i].left = 0;
	ewin->bits[i].is = NULL;
     }

   {
      Window             *wl;
      int                 j = 0;

      wl = EMALLOC(Window, b->num_winparts + 1);
      if (!wl)
	 return;
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (b->part[i].ontop)
	      wl[j++] = WinGetXwin(ewin->bits[i].win);
	}
      wl[j++] = WinGetXwin(ewin->win_container);
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (!b->part[i].ontop)
	      wl[j++] = WinGetXwin(ewin->bits[i].win);
	}
      EXRestackWindows(wl, j);
      Efree(wl);
   }

   if (!ewin->state.shaded)
      EMoveWindow(ewin->win_container, b->border.left, b->border.top);

   ewin->update.shape = 1;
   EwinBorderCalcSizes(ewin, 0);
   EwinStateUpdate(ewin);

   SnapshotEwinUpdate(ewin, SNAP_USE_BORDER);
}

void
EwinBorderChange(EWin * ewin, const Border * b, int normal)
{
   if (!b || ewin->border == b ||
       ewin->inh_wm.b.border || ewin->state.fullscreen)
      return;

   EwinBorderSetTo(ewin, b);
   EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
		  ewin->client.w, ewin->client.h, 0);

   if (normal)
      ewin->normal_border = b;
}

static void
EwinBorderAssign(EWin * ewin, const Border * b)
{
   if (!b || ewin->border == b || ewin->inh_wm.b.border)
      return;

   if (ewin->border)
      BorderDecRefcount(ewin->border);
   BorderIncRefcount(b);

   ewin->border = ewin->normal_border = b;
}

void
EwinBorderSetInitially(EWin * ewin, const char *name)
{
   EwinBorderAssign(ewin, BorderFind(name));
}

static Border      *
BorderCreate(const char *name)
{
   Border             *b;

   b = ECALLOC(Border, 1);
   if (!b)
      return NULL;

   if (!border_list)
      border_list = ecore_list_new();
   ecore_list_prepend(border_list, b);

   b->name = Estrdup(name);
   b->group_border_name = NULL;
   b->shadedir = 2;

   return b;
}

static void
BorderDestroy(Border * b)
{
   int                 i;

   if (!b)
      return;

   if (b->ref_count > 0)
     {
	DialogOK("Border Error!", _("%u references remain"), b->ref_count);
	return;
     }

   ecore_list_node_remove(border_list, (Border *) b);

   for (i = 0; i < b->num_winparts; i++)
     {
	ImageclassFree(b->part[i].iclass);
	ActionclassFree(b->part[i].aclass);
	TextclassFree(b->part[i].tclass);
	ECursorFree(b->part[i].ec);
     }

   Efree(b->part);
   Efree(b->name);
   Efree(b->group_border_name);
   ActionclassFree(b->aclass);
   Efree(b);
}

static int
_BorderMatchName(const void *data, const void *match)
{
   return strcmp(((const Border *)data)->name, (const char *)match);
}

Border             *
BorderFind(const char *name)
{
   if (!name)
      return NULL;
   return (Border *) ecore_list_find(border_list, _BorderMatchName, name);
}

static void
BorderWinpartAdd(Border * b, const char *iclass, const char *aclass,
		 const char *tclass, const char *cclass, char ontop, int flags,
		 char isregion __UNUSED__, int wmin, int wmax, int hmin,
		 int hmax, int torigin, int txp, int txa, int typ, int tya,
		 int borigin, int bxp, int bxa, int byp, int bya,
		 char keep_for_shade)
{
   int                 n;

   b->num_winparts++;
   n = b->num_winparts;

   b->part = EREALLOC(WinPart, b->part, n);

   b->part[n - 1].iclass = (iclass) ? ImageclassAlloc(iclass, 1) : NULL;
   b->part[n - 1].aclass = (aclass) ? ActionclassAlloc(aclass) : NULL;
   b->part[n - 1].tclass = (tclass) ? TextclassAlloc(tclass, 1) : NULL;
   b->part[n - 1].ec = (cclass) ? ECursorAlloc(cclass) : NULL;

   b->part[n - 1].ontop = ontop;
   b->part[n - 1].flags = flags;
   b->part[n - 1].keep_for_shade = keep_for_shade;
   b->part[n - 1].geom.width.min = wmin;
   b->part[n - 1].geom.width.max = wmax;
   b->part[n - 1].geom.height.min = hmin;
   b->part[n - 1].geom.height.max = hmax;
   b->part[n - 1].geom.topleft.originbox = torigin;
   b->part[n - 1].geom.topleft.x.percent = txp;
   b->part[n - 1].geom.topleft.x.absolute = txa;
   b->part[n - 1].geom.topleft.y.percent = typ;
   b->part[n - 1].geom.topleft.y.absolute = tya;
   b->part[n - 1].geom.bottomright.originbox = borigin;
   b->part[n - 1].geom.bottomright.x.percent = bxp;
   b->part[n - 1].geom.bottomright.x.absolute = bxa;
   b->part[n - 1].geom.bottomright.y.percent = byp;
   b->part[n - 1].geom.bottomright.y.absolute = bya;
}

void
EwinBorderMinShadeSize(const EWin * ewin, int *mw, int *mh)
{
   int                 i, pw, ph, w, h, min_w, min_h;
   int                 leftborderwidth, rightborderwidth;
   int                 topborderwidth, bottomborderwidth;

   pw = ewin->client.w + ewin->border->border.left + ewin->border->border.right;
   ph = ewin->client.h + ewin->border->border.top + ewin->border->border.bottom;

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i, pw, ph);

   switch (ewin->border->shadedir)
     {
     case 0:
     case 1:
	/* get the correct width, based on the borderparts that */
	/*are remaining visible */
	leftborderwidth = rightborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (!ewin->border->part[i].keep_for_shade)
		continue;

	     w = ewin->border->border.left - ewin->bits[i].x;
	     if (leftborderwidth < w)
		leftborderwidth = w;

	     w = ewin->bits[i].x + ewin->bits[i].w -
		(pw - ewin->border->border.right);
	     if (rightborderwidth < w)
		rightborderwidth = w;
	  }
	pw = rightborderwidth + leftborderwidth;
	break;
     case 2:
     case 3:
	topborderwidth = bottomborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (!ewin->border->part[i].keep_for_shade)
		continue;

	     h = ewin->border->border.top - ewin->bits[i].y;
	     if (topborderwidth < h)
		topborderwidth = h;

	     h = ewin->bits[i].y + ewin->bits[i].h -
		(ph - ewin->border->border.bottom);
	     if (bottomborderwidth < h)
		bottomborderwidth = h;
	  }
	ph = bottomborderwidth + topborderwidth;
	break;
     default:
	break;
     }

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i, pw, ph);

   min_w = 0;
   min_h = 0;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (!ewin->border->part[i].keep_for_shade)
	   continue;

	w = ewin->bits[i].x + ewin->bits[i].w;
	if (min_w < w)
	   min_w = w;

	h = ewin->bits[i].y + ewin->bits[i].h;
	if (min_h < h)
	   min_h = h;
     }

   *mw = min_w;
   *mh = min_h;
}

int
BorderWinpartIndex(EWin * ewin, Win win)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (win == ewin->bits[i].win)
	   return i;
     }

   return -1;			/* Not found */
}

/*
 * Border event handlers
 */
#define DEBUG_BORDER_EVENTS 0

static void
BorderWinpartEventMouseDown(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

#if 0				/* Remove? */
   GrabPointerSet(wbit->win, 0, 0);
#endif

   wbit->state = STATE_CLICKED;
#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventMouseDown %#lx %d\n", WinGetXwin(wbit->win),
	   wbit->state);
#endif
   BorderWinpartChange(ewin, part, 0);

   FocusHandleClick(ewin, wbit->win);

   if (ewin->border->part[part].aclass)
      ActionclassEvent(ewin->border->part[part].aclass, ev, ewin);
}

static void
BorderWinpartEventMouseUp(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;
   int                 left = wbit->left;

   if (ev)
     {
#if 0				/* Remove? */
	GrabPointerRelease();
#endif
	/*
	 * During a move/resize operation initiated by a border part click
	 * the ButtonRelease event may be reported on the border part window
	 * if the event happens before the pointer grab is moved to the
	 * move/resize event window.
	 * This can be tested e.g. by inserting usleep(100000) at the start
	 * of BorderWinpartEventMouseDown().
	 */
	MoveResizeEnd(ewin);
     }

   if ((wbit->state == STATE_CLICKED) && (!wbit->left))
      wbit->state = STATE_HILITED;
   else
      wbit->state = STATE_NORMAL;
#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventMouseUp %#lx %d\n", WinGetXwin(wbit->win),
	   wbit->state);
#endif
   BorderWinpartChange(ewin, part, 0);

   /* Beware! Actions may destroy the current border */
   wbit->left = 0;

   if (ev && WinGetXwin(wbit->win) == Mode.events.last_bpress && !left &&
       ewin->border->part[part].aclass)
      ActionclassEvent(ewin->border->part[part].aclass, ev, ewin);
}

static void
BorderWinpartEventEnter(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventEnter %#lx %d\n", WinGetXwin(wbit->win),
	   wbit->state);
#endif
   if (wbit->state == STATE_CLICKED)
      wbit->left = 0;

   wbit->state = STATE_HILITED;
   BorderWinpartChange(ewin, part, 0);
   if (ewin->border->part[part].aclass)
      ActionclassEvent(ewin->border->part[part].aclass, ev, ewin);
}

static void
BorderWinpartEventLeave(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventLeave %#lx %d\n", WinGetXwin(wbit->win),
	   wbit->state);
#endif
   if (wbit->state == STATE_CLICKED)
      wbit->left = 1;
   else
     {
	wbit->state = STATE_NORMAL;
	BorderWinpartChange(ewin, part, 0);
	if (ewin->border->part[part].aclass)
	   ActionclassEvent(ewin->border->part[part].aclass, ev, ewin);
     }
}

void
BorderCheckState(EWin * ewin, XEvent * ev)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	switch (ev->type)
	  {
	  default:
	     break;

	  case ButtonRelease:
	     BorderWinpartEventMouseUp(ewin->bits + i, NULL);
	     break;
	  }
     }
}

static int
_BorderAutoshadeTimeout(void *data)
{
   EWin               *ewin = (EWin *) data;

   if (!EwinFindByPtr(ewin))	/* Check, window may be gone */
      return 0;

   ewin->timer = NULL;
   EwinOpShade(ewin, OPSRC_USER, 1);

   return 0;
}

static void
BorderFrameHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EWin               *ewin = (EWin *) prm;
   int                 x, y;

   switch (ev->type)
     {
     case EnterNotify:
	if (ewin->props.autoshade)
	  {
	     EwinOpShade(ewin, OPSRC_USER, 0);
	  }
	if (ewin->border->aclass)
	   ActionclassEvent(ewin->border->aclass, ev, ewin);
	break;
     case LeaveNotify:
	if (ewin->props.autoshade && !ewin->state.shaded)
	  {
	     EQueryPointer(EoGetWin(ewin), &x, &y, NULL, NULL);
	     if (x >= 4 && x < EoGetW(ewin) - 4 &&
		 y >= 4 && y < EoGetH(ewin) - 4)
		break;
	     TIMER_DEL(ewin->timer);
	     TIMER_ADD(ewin->timer, 500, _BorderAutoshadeTimeout, ewin);
	  }
	if (ewin->border->aclass)
	   ActionclassEvent(ewin->border->aclass, ev, ewin);
	break;
     }
}

static ActionClass *
BorderWinpartGetAclass(void *data)
{
   EWinBit            *wbit = (EWinBit *) data;
   EWin               *ewin;
   int                 part;

   /* Validate border part */
   ewin = Mode.mouse_over_ewin;
   if (!ewin)
      return NULL;

   part = wbit - ewin->bits;
   if (part < 0 || part >= ewin->border->num_winparts)
      return NULL;

   return ewin->border->part[part].aclass;
}

static void
BorderWinpartHandleTooltip(EWinBit * wbit)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

   if (!ewin->border->part[part].aclass)
      return;
   TooltipsSetPending(0, BorderWinpartGetAclass, wbit);
}

static void
BorderWinpartHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EWinBit            *wbit = (EWinBit *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	BorderWinpartEventMouseDown(wbit, ev);
	break;
     case ButtonRelease:
	BorderWinpartEventMouseUp(wbit, ev);
	break;
     case EnterNotify:
	/* Beware! Actions may destroy the current border */
	BorderWinpartHandleTooltip(wbit);
	BorderWinpartEventEnter(wbit, ev);
	break;
     case LeaveNotify:
	BorderWinpartEventLeave(wbit, ev);
	break;
     case MotionNotify:
	BorderWinpartHandleTooltip(wbit);
	break;
     }
}

/*
 * Configuration load/save
 */
#include "conf.h"

static int
BorderPartLoad(FILE * fs, char type __UNUSED__, Border * b)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, i2;
   char                iclass[64], aclass[64], tclass[64], cclass[64];
   char               *piclass, *paclass, *ptclass, *pcclass;
   char                ontop = 1;
   int                 flags = FLAG_BUTTON;
   char                isregion = 0, keepshade = 1;
   int                 wmin = 0, wmax = 0, hmin = 0, hmax = 0, torigin =
      0, txp = 0, txa = 0, typ = 0, tya = 0, borigin = 0;
   int                 bxp = 0, bxa = 0, byp = 0, bya = 0;

   piclass = paclass = ptclass = pcclass = NULL;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	i2 = atoi(s2);
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     BorderWinpartAdd(b, piclass, paclass, ptclass, pcclass, ontop,
			      flags, isregion, wmin, wmax, hmin, hmax,
			      torigin, txp, txa, typ, tya,
			      borigin, bxp, bxa, byp, bya, keepshade);
	     goto done;
	  case CONFIG_IMAGECLASS:
	  case BORDERPART_ICLASS:
	     STRCPY(iclass, s2);
	     piclass = iclass;
	     break;
	  case CONFIG_ACTIONCLASS:
	  case BORDERPART_ACLASS:
	     STRCPY(aclass, s2);
	     paclass = aclass;
	     break;
	  case CONFIG_TEXT:
	  case BORDERPART_TEXTCLASS:
	     STRCPY(tclass, s2);
	     ptclass = tclass;
	     break;
	  case CONFIG_CURSOR:
	     STRCPY(cclass, s2);
	     pcclass = cclass;
	     break;
	  case BORDERPART_ONTOP:
	     ontop = i2;
	     break;
	  case BORDERPART_FLAGS:
	     flags = i2;
	     break;
	  case BORDERPART_ISREGION:
	     isregion = i2;
	     break;
	  case BORDERPART_WMIN:
	     wmin = i2;
	     if (!wmax)
		wmax = wmin;
	     break;
	  case BORDERPART_WMAX:
	     wmax = i2;
	     break;
	  case BORDERPART_HMIN:
	     hmin = i2;
	     if (!hmax)
		hmax = hmin;
	     break;
	  case BORDERPART_HMAX:
	     hmax = i2;
	     break;
	  case BORDERPART_TORIGIN:
	     torigin = i2;
	     break;
	  case BORDERPART_TXP:
	     txp = i2;
	     break;
	  case BORDERPART_TXA:
	     txa = i2;
	     break;
	  case BORDERPART_TYP:
	     typ = i2;
	     break;
	  case BORDERPART_TYA:
	     tya = i2;
	     break;
	  case BORDERPART_BORIGIN:
	     borigin = i2;
	     break;
	  case BORDERPART_BXP:
	     bxp = i2;
	     break;
	  case BORDERPART_BXA:
	     bxa = i2;
	     break;
	  case BORDERPART_BYP:
	     byp = i2;
	     break;
	  case BORDERPART_BYA:
	     bya = i2;
	     break;
	  case BORDERPART_KEEPSHADE:
	     keepshade = i2;
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
BorderConfigLoad(FILE * fs)
{
   int                 err = 0;
   Border             *b = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, i2;

   while (GetLine(s, sizeof(s), fs))
     {
	i1 = ConfigParseline1(s, s2, NULL, NULL);
	i2 = atoi(s2);

	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     goto done;
	  case CONFIG_CLASSNAME:
	  case BORDER_NAME:
	     if (BorderFind(s2))
	       {
		  SkipTillEnd(fs);
		  goto done;
	       }
	     b = BorderCreate(s2);
	     continue;
	  }

	if (!b)
	   break;

	switch (i1)
	  {
	  default:
	     break;
	  case BORDER_INIT:
	     if (i2 != CONFIG_OPEN)
		break;
	     err = BorderPartLoad(fs, i1, b);
	     if (err)
		break;
	     break;
	  case BORDER_GROUP_NAME:
	     b->group_border_name = Estrdup(s2);
	     break;
	  case BORDER_LEFT:
	     b->border.left = i2;
	     break;
	  case BORDER_RIGHT:
	     b->border.right = i2;
	     break;
	  case BORDER_TOP:
	     b->border.top = i2;
	     break;
	  case BORDER_BOTTOM:
	     b->border.bottom = i2;
	     break;
	  case BORDER_SHADEDIR:
	     b->shadedir = i2;
	     break;
	  case BORDER_CHANGES_SHAPE:
	     b->changes_shape = i2;
	     break;
	  case CONFIG_ACTIONCLASS:
	     b->aclass = ActionclassFind(s2);
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

Border             *
BorderCreateFiller(int w, int h, int sw, int sh)
{
   Border             *b;
   int                 left, right, top, bottom;

   if (w > sw || h > sh)	/* Borders must be >= 0 */
      return NULL;

   b = BorderCreate("__FILLER");
   if (!b)
      return b;

   left = (sw - w) / 2;
   left = (left < 0) ? 0 : left;
   right = sw - w - left;
   top = (sh - h) / 2;
   top = (top < 0) ? 0 : top;
   bottom = sh - h - top;

   b->throwaway = 1;

   b->border.left = left;
   b->border.right = right;
   b->border.top = top;
   b->border.bottom = bottom;

   ImageclassGetBlack();	/* Creates the __BLACK ImageClass */

   if (top)
      BorderWinpartAdd(b, "__BLACK", NULL, NULL, NULL, 1, FLAG_BUTTON, 0,
		       1, 99999, 1, 99999,
		       -1, 0, 0, 0, 0, -1, 1024, -1, 0, top - 1, 1);
   if (bottom)
      BorderWinpartAdd(b, "__BLACK", NULL, NULL, NULL, 1, FLAG_BUTTON, 0,
		       1, 99999, 1, 99999,
		       -1, 0, 0, 1024, -bottom, -1, 1024, -1, 1024, -1, 1);
   if (left)
      BorderWinpartAdd(b, "__BLACK", NULL, NULL, NULL, 1, FLAG_BUTTON, 0,
		       1, 99999, 1, 99999,
		       -1, 0, 0, 0, top,
		       -1, 0, left - 1, 1024, -(bottom + 1), 1);
   if (right)
      BorderWinpartAdd(b, "__BLACK", NULL, NULL, NULL, 1, FLAG_BUTTON, 0,
		       1, 99999, 1, 99999,
		       -1, 1024, -right, 0, top,
		       -1, 1024, -1, 1024, -(bottom + 1), 1);

   return b;
}

void
BordersForeach(void (*func) (Border * b, void *data), void *data)
{
   ecore_list_for_each(border_list, (Ecore_For_Each) func, data);
}

Border            **
BordersGetList(int *pnum)
{
   return (Border **) ecore_list_items_get(border_list, pnum);
}

static ActionClass *
BorderGetFallbackAclass(void)
{
   ActionClass        *ac;
   Action             *aa;

   ac = ActionclassFind("__fb_bd_ac");
   if (ac)
      return ac;

   /* Create fallback actionclass for the fallback border */
   ac = ActionclassCreate("__fb_bd_ac", 0);
   if (!ac)
      return ac;

   aa = ActionCreate(EVENT_MOUSE_DOWN, 1, 0, 0, 1, 0, NULL, NULL);
   ActionclassAddAction(ac, aa);
   ActionAddTo(aa, "wop * mo ptr");

   aa = ActionCreate(EVENT_MOUSE_DOWN, 1, 0, 0, 2, 0, NULL, NULL);
   ActionclassAddAction(ac, aa);
   ActionAddTo(aa, "wop * close");

   aa = ActionCreate(EVENT_MOUSE_DOWN, 1, 0, 0, 3, 0, NULL, NULL);
   ActionclassAddAction(ac, aa);
   ActionAddTo(aa, "wop * sz ptr");

   return ac;
}

static Border      *
BorderGetFallback(void)
{
   /*
    * This function creates simple internal data members to be used in 
    * emergencies - ie when all else fails - ie a button is told to use an
    * imageclass that doesn't exist, or no DEFAULT border is defined... at 
    * least E won't barf on us then.
    */
   Border             *b;

   b = BorderFind("__fb_bd");
   if (b)
      return b;

   BorderGetFallbackAclass();	/* Creates the fallback ac */

   /* Create fallback border */
   b = BorderCreate("__fb_bd");
   if (!b)
      return b;

   b->border.left = 8;
   b->border.right = 8;
   b->border.top = 8;
   b->border.bottom = 8;
   BorderWinpartAdd(b, "__fb_ic", "__fb_bd_ac", NULL, NULL,
		    1, FLAG_BUTTON, 0, 8, 99999, 8, 99999, -1, 0, 0, 0, 0,
		    -1, 1024, -1, 0, 7, 1);
   BorderWinpartAdd(b, "__fb_ic", "__fb_bd_ac", NULL, NULL,
		    1, FLAG_BUTTON, 0, 8, 99999, 8, 99999, -1, 0, 0, 1024,
		    -8, -1, 1024, -1, 1024, -1, 1);
   BorderWinpartAdd(b, "__fb_ic", "__fb_bd_ac", NULL, NULL,
		    1, FLAG_BUTTON, 0, 8, 99999, 8, 99999, -1, 0, 0, 0, 8,
		    -1, 0, 7, 1024, -9, 1);
   BorderWinpartAdd(b, "__fb_ic", "__fb_bd_ac", NULL, NULL,
		    1, FLAG_BUTTON, 0, 8, 99999, 8, 99999, -1, 1024, -8, 0,
		    8, -1, 1024, -1, 1024, -9, 1);

   return b;
}
