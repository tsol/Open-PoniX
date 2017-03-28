/*
 * Copyright (C) 2004-2010 Kim Woelders
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
#include "eimage.h"
#include "xwin.h"
#include <Imlib2.h>
#if USE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

void
EImageInit(void)
{
   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);

   imlib_set_color_usage(128);

   imlib_context_set_display(disp);
   imlib_context_set_visual(WinGetVisual(VROOT));
   imlib_context_set_colormap(WinGetCmap(VROOT));

#ifdef HAVE_IMLIB_CONTEXT_SET_MASK_ALPHA_THRESHOLD
   imlib_context_set_mask_alpha_threshold(Conf.testing.mask_alpha_threshold);
#endif

   imlib_context_set_anti_alias(0);
   imlib_context_set_dither(1);
}

void
EImageExit(int quit __UNUSED__)
{
#if HAVE_IMLIB_CONTEXT_DISCONNECT_DISPLAY
   imlib_context_disconnect_display();
#endif
}

int
EImageSetCacheSize(int size)
{
   int                 size_old;

   size_old = imlib_get_cache_size();
   imlib_set_cache_size(size);

   return size_old;
}

static void
_EImageFlagsSet(int flags)
{
   if (flags & EIMAGE_ANTI_ALIAS)
      imlib_context_set_anti_alias(1);
   if (flags & EIMAGE_BLEND)
      imlib_context_set_blend(1);
#ifdef HAVE_IMLIB_CONTEXT_SET_MASK_ALPHA_THRESHOLD
   if (flags & EIMAGE_HIGH_MASK_THR)
      imlib_context_set_mask_alpha_threshold(128);
#endif
}

static void
_EImageFlagsReset(void)
{
   imlib_context_set_anti_alias(0);
   imlib_context_set_blend(0);
#ifdef HAVE_IMLIB_CONTEXT_SET_MASK_ALPHA_THRESHOLD
   imlib_context_set_mask_alpha_threshold(Conf.testing.mask_alpha_threshold);
#endif
}

EImage             *
EImageLoad(const char *file)
{
   return imlib_load_image(file);
}

void
EImageSave(EImage * im, const char *file)
{
   imlib_context_set_image(im);
   imlib_image_set_format("png");
   imlib_save_image(file);
}

EImage             *
EImageCreate(int w, int h)
{
   EImage             *im;

   im = imlib_create_image(w, h);

   return im;
}

EImage             *
EImageCreateFromData(int w, int h, unsigned int *data)
{
   EImage             *im;

   im = imlib_create_image_using_copied_data(w, h, data);

   return im;
}

EImage             *
EImageCreateScaled(EImage * im, int sx, int sy, int sw, int sh, int dw, int dh)
{
   imlib_context_set_image(im);
   if (sw <= 0)
      sw = imlib_image_get_width();
   if (sh <= 0)
      sh = imlib_image_get_height();
   if (dw <= 0)
      dw = sw;
   if (dh <= 0)
      dh = sh;
   return imlib_create_cropped_scaled_image(sx, sy, sw, sh, dw, dh);
}

void
EImageFree(EImage * im)
{
   imlib_context_set_image(im);
   imlib_free_image();
}

void
EImageDecache(EImage * im)
{
   imlib_context_set_image(im);
   imlib_free_image_and_decache();
}

static int
_EImageCheckAlpha(void)
{
   static const short  oink = 3;	/* For endianness checking */
   unsigned char      *pb, *pe;

   if (!imlib_image_has_alpha())
      return 0;

   pb = (unsigned char *)imlib_image_get_data_for_reading_only();
   if (!pb)
      return 0;

   pe = pb + 4 * imlib_image_get_width() * imlib_image_get_height();
   pb += *((char *)(&oink));
   for (; pb < pe; pb += 4)
      if (*pb != 0xff)
	 return 1;

   return 0;
}

void
EImageCheckAlpha(EImage * im)
{
   imlib_context_set_image(im);

   if (imlib_image_has_alpha() && !_EImageCheckAlpha())
     {
#if 0
	Eprintf("Alpha set but no shape %s\n", is->real_file);
#endif
	imlib_image_set_has_alpha(0);
     }
}

void
EImageSetHasAlpha(EImage * im, int has_alpha)
{
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(has_alpha);
}

void
EImageSetBorder(EImage * im, EImageBorder * border)
{
   Imlib_Border        ib;

   ib.left = border->left;
   ib.right = border->right;
   ib.top = border->top;
   ib.bottom = border->bottom;
   imlib_context_set_image(im);
   imlib_image_set_border(&ib);
}

int
EImageHasAlpha(EImage * im)
{
   imlib_context_set_image(im);
   return imlib_image_has_alpha();
}

void
EImageGetSize(EImage * im, int *pw, int *ph)
{
   imlib_context_set_image(im);
   *pw = imlib_image_get_width();
   *ph = imlib_image_get_height();
}

void               *
EImageGetData(EImage * im)
{
   imlib_context_set_image(im);
   return imlib_image_get_data_for_reading_only();
}

void
EImageFill(EImage * im, int x, int y, int w, int h, unsigned int color)
{
   int                 a, r, g, b;

   imlib_context_set_image(im);
   COLOR32_TO_ARGB(color, a, r, g, b);
   imlib_context_set_color(r, g, b, a);
   imlib_context_set_blend(0);
   imlib_image_fill_rectangle(x, y, w, h);
}

void
EImageOrientate(EImage * im, int orientation)
{
   imlib_context_set_image(im);
   imlib_image_orientate(orientation);
}

void
EImageBlend(EImage * im, EImage * src, int flags,
	    int sx, int sy, int sw, int sh,
	    int dx, int dy, int dw, int dh, int merge_alpha)
{
   imlib_context_set_image(im);
   if (flags)
      _EImageFlagsSet(flags);
   imlib_blend_image_onto_image(src, merge_alpha, sx, sy, sw, sh,
				dx, dy, dw, dh);
   if (flags)
      _EImageFlagsReset();
}

void
EImageBlendCM(EImage * im, EImage * src, EImageColorModifier * icm)
{
   int                 w, h, iw, ih;

   imlib_context_set_image(src);
   iw = imlib_image_get_width();
   ih = imlib_image_get_height();
   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   imlib_context_set_blend(1);
   if (icm)
      imlib_context_set_color_modifier(icm);
   imlib_context_set_operation(IMLIB_OP_COPY);
   imlib_blend_image_onto_image(src, 0, 0, 0, iw, ih, 0, 0, w, h);
   imlib_context_set_blend(0);
   if (icm)
      imlib_context_set_color_modifier(NULL);
}

void
EImageTile(EImage * im, EImage * tile, int flags, int tw, int th,
	   int dx, int dy, int dw, int dh, int ox, int oy)
{
   Imlib_Image         tim;
   int                 x, y, tx, ty, ww, hh;
   int                 sw, sh;

   if (tw <= 0 || th <= 0)
      return;

   if (flags)
      _EImageFlagsSet(flags);

   imlib_context_set_image(tile);
   sw = imlib_image_get_width();
   sh = imlib_image_get_height();
   if (sw == tw && sh == th)
     {
	tim = tile;
     }
   else
     {
	tim = imlib_create_image(tw, th);
	imlib_context_set_image(tim);
	imlib_context_set_blend(0);
	imlib_context_set_anti_alias(1);
	imlib_blend_image_onto_image(tile, 0, 0, 0, sw, sh, 0, 0, tw, th);
	imlib_context_set_anti_alias(0);
     }
   imlib_context_set_image(im);

   if (ox)
     {
	ox = tw - ox;
	ox %= tw;
	if (ox < 0)
	   ox += tw;
     }
   if (oy)
     {
	oy = th - oy;
	oy %= th;
	if (oy < 0)
	   oy += th;
     }
   dw += dx;
   dh += dy;
   y = dy;
   ty = oy;
   hh = th - oy;
   for (;;)
     {
	if (y + hh >= dh)
	   hh = dh - y;
	if (hh <= 0)
	   break;
	x = dx;
	tx = ox;
	ww = tw - ox;
	for (;;)
	  {
	     if (x + ww >= dw)
		ww = dw - x;
	     if (ww <= 0)
		break;
	     imlib_blend_image_onto_image(tim, 0, tx, ty, ww, hh, x, y, ww, hh);
	     tx = 0;
	     x += ww;
	     ww = tw;
	  }
	ty = 0;
	y += hh;
	hh = th;
     }
   if (tim != tile)
     {
	imlib_context_set_image(tim);
	imlib_free_image();
	imlib_context_set_image(im);	/* FIXME - Remove */
     }

   if (flags)
      _EImageFlagsReset();
}

EImage             *
EImageGrabDrawable(Drawable draw, Pixmap mask, int x, int y, int w, int h,
		   int grab)
{
   EImage             *im;
   Colormap            cm;

   cm = imlib_context_get_colormap();
   imlib_context_set_colormap(None);	/* Fix for grabbing bitmaps */
   imlib_context_set_drawable(draw);
   im = imlib_create_image_from_drawable(mask, x, y, w, h, grab);
   imlib_context_set_colormap(cm);

   return im;
}

EImage             *
EImageGrabDrawableScaled(Win win, Drawable draw, Pixmap mask,
			 int x, int y, int w, int h,
			 int iw, int ih, int grab, int get_mask_from_shape)
{
   EImage             *im;
   Visual             *vis;

   imlib_context_set_drawable(draw);
   vis = (win) ? WinGetVisual(win) : NULL;
   if (vis)
      imlib_context_set_visual(vis);

   im = imlib_create_scaled_image_from_drawable(mask, x, y, w, h, iw, ih, grab,
						get_mask_from_shape);

   if (vis)
      imlib_context_set_visual(WinGetVisual(VROOT));

   return im;
}

void
EImageRenderOnDrawable(EImage * im, Win win, Drawable draw, int flags,
		       int x, int y, int w, int h)
{
   Visual             *vis;

   imlib_context_set_image(im);
   imlib_context_set_drawable((draw != None) ? draw : WinGetXwin(win));
   vis = (win) ? WinGetVisual(win) : NULL;
   if (vis)
      imlib_context_set_visual(vis);

   if (flags)
      _EImageFlagsSet(flags);
   imlib_render_image_on_drawable_at_size(x, y, w, h);
   if (flags)
      _EImageFlagsReset();

   if (vis)
      imlib_context_set_visual(WinGetVisual(VROOT));
}

void
EImageRenderPixmaps(EImage * im, Win win, int flags,
		    Pixmap * pmap, Pixmap * mask, int w, int h)
{
   Visual             *vis;
   Pixmap              m;

   imlib_context_set_image(im);
   imlib_context_set_drawable((win) ? WinGetXwin(win) : WinGetXwin(VROOT));
   vis = (win) ? WinGetVisual(win) : NULL;
   if (vis)
      imlib_context_set_visual(vis);

   *pmap = None;
   if (!mask)			/* Imlib2 <= 1.3.0 needs a mask pointer */
      mask = &m;		/* ... to avoid bogus error messages    */
   if (mask)
      *mask = None;

   if (flags)
      _EImageFlagsSet(flags);
   if (w <= 0 || h <= 0)
      imlib_render_pixmaps_for_whole_image(pmap, mask);
   else
      imlib_render_pixmaps_for_whole_image_at_size(pmap, mask, w, h);
   if (flags)
      _EImageFlagsReset();

   if (vis)
      imlib_context_set_visual(WinGetVisual(VROOT));
}

void
EImagePixmapsFree(Pixmap pmap, Pixmap mask __UNUSED__)
{
   imlib_free_pixmap_and_mask(pmap);
}

void
EImageApplyToWin(EImage * im, Win win, int flags, int w, int h)
{
   Pixmap              pmap, mask;

   EImageRenderPixmaps(im, win, flags, &pmap, &mask, w, h);
   ESetWindowBackgroundPixmap(win, pmap);
   if ((mask != None) || (mask == None && WinIsShaped(win)))
      EShapeSetMask(win, 0, 0, mask);
   EImagePixmapsFree(pmap, mask);
   EClearWindow(win);
}

void
ScaleRect(Win wsrc, Drawable src, Win wdst, Pixmap dst,
	  int sx, int sy, int sw, int sh,
	  int dx, int dy, int dw, int dh, int flags)
{
#if USE_XRENDER
   if (Conf.testing.use_render_for_scaling)
     {
	XRenderPictFormat  *pictfmt;
	XRenderPictureAttributes pa;
	XTransform          tr;
	Picture             psrc, pdst;
	double              scale_x, scale_y;

	scale_x = (double)sw / (double)dw;
	scale_y = (double)sh / (double)dh;
	memset(&tr, 0, sizeof(tr));
	tr.matrix[0][0] = XDoubleToFixed(scale_x);
	tr.matrix[1][1] = XDoubleToFixed(scale_y);
	tr.matrix[2][2] = XDoubleToFixed(1.);

	pa.subwindow_mode = IncludeInferiors;
	pictfmt = XRenderFindVisualFormat(disp, wsrc->visual);
	psrc = XRenderCreatePicture(disp, src, pictfmt, CPSubwindowMode, &pa);
	pictfmt = XRenderFindVisualFormat(disp, wdst->visual);
	pdst = XRenderCreatePicture(disp, dst, pictfmt, CPSubwindowMode, &pa);

	XRenderSetPictureFilter(disp, psrc, (flags & EIMAGE_ANTI_ALIAS) ?
				FilterBest : FilterNearest, NULL, 0);
	XRenderSetPictureTransform(disp, psrc, &tr);
	XRenderComposite(disp, PictOpSrc, psrc, None, pdst,
			 (int)(sx / scale_x + .5), (int)(sy / scale_y + .5),
			 0, 0, dx, dy, dw, dh);
	XRenderFreePicture(disp, psrc);
	XRenderFreePicture(disp, pdst);
     }
   else
#endif
     {
	int                 scale;
	Imlib_Image         im;

	if (flags & (EIMAGE_ISCALE))
	  {
	     scale = (flags & EIMAGE_ISCALE) >> 8;
	     im = EImageGrabDrawableScaled(wsrc, src, None, sx, sy, sw, sh,
					   scale * dw, scale * dh, 0, 0);
	     flags |= EIMAGE_ANTI_ALIAS;
	  }
	else
	  {
	     im = EImageGrabDrawableScaled(wsrc, src, None, sx, sy, sw, sh,
					   sw, sh, 0, 0);
	  }

	EImageRenderOnDrawable(im, wdst, dst, flags, dx, dy, dw, dh);
	imlib_free_image();
     }
}

void
ScaleTile(Win wsrc, Drawable src, Win wdst, Pixmap dst,
	  int dx, int dy, int dw, int dh, int scale)
{
   Imlib_Image         im, tim;
   int                 sw, sh, stw, sth, tw, th;

   sw = WinGetW(wsrc);
   sh = WinGetH(wsrc);
   EXGetGeometry(src, NULL, NULL, NULL, &stw, &sth, NULL, NULL);
   if (stw >= sw && sth >= sh)
     {
	ScaleRect(wsrc, src, wdst, dst, 0, 0, sw, sh, dx, dy, dw, dh, scale);
	return;
     }

   /* Source Drawawble is smaller than source window - do scaled tiling */

   scale = (scale) ? 2 : 1;

   tw = (int)((double)(stw * scale * dw) / sw + .5);
   th = (int)((double)(sth * scale * dh) / sh + .5);
#if 0
   Eprintf("ScaleTile: Tile %#lx %dx%d -> %dx%d T %dx%d -> %dx%d\n", src,
	   stw, sth, tw, th, scale * dw, scale * dh, dw, dh);
#endif
   tim =
      EImageGrabDrawableScaled(wsrc, src, None, 0, 0, stw, sth, tw, th, 0, 0);
   im = EImageCreate(scale * dw, scale * dh);
   EImageTile(im, tim, 0, tw, th, 0, 0, scale * dw, scale * dh, 0, 0);
   EImageFree(tim);

   EImageRenderOnDrawable(im, wdst, dst, EIMAGE_ANTI_ALIAS, dx, dy, dw, dh);
   imlib_free_image();
}

#if 0				/* Unused */
void
EDrawableDumpImage(Drawable draw, const char *txt)
{
   static int          seqn = 0;
   char                buf[1024];
   Imlib_Image         im;
   int                 w, h;

   w = h = 0;
   EXGetGeometry(draw, NULL, NULL, NULL, &w, &h, NULL, NULL);
   if (w <= 0 || h <= 0)
      return;
   imlib_context_set_drawable(draw);
   im = imlib_create_image_from_drawable(None, 0, 0, w, h, !EServerIsGrabbed());
   imlib_context_set_image(im);
   imlib_image_set_format("png");
   sprintf(buf, "%s-%#lx-%d.png", txt, draw, seqn++);
   Eprintf("EDrawableDumpImage: %s\n", buf);
   imlib_save_image(buf);
   imlib_free_image_and_decache();
}
#endif

void
PmapMaskInit(PmapMask * pmm, Win win, int w, int h)
{
   if (pmm->pmap)
     {
	if (pmm->w == w && pmm->h == h && pmm->depth == WinGetDepth(win))
	   return;
	PmapMaskFree(pmm);
     }

   pmm->type = 0;
   pmm->depth = WinGetDepth(win);
   pmm->pmap = ECreatePixmap(win, w, h, 0);
   pmm->mask = None;
   pmm->w = w;
   pmm->h = h;
}

void
PmapMaskFree(PmapMask * pmm)
{
   /* type !=0: Created by imlib_render_pixmaps_for_whole_image... */
   if (pmm->pmap)
     {
	if (pmm->type == 0)
	   EFreePixmap(pmm->pmap);
	else
	   imlib_free_pixmap_and_mask(pmm->pmap);
	pmm->pmap = 0;
     }

   if (pmm->mask)
     {
	if (pmm->type == 0)
	   EFreePixmap(pmm->mask);
	pmm->mask = 0;
     }
}

EImageColorModifier *
EImageColorModifierCreate(void)
{
   return imlib_create_color_modifier();
}

void
EImageColorModifierSetTables(EImageColorModifier * icm,
			     unsigned char *r, unsigned char *g,
			     unsigned char *b, unsigned char *a)
{
   if (!icm)
      return;

   imlib_context_set_color_modifier(icm);
#if 0				/* Useful in this context? */
   imlib_modify_color_modifier_gamma(0.5);
   imlib_modify_color_modifier_brightness(0.5);
   imlib_modify_color_modifier_contrast(0.5);
#endif
   imlib_set_color_modifier_tables(r, g, b, a);
   imlib_context_set_color_modifier(NULL);
}
