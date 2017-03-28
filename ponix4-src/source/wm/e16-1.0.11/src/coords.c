/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2011 Kim Woelders
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
#include "hints.h"
#include "iclass.h"
#include "tclass.h"
#include "timers.h"
#include "xwin.h"

static EObj        *coord_eo = NULL;

static void
_CoordsShow(EWin * ewin, int mode)
{
   TextClass          *tc;
   ImageClass         *ic;
   char                s[256];
   int                 md;
   int                 x, y;
   unsigned int        w, h;
   int                 cx, cy, cw, ch;
   EObj               *eo = coord_eo;
   EImageBorder       *pad;
   int                 bl, br, bt, bb;

   if (!Conf.movres.mode_info)
      return;
   if (!ewin || !ewin->state.show_coords)
      return;

   tc = TextclassFind("COORDS", 1);
   ic = ImageclassFind("COORDS", 1);
   if ((!ic) || (!tc))
      return;

   cx = cy = cw = ch = 0;

   x = ewin->shape_x;
   y = ewin->shape_y;
   w = (ewin->state.shaded) ? ewin->client.w : ewin->shape_w;
   h = (ewin->state.shaded) ? ewin->client.h : ewin->shape_h;
   ICCCM_GetIncrementalSize(ewin, w, h, &w, &h);

   switch (mode)
     {
     default:
     case 0:
	Esnprintf(s, sizeof(s), "%i x %i (%i, %i)", w, h, x, y);
	break;
     case 1:
	Esnprintf(s, sizeof(s), _("Focused/unfocused opacity: %d/%d %%"),
		  OpacityToPercent(ewin->props.focused_opacity),
		  OpacityToPercent(ewin->ewmh.opacity));
	break;
     }
   TextSize(tc, 0, 0, 0, s, &cw, &ch, 17);
   pad = ImageclassGetPadding(ic);
   cw += pad->left + pad->right;
   ch += pad->top + pad->bottom;

   /* Width hysteresis (hack - assuming horizontal text) */
   cw += 8;
   if (eo && abs(EobjGetW(eo) - cw) < 8)
      cw = EobjGetW(eo);

   if (Mode.mode == MODE_MOVE)
      md = Conf.movres.mode_move;
   else
      md = Conf.movres.mode_resize;

   if ((md == 0) || ((cw < ewin->shape_w - 2) && (ch < ewin->shape_h - 2)))
     {
	if (Conf.movres.mode_info == 1)
	  {
	     switch (md)
	       {
	       case MR_OPAQUE:
	       case MR_TECHNICAL:
	       case MR_BOX:
	       case MR_TECH_OPAQUE:
		  EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
		  w = (ewin->state.shaded) ?
		     EoGetW(ewin) : ewin->shape_w + bl + br;
		  h = (ewin->state.shaded) ?
		     EoGetH(ewin) : ewin->shape_h + bt + bb;
		  cx = x + (w - cw) / 2 + EoGetX(EoGetDesk(ewin));
		  cy = y + (h - ch) / 2 + EoGetY(EoGetDesk(ewin));
		  break;
	       }
	  }
     }

   if (!eo)
     {
	eo = EobjWindowCreate(EOBJ_TYPE_MISC, 0, 0, 1, 1, 2, "Coord");
	if (!eo)
	   return;
	coord_eo = eo;
	eo->fade = eo->shadow = 1;

	/* Center text (override theme) */
	TextclassSetJustification(tc, 512);
     }

#define TEST_COORD_REPARENT_TO_FRAME 0
#if TEST_COORD_REPARENT_TO_FRAME
   cx -= x;
   cy -= y;
#endif
   md = cw != EobjGetW(eo) || ch != EobjGetH(eo);	/* md is change size flag */
   EobjMoveResize(eo, cx, cy, cw, ch);

   if (!eo->shown)
     {
#if TEST_COORD_REPARENT_TO_FRAME
	EobjReparent(eo, EoObj(ewin), cx, cy);
#endif
	EobjMap(eo, 0);
     }

   ITApply(EobjGetWin(eo), ic, NULL, STATE_NORMAL, 1, 0, ST_SOLID, tc, NULL, s,
	   1);

   if (md)			/* Assuming that shape change only happens when size changes too */
      EobjShapeUpdate(eo, 0);

   EFlush();
}

void
CoordsHide(void)
{
   EObj               *eo = coord_eo;

   if (eo && eo->shown)
     {
	EobjUnmap(eo);
#if TEST_COORD_REPARENT_TO_FRAME
	EobjReparent(eo, EoObj(DeskGet(0)), 0, 0);
#endif
     }
}

void
CoordsShow(EWin * ewin)
{
   _CoordsShow(ewin, 0);
}

static Timer       *timer_show_op = NULL;

static int
_CoordsHideTimeout(void *data __UNUSED__)
{
   CoordsHide();

   timer_show_op = NULL;
   return 0;
}

void
CoordsShowOpacity(EWin * ewin)
{
   EwinShapeSet(ewin);
   ewin->state.show_coords = 1;
   _CoordsShow(ewin, 1);
   TIMER_DEL(timer_show_op);
   TIMER_ADD(timer_show_op, 1000, _CoordsHideTimeout, NULL);
}
