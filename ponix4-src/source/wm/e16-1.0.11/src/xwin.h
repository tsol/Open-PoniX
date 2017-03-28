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
#ifndef _XWIN_H_
#define _XWIN_H_

#include <X11/Xlib.h>
#include "util.h"
#include "xtypes.h"

typedef struct {
   char               *name;
   int                 screens;
   int                 screen;
   unsigned int        pixel_black;
   unsigned int        pixel_white;

   Win                 rroot;	/* Real root window */
   Win                 vroot;	/* Virtual root window */

   int                 server_grabbed;

   unsigned char       last_error_code;
} EDisplay;

__EXPORT__ extern EDisplay Dpy;
__EXPORT__ extern Display *disp;

#define RROOT Dpy.rroot
#define VROOT Dpy.vroot

void                EXInit(void);

int                 EDisplayOpen(const char *dstr, int scr);
void                EDisplayClose(void);
void                EDisplayDisconnect(void);

typedef void        (EXErrorHandler) (const XErrorEvent *);
typedef void        (EXIOErrorHandler) (void);
void                EDisplaySetErrorHandlers(EXErrorHandler * error,
					     EXIOErrorHandler * fatal);

void                EGrabServer(void);
void                EUngrabServer(void);
int                 EServerIsGrabbed(void);
void                EFlush(void);

#define ESYNC_MAIN	0x0001
#define ESYNC_DESKS	0x0002
#define ESYNC_MENUS	0x0004
#define ESYNC_MOVRES	0x0008
#define ESYNC_FOCUS	0x0010
#define ESYNC_TLOOP	0x0040
#define ESYNC_SLIDEOUT	0x0080
#define ESYNC_STARTUP	0x0100
void                ESync(unsigned int mask);

int                 EVisualIsARGB(Visual * vis);
Visual             *EVisualFindARGB(void);

Time                EGetTimestamp(void);

typedef void        (EventCallbackFunc) (Win win, XEvent * ev, void *prm);

typedef struct {
   EventCallbackFunc  *func;
   void               *prm;
} EventCallbackItem;

typedef struct {
   int                 num;
   EventCallbackItem  *lst;
} EventCallbackList;

struct _xwin {
   struct _xwin       *next;
   struct _xwin       *prev;
   EventCallbackList   cbl;
   Window              xwin;
   Win                 parent;
   int                 x, y, w, h;
   short               depth;
   unsigned short      bw;
   char                argb;
   char                mapped;
   char                in_use;
   signed char         do_del;
   char                attached;
   signed char         bg_owned;	/* bgpmap "owned" by Win */
   int                 num_rect;
   int                 ord;
   XRectangle         *rects;
   Visual             *visual;
   Colormap            cmap;
   Pixmap              bgpmap;
   unsigned int        bgcol;
#if USE_XI2
   unsigned int        event_mask;
#endif
};

Win                 ELookupXwin(Window xwin);

#define             WinGetXwin(win)		((win)->xwin)
#define             WinGetPmap(win)		((win)->bgpmap)
#define             WinGetX(win)		((win)->x)
#define             WinGetY(win)		((win)->y)
#define             WinGetW(win)		((win)->w)
#define             WinGetH(win)		((win)->h)
#define             WinGetBorderWidth(win)	((win)->bw)
#define             WinGetDepth(win)		((win)->depth)
#define             WinGetVisual(win)		((win)->visual)
#define             WinGetCmap(win)		((win)->cmap)
#define             WinGetNumRect(win)		((win)->num_rect)
#define             WinIsMapped(win)		((win)->mapped != 0)
#define             WinIsShaped(win)		((win)->num_rect != 0)

Win                 ECreateWinFromXwin(Window xwin);
void                EDestroyWin(Win win);

Win                 ERegisterWindow(Window xwin, XWindowAttributes * pxwa);
void                EUnregisterWindow(Win win);
void                EUnregisterXwin(Window xwin);
void                EventCallbackRegister(Win win, EventCallbackFunc * func,
					  void *prm);
void                EventCallbackUnregister(Win win, EventCallbackFunc * func,
					    void *prm);
void                EventCallbacksProcess(Win win, XEvent * ev);

Win                 ECreateWindow(Win parent, int x, int y, int w, int h,
				  int saveunder);
Win                 ECreateArgbWindow(Win parent, int x, int y, int w, int h,
				      Win cwin);
Win                 ECreateClientWindow(Win parent, int x, int y, int w, int h);

#define WIN_TYPE_CLIENT     0
#define WIN_TYPE_INTERNAL   1
#define WIN_TYPE_NO_ARGB    2
#define WIN_TYPE_GLX        3
Win                 ECreateObjectWindow(Win parent, int x, int y, int w,
					int h, int saveunder, int type,
					Win cwin);
Win                 ECreateEventWindow(Win parent, int x, int y, int w, int h);
Win                 ECreateFocusWindow(Win parent, int x, int y, int w, int h);
void                EWindowSync(Win win);
void                EWindowSetGeometry(Win win, int x, int y, int w, int h,
				       int bw);
void                EWindowSetMapped(Win win, int mapped);

void                EMoveWindow(Win win, int x, int y);
void                EResizeWindow(Win win, int w, int h);
void                EMoveResizeWindow(Win win, int x, int y, int w, int h);
void                EDestroyWindow(Win win);
void                EMapWindow(Win win);
void                EMapRaised(Win win);
void                EUnmapWindow(Win win);
void                EReparentWindow(Win win, Win parent, int x, int y);
int                 EGetGeometry(Win win, Window * root_return,
				 int *x, int *y, int *w, int *h, int *bw,
				 int *depth);
void                EGetWindowAttributes(Win win, XWindowAttributes * pxwa);
void                EConfigureWindow(Win win, unsigned int mask,
				     XWindowChanges * wc);
void                ESetWindowBackgroundPixmap(Win win, Pixmap pmap);
Pixmap              EGetWindowBackgroundPixmap(Win win);
void                EFreeWindowBackgroundPixmap(Win win);
void                ESetWindowBackground(Win win, unsigned int col);
int                 ETranslateCoordinates(Win src_w, Win dst_w,
					  int src_x, int src_y,
					  int *dest_x_return,
					  int *dest_y_return,
					  Window * child_return);
int                 EDrawableCheck(Drawable draw, int grab);

void                ESelectInput(Win win, unsigned int event_mask);
void                ESelectInputChange(Win win, unsigned int set,
				       unsigned int clear);
void                EChangeWindowAttributes(Win win, unsigned int mask,
					    XSetWindowAttributes * attr);
void                ESetWindowBorderWidth(Win win, unsigned int bw);
void                ERaiseWindow(Win win);
void                ELowerWindow(Win win);
void                EClearWindow(Win win);
void                EClearArea(Win win, int x, int y,
			       unsigned int w, unsigned int h);

Pixmap              ECreatePixmap(Win win, unsigned int width,
				  unsigned int height, unsigned int depth);
void                EFreePixmap(Pixmap pixmap);

int                 EShapeUpdate(Win win);
void                EShapeSetMask(Win win, int x, int y, Pixmap mask);
void                EShapeUnionMask(Win win, int x, int y, Pixmap mask);
void                EShapeSetMaskTiled(Win win, int x, int y, Pixmap mask,
				       int w, int h);
void                EShapeSetRects(Win win, int x, int y,
				   XRectangle * rect, int n_rects);
void                EShapeUnionRects(Win win, int x, int y,
				     XRectangle * rect, int n_rects);
int                 EShapeSetShape(Win win, int x, int y, Win src_win);
int                 EShapePropagate(Win win);
int                 EShapeCheck(Win win);
Pixmap              EWindowGetShapePixmap(Win win);
Pixmap              EWindowGetShapePixmapInverted(Win win);

void                EWarpPointer(Win win, int x, int y);
Bool                EQueryPointer(Win win, int *px, int *py,
				  Window * pchild, unsigned int *pmask);

unsigned int        EAllocColor(Colormap cmap, unsigned int argb);

#define _A(x)   (((x) >> 24) & 0xff)
#define _R(x)   (((x) >> 16) & 0xff)
#define _G(x)   (((x) >>  8) & 0xff)
#define _B(x)   (((x)      ) & 0xff)
#define _A16(x) (((x) >> 16) & 0xff00)
#define _R16(x) (((x) >>  8) & 0xff00)
#define _G16(x) (((x)      ) & 0xff00)
#define _B16(x) (((x) <<  8) & 0xff00)

#define COLOR32_FROM_RGB(c, r, g, b) \
    c = (0xff000000 | (((r) & 0xff) << 16) | (((g) & 0xff) << 8) | ((b) & 0xff))
#define COLOR32_TO_RGB(c, r, g, b) \
  do { r = _R(c); g = _G(c); b = _B(c); } while (0)
#define COLOR32_TO_ARGB(c, a, r, g, b) \
  do { a = _A(c); r = _R(c); g = _G(c); b = _B(c); } while (0)
#define COLOR32_TO_RGB16(c, r, g, b) \
  do { r = _R16(c); g = _G16(c); b = _B16(c); } while (0)
#define COLOR32_TO_ARGB16(c, a, r, g, b) \
  do { a = _A16(c); r = _R16(c); g = _G16(c); b = _B16(c); } while (0)

Window              EXWindowGetParent(Window xwin);
int                 EXGetGeometry(Window xwin, Window * root_return,
				  int *x, int *y, int *w, int *h, int *bw,
				  int *depth);

void                EXRestackWindows(Window * windows, int nwindows);

void                EXCopyArea(Drawable src, Drawable dst, int sx, int sy,
			       unsigned int w, unsigned int h, int dx, int dy);
void                EXCopyAreaTiled(Drawable src, Pixmap mask, Drawable dst,
				    int sx, int sy,
				    unsigned int w, unsigned int h,
				    int dx, int dy);

void                EXWarpPointer(Window xwin, int x, int y);
Bool                EXQueryPointer(Window xwin, int *px, int *py,
				   Window * pchild, unsigned int *pmask);

Pixmap              EXCreatePixmapCopy(Pixmap src, unsigned int w,
				       unsigned int h, unsigned int depth);

GC                  EXCreateGC(Drawable draw, unsigned int mask,
			       XGCValues * val);
void                EXFreeGC(GC gc);

void                EXSendEvent(Window xwin,
				unsigned int event_mask, XEvent * ev);

KeyCode             EKeysymToKeycode(KeySym keysym);
KeyCode             EKeynameToKeycode(const char *name);

Atom                EInternAtom(const char *name);

typedef struct {
   char                type;
   char                depth;
   Pixmap              pmap;
   Pixmap              mask;
   unsigned short      w, h;
} PmapMask;

void                PmapMaskInit(PmapMask * pmm, Win win, int w, int h);
void                PmapMaskFree(PmapMask * pmm);

#if USE_XRENDER
#include <X11/extensions/Xrender.h>

Picture             EPictureCreate(Win win, Drawable draw);
Picture             EPictureCreateSolid(Window xwin, int argb,
					unsigned int a, unsigned int rgb);
Picture             EPictureCreateBuffer(Win win, int w, int h, Pixmap * ppmap);
void                EPictureDestroy(Picture pict);

#endif /* USE_XRENDER */

#if USE_COMPOSITE
#include <X11/extensions/Xfixes.h>

XserverRegion       ERegionCreate(void);
XserverRegion       ERegionCreateRect(int x, int y, int w, int h);

#if USE_DESK_EXPOSE
XserverRegion       ERegionCreateFromRects(XRectangle * rectangles,
					   int nrectangles);
#endif
XserverRegion       ERegionCreateFromWindow(Win win);
XserverRegion       ERegionCopy(XserverRegion rgn, XserverRegion src);
XserverRegion       ERegionClone(XserverRegion src);
void                ERegionDestroy(XserverRegion rgn);
void                ERegionEmpty(XserverRegion rgn);
void                ERegionSetRect(XserverRegion rgn, int x, int y, int w,
				   int h);
void                ERegionTranslate(XserverRegion rgn, int dx, int dy);
void                ERegionIntersect(XserverRegion dst, XserverRegion src);
void                ERegionSubtract(XserverRegion dst, XserverRegion src);
void                ERegionUnion(XserverRegion dst, XserverRegion src);
void                ERegionIntersectOffset(XserverRegion dst, int dx, int dy,
					   XserverRegion src,
					   XserverRegion tmp);
void                ERegionSubtractOffset(XserverRegion dst, int dx, int dy,
					  XserverRegion src, XserverRegion tmp);
void                ERegionUnionOffset(XserverRegion dst, int dx, int dy,
				       XserverRegion src, XserverRegion tmp);
#if 0				/* Unused (for debug) */
int                 ERegionIsEmpty(XserverRegion rgn);
#endif
void                ERegionShow(const char *txt, XserverRegion rgn,
				void (*prf) (const char *fmt, ...));

void                EPictureSetClip(Picture pict, XserverRegion clip);

Pixmap              EWindowGetPixmap(const Win win);

#endif /* USE_COMPOSITE */

#endif /* _XWIN_H_ */
