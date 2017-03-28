/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2007-2012 Kim Woelders
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
#include "piximg.h"
#include "xwin.h"

#if ENABLE_OLDMOVRES
#define MR_ENABLE_STIPPLED         1	/* Enable shaded/semi-solid modes */
#define MR_ENABLE_TRANSLUCENT      1	/* Enable translucent mode */
#define MR_MODES_MOVE           0x7f	/* MR_OPAQUE through MR_TECH_OPAQUE */
#define MR_MODES_RESIZE         0x5f	/* MR_OPAQUE through MR_SEMI_SOLID and MR_TECH_OPAQUE */
#else
#define MR_MODES_MOVE           0x47	/* MR_OPAQUE through MR_BOX and MR_TECH_OPAQUE */
#define MR_MODES_RESIZE         0x47	/* MR_OPAQUE through MR_BOX and MR_TECH_OPAQUE */
#endif

#if MR_ENABLE_STIPPLED
#if 0
#include <X11/bitmaps/gray>
#include <X11/bitmaps/gray3>
#else
/* Include contents of X11/bitmaps/gray+gray3.
 * (avoid build failure if x11 bitmap package isn't installed) */

#define gray_width 2
#define gray_height 2
static const char   gray_bits[] = { 0x01, 0x02 };

#define gray3_width 4
#define gray3_height 4
static const char   gray3_bits[] = { 0x01, 0x00, 0x04, 0x00 };
#endif

static Pixmap       b2 = None;	/* Used in modes 3,4 */
static Pixmap       b3 = None;	/* Used in mode 3 */
#endif /* MR_ENABLE_STIPPLED */

static Font         font = None;	/* Used in mode 1 (technical) */

static void
draw_h_arrow(Drawable dr, GC gc, int x1, int x2, int y1)
{
   char                str[32];

   if (x2 - x1 >= 12)
     {
	XDrawLine(disp, dr, gc, x1, y1, x1 + 6, y1 - 3);
	XDrawLine(disp, dr, gc, x1, y1, x1 + 6, y1 + 3);
	XDrawLine(disp, dr, gc, x2, y1, x2 - 6, y1 - 3);
	XDrawLine(disp, dr, gc, x2, y1, x2 - 6, y1 + 3);
     }
   if (x2 >= x1)
     {
	XDrawLine(disp, dr, gc, x1, y1, x2, y1);
	Esnprintf(str, sizeof(str), "%i", x2 - x1 + 1);
	XDrawString(disp, dr, gc, (x1 + x2) / 2, y1 - 10, str, strlen(str));
     }
}

static void
draw_v_arrow(Drawable dr, GC gc, int y1, int y2, int x1)
{
   char                str[32];

   if (y2 - y1 >= 12)
     {
	XDrawLine(disp, dr, gc, x1, y1, x1 + 3, y1 + 6);
	XDrawLine(disp, dr, gc, x1, y1, x1 - 3, y1 + 6);
	XDrawLine(disp, dr, gc, x1, y2, x1 + 3, y2 - 6);
	XDrawLine(disp, dr, gc, x1, y2, x1 - 3, y2 - 6);
     }
   if (y2 >= y1)
     {
	XDrawLine(disp, dr, gc, x1, y1, x1, y2);
	Esnprintf(str, sizeof(str), "%i", y2 - y1 + 1);
	XDrawString(disp, dr, gc, x1 + 10, (y1 + y2) / 2, str, strlen(str));
     }
}

void
do_draw_technical(Drawable dr, GC gc,
		  int a, int b, int c, int d, int bl, int br, int bt, int bb)
{
   if (!font)
      font = XLoadFont(disp, "-*-helvetica-medium-r-*-*-10-*-*-*-*-*-*-*");
   XSetFont(disp, gc, font);

   if (c < 3)
      c = 3;
   if (d < 3)
      d = 3;

   draw_h_arrow(dr, gc, a + bl, a + bl + c - 1, b + bt + d - 16);
   draw_h_arrow(dr, gc, 0, a - 1, b + bt + (d / 2));
   draw_h_arrow(dr, gc, a + c + bl + br, WinGetW(VROOT) - 1, b + bt + (d / 2));
   draw_v_arrow(dr, gc, b + bt, b + bt + d - 1, a + bl + 16);
   draw_v_arrow(dr, gc, 0, b - 1, a + bl + (c / 2));
   draw_v_arrow(dr, gc, b + d + bt + bb, WinGetH(VROOT) - 1, a + bl + (c / 2));

   XDrawLine(disp, dr, gc, a, 0, a, WinGetH(VROOT));
   XDrawLine(disp, dr, gc, a + c + bl + br - 1, 0,
	     a + c + bl + br - 1, WinGetH(VROOT));
   XDrawLine(disp, dr, gc, 0, b, WinGetW(VROOT), b);
   XDrawLine(disp, dr, gc, 0, b + d + bt + bb - 1,
	     WinGetW(VROOT), b + d + bt + bb - 1);

   XDrawRectangle(disp, dr, gc, a + bl + 1, b + bt + 1, c - 3, d - 3);
}

static void
do_draw_boxy(Drawable dr, GC gc,
	     int a, int b, int c, int d, int bl, int br, int bt, int bb)
{
   if (c < 3)
      c = 3;
   if (d < 3)
      d = 3;
   XDrawRectangle(disp, dr, gc, a, b, c + bl + br - 1, d + bt + bb - 1);
   XDrawRectangle(disp, dr, gc, a + bl + 1, b + bt + 1, c - 3, d - 3);
}

#if MR_ENABLE_STIPPLED
static void
do_draw_shaded(Drawable dr, GC gc,
	       int a, int b, int c, int d, int bl, int br, int bt, int bb)
{
   XSetFillStyle(disp, gc, FillStippled);
   XSetStipple(disp, gc, b2);

   if ((c + bl + br > 0) && (bt > 0))
      XFillRectangle(disp, dr, gc, a, b, c + bl + br, bt);
   if ((c + bl + br > 0) && (bb > 0))
      XFillRectangle(disp, dr, gc, a, b + d + bt, c + bl + br, bb);
   if ((d > 0) && (bl > 0))
      XFillRectangle(disp, dr, gc, a, b + bt, bl, d);
   if ((d > 0) && (br > 0))
      XFillRectangle(disp, dr, gc, a + c + bl, b + bt, br, d);
   XSetStipple(disp, gc, b3);
   if ((c > 0) && (d > 0))
      XFillRectangle(disp, dr, gc, a + bl + 1, b + bt + 1, c - 3, d - 3);
}

static void
do_draw_semi_solid(Drawable dr, GC gc,
		   int a, int b, int c, int d, int bl, int br, int bt, int bb)
{
   XSetFillStyle(disp, gc, FillStippled);
   XSetStipple(disp, gc, b2);
   XFillRectangle(disp, dr, gc, a, b, c + bl + br, d + bt + bb);
}
#endif /* MR_ENABLE_STIPPLED */

typedef struct {
   EWin               *ewin;
   Window              root;
   GC                  gc;
   int                 xo, yo, wo, ho;
   int                 bl, br, bt, bb;
#if MR_ENABLE_TRANSLUCENT
   PixImg             *root_pi;
   PixImg             *ewin_pi;
   PixImg             *draw_pi;
#endif
} ShapeData;

static void
_ShapeDrawNograb_tech_box(ShapeData * psd, int md, int firstlast,
			  int xn, int yn, int wn, int hn, int seqno)
{
   static ShapeWin    *shape_win = NULL;

   if (firstlast == 0 && !shape_win)
      shape_win = ShapewinCreate(md);
   if (!shape_win)
      return;

   ShapewinShapeSet(shape_win, md, xn, yn, wn, hn, psd->bl, psd->br, psd->bt,
		    psd->bb, seqno);
   EoMap(shape_win, 0);

   CoordsShow(psd->ewin);

   if (firstlast == 2)
     {
	ShapewinDestroy(shape_win);
	shape_win = NULL;
     }
}

typedef void        (DrawFunc) (Drawable dr, GC gc, int a, int b, int c, int d,
				int bl, int br, int bt, int bb);

static DrawFunc    *const draw_functions[] = {
   do_draw_technical, do_draw_boxy,
#if MR_ENABLE_STIPPLED
   do_draw_shaded, do_draw_semi_solid,
#else
   NULL, NULL,
#endif /* MR_ENABLE_STIPPLED */
   NULL, do_draw_technical,
};

static void
_ShapeDrawNontranslucent(ShapeData * psd, int md, int firstlast,
			 int xn, int yn, int wn, int hn)
{
   DrawFunc           *drf;

   if (firstlast == 0)
     {
	XGCValues           gcv;

	gcv.function = GXxor;
	gcv.foreground = Dpy.pixel_white;
	if (gcv.foreground == 0)
	   gcv.foreground = Dpy.pixel_black;
	gcv.subwindow_mode = IncludeInferiors;
	psd->gc = EXCreateGC(psd->root,
			     GCFunction | GCForeground | GCSubwindowMode, &gcv);
#if MR_ENABLE_STIPPLED
	if (md == MR_SHADED || md == MR_SEMI_SOLID)
	  {
	     if (!b2)
		b2 = XCreateBitmapFromData(disp, psd->root, gray_bits,
					   gray_width, gray_height);
	     if (!b3)
		b3 = XCreateBitmapFromData(disp, psd->root, gray3_bits,
					   gray3_width, gray3_height);
	  }
#endif /* MR_ENABLE_STIPPLED */
     }

   drf = draw_functions[md - 1];

   if (firstlast > 0)
      drf(psd->root, psd->gc, psd->xo, psd->yo, psd->wo, psd->ho,
	  psd->bl, psd->br, psd->bt, psd->bb);

   CoordsShow(psd->ewin);

   if (firstlast < 2)
      drf(psd->root, psd->gc, xn, yn, wn, hn,
	  psd->bl, psd->br, psd->bt, psd->bb);

   if (firstlast == 2)
     {
	EXFreeGC(psd->gc);
	psd->gc = NULL;
     }
}

#if MR_ENABLE_TRANSLUCENT
static void
_ShapeDrawTranslucent(ShapeData * psd, int md __UNUSED__, int firstlast,
		      int xn, int yn, int wn, int hn)
{
   XGCValues           gcv;
   int                 dx, dy, adx, ady;
   int                 xo, yo;

   xo = psd->xo;
   yo = psd->yo;

   /* Using frame window size here */
   wn = EoGetW(psd->ewin);
   hn = EoGetH(psd->ewin);

   switch (firstlast)
     {
     default:
	break;
     case 0:
	gcv.subwindow_mode = IncludeInferiors;
	psd->gc = EXCreateGC(psd->root, GCSubwindowMode, &gcv);

	psd->root_pi =
	   PixImgCreate(NULL, psd->gc, WinGetW(VROOT), WinGetH(VROOT));
	psd->ewin_pi = PixImgCreate(NULL, psd->gc, wn, hn);
	psd->draw_pi = PixImgCreate(VROOT, psd->gc, wn, hn);
	if ((!psd->root_pi) || (!psd->ewin_pi) || (!psd->draw_pi))
	  {
	     /* Trouble - Fall back to opaque mode */
	     Conf.movres.mode_move = MR_OPAQUE;
	     goto do_cleanup;
	  }

	if (EoGetWin(psd->ewin)->num_rect > 0)
	  {
	     Pixmap              mask;

	     mask = EWindowGetShapePixmapInverted(EoGetWin(psd->ewin));
	     PixImgSetMask(psd->draw_pi, mask, 0, 0);
	  }

	PixImgFill(psd->root_pi, psd->root, 0, 0);
	PixImgFill(psd->ewin_pi, psd->root, xn, yn);

	PixImgBlend(psd->root_pi, psd->ewin_pi, psd->draw_pi, psd->root,
		    xn, yn, wn, hn);
	break;

     case 1:
	dx = xn - xo;
	dy = yn - yo;
	if (dx < 0)
	   adx = -dx;
	else
	   adx = dx;
	if (dy < 0)
	   ady = -dy;
	else
	   ady = dy;
	if ((adx <= wn) && (ady <= hn))
	  {
	     PixImgBlend(psd->root_pi, psd->ewin_pi, psd->draw_pi, psd->root,
			 xn, yn, wn, hn);
	     if (dx > 0)
		PixImgPaste11(psd->root_pi, psd->draw_pi, xo, yo, dx, hn);
	     else if (dx < 0)
		PixImgPaste11(psd->root_pi, psd->draw_pi, xo + wn + dx,
			      yo, -dx, hn);
	     if (dy > 0)
		PixImgPaste11(psd->root_pi, psd->draw_pi, xo, yo, wn, dy);
	     else if (dy < 0)
		PixImgPaste11(psd->root_pi, psd->draw_pi, xo,
			      yo + hn + dy, wn, -dy);
	  }
	else
	  {
	     PixImgPaste11(psd->root_pi, psd->draw_pi, xo, yo, wn, hn);
	     PixImgBlend(psd->root_pi, psd->ewin_pi, psd->draw_pi, psd->root,
			 xn, yn, wn, hn);
	  }
	if (EoGetWin(psd->ewin)->num_rect > 0)
	  {
	     PixImgSetMask(psd->draw_pi, 1, xn, yn);
	     PixImgPaste11(psd->root_pi, psd->draw_pi, xn, yn, wn, hn);
	     PixImgSetMask(psd->draw_pi, 0, 0, 0);
	  }
	break;

     case 2:
	PixImgPaste11(psd->root_pi, psd->draw_pi, xo, yo, wn, hn);
      do_cleanup:
	PixImgDestroy(psd->root_pi);
	PixImgDestroy(psd->ewin_pi);
	PixImgDestroy(psd->draw_pi);
	psd->root_pi = NULL;
	psd->ewin_pi = NULL;
	psd->draw_pi = NULL;
	EXFreeGC(psd->gc);
	psd->gc = NULL;
	break;

     case 3:
	PixImgPaste11(psd->root_pi, psd->draw_pi, xo, yo, wn, hn);
	PixImgDestroy(psd->root_pi);
	psd->root_pi = NULL;
	break;

     case 4:
	psd->root_pi =
	   PixImgCreate(NULL, psd->gc, WinGetW(VROOT), WinGetH(VROOT));
	PixImgFill(psd->root_pi, psd->root, 0, 0);
	PixImgBlend(psd->root_pi, psd->ewin_pi, psd->draw_pi, psd->root,
		    xn, yn, wn, hn);
	break;
     }
}
#endif /* MR_ENABLE_TRANSLUCENT */

void
DrawEwinShape(EWin * ewin, int md, int x, int y, int w, int h,
	      int firstlast, int seqno)
{
   static ShapeData    sd, *psd = &sd;
   Window              root = WinGetXwin(VROOT);
   int                 dx, dy;

   /* Quit if no change */
   if (firstlast == 1 &&
       (x == ewin->shape_x && y == ewin->shape_y &&
	(ewin->state.shaded || (w == ewin->shape_w && h == ewin->shape_h))))
      return;

   if ((md == MR_OPAQUE) || (md == MR_TECH_OPAQUE))
     {
	EwinOpMoveResize(ewin, OPSRC_USER, x, y, w, h);
	EwinShapeSet(ewin);
	CoordsShow(ewin);
	if (md == MR_OPAQUE)
	   goto done;
     }

   if (firstlast == 0)
     {
	EwinShapeSet(ewin);

	psd->ewin = ewin;
	psd->root = root;
     }

   dx = EoGetX(EoGetDesk(ewin));
   dy = EoGetY(EoGetDesk(ewin));
   ewin->shape_x = x;
   ewin->shape_y = y;
   x += dx;
   y += dy;

   if (!ewin->state.shaded)
     {
	ewin->shape_w = w;
	ewin->shape_h = h;
     }
   else
     {
	w = ewin->shape_w;
	h = ewin->shape_h;
     }

   EwinBorderGetSize(ewin, &psd->bl, &psd->br, &psd->bt, &psd->bb);

   if (((md <= MR_BOX) || (md == MR_TECH_OPAQUE)) &&
       Conf.movres.avoid_server_grab)
     {
	_ShapeDrawNograb_tech_box(psd, md, firstlast, x, y, w, h, seqno);
	goto done;
     }

   switch (md)
     {
     case MR_TECHNICAL:
     case MR_TECH_OPAQUE:
     case MR_BOX:
#if MR_ENABLE_STIPPLED
     case MR_SHADED:
     case MR_SEMI_SOLID:
#endif
	_ShapeDrawNontranslucent(psd, md, firstlast, x, y, w, h);
	break;
#if MR_ENABLE_TRANSLUCENT
     case MR_TRANSLUCENT:
	_ShapeDrawTranslucent(psd, md, firstlast, x, y, w, h);
	CoordsShow(ewin);
	break;
#endif
     default:
	/* Fall back to opaque mode */
	Conf.movres.mode_move = MR_OPAQUE;
	break;
     }

   psd->xo = x;
   psd->yo = y;
   psd->wo = w;
   psd->ho = h;

 done:
   if (firstlast == 0 || firstlast == 2 || firstlast == 4)
     {
	ewin->req_x = ewin->shape_x;
	ewin->req_y = ewin->shape_y;
	if (firstlast == 2)
	   CoordsHide();
     }
}

int
DrawEwinShapeNeedsGrab(int mode)
{
   if (mode == MR_OPAQUE)
      return 0;
   if ((mode <= MR_BOX) || (mode == MR_TECH_OPAQUE))
      return !Conf.movres.avoid_server_grab;
   return 1;
}

static int
_MoveResizeModeValidate(unsigned int valid, int md)
{
   if (md & ~0x1f)
      return MR_OPAQUE;
   if (valid & (1U << md))
      return md;
   return MR_OPAQUE;
}

int
MoveResizeModeValidateMove(int md)
{
   return _MoveResizeModeValidate(MR_MODES_MOVE, md);
}

int
MoveResizeModeValidateResize(int md)
{
   return _MoveResizeModeValidate(MR_MODES_RESIZE, md);
}
