/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "hints.h"

/* Motif window hints */
#define MWM_HINTS_FUNCTIONS           (1L << 0)
#define MWM_HINTS_DECORATIONS         (1L << 1)
#define MWM_HINTS_INPUT_MODE          (1L << 2)
#define MWM_HINTS_STATUS              (1L << 3)

/* bit definitions for MwmHints.functions */
#define MWM_FUNC_ALL            (1L << 0)
#define MWM_FUNC_RESIZE         (1L << 1)
#define MWM_FUNC_MOVE           (1L << 2)
#define MWM_FUNC_MINIMIZE       (1L << 3)
#define MWM_FUNC_MAXIMIZE       (1L << 4)
#define MWM_FUNC_CLOSE          (1L << 5)

/* bit definitions for MwmHints.decorations */
#define MWM_DECOR_ALL                 (1L << 0)
#define MWM_DECOR_BORDER              (1L << 1)
#define MWM_DECOR_RESIZEH             (1L << 2)
#define MWM_DECOR_TITLE               (1L << 3)
#define MWM_DECOR_MENU                (1L << 4)
#define MWM_DECOR_MINIMIZE            (1L << 5)
#define MWM_DECOR_MAXIMIZE            (1L << 6)

/* bit definitions for MwmHints.inputMode */
#define MWM_INPUT_MODELESS                  0
#define MWM_INPUT_PRIMARY_APPLICATION_MODAL 1
#define MWM_INPUT_SYSTEM_MODAL              2
#define MWM_INPUT_FULL_APPLICATION_MODAL    3

#define PROP_MWM_HINTS_ELEMENTS             5
#define PROP_MWM_HINTS_ELEMENTS_MIN         4

static Atom         _MOTIF_WM_HINTS = 0;

/* Motif window hints */
typedef struct {
   long                flags;
   long                functions;
   long                decorations;
   long                inputMode;
   long                status;
} MWMHints;

void
MWM_GetHints(EWin * ewin, Atom atom_change)
{
   int                 fmt;
   Atom                a2;
   unsigned long       num, end;
   MWMHints           *mwmhints;
   unsigned char      *puc;

   if (EwinIsInternal(ewin))
      return;

   if (!_MOTIF_WM_HINTS)
      _MOTIF_WM_HINTS = EInternAtom("_MOTIF_WM_HINTS");

   if (atom_change && atom_change != _MOTIF_WM_HINTS)
      return;

   ewin->mwm.valid = 1;
   ewin->mwm.decor_border = 1;
   ewin->mwm.decor_resizeh = 1;
   ewin->mwm.decor_title = 1;
   ewin->mwm.decor_menu = 1;
   ewin->mwm.decor_minimize = 1;
   ewin->mwm.decor_maximize = 1;
   ewin->mwm.func_resize = 1;
   ewin->mwm.func_move = 1;
   ewin->mwm.func_minimize = 1;
   ewin->mwm.func_maximize = 1;
   ewin->mwm.func_close = 1;

   puc = NULL;
   XGetWindowProperty(disp, EwinGetClientXwin(ewin), _MOTIF_WM_HINTS, 0, 20,
		      False, _MOTIF_WM_HINTS, &a2, &fmt, &num, &end, &puc);
   mwmhints = (MWMHints *) puc;
   if (!mwmhints)
      return;

   if (num < PROP_MWM_HINTS_ELEMENTS_MIN)
      goto done;

   if (mwmhints->flags & MWM_HINTS_DECORATIONS)
     {
	ewin->mwm.decor_border = 0;
	ewin->mwm.decor_resizeh = 0;
	ewin->mwm.decor_title = 0;
	ewin->mwm.decor_menu = 0;
	ewin->mwm.decor_minimize = 0;
	ewin->mwm.decor_maximize = 0;
	if (mwmhints->decorations & MWM_DECOR_ALL)
	  {
	     ewin->mwm.decor_border = 1;
	     ewin->mwm.decor_resizeh = 1;
	     ewin->mwm.decor_title = 1;
	     ewin->mwm.decor_menu = 1;
	     ewin->mwm.decor_minimize = 1;
	     ewin->mwm.decor_maximize = 1;
	  }
	if (mwmhints->decorations & MWM_DECOR_BORDER)
	   ewin->mwm.decor_border = 1;
	if (mwmhints->decorations & MWM_DECOR_RESIZEH)
	   ewin->mwm.decor_resizeh = 1;
	if (mwmhints->decorations & MWM_DECOR_TITLE)
	   ewin->mwm.decor_title = 1;
	if (mwmhints->decorations & MWM_DECOR_MENU)
	   ewin->mwm.decor_menu = 1;
	if (mwmhints->decorations & MWM_DECOR_MINIMIZE)
	   ewin->mwm.decor_minimize = 1;
	if (mwmhints->decorations & MWM_DECOR_MAXIMIZE)
	   ewin->mwm.decor_maximize = 1;
     }

   if (mwmhints->flags & MWM_HINTS_FUNCTIONS)
     {
	ewin->mwm.func_resize = 0;
	ewin->mwm.func_move = 0;
	ewin->mwm.func_minimize = 0;
	ewin->mwm.func_maximize = 0;
	ewin->mwm.func_close = 0;
	if (mwmhints->functions & MWM_FUNC_ALL)
	  {
	     ewin->mwm.func_resize = 1;
	     ewin->mwm.func_move = 1;
	     ewin->mwm.func_minimize = 1;
	     ewin->mwm.func_maximize = 1;
	     ewin->mwm.func_close = 1;
	  }
	if (mwmhints->functions & MWM_FUNC_RESIZE)
	   ewin->mwm.func_resize = 1;
	if (mwmhints->functions & MWM_FUNC_MOVE)
	   ewin->mwm.func_move = 1;
	if (mwmhints->functions & MWM_FUNC_MINIMIZE)
	   ewin->mwm.func_minimize = 1;
	if (mwmhints->functions & MWM_FUNC_MAXIMIZE)
	   ewin->mwm.func_maximize = 1;
	if (mwmhints->functions & MWM_FUNC_CLOSE)
	   ewin->mwm.func_close = 1;
     }

   if (!ewin->mwm.decor_title && !ewin->mwm.decor_border)
      ewin->props.no_border = 1;

 done:
   XFree(mwmhints);
}

void
MWM_SetInfo(void)
{
   Atom                a1;
   struct {
      long                flags;
      Window              win;
   } mwminfo;

   a1 = EInternAtom("_MOTIF_WM_INFO");
   mwminfo.flags = 2;
   mwminfo.win = WinGetXwin(VROOT);
   XChangeProperty(disp, WinGetXwin(VROOT), a1, a1, 32, PropModeReplace,
		   (unsigned char *)&mwminfo, 2);
}
