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

#include "misc.h"

static int trapped_error_code = 0;
static int (*old_error_handler) (Display *d, XErrorEvent *e);

#ifndef HAVE_STRSEP 	 
char *
strsep(char **stringp, char *delim)
{
  char *start = *stringp;
  char *cp;
  char ch;

  if (start == NULL)
    return NULL;

  for (cp = start; ch = *cp; cp++) {
    if (strchr(delim, ch)) {
      *cp++ = 0;
      *stringp = cp;
      return start;
    }
  }
  *stringp = NULL;
  return start;
}
#endif

void 
fork_exec(char *cmd)
{
  switch (fork())
    {
    case 0:
      execlp ("/bin/sh", "sh", "-c", cmd, (char *)NULL);
      fprintf (stderr, "Exec failed.\n");
      exit (0);
      break;
    case -1:
      fprintf (stderr, "Fork failed.\n");
      break;
    }
}

void 
sig_handler(int signal)
{
    switch (signal) {
        case SIGINT:
        case SIGTERM:
        case SIGHUP:
	  exit(1); break;
        case SIGCHLD:
          wait(NULL); break;
    }
}

int 
handle_xerror(Display *dpy, XErrorEvent *e)
{
    if (e->error_code == BadAccess &&
	e->resourceid == RootWindow(dpy, DefaultScreen(dpy)) ) 
      {
        fprintf(stderr, 
		"root window unavailible (maybe another wm is running?)\n");
        exit(1);
      } 
    else 
      {
        char msg[255];
        XGetErrorText(dpy, e->error_code, msg, sizeof msg);
        fprintf(stderr, "matchbox-wm: X error warning (%#lx): %s (opcode: %i)\n",
		e->resourceid, msg, e->request_code);
      }

    return 0;
}

int 
ignore_xerror(Display *dpy, XErrorEvent *e)
{
   return 0;
}

static int
trapped_xerror_handler(Display     *display,
	      XErrorEvent *error)
{
   trapped_error_code = error->error_code;
   return 0;
}

void
misc_trap_xerrors(void)
{
  trapped_error_code = 0;
  dbg("**** %s() *****\n", __func__);
  old_error_handler = XSetErrorHandler(trapped_xerror_handler);
}

int
misc_untrap_xerrors(void)
{
  dbg("**** %s() *****\n", __func__);
  XSetErrorHandler(old_error_handler);
  return trapped_error_code;
}


 /* check for ageing mwm hints, it probably shouldn't be in misc.c ..  */
int 
mwm_get_decoration_flags(Wm *w, Window win)
{

#define PROP_MOTIF_WM_HINTS_ELEMENTS    5
#define MWM_HINTS_DECORATIONS   (1L << 1)

#define MWM_DECOR_BORDER              (1L << 1)
#define MWM_DECOR_RESIZEH             (1L << 2)
#define MWM_DECOR_TITLE               (1L << 3)
#define MWM_DECOR_MENU                (1L << 4)
#define MWM_DECOR_MINIMIZE            (1L << 5)
#define MWM_DECOR_MAXIMIZE            (1L << 6)

  typedef struct
  {
    unsigned long       flags;
    unsigned long       functions;
    unsigned long       decorations;
    long                inputMode;
    unsigned long       status;
  } PropMotifWmHints;

  Atom type;
  int format;
  PropMotifWmHints *hints = NULL;
  unsigned long n_items, bytes_after;

  if (XGetWindowProperty (w->dpy, win, w->atoms[_MOTIF_WM_HINTS],
                          0, PROP_MOTIF_WM_HINTS_ELEMENTS,
                          False, AnyPropertyType, &type, &format, &n_items,
                          &bytes_after, (unsigned char **)&hints) != Success ||
      type == None)
    {
      dbg("MWM xgetwinprop failed\n");
      if (hints) XFree(hints);
      return 0;
    }
  
  if (type == None || n_items != PROP_MOTIF_WM_HINTS_ELEMENTS)
    {
      dbg("MWM xgetwinprop failed - %li items != 5\n", n_items);
      if (hints) XFree(hints);
      return 0;
    }

  dbg("MWM xgetwinprop success \n");

  if (hints->flags & MWM_HINTS_DECORATIONS)
  {
    if (hints->decorations == 0)
      {
	dbg("%s() MWM is no decor. \n", __func__);
	if (hints) XFree(hints);
	return CLIENT_TITLE_HIDDEN_FLAG; /* XXX should really change flag to 
					    NO_DECOR */
      }

    if (hints->decorations == MWM_DECOR_BORDER)
      {
	dbg("%s() MWM is borders only. \n", __func__);
	if (hints) XFree(hints);
	return CLIENT_BORDERS_ONLY_FLAG;
      }
  }

  dbg("%s() MWM, nothing of interest here. \n", __func__);

  if (hints) XFree(hints);

  return 0;

}

void
misc_scale_wm_app_icon(Wm *w)
{
#ifndef STANDALONE
  if (w->img_generic_icon->width != 16 
      || w->img_generic_icon->height != 16) 
    {
      MBPixbufImage *img_tmp;
      img_tmp = mb_pixbuf_img_scale_down(w->pb, w->img_generic_icon,
					 16, 16);
      mb_pixbuf_img_free(w->pb, w->img_generic_icon);
      w->img_generic_icon = img_tmp;
    }
#endif
}
