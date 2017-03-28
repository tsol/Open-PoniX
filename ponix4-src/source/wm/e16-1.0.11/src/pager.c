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
#include "backgrounds.h"
#include "desktops.h"
#include "dialog.h"
#include "e16-ecore_list.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "ewins.h"
#include "focus.h"
#include "groups.h"
#include "hints.h"
#include "hiwin.h"
#include "iclass.h"
#include "menus.h"
#include "settings.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"

#define DEBUG_PAGER 0
#if DEBUG_PAGER
#define Dprintf(fmt...) if(EDebug(EDBUG_TYPE_PAGER))Eprintf(fmt)
#else
#define Dprintf(fmt...)
#endif

#define USE_PAGER_BACKGROUND_CACHE 1
#define HIQ ((Conf_pagers.hiq) ? 0x200 : 0x100)

#define PAGER_MODE_SIMPLE 0
#define PAGER_MODE_SNAP   1
#define PAGER_MODE_LIVE   2

#define PAGER_UPD_EWIN_GEOM     0
#define PAGER_UPD_EWIN_GONE     1
#define PAGER_UPD_EWIN_DAMAGE   2

#define EwinGetVX(ew) (ew->vx)
#define EwinGetVY(ew) (ew->vy)
#define EwinGetVX2(ew) (ew->vx + EoGetW(ew))
#define EwinGetVY2(ew) (ew->vy + EoGetH(ew))

static struct {
   char                enable;
   char                zoom;
   char                title;
   char                hiq;
   int                 mode;
   int                 scanspeed;
   int                 sel_button;
   int                 win_button;
   int                 menu_button;
} Conf_pagers;

static struct {
   int                 zoom;
   Idler              *idler;
   char                update_pending;
   char                timer_pending;
} Mode_pagers;

typedef struct {
   EWin               *ewin;
   Win                 win;
   int                 w, h;
   char               *name;
   Pixmap              bgpmap;
   Desk               *dsk;
   int                 dw, dh;
   int                 screen_w, screen_h;
   int                 update_phase;
   Win                 sel_win;
   Timer              *scan_timer;

   /* State flags */
   char                do_newbg;
   char                do_update;
   int                 x1, y1, x2, y2;
   float               scale;
   unsigned int        serial;
   int                 serdif;
} Pager;

static void         PagerScanCancel(Pager * p);
static int          PagerScanTimeout(void *data);
static void         PagerCheckUpdate(Pager * p, void *prm);
static void         PagerUpdateEwinsFromPager(Pager * p);
static void         PagerHiwinHide(void);
static void         PagerEvent(Win win, XEvent * ev, void *prm);
static void         PagerHiwinEvent(Win win, XEvent * ev, void *prm);

static Ecore_List  *pager_list = NULL;

static Hiwin       *hiwin = NULL;

static int
PagersGetMode(void)
{
   int                 mode = Conf_pagers.mode;

   if (mode == PAGER_MODE_LIVE && !ECompMgrIsActive())
      mode = PAGER_MODE_SNAP;

   return mode;
}

static Pager       *
PagerCreate(void)
{
   Pager              *p;

   if (!Conf_pagers.enable)
      return NULL;

   p = ECALLOC(Pager, 1);
   if (!p)
      return NULL;

   if (!pager_list)
      pager_list = ecore_list_new();
   ecore_list_append(pager_list, p);

   p->name = NULL;
   p->win = ECreateClientWindow(VROOT, 0, 0, 1, 1);
   EventCallbackRegister(p->win, PagerEvent, p);
   p->sel_win = ECreateWindow(p->win, 0, 0, 1, 1, 0);

   return p;
}

static void
PagerDestroy(Pager * p)
{
   ecore_list_node_remove(pager_list, p);

   PagerScanCancel(p);
   Efree(p->name);
   PagerHiwinHide();
   if (p->bgpmap != None)
      EFreePixmap(p->bgpmap);

   Efree(p);
}

static void
PagerScanTrig(Pager * p)
{
   if (p->scan_timer || Conf_pagers.scanspeed <= 0)
      return;

   TIMER_ADD(p->scan_timer, 1000 / Conf_pagers.scanspeed, PagerScanTimeout, p);
}

static void
PagerScanCancel(Pager * p)
{
   TIMER_DEL(p->scan_timer);
}

static int
PagerScanTimeout(void *data)
{
   Pager              *p;
   EWin               *ewin;
   int                 y, y2, phase, cx, cy, ww, hh, xx, yy;
   static int          offsets[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
   int                 pager_mode = PagersGetMode();

   p = (Pager *) data;

   if (pager_mode != PAGER_MODE_SNAP)
      goto nomore;

   ewin = p->ewin;
   if (!ewin || !EoIsShown(ewin))
      goto nomore;
   if (p->dsk != DesksGetCurrent())
      goto nomore;
   if (ewin->state.visibility == VisibilityFullyObscured)
      goto nomore;

   if (Conf_pagers.scanspeed <= 0)
      goto nomore;

   if (Mode.mode != MODE_NONE)
      goto nomore;

   DeskCurrentGetArea(&cx, &cy);
   ww = p->dw;
   hh = p->dh;
   xx = cx * ww;
   yy = cy * hh;
   phase = p->update_phase;
   if (ww <= 0 || hh <= 0)
      goto nomore;

#if 0
   /* Due to a bug in imlib2 <= 1.2.0 we have to scan left->right in stead
    * of top->bottom, at least for now. */
   y = ((phase & 0xfffffff8) + offsets[phase % 8]) % hh;
   y2 = (y * WinGetH(VROOT)) / hh;

   ScaleRect(VROOT, WinGetXwin(VROOT), p->win, WinGetPmap(p->win), 0,
	     y2, WinGetW(VROOT), WinGetH(VROOT) / hh, xx, yy + y, ww, 1, HIQ);
   EClearArea(p->win, xx, yy + y, ww, 1);
   y2 = p->h;
#else
   y = ((phase & 0xfffffff8) + offsets[phase % 8]) % ww;
   y2 = (y * WinGetW(VROOT)) / ww;

   ScaleRect(VROOT, WinGetXwin(VROOT), p->win, WinGetPmap(p->win), y2,
	     0, WinGetW(VROOT) / ww, WinGetH(VROOT), xx + y, yy, 1, hh, HIQ);
   EClearArea(p->win, xx + y, yy, 1, hh);
   y2 = p->w;
#endif
   p->update_phase++;
   if (p->update_phase >= y2)
     {
	PagerUpdateEwinsFromPager(p);
	p->update_phase = 0;
     }

   TimerSetInterval(p->scan_timer, 1000 / Conf_pagers.scanspeed);
   return 1;

 nomore:
   p->scan_timer = NULL;
   return 0;
}

#if 0				/* FIXME - Remove? */
static void
PagerHiwinUpdate(Hiwin * phi, Pager * p __UNUSED__, EWin * ewin)
{
   EImage             *im;

   if (!EoIsShown(phi) || !ewin->mini_pmm.pmap)
      return;

   im = EImageGrabDrawable(ewin->mini_pmm.pmap, None, 0, 0,
			   ewin->mini_pmm.w, ewin->mini_pmm.h, 0);
   EImageRenderOnDrawable(im, EoGetWin(phi), 0, 0, 0, EoGetW(phi), EoGetH(phi));
   EImageDecache(im);
}
#endif

static void
PagerEwinUpdateMini(Pager * p, EWin * ewin)
{
   int                 w, h, update, use_iclass, serdif;
   Drawable            draw;
   int                 pager_mode = PagersGetMode();

   w = (EoGetW(ewin) * p->dw) / WinGetW(VROOT);
   h = (EoGetH(ewin) * p->dh) / WinGetH(VROOT);

   if (w < 1)
      w = 1;
   if (h < 1)
      h = 1;

   serdif = EoGetSerial(ewin) - p->serial;

   update = 0;
   if (!ewin->mini_pmm.pmap)
      update = 1;
   if (ewin->mini_pmm.w != w || ewin->mini_pmm.h != h)
      update = 1;

   if (serdif > 0 && ewin->type != EWIN_TYPE_PAGER &&
       pager_mode == PAGER_MODE_LIVE && Mode.mode == 0)
      update = 1;
   if (serdif > p->serdif)
      p->serdif = serdif;

   if (!update)
      return;

   Dprintf("PagerEwinUpdateMini %#lx/%#lx wxh=%dx%d ser=%#x/%#x dif=%d: %s\n",
	   EwinGetClientXwin(ewin), EoGetXwin(ewin), w, h,
	   EoGetSerial(ewin), p->serial, serdif, EwinGetTitle(ewin));

   p->do_update = 1;

   PmapMaskInit(&ewin->mini_pmm, EoGetWin(ewin), w, h);

   draw = None;
   if (pager_mode != PAGER_MODE_SIMPLE)
     {
	draw = EoGetPixmap(ewin);
	if (draw == None && EwinIsOnScreen(ewin))
	   draw = EoGetXwin(ewin);
     }
   use_iclass = draw == None;

   if (use_iclass)
     {
	ImageClass         *ic;

	ic = ImageclassFind("PAGER_WIN", 1);
	ImageclassApplySimple(ic, EoGetWin(ewin), ewin->mini_pmm.pmap,
			      STATE_NORMAL, 0, 0, w, h);
	Dprintf("Use Iclass, pmap=%#lx\n", ewin->mini_pmm.pmap);
     }
   else
     {
	ScaleRect(EoGetWin(ewin), draw, EoGetWin(ewin), ewin->mini_pmm.pmap,
		  0, 0, EoGetW(ewin), EoGetH(ewin), 0, 0, w, h, HIQ);
	Dprintf("Grab scaled, pmap=%#lx\n", ewin->mini_pmm.pmap);
     }

#if 0				/* FIXME - Remove? */
   if (hiwin && ewin == hiwin->ewin)
      PagerHiwinUpdate(hiwin, p, ewin);
#endif
}

static void
doPagerUpdate(Pager * p)
{
   int                 x, y, ax, ay, cx, cy, vx, vy;
   GC                  gc = None;
   EWin               *const *lst;
   int                 i, num, update_screen_included, update_screen_only;
   int                 pager_mode = PagersGetMode();
   Pixmap              pmap;

#if USE_COMPOSITE
   Picture             pager_pict, pict, alpha;
#endif

   p->update_phase = 0;
   DesksGetAreaSize(&ax, &ay);
   DeskGetArea(p->dsk, &cx, &cy);
   vx = cx * WinGetW(VROOT);
   vy = cy * WinGetH(VROOT);

   update_screen_included = update_screen_only = 0;
   if (pager_mode == PAGER_MODE_SNAP && p->dsk == DesksGetCurrent()
       && Mode.mode == 0)
     {
	/* Update from screen unless update area is entirely off-screen */
	if (!(p->x2 <= vx || p->y2 <= vy ||
	      p->x1 >= vx + WinGetW(VROOT) || p->y1 >= vy + WinGetH(VROOT)))
	   update_screen_included = 1;

	/* Check if update area is entirely on-screen */
	if (p->x1 >= vx && p->y1 >= vy &&
	    p->x2 <= vx + WinGetW(VROOT) && p->y2 <= vy + WinGetH(VROOT))
	   update_screen_only = 1;
     }
   p->x1 = p->y1 = 99999;
   p->x2 = p->y2 = -99999;

   pmap = EGetWindowBackgroundPixmap(p->win);

   if (update_screen_only)
      goto do_screen_update;

   lst = EwinListGetForDesk(&num, p->dsk);
   for (i = num - 1; i >= 0; i--)
     {
	EWin               *ewin;

	ewin = lst[i];
	if (!EoIsShown(ewin))
	   continue;

	PagerEwinUpdateMini(p, ewin);
     }
   if (p->serdif > 0)
      p->serial += p->serdif;
   p->serdif = 0;

   if (!p->do_update)
      return;
   p->do_update = 0;

   gc = EXCreateGC(pmap, 0, NULL);
   if (gc == None)
      return;

   Dprintf("doPagerUpdate %d: Repaint\n", p->dsk->num);
   for (y = 0; y < ay; y++)
     {
	for (x = 0; x < ax; x++)
	  {
#if 0				/* Skip? */
	     if (update_screen_included && x == cx && y == cy)
		continue;
#endif
	     XCopyArea(disp, p->bgpmap, pmap, gc, 0, 0, p->dw, p->dh,
		       x * p->dw, y * p->dh);
	  }
     }

#if USE_COMPOSITE
   pager_pict = EPictureCreate(NULL, pmap);
#endif
   for (i = num - 1; i >= 0; i--)
     {
	EWin               *ewin;
	int                 wx, wy, ww, wh;

	ewin = lst[i];
	if (!EoIsShown(ewin))
	   continue;

	wx = (EwinGetVX(ewin) * p->dw) / WinGetW(VROOT);
	wy = (EwinGetVY(ewin) * p->dh) / WinGetH(VROOT);
	ww = (EoGetW(ewin) * p->dw) / WinGetW(VROOT);
	wh = (EoGetH(ewin) * p->dh) / WinGetH(VROOT);

	if (ewin->mini_pmm.pmap)
	  {
#if USE_COMPOSITE
	     /*      pmap set by           depth determined by
	      * PagerEwinUpdateMini()           ewin
	      * PagerEwinUpdateFromPager()      p->win
	      */
	     pict = EPictureCreate(ewin->mini_pmm.depth == WinGetDepth(p->win) ?
				   p->win : EoGetWin(ewin),
				   ewin->mini_pmm.pmap);
	     alpha = ECompMgrWinGetAlphaPict(EoObj(ewin));
	     XRenderComposite(disp, PictOpOver, pict, alpha, pager_pict,
			      0, 0, 0, 0, wx, wy, ww, wh);
	     EPictureDestroy(pict);
#else
#if 0				/* Mask is currently not set anywhere */
	     if (ewin->mini_pmm.mask)
	       {
		  XSetClipMask(disp, gc, ewin->mini_pmm.mask);
		  XSetClipOrigin(disp, gc, wx, wy);
	       }
#endif
	     XCopyArea(disp, ewin->mini_pmm.pmap, pmap, gc, 0, 0,
		       ww, wh, wx, wy);
#if 0				/* Mask is currently not set anywhere */
	     if (ewin->mini_pmm.mask)
		XSetClipMask(disp, gc, None);
#endif
#endif
	  }
	else
	  {
	     XSetForeground(disp, gc, Dpy.pixel_black);
	     XDrawRectangle(disp, pmap, gc, wx - 1, wy - 1, ww + 1, wh + 1);
	     XSetForeground(disp, gc, Dpy.pixel_white);
	     XFillRectangle(disp, pmap, gc, wx, wy, ww, wh);
	  }
     }
#if USE_COMPOSITE
   EPictureDestroy(pager_pict);
#endif

   if (!update_screen_included)
     {
	EClearWindow(p->win);
	goto done;
     }

 do_screen_update:
   Dprintf("doPagerUpdate %d: Snap screen\n", p->dsk->num);
   /* Update pager area by snapshotting entire screen */
   ScaleRect(VROOT, WinGetXwin(VROOT), p->win, pmap, 0, 0,
	     WinGetW(VROOT), WinGetH(VROOT), cx * p->dw, cy * p->dh,
	     p->dw, p->dh, HIQ);

   EClearWindow(p->win);

   /* Update ewin snapshots */
   PagerUpdateEwinsFromPager(p);

 done:
   EXFreeGC(gc);
}

static void
PagerUpdate(Pager * p, int why, int x1, int y1, int x2, int y2)
{
   if (!Conf_pagers.enable)
      return;

   if (p->x1 > x1)
      p->x1 = x1;
   if (p->y1 > y1)
      p->y1 = y1;
   if (p->x2 < x2)
      p->x2 = x2;
   if (p->y2 < y2)
      p->y2 = y2;

   p->do_update = 1;
   Mode_pagers.update_pending |= 1 << why;
}

static void
PagerReconfigure(Pager * p, int apply)
{
   int                 ax, ay, w, h, dx, dy;
   double              aspect, f;

   DesksGetAreaSize(&ax, &ay);

   aspect = ((double)WinGetW(VROOT)) / ((double)WinGetH(VROOT));

   dx = 2;
   for (;;)
     {
	f = dx / aspect;
	dy = (int)(f + .5);
	f -= (double)dy;
	if (f < 0)
	   f = -f;
	if (f < .1)
	   break;
	if (dx >= 8)
	   break;
	dx += 1;
     }

   ICCCM_SetSizeConstraints(p->ewin,
			    WinGetW(VROOT) / 64 * ax,
			    WinGetH(VROOT) / 64 * ay,
			    WinGetW(VROOT) / 4 * ax,
			    WinGetH(VROOT) / 4 * ay, 0, 0, dx * ax, dy * ay,
			    aspect * ((double)ax / (double)ay),
			    aspect * ((double)ax / (double)ay));

   if (apply)
     {
	w = (int)((double)ax * (double)WinGetW(VROOT) / (double)p->scale + .5);
	h = (int)((double)ay * (double)WinGetH(VROOT) / (double)p->scale + .5);
	EwinResize(p->ewin, w + (dx * ax) / 2, h + (dy * ay) / 2, 0);
	EwinReposition(p->ewin);
     }
}

static void
PagerUpdateBg(Pager * p)
{
   Pixmap              pmap;
   GC                  gc;
   Background         *bg;
   ImageClass         *ic;
   int                 pager_mode = PagersGetMode();

   p->x1 = p->y1 = 0;
   p->x2 = p->y2 = 99999;

   pmap = p->bgpmap;
   if (pmap != None)
      EFreePixmap(pmap);
   pmap = p->bgpmap = ECreatePixmap(p->win, p->dw, p->dh, 0);

   bg = DeskBackgroundGet(p->dsk);
   if (pager_mode != PAGER_MODE_SIMPLE && bg)
     {
#if USE_PAGER_BACKGROUND_CACHE
	char                s[4096];
	char               *uniq;
	EImage             *im;

	uniq = BackgroundGetUniqueString(bg);
	Esnprintf(s, sizeof(s), "%s/cached/pager/%s.%i.%i.%s.png",
		  EDirUserCache(), BackgroundGetName(bg), p->dw, p->dh, uniq);
	Efree(uniq);

	im = EImageLoad(s);
	if (im)
	  {
	     EImageRenderOnDrawable(im, p->win, pmap, 0, 0, 0, p->dw, p->dh);
	     EImageDecache(im);
	  }
	else
	  {
#endif
	     BackgroundApplyPmap(bg, p->win, pmap, p->dw, p->dh);
#if USE_PAGER_BACKGROUND_CACHE
	     im = EImageGrabDrawable(pmap, None, 0, 0, p->dw, p->dh, 0);
	     EImageSave(im, s);
	     EImageDecache(im);
	  }
#endif
	return;
     }

   if (pager_mode != PAGER_MODE_SIMPLE && p->dsk->bg.pmap)
     {
	ScaleTile(VROOT, p->dsk->bg.pmap, p->win, pmap,
		  0, 0, p->dw, p->dh, HIQ);
	return;
     }

   ic = ImageclassFind("PAGER_BACKGROUND", 1);
   if (ic)
     {
	ImageclassApplySimple(ic, p->win, pmap, STATE_NORMAL,
			      0, 0, p->dw, p->dh);
	return;
     }

   gc = EXCreateGC(pmap, 0, NULL);
   if (gc == None)
      return;

   XSetForeground(disp, gc, Dpy.pixel_black);
   XDrawRectangle(disp, pmap, gc, 0, 0, p->dw, p->dh);
   XSetForeground(disp, gc, Dpy.pixel_white);
   XFillRectangle(disp, pmap, gc, 1, 1, p->dw - 2, p->dh - 2);

   EXFreeGC(gc);
}

static void
_PagerEwinInit(EWin * ewin)
{
   Pager              *p = (Pager *) ewin->data;
   char                s[128];

   p->ewin = ewin;

   Esnprintf(s, sizeof(s), "Pager-%i", p->dsk->num);
   EwinSetTitle(ewin, s);
   Esnprintf(s, sizeof(s), "%i", p->dsk->num);
   EwinSetClass(ewin, s, "Enlightenment_Pager");

   ewin->props.skip_ext_task = 1;
   ewin->props.skip_ext_pager = 1;
   ewin->props.skip_focuslist = 1;
   ewin->props.skip_winlist = 1;
   EwinInhSetWM(ewin, focus, 1);
   ewin->props.autosave = 1;

   EoSetSticky(ewin, 1);
}

static void
_PagerEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Pager              *p = (Pager *) ewin->data;
   int                 w, h;
   int                 ax, ay, cx, cy;
   ImageClass         *ic;

   if (!Conf_pagers.enable || !p || Mode.mode != MODE_NONE)
      return;

   w = ewin->client.w;
   h = ewin->client.h;
   if ((w == p->w && h == p->h) || w <= 1 || h <= 1)
      return;

   DesksGetAreaSize(&ax, &ay);

   p->w = w;
   p->h = h;
   p->dw = w / ax;
   p->dh = h / ay;

   if (p->scale <= 0. || Mode.op_source == OPSRC_USER)
      p->scale = ((float)WinGetW(VROOT) / p->dw +
		  (float)WinGetH(VROOT) / p->dh) / 2;

   p->do_newbg = 1;
   PagerCheckUpdate(p, NULL);

   ic = ImageclassFind("PAGER_SEL", 0);
   if (ic)
     {
	DeskGetArea(p->dsk, &cx, &cy);
	EMoveResizeWindow(p->sel_win, cx * p->dw, cy * p->dh, p->dw, p->dh);
	ImageclassApply(ic, p->sel_win, 0, 0, STATE_NORMAL, ST_PAGER);
     }
}

static void
_PagerEwinClose(EWin * ewin)
{
   PagerDestroy((Pager *) ewin->data);
   ewin->data = NULL;
}

static const EWinOps _PagerEwinOps = {
   _PagerEwinInit,
   NULL,
   _PagerEwinMoveResize,
   _PagerEwinClose,
};

static void
PagerShow(Pager * p)
{
   EWin               *ewin;

   if (!Conf_pagers.enable)
      return;

   if (p->ewin)
     {
	EwinShow(p->ewin);
	return;
     }

   ewin = AddInternalToFamily(p->win, "PAGER", EWIN_TYPE_PAGER,
			      &_PagerEwinOps, p);
   if (!ewin)
      return;

   p->screen_w = WinGetW(VROOT);
   p->screen_h = WinGetH(VROOT);

   PagerReconfigure(p, 0);

   ewin->client.event_mask |=
      ButtonPressMask | ButtonReleaseMask |
      EnterWindowMask | LeaveWindowMask | PointerMotionMask;
   ESelectInput(p->win, ewin->client.event_mask);

   if (ewin->state.placed)
     {
	EwinMoveResize(ewin, EoGetX(ewin), EoGetY(ewin),
		       ewin->client.w, ewin->client.h, 0);
     }
   else
     {
	/* no snapshots ? first time ? make a row on the bottom left up */
	int                 ax, ay, x, y, w, h;

	DesksGetAreaSize(&ax, &ay);
	w = ((48 * WinGetW(VROOT)) / WinGetH(VROOT)) * ax;
	h = 48 * ay;
	EwinResize(ewin, w, h, 0);	/* Does layout */
	x = 0;
	y = WinGetH(VROOT) - (DesksGetNumber() - p->dsk->num) * EoGetH(ewin);
	EwinMove(ewin, x, y, 0);
     }

   EwinShow(ewin);
}

typedef struct {
   Desk               *dsk;
   void                (*func) (Pager * p, void *prm);
   void               *prm;
} pager_foreach_data;

static void
_PagersForeachFunc(void *item, void *prm)
{
   Pager              *p = (Pager *) item;
   pager_foreach_data *data = (pager_foreach_data *) prm;

   if (data->dsk && data->dsk != p->dsk)
      return;
   data->func(p, data->prm);
}

static void
PagersForeach(Desk * dsk, void (*func) (Pager * p, void *prm), void *prm)
{
   pager_foreach_data  data;

   data.dsk = dsk;
   data.func = func;
   data.prm = prm;
   ecore_list_for_each(pager_list, _PagersForeachFunc, &data);
}

typedef struct {
   int                 why;
   int                 x1, y1, x2, y2;
} pager_update_data;

static void
_PagerUpdate(Pager * p, void *prm)
{
   pager_update_data  *pud = (pager_update_data *) prm;

   PagerUpdate(p, pud->why, pud->x1, pud->y1, pud->x2, pud->y2);
}

static void
PagersUpdate(Desk * dsk, int why, int x1, int y1, int x2, int y2)
{
   pager_update_data   pud;

   if (ecore_list_count(pager_list) <= 0)
      return;

   pud.why = why;
   pud.x1 = x1;
   pud.y1 = y1;
   pud.x2 = x2;
   pud.y2 = y2;
   PagersForeach(dsk, _PagerUpdate, &pud);
}

static void
PagerCheckUpdate(Pager * p, void *prm __UNUSED__)
{
   if (p->do_newbg)
     {
	PagerUpdateBg(p);
	p->do_update = 1;
     }

   if (p->do_update)
      doPagerUpdate(p);

   p->do_newbg = p->do_update = 0;
}

static int
_PagersUpdateTimeout(void *data __UNUSED__)
{
   Mode_pagers.timer_pending = 0;

   return 0;
}

static void
PagersCheckUpdate(void)
{
   static unsigned int tms_last = 0;
   unsigned int        tms;
   int                 dtms;

   if (!Mode_pagers.update_pending || !Conf_pagers.enable)
      return;

   if (Mode_pagers.update_pending == (1 << PAGER_UPD_EWIN_DAMAGE))
     {
	tms = GetTimeMs();
	dtms = (Conf_pagers.scanspeed > 0) ? 1000 / Conf_pagers.scanspeed : 100;
	if ((int)(tms - tms_last) < dtms)
	  {
	     /* The purpose of this timer is to trigger the idler */
	     if (Mode_pagers.timer_pending)
		return;
	     TIMER_ADD_NP(dtms, _PagersUpdateTimeout, NULL);
	     Mode_pagers.timer_pending = 1;
	     return;
	  }
	tms_last = tms;
     }

   PagersForeach(NULL, PagerCheckUpdate, NULL);

   Mode_pagers.update_pending = 0;
}

static void
_PagersIdler(void *data __UNUSED__)
{
   PagersCheckUpdate();
}

static void
PagerEwinUpdateFromPager(Pager * p, EWin * ewin)
{
   int                 x, y, w, h;
   static GC           gc = 0;

   if (!EoIsShown(ewin) || !EwinIsOnScreen(ewin))
      return;

   Dprintf("PagerEwinUpdateFromPager %d\n", p->dsk->num);

   x = EwinGetVX(ewin);
   y = EwinGetVY(ewin);
   w = EoGetW(ewin);
   h = EoGetH(ewin);
   x = (x * p->dw) / WinGetW(VROOT);
   y = (y * p->dh) / WinGetH(VROOT);
   w = (w * p->dw) / WinGetW(VROOT);
   h = (h * p->dh) / WinGetH(VROOT);
   if (w <= 0)
      w = 1;
   if (h <= 0)
      h = 1;

   if (!gc)
      gc = EXCreateGC(WinGetPmap(p->win), 0, NULL);

   PmapMaskInit(&ewin->mini_pmm, p->win, w, h);
   if (!ewin->mini_pmm.pmap)
      return;

   XCopyArea(disp, WinGetPmap(p->win), ewin->mini_pmm.pmap, gc, x, y, w, h, 0,
	     0);

#if 0				/* FIXME - Remove? */
   if (hiwin && ewin == hiwin->ewin)
      PagerHiwinUpdate(hiwin, p, ewin);
#endif
}

static void
PagerUpdateEwinsFromPager(Pager * p)
{
   int                 i, num;
   EWin               *const *lst;

   lst = EwinListGetForDesk(&num, p->dsk);
   for (i = 0; i < num; i++)
      PagerEwinUpdateFromPager(p, lst[i]);
}

static void
PagersUpdateEwin(EWin * ewin, int why)
{
   Desk               *dsk;

   if (!Conf_pagers.enable)
      return;

   switch (why)
     {
     case PAGER_UPD_EWIN_GEOM:
	if (!EoIsShown(ewin))
	   return;
	break;

     case PAGER_UPD_EWIN_GONE:
	if (ewin == HiwinGetEwin(hiwin, 0))
	   PagerHiwinHide();
	break;

     case PAGER_UPD_EWIN_DAMAGE:
	if (ewin->type == EWIN_TYPE_PAGER)
	   return;
	if (PagersGetMode() != PAGER_MODE_LIVE)
	   return;
	break;
     }

   dsk = (EoIsFloating(ewin)) ? DesksGetCurrent() : EoGetDesk(ewin);
   PagersUpdate(dsk, why, EwinGetVX(ewin), EwinGetVY(ewin),
		EwinGetVX2(ewin), EwinGetVY2(ewin));
}

static EWin        *
EwinInPagerAt(Pager * p, int px, int py)
{
   EWin               *const *lst, *ewin;
   int                 i, num, x, y, w, h;

   if (!Conf_pagers.enable)
      return NULL;

   lst = EwinListGetForDesk(&num, p->dsk);
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (!EoIsShown(ewin))
	   continue;

	x = (EwinGetVX(ewin) * p->dw) / WinGetW(VROOT);
	y = (EwinGetVY(ewin) * p->dh) / WinGetH(VROOT);
	w = (EoGetW(ewin) * p->dw) / WinGetW(VROOT);
	h = (EoGetH(ewin) * p->dh) / WinGetH(VROOT);

	if (px >= x && py >= y && px < (x + w) && py < (y + h))
	   return ewin;
     }

   return NULL;
}

static void
PagerMenuShow(Pager * p, int x, int y)
{
   Menu               *m;
   MenuItem           *mi;
   EWin               *ewin;
   char                s[1024];

   if (!Conf_pagers.enable)
      return;

   ewin = EwinInPagerAt(p, x, y);
   if (ewin)
     {
	m = MenuCreate("__pg_win", _("Window Options"), NULL, NULL);
	if (!m)
	   return;

	MenuSetTransient(m);	/* Destroy when hidden */

	Esnprintf(s, sizeof(s), "wop %#lx ic", EwinGetClientXwin(ewin));
	mi = MenuItemCreate(_("Iconify"), NULL, s, NULL);
	MenuAddItem(m, mi);

	Esnprintf(s, sizeof(s), "wop %#lx close", EwinGetClientXwin(ewin));
	mi = MenuItemCreate(_("Close"), NULL, s, NULL);
	MenuAddItem(m, mi);

	Esnprintf(s, sizeof(s), "wop %#lx kill", EwinGetClientXwin(ewin));
	mi = MenuItemCreate(_("Annihilate"), NULL, s, NULL);
	MenuAddItem(m, mi);

	Esnprintf(s, sizeof(s), "wop %#lx st", EwinGetClientXwin(ewin));
	mi = MenuItemCreate(_("Stick / Unstick"), NULL, s, NULL);
	MenuAddItem(m, mi);

	EFunc(NULL, "menus show __pg_win");
	return;
     }

   m = MenuCreate("__pg", _("Desktop Options"), NULL, NULL);
   if (!m)
      return;

   MenuSetTransient(m);		/* Destroy when hidden */

   mi = MenuItemCreate(_("Pager Settings..."), NULL, "cfg pagers", NULL);
   MenuAddItem(m, mi);

   if (PagersGetMode() != PAGER_MODE_SIMPLE)
     {
	mi = MenuItemCreate(_("Snapshotting Off"), NULL, "pg mode simp", NULL);
	MenuAddItem(m, mi);

	if (Conf_pagers.hiq)
	   mi = MenuItemCreate(_("High Quality Off"), NULL, "pg hiq off", NULL);
	else
	   mi = MenuItemCreate(_("High Quality On"), NULL, "pg hiq on", NULL);
	MenuAddItem(m, mi);
     }
   else
     {
	mi = MenuItemCreate(_("Snapshotting On"), NULL, "pg mode live", NULL);
	MenuAddItem(m, mi);
     }
   if (Conf_pagers.zoom)
      mi = MenuItemCreate(_("Zoom Off"), NULL, "pg zoom off", NULL);
   else
      mi = MenuItemCreate(_("Zoom On"), NULL, "pg zoom on", NULL);
   MenuAddItem(m, mi);

   EFunc(NULL, "menus show __pg");
}

static void
PagerClose(Pager * p)
{
   EwinHide(p->ewin);
}

static void
_PagerUpdateSel(Pager * p, void *prm __UNUSED__)
{
   int                 cx, cy;
   ImageClass         *ic;

   if (p->dsk != DesksGetCurrent())
      EUnmapWindow(p->sel_win);
   else
     {
	DeskGetArea(p->dsk, &cx, &cy);
	EMoveWindow(p->sel_win, cx * p->dw, cy * p->dh);
	EMapWindow(p->sel_win);
	ic = ImageclassFind("PAGER_SEL", 0);
	if (ic)
	   ImageclassApply(ic, p->sel_win, 0, 0, STATE_NORMAL, ST_PAGER);
     }
}

static void
UpdatePagerSel(void)
{
   if (!Conf_pagers.enable)
      return;

   PagersForeach(NULL, _PagerUpdateSel, NULL);
}

static void
PagerShowTt(EWin * ewin)
{
   static EWin        *tt_ewin = NULL;
   ToolTip            *tt;

   Dprintf("PagerShowTt %s\n", (ewin) ? EwinGetTitle(ewin) : NULL);

   if (!Conf_pagers.title || (ewin == tt_ewin))
      return;

   if (MenusActive())		/* Don't show Tooltip when menu is up */
      return;

   tt = TooltipFind("PAGER");
   if (tt)
     {
	if (ewin)
	   TooltipShow(tt, EwinGetTitle(ewin), NULL, Mode.events.cx,
		       Mode.events.cy);
	else
	   TooltipHide(tt);
     }

   tt_ewin = ewin;
}

static void
PagerHiwinInit(Pager * p, EWin * ewin)
{
   Hiwin              *phi = hiwin;
   int                 wx, wy, ww, wh, px, py;

   if (!phi)
     {
	phi = HiwinCreate();
	if (!phi)
	   return;
	hiwin = phi;
     }

   wx = (EwinGetVX(ewin) * p->dw) / WinGetW(VROOT);
   wy = (EwinGetVY(ewin) * p->dh) / WinGetH(VROOT);
   ww = (EoGetW(ewin) * p->dw) / WinGetW(VROOT);
   wh = (EoGetH(ewin) * p->dh) / WinGetH(VROOT);
   ETranslateCoordinates(p->win, VROOT, 0, 0, &px, &py, NULL);

   HiwinInit(phi, ewin);
   HiwinSetGeom(phi, px + wx, py + wy, ww, wh);
   HiwinSetCallback(phi, PagerHiwinEvent, p);
}

static void
PagerHiwinHide(void)
{
   Dprintf("PagerHiwinHide\n");
   HiwinHide(hiwin);
   PagerShowTt(NULL);
}

static void
PagerHiwinShow(Pager * p, EWin * ewin, int zoom, int confine)
{
   Hiwin              *phi = hiwin;

   if (MenusActive())		/* Don't show HiWin when menu is up */
      return;

   if (!phi || ewin)
     {
	PagerHiwinInit(p, ewin);
	phi = hiwin;
	if (!phi)
	   return;
     }

   HiwinShow(phi, ewin, zoom, confine);
}

static void
PagerZoomChange(Pager * p, int delta)
{
   Dprintf("PagerZoomChange delta=%d\n", delta);

   if (delta == 0)
      return;

   if (delta > 0)
     {
	if (!ECompMgrIsActive() && Mode_pagers.zoom >= 2)
	   return;
	if (Mode_pagers.zoom >= 8)
	   return;
	Mode_pagers.zoom++;
     }
   else
     {
	if (Mode_pagers.zoom <= 2)
	   return;
	Mode_pagers.zoom--;
     }
   PagerHiwinShow(p, NULL, Mode_pagers.zoom, 0);
}

static void
PagerHandleMotion(Pager * p, int x, int y)
{
   EWin               *ewin;

   if (!Conf_pagers.enable)
      return;

   if (Mode.events.on_screen && x >= 0 && x < p->w && y >= 0 && y < p->h)
      ewin = EwinInPagerAt(p, x, y);
   else
      ewin = NULL;

   if (!Conf_pagers.zoom)
     {
	PagerShowTt(ewin);
	return;
     }

   if (!ewin || EoGetLayer(ewin) <= 0)
     {
	PagerHiwinHide();
     }
   else if (!hiwin || ewin != HiwinGetEwin(hiwin, 0))
     {
	if (Mode_pagers.zoom < 2)
	   Mode_pagers.zoom = 2;
	PagerHiwinShow(p, ewin, Mode_pagers.zoom, 0);
     }
   if (Mode_pagers.zoom <= 2)
      PagerShowTt(ewin);
}

static void
NewPagerForDesktop(Desk * dsk)
{
   Pager              *p;

   p = PagerCreate();
   if (!p)
      return;

   p->dsk = dsk;
   PagerShow(p);
}

static void
_PagerUpdateBackground(Pager * p, void *prm __UNUSED__)
{
   p->do_newbg = 1;
}

static void
PagersUpdateBackground(Desk * dsk)
{
   PagersForeach(dsk, _PagerUpdateBackground, NULL);

   Mode_pagers.update_pending = 1;
}

static void
_EwinsMiniFree(void)
{
   EWin               *const *lst;
   int                 i, num;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
      PmapMaskFree(&(lst[i]->mini_pmm));
}

static void
PagerSetHiQ(char onoff)
{
   Conf_pagers.hiq = onoff;

   _EwinsMiniFree();

   PagersUpdateBackground(NULL);

   autosave();
}

static void
_PagerSetSnap(Pager * p, void *prm __UNUSED__)
{
   PagerScanTrig(p);
}

static void
PagersSetMode(int mode)
{
   if (mode == Conf_pagers.mode)
      return;
   Conf_pagers.mode = mode;

   _EwinsMiniFree();

   PagersUpdateBackground(NULL);

   if (Conf_pagers.mode == PAGER_MODE_SNAP && Conf_pagers.scanspeed > 0)
      PagersForeach(DesksGetCurrent(), _PagerSetSnap, NULL);

   autosave();
}

/*
 * Pager event handlers
 */

static int         *gwin_px, *gwin_py;

static void
PagerEwinGroupSet(void)
{
   int                 i, num;
   EWin               *ewin, **gwins;

   ewin = HiwinGetEwin(hiwin, 0);
   gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
				      Mode.nogroup, &num);
   if (!gwins)
      return;

   gwin_px = EMALLOC(int, num);
   gwin_py = EMALLOC(int, num);

   for (i = 0; i < num; i++)
     {
	gwin_px[i] = EoGetX(gwins[i]);
	gwin_py[i] = EoGetY(gwins[i]);
     }

   Efree(gwins);
}

static void
PagerEwinGroupUnset(void)
{
   _EFREE(gwin_px);
   _EFREE(gwin_py);
}

static void
PagerEventUnmap(Pager * p __UNUSED__)
{
   PagerHiwinHide();
}

static void
EwinGroupMove(EWin * ewin, Desk * dsk, int x, int y)
{
   int                 i, num, dx, dy, newdesk;
   EWin              **gwins;

   if (!ewin)
      return;

   /* Move all group members */
   newdesk = dsk != EoGetDesk(ewin);
   dx = x - EoGetX(ewin);
   dy = y - EoGetY(ewin);
   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (gwins[i]->type == EWIN_TYPE_PAGER)
	   continue;

	if (newdesk)
	   EwinOpMoveToDesktopAt(gwins[i], OPSRC_USER, dsk,
				 EoGetX(gwins[i]) + dx, EoGetY(gwins[i]) + dy);
	else
	   EwinOpMove(gwins[i], OPSRC_USER, EoGetX(gwins[i]) + dx,
		      EoGetY(gwins[i]) + dy);
     }
   Efree(gwins);
}

static void
PagerEwinMove(Pager * p __UNUSED__, Pager * pd, EWin * ewin)
{
   int                 x, y, dx, dy, px, py;
   int                 cx, cy;
   Hiwin              *phi = hiwin;

   /* Delta in pager coords */
   dx = Mode.events.mx - Mode.events.px;
   dy = Mode.events.my - Mode.events.py;

   if (dx == 0 && dy == 0 && p == pd)
      return;

   /* Move mini window */
   HiwinGetXY(phi, &x, &y);
   HiwinMove(phi, x + dx, y + dy);

   /* Find real window position */
   ETranslateCoordinates(VROOT, pd->win, x, y, &px, &py, NULL);
   DeskGetArea(pd->dsk, &cx, &cy);
   x = (px * WinGetW(VROOT)) / pd->dw - cx * WinGetW(VROOT);
   y = (py * WinGetH(VROOT)) / pd->dh - cy * WinGetH(VROOT);

   /* Move all group members */
   EwinGroupMove(ewin, pd->dsk, x, y);
}

static void
PagerHandleMouseDown(Pager * p, int px, int py, int button)
{
   int                 in_pager;
   EWin               *ewin;

   Dprintf("%s m=%d d=%d x,y=%d,%d\n", __func__, Mode.mode, p->dsk->num,
	   px, py);

   in_pager = (px >= 0 && py >= 0 && px < p->w && py < p->h);
   if (!in_pager)
      return;

   if (button == Conf_pagers.menu_button)
     {
	PagerHiwinHide();
	PagerMenuShow(p, px, py);
     }
   else if (button == Conf_pagers.win_button)
     {
	ewin = EwinInPagerAt(p, px, py);
	if (ewin && ewin->type != EWIN_TYPE_PAGER && !ewin->state.inhibit_move)
	  {
	     PagerHiwinShow(p, ewin, 1, !Mode.wm.window);
	     Mode.mode = MODE_PAGER_DRAG_PENDING;
	     EwinRaise(ewin);
	     PagerEwinGroupSet();
	  }
     }
}

static void
PagerHandleMouseUp(Pager * p, int px, int py, int button)
{
   int                 i, num, in_pager, in_vroot;
   EWin               *ewin, *ewin2, **gwins;
   int                 x, y;

   Dprintf("%s m=%d d=%d x,y=%d,%d\n", __func__, Mode.mode, p->dsk->num,
	   px, py);

   in_pager = (px >= 0 && py >= 0 && px < p->w && py < p->h);

   if (button == Conf_pagers.sel_button)
     {
	if (!in_pager)
	   return;
	DeskGoto(p->dsk);
	DeskCurrentGotoArea(px / p->dw, py / p->dh);
	return;
     }

   if (button != Conf_pagers.win_button)
      return;

   ewin = HiwinGetEwin(hiwin, 1);
   PagerHiwinHide();

   if (Mode.mode != MODE_PAGER_DRAG)
     {
	if (Mode.mode == MODE_PAGER_DRAG_PENDING)
	   Mode.mode = MODE_NONE;
	if (!in_pager)
	   goto done;
	DeskGoto(p->dsk);
	DeskCurrentGotoArea(px / p->dw, py / p->dh);
	if (ewin)
	   FocusToEWin(ewin, FOCUS_SET);
	goto done;
     }

   Mode.mode = MODE_NONE;

   if (!ewin)
      goto done;

   /* Finish pager drag */

   in_vroot = (Mode.events.cx >= 0 && Mode.events.cx < WinGetW(VROOT) &&
	       Mode.events.cy >= 0 && Mode.events.cy < WinGetH(VROOT));

   /* Find which pager or iconbox we are in (if any) */
   ewin2 = GetEwinPointerInClient();
   if ((ewin2) && (ewin2->type == EWIN_TYPE_PAGER))
     {
	PagerEwinMove(p, (Pager *) ewin2->data, ewin);
     }
   else if ((ewin2) && (ewin2->type == EWIN_TYPE_ICONBOX))
     {
	/* Pointer is in iconbox */

	/* Iconify after moving back to pre-drag position */
	gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE,
					   Mode.nogroup, &num);
	for (i = 0; i < num; i++)
	  {
	     if (gwins[i]->type != EWIN_TYPE_PAGER)
	       {
		  EwinMove(gwins[i], gwin_px[i], gwin_py[i], 0);
		  EwinIconify(gwins[i]);
	       }
	  }
	Efree(gwins);
     }
   else if (ewin2 && ewin2->props.vroot)
     {
	/* Dropping onto virtual root */
	EwinReparent(ewin, EwinGetClientWin(ewin2));
     }
   else if (!in_vroot)
     {
	/* Move back to real root */
	EwinReparent(ewin, RROOT);
     }
   else
     {
	/* Pointer is not in pager or iconbox */
	/* Move window(s) to pointer location */
	x = Mode.events.cx - EoGetW(ewin) / 2;
	y = Mode.events.cy - EoGetH(ewin) / 2;
	EwinGroupMove(ewin, DesksGetCurrent(), x, y);
     }

 done:
   /* unallocate the space that was holding the old positions of the */
   /* windows */
   PagerEwinGroupUnset();
}

static void
PagerEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Pager              *p = (Pager *) prm;

   Dprintf("PagerEvent ev=%d\n", ev->type);

   switch (ev->type)
     {
     case ButtonPress:
	PagerHandleMouseDown(p, ev->xbutton.x, ev->xbutton.y,
			     (int)ev->xbutton.button);
	break;
     case ButtonRelease:
	if (ev->xbutton.window != Mode.events.last_bpress)
	   break;
	PagerHandleMouseUp(p, ev->xbutton.x, ev->xbutton.y,
			   (int)ev->xbutton.button);
	break;

     case MotionNotify:
	PagerHandleMotion(p, ev->xmotion.x, ev->xmotion.y);
	break;

     case EnterNotify:
	break;
     case LeaveNotify:
	PagerShowTt(NULL);
	break;

     case UnmapNotify:
	PagerEventUnmap(p);
	break;

     case VisibilityNotify:
	if (ev->xvisibility.state != VisibilityFullyObscured)
	   PagerScanTrig(p);
	break;
     }
}

static void
PagerHiwinEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Pager              *p = (Pager *) prm;
   int                 px, py;
   EWin               *ewin;

   if (!p)
      return;

   Dprintf("PagerHiwinEvent ev=%d\n", ev->type);

   switch (ev->type)
     {
     case ButtonPress:
	switch (ev->xbutton.button)
	  {
	  case 4:
	     if (Mode.mode != MODE_NONE)
		break;
	     PagerZoomChange(p, 1);
	     break;
	  case 5:
	     if (Mode.mode != MODE_NONE)
		break;
	     PagerZoomChange(p, -1);
	     break;
	  default:
	     /* Translate x,y to pager window coordinates */
	     ETranslateCoordinates(RROOT, p->win,
				   ev->xbutton.x_root, ev->xbutton.y_root,
				   &px, &py, NULL);
	     PagerHandleMouseDown(p, px, py, (int)ev->xbutton.button);
	     break;
	  }
	break;

     case ButtonRelease:
	switch (ev->xbutton.button)
	  {
	  case 4:
	  case 5:
	     break;
	  default:
	     /* Translate x,y to pager window coordinates */
	     ETranslateCoordinates(RROOT, p->win,
				   ev->xbutton.x_root, ev->xbutton.y_root,
				   &px, &py, NULL);
	     PagerHandleMouseUp(p, px, py, (int)ev->xbutton.button);
	     break;
	  }
	break;

     case MotionNotify:
	switch (Mode.mode)
	  {
	  case MODE_NONE:
	     /* Translate x,y to pager window coordinates */
	     ETranslateCoordinates(RROOT, p->win,
				   ev->xbutton.x_root, ev->xbutton.y_root,
				   &px, &py, NULL);
	     PagerHandleMotion(p, px, py);
	     break;

	  case MODE_PAGER_DRAG_PENDING:
	  case MODE_PAGER_DRAG:
	     ewin = HiwinGetEwin(hiwin, 1);
	     if (!ewin || ewin->type == EWIN_TYPE_PAGER)
	       {
		  Mode.mode = MODE_NONE;
		  break;
	       }

	     Mode.mode = MODE_PAGER_DRAG;
	     PagerEwinMove(p, p, ewin);
	     break;
	  }
	break;

     case LeaveNotify:
	PagerShowTt(NULL);
	break;
     }
}

/*
 * Pagers handling
 */

static int
PagersForDesktopCount(Desk * dsk)
{
   Pager              *p;
   int                 num = 0;

   ECORE_LIST_FOR_EACH(pager_list, p)
   {
      if (p->dsk == dsk)
	 num++;
   }

   return num;
}

static void
_PagerClose(Pager * p, void *prm __UNUSED__)
{
   PagerClose(p);
}

static void
PagersForDesktopEnable(Desk * dsk)
{
   if (PagersForDesktopCount(dsk) <= 0)
      NewPagerForDesktop(dsk);
}

static void
PagersForDesktopDisable(Desk * dsk)
{
   PagersForeach(dsk, _PagerClose, NULL);
}

static void
PagersShow(int enable)
{
   unsigned int        i;

   if (enable && !Conf_pagers.enable)
     {
	Conf_pagers.enable = 1;
	for (i = 0; i < DesksGetNumber(); i++)
	   PagersForDesktopEnable(DeskGet(i));
	UpdatePagerSel();
	Mode_pagers.idler = IdlerAdd(_PagersIdler, NULL);
     }
   else if (!enable && Conf_pagers.enable)
     {
	for (i = 0; i < DesksGetNumber(); i++)
	   PagersForDesktopDisable(DeskGet(i));
	Conf_pagers.enable = 0;
	IdlerDel(Mode_pagers.idler);
	Mode_pagers.idler = NULL;
     }
}

static void
_PagerReconfigure(Pager * p, void *prm __UNUSED__)
{
   PagerReconfigure(p, 1);
}

static int
_PagersReconfigureTimeout(void *data __UNUSED__)
{
   PagersForeach(NULL, _PagerReconfigure, NULL);

   return 0;
}

static void
PagersReconfigure(void)
{
   if (!Conf_pagers.enable)
      return;

   TIMER_ADD_NP(500, _PagersReconfigureTimeout, NULL);
}

#if ENABLE_DIALOGS
/*
 * Configuration dialog
 */
typedef struct {
   char                show_pagers;
   char                pager_hiq;
   int                 pager_mode;
   char                pager_zoom;
   char                pager_title;
   char                pager_do_scan;
   int                 pager_scan_speed;
   int                 pager_sel_button;
   int                 pager_win_button;
   int                 pager_menu_button;
} PagerDlgData;

static void
CB_ConfigurePager(Dialog * d, int val, void *data __UNUSED__)
{
   PagerDlgData       *dd = DLG_DATA_GET(d, PagerDlgData);

   if (val >= 2)
      return;

   PagersShow(dd->show_pagers);
   if (Conf_pagers.hiq != dd->pager_hiq)
      PagerSetHiQ(dd->pager_hiq);
   Conf_pagers.zoom = dd->pager_zoom;
   Conf_pagers.title = dd->pager_title;
   Conf_pagers.sel_button = dd->pager_sel_button;
   Conf_pagers.win_button = dd->pager_win_button;
   Conf_pagers.menu_button = dd->pager_menu_button;
   if ((Conf_pagers.scanspeed != dd->pager_scan_speed)
       || ((!dd->pager_do_scan) && (Conf_pagers.scanspeed > 0))
       || ((dd->pager_do_scan) && (Conf_pagers.scanspeed == 0)))
     {
	if (dd->pager_do_scan)
	   Conf_pagers.scanspeed = dd->pager_scan_speed;
	else
	   Conf_pagers.scanspeed = 0;
     }
   PagersSetMode(dd->pager_mode);

   autosave();
}

static void
CB_PagerScanSlide(Dialog * d __UNUSED__, int val __UNUSED__, void *data)
{
   PagerDlgData       *dd = DLG_DATA_GET(d, PagerDlgData);
   DItem              *di = (DItem *) data;
   char                s[256];

   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     dd->pager_scan_speed, _("lines per second"));
   DialogItemSetText(di, s);
}

static void
_DlgFillPagers(Dialog * d, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *radio, *label;
   char                s[256];
   PagerDlgData       *dd;

   dd = DLG_DATA_SET(d, PagerDlgData);
   if (!dd)
      return;

   dd->show_pagers = Conf_pagers.enable;
   dd->pager_hiq = Conf_pagers.hiq;
   dd->pager_mode = Conf_pagers.mode;
   dd->pager_zoom = Conf_pagers.zoom;
   dd->pager_title = Conf_pagers.title;
   dd->pager_sel_button = Conf_pagers.sel_button;
   dd->pager_win_button = Conf_pagers.win_button;
   dd->pager_menu_button = Conf_pagers.menu_button;
   if (Conf_pagers.scanspeed == 0)
      dd->pager_do_scan = 0;
   else
      dd->pager_do_scan = 1;
   dd->pager_scan_speed = Conf_pagers.scanspeed;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Enable pager display"));
   DialogItemCheckButtonSetPtr(di, &dd->show_pagers);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Pager Mode:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Simple"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Make miniature snapshots of the screen"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

#if USE_COMPOSITE
   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Live Update"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);
#endif
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->pager_mode);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Smooth high quality snapshots in snapshot mode"));
   DialogItemCheckButtonSetPtr(di, &dd->pager_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Zoom in on pager windows when mouse is over them"));
   DialogItemCheckButtonSetPtr(di, &dd->pager_zoom);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di,
		     _("Pop up window title when mouse is over the window"));
   DialogItemCheckButtonSetPtr(di, &dd->pager_title);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Continuously scan screen to update pager"));
   DialogItemCheckButtonSetPtr(di, &dd->pager_do_scan);

   di = label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     dd->pager_scan_speed, _("lines per second"));
   DialogItemSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 1, 256);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetValPtr(di, &dd->pager_scan_speed);
   DialogItemSetCallback(di, CB_PagerScanSlide, 0, label);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Mouse button to select and drag windows:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->pager_win_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Mouse button to select desktops:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->pager_sel_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Mouse button to display pager menu:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &dd->pager_menu_button);
}

const DialogDef     DlgPagers = {
   "CONFIGURE_PAGER",
   N_("Pagers"),
   N_("Pager Settings"),
   SOUND_SETTINGS_PAGER,
   "pix/pager.png",
   N_("Enlightenment Desktop & Area\n" "Pager Settings Dialog"),
   _DlgFillPagers,
   DLG_OAC, CB_ConfigurePager,
};
#endif /* ENABLE_DIALOGS */

/*
 * Pagers Module
 */

static void
PagersSighan(int sig, void *prm)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	memset(&Mode_pagers, 0, sizeof(Mode_pagers));
	break;
     case ESIGNAL_CONFIGURE:
	break;
     case ESIGNAL_START:
	if (!Conf_pagers.enable)
	   break;
	Conf_pagers.enable = 0;
	PagersShow(1);
	break;

     case ESIGNAL_AREA_CONFIGURED:
	PagersReconfigure();
	break;
     case ESIGNAL_AREA_SWITCH_START:
	PagerHiwinHide();
	break;
     case ESIGNAL_AREA_SWITCH_DONE:
	PagersUpdate(DesksGetCurrent(), PAGER_UPD_EWIN_GEOM,
		     0, 0, 99999, 99999);
	UpdatePagerSel();
	break;

     case ESIGNAL_DESK_ADDED:
	if (Mode.wm.startup)
	   break;
	NewPagerForDesktop((Desk *) prm);
	break;
     case ESIGNAL_DESK_REMOVED:
	PagersForDesktopDisable((Desk *) prm);
	break;
     case ESIGNAL_DESK_SWITCH_START:
	PagerHiwinHide();
	break;
     case ESIGNAL_DESK_SWITCH_DONE:
	UpdatePagerSel();
	break;
     case ESIGNAL_DESK_RESIZE:
	PagersReconfigure();
	break;

     case ESIGNAL_BACKGROUND_CHANGE:
	PagersUpdateBackground((Desk *) prm);
	break;

     case ESIGNAL_EWIN_UNMAP:
	PagersUpdateEwin((EWin *) prm, PAGER_UPD_EWIN_GONE);
	break;
     case ESIGNAL_EWIN_CHANGE:
	PagersUpdateEwin((EWin *) prm, PAGER_UPD_EWIN_GEOM);
	break;
     case ESIGNAL_EWIN_DAMAGE:
	PagersUpdateEwin((EWin *) prm, PAGER_UPD_EWIN_DAMAGE);
	break;
     }
}

static void
IPC_Pager(const char *params)
{
   const char         *p = params;
   char                prm1[128];
   int                 len, desk;
   Desk               *dsk;

   if (!p)
      return;

   prm1[0] = '\0';
   len = 0;
   sscanf(p, "%100s %n", prm1, &len);
   p += len;

   if (!strcmp(prm1, "on"))
     {
	PagersShow(1);
     }
   else if (!strcmp(prm1, "off"))
     {
	PagersShow(0);
     }
   else if (!strcmp(prm1, "desk"))
     {
	desk = -1;
	prm1[0] = '\0';
	sscanf(p, "%d %100s", &desk, prm1);
	dsk = DeskGet(desk);

	if (!dsk)
	  {
	     ;
	  }
	else if (!strcmp(prm1, "on"))
	  {
	     PagersForDesktopEnable(dsk);
	  }
	else if (!strcmp(prm1, "new"))
	  {
	     NewPagerForDesktop(dsk);
	  }
	else if (!strcmp(prm1, "off"))
	  {
	     PagersForDesktopDisable(dsk);
	  }
     }
   else if (!strcmp(prm1, "hiq"))
     {
	if (!strcmp(p, "on"))
	   PagerSetHiQ(1);
	else if (!strcmp(p, "off"))
	   PagerSetHiQ(0);
     }
   else if (!strcmp(prm1, "mode"))
     {
	if (!strncmp(p, "si", 2))
	   PagersSetMode(PAGER_MODE_SIMPLE);
	else if (!strncmp(p, "sn", 2))
	   PagersSetMode(PAGER_MODE_SNAP);
	else if (!strncmp(p, "li", 2))
	   PagersSetMode(PAGER_MODE_LIVE);
     }
   else if (!strcmp(prm1, "zoom"))
     {
	if (!strcmp(p, "on"))
	   Conf_pagers.zoom = 1;
	else if (!strcmp(p, "off"))
	   Conf_pagers.zoom = 0;
     }
}

static const IpcItem PagersIpcArray[] = {
   {
    IPC_Pager,
    "pager", "pg",
    "Toggle the status of the Pager and various pager settings",
    "use \"pager <on/off>\" to set the current mode\nuse \"pager ?\" "
    "to get the current mode\n"
    "  pager <#> <on/off/?>   Toggle or test any desktop's pager\n"
    "  pager hiq <on/off>     Toggle high quality pager\n"
    "  pager scanrate <#>     Toggle number of line updates per second\n"
    "  pager mode <mode>      Set pager mode (simple/snapshot/live)\n"
    "  pager title <on/off>   Toggle title display in the pager\n"
    "  pager zoom <on/off>    Toggle zooming in the pager\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(PagersIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem PagersCfgItems[] = {
   CFG_ITEM_BOOL(Conf_pagers, enable, 1),
   CFG_ITEM_BOOL(Conf_pagers, zoom, 1),
   CFG_ITEM_BOOL(Conf_pagers, title, 1),
   CFG_ITEM_BOOL(Conf_pagers, hiq, 1),
   CFG_ITEM_INT(Conf_pagers, mode, PAGER_MODE_LIVE),
   CFG_ITEM_INT(Conf_pagers, scanspeed, 10),
   CFG_ITEM_INT(Conf_pagers, sel_button, 2),
   CFG_ITEM_INT(Conf_pagers, win_button, 1),
   CFG_ITEM_INT(Conf_pagers, menu_button, 3),
};
#define N_CFG_ITEMS (sizeof(PagersCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModPagers;

const EModule       ModPagers = {
   "pagers", "pg",
   PagersSighan,
   {N_IPC_FUNCS, PagersIpcArray},
   {N_CFG_ITEMS, PagersCfgItems}
};
