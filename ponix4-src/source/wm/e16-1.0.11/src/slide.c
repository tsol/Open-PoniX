/*
 * Copyright (C) 2012 Kim Woelders
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
#include "eobj.h"
#include "ewins.h"
#include "focus.h"
#include "slide.h"
#include "xwin.h"

/*
 * EObj sliding functions
 */

void
EobjsSlideBy(EObj ** peo, int num, int dx, int dy, int speed)
{
   int                 i, k, x, y;
   struct _xy {
      int                 x, y;
   }                  *xy;

   if (num <= 0)
      return;

   xy = EMALLOC(struct _xy, num);
   if (!xy)
      return;

   for (i = 0; i < num; i++)
     {
	xy[i].x = EobjGetX(peo[i]);
	xy[i].y = EobjGetY(peo[i]);
     }

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num; i++)
	  {
	     x = ((xy[i].x * (1024 - k)) + ((xy[i].x + dx) * k)) >> 10;
	     y = ((xy[i].y * (1024 - k)) + ((xy[i].y + dy) * k)) >> 10;
	     EobjMove(peo[i], x, y);
	  }

	k = ETimedLoopNext();
     }

   for (i = 0; i < num; i++)
      EobjMove(peo[i], xy[i].x + dx, xy[i].y + dy);

   Efree(xy);
}

void
EobjSlideSizeTo(EObj * eo, int fx, int fy, int tx, int ty, int fw, int fh,
		int tw, int th, int speed)
{
   int                 k, x, y, w, h;

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	w = ((fw * (1024 - k)) + (tw * k)) >> 10;
	h = ((fh * (1024 - k)) + (th * k)) >> 10;
	EobjMoveResize(eo, x, y, w, h);

	k = ETimedLoopNext();
     }
   EobjMoveResize(eo, tx, ty, tw, th);
}

/*
 * EWin sliding functions
 */

void
EwinSlideSizeTo(EWin * ewin, int tx, int ty, int tw, int th,
		int speed, int mode __UNUSED__, int flags __UNUSED__)
{
   int                 k, x, y, w, h;
   int                 fx, fy, fw, fh, warp;

   warp = (ewin == GetEwinPointerInClient());

   if (Conf.movres.maximize_animate)
     {
	fx = EoGetX(ewin);
	fy = EoGetY(ewin);
	fw = ewin->client.w;
	fh = ewin->client.h;

	ETimedLoopInit(0, 1024, speed);
	for (k = 0; k <= 1024;)
	  {
	     x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	     y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	     w = ((fw * (1024 - k)) + (tw * k)) >> 10;
	     h = ((fh * (1024 - k)) + (th * k)) >> 10;
	     EwinMoveResize(ewin, x, y, w, h, MRF_KEEP_MAXIMIZED);

	     k = ETimedLoopNext();
	  }
     }

   EwinMoveResize(ewin, tx, ty, tw, th, MRF_KEEP_MAXIMIZED);

   if (warp && ewin != GetEwinPointerInClient())
     {
	EwinWarpTo(ewin, 1);
	FocusToEWin(ewin, FOCUS_SET);
     }
}

void
EwinSlideTo(EWin * ewin, int fx, int fy, int tx, int ty,
	    int speed, int mode, int flags)
{
   EwinsSlideTo(&ewin, &fx, &fy, &tx, &ty, 1, speed, mode, flags);
}

void
EwinsSlideTo(EWin ** ewin, int *fx, int *fy, int *tx, int *ty, int num_wins,
	     int speed, int mode, int flags __UNUSED__)
{
   int                 k, x, y, w, h, i;
   char                firstlast, grab_server;

   if (num_wins <= 0)
      return;

   firstlast = 0;
   FocusEnable(0);
   SoundPlay(SOUND_WINDOW_SLIDE);

   grab_server = DrawEwinShapeNeedsGrab(mode);
   if (grab_server)
      EGrabServer();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num_wins; i++)
	  {
	     if (!ewin[i])
		continue;

	     x = ((fx[i] * (1024 - k)) + (tx[i] * k)) >> 10;
	     y = ((fy[i] * (1024 - k)) + (ty[i] * k)) >> 10;
	     w = ewin[i]->client.w;
	     h = ewin[i]->client.h;
	     if (mode == 0)
		EoMove(ewin[i], x, y);
	     else
		DrawEwinShape(ewin[i], mode, x, y, w, h, firstlast, i);
	     firstlast = 1;
	  }
	/* We may loop faster here than originally intended */
	k = ETimedLoopNext();
     }

   for (i = 0; i < num_wins; i++)
     {
	if (!ewin[i])
	   continue;

	ewin[i]->state.animated = 0;

	if (mode > 0)
	   DrawEwinShape(ewin[i], mode, tx[i], ty[i], ewin[i]->client.w,
			 ewin[i]->client.h, 2, i);
	EwinMove(ewin[i], tx[i], ty[i], MRF_NOCHECK_ONSCREEN);
     }

   FocusEnable(1);

   if (grab_server)
      EUngrabServer();

   SoundPlay(SOUND_WINDOW_SLIDE_END);
}
