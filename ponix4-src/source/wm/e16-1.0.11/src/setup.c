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

#include <signal.h>
#include <X11/keysym.h>

#include "E.h"
#include "events.h"
#include "ewins.h"
#include "screen.h"
#include "session.h"
#include "xwin.h"

static void
HandleXIOError(void)
{
   SessionExit(EEXIT_ERROR, NULL);
}

/*
 * This function sets up all of our connections to X
 */
void
SetupX(const char *dstr)
{
   int                 err;
   char                buf[128];
   unsigned int        mask;

   if (!dstr)
      dstr = getenv("DISPLAY");
   if (!dstr)
      dstr = ":0.0";

   /* Open a connection to the diplay nominated by the DISPLAY variable */
   err = EDisplayOpen(dstr, Dpy.screen);
   if (err)
     {
	Alert(_("Enlightenment cannot connect to the display nominated by\n"
		"your shell's DISPLAY environment variable. You may set this\n"
		"variable to indicate which display name Enlightenment is to\n"
		"connect to. It may be that you do not have an Xserver already\n"
		"running to serve that Display connection, or that you do not\n"
		"have permission to connect to that display. Please make sure\n"
		"all is correct before trying again. Run an Xserver by running\n"
		"xdm or startx first, or contact your local system\n"
		"administrator, or Xserver vendor, or read the X, xdm and\n"
		"startx manual pages before proceeding.\n"));
	EExit(1);
     }

   if (getenv("ESYNCHRONIZE"))
      XSynchronize(disp, True);

   Dpy.screens = ScreenCount(disp);
   Dpy.screen = DefaultScreen(disp);

   if (Mode.wm.master ||
       Mode.wm.master_screen < 0 || Mode.wm.master_screen >= Dpy.screens)
      Mode.wm.master_screen = Dpy.screen;

   /* Start up on multiple heads, if appropriate */
   if (Dpy.screens > 1 && !Mode.wm.single && !Mode.wm.restart)
     {
	int                 i;

	for (i = 0; i < Dpy.screens; i++)
	  {
	     pid_t               pid;

	     if (i == Dpy.screen)
		continue;

	     pid = fork();
	     if (pid)
	       {
		  /* We are the master */
		  Mode.wm.child_count++;
		  Mode.wm.children =
		     EREALLOC(pid_t, Mode.wm.children, Mode.wm.child_count);
		  Mode.wm.children[Mode.wm.child_count - 1] = pid;
	       }
	     else
	       {
		  /* We are a slave */
		  EDisplayDisconnect();
		  Mode.wm.master = 0;
		  Mode.wm.pid = getpid();
		  Dpy.screen = i;
		  ExtInitWinSet(None);
#ifdef SIGSTOP
		  kill(getpid(), SIGSTOP);
#endif
		  EDisplayOpen(dstr, i);
		  /* Terminate the loop as I am the child process... */
		  break;
	       }
	  }
     }

   Dpy.name = Estrdup(DisplayString(disp));
   Esetenv("DISPLAY", Dpy.name);

   Dpy.pixel_black = BlackPixel(disp, Dpy.screen);
   Dpy.pixel_white = WhitePixel(disp, Dpy.screen);

   EDisplaySetErrorHandlers(EventShowError, HandleXIOError);

   /* Root defaults */
   RROOT = ERegisterWindow(DefaultRootWindow(disp), NULL);

   if (Mode.wm.window)
     {
	VROOT = ECreateWindow(RROOT, 0, 0, Mode.wm.win_w, Mode.wm.win_h, 0);

	/* Enable eesh and edox to pick up the virtual root */
	Esnprintf(buf, sizeof(buf), "%#lx", WinGetXwin(VROOT));
	Esetenv("ENL_WM_ROOT", buf);
     }
   else
     {
	/* Running E normally on the root window */
	VROOT = RROOT;
     }

   /* Initialise event handling */
   EventsInit();

   /* select all the root window events to start managing */
   Dpy.last_error_code = 0;
   mask =
      StructureNotifyMask | SubstructureNotifyMask | SubstructureRedirectMask;
   ESelectInput(VROOT, mask);
   ESync(0);
   if (Dpy.last_error_code)
     {
	AlertX(_("Another Window Manager is already running"),
	       _("OK"), NULL, NULL,
	       _("Another Window Manager is already running.\n" "\n"
		 "You will have to quit your current Window Manager first before\n"
		 "you can successfully run Enlightenment.\n"));
	EExit(1);
     }

   mask |= ButtonPressMask | ButtonReleaseMask;
   ESelectInput(VROOT, mask);
   ESync(0);
   if (Dpy.last_error_code)
     {
	AlertX(_("Cannot select root window button press events"),
	       _("OK"), NULL, NULL,
	       _("Root window button actions will not work.\n"));
     }

   /* warn, if necessary about X version problems */
   if (ProtocolVersion(disp) != 11)
     {
	AlertX(_("X server version error"), _("Ignore this error"), "",
	       _("Quit Enlightenment"),
	       _("WARNING:\n"
		 "This is not an X11 Xserver. It in fact talks the X%i protocol.\n"
		 "This may mean Enlightenment will either not function, or\n"
		 "function incorrectly. If it is later than X11, then your\n"
		 "server is one the author of Enlightenment neither have\n"
		 "access to, nor have heard of.\n"), ProtocolVersion(disp));
     }

   /* damn that bloody numlock stuff - ok I'd rather XFree got fixed to not */
   /* have it as a modifier and everyone have to write specific code to mask */
   /* it out - but well.... */
   /* ok under Xfree Numlock and Scollock are lock modifiers and we need */
   /* to hunt them down to mask them out - EVIL EVIL EVIL hack but needed */
   {
      XModifierKeymap    *mod;
      KeyCode             nl, sl;
      unsigned int        numlock, scrollock;
      int                 i;

      int                 masks[8] = {
	 ShiftMask, LockMask, ControlMask, Mod1Mask, Mod2Mask, Mod3Mask,
	 Mod4Mask, Mod5Mask
      };

      numlock = scrollock = 0;
      mod = XGetModifierMapping(disp);
      nl = EKeysymToKeycode(XK_Num_Lock);
      sl = EKeysymToKeycode(XK_Scroll_Lock);
      if ((mod) && (mod->max_keypermod > 0))
	{
	   for (i = 0; i < (8 * mod->max_keypermod); i++)
	     {
		if ((nl) && (mod->modifiermap[i] == nl))
		   numlock = masks[i / mod->max_keypermod];
		else if ((sl) && (mod->modifiermap[i] == sl))
		   scrollock = masks[i / mod->max_keypermod];
	     }
	}
      Mode.masks.mod_combos[0] = 0;
      Mode.masks.mod_combos[1] = LockMask;
      if (numlock)
	{
	   Mode.masks.mod_combos[2] = numlock;
	   Mode.masks.mod_combos[5] = LockMask | numlock;
	}
      if (scrollock)
	{
	   Mode.masks.mod_combos[3] = scrollock;
	   Mode.masks.mod_combos[6] = LockMask | scrollock;
	}
      if (numlock && scrollock)
	{
	   Mode.masks.mod_combos[4] = numlock | scrollock;
	   Mode.masks.mod_combos[7] = LockMask | numlock | scrollock;
	}

      Mode.masks.mod_key_mask =
	 (ShiftMask | ControlMask | Mod1Mask | Mod2Mask | Mod3Mask | Mod4Mask |
	  Mod5Mask) & (~(numlock | scrollock | LockMask));

      if (mod)
	 XFreeModifiermap(mod);
   }

   ScreenInit();
}
