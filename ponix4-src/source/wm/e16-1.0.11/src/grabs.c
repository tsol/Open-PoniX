/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "cursors.h"
#include "grabs.h"
#include "xwin.h"

int
GrabKeyboardSet(Win win)
{
   int                 rc;

   rc =
      XGrabKeyboard(disp, WinGetXwin(win), False, GrabModeAsync, GrabModeAsync,
		    CurrentTime);

#if 0
   Eprintf("GrabKeyboardSet %#lx %d\n", WinGetXwin(win), rc);
#endif
   return rc;
}

int
GrabKeyboardRelease(void)
{
   int                 rc;

   rc = XUngrabKeyboard(disp, CurrentTime);

#if 0
   Eprintf("GrabKeyboardRelease %d\n", rc);
#endif
   return rc;
}

int
GrabPointerSet(Win win, unsigned int csr, int confine)
{
   int                 ret = -1;
   Window              confine_to = (confine) ? WinGetXwin(VROOT) : None;

   ret = XGrabPointer(disp, WinGetXwin(win), False,
		      ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		      ButtonMotionMask | EnterWindowMask | LeaveWindowMask,
		      GrabModeAsync, GrabModeAsync, confine_to, ECsrGet(csr),
		      CurrentTime);

   Mode.grabs.pointer_grab_window = WinGetXwin(win);
   Mode.grabs.pointer_grab_active = 1;

   if (EDebug(EDBUG_TYPE_GRABS))
      Eprintf("GrabPointerSet: %#lx, ret=%d\n", Mode.grabs.pointer_grab_window,
	      ret);

   return ret;
}

void
GrabPointerRelease(void)
{
   XUngrabPointer(disp, CurrentTime);

   if (EDebug(EDBUG_TYPE_GRABS))
      Eprintf("GrabPointerRelease: %#lx\n", Mode.grabs.pointer_grab_window);

   Mode.grabs.pointer_grab_active = 0;
   Mode.grabs.pointer_grab_window = None;
}

void
GrabButtonSet(unsigned int button, unsigned int modifiers, Win win,
	      unsigned int event_mask, unsigned int csr, int confine)
{
   Bool                owner_events = False;
   int                 pointer_mode = GrabModeSync;
   int                 keyboard_mode = GrabModeAsync;
   Window              confine_to = (confine) ? WinGetXwin(win) : None;
   int                 i;

   if (modifiers == AnyModifier)
     {
	XGrabButton(disp, button, modifiers,
		    WinGetXwin(win), owner_events, event_mask, pointer_mode,
		    keyboard_mode, confine_to, ECsrGet(csr));
	return;
     }

   for (i = 0; i < 8; i++)
     {
	if (i && !Mode.masks.mod_combos[i])
	   continue;
	XGrabButton(disp, button, modifiers | Mode.masks.mod_combos[i],
		    WinGetXwin(win), owner_events, event_mask, pointer_mode,
		    keyboard_mode, confine_to, ECsrGet(csr));
     }
}

void
GrabButtonRelease(unsigned int button, unsigned int modifiers, Win win)
{
   int                 i;

   if (modifiers == AnyModifier)
     {
	XUngrabButton(disp, button, modifiers, WinGetXwin(win));
	return;
     }

   for (i = 0; i < 8; i++)
     {
	if (i && !Mode.masks.mod_combos[i])
	   continue;
	XUngrabButton(disp, button, modifiers | Mode.masks.mod_combos[i],
		      WinGetXwin(win));
     }
}

void
GrabKeySet(unsigned int key, unsigned int modifiers, Win win)
{
   Bool                owner_events = False;
   int                 pointer_mode = GrabModeAsync;
   int                 keyboard_mode = GrabModeSync;
   int                 i;

   if (modifiers == AnyModifier)
     {
	XGrabKey(disp, key, modifiers, WinGetXwin(win), owner_events,
		 pointer_mode, keyboard_mode);
	return;
     }

   for (i = 0; i < 8; i++)
     {
	if (i && !Mode.masks.mod_combos[i])
	   continue;
	XGrabKey(disp, key, modifiers | Mode.masks.mod_combos[i],
		 WinGetXwin(win), owner_events, pointer_mode, keyboard_mode);
     }
}

void
GrabKeyRelease(unsigned int key, unsigned int modifiers, Win win)
{
   int                 i;

   if (modifiers == AnyModifier)
     {
	XUngrabKey(disp, key, modifiers, WinGetXwin(win));
	return;
     }

   for (i = 0; i < 8; i++)
     {
	if (i && !Mode.masks.mod_combos[i])
	   continue;
	XUngrabKey(disp, key, modifiers | Mode.masks.mod_combos[i],
		   WinGetXwin(win));
     }
}
