/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2009 Kim Woelders
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
#include "eobj.h"
#include "iclass.h"
#include "timers.h"
#include "xwin.h"

static EObj        *init_win1 = NULL;
static EObj        *init_win2 = NULL;
static char         bg_sideways = 0;

void
StartupWindowsCreate(void)
{
   Win                 w1, w2, win1, win2, b1, b2;
   Background         *bg;
   ImageClass         *ic;
   int                 x, y, bx, by, bw, bh, dbw;
   EObj               *eo;

   /* Acting only as boolean? */
   if (BackgroundFind("STARTUP_BACKGROUND_SIDEWAYS"))
      bg_sideways = 1;

   ic = ImageclassFind("STARTUP_BAR", 0);
   if (!ic)
      ic = ImageclassFind("DESKTOP_DRAGBUTTON_HORIZ", 0);
   bg = BackgroundFind("STARTUP_BACKGROUND");
   if (!ic || !bg)
      return;

   dbw = Conf.desks.dragbar_width;
   if (dbw <= 0)
      dbw = 16;
   if (bg_sideways)
     {
	x = WinGetW(VROOT) / 2;
	y = 0;
	bx = WinGetW(VROOT) - dbw;
	by = 0;
	bw = dbw;
	bh = WinGetH(VROOT);
     }
   else
     {
	x = 0;
	y = WinGetH(VROOT) / 2;
	bx = 0;
	by = WinGetH(VROOT) - dbw;
	bw = WinGetW(VROOT);
	bh = dbw;
     }

   eo = EobjWindowCreate(EOBJ_TYPE_MISC,
			 -x, -y, WinGetW(VROOT), WinGetH(VROOT), 1, "Init-1");
   if (!eo)
      return;
   init_win1 = eo;
   w1 = EobjGetWin(eo);
   win1 = ECreateWindow(w1, x, y, WinGetW(VROOT), WinGetH(VROOT), 0);

   eo = EobjWindowCreate(EOBJ_TYPE_MISC,
			 x, y, WinGetW(VROOT), WinGetH(VROOT), 1, "Init-2");
   if (!eo)
      return;
   init_win2 = eo;
   w2 = EobjGetWin(eo);
   win2 = ECreateWindow(w2, -x, -y, WinGetW(VROOT), WinGetH(VROOT), 0);

   EMapWindow(win1);
   EMapWindow(win2);

   if (bw > 0 && bh > 0)
     {
	b1 = ECreateWindow(w1, bx, by, bw, bh, 0);
	b2 = ECreateWindow(w2, 0, 0, bw, bh, 0);
	EMapRaised(b1);
	EMapRaised(b2);

	ImageclassApply(ic, b1, 0, 0, 0, ST_SOLID);
	ImageclassApply(ic, b2, 0, 0, 0, ST_SOLID);
     }

   BackgroundSet(bg, win1, WinGetW(VROOT), WinGetH(VROOT));
   BackgroundSet(bg, win2, WinGetW(VROOT), WinGetH(VROOT));
   StartupBackgroundsDestroy();

   EobjMap(init_win1, 0);
   EobjMap(init_win2, 0);

   EobjsRepaint();
}

void
StartupBackgroundsDestroy(void)
{
   BackgroundDestroyByName("STARTUP_BACKGROUND");
   BackgroundDestroyByName("STARTUP_BACKGROUND_SIDEWAYS");
}

static int
doStartupWindowsOpen(void *data __UNUSED__)
{
   static int          kk = 0;
   int                 k, x, y, xOffset, yOffset;

   k = kk;

   if (bg_sideways)
     {				/* so we can have two different slide methods */
	x = WinGetW(VROOT) / 2;
	xOffset = (x * k) >> 10;
	y = 0;
	yOffset = 0;
     }
   else
     {
	x = 0;
	xOffset = 0;
	y = WinGetH(VROOT) / 2;
	yOffset = (y * k) >> 10;
     }

   EobjMove(init_win1, -x - xOffset, -y - yOffset);
   EobjMove(init_win2, x + xOffset, y + yOffset);

   k = (int)(1e-3 * Conf.animation.step * Conf.desks.slidespeed / 2);
   if (k <= 0)
      k = 1;
   kk += k;
   if (kk < 1024)
      return 1;

   Mode.place.enable_features++;
   EobjWindowDestroy(init_win1);
   EobjWindowDestroy(init_win2);
   init_win1 = NULL;
   init_win2 = NULL;

   return 0;
}

void
StartupWindowsOpen(void)
{
   if (!init_win1 || !init_win2)
      return;

   Mode.place.enable_features--;
   ESync(ESYNC_STARTUP);
   AnimatorAdd(doStartupWindowsOpen, NULL);
}
