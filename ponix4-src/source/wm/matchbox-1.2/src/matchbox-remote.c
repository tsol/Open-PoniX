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
#define _GNU_SOURCE

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MB_CMD_SET_THEME     1
#define MB_CMD_EXIT          2
#define MB_CMD_DESKTOP       3
#define MB_CMD_NEXT          4
#define MB_CMD_PREV          5
#define MB_CMD_SHOW_EXT_MENU 6
#define MB_CMD_MISC          7
#define MB_CMD_COMPOSITE     8
#define MB_CMB_KEYS_RELOAD   9

#define MB_CMD_PANEL_TOGGLE_VISIBILITY 1
#define MB_CMD_PANEL_SIZE              2
#define MB_CMD_PANEL_ORIENTATION       3

#define MB_PANEL_ORIENTATION_NORTH     1
#define MB_PANEL_ORIENTATION_EAST      2
#define MB_PANEL_ORIENTATION_SOUTH     3
#define MB_PANEL_ORIENTATION_WEST      4

Display* dpy;	

static void
getRootProperty(char * name, Bool delete)
{
   Atom prop;
   Atom realType;
   unsigned long n;
   unsigned long extra;
   int format;
   int status;
   char * value;
	
   prop = XInternAtom(dpy, name, True);
   if (prop == None) {
      fprintf(stderr, "mbcontrol: Unable to find theme name\n");
      return;
   }
	
   status = XGetWindowProperty(dpy, DefaultRootWindow(dpy),
			       prop, 0L, 512L, delete,
			       AnyPropertyType, &realType, &format,
			       &n, &extra, (unsigned char **) &value);
   
   if (status != Success) { // || value == 0 || *value == 0 || n == 0) {
      fprintf(stderr, "Unable to find theme name\n");
      return;
   }

   if (value) printf("%s\n", value);

}

static void
mbpanelcommand(int cmd_id, int panel_id)
{
  XEvent	ev;
  Window	root, panel;
  Atom          cmd_prop, system_tray_atom;
  char          atomName[64];

  root = DefaultRootWindow(dpy);
  
  snprintf(atomName, 64, "_NET_SYSTEM_TRAY_S%d", panel_id);
  
  system_tray_atom = XInternAtom(dpy, atomName, False);
  cmd_prop         = XInternAtom(dpy, "_MB_COMMAND", False);
  
  if (cmd_id == MB_CMD_PANEL_TOGGLE_VISIBILITY)
    {
      panel = XGetSelectionOwner(dpy, system_tray_atom);
      
      if (panel != None)
	{
	  memset(&ev, '\0', sizeof ev);
	  ev.xclient.type   = ClientMessage;
	  ev.xclient.window = root;
	  ev.xclient.message_type = cmd_prop;
	  ev.xclient.format = 8;
	  ev.xclient.data.l[0] = cmd_id;
	  
	  XSendEvent(dpy, panel,False,
		     SubstructureRedirectMask|SubstructureNotifyMask, &ev);
	  
	}
      else 
	{
	  fprintf(stderr, "Cant find panel %i\n", panel_id);
	  exit(1);
	}
    }
}

static void
mbcommand(int cmd_id, char *data) {

   XEvent	ev;
   Window	root;
   Atom theme_prop, cmd_prop, desktop_manager_atom;

   desktop_manager_atom = XInternAtom(dpy, "_NET_DESKTOP_MANGER",False);

   root = DefaultRootWindow(dpy);
   
   if (cmd_id == MB_CMD_SET_THEME)
   {
      theme_prop = XInternAtom(dpy, "_MB_THEME", False);
      XChangeProperty(dpy, root, theme_prop, XA_STRING, 8,
		      PropModeReplace, (unsigned char*)data, strlen(data));
   }

   if (cmd_id == MB_CMD_DESKTOP)
     {
       /* Check if desktop is running */
       if (!XGetSelectionOwner(dpy, desktop_manager_atom))
	 {
	   fprintf(stderr, "Desktop not running, exiting...\n");
	   switch (fork())
	     {
	     case 0:
	       execvp ("mbdesktop", NULL);
	       break;
	     case -1:
	       fprintf(stderr, "failed to exec mbdesktop");
	       break;
	     }
	   exit(0);
	 }
     }
   
   cmd_prop = XInternAtom(dpy, "_MB_COMMAND", False);
         
   memset(&ev, '\0', sizeof ev);
   ev.xclient.type = ClientMessage;
   ev.xclient.window = root; 	/* we send it _from_ root as we have no win  */
   ev.xclient.message_type = cmd_prop;
   ev.xclient.format = 8;

   ev.xclient.data.l[0] = cmd_id;

   XSendEvent(dpy, root, False, 
	      SubstructureRedirectMask|SubstructureNotifyMask, &ev);

}

void
send_input_manager_request(int show)
{
   XEvent	ev;
   Window	root;
   Atom         atom_input;

   atom_input = XInternAtom(dpy, "_MB_INPUT_REQUEST",False);

   root = DefaultRootWindow(dpy);

   memset(&ev, '\0', sizeof ev);

   ev.xclient.type = ClientMessage;
   ev.xclient.window = root; 	/* we send it _from_ root as we have no win  */
   ev.xclient.message_type = atom_input;
   ev.xclient.format = 8;

   ev.xclient.data.l[0] = show;

   printf("valie : %i\n", show);

   XSendEvent(dpy, root, False, 
	      SubstructureRedirectMask|SubstructureNotifyMask, &ev);
}


static void
usage(char *progname)
{
   printf("Usage: %s [options...]\n", progname);
   printf("Options:\n");
   printf("  -t                       <matchbox theme name>  switch matchbox theme\n");
   printf("  -r                       Print current matchbox theme to stdout \n");
   printf("  -exit                    Request matchbox to exit \n");
   printf("  -next                    Page to next window \n");
   printf("  -prev                    Page to previous window \n");
   printf("  -desktop                 Toggle desktop visibility\n");
   printf("  -menu                    Activate mb-applet-menu-launcher\n");
   printf("  -panel-toggle [panel id] Toogle panel visibility\n");
   printf("  -input-toggle [1|0]      Toggle Input method ( requires input-manager )\n");
   printf("  -composite-toggle        Toggle Compositing Engine ( if enabled )\n");
   printf("  -keys-reload             Reload key shortcut config ( if enabled )\n");


   /*
   printf("  -panel-size <int>\n");
   printf("  -panel-orientate <north|east|south|west>\n");
   */
   printf("  -h  this help\n\n");
   exit(1);
}

int main(int argc, char* argv[])
{
  char *display_name = (char *)getenv("DISPLAY");
  int i;

  if (argc < 2) usage(argv[0]);
  
  dpy = XOpenDisplay(display_name);
  if (dpy == NULL) {
     printf("Cant connect to display: %s\n", display_name);
     exit(1);
  }

  /* pass command line */
  for (i=1; argv[i]; i++) {
     char *arg = argv[i];
     if (*arg=='-') {
	switch (arg[1]) 
	{
	case 't' :
	  if (argv[i+1] != NULL)
	    mbcommand(MB_CMD_SET_THEME, argv[i+1]);
	  i++;
	  break;
	case 'r' :
	  getRootProperty("_MB_THEME", False);
	  i++;
	  break;
	case 'e':
	  mbcommand(MB_CMD_EXIT, NULL);
	  break;
	case 'd':
	  mbcommand(MB_CMD_DESKTOP, NULL);
	  break;
	case 'n':
	  mbcommand(MB_CMD_NEXT, NULL);
	  break;
	case 'c':
	  mbcommand(MB_CMD_COMPOSITE, NULL);
	  break;
	case 'k':
	  mbcommand(MB_CMB_KEYS_RELOAD, NULL);
	  break;
	case 'p':
	  if (!strcmp(arg+1,"panel-toggle"))
	    {
	      int panel_id = 0;
	      
	      if (argc > i+1) panel_id = atoi(argv[i+1]);
	      mbpanelcommand(MB_CMD_PANEL_TOGGLE_VISIBILITY, panel_id);
	    }
	  else if (strcmp(arg+1,"prev") == 0 || strlen(arg+1) == 1)
	    {
	      mbcommand(MB_CMD_PREV, NULL);
	    }
	  else usage(argv[0]);
	  break;
	case 'm':
	  mbcommand(MB_CMD_SHOW_EXT_MENU, NULL);
	  break;
	case 'x':
	  mbcommand(MB_CMD_MISC, NULL);
	  break;
	case 'i':
	  if (argv[i+1] != NULL)
	    send_input_manager_request(atoi(argv[i+1]));
	  else
	    usage(argv[0]);
	  break;
	default:
	  usage(argv[0]);
	  break;
	}
     }
  }
  XSync(dpy, False);
  XCloseDisplay(dpy);

  return 0;
}    

