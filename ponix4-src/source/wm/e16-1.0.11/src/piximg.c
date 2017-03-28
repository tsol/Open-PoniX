/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2011 Kim Woelders
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
#include "config.h"
#if ENABLE_OLDMOVRES
#include "piximg.h"
#include "util.h"
#include "xwin.h"
#if USE_XRENDER
#include <X11/extensions/Xrender.h>

struct _PixImg {
   int                 w, h;
   Picture             pict;
   Picture             alpha;
   Picture             buf;
   Pixmap              pmap;
   Pixmap              mask;
   GC                  gc;
};

PixImg             *
PixImgCreate(Win win, GC gc, int w, int h)
{
   PixImg             *pi;
   XRenderPictFormat  *pictfmt;
   XRenderPictureAttributes pa;

   pi = ECALLOC(PixImg, 1);
   if (!pi)
      return NULL;

   pi->w = w;
   pi->h = h;
   pi->gc = gc;

   if (win)
     {
	pa.subwindow_mode = IncludeInferiors;
	pictfmt = XRenderFindVisualFormat(disp, WinGetVisual(win));
	pi->pict = XRenderCreatePicture(disp, WinGetXwin(win), pictfmt,
					CPSubwindowMode, &pa);
     }
   else
     {
	pi->pict = EPictureCreateBuffer(VROOT, w, h, &pi->pmap);
     }

   return pi;
}

void
PixImgDestroy(PixImg * pi)
{
   if (!pi)
      return;

   XRenderFreePicture(disp, pi->pict);
   if (pi->pmap != None)
      XFreePixmap(disp, pi->pmap);
   if (pi->mask != None)
      XFreePixmap(disp, pi->mask);

   Efree(pi);
}

void
PixImgSetMask(PixImg * pi, Pixmap mask, int x, int y)
{
   XRenderPictureAttributes pa;

   switch (mask)
     {
     case 0:
	pa.clip_mask = None;
	XRenderChangePicture(disp, pi->pict, CPClipMask, &pa);
	break;
     case 1:
	pa.clip_x_origin = x;
	pa.clip_y_origin = y;
	pa.clip_mask = pi->mask;
	XRenderChangePicture(disp, pi->pict,
			     CPClipXOrigin | CPClipYOrigin | CPClipMask, &pa);
	break;
     default:
	pi->mask = mask;
	break;
     }
}

void
PixImgFill(PixImg * pi, Drawable draw, int x, int y)
{
   XCopyArea(disp, draw, pi->pmap, pi->gc, x, y, pi->w, pi->h, 0, 0);
}

#include "eimage.h"
void
PixImgPaste(PixImg * src, PixImg * dst, int xs, int ys,
	    int w, int h, int xt, int yt)
{
   XRenderComposite(disp, PictOpSrc, src->pict, None, dst->pict,
		    xs, ys, 0, 0, xt, yt, w, h);
}

void
PixImgBlend(PixImg * src, PixImg * xxx, PixImg * dst, Drawable draw __UNUSED__,
	    int x, int y, int w, int h)
{
   if (dst->alpha == None)
     {
	dst->alpha = EPictureCreateSolid(WinGetXwin(VROOT), 1, 0x80, 0);
	dst->buf =
	   EPictureCreateBuffer(VROOT, WinGetW(VROOT), WinGetH(VROOT), NULL);
     }
   XRenderComposite(disp, PictOpSrc, src->pict, None, dst->buf,
		    x, y, 0, 0, x, y, w, h);
   XRenderComposite(disp, PictOpOver, xxx->pict, dst->alpha, dst->buf,
		    0, 0, 0, 0, x, y, w, h);
   XRenderComposite(disp, PictOpSrc, dst->buf, None, dst->pict,
		    x, y, 0, 0, x, y, w, h);
}

#else
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>

struct _PixImg {
   XImage             *xim;
   XShmSegmentInfo    *shminfo;
   Drawable            draw;
   Pixmap              mask;
   GC                  gc;
};

PixImg             *
PixImgCreate(Win win, GC gc, int w, int h)
{
   PixImg             *pi;

   pi = ECALLOC(PixImg, 1);
   if (!pi)
      return NULL;

   if (win)
      pi->draw = WinGetXwin(win);
   pi->gc = gc;

   pi->shminfo = EMALLOC(XShmSegmentInfo, 1);
   if (pi->shminfo)
     {
	pi->xim = XShmCreateImage(disp, WinGetVisual(VROOT),
				  WinGetDepth(VROOT), ZPixmap, NULL,
				  pi->shminfo, w, h);
	if (pi->xim)
	  {
	     pi->shminfo->shmid =
		shmget(IPC_PRIVATE, pi->xim->bytes_per_line * pi->xim->height,
		       IPC_CREAT | 0666);
	     if (pi->shminfo->shmid >= 0)
	       {
		  pi->shminfo->shmaddr = pi->xim->data =
		     (char *)shmat(pi->shminfo->shmid, 0, 0);
		  if (pi->shminfo->shmaddr != (void *)-1)
		    {
		       pi->shminfo->readOnly = False;
		       Dpy.last_error_code = 0;
		       XShmAttach(disp, pi->shminfo);
		       ESync(0);
		       if (Dpy.last_error_code == 0)
			  return pi;
		       XShmDetach(disp, pi->shminfo);
		       shmdt(pi->shminfo->shmaddr);
		    }
		  shmctl(pi->shminfo->shmid, IPC_RMID, 0);
	       }
	     XDestroyImage(pi->xim);
	  }
	Efree(pi->shminfo);
     }
   Efree(pi);
   return NULL;
}

void
PixImgDestroy(PixImg * pi)
{
   if (!pi)
      return;
   ESync(0);
   XShmDetach(disp, pi->shminfo);
   shmdt(pi->shminfo->shmaddr);
   shmctl(pi->shminfo->shmid, IPC_RMID, 0);
   XDestroyImage(pi->xim);
   Efree(pi->shminfo);
   if (pi->mask)
      XFreePixmap(disp, pi->mask);
   Efree(pi);
}

void
PixImgSetMask(PixImg * pi, Pixmap mask, int x, int y)
{
   switch (mask)
     {
     case 0:
	XSetClipMask(disp, pi->gc, None);
	break;
     case 1:
	XSetClipMask(disp, pi->gc, pi->mask);
	XSetClipOrigin(disp, pi->gc, x, y);
	break;
     default:
	pi->mask = mask;
	break;
     }
}

static int
_fix_bounds(int *px, int *py, int *pw, int *ph, int *pox, int *poy)
{
   int                 x, y, w, h, ox, oy, wmax, hmax;

   wmax = WinGetW(VROOT);
   hmax = WinGetH(VROOT);

   x = *px;
   y = *py;
   w = *pw;
   h = *ph;

   ox = 0;
   oy = 0;

   if (x + w > wmax)
      w -= (x + w - wmax);
   if (x < 0)
     {
	ox = -x;
	w -= ox;
	x = 0;
     }

   if (y + h > hmax)
      h -= (y + h - hmax);
   if (y < 0)
     {
	oy = -y;
	h -= oy;
	y = 0;
     }

   if (w <= 0 || h <= 0)
      return 1;

   *px = x;
   *py = y;
   *pw = w;
   *ph = h;
   if (pox)
      *pox = ox;
   if (poy)
      *poy = oy;

   return 0;
}

void
PixImgFill(PixImg * pi, Drawable draw, int x, int y)
{
   XShmGetImage(disp, draw, pi->xim, x, y, 0xffffffff);
}

void
PixImgPaste(PixImg * src, PixImg * dst, int xs, int ys,
	    int w, int h, int xt, int yt)
{
   /* FIXME - No bounds checking if (xs,ys) != (xt,yt) */
   if (xs == xt && ys == yt)
     {
	if (_fix_bounds(&xs, &ys, &w, &h, NULL, NULL))
	   return;
	xt = xs;
	yt = ys;
     }

   XShmPutImage(disp, dst->draw, dst->gc, src->xim, xs, ys, xt, yt, w, h,
		False);
}

void
PixImgBlend(PixImg * s1, PixImg * s2, PixImg * dst, Drawable draw,
	    int x, int y, int w, int h)
{
   int                 i, j, ox, oy;

   if (_fix_bounds(&x, &y, &w, &h, &ox, &oy))
      return;

   ESync(0);

   switch (dst->xim->bits_per_pixel)
     {
     case 32:
	for (j = 0; j < h; j++)
	  {
	     unsigned int       *ptr1, *ptr2, *ptr3;

	     ptr1 =
		(unsigned int *)(s1->xim->data +
				 (x * ((s1->xim->bits_per_pixel) >> 3)) +
				 ((j + y) * s1->xim->bytes_per_line));
	     ptr2 =
		(unsigned int *)(s2->xim->data +
				 (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				 ((j + oy) * s2->xim->bytes_per_line));
	     ptr3 =
		(unsigned int *)(dst->xim->data +
				 (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				 ((j + oy) * dst->xim->bytes_per_line));
	     for (i = 0; i < w; i++)
	       {
		  unsigned int        p1, p2;

		  p1 = *ptr1++;
		  p2 = *ptr2++;
		  *ptr3++ = ((p1 >> 1) & 0x7f7f7f7f) +
		     ((p2 >> 1) & 0x7f7f7f7f) + (p1 & p2 & 0x01010101);
	       }
	  }
	break;
     case 24:
	for (j = 0; j < h; j++)
	  {
	     for (i = 0; i < w; i++)
	       {
		  unsigned int        p1, p2;

		  p1 = XGetPixel(s1->xim, (i + x), (j + y));
		  p2 = XGetPixel(s2->xim, (i + ox), (j + oy));
		  XPutPixel(dst->xim, (i + ox), (j + oy),
			    (((p1 >> 1) & 0x7f7f7f7f) +
			     ((p2 >> 1) & 0x7f7f7f7f) +
			     (p1 & p2 & 0x01010101)));
	       }
	  }
	break;
     case 16:
	if (WinGetDepth(VROOT) != 15)
	  {
	     for (j = 0; j < h; j++)
	       {
		  unsigned int       *ptr1, *ptr2, *ptr3;

		  ptr1 =
		     (unsigned int *)(s1->xim->data +
				      (x * ((s1->xim->bits_per_pixel) >> 3)) +
				      ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned int *)(s2->xim->data +
				      (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned int *)(dst->xim->data +
				      (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * dst->xim->bytes_per_line));
		  if (!(w & 0x1))
		    {
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       (p1 & p2 &
				((0x1 << 11) | (0x1 << 5) | (0x1) |
				 (0x1 << 27) | (0x1 << 21) | (0x1 << 16)));
			 }
		    }
		  else
		    {
		       for (i = 0; i < (w - 1); i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 8) | (0x7c << 3) | (0x78 >> 3) |
				 (0x78 << 24) | (0x7c << 19) | (0x78 << 13)))
			       +
			       (p1 & p2 &
				((0x1 << 11) | (0x1 << 5) | (0x1) |
				 (0x1 << 27) | (0x1 << 21) | (0x1 << 16)));
			 }
		       {
			  unsigned short     *pptr1, *pptr2, *pptr3;
			  unsigned short      pp1, pp2;

			  pptr1 = (unsigned short *)ptr1;
			  pptr2 = (unsigned short *)ptr2;
			  pptr3 = (unsigned short *)ptr3;
			  pp1 = *pptr1;
			  pp2 = *pptr2;
			  *pptr3 =
			     ((pp1 >> 1) &
			      ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)))
			     +
			     ((pp2 >> 1) &
			      ((0x78 << 8) | (0x7c << 3) | (0x78 >> 3)))
			     + (pp1 & pp2 & ((0x1 << 11) | (0x1 << 5) | (0x1)));
		       }
		    }
	       }
	  }
	else
	  {
	     for (j = 0; j < h; j++)
	       {
		  unsigned int       *ptr1, *ptr2, *ptr3;

		  ptr1 =
		     (unsigned int *)(s1->xim->data +
				      (x * (s1->xim->bits_per_pixel >> 3)) +
				      ((j + y) * s1->xim->bytes_per_line));
		  ptr2 =
		     (unsigned int *)(s2->xim->data +
				      (ox * ((s2->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * s2->xim->bytes_per_line));
		  ptr3 =
		     (unsigned int *)(dst->xim->data +
				      (ox * ((dst->xim->bits_per_pixel) >> 3)) +
				      ((j + oy) * dst->xim->bytes_per_line));
		  if (!(w & 0x1))
		    {
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       (p1 & p2 &
				((0x1 << 10) | (0x1 << 5) | (0x1) |
				 (0x1 << 26) | (0x1 << 20) | (0x1 << 16)));
			 }
		    }
		  else
		    {
		       for (i = 0; i < (w - 1); i += 2)
			 {
			    unsigned int        p1, p2;

			    p1 = *ptr1++;
			    p2 = *ptr2++;
			    *ptr3++ =
			       ((p1 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       ((p2 >> 1) &
				((0x78 << 7) | (0x78 << 2) | (0x78 >> 3) |
				 (0x78 << 23) | (0x78 << 18) | (0x78 << 13)))
			       +
			       (p1 & p2 &
				((0x1 << 10) | (0x1 << 5) | (0x1) |
				 (0x1 << 26) | (0x1 << 20) | (0x1 << 16)));
			 }
		       {
			  unsigned short     *pptr1, *pptr2, *pptr3;
			  unsigned short      pp1, pp2;

			  pptr1 = (unsigned short *)ptr1;
			  pptr2 = (unsigned short *)ptr2;
			  pptr3 = (unsigned short *)ptr3;
			  pp1 = *pptr1;
			  pp2 = *pptr2;
			  *pptr3 =
			     ((pp1 >> 1) &
			      ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)))
			     +
			     ((pp2 >> 1) &
			      ((0x78 << 7) | (0x78 << 2) | (0x78 >> 3)))
			     + (pp1 & pp2 & ((0x1 << 10) | (0x1 << 5) | (0x1)));
		       }
		    }
	       }
	  }
	break;
     default:
	for (j = 0; j < h; j++)
	  {
	     unsigned char      *ptr1, *ptr2, *ptr3;

	     ptr1 =
		(unsigned char *)(s1->xim->data +
				  ((x) *
				   ((s1->xim->bits_per_pixel) >> 3)) +
				  ((j + y) * s1->xim->bytes_per_line));
	     ptr2 =
		(unsigned char *)(s2->xim->data +
				  ((ox) *
				   ((s2->xim->bits_per_pixel) >> 3)) +
				  ((j + oy) * s2->xim->bytes_per_line));
	     ptr3 =
		(unsigned char *)(dst->xim->data +
				  ((ox) *
				   ((dst->xim->bits_per_pixel) >> 3)) +
				  ((j + oy) * dst->xim->bytes_per_line));
	     if (!(w & 0x1))
	       {
		  if (j & 0x1)
		    {
		       ptr2++;
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned char       p1;

			    p1 = *ptr1;
			    ptr1 += 2;
			    *ptr3++ = p1;
			    p1 = *ptr2;
			    ptr2 += 2;
			    *ptr3++ = p1;
			 }
		    }
		  else
		    {
		       ptr1++;
		       for (i = 0; i < w; i += 2)
			 {
			    unsigned char       p1;

			    p1 = *ptr2;
			    ptr2 += 2;
			    *ptr3++ = p1;
			    p1 = *ptr1;
			    ptr1 += 2;
			    *ptr3++ = p1;
			 }
		    }
	       }
	     else
	       {
		  if (j & 0x1)
		    {
		       ptr2++;
		       for (i = 0; i < (w - 1); i += 2)
			 {
			    unsigned char       p1;

			    p1 = *ptr1;
			    ptr1 += 2;
			    *ptr3++ = p1;
			    p1 = *ptr2;
			    ptr2 += 2;
			    *ptr3++ = p1;
			 }
		       *ptr3 = *ptr1;
		    }
		  else
		    {
		       ptr1++;
		       for (i = 0; i < (w - 1); i += 2)
			 {
			    unsigned char       p1;

			    p1 = *ptr2;
			    ptr2 += 2;
			    *ptr3++ = p1;
			    p1 = *ptr1;
			    ptr1 += 2;
			    *ptr3++ = p1;
			 }
		       *ptr3 = *ptr2;
		    }
	       }
	  }
	break;
     }
   XShmPutImage(disp, draw, dst->gc, dst->xim, ox, oy, x, y, w, h, False);
}

#endif /* USE_XRENDER */

#endif /* ENABLE_OLDMOVRES */
