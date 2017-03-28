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
#include "e16-ecore_hints.h"
#include "eimage.h"
#include "xwin.h"
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#define USE_EIWC_WINDOW 1
#if USE_XRENDER
#define USE_EIWC_RENDER 1
#include <X11/extensions/Xrender.h>
#endif

typedef struct {
   Cursor              curs;
   XSetWindowAttributes attr;
   Window              cwin;
} EiwData;

typedef void        (EiwLoopFunc) (Window win, EImage * im, EiwData * d);

#if USE_EIWC_RENDER
#include <Imlib2.h>

static void         _eiw_render_loop(Window win, EImage * im, EiwData * d);

static EiwLoopFunc *
_eiw_render_init(Window win __UNUSED__, EiwData * d)
{
   Visual             *vis;

   /* Quit if no ARGB visual.
    * If we have, assume(?) a colored XRenderCreateCursor is available. */
   vis = EVisualFindARGB();
   if (!vis)
      return NULL;

   imlib_context_set_visual(vis);
   d->curs = None;

   return _eiw_render_loop;
}

static void
_eiw_render_loop(Window win, EImage * im, EiwData * d)
{
   int                 w, h;
   XRenderPictFormat  *pictfmt;
   Pixmap              pmap;
   Picture             pict;

   EImageGetSize(im, &w, &h);

   pictfmt = XRenderFindStandardFormat(disp, PictStandardARGB32);
   pmap = XCreatePixmap(disp, WinGetXwin(VROOT), w, h, 32);
   imlib_context_set_image(im);
   imlib_context_set_drawable(pmap);
   imlib_render_image_on_drawable(0, 0);
   pict = XRenderCreatePicture(disp, pmap, pictfmt, 0, 0);
   XFreePixmap(disp, pmap);

   if (d->curs != None)
      XFreeCursor(disp, d->curs);
   d->curs = XRenderCreateCursor(disp, pict, w / 2, h / 2);
   XRenderFreePicture(disp, pict);

   XDefineCursor(disp, win, d->curs);
}

#endif /* USE_EIWC_RENDER */

#if USE_EIWC_WINDOW

static void         _eiw_window_loop(Window win, EImage * im, EiwData * d);

static EiwLoopFunc *
_eiw_window_init(Window win, EiwData * d)
{
   Pixmap              pmap, mask;
   GC                  gc;
   XColor              cl;

   d->cwin = XCreateWindow(disp, win, 0, 0, 32, 32, 0, CopyFromParent,
			   InputOutput, CopyFromParent,
			   CWOverrideRedirect | CWBackingStore | CWColormap |
			   CWBackPixel | CWBorderPixel, &d->attr);

   pmap = XCreatePixmap(disp, d->cwin, 16, 16, 1);
   gc = XCreateGC(disp, pmap, 0, NULL);
   XSetForeground(disp, gc, 0);
   XFillRectangle(disp, pmap, gc, 0, 0, 16, 16);
   XFreeGC(disp, gc);

   mask = XCreatePixmap(disp, d->cwin, 16, 16, 1);
   gc = XCreateGC(disp, mask, 0, NULL);
   XSetForeground(disp, gc, 0);
   XFillRectangle(disp, mask, gc, 0, 0, 16, 16);
   XFreeGC(disp, gc);

   d->curs = XCreatePixmapCursor(disp, pmap, mask, &cl, &cl, 0, 0);
   XDefineCursor(disp, win, d->curs);
   XDefineCursor(disp, d->cwin, d->curs);

   return _eiw_window_loop;
}

static void
_eiw_window_loop(Window win, EImage * im, EiwData * d)
{
   Pixmap              pmap, mask;
   Window              ww;
   int                 dd, x, y, w, h;
   unsigned int        mm;

   EImageRenderPixmaps(im, NULL, 0, &pmap, &mask, 0, 0);
   EImageGetSize(im, &w, &h);
   XShapeCombineMask(disp, d->cwin, ShapeBounding, 0, 0, mask, ShapeSet);
   XSetWindowBackgroundPixmap(disp, d->cwin, pmap);
   EImagePixmapsFree(pmap, mask);
   XClearWindow(disp, d->cwin);
   XQueryPointer(disp, win, &ww, &ww, &dd, &dd, &x, &y, &mm);
   XMoveResizeWindow(disp, d->cwin, x - w / 2, y - h / 2, w, h);
   XMapWindow(disp, d->cwin);
}

#endif /* USE_EIWC_WINDOW */

static              Window
ExtInitWinMain(void)
{
   int                 i, loop, err;
   Ecore_X_Window      win;
   XGCValues           gcv;
   GC                  gc;
   Pixmap              pmap;
   Atom                a;
   EiwData             eiwd;
   EiwLoopFunc        *eiwc_loop_func;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinMain enter\n");

   err = EDisplayOpen(NULL, -1);
   if (err)
      return None;

   EGrabServer();

   EImageInit();

   eiwd.attr.backing_store = NotUseful;
   eiwd.attr.override_redirect = True;
   eiwd.attr.colormap = WinGetCmap(VROOT);
   eiwd.attr.border_pixel = 0;
   eiwd.attr.background_pixel = 0;
   eiwd.attr.save_under = True;
   win = XCreateWindow(disp, WinGetXwin(VROOT),
		       0, 0, WinGetW(VROOT), WinGetH(VROOT),
		       0, CopyFromParent, InputOutput, CopyFromParent,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixel | CWBorderPixel, &eiwd.attr);

   pmap = XCreatePixmap(disp, win,
			WinGetW(VROOT), WinGetH(VROOT), WinGetDepth(VROOT));
   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   XCopyArea(disp, WinGetXwin(VROOT), pmap, gc,
	     0, 0, WinGetW(VROOT), WinGetH(VROOT), 0, 0);
   XSetWindowBackgroundPixmap(disp, win, pmap);
   XMapRaised(disp, win);
   XFreePixmap(disp, pmap);
   XFreeGC(disp, gc);

   a = EInternAtom("ENLIGHTENMENT_RESTART_SCREEN");
   ecore_x_window_prop_window_set(WinGetXwin(VROOT), a, &win, 1);

   XSelectInput(disp, win, StructureNotifyMask);

   EUngrabServer();
   ESync(0);

#if USE_EIWC_WINDOW && USE_EIWC_RENDER
   eiwc_loop_func = _eiw_render_init(win, &eiwd);
   if (!eiwc_loop_func)
      eiwc_loop_func = _eiw_window_init(win, &eiwd);
#elif USE_EIWC_RENDER
   eiwc_loop_func = _eiw_render_init(win, &eiwd);
#elif USE_EIWC_WINDOW
   eiwc_loop_func = _eiw_window_init(win, &eiwd);
#endif
   if (!eiwc_loop_func)
      return None;

   {
      XWindowAttributes   xwa;
      char                s[1024];
      EImage             *im;

      for (i = loop = 1;; i++, loop++)
	{
	   if (i > 12)
	      i = 1;

	   /* If we get unmapped we are done */
	   XGetWindowAttributes(disp, win, &xwa);
	   if (xwa.map_state == IsUnmapped)
	      break;

	   Esnprintf(s, sizeof(s), "pix/wait%i.png", i);
	   if (EDebug(EDBUG_TYPE_SESSION) > 1)
	      Eprintf("ExtInitWinCreate - child %s\n", s);

	   im = ThemeImageLoad(s);
	   if (im)
	     {
		eiwc_loop_func(win, im, &eiwd);
		EImageFree(im);
	     }
	   ESync(0);
	   usleep(50000);

	   /* If we still are here after 5 sec something is wrong. */
	   if (loop > 100)
	      break;
	}
   }

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinMain exit\n");

   EDisplayClose();

   exit(0);
}

Window
ExtInitWinCreate(void)
{
   Ecore_X_Window      win_ex;	/* Hmmm.. */
   Window              win;
   Atom                a;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinCreate\n");

   a = EInternAtom("ENLIGHTENMENT_RESTART_SCREEN");
   ESync(0);

   if (fork())
     {
	/* Parent */
	EUngrabServer();

	for (;;)
	  {
	     if (EDebug(EDBUG_TYPE_SESSION))
		Eprintf("ExtInitWinCreate - parent\n");

	     /* Hack to give the child some space. Not foolproof. */
	     sleep(1);

	     if (ecore_x_window_prop_window_get
		 (WinGetXwin(VROOT), a, &win_ex, 1) > 0)
		break;
	  }

	win = win_ex;
	if (EDebug(EDBUG_TYPE_SESSION))
	   Eprintf("ExtInitWinCreate - parent - %#lx\n", win);

	return win;
     }

   /* Child - Create the init window */

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinCreate - child\n");

   /* Clean up inherited stuff */

   SignalsRestore();

   EImageExit(0);
   EDisplayDisconnect();

   ExtInitWinMain();

   /* We will never get here */
   return None;
}

static Window       init_win_ext = None;

void
ExtInitWinSet(Window win)
{
   init_win_ext = win;
}

Window
ExtInitWinGet(void)
{
   return init_win_ext;
}

void
ExtInitWinKill(void)
{
   if (!disp || init_win_ext == None)
      return;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("Kill init window %#lx\n", init_win_ext);
   XUnmapWindow(disp, init_win_ext);
   init_win_ext = None;
}
