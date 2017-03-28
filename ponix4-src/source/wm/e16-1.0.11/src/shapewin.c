/*
 * Copyright (C) 2007-2011 Kim Woelders
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
#include "ewins.h"
#include "xwin.h"

#define _SHAPE_SET_RECT(rl, _x, _y, _w, _h) \
  do { \
    rl[0].x = (_x);        rl[0].y = (_y);        rl[0].width = (_w); rl[0].height = 1; \
    rl[1].x = (_x);        rl[1].y = (_y)+(_h)-1; rl[1].width = (_w); rl[1].height = 1; \
    rl[2].x = (_x);        rl[2].y = (_y)+1;      rl[2].width = 1;    rl[2].height = (_h)-2; \
    rl[3].x = (_x)+(_w)-1; rl[3].y = (_y)+1;      rl[3].width = 1;    rl[3].height = (_h)-2; \
  } while(0)

static unsigned int
_ShapeGetColor(void)
{
   static char         color_valid = 0;
   static unsigned int color_value = 0;
   static unsigned int color_pixel;

   if (color_valid && color_value == Conf.movres.color)
      goto done;

   color_value = Conf.movres.color;
   color_pixel = EAllocColor(WinGetCmap(VROOT), color_value);
   color_valid = 1;

 done:
   return color_pixel;
}

void
ShapewinDestroy(ShapeWin * sw)
{
   if (!sw)
      return;

   EoUnmap(sw);
   EoFini(sw);
   EXFreeGC(sw->gc);
   if (sw->mask != None)
      EFreePixmap(sw->mask);
   Efree(sw);
}

ShapeWin           *
ShapewinCreate(int md)
{
   ShapeWin           *sw;

   sw = ECALLOC(ShapeWin, 1);
   if (!sw)
      return NULL;

   EoInit(sw, EOBJ_TYPE_MISC, None,
	  0, 0, WinGetW(VROOT), WinGetH(VROOT), 2, "Wires");
   if (!EoGetWin(sw))
      goto bail_out;

   EoSetFloating(sw, 1);
   EoSetLayer(sw, 18);
   ESetWindowBackground(EoGetWin(sw), _ShapeGetColor());
#ifdef ShapeInput		/* Should really check server too */
   XShapeCombineRectangles(disp, EoGetXwin(sw),
			   ShapeInput, 0, 0, NULL, 0, ShapeSet, Unsorted);
#endif

   if ((md == MR_TECHNICAL) || (md == MR_TECH_OPAQUE))
     {
	sw->mask =
	   ECreatePixmap(EoGetWin(sw), WinGetW(VROOT), WinGetH(VROOT), 1);
	sw->gc = EXCreateGC(sw->mask, 0, NULL);
	if (sw->mask == None || !sw->gc)
	   goto bail_out;
     }

   return sw;

 bail_out:
   ShapewinDestroy(sw);
   return NULL;
}

void
ShapewinShapeSet(ShapeWin * sw, int md, int x, int y, int w, int h,
		 int bl, int br, int bt, int bb, int seqno)
{
   int                 w2, h2;

   w2 = w + bl + br;
   h2 = h + bt + bb;

   if ((md == MR_TECHNICAL) || (md == MR_TECH_OPAQUE))
     {
	XSetForeground(disp, sw->gc, 0);
	XFillRectangle(disp, sw->mask, sw->gc,
		       0, 0, WinGetW(VROOT), WinGetH(VROOT));
	XSetForeground(disp, sw->gc, 1);
	do_draw_technical(sw->mask, sw->gc, x, y, w, h, bl, br, bt, bb);
	if (seqno == 0)
	   EShapeSetMask(EoGetWin(sw), 0, 0, sw->mask);
	else
	   EShapeUnionMask(EoGetWin(sw), 0, 0, sw->mask);
     }
   else
     {
	XRectangle          rl[8];

	_SHAPE_SET_RECT((&rl[0]), x, y, w2, h2);
	w = (w > 5) ? w - 2 : 3;
	h = (h > 5) ? h - 2 : 3;
	_SHAPE_SET_RECT((&rl[4]), x + bl + 1, y + bt + 1, w, h);

	if (seqno == 0)
	   EShapeSetRects(EoGetWin(sw), 0, 0, rl, 8);
	else
	   EShapeUnionRects(EoGetWin(sw), 0, 0, rl, 8);
     }
   EoShapeUpdate(sw, 0);
}
