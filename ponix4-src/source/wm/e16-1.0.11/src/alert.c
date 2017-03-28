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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include "alert.h"
#include "lang.h"
#include "session.h"
#include "sounds.h"
#include "util.h"
#if USE_COMPOSITE
#include <X11/extensions/Xcomposite.h>
/* Composite Overlay Window (client) availability */
#if (1000 * COMPOSITE_MAJOR + COMPOSITE_MINOR) >= 3
#define USE_COMPOSITE_OVERLAY_WINDOW 1
#else
#define USE_COMPOSITE_OVERLAY_WINDOW 0
#endif
#endif

#define ExTextExtents XmbTextExtents
#define ExDrawString XmbDrawString

#define ExSetColor(pxc, r, g, b) \
  do { \
    (pxc)->red = (r << 8) | r; (pxc)->green = (g << 8) | g; (pxc)->blue = (b << 8) | b; \
  } while (0)

static XFontSet     xfs = NULL;

#define DRAW_BOX_OUT(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawBox(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[0], cols[2], cols[3])
#define DRAW_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawBox(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[2], cols[0], cols[3])
static void
AlertDrawBox(Display * mdd, GC mgc, Window mwin, int mx, int my, int mw, int mh,
	     int colorful, unsigned int c1, unsigned int c2, unsigned int cb)
{
   if (colorful)
     {
	XSetForeground(mdd, mgc, cb);
	XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1);
	XSetForeground(mdd, mgc, c1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3);
	XSetForeground(mdd, mgc, c2);
	XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2,
		  my + mh - 2);
	XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2,
		  my + mh - 2);
     }
   else
     {
	XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1);
     }
}

#define DRAW_THIN_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawThinBoxIn(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[2], cols[0])
static void
AlertDrawThinBoxIn(Display * mdd, GC mgc, Window mwin, int mx, int my, int mw,
		   int mh, int colorful, unsigned int c1, unsigned int c2)
{
   if (colorful)
     {
	XSetForeground(mdd, mgc, c1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3);
	XSetForeground(mdd, mgc, c2);
	XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2,
		  my + mh - 2);
	XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2,
		  my + mh - 2);
     }
}

#define DRAW_HEADER(mdd, mgc, mwin, mx, my, mstr) \
        AlertDrawHeader(mdd, mgc, mwin, mx, my, mstr, \
        colorful, cols[2], cols[3], cols[4])
static void
AlertDrawHeader(Display * mdd, GC mgc, Window mwin, int mx, int my,
		const char *mstr, int colorful, unsigned int cb,
		unsigned int ct1, unsigned int ct2)
{
   int                 len = strlen(mstr);

   if (colorful)
     {
	XSetForeground(mdd, mgc, cb);
	ExDrawString(mdd, mwin, xfs, mgc, mx + 1, my + 1, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx + 2, my + 1, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx + 2, my + 2, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx + 1, my + 2, mstr, len);
	XSetForeground(mdd, mgc, ct1);
	ExDrawString(mdd, mwin, xfs, mgc, mx - 1, my, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx, my - 1, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx + 1, my, mstr, len);
	ExDrawString(mdd, mwin, xfs, mgc, mx, my + 1, mstr, len);
	XSetForeground(mdd, mgc, ct2);
	ExDrawString(mdd, mwin, xfs, mgc, mx, my, mstr, len);
     }
   else
     {
	ExDrawString(mdd, mwin, xfs, mgc, mx, my, mstr, len);
     }
}

#define DRAW_STRING(mdd, mgc, mwin, mx, my, mstr, len) \
        AlertDrawString(mdd, mgc, mwin, mx, my, mstr, len, colorful, cols[3])
static void
AlertDrawString(Display * mdd, GC mgc, Window mwin, int mx, int my,
		const char *mstr, int len, int colorful, unsigned int ct1)
{
   if (colorful)
     {
	XSetForeground(mdd, mgc, ct1);
	ExDrawString(mdd, mwin, xfs, mgc, mx, my, mstr, len);
     }
   else
     {
	ExDrawString(mdd, mwin, xfs, mgc, mx, my, mstr, len);
     }
}

static char        *
AlertButtonText(int btn, char *buf, unsigned int len, const char *text)
{
   if (!text)
      return NULL;

   Esnprintf(buf, len, "(F%d) %s", btn, text);

   return buf;
}

static void
ShowAlert(const char *title,
	  const char *ignore, const char *restart, const char *quit,
	  const char *fmt, va_list args)
{
   char                text[4096], buf1[64], buf2[64], buf3[64];
   Window              win, b1 = 0, b2 = 0, b3 = 0, root;
   Display            *dd;
   int                 wid, hih, w, h, i, k, mask;
   XGCValues           gcv;
   GC                  gc;
   unsigned int        len;
   XEvent              ev;
   XSetWindowAttributes att;
   XRectangle          rect1, rect2;
   char                colorful;
   unsigned long       cols[5];
   XColor              xcl;
   Colormap            cmap;
   int                 cnum, fh, x, y, ww, hh, bw, bh;
   char               *str1, *str2, *str3, *p;
   KeyCode             key;
   int                 button;
   char              **missing_charset_list_return, *def_string_return;
   int                 missing_charset_count_return;
   XFontStruct       **font_struct_list_return;
   char              **font_name_list_return;

   SoundPlay(SOUND_ALERT);

   if (!fmt)
      return;

   Evsnprintf(text, sizeof(text), fmt, args);

   /*
    * We may get here from obscure places like an X-error or signal handler
    * and things seem to work properly only if we do a new XOpenDisplay().
    */
   dd = XOpenDisplay(NULL);
   if (!dd)
     {
	fprintf(stderr, "%s\n", text);
	fflush(stderr);
	return;
     }

   button = 0;

   if (!title)
      title = _("Enlightenment Error");
   str1 = AlertButtonText(1, buf1, sizeof(buf1), ignore);
   str2 = AlertButtonText(2, buf2, sizeof(buf2), restart);
   str3 = AlertButtonText(3, buf3, sizeof(buf3), quit);

   cnum = 0;
   colorful = 0;
   cols[0] = cols[1] = cols[2] = cols[3] = cols[4] = 0;
   cmap = DefaultColormap(dd, DefaultScreen(dd));
   if (DefaultDepth(dd, DefaultScreen(dd)) > 4)
     {
	ExSetColor(&xcl, 220, 220, 220);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ExSetColor(&xcl, 160, 160, 160);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ExSetColor(&xcl, 100, 100, 100);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ExSetColor(&xcl, 0, 0, 0);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ExSetColor(&xcl, 255, 255, 255);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	colorful = 1;
     }
 CN:

   if (colorful)
      att.background_pixel = cols[1];
   else
      att.background_pixel = BlackPixel(dd, DefaultScreen(dd));
   if (colorful)
      att.border_pixel = cols[3];
   else
      att.border_pixel = WhitePixel(dd, DefaultScreen(dd));
   att.backing_store = Always;
   att.save_under = True;
   att.override_redirect = True;
   mask = CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWSaveUnder |
      CWBackingStore;

#if USE_COMPOSITE_OVERLAY_WINDOW
   /*
    * Intended workings:
    * Composite extension not enabled (or COW not available?)
    * - fall back to root
    * Composite extension enabled
    * - use COW whether or not compositing is enabled, window mode too
    */
   root = XCompositeGetOverlayWindow(dd, DefaultRootWindow(dd));
   if (root == None)
#endif
     {
	root = DefaultRootWindow(dd);
     }
   win = XCreateWindow(dd, root, -100, -100, 1, 1, 0,
		       CopyFromParent, InputOutput, CopyFromParent, mask, &att);

   gc = XCreateGC(dd, win, 0, &gcv);
   if (colorful)
      XSetForeground(dd, gc, cols[3]);
   else
      XSetForeground(dd, gc, att.border_pixel);

   xfs = XCreateFontSet(dd, "fixed",
			&missing_charset_list_return,
			&missing_charset_count_return, &def_string_return);
   if (!xfs)
      goto done;

   if (missing_charset_list_return)
      XFreeStringList(missing_charset_list_return);

   k = XFontsOfFontSet(xfs, &font_struct_list_return, &font_name_list_return);
   fh = 0;
   for (i = 0; i < k; i++)
     {
	h = font_struct_list_return[i]->ascent +
	   font_struct_list_return[i]->descent;
	if (fh < h)
	   fh = h;
     }

   XSelectInput(dd, win, ExposureMask);
   XMapWindow(dd, win);

   XGrabServer(dd);

   XGrabPointer(dd, win, False, ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
   XGrabKeyboard(dd, win, False, GrabModeAsync, GrabModeAsync, CurrentTime);
   XSetInputFocus(dd, win, RevertToPointerRoot, CurrentTime);

   XSync(dd, False);

   wid = DisplayWidth(dd, DefaultScreen(dd));
   hih = DisplayHeight(dd, DefaultScreen(dd));
   ww = (wid >= 600) ? 600 : (wid / 40) * 40;
   hh = (hih >= 440) ? 440 : (hih / 40) * 40;

   for (i = 40; i < ww; i += 40)
     {
	w = i;
	h = (i * hh) / ww;
	x = (wid - w) >> 1;
	y = (hih - h) >> 1;
	XMoveResizeWindow(dd, win, x, y, w, h);
	DRAW_BOX_OUT(dd, gc, win, 0, 0, w, h);
	XSync(dd, False);
	usleep(20000);
     }
   x = (wid - ww) >> 1;
   y = (hih - hh) >> 1;
   XMoveResizeWindow(dd, win, x, y, ww, hh);
   XSync(dd, False);

   bw = 0;
   if (str1)
     {
	ExTextExtents(xfs, str1, strlen(str1), &rect1, &rect2);
	bw = (rect2.width > bw) ? rect2.width : bw;
     }
   if (str2)
     {
	ExTextExtents(xfs, str2, strlen(str2), &rect1, &rect2);
	bw = (rect2.width > bw) ? rect2.width : bw;
     }
   if (str3)
     {
	ExTextExtents(xfs, str3, strlen(str3), &rect1, &rect2);
	bw = (rect2.width > bw) ? rect2.width : bw;
     }
   bw += 20;
   bh = fh + 10;

#define BX(i) (5 + (((ww - bw - 10) * (i)) / 2))
#define BY    (hh - bh - 5)

   if (str1)
     {
	b1 = XCreateWindow(dd, win, BX(0), BY, bw, bh, 0, CopyFromParent,
			   InputOutput, CopyFromParent, mask, &att);
	XMapWindow(dd, b1);
     }
   if (str2)
     {
	b2 = XCreateWindow(dd, win, BX(1), BY, bw, bh, 0, CopyFromParent,
			   InputOutput, CopyFromParent, mask, &att);
	XMapWindow(dd, b2);
     }
   if (str3)
     {
	b3 = XCreateWindow(dd, win, BX(2), BY, bw, bh, 0, CopyFromParent,
			   InputOutput, CopyFromParent, mask, &att);
	XMapWindow(dd, b3);
     }
   XSync(dd, False);

   button = 0;
   for (; button == 0;)
     {
	XNextEvent(dd, &ev);
	switch (ev.type)
	  {
	  case KeyPress:
	     key = XKeysymToKeycode(dd, XK_F1);
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b1, 0, 0, bw, bh);
		  XSync(dd, False);
		  usleep(500000);
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, bw, bh);
		  button = 1;
		  goto do_sync;
	       }
	     key = XKeysymToKeycode(dd, XK_F2);
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b2, 0, 0, bw, bh);
		  XSync(dd, False);
		  usleep(500000);
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, bw, bh);
		  button = 2;
		  goto do_sync;
	       }
	     key = XKeysymToKeycode(dd, XK_F3);
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b3, 0, 0, bw, bh);
		  XSync(dd, False);
		  usleep(500000);
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, bw, bh);
		  button = 3;
		  goto do_sync;
	       }
	     break;

	  case ButtonPress:
	     if (!(ev.xbutton.y >= BY && ev.xbutton.y < BY + bh))
		break;

	     x = BX(0);
	     if (b1 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_IN(dd, gc, b1, 0, 0, bw, bh);
		  goto do_sync;
	       }
	     x = BX(1);
	     if (b2 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_IN(dd, gc, b2, 0, 0, bw, bh);
		  goto do_sync;
	       }
	     x = BX(2);
	     if (b3 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_IN(dd, gc, b3, 0, 0, bw, bh);
		  goto do_sync;
	       }
	     break;

	  case ButtonRelease:
	     if (!(ev.xbutton.y >= BY && ev.xbutton.y < BY + bh))
		break;

	     x = BX(0);
	     if (b1 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, bw, bh);
		  button = 1;
		  goto do_sync;
	       }
	     x = BX(1);
	     if (b2 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, bw, bh);
		  button = 2;
		  goto do_sync;
	       }
	     x = BX(2);
	     if (b3 && ev.xbutton.x >= x && ev.xbutton.x < x + bw)
	       {
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, bw, bh);
		  button = 3;
		  goto do_sync;
	       }
	     break;

	  case Expose:
	     /* Flush all other Expose events */
	     while (XCheckTypedWindowEvent(dd, ev.xexpose.window, Expose, &ev))
		;

	     ExTextExtents(xfs, title, strlen(title), &rect1, &rect2);
	     w = rect2.width;

	     DRAW_HEADER(dd, gc, win, (ww - w) / 2, 5 - rect2.y, title);
	     DRAW_BOX_OUT(dd, gc, win, 0, 0, ww, bh);
	     DRAW_BOX_OUT(dd, gc, win, 0, bh - 1, ww, hh - fh - fh - 30 + 2);
	     DRAW_BOX_OUT(dd, gc, win, 0, hh - fh - 20, ww, fh + 20);
	     k = bh;
	     for (p = text;; p += len + 1)
	       {
		  len = strcspn(p, "\n");
		  DRAW_STRING(dd, gc, win, 6, 6 + k + fh, p, len);
		  k += fh + 2;
		  if (p[len] == '\0')
		     break;
	       }
	     if (str1)
	       {
		  ExTextExtents(xfs, str1, strlen(str1), &rect1, &rect2);
		  w = rect2.width;
		  DRAW_HEADER(dd, gc, b1, (bw - w) / 2, 5 - rect2.y, str1);
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, bw, bh);
		  DRAW_THIN_BOX_IN(dd, gc, win,
				   BX(0) - 2, BY - 2, bw + 4, bh + 4);
	       }
	     if (str2)
	       {
		  ExTextExtents(xfs, str2, strlen(str2), &rect1, &rect2);
		  w = rect2.width;
		  DRAW_HEADER(dd, gc, b2, (bw - w) / 2, 5 - rect2.y, str2);
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, bw, bh);
		  DRAW_THIN_BOX_IN(dd, gc, win,
				   BX(1) - 2, BY - 2, bw + 4, bh + 4);
	       }
	     if (str3)
	       {
		  ExTextExtents(xfs, str3, strlen(str3), &rect1, &rect2);
		  w = rect2.width;
		  DRAW_HEADER(dd, gc, b3, (bw - w) / 2, 5 - rect2.y, str3);
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, bw, bh);
		  DRAW_THIN_BOX_IN(dd, gc, win,
				   BX(2) - 2, BY - 2, bw + 4, bh + 4);
	       }
	   do_sync:
	     XSync(dd, False);
	     break;

	  default:
	     break;
	  }
     }

   XFreeFontSet(dd, xfs);
 done:
   XUngrabServer(dd);
   XDestroyWindow(dd, win);
   XFreeGC(dd, gc);
   if (cnum > 0)
      XFreeColors(dd, cmap, cols, cnum, 0);
   XCloseDisplay(dd);

   switch (button)
     {
     default:
     case 1:
	break;
     case 2:
	SessionExit(EEXIT_RESTART, NULL);
	break;
     case 3:
	SessionExit(EEXIT_EXIT, NULL);
	break;
     }
}

void
AlertX(const char *title, const char *ignore,
       const char *restart, const char *quit, const char *fmt, ...)
{
   va_list             args;

   va_start(args, fmt);
   ShowAlert(title, ignore, restart, quit, fmt, args);
   va_end(args);
}

void
Alert(const char *fmt, ...)
{
   va_list             args;

   va_start(args, fmt);
   ShowAlert(_("Enlightenment Message Dialog"), _("Ignore this"),
	     _("Restart Enlightenment"), _("Quit Enlightenment"), fmt, args);
   va_end(args);
}

void
AlertOK(const char *fmt, ...)
{
   va_list             args;

   va_start(args, fmt);
   ShowAlert(_("Attention !!!"), _("OK"), NULL, NULL, fmt, args);
   va_end(args);
}
