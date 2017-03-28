/* msm-pixmap.c
 *
 * Copyright (c) 2009 - 2010 Code Aurora Forum. All rights reserved.
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

#include "msm.h"

#ifdef HAVE_XA
#  include <xa_tracker.h>
#endif

struct fd_bo *
msm_get_pixmap_bo(PixmapPtr pix)
{
	struct msm_pixmap_priv *priv = exaGetPixmapDriverPrivate(pix);

	if (priv && priv->bo)
		return priv->bo;

#ifdef HAVE_XA
	/* we should only hit this path for pageflip/dri2 (in which case
	 * the buffer is already exported to an flink name):
	 */
	if (priv && priv->surf) {
		MSMPtr pMsm = MSMPTR_FROM_PIXMAP(pix);
		uint32_t name, stride;
		xa_surface_handle(priv->surf, xa_handle_type_shared, &name, &stride);
		priv->bo = fd_bo_from_name(pMsm->dev, name);
		return priv->bo;
	}
#endif

	assert(!priv);

	return NULL;
}

void
msm_set_pixmap_bo(PixmapPtr pix, struct fd_bo *bo)
{
	struct msm_pixmap_priv *priv = exaGetPixmapDriverPrivate(pix);

	if (priv) {
		struct fd_bo *old_bo = priv->bo;
		priv->bo = bo ? fd_bo_ref(bo) : NULL;
		if (old_bo)
			fd_bo_del(old_bo);
#ifdef HAVE_XA
		if (priv->surf) {
			xa_surface_unref(priv->surf);
			priv->surf = NULL;
		}
		if (bo) {
			MSMPtr pMsm = MSMPTR_FROM_PIXMAP(pix);
			if (pMsm->xa) {
				enum xa_surface_type type;
				uint32_t name;

				type = (pix->drawable.bitsPerPixel > 8) ?
						xa_type_argb : xa_type_a;

				fd_bo_get_name(bo, &name);

				priv->surf = xa_surface_from_handle(pMsm->xa,
						pix->drawable.width, pix->drawable.height,
						pix->drawable.depth, type, xa_format_unknown,
						XA_FLAG_SHARED | XA_FLAG_RENDER_TARGET | XA_FLAG_SCANOUT,
						name, exaGetPixmapPitch(pix));
			}
		}
#endif
	}
}

#ifdef HAVE_XA
struct xa_surface *
msm_get_pixmap_surf(PixmapPtr pix)
{
	struct msm_pixmap_priv *priv = exaGetPixmapDriverPrivate(pix);

	if (priv && priv->surf)
		return priv->surf;

	return NULL;
}
#endif

int
msm_get_pixmap_name(PixmapPtr pix, unsigned int *name, unsigned int *stride)
{
	MSMPtr pMsm = MSMPTR_FROM_PIXMAP(pix);
	int ret = -1;

	if (pMsm->xa) {
#ifdef HAVE_XA
		struct xa_surface *surf = msm_get_pixmap_surf(pix);
		if (surf) {
			ret = xa_surface_handle(surf, xa_handle_type_shared, name, stride);
		}
#endif
	} else {
		struct fd_bo *bo = msm_get_pixmap_bo(pix);
		if (bo) {
			*stride = exaGetPixmapPitch(pix);
			ret = fd_bo_get_name(bo, name);
		}
	}

	return ret;
}

void
msm_pixmap_exchange(PixmapPtr a, PixmapPtr b)
{
	struct msm_pixmap_priv *apriv = exaGetPixmapDriverPrivate(a);
	struct msm_pixmap_priv *bpriv = exaGetPixmapDriverPrivate(b);
	exchange(apriv->bo, bpriv->bo);
	exchange(apriv->ptr, bpriv->ptr);
#ifdef HAVE_XA
	exchange(apriv->surf, bpriv->surf);
#endif
}
