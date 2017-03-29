/*
 * Copyright © 2001 Keith Packard
 *             2010 Intel Corporation
 *             2012 Advanced Micro Devices, Inc.
 *
 * Partly based on code Copyright © 2008 Red Hat, Inc.
 * Partly based on code Copyright © 2000 SuSE, Inc.
 *
 * Partly based on code that is Copyright © The XFree86 Project Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the opyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */


#ifdef HAVE_DIX_CONFIG_H
#include <dix-config.h>
#endif

#include <stdlib.h>

#include "radeon_glamor_wrappers.h"
#include "mipict.h"


#if HAS_DEVPRIVATEKEYREC
DevPrivateKeyRec glamor_screen_index;
#else
int glamor_screen_index;
#endif

/**
 * glamor_get_drawable_pixmap() returns a backing pixmap for a given drawable.
 *
 * @param pDrawable the drawable being requested.
 *
 * This function returns the backing pixmap for a drawable, whether it is a
 * redirected window, unredirected window, or already a pixmap.  Note that
 * coordinate translation is needed when drawing to the backing pixmap of a
 * redirected window, and the translation coordinates are provided by calling
 * glamor_get_drawable_pixmap() on the drawable.
 */
static PixmapPtr glamor_get_drawable_pixmap(DrawablePtr pDrawable)
{
	if (pDrawable->type == DRAWABLE_WINDOW)
		return pDrawable->pScreen->
		    GetWindowPixmap((WindowPtr) pDrawable);
	else
		return (PixmapPtr) pDrawable;
}

/**
 * Sets the offsets to add to coordinates to make them address the same bits in
 * the backing drawable. These coordinates are nonzero only for redirected
 * windows.
 */
static void
glamor_get_drawable_deltas(DrawablePtr pDrawable, PixmapPtr pPixmap,
			int *xp, int *yp)
{
#ifdef COMPOSITE
	if (pDrawable->type == DRAWABLE_WINDOW) {
		*xp = -pPixmap->screen_x;
		*yp = -pPixmap->screen_y;
		return;
	}
#endif

	*xp = 0;
	*yp = 0;
}

/**
 * glamor_drawable_is_offscreen() is a convenience wrapper for
 * radeon_glamor_pixmap_is_offscreen().
 */
static Bool glamor_drawable_is_offscreen(DrawablePtr pDrawable)
{
	return radeon_glamor_pixmap_is_offscreen(glamor_get_drawable_pixmap(pDrawable));
}

/**
 * glamor_prepare_access() is GLAMOR's wrapper for the driver's PrepareAccess() handler.
 *
 * It deals with waiting for synchronization with the card, determining if
 * PrepareAccess() is necessary, and working around PrepareAccess() failure.
 */
static Bool glamor_prepare_access(DrawablePtr pDrawable, glamor_access_t access)
{
	PixmapPtr pPixmap = glamor_get_drawable_pixmap(pDrawable);

	return radeon_glamor_prepare_access(pPixmap, access);
}

/**
 * glamor_finish_access() is GLAMOR's wrapper for the driver's finish_access() handler.
 *
 * It deals with calling the driver's finish_access() only if necessary.
 */
static void glamor_finish_access(DrawablePtr pDrawable, glamor_access_t access)
{
	PixmapPtr pPixmap = glamor_get_drawable_pixmap(pDrawable);

	radeon_glamor_finish_access(pPixmap, access);
}

static Bool glamor_prepare_access_window(WindowPtr pWin)
{
	if (pWin->backgroundState == BackgroundPixmap) {
		if (!glamor_prepare_access
		    (&pWin->background.pixmap->drawable, GLAMOR_CPU_ACCESS_RO))
			return FALSE;
	}

	if (pWin->borderIsPixel == FALSE) {
		if (!glamor_prepare_access
		    (&pWin->border.pixmap->drawable, GLAMOR_CPU_ACCESS_RO)) {
			if (pWin->backgroundState == BackgroundPixmap)
				glamor_finish_access(&pWin->background.pixmap->
						  drawable, GLAMOR_CPU_ACCESS_RO);
			return FALSE;
		}
	}
	return TRUE;
}

static void glamor_finish_access_window(WindowPtr pWin)
{
	if (pWin->backgroundState == BackgroundPixmap)
		glamor_finish_access(&pWin->background.pixmap->drawable, GLAMOR_CPU_ACCESS_RO);

	if (pWin->borderIsPixel == FALSE)
		glamor_finish_access(&pWin->border.pixmap->drawable, GLAMOR_CPU_ACCESS_RO);
}

static Bool glamor_change_window_attributes(WindowPtr pWin, unsigned long mask)
{
	Bool ret;

	if (!glamor_prepare_access_window(pWin))
		return FALSE;
	ret = fbChangeWindowAttributes(pWin, mask);
	glamor_finish_access_window(pWin);
	return ret;
}

static RegionPtr glamor_bitmap_to_region(PixmapPtr pPix)
{
	RegionPtr ret;
	if (!glamor_prepare_access(&pPix->drawable, GLAMOR_CPU_ACCESS_RO))
		return NULL;
	ret = fbPixmapToRegion(pPix);
	glamor_finish_access(&pPix->drawable, GLAMOR_CPU_ACCESS_RO);
	return ret;
}

void glamor_set_fallback_debug(ScreenPtr screen, Bool enable)
{
	glamor_screen_t *glamor_screen = glamor_get_screen(screen);

	glamor_screen->fallback_debug = enable;
}


/*
 * These functions wrap the low-level fb rendering functions and
 * synchronize framebuffer/accelerated drawing by stalling until
 * the accelerator is idle
 */

/**
 * Calls glamor_prepare_access with GLAMOR_PREPARE_SRC for the tile, if that is the
 * current fill style.
 *
 * Solid doesn't use an extra pixmap source, and Stippled/OpaqueStippled are
 * 1bpp and never in fb, so we don't worry about them.
 * We should worry about them for completeness sake and going forward.
 */
static Bool glamor_prepare_access_gc(GCPtr pGC)
{
	if (pGC->stipple)
		if (!glamor_prepare_access(&pGC->stipple->drawable, GLAMOR_CPU_ACCESS_RO))
			return FALSE;
	if (pGC->fillStyle == FillTiled)
		if (!glamor_prepare_access
		    (&pGC->tile.pixmap->drawable, GLAMOR_CPU_ACCESS_RO)) {
			if (pGC->stipple)
				glamor_finish_access(&pGC->stipple->drawable, GLAMOR_CPU_ACCESS_RO);
			return FALSE;
		}
	return TRUE;
}

/**
 * Finishes access to the tile in the GC, if used.
 */
static void glamor_finish_access_gc(GCPtr pGC)
{
	if (pGC->fillStyle == FillTiled)
		glamor_finish_access(&pGC->tile.pixmap->drawable, GLAMOR_CPU_ACCESS_RO);
	if (pGC->stipple)
		glamor_finish_access(&pGC->stipple->drawable, GLAMOR_CPU_ACCESS_RO);
}

static Bool glamor_picture_prepare_access(PicturePtr picture, int mode)
{
	if (picture->pDrawable == NULL)
		return TRUE;

	if (!glamor_prepare_access(picture->pDrawable, mode))
		return FALSE;

	if (picture->alphaMap &&
	    !glamor_prepare_access(picture->alphaMap->pDrawable, mode)) {
		glamor_finish_access(picture->pDrawable, mode);
		return FALSE;
	}

	return TRUE;
}

static void glamor_picture_finish_access(PicturePtr picture, int mode)
{
	if (picture->pDrawable == NULL)
		return;

	glamor_finish_access(picture->pDrawable, mode);
	if (picture->alphaMap)
		glamor_finish_access(picture->alphaMap->pDrawable, mode);
}


static char glamor_drawable_location(DrawablePtr pDrawable)
{
	return glamor_drawable_is_offscreen(pDrawable) ? 's' : 'm';
}

static void
glamor_check_fill_spans(DrawablePtr pDrawable, GCPtr pGC, int nspans,
		     DDXPointPtr ppt, int *pwidth, int fSorted)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access_gc(pGC)) {
			fbFillSpans(pDrawable, pGC, nspans, ppt, pwidth,
				    fSorted);
			glamor_finish_access_gc(pGC);
		}
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_set_spans(DrawablePtr pDrawable, GCPtr pGC, char *psrc,
		    DDXPointPtr ppt, int *pwidth, int nspans, int fSorted)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		fbSetSpans(pDrawable, pGC, psrc, ppt, pwidth, nspans, fSorted);
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_put_image(DrawablePtr pDrawable, GCPtr pGC, int depth,
		    int x, int y, int w, int h, int leftPad, int format,
		    char *bits)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		fbPutImage(pDrawable, pGC, depth, x, y, w, h, leftPad, format,
			   bits);
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static RegionPtr
glamor_check_copy_plane(DrawablePtr pSrc, DrawablePtr pDst, GCPtr pGC,
		     int srcx, int srcy, int w, int h, int dstx, int dsty,
		     unsigned long bitPlane)
{
	ScreenPtr screen = pSrc->pScreen;
	RegionPtr ret = NULL;

	GLAMOR_FALLBACK(("from %p to %p (%c,%c)\n", pSrc, pDst,
		      glamor_drawable_location(pSrc),
		      glamor_drawable_location(pDst)));
	if (glamor_prepare_access(pDst, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access(pSrc, GLAMOR_CPU_ACCESS_RO)) {
			ret =
			    fbCopyPlane(pSrc, pDst, pGC, srcx, srcy, w, h, dstx,
					dsty, bitPlane);
			glamor_finish_access(pSrc, GLAMOR_CPU_ACCESS_RO);
		}
		glamor_finish_access(pDst, GLAMOR_CPU_ACCESS_RW);
	}
	return ret;
}

static void
glamor_check_poly_point(DrawablePtr pDrawable, GCPtr pGC, int mode, int npt,
		     DDXPointPtr pptInit)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		fbPolyPoint(pDrawable, pGC, mode, npt, pptInit);
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_poly_lines(DrawablePtr pDrawable, GCPtr pGC,
		     int mode, int npt, DDXPointPtr ppt)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c), width %d, mode %d, count %d\n",
		      pDrawable, glamor_drawable_location(pDrawable),
		      pGC->lineWidth, mode, npt));

	if (pGC->lineWidth == 0) {
		if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
			if (glamor_prepare_access_gc(pGC)) {
				fbPolyLine(pDrawable, pGC, mode, npt, ppt);
				glamor_finish_access_gc(pGC);
			}
			glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
		}
		return;
	}
	/* fb calls mi functions in the lineWidth != 0 case. */
	fbPolyLine(pDrawable, pGC, mode, npt, ppt);
}

static void
glamor_check_poly_segment(DrawablePtr pDrawable, GCPtr pGC,
		       int nsegInit, xSegment * pSegInit)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c) width %d, count %d\n", pDrawable,
		      glamor_drawable_location(pDrawable), pGC->lineWidth,
		      nsegInit));
	if (pGC->lineWidth == 0) {
		if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
			if (glamor_prepare_access_gc(pGC)) {
				fbPolySegment(pDrawable, pGC, nsegInit,
					      pSegInit);
				glamor_finish_access_gc(pGC);
			}
			glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
		}
		return;
	}
	/* fb calls mi functions in the lineWidth != 0 case. */
	fbPolySegment(pDrawable, pGC, nsegInit, pSegInit);
}

static void
glamor_check_poly_arc(DrawablePtr pDrawable, GCPtr pGC, int narcs, xArc * pArcs)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));

	/* Disable this as fbPolyArc can call miZeroPolyArc which in turn
	 * can call accelerated functions, that as yet, haven't been notified
	 * with glamor_finish_access().
	 */
#if 0
	if (pGC->lineWidth == 0) {
		if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
			if (glamor_prepare_access_gc(pGC)) {
				fbPolyArc(pDrawable, pGC, narcs, pArcs);
				glamor_finish_access_gc(pGC);
			}
			glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
		}
		return;
	}
#endif
	miPolyArc(pDrawable, pGC, narcs, pArcs);
}

static void
glamor_check_poly_fill_rect(DrawablePtr pDrawable, GCPtr pGC,
			 int nrect, xRectangle * prect)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));

	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access_gc(pGC)) {
			fbPolyFillRect(pDrawable, pGC, nrect, prect);
			glamor_finish_access_gc(pGC);
		}
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_image_glyph_blt(DrawablePtr pDrawable, GCPtr pGC,
			  int x, int y, unsigned int nglyph,
			  CharInfoPtr * ppci, pointer pglyphBase)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access_gc(pGC)) {
			fbImageGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci,
					pglyphBase);
			glamor_finish_access_gc(pGC);
		}
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_poly_glyph_blt(DrawablePtr pDrawable, GCPtr pGC,
			 int x, int y, unsigned int nglyph,
			 CharInfoPtr * ppci, pointer pglyphBase)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("to %p (%c), style %d alu %d\n", pDrawable,
		      glamor_drawable_location(pDrawable), pGC->fillStyle,
		      pGC->alu));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access_gc(pGC)) {
			fbPolyGlyphBlt(pDrawable, pGC, x, y, nglyph, ppci,
				       pglyphBase);
			glamor_finish_access_gc(pGC);
		}
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_push_pixels(GCPtr pGC, PixmapPtr pBitmap,
		      DrawablePtr pDrawable, int w, int h, int x, int y)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("from %p to %p (%c,%c)\n", pBitmap, pDrawable,
		      glamor_drawable_location(&pBitmap->drawable),
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_prepare_access(&pBitmap->drawable, GLAMOR_CPU_ACCESS_RO)) {
			if (glamor_prepare_access_gc(pGC)) {
				fbPushPixels(pGC, pBitmap, pDrawable, w, h, x,
					     y);
				glamor_finish_access_gc(pGC);
			}
			glamor_finish_access(&pBitmap->drawable, GLAMOR_CPU_ACCESS_RO);
		}
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_get_spans(DrawablePtr pDrawable,
		    int wMax,
		    DDXPointPtr ppt, int *pwidth, int nspans, char *pdstStart)
{
	ScreenPtr screen = pDrawable->pScreen;

	GLAMOR_FALLBACK(("from %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));
	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RO)) {
		fbGetSpans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RO);
	}
}

static void
glamor_check_composite(CARD8 op,
		    PicturePtr pSrc,
		    PicturePtr pMask,
		    PicturePtr pDst,
		    INT16 xSrc, INT16 ySrc,
		    INT16 xMask, INT16 yMask,
		    INT16 xDst, INT16 yDst,
		    CARD16 width, CARD16 height)
{
	ScreenPtr screen = pDst->pDrawable->pScreen;

	GLAMOR_FALLBACK(("from picts %p/%p to pict %p\n", pSrc, pMask, pDst));

	if (glamor_picture_prepare_access(pDst, GLAMOR_CPU_ACCESS_RW)) {
		if (glamor_picture_prepare_access(pSrc, GLAMOR_CPU_ACCESS_RO)) {
			if (!pMask || glamor_picture_prepare_access(pMask, GLAMOR_CPU_ACCESS_RO)) {
				fbComposite(op, pSrc, pMask, pDst,
					    xSrc, ySrc,
					    xMask, yMask,
					    xDst, yDst,
					    width, height);
				if (pMask)
					glamor_picture_finish_access(pMask, GLAMOR_CPU_ACCESS_RO);
			}
			glamor_picture_finish_access(pSrc, GLAMOR_CPU_ACCESS_RO);
		}
		glamor_picture_finish_access(pDst, GLAMOR_CPU_ACCESS_RW);
	}
}

static void
glamor_check_add_traps(PicturePtr pPicture,
		    INT16 x_off, INT16 y_off, int ntrap, xTrap * traps)
{
	ScreenPtr screen = pPicture->pDrawable->pScreen;

	GLAMOR_FALLBACK(("to pict %p (%c)\n", pPicture,
		      glamor_drawable_location(pPicture->pDrawable)));
	if (glamor_picture_prepare_access(pPicture, GLAMOR_CPU_ACCESS_RW)) {
		fbAddTraps(pPicture, x_off, y_off, ntrap, traps);
		glamor_picture_finish_access(pPicture, GLAMOR_CPU_ACCESS_RW);
	}
}


static void
glamor_fill_spans(DrawablePtr pDrawable, GCPtr pGC, int n,
	       DDXPointPtr ppt, int *pwidth, int fSorted)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_fill_spans_nf(pDrawable,
					  pGC, n, ppt, pwidth, fSorted);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_fill_spans(pDrawable, pGC, n, ppt, pwidth, fSorted);
}

static void
glamor_put_image(DrawablePtr pDrawable, GCPtr pGC, int depth, int x, int y,
	      int w, int h, int leftPad, int format, char *bits)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_put_image_nf(pDrawable,
					 pGC, depth, x, y, w, h,
					 leftPad, format, bits);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_put_image(pDrawable, pGC, depth, x, y, w, h, leftPad,
				       format, bits);
}

static void
glamor_copy_n_to_n(DrawablePtr pSrcDrawable,
		DrawablePtr pDstDrawable,
		GCPtr pGC,
		BoxPtr pbox,
		int nbox,
		int dx,
		int dy,
		Bool reverse, Bool upsidedown, Pixel bitplane, void *closure)
{
	ScreenPtr screen = pDstDrawable->pScreen;
	int ok;

	if (!glamor_prepare_access(pSrcDrawable, GLAMOR_GPU_ACCESS_RO))
		goto fallback;
	ok = glamor_prepare_access(pDstDrawable, GLAMOR_GPU_ACCESS_RW);
	if (!ok)
		goto finish_src;
	ok = glamor_copy_n_to_n_nf(pSrcDrawable, pDstDrawable,
				   pGC, pbox, nbox, dx, dy,
				   reverse, upsidedown, bitplane,
				   closure);
	glamor_finish_access(pDstDrawable, GLAMOR_GPU_ACCESS_RW);
finish_src:
	glamor_finish_access(pSrcDrawable, GLAMOR_GPU_ACCESS_RO);

	if (ok)
		return;

fallback:
	GLAMOR_FALLBACK(("from %p to %p (%c,%c)\n", pSrcDrawable, pDstDrawable,
		      glamor_drawable_location(pSrcDrawable),
		      glamor_drawable_location(pDstDrawable)));
	if (glamor_prepare_access(pDstDrawable, GLAMOR_CPU_ACCESS_RW)) {
		if (pSrcDrawable == pDstDrawable ||
		    glamor_prepare_access(pSrcDrawable, GLAMOR_CPU_ACCESS_RO)) {
			fbCopyNtoN(pSrcDrawable, pDstDrawable, pGC, pbox, nbox,
				   dx, dy, reverse, upsidedown, bitplane,
				   closure);
			if (pSrcDrawable != pDstDrawable)
				glamor_finish_access(pSrcDrawable, GLAMOR_CPU_ACCESS_RO);
		}
		glamor_finish_access(pDstDrawable, GLAMOR_CPU_ACCESS_RW);
	}
}

static RegionPtr
glamor_copy_area(DrawablePtr pSrcDrawable, DrawablePtr pDstDrawable, GCPtr pGC,
	      int srcx, int srcy, int width, int height, int dstx, int dsty)
{
	return miDoCopy(pSrcDrawable, pDstDrawable, pGC,
			srcx, srcy, width, height,
			dstx, dsty, glamor_copy_n_to_n, 0, NULL);
}

static void
glamor_poly_point(DrawablePtr pDrawable, GCPtr pGC, int mode, int npt,
	       DDXPointPtr ppt)
{
	int i;
	xRectangle *prect;
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_poly_point_nf(pDrawable, pGC, mode, npt, ppt);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (ok)
		return;

	/* If we can't reuse the current GC as is, don't bother accelerating the
	 * points.
	 */
	if (pGC->fillStyle != FillSolid) {
		glamor_check_poly_point(pDrawable, pGC, mode, npt, ppt);
		return;
	}

	prect = malloc(sizeof(xRectangle) * npt);
	if (!prect)
		return;
	for (i = 0; i < npt; i++) {
		prect[i].x = ppt[i].x;
		prect[i].y = ppt[i].y;
		if (i > 0 && mode == CoordModePrevious) {
			prect[i].x += prect[i - 1].x;
			prect[i].y += prect[i - 1].y;
		}
		prect[i].width = 1;
		prect[i].height = 1;
	}
	pGC->ops->PolyFillRect(pDrawable, pGC, npt, prect);
	free(prect);
}

/**
 * glamor_poly_lines() checks if it can accelerate the lines as a group of
 * horizontal or vertical lines (rectangles), and uses existing rectangle fill
 * acceleration if so.
 */
static void
glamor_poly_lines(DrawablePtr pDrawable, GCPtr pGC, int mode, int npt,
	       DDXPointPtr ppt)
{
	xRectangle *prect;
	int x1, x2, y1, y2;
	int i;
	int ok;

	if (!glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_poly_lines_nf(pDrawable, pGC, mode, npt, ppt);
	glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	/* Don't try to do wide lines or non-solid fill style. */
	if (pGC->lineWidth != 0 || pGC->lineStyle != LineSolid ||
	    pGC->fillStyle != FillSolid) {
		glamor_check_poly_lines(pDrawable, pGC, mode, npt, ppt);
		return;
	}

	prect = malloc(sizeof(xRectangle) * (npt - 1));
	if (!prect)
		return;
	x1 = ppt[0].x;
	y1 = ppt[0].y;
	/* If we have any non-horizontal/vertical, fall back. */
	for (i = 0; i < npt - 1; i++) {
		if (mode == CoordModePrevious) {
			x2 = x1 + ppt[i + 1].x;
			y2 = y1 + ppt[i + 1].y;
		} else {
			x2 = ppt[i + 1].x;
			y2 = ppt[i + 1].y;
		}

		if (x1 != x2 && y1 != y2) {
			free(prect);
			glamor_check_poly_lines(pDrawable, pGC, mode, npt, ppt);
			return;
		}

		if (x1 < x2) {
			prect[i].x = x1;
			prect[i].width = x2 - x1 + 1;
		} else {
			prect[i].x = x2;
			prect[i].width = x1 - x2 + 1;
		}
		if (y1 < y2) {
			prect[i].y = y1;
			prect[i].height = y2 - y1 + 1;
		} else {
			prect[i].y = y2;
			prect[i].height = y1 - y2 + 1;
		}

		x1 = x2;
		y1 = y2;
	}
	pGC->ops->PolyFillRect(pDrawable, pGC, npt - 1, prect);
	free(prect);
}

/**
 * glamor_poly_segment() checks if it can accelerate the lines as a group of
 * horizontal or vertical lines (rectangles), and uses existing rectangle fill
 * acceleration if so.
 */
static void
glamor_poly_segment(DrawablePtr pDrawable, GCPtr pGC, int nseg, xSegment * pSeg)
{
	xRectangle *prect;
	int i;
	int ok;

	if (!glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_poly_segment_nf(pDrawable, pGC, nseg, pSeg);
	glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	/* Don't try to do wide lines or non-solid fill style. */
	if (pGC->lineWidth != 0 || pGC->lineStyle != LineSolid ||
	    pGC->fillStyle != FillSolid) {
		glamor_check_poly_segment(pDrawable, pGC, nseg, pSeg);
		return;
	}

	/* If we have any non-horizontal/vertical, fall back. */
	for (i = 0; i < nseg; i++) {
		if (pSeg[i].x1 != pSeg[i].x2 && pSeg[i].y1 != pSeg[i].y2) {
			glamor_check_poly_segment(pDrawable, pGC, nseg, pSeg);
			return;
		}
	}

	prect = malloc(sizeof(xRectangle) * nseg);
	if (!prect)
		return;
	for (i = 0; i < nseg; i++) {
		if (pSeg[i].x1 < pSeg[i].x2) {
			prect[i].x = pSeg[i].x1;
			prect[i].width = pSeg[i].x2 - pSeg[i].x1 + 1;
		} else {
			prect[i].x = pSeg[i].x2;
			prect[i].width = pSeg[i].x1 - pSeg[i].x2 + 1;
		}
		if (pSeg[i].y1 < pSeg[i].y2) {
			prect[i].y = pSeg[i].y1;
			prect[i].height = pSeg[i].y2 - pSeg[i].y1 + 1;
		} else {
			prect[i].y = pSeg[i].y2;
			prect[i].height = pSeg[i].y1 - pSeg[i].y2 + 1;
		}

		/* don't paint last pixel */
		if (pGC->capStyle == CapNotLast) {
			if (prect[i].width == 1)
				prect[i].height--;
			else
				prect[i].width--;
		}
	}
	pGC->ops->PolyFillRect(pDrawable, pGC, nseg, prect);
	free(prect);
}

static void
glamor_poly_fill_rect(DrawablePtr pDrawable,
		   GCPtr pGC, int nrect, xRectangle * prect)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_poly_fill_rect_nf(pDrawable, pGC, nrect, prect);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_poly_fill_rect(pDrawable, pGC, nrect, prect);
}

static void
glamor_get_spans(DrawablePtr pDrawable,
	      int wMax,
	      DDXPointPtr ppt, int *pwidth, int nspans, char *pdstStart)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_get_spans_nf(pDrawable, wMax, ppt,
					 pwidth, nspans, pdstStart);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_get_spans(pDrawable, wMax, ppt, pwidth, nspans, pdstStart);
}

static void
glamor_set_spans(DrawablePtr pDrawable, GCPtr gc, char *src,
                 DDXPointPtr points, int *widths, int n, int sorted)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_set_spans_nf(pDrawable, gc, src,
					 points, widths, n, sorted);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_set_spans(pDrawable, gc, src, points, widths, n, sorted);
}

static RegionPtr
glamor_copy_plane(DrawablePtr pSrc, DrawablePtr pDst, GCPtr pGC,
	       int srcx, int srcy, int w, int h, int dstx, int dsty,
	       unsigned long bitPlane)
{
	int ok;
	RegionPtr region;

	if (!glamor_prepare_access(pDst, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_prepare_access(pSrc, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_dst;
	ok = glamor_copy_plane_nf(pSrc, pDst, pGC, srcx, srcy, w, h,
				  dstx, dsty, bitPlane, &region);
	glamor_finish_access(pSrc, GLAMOR_GPU_ACCESS_RO);
finish_dst:
	glamor_finish_access(pDst, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return region;

fallback:
	return glamor_check_copy_plane(pSrc, pDst, pGC, srcx, srcy, w, h,
				    dstx, dsty, bitPlane);
}

static void
glamor_image_glyph_blt(DrawablePtr pDrawable, GCPtr pGC,
		    int x, int y, unsigned int nglyph,
		    CharInfoPtr * ppci, pointer pglyphBase)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_image_glyph_blt_nf(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_image_glyph_blt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase);
}

static void
glamor_poly_glyph_blt(DrawablePtr pDrawable, GCPtr pGC,
		   int x, int y, unsigned int nglyph,
		   CharInfoPtr * ppci, pointer pglyphBase)
{
	int ok;

	ok = glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_poly_glyph_blt_nf(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase);
		glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_poly_glyph_blt(pDrawable, pGC, x, y, nglyph, ppci, pglyphBase);
}

static void
glamor_push_pixels(GCPtr pGC, PixmapPtr pBitmap,
		DrawablePtr pDrawable, int w, int h, int x, int y)
{
	int ok;

	if (!glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_prepare_access(&pBitmap->drawable, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_drawable;
	ok = glamor_push_pixels_nf(pGC, pBitmap, pDrawable, w, h, x, y);
	glamor_finish_access(&pBitmap->drawable, GLAMOR_GPU_ACCESS_RO);
finish_drawable:
	glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW);

	if (ok)
	  return;

fallback:
	glamor_check_push_pixels(pGC, pBitmap, pDrawable, w, h, x, y);
}

const GCOps glamor_ops = {
	glamor_fill_spans,
	glamor_set_spans,
	glamor_put_image,
	glamor_copy_area,
	glamor_copy_plane,
	glamor_poly_point,
	glamor_poly_lines,
	glamor_poly_segment,
	miPolyRectangle,
	glamor_check_poly_arc,
	miFillPolygon,
	glamor_poly_fill_rect,
	miPolyFillArc,
	miPolyText8,
	miPolyText16,
	miImageText8,
	miImageText16,
	glamor_image_glyph_blt,
	glamor_poly_glyph_blt,
	glamor_push_pixels,
};

/**
 * glamor_validate_gc() sets the ops to GLAMOR's implementations, which may be
 * accelerated or may sync the card and fall back to fb.
 */
static void
radeon_glamor_validate_gc(GCPtr pGC, unsigned long changes, DrawablePtr pDrawable)
{
	glamor_validate_gc(pGC, changes, pDrawable);
	pGC->ops = (GCOps *) & glamor_ops;
}

static GCFuncs glamorGCFuncs = {
	radeon_glamor_validate_gc,
	miChangeGC,
	miCopyGC,
	miDestroyGC,
	miChangeClip,
	miDestroyClip,
	miCopyClip
};

/**
 * radeon_glamor_create_gc makes a new GC and hooks up its funcs handler, so that
 * radeon_glamor_validate_gc() will get called.
 */
static int radeon_glamor_create_gc(GCPtr pGC)
{
	if (!fbCreateGC(pGC))
		return FALSE;

	pGC->funcs = &glamorGCFuncs;

	return TRUE;
}

static void glamor_copy_window(WindowPtr pWin, DDXPointRec ptOldOrg, RegionPtr prgnSrc)
{
	RegionRec rgnDst;
	int dx, dy;
	PixmapPtr pPixmap = (*pWin->drawable.pScreen->GetWindowPixmap) (pWin);

	dx = ptOldOrg.x - pWin->drawable.x;
	dy = ptOldOrg.y - pWin->drawable.y;
	REGION_TRANSLATE(pWin->drawable.pScreen, prgnSrc, -dx, -dy);

	REGION_INIT(pWin->drawable.pScreen, &rgnDst, NullBox, 0);

	REGION_INTERSECT(pWin->drawable.pScreen, &rgnDst, &pWin->borderClip,
			 prgnSrc);
#ifdef COMPOSITE
	if (pPixmap->screen_x || pPixmap->screen_y)
		REGION_TRANSLATE(pWin->drawable.pScreen, &rgnDst,
				 -pPixmap->screen_x, -pPixmap->screen_y);
#endif

	miCopyRegion(&pPixmap->drawable, &pPixmap->drawable,
		     NULL, &rgnDst, dx, dy, glamor_copy_n_to_n, 0, NULL);

	REGION_UNINIT(pWin->drawable.pScreen, &rgnDst);
}

/**
 * Accelerates GetImage for solid ZPixmap downloads from framebuffer memory.
 *
 * This is probably the only case we actually care about.  The rest fall through
 * to migration and fbGetImage, which hopefully will result in migration pushing
 * the pixmap out of framebuffer.
 */
void
static glamor_get_image(DrawablePtr pDrawable, int x, int y, int w, int h,
	      unsigned int format, unsigned long planeMask, char *d)
{
	ScreenPtr screen = pDrawable->pScreen;
	BoxRec Box;
	PixmapPtr pPix = glamor_get_drawable_pixmap(pDrawable);
	int xoff, yoff;
	Bool ok;

	if (!glamor_prepare_access(pDrawable, GLAMOR_GPU_ACCESS_RW))
		goto fallback;

	glamor_get_drawable_deltas(pDrawable, pPix, &xoff, &yoff);

	Box.x1 = pDrawable->y + x + xoff;
	Box.y1 = pDrawable->y + y + yoff;
	Box.x2 = Box.x1 + w;
	Box.y2 = Box.y1 + h;

	ok = glamor_get_image_nf(pDrawable, x, y, w, h,
				 format, planeMask, d);
	glamor_finish_access(pDrawable, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	GLAMOR_FALLBACK(("from %p (%c)\n", pDrawable,
		      glamor_drawable_location(pDrawable)));

	if (glamor_prepare_access(pDrawable, GLAMOR_CPU_ACCESS_RO)) {
		fbGetImage(pDrawable, x, y, w, h, format, planeMask, d);
		glamor_finish_access(pDrawable, GLAMOR_CPU_ACCESS_RO);
	}

	return;
}


/* Cut and paste from render/glyph.c - probably should export it instead */
static void
glamor_glyph_extents(int nlist,
		  GlyphListPtr list, GlyphPtr * glyphs, BoxPtr extents)
{
	int x1, x2, y1, y2;
	int x, y, n;

	x1 = y1 = MAXSHORT;
	x2 = y2 = MINSHORT;
	x = y = 0;
	while (nlist--) {
		x += list->xOff;
		y += list->yOff;
		n = list->len;
		list++;
		while (n--) {
			GlyphPtr glyph = *glyphs++;
			int v;

			v = x - glyph->info.x;
			if (v < x1)
			    x1 = v;
			v += glyph->info.width;
			if (v > x2)
			    x2 = v;

			v = y - glyph->info.y;
			if (v < y1)
			    y1 = v;
			v += glyph->info.height;
			if (v > y2)
			    y2 = v;

			x += glyph->info.xOff;
			y += glyph->info.yOff;
		}
	}

	extents->x1 = x1 < MINSHORT ? MINSHORT : x1;
	extents->x2 = x2 > MAXSHORT ? MAXSHORT : x2;
	extents->y1 = y1 < MINSHORT ? MINSHORT : y1;
	extents->y2 = y2 > MAXSHORT ? MAXSHORT : y2;
}

#define NeedsComponent(f) (PICT_FORMAT_A(f) != 0 && PICT_FORMAT_RGB(f) != 0)

static void
glamor_check_glyphs(CARD8 op,
		 PicturePtr src,
		 PicturePtr dst,
		 PictFormatPtr maskFormat,
		 INT16 xSrc,
		 INT16 ySrc, int nlist, GlyphListPtr list, GlyphPtr * glyphs)
{
	pixman_image_t *image;
	PixmapPtr scratch;
	PicturePtr mask;
	int width = 0, height = 0;
	int x, y, n;
	int xDst = list->xOff, yDst = list->yOff;
	BoxRec extents = { 0, 0, 0, 0 };

	if (maskFormat) {
		pixman_format_code_t format;
		CARD32 component_alpha;
		int error;

		glamor_glyph_extents(nlist, list, glyphs, &extents);
		if (extents.x2 <= extents.x1 || extents.y2 <= extents.y1)
			return;

		width = extents.x2 - extents.x1;
		height = extents.y2 - extents.y1;

		format = maskFormat->format |
			(BitsPerPixel(maskFormat->depth) << 24);
		image =
			pixman_image_create_bits(format, width, height, NULL, 0);
		if (!image)
			return;

		scratch = GetScratchPixmapHeader(dst->pDrawable->pScreen, width, height,
						 PIXMAN_FORMAT_DEPTH(format),
						 PIXMAN_FORMAT_BPP(format),
						 pixman_image_get_stride(image),
						 pixman_image_get_data(image));

		if (!scratch) {
			pixman_image_unref(image);
			return;
		}

		component_alpha = NeedsComponent(maskFormat->format);
		mask = CreatePicture(0, &scratch->drawable,
				     maskFormat, CPComponentAlpha,
				     &component_alpha, serverClient, &error);
		if (!mask) {
			FreeScratchPixmapHeader(scratch);
			pixman_image_unref(image);
			return;
		}
		ValidatePicture(mask);

		x = -extents.x1;
		y = -extents.y1;
	} else {
		mask = dst;
		x = 0;
		y = 0;
	}

	while (nlist--) {
		x += list->xOff;
		y += list->yOff;
		n = list->len;
		while (n--) {
			GlyphPtr glyph = *glyphs++;
			PicturePtr g = GetGlyphPicture(glyph, dst->pDrawable->pScreen);
			if (g) {
				if (maskFormat) {
					CompositePicture(PictOpAdd, g, NULL, mask,
							 0, 0,
							 0, 0,
							 x - glyph->info.x,
							 y - glyph->info.y,
							 glyph->info.width,
							 glyph->info.height);
				} else {
					CompositePicture(op, src, g, dst,
							 xSrc + (x - glyph->info.x) - xDst,
							 ySrc + (y - glyph->info.y) - yDst,
							 0, 0,
							 x - glyph->info.x,
							 y - glyph->info.y,
							 glyph->info.width,
							 glyph->info.height);
				}
			}

			x += glyph->info.xOff;
			y += glyph->info.yOff;
		}
		list++;
	}

	if (maskFormat) {
		x = extents.x1;
		y = extents.y1;
		CompositePicture(op, src, mask, dst,
				 xSrc + x - xDst,
				 ySrc + y - yDst,
				 0, 0,
				 x, y,
				 width, height);
		FreePicture(mask, 0);
		FreeScratchPixmapHeader(scratch);
		pixman_image_unref(image);
	}
}

void
glamor_glyphs(CARD8 op,
	   PicturePtr pSrc,
	   PicturePtr pDst,
	   PictFormatPtr maskFormat,
	   INT16 xSrc, INT16 ySrc,
	   int nlist, GlyphListPtr list, GlyphPtr * glyphs)
{
	int ok;

	if (!glamor_picture_prepare_access(pDst, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_picture_prepare_access(pSrc, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_dst;
	ok = glamor_glyphs_nf(op,
			      pSrc, pDst, maskFormat,
			      xSrc, ySrc, nlist, list, glyphs);
	glamor_picture_finish_access(pSrc, GLAMOR_GPU_ACCESS_RO);
finish_dst:
	glamor_picture_finish_access(pDst, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	glamor_check_glyphs(op, pSrc, pDst, maskFormat, xSrc, ySrc, nlist, list, glyphs);
}


#ifdef RENDER

/* Note: when using glamor we can not fail through to the ordinary GLAMOR
 * code paths, as glamor keeps an internal texture which will become
 * inconsistent with the original bo. (The texture is replaced whenever
 * the format changes, e.g. switching between xRGB and ARGB, for which mesa
 * will allocate its own bo.)
 *
 * Ergo it is unsafe to fall through to the original backend operations if
 * glamor is enabled.
 *
 * XXX This has some serious implications for mixing Render, DRI, scanout...
 */

static void glamor_composite_fallback_pict_desc(PicturePtr pict, char *string,
					     int n)
{
	char format[20];
	char size[20];
	char loc;

	if (!pict) {
		snprintf(string, n, "None");
		return;
	}

	if (pict->pDrawable == NULL) {
		snprintf(string, n, "source-only");
		return;
	}

	switch (pict->format) {
	case PICT_a8r8g8b8:
		snprintf(format, 20, "ARGB8888");
		break;
	case PICT_x8r8g8b8:
		snprintf(format, 20, "XRGB8888");
		break;
	case PICT_r5g6b5:
		snprintf(format, 20, "RGB565  ");
		break;
	case PICT_x1r5g5b5:
		snprintf(format, 20, "RGB555  ");
		break;
	case PICT_a8:
		snprintf(format, 20, "A8      ");
		break;
	case PICT_a1:
		snprintf(format, 20, "A1      ");
		break;
	default:
		snprintf(format, 20, "0x%x", (int)pict->format);
		break;
	}

	loc = glamor_drawable_is_offscreen(pict->pDrawable) ? 's' : 'm';

	snprintf(size, 20, "%dx%d%s", pict->pDrawable->width,
		 pict->pDrawable->height, pict->repeat ? " R" : "");

	snprintf(string, n, "%p:%c fmt %s (%s)%s",
		 pict->pDrawable, loc, format, size,
		 pict->alphaMap ? " with alpha map" :"");
}

static const char *
op_to_string(CARD8 op)
{
    switch (op) {
#define C(x) case PictOp##x: return #x
	C(Clear);
	C(Src);
	C(Dst);
	C(Over);
	C(OverReverse);
	C(In);
	C(InReverse);
	C(Out);
	C(OutReverse);
	C(Atop);
	C(AtopReverse);
	C(Xor);
	C(Add);
	C(Saturate);

	/*
	 * Operators only available in version 0.2
	 */
#if RENDER_MAJOR >= 1 || RENDER_MINOR >= 2
	C(DisjointClear);
	C(DisjointSrc);
	C(DisjointDst);
	C(DisjointOver);
	C(DisjointOverReverse);
	C(DisjointIn);
	C(DisjointInReverse);
	C(DisjointOut);
	C(DisjointOutReverse);
	C(DisjointAtop);
	C(DisjointAtopReverse);
	C(DisjointXor);

	C(ConjointClear);
	C(ConjointSrc);
	C(ConjointDst);
	C(ConjointOver);
	C(ConjointOverReverse);
	C(ConjointIn);
	C(ConjointInReverse);
	C(ConjointOut);
	C(ConjointOutReverse);
	C(ConjointAtop);
	C(ConjointAtopReverse);
	C(ConjointXor);
#endif

	/*
	 * Operators only available in version 0.11
	 */
#if RENDER_MAJOR >= 1 || RENDER_MINOR >= 11
	C(Multiply);
	C(Screen);
	C(Overlay);
	C(Darken);
	C(Lighten);
	C(ColorDodge);
	C(ColorBurn);
	C(HardLight);
	C(SoftLight);
	C(Difference);
	C(Exclusion);
	C(HSLHue);
	C(HSLSaturation);
	C(HSLColor);
	C(HSLLuminosity);
#endif
    default: return "garbage";
#undef C
    }
}

static void
glamor_print_composite_fallback(const char *func, CARD8 op,
			     PicturePtr pSrc, PicturePtr pMask, PicturePtr pDst)
{
	glamor_screen_t *glamor_screen = glamor_get_screen(pDst->pDrawable->pScreen);
	char srcdesc[40], maskdesc[40], dstdesc[40];

	if (! glamor_screen->fallback_debug)
		return;

	glamor_composite_fallback_pict_desc(pSrc, srcdesc, 40);
	glamor_composite_fallback_pict_desc(pMask, maskdesc, 40);
	glamor_composite_fallback_pict_desc(pDst, dstdesc, 40);

	ErrorF("Composite fallback at %s:\n"
	       "  op   %s, \n"
	       "  src  %s, \n"
	       "  mask %s, \n"
	       "  dst  %s, \n",
	       func, op_to_string (op), srcdesc, maskdesc, dstdesc);
}


static void
glamor_composite(CARD8 op,
	      PicturePtr pSrc,
	      PicturePtr pMask,
	      PicturePtr pDst,
	      INT16 xSrc, INT16 ySrc,
	      INT16 xMask, INT16 yMask,
	      INT16 xDst, INT16 yDst,
	      CARD16 width, CARD16 height)
{
	int ok;

	if (!glamor_picture_prepare_access(pDst, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_picture_prepare_access(pSrc, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_dst;
	if (pMask) {
		ok = glamor_picture_prepare_access(pMask, GLAMOR_GPU_ACCESS_RO);
		if (!ok)
			goto finish_src;
	}

	ok = glamor_composite_nf(op,
				 pSrc, pMask, pDst, xSrc, ySrc,
				 xMask, yMask, xDst, yDst,
				 width, height);

	if (pMask)
		glamor_picture_finish_access(pMask, GLAMOR_GPU_ACCESS_RO);
finish_src:
	glamor_picture_finish_access(pSrc, GLAMOR_GPU_ACCESS_RO);
finish_dst:
	glamor_picture_finish_access(pDst, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	glamor_print_composite_fallback("glamor_composite",
				     op, pSrc, pMask, pDst);

	glamor_check_composite(op, pSrc, pMask, pDst, xSrc, ySrc,
			    xMask, yMask, xDst, yDst, width, height);
}


static void
glamor_check_trapezoids(CARD8 op, PicturePtr src, PicturePtr dst,
		     PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
		     int ntrap, xTrapezoid * traps)
{
	ScreenPtr screen = dst->pDrawable->pScreen;

	if (maskFormat) {
		PixmapPtr scratch = NULL;
		PicturePtr mask;
		INT16 xDst, yDst;
		INT16 xRel, yRel;
		BoxRec bounds;
		int width, height;
		pixman_image_t *image;
		pixman_format_code_t format;
		int error;

		xDst = traps[0].left.p1.x >> 16;
		yDst = traps[0].left.p1.y >> 16;

		miTrapezoidBounds (ntrap, traps, &bounds);
		if (bounds.y1 >= bounds.y2 || bounds.x1 >= bounds.x2)
			return;

		width  = bounds.x2 - bounds.x1;
		height = bounds.y2 - bounds.y1;

		format = maskFormat->format |
			(BitsPerPixel(maskFormat->depth) << 24);
		image =
		    pixman_image_create_bits(format, width, height, NULL, 0);
		if (!image)
			return;

		for (; ntrap; ntrap--, traps++)
			pixman_rasterize_trapezoid(image,
						   (pixman_trapezoid_t *) traps,
						   -bounds.x1, -bounds.y1);


		scratch = GetScratchPixmapHeader(screen, width, height,
						 PIXMAN_FORMAT_DEPTH(format),
						 PIXMAN_FORMAT_BPP(format),
						 pixman_image_get_stride(image),
						 pixman_image_get_data(image));
		if (!scratch) {
			pixman_image_unref(image);
			return;
		}

		mask = CreatePicture(0, &scratch->drawable,
				     PictureMatchFormat(screen,
							PIXMAN_FORMAT_DEPTH(format),
							format),
				     0, 0, serverClient, &error);
		if (!mask) {
			FreeScratchPixmapHeader(scratch);
			pixman_image_unref(image);
			return;
		}

		xRel = bounds.x1 + xSrc - xDst;
		yRel = bounds.y1 + ySrc - yDst;
		CompositePicture(op, src, mask, dst,
				 xRel, yRel,
				 0, 0,
				 bounds.x1, bounds.y1,
				 width, height);
		FreePicture(mask, 0);

		FreeScratchPixmapHeader(scratch);
		pixman_image_unref(image);
	} else {
		if (dst->polyEdge == PolyEdgeSharp)
			maskFormat = PictureMatchFormat(screen, 1, PICT_a1);
		else
			maskFormat = PictureMatchFormat(screen, 8, PICT_a8);

		for (; ntrap; ntrap--, traps++)
			glamor_check_trapezoids(op, src, dst, maskFormat, xSrc, ySrc, 1, traps);
	}
}

/**
 * glamor_trapezoids is essentially a copy of miTrapezoids that uses
 * glamor_create_alpha_picture instead of miCreateAlphaPicture.
 *
 * The problem with miCreateAlphaPicture is that it calls PolyFillRect
 * to initialize the contents after creating the pixmap, which
 * causes the pixmap to be moved in for acceleration. The subsequent
 * call to RasterizeTrapezoid won't be accelerated however, which
 * forces the pixmap to be moved out again.
 *
 * glamor_create_alpha_picture avoids this roundtrip by using
 * glamor_check_poly_fill_rect to initialize the contents.
 */
static void
glamor_trapezoids(CARD8 op, PicturePtr src, PicturePtr dst,
		  PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
		  int ntrap, xTrapezoid * traps)
{
	int ok;

	if (!glamor_picture_prepare_access(dst, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_picture_prepare_access(src, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_dst;
	ok = glamor_trapezoids_nf(op,
				  src, dst, maskFormat, xSrc,
				  ySrc, ntrap, traps);
	glamor_picture_finish_access(src, GLAMOR_GPU_ACCESS_RO);
finish_dst:
	glamor_picture_finish_access(dst, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	glamor_check_trapezoids(op, src, dst, maskFormat, xSrc, ySrc, ntrap, traps);
}

static void
glamor_check_triangles(CARD8 op, PicturePtr src, PicturePtr dst,
		    PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
		    int ntri, xTriangle *tri)
{
	ScreenPtr screen = dst->pDrawable->pScreen;

	if (maskFormat) {
		PixmapPtr scratch = NULL;
		PicturePtr mask;
		INT16 xDst, yDst;
		INT16 xRel, yRel;
		BoxRec bounds;
		int width, height;
		pixman_image_t *image;
		pixman_format_code_t format;
		int error;

		xDst = pixman_fixed_to_int(tri[0].p1.x);
		yDst = pixman_fixed_to_int(tri[0].p1.y);

		miTriangleBounds (ntri, tri, &bounds);
		if (bounds.y1 >= bounds.y2 || bounds.x1 >= bounds.x2)
			return;

		width  = bounds.x2 - bounds.x1;
		height = bounds.y2 - bounds.y1;

		format = maskFormat->format |
			(BitsPerPixel(maskFormat->depth) << 24);
		image =
		    pixman_image_create_bits(format, width, height, NULL, 0);
		if (!image)
			return;

		pixman_add_triangles(image,
				     -bounds.x1, -bounds.y1,
				     ntri, (pixman_triangle_t *)tri);

		scratch = GetScratchPixmapHeader(screen, width, height,
						 PIXMAN_FORMAT_DEPTH(format),
						 PIXMAN_FORMAT_BPP(format),
						 pixman_image_get_stride(image),
						 pixman_image_get_data(image));
		if (!scratch) {
			pixman_image_unref(image);
			return;
		}

		mask = CreatePicture(0, &scratch->drawable,
				     PictureMatchFormat(screen,
							PIXMAN_FORMAT_DEPTH(format),
							format),
				     0, 0, serverClient, &error);
		if (!mask) {
			FreeScratchPixmapHeader(scratch);
			pixman_image_unref(image);
			return;
		}

		xRel = bounds.x1 + xSrc - xDst;
		yRel = bounds.y1 + ySrc - yDst;
		CompositePicture(op, src, mask, dst,
				 xRel, yRel,
				 0, 0,
				 bounds.x1, bounds.y1,
				 width, height);
		FreePicture(mask, 0);

		FreeScratchPixmapHeader(scratch);
		pixman_image_unref(image);
	} else {
		if (dst->polyEdge == PolyEdgeSharp)
			maskFormat = PictureMatchFormat(screen, 1, PICT_a1);
		else
			maskFormat = PictureMatchFormat(screen, 8, PICT_a8);

		for (; ntri; ntri--, tri++)
			glamor_check_triangles(op, src, dst, maskFormat, xSrc, ySrc, 1, tri);
	}
}

/**
 * glamor_triangles is essentially a copy of miTriangles that uses
 * glamor_create_alpha_picture instead of miCreateAlphaPicture.
 *
 * The problem with miCreateAlphaPicture is that it calls PolyFillRect
 * to initialize the contents after creating the pixmap, which
 * causes the pixmap to be moved in for acceleration. The subsequent
 * call to AddTriangles won't be accelerated however, which forces the pixmap
 * to be moved out again.
 *
 * glamor_create_alpha_picture avoids this roundtrip by using
 * glamor_check_poly_fill_rect to initialize the contents.
 */
static void
glamor_triangles(CARD8 op, PicturePtr pSrc, PicturePtr pDst,
	      PictFormatPtr maskFormat, INT16 xSrc, INT16 ySrc,
	      int ntri, xTriangle * tris)
{
	int ok;

	if (!glamor_picture_prepare_access(pDst, GLAMOR_GPU_ACCESS_RW))
		goto fallback;
	ok = glamor_picture_prepare_access(pSrc, GLAMOR_GPU_ACCESS_RO);
	if (!ok)
		goto finish_dst;
	ok = glamor_triangles_nf(op,
				 pSrc, pDst, maskFormat, xSrc,
				 ySrc, ntri, tris);
	glamor_picture_finish_access(pSrc, GLAMOR_GPU_ACCESS_RO);
finish_dst:
	glamor_picture_finish_access(pDst, GLAMOR_GPU_ACCESS_RW);

	if (ok)
		return;

fallback:
	glamor_check_triangles(op, pSrc, pDst, maskFormat,
			       xSrc, ySrc, ntri, tris);
}

void
glamor_add_traps(PicturePtr pPicture,
		 INT16 x_off, INT16 y_off, int ntrap, xTrap * traps)
{
	int ok;

	ok = glamor_picture_prepare_access(pPicture, GLAMOR_GPU_ACCESS_RW);
	if (ok) {
		ok = glamor_add_traps_nf(pPicture,
					 x_off, y_off, ntrap, traps);
		glamor_picture_finish_access(pPicture, GLAMOR_GPU_ACCESS_RW);
	}

	if (!ok)
		glamor_check_add_traps(pPicture, x_off, y_off, ntrap, traps);
}

#endif /* RENDER */


/**
 * radeon_glamor_close_screen() unwraps its wrapped screen functions and tears down GLAMOR's
 * screen private, before calling down to the next CloseSccreen.
 */
static Bool radeon_glamor_close_screen(CLOSE_SCREEN_ARGS_DECL)
{
	glamor_screen_t *glamor_screen = glamor_get_screen(pScreen);
#ifdef RENDER
	PictureScreenPtr ps = GetPictureScreenIfSet(pScreen);
#endif

	pScreen->CreateGC = glamor_screen->SavedCreateGC;
	pScreen->CloseScreen = glamor_screen->SavedCloseScreen;
	pScreen->GetImage = glamor_screen->SavedGetImage;
	pScreen->GetSpans = glamor_screen->SavedGetSpans;
	pScreen->CreatePixmap = glamor_screen->SavedCreatePixmap;
	pScreen->DestroyPixmap = glamor_screen->SavedDestroyPixmap;
	pScreen->CopyWindow = glamor_screen->SavedCopyWindow;
	pScreen->ChangeWindowAttributes =
	    glamor_screen->SavedChangeWindowAttributes;
	pScreen->BitmapToRegion = glamor_screen->SavedBitmapToRegion;
#ifdef RENDER
	if (ps) {
		ps->Composite = glamor_screen->SavedComposite;
		ps->Glyphs = glamor_screen->SavedGlyphs;
		ps->Trapezoids = glamor_screen->SavedTrapezoids;
		ps->AddTraps = glamor_screen->SavedAddTraps;
		ps->Triangles = glamor_screen->SavedTriangles;

		ps->UnrealizeGlyph = glamor_screen->SavedUnrealizeGlyph;
	}
#endif

	free(glamor_screen);

	return (*pScreen->CloseScreen) (CLOSE_SCREEN_ARGS);
}

/**
 * @param screen screen being initialized
 */
Bool glamor_screen_init(ScreenPtr screen)
{
	glamor_screen_t *glamor_screen;

#if HAS_DIXREGISTERPRIVATEKEY
	if (!dixRegisterPrivateKey(&glamor_screen_index, PRIVATE_SCREEN, 0))
	    return FALSE;
#endif
	glamor_screen = calloc(sizeof(glamor_screen_t), 1);

	if (!glamor_screen) {
		LogMessage(X_WARNING,
			   "GLAMOR(%d): Failed to allocate screen private\n",
			   screen->myNum);
		return FALSE;
	}

	dixSetPrivate(&screen->devPrivates, &glamor_screen_index, glamor_screen);

	/*
	 * Replace various fb screen functions
	 */
	glamor_screen->SavedCloseScreen = screen->CloseScreen;
	screen->CloseScreen = radeon_glamor_close_screen;

	glamor_screen->SavedCreateGC = screen->CreateGC;
	screen->CreateGC = radeon_glamor_create_gc;

	glamor_screen->SavedGetImage = screen->GetImage;
	screen->GetImage = glamor_get_image;

	glamor_screen->SavedGetSpans = screen->GetSpans;
	screen->GetSpans = glamor_get_spans;

	glamor_screen->SavedCreatePixmap = screen->CreatePixmap;
	glamor_screen->SavedDestroyPixmap = screen->DestroyPixmap;

	glamor_screen->SavedCopyWindow = screen->CopyWindow;
	screen->CopyWindow = glamor_copy_window;

	glamor_screen->SavedChangeWindowAttributes =
	    screen->ChangeWindowAttributes;
	screen->ChangeWindowAttributes = glamor_change_window_attributes;

	glamor_screen->SavedBitmapToRegion = screen->BitmapToRegion;
	screen->BitmapToRegion = glamor_bitmap_to_region;

#ifdef RENDER
	{
		PictureScreenPtr ps = GetPictureScreenIfSet(screen);
		if (ps) {
			glamor_screen->SavedComposite = ps->Composite;
			ps->Composite = glamor_composite;

			glamor_screen->SavedGlyphs = ps->Glyphs;
			ps->Glyphs = glamor_glyphs;

			glamor_screen->SavedTriangles = ps->Triangles;
			ps->Triangles = glamor_triangles;

			glamor_screen->SavedTrapezoids = ps->Trapezoids;
			ps->Trapezoids = glamor_trapezoids;

			glamor_screen->SavedAddTraps = ps->AddTraps;
			ps->AddTraps = glamor_add_traps;
		}
	}
#endif

	return TRUE;
}
