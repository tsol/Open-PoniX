/* 
 *  Matchbox Window Manager - A lightweight window manager not for the
 *                            desktop.
 *
 *  Authored By Matthew Allum <mallum@o-hand.com>
 *
 *  Copyright (c) 2002, 2004 OpenedHand Ltd - http://o-hand.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 */

#include "structs.h"
#include "wm.h"
#include "misc.h"

int 
main(int argc, char **argv)
{
   Wm *w;
   struct sigaction act;
   memset(&act, 0, sizeof(struct sigaction));
   
   act.sa_handler = sig_handler;
   act.sa_flags = 0;
   sigaction(SIGTERM, &act, NULL);
   sigaction(SIGINT, &act,  NULL);
   sigaction(SIGHUP, &act,  NULL);
   sigaction(SIGCHLD, &act, NULL);

   signal (SIGCHLD, SIG_IGN); 	/* as now we exec via keyboard */

   w = wm_new(argc, argv);

   if (getenv("MB_SYNC")) 
     XSynchronize (w->dpy, True);

   wm_init_existing(w);

   wm_event_loop(w);
   
   return 1;
}






