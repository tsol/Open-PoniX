/*
 * (c) Copyright IBM Corporation 2007
 * All Rights Reserved.
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
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Ian Romanick <idr@us.ibm.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xgi.h"
#include "xgi_version.h"

#include "xgi_drm.h"
#include "sarea.h"
#include "xgi_dri.h"

static void XGIDRITransitionNoop(ScreenPtr pScreen)
{
    return;
}


static Bool XGICreateContext(ScreenPtr pScreen, VisualPtr visual,
                             drm_context_t hwContext, void *pVisualConfigPriv,
                             DRIContextType contextStore)
{
    return TRUE;
}


static void XGIDestroyContext(ScreenPtr pScreen, drm_context_t hwContext,
                              DRIContextType contextStore)
{
    return;
}


static void XGIDRIInitBuffers(WindowPtr pWin, RegionPtr prgn, CARD32 indx)
{
    return;
}


static void XGIDRIMoveBuffers(WindowPtr pParent, DDXPointRec ptOldOrg,
                              RegionPtr prgnSrc, CARD32 indx)
{
    return;
}


static void XGIDRISwapContext(ScreenPtr pScreen, DRISyncType sync_type,
                              DRIContextType old_ctx_type, void *old_ctx,
                              DRIContextType new_ctx_type, void *new_ctx)
{
    return;
}


Bool XGIDRIScreenInit(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn  = xf86ScreenToScrn(pScreen);
    XGIPtr         pXGI = XGIPTR(pScrn);
    struct xgi_dri_private *dri_priv;
    DRIInfoPtr     dri_info;
    void *scratch_ptr;
    int scratch_int;
    drmVersionPtr kernel_version;

    if (pXGI->dri_info != NULL) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] %s called twice.  Disabling DRI.\n", __func__);
	return FALSE;
    }

    /* Create the DRI data structure, and fill it in before calling the
     * DRIScreenInit().
     */
    dri_info = DRICreateInfoRec();
    if (dri_info == NULL) {
        return FALSE;
    }

    pXGI->dri_info = dri_info;
    dri_info->drmDriverName              = "xgi";
    dri_info->clientDriverName           = "xgi";
    dri_info->busIdString = DRICreatePCIBusID(pXGI->pPciInfo);

    dri_info->ddxDriverMajorVersion      = XGI_VERSION_MAJOR;
    dri_info->ddxDriverMinorVersion      = XGI_VERSION_MINOR;
    dri_info->ddxDriverPatchVersion      = XGI_VERSION_PATCH;
    dri_info->frameBufferPhysicalAddress = (void *) pScrn->memPhysBase;
    dri_info->frameBufferSize            = pXGI->fbSize;
    dri_info->frameBufferStride          = (pScrn->displayWidth *
                                            (pScrn->bitsPerPixel >> 3));
    dri_info->ddxDrawableTableEntry      = SAREA_MAX_DRAWABLES;
    dri_info->maxDrawableTableEntry      = SAREA_MAX_DRAWABLES;


    /* For now the mapping works by using a fixed size defined
     * in the SAREA header
     */
    if (sizeof(XF86DRISAREARec) + sizeof(struct drm_xgi_sarea) > SAREA_MAX) {
        ErrorF("Data does not fit in SAREA\n");
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }
    dri_info->SAREASize = SAREA_MAX;

    dri_priv = calloc(sizeof(struct xgi_dri_private), 1);
    dri_info->devPrivate = dri_priv;

    if (dri_priv == NULL) {
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }

    dri_info->devPrivateSize = sizeof(struct xgi_dri_private);
    dri_info->contextSize    = sizeof(struct XGIDRIContext);

    dri_info->CreateContext  = XGICreateContext;
    dri_info->DestroyContext = XGIDestroyContext;
    dri_info->SwapContext    = XGIDRISwapContext;
    dri_info->InitBuffers    = XGIDRIInitBuffers;
    dri_info->MoveBuffers    = XGIDRIMoveBuffers;
    dri_info->bufferRequests = DRI_ALL_WINDOWS;

    dri_info->createDummyCtx     = TRUE;
    dri_info->createDummyCtxPriv = FALSE;

    if (!DRIScreenInit(pScreen, dri_info, &pXGI->drm_fd)) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] DRIScreenInit failed.  Disabling DRI.\n");
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }

    pXGI->dri_screen_open = TRUE;

    kernel_version = drmGetVersion(pXGI->drm_fd);
    if (kernel_version == NULL) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] Failed to get kernel module version.  "
		   "Disabling DRI.\n");
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }

	
    if ((kernel_version->version_major != 1)
	&& (kernel_version->version_minor != 2)) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] Kernel module version mismatch.  "
		   "Version 1.2.x or later required!  Disabling DRI.\n");
	drmFreeVersion(kernel_version);
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }

    drmFreeVersion(kernel_version);

    DRIGetDeviceInfo(pScreen, &pXGI->fb_handle,
                     &scratch_int, &scratch_int,
                     &scratch_int, &scratch_int,
                     &scratch_ptr);

    return TRUE;
}


Bool XGIDRIFinishScreenInit(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr pXGI = XGIPTR(pScrn);
    struct xgi_dri_private *dri_priv = pXGI->dri_info->devPrivate;
    int err;
    struct xgi_bootstrap  bs;


    if (pXGI->gart_vaddr != NULL) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] %s called twice.  Disabling DRI.\n", __func__);
	return FALSE;
    }

    pXGI->dri_info->driverSwapMethod = DRI_HIDE_X_CONTEXT;

    if (!DRIFinishScreenInit(pScreen)) {
        XGIDRICloseScreen(pScreen);
        return FALSE;
    }


    bs.gart.size = 16 * 1024 * 1024;
    err = drmCommandWriteRead(pXGI->drm_fd, DRM_XGI_BOOTSTRAP, & bs,
			      sizeof(bs));
    if (err) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] Unable to bootstrap card for DMA (%d, %s).  "
		   "Disabling DRI.\n", -err, strerror(-err));
	return FALSE;
    }


    pXGI->gart_size = bs.gart.size;
    pXGI->gart_handle = bs.gart.handle;

    err = drmMap(pXGI->drm_fd, pXGI->gart_handle, pXGI->gart_size,
		 (drmAddressPtr) & pXGI->gart_vaddr);
    if (err) {
        xf86DrvMsg(pScreen->myNum, X_ERROR,
                   "[dri] Unable to map GART range (%d, %s).  "
		   "Disabling DRI.\n", -err, strerror(-err));
	return FALSE;
    }

    dri_priv->bpp = pScrn->bitsPerPixel;
    dri_priv->sarea_priv_offset = sizeof(XF86DRISAREARec);

    return TRUE;
}


void XGIDRICloseScreen(ScreenPtr pScreen)
{
    ScrnInfoPtr    pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr pXGI = XGIPTR(pScrn);

    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "[dri] %s\n", __func__);
    
    if (pXGI->gart_vaddr != NULL) {
	drmUnmap(pXGI->gart_vaddr, pXGI->gart_size);
	pXGI->gart_vaddr = NULL;
    }

    /* De-allocate all DRI resources */
    if (pXGI->dri_screen_open) {
        DRICloseScreen(pScreen);
    }

    /* De-allocate all DRI data structures */
    if (pXGI->dri_info) {
        if (pXGI->dri_info->devPrivate) {
            free(pXGI->dri_info->devPrivate);
            pXGI->dri_info->devPrivate = NULL;
        }

        DRIDestroyInfoRec(pXGI->dri_info);
        pXGI->dri_info = NULL;
    }
}
