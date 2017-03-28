/* msm-exa-xa.c
 *
 * Copyright © 2013 Rob Clark <robclark@freedesktop.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "exa.h"

#include "msm.h"
#include "msm-accel.h"

#ifdef HAVE_XA
#  include <xa_tracker.h>
#  include <xa_context.h>
#  include <xa_composite.h>
#endif

#define xFixedtoDouble(_f) (double) ((_f)/(double) xFixed1)

#define MSM_LOCALS(pDraw) \
    ScrnInfoPtr pScrn = xf86ScreenToScrn(((DrawablePtr)(pDraw))->pScreen); \
    MSMPtr pMsm = MSMPTR(pScrn);                                    \
    struct exa_state *exa = pMsm->exa; (void)exa
    ;

struct exa_state {
	struct xa_context *ctx;
	struct xa_composite comp;
	struct xa_picture dst_pict, src_pict, mask_pict;
	union xa_source_pict dst_spict, src_spict, mask_spict;
};

/**
 * PrepareSolid() sets up the driver for doing a solid fill.
 * @param pPixmap Destination pixmap
 * @param alu raster operation
 * @param planemask write mask for the fill
 * @param fg "foreground" color for the fill
 *
 * This call should set up the driver for doing a series of solid fills
 * through the Solid() call.  The alu raster op is one of the GX*
 * graphics functions listed in X.h, and typically maps to a similar
 * single-byte "ROP" setting in all hardware.  The planemask controls
 * which bits of the destination should be affected, and will only represent
 * the bits up to the depth of pPixmap.  The fg is the pixel value of the
 * foreground color referred to in ROP descriptions.
 *
 * Note that many drivers will need to store some of the data in the driver
 * private record, for sending to the hardware with each drawing command.
 *
 * The PrepareSolid() call is required of all drivers, but it may fail for any
 * reason.  Failure results in a fallback to software rendering.
 */
static Bool
XAPrepareSolid(PixmapPtr pPixmap, int alu, Pixel planemask, Pixel fg)
{
	MSM_LOCALS(pPixmap);
	struct xa_surface *dst = msm_get_pixmap_surf(pPixmap);
	EXA_FAIL_IF(!(pMsm->examask & ACCEL_SOLID));
	EXA_FAIL_IF(planemask != FB_ALLONES);
	EXA_FAIL_IF(alu != GXcopy);
	if (!dst)
		return FALSE;
	return xa_solid_prepare(exa->ctx, dst, fg) == XA_ERR_NONE;
}

/**
 * Solid() performs a solid fill set up in the last PrepareSolid() call.
 *
 * @param pPixmap destination pixmap
 * @param x1 left coordinate
 * @param y1 top coordinate
 * @param x2 right coordinate
 * @param y2 bottom coordinate
 *
 * Performs the fill set up by the last PrepareSolid() call, covering the
 * area from (x1,y1) to (x2,y2) in pPixmap.  Note that the coordinates are
 * in the coordinate space of the destination pixmap, so the driver will
 * need to set up the hardware's offset and pitch for the destination
 * coordinates according to the pixmap's offset and pitch within
 * framebuffer.  This likely means using exaGetPixmapOffset() and
 * exaGetPixmapPitch().
 *
 * This call is required if PrepareSolid() ever succeeds.
 */
static void
XASolid(PixmapPtr pPixmap, int x1, int y1, int x2, int y2)
{
	MSM_LOCALS(pPixmap);
	xa_solid(exa->ctx, x1, y1, x2 - x1, y2 - y1);
}

/**
 * DoneSolid() finishes a set of solid fills.
 *
 * @param pPixmap destination pixmap.
 *
 * The DoneSolid() call is called at the end of a series of consecutive
 * Solid() calls following a successful PrepareSolid().  This allows drivers
 * to finish up emitting drawing commands that were buffered, or clean up
 * state from PrepareSolid().
 *
 * This call is required if PrepareSolid() ever succeeds.
 */
static void
XADoneSolid(PixmapPtr pPixmap)
{
	MSM_LOCALS(pPixmap);
	xa_solid_done(exa->ctx);
}

/**
 * PrepareCopy() sets up the driver for doing a copy within video
 * memory.
 *
 * @param pSrcPixmap source pixmap
 * @param pDstPixmap destination pixmap
 * @param dx X copy direction
 * @param dy Y copy direction
 * @param alu raster operation
 * @param planemask write mask for the fill
 *
 * This call should set up the driver for doing a series of copies from the
 * the pSrcPixmap to the pDstPixmap.  The dx flag will be positive if the
 * hardware should do the copy from the left to the right, and dy will be
 * positive if the copy should be done from the top to the bottom.  This
 * is to deal with self-overlapping copies when pSrcPixmap == pDstPixmap.
 * If your hardware can only support blits that are (left to right, top to
 * bottom) or (right to left, bottom to top), then you should set
 * #EXA_TWO_BITBLT_DIRECTIONS, and EXA will break down Copy operations to
 * ones that meet those requirements.  The alu raster op is one of the GX*
 * graphics functions listed in X.h, and typically maps to a similar
 * single-byte "ROP" setting in all hardware.  The planemask controls which
 * bits of the destination should be affected, and will only represent the
 * bits up to the depth of pPixmap.
 *
 * Note that many drivers will need to store some of the data in the driver
 * private record, for sending to the hardware with each drawing command.
 *
 * The PrepareCopy() call is required of all drivers, but it may fail for any
 * reason.  Failure results in a fallback to software rendering.
 */
static Bool
XAPrepareCopy(PixmapPtr pSrcPixmap, PixmapPtr pDstPixmap, int dx, int dy,
		int alu, Pixel planemask)
{
	MSM_LOCALS(pDstPixmap);
	struct xa_surface *src = msm_get_pixmap_surf(pSrcPixmap);
	struct xa_surface *dst = msm_get_pixmap_surf(pDstPixmap);
	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COPY));
	if (!(src && dst))
		return FALSE;
	return xa_copy_prepare(exa->ctx, dst, src) == XA_ERR_NONE;
}

/**
 * Copy() performs a copy set up in the last PrepareCopy call.
 *
 * @param pDstPixmap destination pixmap
 * @param srcX source X coordinate
 * @param srcY source Y coordinate
 * @param dstX destination X coordinate
 * @param dstY destination Y coordinate
 * @param width width of the rectangle to be copied
 * @param height height of the rectangle to be copied.
 *
 * Performs the copy set up by the last PrepareCopy() call, copying the
 * rectangle from (srcX, srcY) to (srcX + width, srcY + width) in the source
 * pixmap to the same-sized rectangle at (dstX, dstY) in the destination
 * pixmap.  Those rectangles may overlap in memory, if
 * pSrcPixmap == pDstPixmap.  Note that this call does not receive the
 * pSrcPixmap as an argument -- if it's needed in this function, it should
 * be stored in the driver private during PrepareCopy().  As with Solid(),
 * the coordinates are in the coordinate space of each pixmap, so the driver
 * will need to set up source and destination pitches and offsets from those
 * pixmaps, probably using exaGetPixmapOffset() and exaGetPixmapPitch().
 *
 * This call is required if PrepareCopy ever succeeds.
 */
static void
XACopy(PixmapPtr pDstPixmap, int srcX, int srcY, int dstX, int dstY,
		int width, int height)
{
	MSM_LOCALS(pDstPixmap);
	xa_copy(exa->ctx, dstX, dstY, srcX, srcY, width, height);
}

/**
 * DoneCopy() finishes a set of copies.
 *
 * @param pPixmap destination pixmap.
 *
 * The DoneCopy() call is called at the end of a series of consecutive
 * Copy() calls following a successful PrepareCopy().  This allows drivers
 * to finish up emitting drawing commands that were buffered, or clean up
 * state from PrepareCopy().
 *
 * This call is required if PrepareCopy() ever succeeds.
 */
static void
XADoneCopy(PixmapPtr pDstPixmap)
{
	MSM_LOCALS(pDstPixmap);
	xa_copy_done(exa->ctx);
}

/*
 * Composite:
 */

static const enum xa_surface_type stype_map[] = {
		[PICT_TYPE_OTHER] = xa_type_other,
		[PICT_TYPE_A] = xa_type_a,
		[PICT_TYPE_ARGB] = xa_type_argb,
		[PICT_TYPE_ABGR] = xa_type_abgr,
		[PICT_TYPE_BGRA] = xa_type_bgra
};


/*
 * Create an xa format from a PICT format.
 */
enum xa_formats
to_xa_format(enum _PictFormatShort format)
{
	uint32_t ptype = PICT_FORMAT_TYPE(format);

	if ((ptype >= ARRAY_SIZE(stype_map)) ||
			(stype_map[ptype] == 0) ||
			(stype_map[ptype] == xa_type_other))
		return xa_format_unknown;

	return xa_format(PICT_FORMAT_BPP(format),
			stype_map[ptype],
			PICT_FORMAT_A(format),
			PICT_FORMAT_R(format),
			PICT_FORMAT_G(format),
			PICT_FORMAT_B(format));
}


static Bool
matrix_from_pict_transform(PictTransform *trans, float *matrix)
{
	if (!trans)
		return FALSE;

	matrix[0] = pixman_fixed_to_double(trans->matrix[0][0]);
	matrix[3] = pixman_fixed_to_double(trans->matrix[0][1]);
	matrix[6] = pixman_fixed_to_double(trans->matrix[0][2]);

	matrix[1] = pixman_fixed_to_double(trans->matrix[1][0]);
	matrix[4] = pixman_fixed_to_double(trans->matrix[1][1]);
	matrix[7] = pixman_fixed_to_double(trans->matrix[1][2]);

	matrix[2] = pixman_fixed_to_double(trans->matrix[2][0]);
	matrix[5] = pixman_fixed_to_double(trans->matrix[2][1]);
	matrix[8] = pixman_fixed_to_double(trans->matrix[2][2]);

	return TRUE;
}

static enum xa_composite_wrap
xa_setup_wrap(Bool pict_has_repeat, int pict_repeat)
{
	enum xa_composite_wrap wrap = xa_wrap_clamp_to_border;

	if (!pict_has_repeat)
		return wrap;

	switch(pict_repeat) {
	case RepeatNormal:
		wrap = xa_wrap_repeat;
		break;
	case RepeatReflect:
		wrap = xa_wrap_mirror_repeat;
		break;
	case RepeatPad:
		wrap = xa_wrap_clamp_to_edge;
		break;
	default:
		break;
	}
	return wrap;
}

static enum xa_composite_filter
xa_setup_filter(int xrender_filter)
{
	switch (xrender_filter) {
	case PictFilterConvolution:
	case PictFilterNearest:
	case PictFilterFast:
	default:
		return xa_filter_nearest;
	case PictFilterBest:
	case PictFilterGood:
	case PictFilterBilinear:
		return xa_filter_linear;
	}
}

static Bool
xa_setup_spict(union xa_source_pict *spict, SourcePictPtr pSourcePict)
{
	switch (pSourcePict->type) {
	case SourcePictTypeSolidFill:
		spict->type = xa_src_pict_solid_fill;
		spict->solid_fill.color = pSourcePict->solidFill.color;
		return TRUE;
	default:
		/* TODO */
		TRACE_EXA("unhandled spict type: %u", pSourcePict->type);
		return FALSE;
	}
}

static Bool
xa_setup_pict(struct xa_picture *pict, union xa_source_pict *spict,
		PicturePtr pPict)
{
	memset(pict, 0, sizeof(*pict));

	pict->pict_format = to_xa_format(pPict->format);
	EXA_FAIL_IF(pict->pict_format == xa_format_unknown);

	pict->alpha_map = NULL;
	pict->component_alpha = pPict->componentAlpha;
	pict->has_transform = matrix_from_pict_transform(
			pPict->transform, pict->transform);
	pict->wrap = xa_setup_wrap(pPict->repeat, pPict->repeatType);
	pict->filter = xa_setup_filter(pPict->filter);

	if (pPict->pSourcePict) {
		if (!xa_setup_spict(spict, pPict->pSourcePict))
			return FALSE;
		pict->src_pict = spict;
	} else {
		pict->src_pict = NULL;
	}

	return TRUE;
}

static const enum xa_composite_op op_map[] = {
		[PictOpClear] = xa_op_clear,
		[PictOpSrc] = xa_op_src,
		[PictOpDst] = xa_op_dst,
		[PictOpOver] = xa_op_over,
		[PictOpOverReverse] = xa_op_over_reverse,
		[PictOpIn] = xa_op_in,
		[PictOpInReverse] = xa_op_in_reverse,
		[PictOpOut] = xa_op_out,
		[PictOpOutReverse] = xa_op_out_reverse,
		[PictOpAtop] = xa_op_atop,
		[PictOpAtopReverse] = xa_op_atop_reverse,
		[PictOpXor] = xa_op_xor,
		[PictOpAdd] = xa_op_add
};

static Bool
xa_setup_composite(struct exa_state *exa, int op, PicturePtr pSrcPicture,
		PicturePtr pMaskPicture, PicturePtr pDstPicture)
{
	struct xa_composite *comp = &exa->comp;

	EXA_FAIL_IF(op >= ARRAY_SIZE(op_map));

	comp->op = op_map[op];
	EXA_FAIL_IF((comp->op == xa_op_clear) && (op != PictOpClear));

	EXA_FAIL_IF(!xa_setup_pict(&exa->dst_pict, &exa->dst_spict, pDstPicture));
	EXA_FAIL_IF(!xa_setup_pict(&exa->src_pict, &exa->src_spict, pSrcPicture));
	EXA_FAIL_IF(pMaskPicture &&
		!xa_setup_pict(&exa->mask_pict, &exa->mask_spict, pMaskPicture));

	comp->dst = &exa->dst_pict;
	comp->src = &exa->src_pict;
	comp->mask = pMaskPicture ? &exa->mask_pict : NULL;

	return TRUE;
}

static Bool
xa_update_composite(struct xa_composite *comp, PixmapPtr pSrc,
		PixmapPtr pMask, PixmapPtr pDst)
{
	comp->dst->srf = msm_get_pixmap_surf(pDst);
	EXA_FAIL_IF(!comp->dst->srf);

	if (pSrc) {
		comp->src->srf = msm_get_pixmap_surf(pSrc);
		EXA_FAIL_IF(!comp->src->srf);
	} else {
		comp->src->srf = NULL;
	}

	if (comp->mask) {
		if (pMask) {
			comp->mask->srf = msm_get_pixmap_surf(pMask);
			EXA_FAIL_IF(!comp->mask->srf);
		} else {
			comp->mask->srf = NULL;
		}
	}

	return TRUE;
}

/**
 * CheckComposite() checks to see if a composite operation could be
 * accelerated.
 *
 * @param op Render operation
 * @param pSrcPicture source Picture
 * @param pMaskPicture mask picture
 * @param pDstPicture destination Picture
 *
 * The CheckComposite() call checks if the driver could handle acceleration
 * of op with the given source, mask, and destination pictures.  This allows
 * drivers to check source and destination formats, supported operations,
 * transformations, and component alpha state, and send operations it can't
 * support to software rendering early on.  This avoids costly pixmap
 * migration to the wrong places when the driver can't accelerate
 * operations.  Note that because migration hasn't happened, the driver
 * can't know during CheckComposite() what the offsets and pitches of the
 * pixmaps are going to be.
 *
 * See PrepareComposite() for more details on likely issues that drivers
 * will have in accelerating Composite operations.
 *
 * The CheckComposite() call is recommended if PrepareComposite() is
 * implemented, but is not required.
 */
static Bool
XACheckComposite(int op, PicturePtr pSrcPicture, PicturePtr pMaskPicture,
		PicturePtr pDstPicture)
{
	MSM_LOCALS(pDstPicture->pDrawable);
	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COMPOSITE));
	if (!xa_setup_composite(exa, op, pSrcPicture, pMaskPicture, pDstPicture))
		return FALSE;
	EXA_FAIL_IF(xa_composite_check_accelerated(&exa->comp) != XA_ERR_NONE);
	return TRUE;
}

/**
 * PrepareComposite() sets up the driver for doing a Composite operation
 * described in the Render extension protocol spec.
 *
 * @param op Render operation
 * @param pSrcPicture source Picture
 * @param pMaskPicture mask picture
 * @param pDstPicture destination Picture
 * @param pSrc source pixmap
 * @param pMask mask pixmap
 * @param pDst destination pixmap
 *
 * This call should set up the driver for doing a series of Composite
 * operations, as described in the Render protocol spec, with the given
 * pSrcPicture, pMaskPicture, and pDstPicture.  The pSrc, pMask, and
 * pDst are the pixmaps containing the pixel data, and should be used for
 * setting the offset and pitch used for the coordinate spaces for each of
 * the Pictures.
 *
 * Notes on interpreting Picture structures:
 * - The Picture structures will always have a valid pDrawable.
 * - The Picture structures will never have alphaMap set.
 * - The mask Picture (and therefore pMask) may be NULL, in which case the
 *   operation is simply src OP dst instead of src IN mask OP dst, and
 *   mask coordinates should be ignored.
 * - pMarkPicture may have componentAlpha set, which greatly changes
 *   the behavior of the Composite operation.  componentAlpha has no effect
 *   when set on pSrcPicture or pDstPicture.
 *   Note: componentAlpha means to treat each R/G/B channel as an independent
 *   alpha value for the corresponding channel in the src.
 * - The source and mask Pictures may have a transformation set
 *   (Picture->transform != NULL), which means that the source coordinates
 *   should be transformed by that transformation, resulting in scaling,
 *   rotation, etc.  The PictureTransformPoint() call can transform
 *   coordinates for you.  Transforms have no effect on Pictures when used
 *   as a destination.
 * - The source and mask pictures may have a filter set.  PictFilterNearest
 *   and PictFilterBilinear are defined in the Render protocol, but others
 *   may be encountered, and must be handled correctly (usually by
 *   PrepareComposite failing, and falling back to software).  Filters have
 *   no effect on Pictures when used as a destination.
 * - The source and mask Pictures may have repeating set, which must be
 *   respected.  Many chipsets will be unable to support repeating on
 *   pixmaps that have a width or height that is not a power of two.
 *
 * If your hardware can't support source pictures (textures) with
 * non-power-of-two pitches, you should set #EXA_OFFSCREEN_ALIGN_POT.
 *
 * Note that many drivers will need to store some of the data in the driver
 * private record, for sending to the hardware with each drawing command.
 *
 * The PrepareComposite() call is not required.  However, it is highly
 * recommended for performance of antialiased font rendering and performance
 * of cairo applications.  Failure results in a fallback to software
 * rendering.
 */
static Bool
XAPrepareComposite(int op, PicturePtr pSrcPicture, PicturePtr pMaskPicture,
		PicturePtr pDstPicture, PixmapPtr pSrc, PixmapPtr pMask, PixmapPtr pDst)
{
	MSM_LOCALS(pDst);
	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COMPOSITE));
	if (!xa_update_composite(&exa->comp, pSrc, pMask, pDst))
		return FALSE;
	EXA_FAIL_IF(xa_composite_prepare(exa->ctx, &exa->comp) != XA_ERR_NONE);
	return TRUE;
}

/**
 * Composite() performs a Composite operation set up in the last
 * PrepareComposite() call.
 *
 * @param pDstPixmap destination pixmap
 * @param srcX source X coordinate
 * @param srcY source Y coordinate
 * @param maskX source X coordinate
 * @param maskY source Y coordinate
 * @param dstX destination X coordinate
 * @param dstY destination Y coordinate
 * @param width destination rectangle width
 * @param height destination rectangle height
 *
 * Performs the Composite operation set up by the last PrepareComposite()
 * call, to the rectangle from (dstX, dstY) to (dstX + width, dstY + height)
 * in the destination Pixmap.  Note that if a transformation was set on
 * the source or mask Pictures, the source rectangles may not be the same
 * size as the destination rectangles and filtering.  Getting the coordinate
 * transformation right at the subpixel level can be tricky, and rendercheck
 * can test this for you.
 *
 * This call is required if PrepareComposite() ever succeeds.
 */
static void
XAComposite(PixmapPtr pDstPixmap, int srcX, int srcY, int maskX, int maskY,
		int dstX, int dstY, int width, int height)
{
	MSM_LOCALS(pDstPixmap);
	xa_composite_rect(exa->ctx, srcX, srcY, maskX, maskY,
			dstX, dstY, width, height);
}

/**
 * DoneComposite() finishes a set of Composite operations.
 *
 * @param pPixmap destination pixmap.
 *
 * The DoneComposite() call is called at the end of a series of consecutive
 * Composite() calls following a successful PrepareComposite().  This allows
 * drivers to finish up emitting drawing commands that were buffered, or
 * clean up state from PrepareComposite().
 *
 * This call is required if PrepareComposite() ever succeeds.
 */
static void
XADoneComposite(PixmapPtr pDstPixmap)
{
	MSM_LOCALS(pDstPixmap);
	xa_composite_done(exa->ctx);
}

/**
 * MarkSync() requests that the driver mark a synchronization point,
 * returning an driver-defined integer marker which could be requested for
 * synchronization to later in WaitMarker().  This might be used in the
 * future to avoid waiting for full hardware stalls before accessing pixmap
 * data with the CPU, but is not important in the current incarnation of
 * EXA.
 *
 * Note that drivers should call exaMarkSync() when they have done some
 * acceleration, rather than their own MarkSync() handler, as otherwise EXA
 * will be unaware of the driver's acceleration and not sync to it during
 * fallbacks.
 *
 * MarkSync() is optional.
 */
static int
XAMarkSync(ScreenPtr pScreen)
{
	return 1;
}


/**
 * WaitMarker() waits for all rendering before the given marker to have
 * completed.  If the driver does not implement MarkSync(), marker is
 * meaningless, and all rendering by the hardware should be completed before
 * WaitMarker() returns.
 *
 * Note that drivers should call exaWaitSync() to wait for all acceleration
 * to finish, as otherwise EXA will be unaware of the driver having
 * synchronized, resulting in excessive WaitMarker() calls.
 *
 * WaitMarker() is required of all drivers.
 */
static void
XAWaitMarker(ScreenPtr pScreen, int marker)
{
	/* nothing to do, handled by xa_surface_map() in PrepareAccess */
}

static Bool
XAPixmapIsOffscreen(PixmapPtr pPixmap)
{
	ScreenPtr pScreen = pPixmap->drawable.pScreen;
	struct xa_surface *surf = msm_get_pixmap_surf(pPixmap);

	if ((pScreen->GetScreenPixmap(pScreen) == pPixmap)) {
		return TRUE;
	}

	if (!surf) {
		/* because we are pretending to handle unaccel (1bpp, etc)
		 * pixmaps too..
		 * this is pretty lame, revisit using EXA_MIXED_PIXMAPS
		 */
		struct msm_pixmap_priv *priv =
			exaGetPixmapDriverPrivate(pPixmap);

		if (priv && priv->ptr) {
			return TRUE;
		}
	}

	if (surf)
		return TRUE;

	return FALSE;
}

static void XAFinishAccess(PixmapPtr pPixmap, int index);

static Bool
XAPrepareAccess(PixmapPtr pPixmap, int index)
{
	static const unsigned int usage[EXA_NUM_PREPARE_INDICES] = {
			[EXA_PREPARE_DEST] = XA_MAP_READ | XA_MAP_WRITE,
			[EXA_PREPARE_SRC]  = XA_MAP_READ,
			[EXA_PREPARE_MASK] = XA_MAP_READ,
			[EXA_PREPARE_AUX_DEST] = XA_MAP_READ | XA_MAP_WRITE,
			[EXA_PREPARE_AUX_SRC]  = XA_MAP_READ,
			[EXA_PREPARE_AUX_MASK] = XA_MAP_READ,
	};
	MSM_LOCALS(pPixmap);

	if (pPixmap->devPrivate.ptr)
		XAFinishAccess(pPixmap, 0);

	if (pPixmap->devPrivate.ptr == NULL) {
		struct xa_surface *surf = msm_get_pixmap_surf(pPixmap);
		void *ptr;
		if (surf) {
			xa_context_flush(exa->ctx);
			ptr = xa_surface_map(exa->ctx, surf, usage[index]);
		} else {
			struct msm_pixmap_priv *priv =
				exaGetPixmapDriverPrivate(pPixmap);
			ptr = priv ? priv->ptr : NULL;
		}
		pPixmap->devPrivate.ptr = ptr;
	}

	if (!pPixmap->devPrivate.ptr) {
		ERROR_MSG("PrepareAccess failed!!");
		return FALSE;
	}

	return TRUE;
}

static void
XAFinishAccess(PixmapPtr pPixmap, int index)
{
	MSM_LOCALS(pPixmap);
	struct xa_surface *surf = msm_get_pixmap_surf(pPixmap);
	if (surf) {
		pPixmap->devPrivate.ptr = NULL;
		xa_surface_unmap(surf);
	}
}

#define EXA_ALIGN(offset, align) (((offset) + (align) - 1) - \
	(((offset) + (align) - 1) % (align)))

static void *
XACreatePixmap2(ScreenPtr pScreen, int width, int height,
		int depth, int usage_hint, int bpp,
		int *new_fb_pitch)
{
	struct msm_pixmap_priv *priv;
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	unsigned int flags = XA_FLAG_RENDER_TARGET;

	priv = calloc(1, sizeof(struct msm_pixmap_priv));

	if (priv == NULL)
		return NULL;

	if (usage_hint & CREATE_PIXMAP_USAGE_DRI2)
		flags |= XA_FLAG_SHARED;

	if ((width * height) > 0) {
		enum xa_surface_type type =
				(bpp > 8) ? xa_type_argb : xa_type_a;
		priv->surf = xa_surface_create(pMsm->xa,
				width, height, depth,
				type, xa_format_unknown, flags);
	}

	if (priv->surf) {
		uint32_t handle, stride;
		xa_surface_handle(priv->surf, xa_handle_type_kms,
				&handle, &stride);
		*new_fb_pitch = stride;
		return priv;
	}

	*new_fb_pitch = EXA_ALIGN(width * bpp,
			pMsm->pExa->pixmapPitchAlign * 8) / 8;

	/* 1bpp pixmaps aren't handled by XA.. probably everything
	 * else should be:
	 */
	if (depth != 1) {
		ERROR_MSG("cannot accel pixmap: %dx%d@%d (usage: %x)",
				width, height, depth, usage_hint);
	}
	priv->ptr = calloc(1, *new_fb_pitch * height);

	if (priv->ptr)
		return priv;

	free(priv);
	return NULL;
}

static void
XADestroyPixmap(ScreenPtr pScreen, void *dpriv)
{
	struct msm_pixmap_priv *priv = dpriv;

	if (!priv)
		return;

	if (priv->surf)
		xa_surface_unref(priv->surf);

	free(priv);
}

/**
 * Flush pending draw cmds
 */
void
MSMFlushXA(MSMPtr pMsm)
{
	xa_context_flush(pMsm->exa->ctx);
}

Bool
MSMSetupExaXA(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	ExaDriverPtr pExa;

	/* Set up EXA */
	xf86LoadSubModule(pScrn, "exa");

	if (pMsm->pExa == NULL) {
		pMsm->pExa = exaDriverAlloc();
		pMsm->exa = calloc(1, sizeof(*pMsm->exa));
		pMsm->exa->ctx = xa_context_default(pMsm->xa);
	}

	if (pMsm->pExa == NULL)
		return FALSE;

	pExa = pMsm->pExa;

	/* This is the current major/minor that we support */

	pExa->exa_major = 2;
	pExa->exa_minor = 2;

	/* Max blit extents that hw supports */
	pExa->maxX = 4096;
	pExa->maxY = 4096;

	pExa->flags = EXA_OFFSCREEN_PIXMAPS |
			EXA_HANDLES_PIXMAPS |
			EXA_SUPPORTS_PREPARE_AUX;

	/* Align pixmap offsets along page boundaries */
	pExa->pixmapOffsetAlign = 4096;

	/* Align pixmap pitches to the maximum needed aligment for the
      GPU - this ensures that we have enough room, and we adjust the
      pitches down to the depth later */

	pExa->pixmapPitchAlign = 128;

	/* The maximum acceleratable pitch is 2048 pixels */
	pExa->maxPitchPixels = 2048;

	pExa->PrepareSolid       = XAPrepareSolid;
	pExa->Solid              = XASolid;
	pExa->DoneSolid          = XADoneSolid;
	pExa->PrepareCopy        = XAPrepareCopy;
	pExa->Copy               = XACopy;
	pExa->DoneCopy           = XADoneCopy;
	pExa->CheckComposite     = XACheckComposite;
	pExa->PrepareComposite   = XAPrepareComposite;
	pExa->Composite          = XAComposite;
	pExa->DoneComposite      = XADoneComposite;
	pExa->MarkSync           = XAMarkSync;
	pExa->WaitMarker         = XAWaitMarker;
	pExa->PixmapIsOffscreen  = XAPixmapIsOffscreen;
	pExa->CreatePixmap2      = XACreatePixmap2;
	pExa->DestroyPixmap      = XADestroyPixmap;
	pExa->PrepareAccess      = XAPrepareAccess;
	pExa->FinishAccess       = XAFinishAccess;

	return exaDriverInit(pScreen, pMsm->pExa);
}
