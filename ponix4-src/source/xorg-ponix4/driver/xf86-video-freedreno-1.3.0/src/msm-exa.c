/* msm-exa.c
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
 * Copyright © 2012 Rob Clark <robclark@freedesktop.org>
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

#include "freedreno_z1xx.h"

#define xFixedtoDouble(_f) (double) ((_f)/(double) xFixed1)

#define MSM_LOCALS(pDraw) \
    ScrnInfoPtr pScrn = xf86ScreenToScrn(((DrawablePtr)(pDraw))->pScreen); \
    MSMPtr pMsm = MSMPTR(pScrn);                                    \
    struct fd_ringbuffer *ring = NULL; (void)ring;                  \
    struct exa_state *exa = pMsm->exa; (void)exa


struct exa_state {
	/* solid state: */
	uint32_t fill;

	/* copy/composite state: */
	const uint32_t *op_dwords;
	PixmapPtr src, mask;
	PicturePtr dstpic, srcpic, maskpic;

	uint32_t input;
};

/* input fields seem to be enabled/disabled in a certain order: */
static uint32_t iena(struct exa_state *exa, uint32_t enable)
{
	exa->input |= enable;
	return exa->input;
}

static uint32_t idis(struct exa_state *exa, uint32_t enable)
{
	exa->input &= ~enable;
	return exa->input;
}

/* NOTE ARGB and A8 seem to be treated the same when it comes to the
 * composite-op dwords:
 */
static const uint32_t composite_op_dwords[4][PictOpAdd+1][4] = {
	{ /* xRGB->xRGB */         /*           G2D_BLEND_A0            G2D_BLEND_C0 */
		[PictOpSrc]          = { 0x7c000114, 0x10002010, 0x00000000, 0x18012210 },
		[PictOpIn]           = { 0x7c000114, 0xb0100004, 0x00000000, 0x18110a04 },
		[PictOpOut]          = { 0x7c000114, 0xb0102004, 0x00000000, 0x18112a04 },
		[PictOpOver]         = { 0x7c000114, 0xd080a004, 0x7c000118, 0x8081aa04 },
		[PictOpOutReverse]   = { 0x7c000114, 0x80808040, 0x7c000118, 0x80808840 },
		[PictOpAdd]          = { 0x7c000114, 0x5080a004, 0x7c000118, 0x20818204 },
		[PictOpOverReverse]  = { 0x7c000114, 0x7090a004, 0x7c000118, 0x2091a204 },
		[PictOpInReverse]    = { 0x7c000114, 0x80800040, 0x7c000118, 0x80800840 },
		[PictOpAtop]         = { 0x7c000114, 0xf0908004, 0x7c000118, 0xa0918a04 },
		[PictOpAtopReverse]  = { 0x7c000114, 0xf0902004, 0x7c000118, 0xa0912a04 },
		[PictOpXor]          = { 0x7c000114, 0xf090a004, 0x7c000118, 0xa091aa04 },
	},
	{ /* xRGB->ARGB, xRGB->A8 */
		[PictOpSrc]          = { 0x7c000114, 0x10002010, 0x00000000, 0x18012210 },
		[PictOpIn]           = { 0x7c000114, 0x90100004, 0x00000000, 0x18110a04 },
		[PictOpOut]          = { 0x7c000114, 0x90102004, 0x00000000, 0x18112a04 },
		[PictOpOver]         = { 0x7c000114, 0x9080a004, 0x7c000118, 0x8081aa04 },
		[PictOpOutReverse]   = { 0x7c000114, 0x80808040, 0x7c000118, 0x80808840 },
		[PictOpAdd]          = { 0x7c000114, 0x1080a004, 0x7c000118, 0x20818204 },
		[PictOpOverReverse]  = { 0x7c000114, 0x1090a004, 0x00000000, 0x1891a204 },
		[PictOpInReverse]    = { 0x7c000114, 0x80800040, 0x7c000118, 0x80800840 },
		[PictOpAtop]         = { 0x7c000114, 0x90908004, 0x7c000118, 0x80918a04 },
		[PictOpAtopReverse]  = { 0x7c000114, 0x90902004, 0x7c000118, 0x80912a04 },
		[PictOpXor]          = { 0x7c000114, 0x9090a004, 0x7c000118, 0x8091aa04 },
	},
	{ /* ARGB->xRGB, A8->xRGB */
		[PictOpSrc]          = { 0x00000000, 0x14012010, 0x00000000, 0x18012210 },
		[PictOpIn]           = { 0x7c000114, 0x20110004, 0x00000000, 0x18110a04 },
		[PictOpOut]          = { 0x7c000114, 0x20112004, 0x00000000, 0x18112a04 },
		[PictOpOver]         = { 0x7c000114, 0x4281a004, 0x7c000118, 0x0281aa04 },
		[PictOpOutReverse]   = { 0x7c000114, 0x02808040, 0x7c000118, 0x02808840 },
		[PictOpAdd]          = { 0x7c000114, 0x4081a004, 0x00000000, 0x18898204 },
		[PictOpOverReverse]  = { 0x7c000114, 0x6091a004, 0x7c000118, 0x2091a204 },
		[PictOpInReverse]    = { 0x7c000114, 0x02800040, 0x7c000118, 0x02800840 },
		[PictOpAtop]         = { 0x7c000114, 0x62918004, 0x7c000118, 0x22918a04 },
		[PictOpAtopReverse]  = { 0x7c000114, 0x62912004, 0x7c000118, 0x22912a04 },
		[PictOpXor]          = { 0x7c000114, 0x6291a004, 0x7c000118, 0x2291aa04 },
	},
	{ /* ARGB->ARGB, A8->A8 */
		[PictOpSrc]          = { 0x00000000, 0x14012010, 0x00000000, 0x18012210 },
		[PictOpIn]           = { 0x00000000, 0x14110004, 0x00000000, 0x18110a04 },
		[PictOpOut]          = { 0x00000000, 0x14112004, 0x00000000, 0x18112a04 },
		[PictOpOver]         = { 0x7c000114, 0x0281a004, 0x7c000118, 0x0281aa04 },
		[PictOpOutReverse]   = { 0x7c000114, 0x02808040, 0x7c000118, 0x02808840 },
		[PictOpAdd]          = { 0x00000000, 0x1481a004, 0x00000000, 0x18898204 },
		[PictOpOverReverse]  = { 0x00000000, 0x1491a004, 0x00000000, 0x1891a204 },
		[PictOpInReverse]    = { 0x7c000114, 0x02800040, 0x7c000118, 0x02800840 },
		[PictOpAtop]         = { 0x7c000114, 0x02918004, 0x7c000118, 0x02918a04 },
		[PictOpAtopReverse]  = { 0x7c000114, 0x02912004, 0x7c000118, 0x02912a04 },
		[PictOpXor]          = { 0x7c000114, 0x0291a004, 0x7c000118, 0x0291aa04 },
	},
};

static inline enum g2d_format
pixfmt(PixmapPtr pix)
{
	return (pix->drawable.depth == 8) ? G2D_A8 : G2D_8888;
}

/* 15 dwords */
static inline void
out_dstpix(struct fd_ringbuffer *ring, PixmapPtr pix)
{
	struct fd_bo *bo = msm_get_pixmap_bo(pix);
	uint32_t w, h, p;

	w = pix->drawable.width;
	h = pix->drawable.height;

	/* pitch specified in units of 32 bytes, it appears.. not quite sure
	 * max size yet, but I think 11 or 12 bits..
	 */
	p = (exaGetPixmapPitch(pix) / 32) & 0xfff;

	TRACE_EXA("DST: %p, %dx%d,%d,%d", bo, w, h, p, pix->drawable.depth);

	OUT_RING (ring, REG(G2D_ALPHABLEND) | 0x0);
	OUT_RING (ring, REG(G2D_BLENDERCFG) | 0x0);
	OUT_RING (ring, REG(G2D_GRADIENT) | 0x030000);
	OUT_RING (ring, GRADW_TEXSIZE_WIDTH(w) | /* GRADW_TEXSIZE */
			GRADW_TEXSIZE_HEIGHT(h));
	OUT_RING (ring, REG(G2D_CFG0) |
			G2D_CFGn_PITCH(p) |
			G2D_CFGn_FORMAT(pixfmt(pix)));
	OUT_RING (ring, REGM(G2D_BASE0, 1));
	OUT_RELOC(ring, bo, TRUE);
	OUT_RING (ring, REGM(GRADW_TEXBASE, 1));
	OUT_RELOC(ring, bo, TRUE);
	OUT_RING (ring, REGM(GRADW_TEXCFG, 1));
	OUT_RING (ring, 0x40000000 |
			GRADW_TEXCFG_PITCH(p) |
			GRADW_TEXCFG_FORMAT(pixfmt(pix)));
	OUT_RING (ring, REG(GRADW_TEXCFG2) | 0x0);
	OUT_RING (ring, REG(G2D_ALPHABLEND) | 0x0);
	OUT_RING (ring, REG(G2D_SCISSORX) | (w & 0xfff) << 12);
	OUT_RING (ring, REG(G2D_SCISSORY) | (h & 0xfff) << 12);
}

/* 4 dwords */
static inline void
out_srcpix(struct fd_ringbuffer *ring, PixmapPtr pix)
{
	struct fd_bo *bo = msm_get_pixmap_bo(pix);
	uint32_t w, h, p;
	uint32_t texcfg;

	w = pix->drawable.width;
	h = pix->drawable.height;

	/* pitch specified in units of 32 bytes, it appears.. not quite sure
	 * max size yet, but I think 11 or 12 bits..
	 */
	p = (exaGetPixmapPitch(pix) / 32) & 0xfff;

	TRACE_EXA("SRC: %p, %dx%d,%d,%d", bo, w, h, p, pix->drawable.depth);

	texcfg = GRADW_TEXCFG_PITCH(p) |
			GRADW_TEXCFG_FORMAT(pixfmt(pix));

	OUT_RING (ring, REGM(GRADW_TEXCFG, 3));
	OUT_RING (ring, texcfg);                /* GRADW_TEXCFG */
	OUT_RING (ring, GRADW_TEXSIZE_WIDTH(w) |/* GRADW_TEXSIZE */
			GRADW_TEXSIZE_HEIGHT(h));
	OUT_RELOC(ring, bo, FALSE);             /* GRADW_TEXBASE */
}

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
MSMPrepareSolid(PixmapPtr pPixmap, int alu, Pixel planemask, Pixel fg)
{
	MSM_LOCALS(pPixmap);

	EXA_FAIL_IF(!(pMsm->examask & ACCEL_SOLID));
	EXA_FAIL_IF(planemask != FB_ALLONES);
	EXA_FAIL_IF(alu != GXcopy);

	// TODO other color formats
	EXA_FAIL_IF(pPixmap->drawable.bitsPerPixel != 32);

	exa->fill = fg;

	/* Note: 16bpp 565 we want something like this.. I think..

		color  = ((fg << 3) & 0xf8) | ((fg >> 2) & 0x07) |
			((fg << 5) & 0xfc00)    | ((fg >> 1) & 0x300) |
			((fg << 8) & 0xf80000)  | ((fg << 3) & 0x70000) |
			0xff000000; // implicitly DISABLE_ALPHA

	 */

	return TRUE;
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
MSMSolid(PixmapPtr pPixmap, int x1, int y1, int x2, int y2)
{
	MSM_LOCALS(pPixmap);

	TRACE_EXA("SOLID: x1=%d\ty1=%d\tx2=%d\ty2=%d\tfill=%08x",
			x1, y1, x2, y2, exa->fill);

	BEGIN_RING(pMsm, 25);
	ring = pMsm->ring.ring;
	out_dstpix(ring, pPixmap);
	OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_SCOORD1));
	OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_SCOORD2));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0x0));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, G2D_INPUT_COLOR));
	OUT_RING  (ring, REG(G2D_CONFIG) | 0x0);
	OUT_RING  (ring, REGM(G2D_XY, 2));
	OUT_RING  (ring, G2D_XY_X(x1) | G2D_XY_Y(y1));    /* G2D_XY */
	OUT_RING  (ring, G2D_WIDTHHEIGHT_WIDTH(x2-x1) |   /* G2D_WIDTHHEIGHT */
			G2D_WIDTHHEIGHT_HEIGHT(y2-y1));
	OUT_RING  (ring, REGM(G2D_COLOR, 1));
	OUT_RING  (ring, exa->fill);
	END_RING  (pMsm);
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
MSMDoneSolid(PixmapPtr pPixmap)
{

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
MSMPrepareCopy(PixmapPtr pSrcPixmap, PixmapPtr pDstPixmap, int dx, int dy,
		int alu, Pixel planemask)
{
	MSM_LOCALS(pDstPixmap);

	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COPY));
	EXA_FAIL_IF(planemask != FB_ALLONES);
	EXA_FAIL_IF(alu != GXcopy);

	// TODO other color formats
	EXA_FAIL_IF(pSrcPixmap->drawable.bitsPerPixel != 32);
	EXA_FAIL_IF(pDstPixmap->drawable.bitsPerPixel != 32);

	exa->src = pSrcPixmap;

	return TRUE;
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
MSMCopy(PixmapPtr pDstPixmap, int srcX, int srcY, int dstX, int dstY,
		int width, int height)
{
	MSM_LOCALS(pDstPixmap);
	PixmapPtr pSrcPixmap = exa->src;

	TRACE_EXA("COPY: srcX=%d\tsrcY=%d\tdstX=%d\tdstY=%d\twidth=%d\theight=%d",
			srcX, srcY, dstX, dstY, width, height);

	BEGIN_RING(pMsm, 46);
	ring = pMsm->ring.ring;
	out_dstpix(ring, pDstPixmap);
	OUT_RING  (ring, REGM(G2D_FOREGROUND, 2));
	OUT_RING  (ring, 0xff000000);      /* G2D_FOREGROUND */
	OUT_RING  (ring, 0xff000000);      /* G2D_BACKGROUND */
	OUT_RING  (ring, REG(G2D_BLENDERCFG) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	out_srcpix(ring, pSrcPixmap);
	OUT_RING  (ring, REG(GRADW_TEXCFG2) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, G2D_INPUT_SCOORD1));
	OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_SCOORD2));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_COLOR));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, G2D_INPUT_COPYCOORD));
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_CONFIG) | G2D_CONFIG_SRC1); /* we don't read from dst */
	OUT_RING  (ring, REGM(G2D_XY, 3));
	OUT_RING  (ring, G2D_XY_X(dstX) | G2D_XY_Y(dstY));/* G2D_XY */
	OUT_RING  (ring, G2D_WIDTHHEIGHT_WIDTH(width) |   /* G2D_WIDTHHEIGHT */
			G2D_WIDTHHEIGHT_HEIGHT(height));
	OUT_RING  (ring, G2D_SXYn_X(srcX) |               /* G2D_SXY */
			G2D_SXYn_Y(srcY));
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	END_RING  (pMsm);
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
MSMDoneCopy(PixmapPtr pDstPixmap)
{

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
MSMCheckComposite(int op, PicturePtr pSrcPicture, PicturePtr pMaskPicture,
		PicturePtr pDstPicture)
{
	MSM_LOCALS(pDstPicture->pDrawable);
	int idx = 0;

	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COMPOSITE));

	// TODO proper handling for RGB vs BGR!

	EXA_FAIL_IF((pDstPicture->format != PICT_a8r8g8b8) &&
			(pDstPicture->format != PICT_a8b8g8r8) &&
			(pDstPicture->format != PICT_x8r8g8b8) &&
			(pDstPicture->format != PICT_x8b8g8r8) &&
			(pDstPicture->format != PICT_a8));
	EXA_FAIL_IF((pSrcPicture->format != PICT_a8r8g8b8) &&
			(pSrcPicture->format != PICT_a8b8g8r8) &&
			(pSrcPicture->format != PICT_x8r8g8b8) &&
			(pSrcPicture->format != PICT_x8b8g8r8) &&
			(pSrcPicture->format != PICT_a8));

	if (pMaskPicture) {
		EXA_FAIL_IF((pMaskPicture->format != PICT_a8r8g8b8) &&
				(pMaskPicture->format != PICT_a8b8g8r8) &&
				(pMaskPicture->format != PICT_x8r8g8b8) &&
				(pMaskPicture->format != PICT_x8b8g8r8) &&
				(pMaskPicture->format != PICT_a8));
		EXA_FAIL_IF(pMaskPicture->transform);
		/* this doesn't appear to be supported by libC2D2.. although
		 * perhaps it is supported by the hw?  It might be worth
		 * experimenting with this at some point
		 */
		EXA_FAIL_IF(pMaskPicture->componentAlpha);
	}

	// TODO src add transforms later:
	EXA_FAIL_IF(pSrcPicture->transform);

	if (PICT_FORMAT_A(pSrcPicture->format))
		idx += 2;
	if (PICT_FORMAT_A(pDstPicture->format))
		idx += 1;

	/* check for unsupported op: */
	EXA_FAIL_IF((op >= ARRAY_SIZE(composite_op_dwords[idx])) ||
			!composite_op_dwords[idx][op][1]);

	exa->op_dwords = composite_op_dwords[idx][op];
	exa->dstpic    = pDstPicture;
	exa->srcpic    = pSrcPicture;
	exa->maskpic   = pMaskPicture;

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
MSMPrepareComposite(int op, PicturePtr pSrcPicture, PicturePtr pMaskPicture,
		PicturePtr pDstPicture, PixmapPtr pSrc, PixmapPtr pMask, PixmapPtr pDst)
{
	MSM_LOCALS(pDst);

	EXA_FAIL_IF(!(pMsm->examask & ACCEL_COMPOSITE));

	// TODO, maybe we can support this.. pSrcPicture could be telling
	// us to do solid, which we could probably support
	EXA_FAIL_IF(!pSrc);

	// XXX for now only supporting it on src..
	EXA_FAIL_IF(pMaskPicture && pMaskPicture->repeat);

	exa->src  = pSrc;
	exa->mask = pMask;

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
MSMComposite(PixmapPtr pDstPixmap, int srcX, int srcY, int maskX, int maskY,
		int dstX, int dstY, int width, int height)
{
	MSM_LOCALS(pDstPixmap);
	PixmapPtr pSrcPixmap = exa->src;
	PixmapPtr pMaskPixmap = exa->mask;

	TRACE_EXA("COMPOSITE: srcX=%d\tsrcY=%d\tmaskX=%d\tmaskY=%d\t"
			"dstX=%d\tdstY=%d\twidth=%d\theight=%d\t"
			"srcformat=%08x\tdstformat=%08x",
			srcX, srcY, maskX, maskY, dstX, dstY,
			width, height, exa->srcpic->format, exa->dstpic->format);

	BEGIN_RING(pMsm, 71);
	ring = pMsm->ring.ring;
	out_dstpix(ring, pDstPixmap);

	if (!PICT_FORMAT_A(exa->dstpic->format)) {
		OUT_RING(ring, REGM(G2D_FOREGROUND, 2));
		OUT_RING(ring, 0xff000000);      /* G2D_FOREGROUND */
		OUT_RING(ring, 0xff000000);      /* G2D_BACKGROUND */
		OUT_RING(ring, REGM(G2D_CONST2, 1));
		OUT_RING(ring, 0xff000000);
	} else {
		OUT_RING(ring, REG(G2D_FOREGROUND) | 0x000000);
		OUT_RING(ring, REG(G2D_BACKGROUND) | 0x000000);
	}

	if (!PICT_FORMAT_A(exa->srcpic->format)) {
		OUT_RING(ring, REGM(G2D_CONST0, 1));
		OUT_RING(ring, 0xff000000);
	}

	if (exa->op_dwords[0])
		OUT_RING(ring, exa->op_dwords[0]);
	OUT_RING(ring, exa->op_dwords[1]);
	if (exa->op_dwords[2])
		OUT_RING(ring, exa->op_dwords[2]);
	OUT_RING(ring, exa->op_dwords[3]);

	OUT_RING  (ring, REG(G2D_BLENDERCFG) |
			G2D_BLENDERCFG_ENABLE |
			G2D_BLENDERCFG_OOALPHA |
			(pMaskPixmap ? 0 : G2D_BLENDERCFG_NOMASK) |
			(PICT_FORMAT_A(exa->dstpic->format) ? 0 : 0x00200000));
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	out_srcpix(ring, pSrcPixmap);
	if (exa->srcpic && exa->srcpic->repeat) {
		/* magic: */
		OUT_RING(ring, REGM(GRADW_INST0, 2));
		OUT_RING(ring, 0x10080632);
		OUT_RING(ring, 0x12098695);
		OUT_RING(ring, REGM(GRADW_CONST0, 6));
		OUT_RING(ring, 0x00000000);
		OUT_RING(ring, 0x00400000);
		OUT_RING(ring, 0x0088fa80);
		OUT_RING(ring, 0x00400000);
		OUT_RING(ring, 0x00000000);
		OUT_RING(ring, 0x00890740);
	}
	OUT_RING  (ring, REG(GRADW_TEXCFG2) | 0x0);
	if (pMaskPixmap) {
		OUT_RING  (ring, REG(G2D_GRADIENT) | 0x20000);
		out_srcpix(ring, pMaskPixmap);
		OUT_RING  (ring, REG(GRADW_TEXCFG2) | GRADW_TEXCFG2_ALPHA_TEX);
	}
	if (!(exa->srcpic && exa->srcpic->repeat)) {
		OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	}
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, G2D_INPUT_SCOORD1));
	if (pMaskPixmap) {
		OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, G2D_INPUT_SCOORD2));
	} else {
		OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_SCOORD2));
	}
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | idis(exa, G2D_INPUT_COLOR));
	if (exa->srcpic && exa->srcpic->repeat) {
		OUT_RING  (ring, REG(G2D_GRADIENT) | 0x1001);
	} else {
		OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	}
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_INPUT) | iena(exa, 0));
	OUT_RING  (ring, REG(G2D_CONFIG) |
			G2D_CONFIG_DST | G2D_CONFIG_SRC1 |
			(pMaskPixmap ? G2D_CONFIG_SRC2 : 0));
	OUT_RING  (ring, REGM(G2D_XY, 3));
	OUT_RING  (ring, G2D_XY_X(dstX) | G2D_XY_Y(dstY));/* G2D_XY */
	OUT_RING  (ring, G2D_WIDTHHEIGHT_WIDTH(width) |   /* G2D_WIDTHHEIGHT */
			G2D_WIDTHHEIGHT_HEIGHT(height));
	OUT_RING  (ring, G2D_SXYn_X(srcX) |               /* G2D_SXY */
			G2D_SXYn_Y(srcY));
	if (pMaskPixmap) {
		OUT_RING  (ring, REGM(G2D_SXY2, 1));
		OUT_RING  (ring, G2D_SXYn_X(maskX) |          /* G2D_SXY */
				G2D_SXYn_Y(maskY));
	}
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	OUT_RING  (ring, REG(G2D_GRADIENT) | 0x0);
	END_RING  (pMsm);
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
MSMDoneComposite(PixmapPtr pDst)
{

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
MSMMarkSync(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	return pMsm->ring.timestamp;
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
MSMWaitMarker(ScreenPtr pScreen, int marker)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	if (pMsm->pipe) {
		FIRE_RING(pMsm);
		TRACE_EXA("WAIT: %d", pMsm->ring.timestamp);
		fd_pipe_wait(pMsm->pipe, pMsm->ring.timestamp);
	}
}

static Bool
MSMPixmapIsOffscreen(PixmapPtr pPixmap)
{
	ScreenPtr pScreen = pPixmap->drawable.pScreen;
	struct msm_pixmap_priv *priv;

	if ((pScreen->GetScreenPixmap(pScreen) == pPixmap)) {
		return TRUE;
	}

	priv = exaGetPixmapDriverPrivate(pPixmap);

	if (priv && priv->bo)
		return TRUE;

	return FALSE;
}

static Bool
MSMPrepareAccess(PixmapPtr pPixmap, int index)
{
	static const unsigned int usage[EXA_NUM_PREPARE_INDICES] = {
			[EXA_PREPARE_DEST] = DRM_FREEDRENO_PREP_READ | DRM_FREEDRENO_PREP_WRITE,
			[EXA_PREPARE_SRC]  = DRM_FREEDRENO_PREP_READ,
			[EXA_PREPARE_MASK] = DRM_FREEDRENO_PREP_READ,
			[EXA_PREPARE_AUX_DEST] = DRM_FREEDRENO_PREP_READ | DRM_FREEDRENO_PREP_WRITE,
			[EXA_PREPARE_AUX_SRC]  = DRM_FREEDRENO_PREP_READ,
			[EXA_PREPARE_AUX_MASK] = DRM_FREEDRENO_PREP_READ,
	};
	MSM_LOCALS(pPixmap);
	struct msm_pixmap_priv *priv;

	priv = exaGetPixmapDriverPrivate(pPixmap);

	if (!priv)
		return FALSE;

	if (!priv->bo)
		return TRUE;

	fd_bo_cpu_prep(priv->bo, pMsm->pipe, usage[index]);

	pPixmap->devPrivate.ptr = fd_bo_map(priv->bo);

	return TRUE;
}

static void
MSMFinishAccess(PixmapPtr pPixmap, int index)
{
	struct msm_pixmap_priv *priv;
	priv = exaGetPixmapDriverPrivate(pPixmap);

	if (!priv || !priv->bo)
		return;

	fd_bo_cpu_fini(priv->bo);

	pPixmap->devPrivate.ptr = NULL;
}

static void *
MSMCreatePixmap2(ScreenPtr pScreen, int width, int height,
		int depth, int usage_hint, int bpp,
		int *new_fb_pitch)
{
	struct msm_pixmap_priv *priv;
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	int pitch, size;

	pitch = MSMAlignedStride(width, bpp);
	size = pitch * height;

	*new_fb_pitch = pitch;

	priv = calloc(1, sizeof(struct msm_pixmap_priv));

	if (priv == NULL)
		return NULL;

	if (!size)
		return priv;

	if (usage_hint & CREATE_PIXMAP_USAGE_DRI2) {
		priv->bo = fd_bo_new(pMsm->dev, size,
				DRM_FREEDRENO_GEM_TYPE_KMEM |
				DRM_FREEDRENO_GEM_TYPE_SMI);
	}

	if (!priv->bo) {
		priv->bo = fd_bo_new(pMsm->dev, size,
				DRM_FREEDRENO_GEM_TYPE_KMEM);
	}

	if (priv->bo)
		return priv;

	free(priv);
	return NULL;
}

static void
MSMDestroyPixmap(ScreenPtr pScreen, void *dpriv)
{
	struct msm_pixmap_priv *priv = dpriv;

	if (!priv)
		return;

	if (priv->bo)
		fd_bo_del(priv->bo);

	free(priv);
}

static Bool
MSMPrepareSolidFail(PixmapPtr pPixmap, int alu, Pixel planemask, Pixel fg)
{
	return FALSE;
}

static Bool
MSMPrepareCopyFail(PixmapPtr pSrcPixmap, PixmapPtr pDstPixmap, int dx, int dy,
		int alu, Pixel planemask)
{
	return FALSE;
}

static Bool
MSMPrepareCompositeFail(int op, PicturePtr pSrcPicture, PicturePtr pMaskPicture,
		PicturePtr pDstPicture, PixmapPtr pSrc, PixmapPtr pMask, PixmapPtr pDst)
{
	return FALSE;
}

Bool
MSMSetupExa(ScreenPtr pScreen, Bool softexa)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	struct fd_ringbuffer *ring;
	ExaDriverPtr pExa;

	if (!softexa) {
		pMsm->ring.context_bos[0] = fd_bo_new(pMsm->dev, 0x1000,
				DRM_FREEDRENO_GEM_TYPE_KMEM);
		pMsm->ring.context_bos[1] = fd_bo_new(pMsm->dev, 0x9000,
				DRM_FREEDRENO_GEM_TYPE_KMEM);
		pMsm->ring.context_bos[2] = fd_bo_new(pMsm->dev, 0x81000,
				DRM_FREEDRENO_GEM_TYPE_KMEM);

		/* Set up ringbuffers: */
		next_ring(pMsm);
		ring = pMsm->ring.ring;
		ring_pre(ring);

		/* Set up hardware: */
		BEGIN_RING(pMsm, 8);
		OUT_RING  (ring, REGM(VGV1_DIRTYBASE, 3));
		OUT_RELOC (ring, pMsm->ring.context_bos[0], TRUE); /* VGV1_DIRTYBASE */
		OUT_RELOC (ring, pMsm->ring.context_bos[1], TRUE); /* VGV1_CBASE1 */
		OUT_RELOC (ring, pMsm->ring.context_bos[2], TRUE); /* VGV1_UBASE2 */
		OUT_RING  (ring, 0x11000000);
		OUT_RING  (ring, 0x10fff000);
		OUT_RING  (ring, 0x10ffffff);
		OUT_RING  (ring, 0x0d000404);
		END_RING  (pMsm);
	}

	/* Set up EXA */
	xf86LoadSubModule(pScrn, "exa");

	if (pMsm->pExa == NULL) {
		pMsm->pExa = exaDriverAlloc();
		pMsm->exa = calloc(1, sizeof(*pMsm->exa));
	}

	if (pMsm->pExa == NULL)
		return FALSE;

	pExa = pMsm->pExa;

	/* This is the current major/minor that we support */

	pExa->exa_major = 2;
	pExa->exa_minor = 2;

	/* Max blit extents that hw supports */
	pExa->maxX = 2048;
	pExa->maxY = 2048;

	pExa->flags = EXA_OFFSCREEN_PIXMAPS | EXA_HANDLES_PIXMAPS | EXA_SUPPORTS_PREPARE_AUX;

	/* Align pixmap offsets along page boundaries */
	pExa->pixmapOffsetAlign = 4096;

	/* Align pixmap pitches to the maximum needed aligment for the
      GPU - this ensures that we have enough room, and we adjust the
      pitches down to the depth later */

	pExa->pixmapPitchAlign = 128;

	/* The maximum acceleratable pitch is 2048 pixels */
	pExa->maxPitchPixels = 2048;

	pExa->PrepareSolid       = MSMPrepareSolid;
	pExa->Solid              = MSMSolid;
	pExa->DoneSolid          = MSMDoneSolid;
	pExa->PrepareCopy        = MSMPrepareCopy;
	pExa->Copy               = MSMCopy;
	pExa->DoneCopy           = MSMDoneCopy;
	pExa->CheckComposite     = MSMCheckComposite;
	pExa->PrepareComposite   = MSMPrepareComposite;
	pExa->Composite          = MSMComposite;
	pExa->DoneComposite      = MSMDoneComposite;
	pExa->MarkSync           = MSMMarkSync;
	pExa->WaitMarker         = MSMWaitMarker;
	pExa->PixmapIsOffscreen  = MSMPixmapIsOffscreen;
	pExa->CreatePixmap2      = MSMCreatePixmap2;
	pExa->DestroyPixmap      = MSMDestroyPixmap;
	pExa->PrepareAccess      = MSMPrepareAccess;
	pExa->FinishAccess       = MSMFinishAccess;

	if (softexa) {
		DEBUG_MSG("soft-exa");
		pExa->PrepareSolid   = MSMPrepareSolidFail;
		pExa->PrepareCopy    = MSMPrepareCopyFail;
		pExa->PrepareComposite = MSMPrepareCompositeFail;
	}

	return exaDriverInit(pScreen, pMsm->pExa);
}
