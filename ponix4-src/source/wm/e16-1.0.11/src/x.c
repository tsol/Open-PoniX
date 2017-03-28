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
#include "E.h"
#include "edebug.h"
#include "util.h"
#include "xwin.h"
#include <string.h>
#include <unistd.h>
#ifdef USE_ECORE_X
#include <Ecore_X.h>
#endif
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xresource.h>
#include <X11/extensions/shape.h>
#if USE_XRENDER
#include <X11/extensions/Xrender.h>
#endif
#if USE_GLX
#include "eglx.h"
#endif
#if USE_XI2
#include "X11/extensions/XInput2.h"
#endif

#define DEBUG_XWIN   0
#define DEBUG_PIXMAP 0

EDisplay            Dpy;
Display            *disp;

#if USE_COMPOSITE
static Visual      *argb_visual = NULL;
static Colormap     argb_cmap = None;
#endif

static XContext     xid_context = 0;

static Win          win_first = NULL;
static Win          win_last = NULL;

#define WinBgInvalidate(win) if (win->bg_owned > 0) win->bg_owned = -1

void
EXInit(void)
{
   memset(&Dpy, 0, sizeof(Dpy));
}

static              Win
EXidCreate(void)
{
   Win                 win;

   win = ECALLOC(struct _xwin, 1);

   win->bgcol = 0xffffffff;

   return win;
}

static void
EXidDestroy(Win win)
{
#if DEBUG_XWIN
   Eprintf("EXidDestroy: %p %#lx\n", win, win->xwin);
#endif
   if (win->rects)
      XFree(win->rects);
   Efree(win->cbl.lst);
   Efree(win);
}

static void
EXidAdd(Win win)
{
#if DEBUG_XWIN
   Eprintf("EXidAdd: %p %#lx\n", win, win->xwin);
#endif
   if (!xid_context)
      xid_context = XUniqueContext();

   XSaveContext(disp, win->xwin, xid_context, (XPointer) win);

   if (!win_first)
     {
	win_first = win_last = win;
     }
   else
     {
	win->prev = win_last;
	win_last->next = win;
	win_last = win;
     }
}

static void
EXidDel(Win win)
{
#if DEBUG_XWIN
   Eprintf("EXidDel: %p %#lx\n", win, win->xwin);
#endif
   if (win == win_first)
     {
	if (win == win_last)
	  {
	     win_first = win_last = NULL;
	  }
	else
	  {
	     win_first = win->next;
	     win->next->prev = NULL;
	  }
     }
   else if (win == win_last)
     {
	win_last = win->prev;
	win->prev->next = NULL;
     }
   else
     {
	win->prev->next = win->next;
	win->next->prev = win->prev;
     }

   XDeleteContext(disp, win->xwin, xid_context);
   if (win->in_use)
      win->do_del = 1;
   else
      EXidDestroy(win);
}

#define EXidLookup ELookupXwin

Win
EXidLookup(Window xwin)
{
   Win                 win;
   XPointer            xp;

   if (!xid_context)
      return NULL;

   xp = NULL;
   if (XFindContext(disp, xwin, xid_context, &xp) == XCNOENT)
      xp = NULL;
   win = (Win) xp;

   return win;
}

static              Win
EXidSet(Window xwin, Win parent, int x, int y, int w, int h, int depth,
	Visual * visual, Colormap cmap)
{
   Win                 win;

   win = EXidCreate();
   win->parent = parent;
   win->xwin = xwin;
   win->x = x;
   win->y = y;
   win->w = w;
   win->h = h;
   win->depth = depth;
   win->visual = visual;
   win->cmap = cmap;
   win->argb = depth == 32;
#if DEBUG_XWIN
   Eprintf("EXidSet: %#lx\n", win->xwin);
#endif
   EXidAdd(win);

   return win;
}

void
EventCallbackRegister(Win win, EventCallbackFunc * func, void *prm)
{
   EventCallbackItem  *eci;

   if (!win)
      return;
#if 0
   Eprintf("EventCallbackRegister: %p %#lx: func=%p prm=%p\n", win, win->xwin,
	   func, prm);
#endif

   win->cbl.num++;
   win->cbl.lst = EREALLOC(EventCallbackItem, win->cbl.lst, win->cbl.num);
   eci = win->cbl.lst + win->cbl.num - 1;
   eci->func = func;
   eci->prm = prm;
}

void
EventCallbackUnregister(Win win, EventCallbackFunc * func, void *prm)
{
   EventCallbackList  *ecl;
   EventCallbackItem  *eci;
   int                 i;

   if (!win)
      return;
#if 0
   Eprintf("EventCallbackUnregister: %p %#lx: func=%p prm=%p\n", win, win->xwin,
	   func, prm);
#endif

   ecl = &win->cbl;
   eci = ecl->lst;
   for (i = 0; i < ecl->num; i++, eci++)
      if (eci->func == func && eci->prm == prm)
	{
	   ecl->num--;
	   if (ecl->num)
	     {
		for (; i < ecl->num; i++, eci++)
		   *eci = *(eci + 1);
		win->cbl.lst =
		   EREALLOC(EventCallbackItem, win->cbl.lst, ecl->num);
	     }
	   else
	     {
		Efree(win->cbl.lst);
		win->cbl.lst = NULL;
	     }
	   return;
	}
}

void
EventCallbacksProcess(Win win, XEvent * ev)
{
   EventCallbackList  *ecl;
   EventCallbackItem  *eci;
   int                 i;

   if (!win)
      return;

   win->in_use = 1;
   ecl = &win->cbl;
   eci = ecl->lst;
   for (i = 0; i < ecl->num; i++, eci++)
     {
	if (EDebug(EDBUG_TYPE_DISPATCH))
	   Eprintf("EventDispatch: type=%d win=%#lx func=%p prm=%p\n",
		   ev->type, ev->xany.window, eci->func, eci->prm);
	eci->func(win, ev, eci->prm);
	if (win->do_del)
	  {
	     EXidDestroy(win);
	     return;
	  }
     }
   win->in_use = 0;
}

Win
ECreateWindow(Win parent, int x, int y, int w, int h, int saveunder)
{
   Win                 win;
   Window              xwin;
   XSetWindowAttributes attr;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = parent->cmap;
   attr.border_pixel = 0;
/*   attr.background_pixel = 0; */
   attr.background_pixmap = None;
   if ((saveunder == 1) && (Conf.save_under))
      attr.save_under = True;
   else if (saveunder == 2)
      attr.save_under = True;
   else
      attr.save_under = False;

   xwin = XCreateWindow(disp, parent->xwin, x, y, w, h, 0,
			CopyFromParent, InputOutput, CopyFromParent,
			CWOverrideRedirect | CWSaveUnder | CWBackingStore |
			CWColormap | CWBackPixmap | CWBorderPixel, &attr);
   win = EXidSet(xwin, parent, x, y, w, h, parent->depth, parent->visual,
		 parent->cmap);

   return win;
}

#if USE_COMPOSITE
static              Win
ECreateWindowVDC(Win parent, int x, int y, int w, int h,
		 Visual * vis, unsigned int depth, Colormap cmap)
{
   Win                 win;
   Window              xwin;
   XSetWindowAttributes attr;

   attr.background_pixmap = None;
   attr.border_pixel = 0;
   attr.backing_store = NotUseful;
   attr.save_under = False;
   attr.override_redirect = False;
   attr.colormap = cmap;

   xwin = XCreateWindow(disp, parent->xwin, x, y, w, h, 0,
			depth, InputOutput, vis,
			CWOverrideRedirect | CWSaveUnder | CWBackingStore |
			CWColormap | CWBackPixmap | CWBorderPixel, &attr);
   win = EXidSet(xwin, parent, x, y, w, h, depth, vis, cmap);

   return win;
}

Win
ECreateArgbWindow(Win parent, int x, int y, int w, int h, Win cwin)
{
   int                 depth;
   Visual             *vis;
   Colormap            cmap;

   if (cwin && Conf.testing.argb_clients_inherit_attr)
     {
	depth = cwin->depth;
	vis = cwin->visual;
	cmap = cwin->cmap;
     }
   else
     {
	if (!argb_visual)
	  {
	     argb_visual = EVisualFindARGB();
	     argb_cmap =
		XCreateColormap(disp, WinGetXwin(VROOT), argb_visual,
				AllocNone);
	  }
	depth = 32;
	vis = argb_visual;
	cmap = argb_cmap;
     }

   return ECreateWindowVDC(parent, x, y, w, h, vis, depth, cmap);
}

#if USE_GLX
static              Win
ECreateWindowVD(Win parent, int x, int y, int w, int h,
		Visual * vis, unsigned int depth)
{
   Colormap            cmap;

   if (!vis || depth == 0)
      return 0;

   cmap = XCreateColormap(disp, WinGetXwin(VROOT), vis, AllocNone);

   return ECreateWindowVDC(parent, x, y, w, h, vis, depth, cmap);
}
#endif

Win
ECreateObjectWindow(Win parent, int x, int y, int w, int h, int saveunder,
		    int type, Win cwin)
{
   Win                 win;
   int                 argb = 0;

   switch (type)
     {
     default:
     case WIN_TYPE_NO_ARGB:
	break;
     case WIN_TYPE_CLIENT:
	if (Conf.testing.argb_clients || EVisualIsARGB(cwin->visual))
	   argb = 1;
	break;
     case WIN_TYPE_INTERNAL:
	if (Conf.testing.argb_internal_objects)
	   argb = 1;
	break;
#if USE_GLX
     case WIN_TYPE_GLX:	/* Internal GL */
	win =
	   ECreateWindowVD(parent, x, y, w, h, EGlGetVisual(), EGlGetDepth());
	return win;
#endif
     }

   if (argb)
      win = ECreateArgbWindow(parent, x, y, w, h, cwin);
   else
      win = ECreateWindow(parent, x, y, w, h, saveunder);

   return win;
}

#else

Win
ECreateObjectWindow(Win parent, int x, int y, int w, int h, int saveunder,
		    int type __UNUSED__, Win cwin __UNUSED__)
{
   return ECreateWindow(parent, x, y, w, h, saveunder);
}

#endif /* USE_COMPOSITE */

Win
ECreateClientWindow(Win parent, int x, int y, int w, int h)
{
#if USE_COMPOSITE
   if (Conf.testing.argb_internal_clients)
      return ECreateArgbWindow(parent, x, y, w, h, NULL);
#endif

   return ECreateWindow(parent, x, y, w, h, 0);
}

Win
ECreateEventWindow(Win parent, int x, int y, int w, int h)
{
   Win                 win;
   Window              xwin;
   XSetWindowAttributes attr;

   attr.override_redirect = False;

   xwin = XCreateWindow(disp, parent->xwin, x, y, w, h, 0, 0, InputOnly,
			CopyFromParent, CWOverrideRedirect, &attr);
   win = EXidSet(xwin, parent, x, y, w, h, 0, NULL, None);

   return win;
}

#if 0				/* Not used */
/*
 * create a window which will accept the keyboard focus when no other 
 * windows have it
 */
Win
ECreateFocusWindow(Win parent, int x, int y, int w, int h)
{
   Win                 win;
   XSetWindowAttributes attr;

   attr.backing_store = NotUseful;
   attr.override_redirect = False;
   attr.colormap = WinGetCmap(VROOT);
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   attr.event_mask = KeyPressMask | FocusChangeMask;

   Window              xwin, xpar;

   win = XCreateWindow(disp, parent, x, y, w, h, 0, 0, InputOnly,
		       CopyFromParent,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixel | CWBorderPixel | CWEventMask,
		       &attr);

   XSetWindowBackground(disp, win, 0);
   XMapWindow(disp, win);
   XSetInputFocus(disp, win, RevertToParent, CurrentTime);

   return win;
}
#endif

void
EMoveWindow(Win win, int x, int y)
{
   if (!win)
      return;

#if 0
   Eprintf("EMoveWindow: %p %#lx: %d,%d %dx%d -> %d,%d\n",
	   win, win->xwin, win->x, win->y, win->w, win->h, x, y);
#endif
   if ((x == win->x) && (y == win->y))
      return;

   win->x = x;
   win->y = y;

   XMoveWindow(disp, win->xwin, x, y);
}

void
EResizeWindow(Win win, int w, int h)
{
   if (!win)
      return;

   if ((w == win->w) && (h == win->h))
      return;

   WinBgInvalidate(win);
   win->w = w;
   win->h = h;

   XResizeWindow(disp, win->xwin, w, h);
}

void
EMoveResizeWindow(Win win, int x, int y, int w, int h)
{
   if (!win)
      return;

#if 0
   Eprintf("EMoveResizeWindow: %p %#lx: %d,%d %dx%d -> %d,%d %dx%d\n",
	   win, win->xwin, win->x, win->y, win->w, win->h, x, y, w, h);
#endif
   if ((w == win->w) && (h == win->h) && (x == win->x) && (y == win->y))
      return;

   if (w != win->w || h != win->h)
      WinBgInvalidate(win);

   win->x = x;
   win->y = y;
   win->w = w;
   win->h = h;

   XMoveResizeWindow(disp, win->xwin, x, y, w, h);
}

static int
ExDelTree(Win win)
{
   Win                 win2;
   int                 nsub;

   win->do_del = -1;

   nsub = 0;
   for (win2 = win_first; win2; win2 = win2->next)
     {
	if (win2->parent != win)
	   continue;
	ExDelTree(win2);
	nsub++;
     }

   return nsub;
}

void
EDestroyWindow(Win win)
{
   Win                 next;
   int                 nsub;

   if (!win)
      return;

#if DEBUG_XWIN
   Eprintf("EDestroyWindow: %p %#lx\n", win, win->xwin);
#endif
   if (win->parent != None)
     {
	EFreeWindowBackgroundPixmap(win);
	XDestroyWindow(disp, win->xwin);
     }

   /* Mark the ones to be deleted */
   nsub = ExDelTree(win);
   if (nsub == 0)
     {
	/* No children */
	EXidDel(win);
	return;
     }

   /* Delete entire tree */
   for (win = win_first; win; win = next)
     {
	next = win->next;
	if (win->do_del < 0)
	   EXidDel(win);
     }
}

void
EWindowSync(Win win)
{
   Window              rr;
   int                 x, y;
   unsigned int        w, h, bw, depth;

   if (!win)
      return;

   XGetGeometry(disp, win->xwin, &rr, &x, &y, &w, &h, &bw, &depth);
#if 0
   Eprintf("EWindowSync: %p %#lx: %d,%d %dx%d -> %d,%d %dx%d\n",
	   win, win->xwin, win->x, win->y, win->w, win->h, x, y, w, h);
#endif
   win->x = x;
   win->y = y;
   win->w = w;
   win->h = h;
   win->depth = depth;
}

void
EWindowSetGeometry(Win win, int x, int y, int w, int h, int bw)
{
   if (!win)
      return;

   win->x = x;
   win->y = y;
   win->w = w;
   win->h = h;
   win->bw = bw;
}

void
EWindowSetMapped(Win win, int mapped)
{
   if (!win)
      return;

   win->mapped = mapped;
}

Window
EXWindowGetParent(Window xwin)
{
   Window              parent, rt;
   Window             *pch = NULL;
   unsigned int        nch = 0;

   parent = None;
   if (!XQueryTree(disp, xwin, &rt, &parent, &pch, &nch))
      parent = None;
   else if (pch)
      XFree(pch);

   return parent;
}

Win
ECreateWinFromXwin(Window xwin)
{
   Win                 win;
   Window              rr;
   int                 x, y;
   unsigned int        w, h, bw, depth;

   if (!XGetGeometry(disp, xwin, &rr, &x, &y, &w, &h, &bw, &depth))
      return NULL;

   win = EXidCreate();
   if (!win)
      return NULL;

   win->xwin = xwin;
   win->x = x;
   win->y = y;
   win->w = w;
   win->h = h;
   win->depth = depth;
   win->visual = WinGetVisual(VROOT);
   win->cmap = WinGetCmap(VROOT);
#if DEBUG_XWIN
   Eprintf("EXidWFX: %p %#lx\n", win, win->xwin);
#endif

   return win;
}

void
EDestroyWin(Win win)
{
   EXidDestroy(win);
}

Win
ERegisterWindow(Window xwin, XWindowAttributes * pxwa)
{
   Win                 win;
   XWindowAttributes   xwa;

   win = EXidLookup(xwin);
   if (win)
      goto done;

   if (!pxwa)
     {
	pxwa = &xwa;
	if (!XGetWindowAttributes(disp, xwin, pxwa))
	   goto done;
     }

#if 0
   Eprintf("ERegisterWindow %#lx %d+%d %dx%d\n", win, x, y, w, h);
#endif
   win = EXidSet(xwin, None, pxwa->x, pxwa->y, pxwa->width, pxwa->height,
		 pxwa->depth, pxwa->visual, pxwa->colormap);
   win->mapped = pxwa->map_state != IsUnmapped;
   win->attached = 1;

 done:
   return win;
}

void
EUnregisterXwin(Window xwin)
{
   Win                 win;

   win = EXidLookup(xwin);
   if (!win)
      return;

   /* FIXME - We shouldn't go here */
   EXidDel(win);
#if 1				/* Debug - Fix code if we get here */
   Eprintf("*** FIXME - EUnregisterXwin %#lx\n", xwin);
#endif
}

void
EUnregisterWindow(Win win)
{
   if (!win)
      return;

   if (win->cbl.lst)
     {
	if (EDebug(1))
	   Eprintf("EUnregisterWindow(%#lx) Ignored (%d callbacks remain)\n",
		   win->xwin, win->cbl.num);
	return;
     }

   EXidDel(win);
}

void
EMapWindow(Win win)
{
   if (!win)
      return;

   if (win->mapped)
      return;
   win->mapped = 1;

   XMapWindow(disp, win->xwin);
}

void
EUnmapWindow(Win win)
{
   if (!win)
      return;

   if (!win->mapped)
      return;
   win->mapped = 0;

   XUnmapWindow(disp, win->xwin);
}

void
EReparentWindow(Win win, Win parent, int x, int y)
{
   if (!win)
      return;

#if 0
   Eprintf
      ("EReparentWindow: %p %#lx: %d %#lx->%#lx %d,%d %dx%d -> %d,%d\n",
       win, win->xwin, win->mapped, (win->parent) ? win->parent->xwin : None,
       parent->xwin, win->x, win->y, win->w, win->h, x, y);
#endif
   if (parent == win->parent)
     {
	if ((x != win->x) || (y != win->y))
	  {
	     win->x = x;
	     win->y = y;
	     XMoveWindow(disp, win->xwin, x, y);
	  }
	return;
     }
   else
     {
	win->parent = parent;
	win->x = x;
	win->y = y;
     }

   XReparentWindow(disp, win->xwin, parent->xwin, x, y);
}

void
EMapRaised(Win win)
{
   if (!win)
      return;

   if (win->mapped)
     {
	XRaiseWindow(disp, win->xwin);
	return;
     }
   else
     {
	win->mapped = 1;
     }

   XMapRaised(disp, win->xwin);
}

int
EXGetGeometry(Drawable draw, Window * root_return, int *x, int *y,
	      int *w, int *h, int *bw, int *depth)
{
   int                 ok;
   Window              rr;
   int                 xx, yy;
   unsigned int        ww, hh, bb, dd;

   ok = XGetGeometry(disp, draw, &rr, &xx, &yy, &ww, &hh, &bb, &dd);
   if (!ok)
      goto done;

   if (root_return)
      *root_return = rr;
   if (x)
      *x = xx;
   if (y)
      *y = yy;
   if (w)
      *w = ww;
   if (h)
      *h = hh;
   if (bw)
      *bw = bb;
   if (depth)
      *depth = dd;

 done:
#if 0				/* Debug */
   if (!ok)
      Eprintf("EGetGeometry win=%#x, error %d\n", (unsigned)win, ok);
#endif
   return ok;
}

int
EGetGeometry(Win win, Window * root_return, int *x, int *y,
	     int *w, int *h, int *bw, int *depth)
{
   if (!win)
      return 0;

   if (x)
      *x = win->x;
   if (y)
      *y = win->y;
   if (w)
      *w = win->w;
   if (h)
      *h = win->h;
   if (bw)
      *bw = 0;
   if (depth)
      *depth = win->depth;
   if (root_return)
      *root_return = WinGetXwin(VROOT);

   return 1;
}

void
EGetWindowAttributes(Win win, XWindowAttributes * pxwa)
{
   if (!win)
      return;

   pxwa->x = win->x;
   pxwa->y = win->y;
   pxwa->width = win->w;
   pxwa->height = win->h;
   pxwa->border_width = win->bw;
   pxwa->depth = win->depth;
   pxwa->visual = win->visual;
   pxwa->colormap = win->cmap;
}

#if 0				/* Unused */
void
EConfigureWindow(Win win, unsigned int mask, XWindowChanges * wc)
{
   char                doit = 0;

   if (!win)
      return;

   if ((mask & CWX) && (wc->x != win->x))
     {
	win->x = wc->x;
	doit = 1;
     }
   if ((mask & CWY) && (wc->y != win->y))
     {
	win->y = wc->y;
	doit = 1;
     }
   if ((mask & CWWidth) && (wc->width != win->w))
     {
	WinBgInvalidate(win);
	win->w = wc->width;
	doit = 1;
     }
   if ((mask & CWHeight) && (wc->height != win->h))
     {
	WinBgInvalidate(win);
	win->h = wc->height;
	doit = 1;
     }

   if ((doit) || (mask & (CWBorderWidth | CWSibling | CWStackMode)))
      XConfigureWindow(disp, win->xwin, mask, wc);
}
#endif

void
ESetWindowBackgroundPixmap(Win win, Pixmap pmap)
{
   if (!win)
      return;

   if (win->bgpmap && win->bg_owned)
      EFreeWindowBackgroundPixmap(win);
   win->bgpmap = pmap;
   win->bg_owned = 0;		/* Don't manage pixmap */
   win->bgcol = 0xffffffff;	/* Hmmm.. */

   XSetWindowBackgroundPixmap(disp, win->xwin, pmap);
}

Pixmap
EGetWindowBackgroundPixmap(Win win)
{
   Pixmap              pmap;

   if (!win)
      return None;

   if (win->bg_owned < 0)	/* Free if invalidated */
      EFreeWindowBackgroundPixmap(win);
   else if (win->bgpmap)
      return win->bgpmap;

   /* Allocate/set new */
   pmap = ECreatePixmap(win, win->w, win->h, 0);
   ESetWindowBackgroundPixmap(win, pmap);
   win->bg_owned = 1;		/* Manage pixmap */

   return pmap;
}

void
EFreeWindowBackgroundPixmap(Win win)
{
   if (!win || !win->bgpmap)
      return;

   if (win->bg_owned)
      EFreePixmap(win->bgpmap);
   win->bgpmap = 0;
   win->bg_owned = 0;
}

void
ESetWindowBackground(Win win, unsigned int col)
{
   if (!win)
      return;

   if (win->bgpmap)
     {
	EFreeWindowBackgroundPixmap(win);
	win->bgcol = col;
     }
   else if (win->bgcol != col)
     {
	win->bgcol = col;
     }
   else
      return;

   XSetWindowBackground(disp, win->xwin, col);
}

void
ESelectInput(Win win, unsigned int event_mask)
{
#if USE_XI2

#define EVENTS_TO_XI \
  (/* KeyPressMask | KeyReleaseMask | */ \
   ButtonPressMask | ButtonReleaseMask | PointerMotionMask)

   win->event_mask = event_mask;

   if (Mode.server.extensions & XEXT_XI)
     {
	XIEventMask         em;
	unsigned char       mask[(XI_LASTEVENT + 8) / 8];

	em.deviceid = XIAllMasterDevices;	/* XIAllDevices; */
	em.mask_len = sizeof(mask);
	em.mask = mask;
	memset(mask, 0, sizeof(mask));
#if 0
	if (event_mask & KeyPressMask)
	   XISetMask(mask, XI_KeyPress);
	if (event_mask & KeyReleaseMask)
	   XISetMask(mask, XI_KeyRelease);
#endif
	if (event_mask & ButtonPressMask)
	   XISetMask(mask, XI_ButtonPress);
	if (event_mask & ButtonReleaseMask)
	   XISetMask(mask, XI_ButtonRelease);
	if (event_mask & PointerMotionMask)
	   XISetMask(mask, XI_Motion);
	XISelectEvents(disp, win->xwin, &em, 1);
	event_mask &= ~EVENTS_TO_XI;
     }
#endif

   XSelectInput(disp, win->xwin, event_mask);
}

void
ESelectInputChange(Win win, unsigned int set, unsigned int clear)
{
#if USE_XI2
   win->event_mask |= set;
   win->event_mask &= ~clear;
   ESelectInput(win, win->event_mask);
#else
   XWindowAttributes   xwa;

   XGetWindowAttributes(disp, win->xwin, &xwa);
   xwa.your_event_mask |= set;
   xwa.your_event_mask &= ~clear;
   XSelectInput(disp, win->xwin, xwa.your_event_mask);
#endif
}

void
EChangeWindowAttributes(Win win, unsigned int mask, XSetWindowAttributes * attr)
{
   XChangeWindowAttributes(disp, win->xwin, mask, attr);
}

void
ESetWindowBorderWidth(Win win, unsigned int bw)
{
   XSetWindowBorderWidth(disp, win->xwin, bw);
}

void
ERaiseWindow(Win win)
{
   XRaiseWindow(disp, win->xwin);
}

void
ELowerWindow(Win win)
{
   XLowerWindow(disp, win->xwin);
}

void
EXRestackWindows(Window * windows, int nwindows)
{
   XRestackWindows(disp, windows, nwindows);
}

void
EClearWindow(Win win)
{
   XClearWindow(disp, win->xwin);
}

void
EClearArea(Win win, int x, int y, unsigned int w, unsigned int h)
{
   XClearArea(disp, win->xwin, x, y, w, h, False);
}

int
ETranslateCoordinates(Win src_w, Win dst_w, int src_x, int src_y,
		      int *dest_x_return, int *dest_y_return,
		      Window * child_return)
{
   Window              child;

   if (!child_return)
      child_return = &child;

   return XTranslateCoordinates(disp, src_w->xwin, dst_w->xwin, src_x, src_y,
				dest_x_return, dest_y_return, child_return);
}

void
EXWarpPointer(Window xwin, int x, int y)
{
   XWarpPointer(disp, None, xwin, 0, 0, 0, 0, x, y);
}

void
EWarpPointer(Win win, int x, int y)
{
   EXWarpPointer(win ? win->xwin : None, x, y);
}

Bool
EXQueryPointer(Window xwin, int *px, int *py, Window * pchild,
	       unsigned int *pmask)
{
   Window              root, child;
   int                 root_x, root_y;
   unsigned int        mask;

   if (xwin == None)
      xwin = WinGetXwin(VROOT);

   if (!px)
      px = &root_x;
   if (!py)
      py = &root_y;
   if (!pchild)
      pchild = &child;
   if (!pmask)
      pmask = &mask;

   return XQueryPointer(disp, xwin, &root, pchild, &root_x, &root_y, px, py,
			pmask);
}

Bool
EQueryPointer(Win win, int *px, int *py, Window * pchild, unsigned int *pmask)
{
   Window              xwin;

   xwin = (win) ? win->xwin : WinGetXwin(VROOT);

   return EXQueryPointer(xwin, px, py, pchild, pmask);
}

int
EDrawableCheck(Drawable draw, int grab)
{
   int                 ok;

   if (draw == None)
      return 0;

   if (grab)
      EGrabServer();

   ok = EXGetGeometry(draw, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

   if (grab && !ok)
      EUngrabServer();

   return ok;
}

KeyCode
EKeysymToKeycode(KeySym keysym)
{
   return XKeysymToKeycode(disp, keysym);
}

KeyCode
EKeynameToKeycode(const char *name)
{
   return XKeysymToKeycode(disp, XStringToKeysym(name));
}

Atom
EInternAtom(const char *name)
{
   return XInternAtom(disp, name, False);
}

#define DEBUG_SHAPE_OPS 0
#define DEBUG_SHAPE_PROPAGATE 0

#if DEBUG_SHAPE_OPS || DEBUG_SHAPE_PROPAGATE
static void
EShapeShow(const char *txt, Window xwin, XRectangle * pr, int nr)
{
   int                 i;

   Eprintf("%s %#lx nr=%d\n", txt, xwin, nr);
   for (i = 0; i < nr; i++)
      Eprintf(" %d - %4d,%4d %4dx%4d\n", i,
	      pr[i].x, pr[i].y, pr[i].width, pr[i].height);
}
#endif

int
EShapeUpdate(Win win)
{
   if (win->rects)
     {
	XFree(win->rects);
	win->num_rect = 0;
     }

   win->rects =
      XShapeGetRectangles(disp, win->xwin, ShapeBounding, &(win->num_rect),
			  &(win->ord));
   if (win->rects)
     {
	if (win->num_rect == 1)
	  {
	     if ((win->rects[0].x == 0) && (win->rects[0].y == 0)
		 && (win->rects[0].width == win->w)
		 && (win->rects[0].height == win->h))
	       {
		  win->num_rect = 0;
		  XFree(win->rects);
		  win->rects = NULL;
		  XShapeCombineMask(disp, win->xwin, ShapeBounding, 0, 0,
				    None, ShapeSet);
	       }
	  }
	else if (win->num_rect > 4096)
	  {
	     Eprintf("*** EShapeUpdate: nrect=%d - Not likely, ignoring.\n",
		     win->num_rect);
	     XShapeCombineMask(disp, win->xwin, ShapeBounding, 0, 0, None,
			       ShapeSet);
	     win->num_rect = 0;
	     XFree(win->rects);
	     win->rects = NULL;
	  }
     }
   else
     {
	win->num_rect = -1;
     }
#if DEBUG_SHAPE_OPS
   EShapeShow("EShapeUpdate", win->xwin, win->rects, win->num_rect);
#endif
   return win->num_rect != 0;
}

static void
EShapeCombineMask(Win win, int dest, int x, int y, Pixmap pmap, int op)
{
   char                wasshaped = 0;

   if (!win)
      return;

   if (win->rects || win->num_rect < 0)
     {
	win->num_rect = 0;
	if (win->rects)
	   XFree(win->rects);
	win->rects = NULL;
	wasshaped = 1;
     }
#if DEBUG_SHAPE_OPS
   Eprintf("EShapeCombineMask %#lx %d,%d %dx%d mask=%#lx wassh=%d\n",
	   win->xwin, win->x, win->y, win->w, win->h, pmap, wasshaped);
#endif
   if (pmap)
     {
	XShapeCombineMask(disp, win->xwin, dest, x, y, pmap, op);
	EShapeUpdate(win);
     }
   else if (wasshaped)
      XShapeCombineMask(disp, win->xwin, dest, x, y, pmap, op);
}

static void
EShapeCombineMaskTiled(Win win, int dest, int x, int y,
		       Pixmap pmap, int op, int w, int h)
{
   XGCValues           gcv;
   GC                  gc;
   Window              tm;

   gcv.fill_style = FillTiled;
   gcv.tile = pmap;
   gcv.ts_x_origin = 0;
   gcv.ts_y_origin = 0;
   tm = ECreatePixmap(win, w, h, 1);
   gc = EXCreateGC(tm, GCFillStyle | GCTile |
		   GCTileStipXOrigin | GCTileStipYOrigin, &gcv);
   XFillRectangle(disp, tm, gc, 0, 0, w, h);
   EXFreeGC(gc);
   EShapeCombineMask(win, dest, x, y, tm, op);
   EFreePixmap(tm);
}

static void
EShapeCombineRectangles(Win win, int dest, int x, int y,
			XRectangle * rect, int n_rects, int op, int ordering)
{
   if (!win)
      return;
#if DEBUG_SHAPE_OPS
   Eprintf("EShapeCombineRectangles %#lx %d\n", win->xwin, n_rects);
#endif

   if (n_rects == 1 && op == ShapeSet)
     {
	if ((rect[0].x == 0) && (rect[0].y == 0) &&
	    (rect[0].width == win->w) && (rect[0].height == win->h))
	  {
	     win->num_rect = 0;
	     XFree(win->rects);
	     win->rects = NULL;
	     XShapeCombineMask(disp, win->xwin, dest, x, y, None, op);
	     return;
	  }
     }
   XShapeCombineRectangles(disp, win->xwin, dest, x, y, rect, n_rects, op,
			   ordering);
   if (n_rects > 1)
     {
	/* Limit shape to window extents */
	XRectangle          r;

	r.x = r.y = 0;
	r.width = win->w;
	r.height = win->h;
	XShapeCombineRectangles(disp, win->xwin, ShapeBounding, 0, 0, &r,
				1, ShapeIntersect, Unsorted);
     }
   EShapeUpdate(win);
}

static void
EShapeCombineShape(Win win, int dest, int x, int y,
		   Win src_win, int src_kind, int op)
{
   if (!win)
      return;

   XShapeCombineShape(disp, win->xwin, dest, x, y, src_win->xwin, src_kind, op);
   EShapeUpdate(win);
}

int
EShapePropagate(Win win)
{
   Win                 xch;
   unsigned int        num_rects;
   int                 k, rn;
   int                 x, y, w, h;
   XRectangle         *rects, *rectsn, *rl;

   if (!win || win->w <= 0 || win->h <= 0)
      return 0;

#if DEBUG_SHAPE_PROPAGATE
   Eprintf("EShapePropagate %#lx %d,%d %dx%d\n", win->xwin,
	   win->x, win->y, win->w, win->h);
#endif

   num_rects = 0;
   rects = NULL;

   /* go through all child windows and create/inset spans */
   for (xch = win_first; xch; xch = xch->next)
     {
	if (xch->parent != win)
	   continue;

#if DEBUG_SHAPE_PROPAGATE > 1
	Eprintf("%#lx(%d): %4d,%4d %4dx%4d\n", xch->xwin, xch->mapped,
		xch->x, xch->y, xch->w, xch->h);
#endif
	if (!xch->mapped)
	   continue;

	x = xch->x;
	y = xch->y;
	w = xch->w;
	h = xch->h;
	if (x >= win->w || y >= win->h || x + w < 0 || y + h < 0)
	   continue;

	rn = xch->num_rect;

	if (rn > 0)
	  {
	     rl = xch->rects;
	     rectsn = EREALLOC(XRectangle, rects, num_rects + rn);
	     if (!rectsn)
		goto bail_out;
	     rects = rectsn;

	     /* go through all clip rects in thsi window's shape */
	     for (k = 0; k < rn; k++)
	       {
		  /* for each clip rect, add it to the rect list */
		  rects[num_rects + k].x = x + rl[k].x;
		  rects[num_rects + k].y = y + rl[k].y;
		  rects[num_rects + k].width = rl[k].width;
		  rects[num_rects + k].height = rl[k].height;
#if DEBUG_SHAPE_PROPAGATE > 1
		  Eprintf(" - %d: %4d,%4d %4dx%4d\n", k,
			  rects[num_rects + k].x,
			  rects[num_rects + k].y, rects[num_rects + k].width,
			  rects[num_rects + k].height);
#endif
	       }
	     num_rects += rn;
	  }
	else if (rn == 0)
	  {
	     /* Unshaped */
	     rectsn = EREALLOC(XRectangle, rects, num_rects + 1);
	     if (!rectsn)
		goto bail_out;
	     rects = rectsn;

	     rects[num_rects].x = x;
	     rects[num_rects].y = y;
	     rects[num_rects].width = w;
	     rects[num_rects].height = h;
	     num_rects++;
	  }
     }

#if DEBUG_SHAPE_PROPAGATE
   EShapeShow("EShapePropagate", win->xwin, rects, num_rects);
#endif

   /* set the rects as the shape mask */
   if (rects)
     {
	EShapeCombineRectangles(win, ShapeBounding, 0, 0, rects,
				num_rects, ShapeSet, Unsorted);
	Efree(rects);
     }
   else
     {
	/* Empty shape */
	EShapeCombineRectangles(win, ShapeBounding, 0, 0, NULL, 0, ShapeSet,
				Unsorted);
     }

   return win->num_rect;

 bail_out:
   Efree(rects);
   EShapeCombineMask(win, ShapeBounding, 0, 0, None, ShapeSet);
   return 0;
}

int
EShapeCheck(Win win)
{
   if (!win)
      return 0;

   return win->num_rect;
}

void
EShapeSetMask(Win win, int x, int y, Pixmap mask)
{
   EShapeCombineMask(win, ShapeBounding, x, y, mask, ShapeSet);
}

void
EShapeUnionMask(Win win, int x, int y, Pixmap mask)
{
   EShapeCombineMask(win, ShapeBounding, x, y, mask, ShapeUnion);
}

void
EShapeSetMaskTiled(Win win, int x, int y, Pixmap mask, int w, int h)
{
   EShapeCombineMaskTiled(win, ShapeBounding, x, y, mask, ShapeSet, w, h);
}

void
EShapeSetRects(Win win, int x, int y, XRectangle * rect, int n_rects)
{
   EShapeCombineRectangles(win, ShapeBounding, x, y, rect, n_rects,
			   ShapeSet, Unsorted);
}

void
EShapeUnionRects(Win win, int x, int y, XRectangle * rect, int n_rects)
{
   EShapeCombineRectangles(win, ShapeBounding, x, y, rect, n_rects,
			   ShapeUnion, Unsorted);
}

int
EShapeSetShape(Win win, int x, int y, Win src_win)
{
   EShapeCombineShape(win, ShapeBounding, x, y,
		      src_win, ShapeBounding, ShapeSet);
   return win->num_rect != 0;
}

static              Pixmap
_EWindowGetShapePixmap(Win win, unsigned int fg, unsigned int bg)
{
   Pixmap              mask;
   GC                  gc;
   int                 i;
   const XRectangle   *rect;

   if (win->num_rect == 0)	/* Not shaped */
      return None;

   mask = ECreatePixmap(win, win->w, win->h, 1);
   gc = EXCreateGC(mask, 0, NULL);

   XSetForeground(disp, gc, bg);
   XFillRectangle(disp, mask, gc, 0, 0, win->w, win->h);

   XSetForeground(disp, gc, fg);
   rect = win->rects;
   for (i = 0; i < win->num_rect; i++)
      XFillRectangle(disp, mask, gc, rect[i].x, rect[i].y,
		     rect[i].width, rect[i].height);

   EXFreeGC(gc);

   return mask;
}

/* Build mask from window shape rects */
Pixmap
EWindowGetShapePixmap(Win win)
{
   return _EWindowGetShapePixmap(win, 1, 0);
}

/* Build inverted mask from window shape rects */
Pixmap
EWindowGetShapePixmapInverted(Win win)
{
   return _EWindowGetShapePixmap(win, 0, 1);
}

Pixmap
ECreatePixmap(Win win, unsigned int width, unsigned int height,
	      unsigned int depth)
{
   Pixmap              pmap;

   if (depth == 0)
      depth = win->depth;

   pmap = XCreatePixmap(disp, win->xwin, width, height, depth);
#if DEBUG_PIXMAP
   Eprintf("%s: %#lx\n", __func__, pmap);
#endif
   return pmap;
}

void
EFreePixmap(Pixmap pmap)
{
#if DEBUG_PIXMAP
   Eprintf("%s: %#lx\n", __func__, pmap);
#endif
   XFreePixmap(disp, pmap);
}

Pixmap
EXCreatePixmapCopy(Pixmap src, unsigned int w, unsigned int h,
		   unsigned int depth)
{
   Pixmap              pmap;
   GC                  gc;

   pmap = XCreatePixmap(disp, src, w, h, depth);
   gc = EXCreateGC(src, 0, NULL);
   XCopyArea(disp, src, pmap, gc, 0, 0, w, h, 0, 0);
   EXFreeGC(gc);
#if DEBUG_PIXMAP
   Eprintf("%s: %#lx\n", __func__, pmap);
#endif
   return pmap;
}

void
EXCopyArea(Drawable src, Drawable dst, int sx, int sy, unsigned int w,
	   unsigned int h, int dx, int dy)
{
   GC                  gc;

   gc = EXCreateGC(src, 0, NULL);
   XCopyArea(disp, src, dst, gc, sx, sy, w, h, dx, dy);
   EXFreeGC(gc);
}

void
EXCopyAreaTiled(Drawable src, Pixmap mask, Drawable dst, int sx, int sy,
		unsigned int w, unsigned int h, int dx, int dy)
{
   GC                  gc;
   XGCValues           gcv;

   gcv.fill_style = FillTiled;
   gcv.tile = src;
   gcv.ts_x_origin = sx;
   gcv.ts_y_origin = sy;
   gcv.clip_mask = mask;
   gc = EXCreateGC(dst, GCFillStyle |
		   GCTile | GCTileStipXOrigin | GCTileStipYOrigin | GCClipMask,
		   &gcv);
   XFillRectangle(disp, dst, gc, dx, dy, w, h);
   EXFreeGC(gc);
}

GC
EXCreateGC(Drawable draw, unsigned int mask, XGCValues * val)
{
   XGCValues           xgcv;

   if (val)
     {
	mask |= GCGraphicsExposures;
	val->graphics_exposures = False;
     }
   else
     {
	mask = GCGraphicsExposures;
	val = &xgcv;
	val->graphics_exposures = False;
     }
   return XCreateGC(disp, draw, mask, val);
}

void
EXFreeGC(GC gc)
{
   if (gc)
      XFreeGC(disp, gc);
}

void
EXSendEvent(Window xwin, unsigned int event_mask, XEvent * ev)
{
   XSendEvent(disp, xwin, False, event_mask, ev);
}

unsigned int
EAllocColor(Colormap cmap, unsigned int argb)
{
   XColor              xc;

   COLOR32_TO_RGB16(argb, xc.red, xc.green, xc.blue);
   XAllocColor(disp, cmap, &xc);

   return xc.pixel;
}

/*
 * Display
 */

int
EDisplayOpen(const char *dstr, int scr)
{
   char                dbuf[256], *s;
   unsigned int        ddpy, dscr;

   if (!dstr)
      goto do_open;

   Esnprintf(dbuf, sizeof(dbuf), "%s", dstr);
   s = strchr(dbuf, ':');
   if (!s)
      return -1;
   s++;

   ddpy = dscr = 0;
   sscanf(s, "%u.%u", &ddpy, &dscr);
   if (scr >= 0)		/* Override screen */
      dscr = scr;
   Esnprintf(s, sizeof(dbuf) - (s - dbuf), "%u.%u", ddpy, dscr);
   dstr = dbuf;

 do_open:
#ifdef USE_ECORE_X
   ecore_x_init(dstr);
   disp = ecore_x_display_get();
#else
   disp = XOpenDisplay(dstr);
#endif

   return (disp) ? 0 : -1;
}

void
EDisplayClose(void)
{
   if (!disp)
      return;
#ifdef USE_ECORE_X
   ecore_x_shutdown();
#else
   XCloseDisplay(disp);
#endif
   XSetErrorHandler(NULL);
   XSetIOErrorHandler(NULL);
   disp = NULL;
}

void
EDisplayDisconnect(void)
{
   if (!disp)
      return;
#ifdef USE_ECORE_X
   ecore_x_disconnect();
#else
   close(ConnectionNumber(disp));
#endif
   XSetErrorHandler(NULL);
   XSetIOErrorHandler(NULL);

   disp = NULL;
}

static EXErrorHandler *EXErrorFunc = NULL;
static EXIOErrorHandler *EXIOErrorFunc = NULL;

static int
HandleXError(Display * dpy __UNUSED__, XErrorEvent * ev)
{
   if (EDebug(1) && EXErrorFunc)
      EXErrorFunc(ev);

   Dpy.last_error_code = ev->error_code;

   return 0;
}

static int
HandleXIOError(Display * dpy __UNUSED__)
{
   disp = NULL;

   if (EXIOErrorFunc)
      EXIOErrorFunc();

   return 0;
}

void
EDisplaySetErrorHandlers(EXErrorHandler * error, EXIOErrorHandler * fatal)
{
   /* set up an error handler for then E would normally have fatal X errors */
   EXErrorFunc = error;
   XSetErrorHandler(HandleXError);

   /* set up a handler for when the X Connection goes down */
   EXIOErrorFunc = fatal;
   XSetIOErrorHandler(HandleXIOError);
}

/*
 * Server
 */

void
EGrabServer(void)
{
   if (Dpy.server_grabbed <= 0)
     {
	if (EDebug(EDBUG_TYPE_GRABS))
	   Eprintf("EGrabServer\n");
	XGrabServer(disp);
     }
   Dpy.server_grabbed++;
}

void
EUngrabServer(void)
{
   if (Dpy.server_grabbed == 1)
     {
	XUngrabServer(disp);
	XFlush(disp);
	if (EDebug(EDBUG_TYPE_GRABS))
	   Eprintf("EUngrabServer\n");
     }
   Dpy.server_grabbed--;
   if (Dpy.server_grabbed < 0)
      Dpy.server_grabbed = 0;
}

int
EServerIsGrabbed(void)
{
   return Dpy.server_grabbed;
}

void
EFlush(void)
{
   XFlush(disp);
}

void
ESync(unsigned int mask)
{
   if (mask & Conf.testing.no_sync_mask)
      return;
   XSync(disp, False);
}

/*
 * Visuals
 */

#if USE_XRENDER

Visual             *
EVisualFindARGB(void)
{
   XVisualInfo        *xvi, xvit;
   int                 i, num;
   Visual             *vis;

   xvit.screen = Dpy.screen;
   xvit.depth = 32;
#if __cplusplus
   xvit.c_class = TrueColor;
#else
   xvit.class = TrueColor;
#endif

   xvi = XGetVisualInfo(disp,
			VisualScreenMask | VisualDepthMask | VisualClassMask,
			&xvit, &num);
   if (!xvi)
      return NULL;

   for (i = 0; i < num; i++)
     {
	if (EVisualIsARGB(xvi[i].visual))
	   break;
     }

   vis = (i < num) ? xvi[i].visual : NULL;

   XFree(xvi);

   return vis;
}

int
EVisualIsARGB(Visual * vis)
{
   XRenderPictFormat  *pictfmt;

   pictfmt = XRenderFindVisualFormat(disp, vis);
   if (!pictfmt)
      return 0;

#if 0
   Eprintf("Visual ID=%#lx Type=%d, alphamask=%d\n", vis->visualid,
	   pictfmt->type, pictfmt->direct.alphaMask);
#endif
   return pictfmt->type == PictTypeDirect && pictfmt->direct.alphaMask;
}

#endif /* USE_XRENDER */

/*
 * Misc
 */

Time
EGetTimestamp(void)
{
   static Window       win_ts = None;
   XSetWindowAttributes attr;
   XEvent              ev;

   if (win_ts == None)
     {
	attr.override_redirect = False;
	win_ts = XCreateWindow(disp, WinGetXwin(VROOT), -100, -100, 1, 1, 0,
			       CopyFromParent, InputOnly, CopyFromParent,
			       CWOverrideRedirect, &attr);
	XSelectInput(disp, win_ts, PropertyChangeMask);
     }

   XChangeProperty(disp, win_ts, XA_WM_NAME, XA_STRING, 8,
		   PropModeAppend, (unsigned char *)"", 0);
   XWindowEvent(disp, win_ts, PropertyChangeMask, &ev);

   return ev.xproperty.time;
}

#if USE_COMPOSITE

#include <X11/extensions/Xcomposite.h>

Pixmap
EWindowGetPixmap(const Win win)
{
   XWindowAttributes   xwa;

   if (XGetWindowAttributes(disp, win->xwin, &xwa) == 0 ||
       xwa.map_state == IsUnmapped)
      return None;

   return XCompositeNameWindowPixmap(disp, WinGetXwin(win));
}

#endif /* USE_COMPOSITE */

#if USE_XRENDER

/*
 * Pictures
 */
#define _R(x) (((x) >> 16) & 0xff)
#define _G(x) (((x) >>  8) & 0xff)
#define _B(x) (((x)      ) & 0xff)

Picture
EPictureCreate(Win win, Drawable draw)
{
   Picture             pict;
   XRenderPictFormat  *pictfmt;

   if (!win)
      win = VROOT;
   pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(win));
   pict = XRenderCreatePicture(disp, draw, pictfmt, 0, 0);

   return pict;
}

Picture
EPictureCreateSolid(Window xwin, int argb, unsigned int a, unsigned int rgb)
{
   Display            *dpy = disp;
   Pixmap              pmap;
   Picture             pict;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;
   XRenderColor        c;

   pmap = XCreatePixmap(dpy, xwin, 1, 1, argb ? 32 : 8);
   pictfmt = XRenderFindStandardFormat(dpy,
				       argb ? PictStandardARGB32 :
				       PictStandardA8);
   pa.repeat = True;
   pict = XRenderCreatePicture(dpy, pmap, pictfmt, CPRepeat, &pa);

   c.alpha = (unsigned short)(a * 0x101);
   c.red = (unsigned short)(_R(rgb) * 0x101);
   c.green = (unsigned short)(_G(rgb) * 0x101);
   c.blue = (unsigned short)(_B(rgb) * 0x101);
   XRenderFillRectangle(dpy, PictOpSrc, pict, &c, 0, 0, 1, 1);

   XFreePixmap(dpy, pmap);

   return pict;
}

Picture
EPictureCreateBuffer(Win win, int w, int h, Pixmap * ppmap)
{
   Picture             pict;
   Pixmap              pmap;
   XRenderPictFormat  *pictfmt;

   pmap = XCreatePixmap(disp, WinGetXwin(win), w, h, WinGetDepth(win));
   pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(win));
   pict = XRenderCreatePicture(disp, pmap, pictfmt, 0, 0);
   if (ppmap)
      *ppmap = pmap;
   else
      XFreePixmap(disp, pmap);

   return pict;
}

void
EPictureDestroy(Picture pict)
{
   XRenderFreePicture(disp, pict);
}

#endif /* USE_XRENDER */

#if USE_COMPOSITE

void
EPictureSetClip(Picture pict, XserverRegion clip)
{
   XFixesSetPictureClipRegion(disp, pict, 0, 0, clip);
}

/*
 * Regions
 */
#define DEBUG_REGIONS 0

#if DEBUG_REGIONS
static int          n_rgn_c = 0;
static int          n_rgn_d = 0;
#endif

XserverRegion
ERegionCreate(void)
{
   XserverRegion       rgn;

   rgn = XFixesCreateRegion(disp, NULL, 0);

#if DEBUG_REGIONS
   n_rgn_c++;
   Eprintf("%s: %#lx %d %d %d\n", __func__, rgn,
	   n_rgn_c - n_rgn_d, n_rgn_c, n_rgn_d);
#endif
   return rgn;
}

XserverRegion
ERegionCreateRect(int x, int y, int w, int h)
{
   XserverRegion       rgn;
   XRectangle          rct;

   rct.x = x;
   rct.y = y;
   rct.width = w;
   rct.height = h;
   rgn = XFixesCreateRegion(disp, &rct, 1);

#if DEBUG_REGIONS
   n_rgn_c++;
   Eprintf("%s: %#lx %d %d %d\n", __func__, rgn,
	   n_rgn_c - n_rgn_d, n_rgn_c, n_rgn_d);
#endif
   return rgn;
}

#if USE_DESK_EXPOSE
XserverRegion
ERegionCreateFromRects(XRectangle * rectangles, int nrectangles)
{
   XserverRegion       rgn;

   rgn = XFixesCreateRegion(disp, rectangles, nrectangles);

#if DEBUG_REGIONS
   n_rgn_c++;
   Eprintf("%s: %#lx %d %d %d\n", __func__, rgn,
	   n_rgn_c - n_rgn_d, n_rgn_c, n_rgn_d);
#endif
   return rgn;
}
#endif

XserverRegion
ERegionCreateFromWindow(Win win)
{
   XserverRegion       rgn;

   rgn =
      XFixesCreateRegionFromWindow(disp, WinGetXwin(win), WindowRegionBounding);

#if DEBUG_REGIONS
   n_rgn_c++;
   Eprintf("%s: %#lx %d %d %d\n", __func__, rgn,
	   n_rgn_c - n_rgn_d, n_rgn_c, n_rgn_d);
#endif
   return rgn;
}

XserverRegion
ERegionCopy(XserverRegion rgn, XserverRegion src)
{
   XFixesCopyRegion(disp, rgn, src);
   return rgn;
}

XserverRegion
ERegionClone(XserverRegion src)
{
   XserverRegion       rgn;

   rgn = ERegionCreate();
   ERegionCopy(rgn, src);

   return rgn;
}

void
ERegionDestroy(XserverRegion rgn)
{
#if DEBUG_REGIONS
   n_rgn_d++;
   Eprintf("%s: %#lx %d %d %d\n", __func__, rgn,
	   n_rgn_c - n_rgn_d, n_rgn_c, n_rgn_d);
#endif
   XFixesDestroyRegion(disp, rgn);
}

void
ERegionEmpty(XserverRegion rgn)
{
   XFixesSetRegion(disp, rgn, NULL, 0);
}

void
ERegionSetRect(XserverRegion rgn, int x, int y, int w, int h)
{
   XRectangle          rct;

   rct.x = x;
   rct.y = y;
   rct.width = w;
   rct.height = h;
   XFixesSetRegion(disp, rgn, &rct, 1);
}

void
ERegionTranslate(XserverRegion rgn, int dx, int dy)
{
   if (dx == 0 && dy == 0)
      return;
   XFixesTranslateRegion(disp, rgn, dx, dy);
}

void
ERegionIntersect(XserverRegion dst, XserverRegion src)
{
   XFixesIntersectRegion(disp, dst, dst, src);
}

void
ERegionUnion(XserverRegion dst, XserverRegion src)
{
   XFixesUnionRegion(disp, dst, dst, src);
}

void
ERegionSubtract(XserverRegion dst, XserverRegion src)
{
   XFixesSubtractRegion(disp, dst, dst, src);
}

void
ERegionIntersectOffset(XserverRegion dst, int dx, int dy, XserverRegion src,
		       XserverRegion tmp)
{
   Display            *dpy = disp;
   XserverRegion       rgn;

   rgn = src;
   if (dx != 0 || dy != 0)
     {
	rgn = ERegionCopy(tmp, src);
	XFixesTranslateRegion(dpy, rgn, dx, dy);
     }
   XFixesIntersectRegion(dpy, dst, dst, rgn);
}

void
ERegionSubtractOffset(XserverRegion dst, int dx, int dy, XserverRegion src,
		      XserverRegion tmp)
{
   Display            *dpy = disp;
   XserverRegion       rgn;

   rgn = src;
   if (dx != 0 || dy != 0)
     {
	rgn = ERegionCopy(tmp, src);
	XFixesTranslateRegion(dpy, rgn, dx, dy);
     }
   XFixesSubtractRegion(dpy, dst, dst, rgn);
}

#if 0				/* Unused */
void
ERegionUnionOffset(XserverRegion dst, int dx, int dy, XserverRegion src,
		   XserverRegion tmp)
{
   Display            *dpy = disp;
   XserverRegion       rgn;

   rgn = src;
   if (dx != 0 || dy != 0)
     {
	rgn = ERegionCopy(tmp, src);
	XFixesTranslateRegion(dpy, rgn, dx, dy);
     }
   XFixesUnionRegion(dpy, dst, dst, rgn);
}
#endif

#if 0				/* Unused (for debug) */
int
ERegionIsEmpty(XserverRegion rgn)
{
   int                 nr;
   XRectangle         *pr;

   pr = XFixesFetchRegion(disp, rgn, &nr);
   if (pr)
      XFree(pr);
   return nr == 0;
}
#endif

void
ERegionShow(const char *txt, XserverRegion rgn,
	    void (*prf) (const char *fmt, ...))
{
   int                 i, nr;
   XRectangle         *pr;

   prf = (prf) ? prf : Eprintf;

   if (rgn == None)
     {
	prf(" - region: %s %#lx is None\n", txt, rgn);
	return;
     }

   pr = XFixesFetchRegion(disp, rgn, &nr);
   if (!pr || nr <= 0)
     {
	prf(" - region: %s %#lx is empty\n", txt, rgn);
	goto done;
     }

   prf(" - region: %s %#lx:\n", txt, rgn);
   for (i = 0; i < nr; i++)
      prf("%4d: %4d+%4d %4dx%4d\n", i, pr[i].x, pr[i].y, pr[i].width,
	  pr[i].height);

 done:
   if (pr)
      XFree(pr);
}

#endif /* USE_COMPOSITE */
