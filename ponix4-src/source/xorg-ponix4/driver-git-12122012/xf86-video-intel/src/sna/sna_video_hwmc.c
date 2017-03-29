/*
 * Copyright © 2007 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Authors:
 *    Zhenyu Wang <zhenyu.z.wang@sna.com>
 *
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _SNA_XVMC_SERVER_
#include "sna.h"
#include "sna_video_hwmc.h"

#include <X11/extensions/Xv.h>
#include <X11/extensions/XvMC.h>
#include <fourcc.h>

static int create_subpicture(ScrnInfoPtr scrn, XvMCSubpicturePtr subpicture,
			     int *num_priv, CARD32 ** priv)
{
	return Success;
}

static void destroy_subpicture(ScrnInfoPtr scrn, XvMCSubpicturePtr subpicture)
{
}

static int create_surface(ScrnInfoPtr scrn, XvMCSurfacePtr surface,
			  int *num_priv, CARD32 ** priv)
{
	return Success;
}

static void destroy_surface(ScrnInfoPtr scrn, XvMCSurfacePtr surface)
{
}

static int create_context(ScrnInfoPtr scrn, XvMCContextPtr pContext,
				    int *num_priv, CARD32 **priv)
{
	struct sna *sna = to_sna(scrn);
	struct sna_xvmc_hw_context *contextRec;

	*priv = calloc(1, sizeof(struct sna_xvmc_hw_context));
	contextRec = (struct sna_xvmc_hw_context *) *priv;
	if (!contextRec) {
		*num_priv = 0;
		return BadAlloc;
	}

	*num_priv = sizeof(struct sna_xvmc_hw_context) >> 2;

	if (sna->kgem.gen >= 040) {
		if (sna->kgem.gen >= 045)
			contextRec->type = XVMC_I965_MPEG2_VLD;
		else
			contextRec->type = XVMC_I965_MPEG2_MC;
		contextRec->i965.is_g4x = sna->kgem.gen == 045;
		contextRec->i965.is_965_q = IS_965_Q(sna);
		contextRec->i965.is_igdng = sna->kgem.gen == 050;
	} else {
		contextRec->type = XVMC_I915_MPEG2_MC;
		contextRec->i915.use_phys_addr = 0;
	}

	return Success;
}

static void destroy_context(ScrnInfoPtr scrn, XvMCContextPtr context)
{
}

/* i915 hwmc support */
static XF86MCSurfaceInfoRec i915_YV12_mpg2_surface = {
	FOURCC_YV12,
	XVMC_CHROMA_FORMAT_420,
	0,
	720,
	576,
	720,
	576,
	XVMC_MPEG_2,
	/* XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING, */
	0,
	/* &yv12_subpicture_list */
	NULL,
};

static XF86MCSurfaceInfoRec i915_YV12_mpg1_surface = {
	FOURCC_YV12,
	XVMC_CHROMA_FORMAT_420,
	0,
	720,
	576,
	720,
	576,
	XVMC_MPEG_1,
	/* XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING, */
	0,
	NULL,
};

static XF86MCSurfaceInfoPtr surface_info_i915[2] = {
	(XF86MCSurfaceInfoPtr) & i915_YV12_mpg2_surface,
	(XF86MCSurfaceInfoPtr) & i915_YV12_mpg1_surface
};

/* i965 and later hwmc support */
#ifndef XVMC_VLD
#define XVMC_VLD  0x00020000
#endif

static XF86MCSurfaceInfoRec yv12_mpeg2_vld_surface = {
	FOURCC_YV12,
	XVMC_CHROMA_FORMAT_420,
	0,
	1936,
	1096,
	1920,
	1080,
	XVMC_MPEG_2 | XVMC_VLD,
	XVMC_INTRA_UNSIGNED,
	NULL
};

static XF86MCSurfaceInfoRec yv12_mpeg2_i965_surface = {
	FOURCC_YV12,
	XVMC_CHROMA_FORMAT_420,
	0,
	1936,
	1096,
	1920,
	1080,
	XVMC_MPEG_2 | XVMC_MOCOMP,
	/* XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING, */
	XVMC_INTRA_UNSIGNED,
	/* &yv12_subpicture_list */
	NULL
};

static XF86MCSurfaceInfoRec yv12_mpeg1_i965_surface = {
	FOURCC_YV12,
	XVMC_CHROMA_FORMAT_420,
	0,
	1920,
	1080,
	1920,
	1080,
	XVMC_MPEG_1 | XVMC_MOCOMP,
	/*XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING |
	   XVMC_INTRA_UNSIGNED, */
	XVMC_INTRA_UNSIGNED,

	/*&yv12_subpicture_list */
	NULL
};

static XF86MCSurfaceInfoPtr surface_info_i965[] = {
	&yv12_mpeg2_i965_surface,
	&yv12_mpeg1_i965_surface
};

static XF86MCSurfaceInfoPtr surface_info_vld[] = {
	&yv12_mpeg2_vld_surface,
	&yv12_mpeg2_i965_surface,
};

/* check chip type and load xvmc driver */
Bool sna_video_xvmc_setup(struct sna *sna,
			  ScreenPtr screen,
			  XF86VideoAdaptorPtr target)
{
	XF86MCAdaptorRec *pAdapt;
	const char *name;
	char buf[64];

	/* Needs KMS support. */
	if (sna->kgem.gen < 031)
		return FALSE;

	/* Not implemented */
	if (sna->kgem.gen >= 060)
		return FALSE;

	pAdapt = calloc(1, sizeof(XF86MCAdaptorRec));
	if (!pAdapt)
		return FALSE;

	pAdapt->name = target->name;
	pAdapt->num_subpictures = 0;
	pAdapt->subpictures = NULL;
	pAdapt->CreateContext = create_context;
	pAdapt->DestroyContext = destroy_context;
	pAdapt->CreateSurface = create_surface;
	pAdapt->DestroySurface = destroy_surface;
	pAdapt->CreateSubpicture =  create_subpicture;
	pAdapt->DestroySubpicture = destroy_subpicture;

	if (sna->kgem.gen >= 045) {
		name = "xvmc_vld",
		pAdapt->num_surfaces = ARRAY_SIZE(surface_info_vld);
		pAdapt->surfaces = surface_info_vld;
	} else if (sna->kgem.gen >= 040) {
		name = "i965_xvmc",
		pAdapt->num_surfaces = ARRAY_SIZE(surface_info_i965);
		pAdapt->surfaces = surface_info_i965;
	} else {
		name = "i915_xvmc",
		pAdapt->num_surfaces = ARRAY_SIZE(surface_info_i915);
		pAdapt->surfaces = surface_info_i915;
	}

	if (xf86XvMCScreenInit(screen, 1, &pAdapt)) {
		xf86DrvMsg(sna->scrn->scrnIndex, X_INFO,
			   "[XvMC] %s driver initialized.\n",
			   name);
	} else {
		xf86DrvMsg(sna->scrn->scrnIndex, X_INFO,
			   "[XvMC] Failed to initialize XvMC.\n");
		return FALSE;
	}

	sprintf(buf, "pci:%04x:%02x:%02x.%d",
		sna->PciInfo->domain,
		sna->PciInfo->bus, sna->PciInfo->dev, sna->PciInfo->func);

	xf86XvMCRegisterDRInfo(screen, SNA_XVMC_LIBNAME,
			       buf,
			       SNA_XVMC_MAJOR, SNA_XVMC_MINOR,
			       SNA_XVMC_PATCHLEVEL);
	return TRUE;
}
