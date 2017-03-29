/***************************************************************************
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.			   *
 *									   *
 * All Rights Reserved.							   *
 *									   *
 * Permission is hereby granted, free of charge, to any person obtaining   *
 * a copy of this software and associated documentation files (the	   *
 * "Software"), to deal in the Software without restriction, including	   *
 * without limitation on the rights to use, copy, modify, merge,	   *
 * publish, distribute, sublicense, and/or sell copies of the Software,	   *
 * and to permit persons to whom the Software is furnished to do so,	   *
 * subject to the following conditions:					   *
 *									   *
 * The above copyright notice and this permission notice (including the	   *
 * next paragraph) shall be included in all copies or substantial	   *
 * portions of the Software.						   *
 *									   *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,	   *
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF	   *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND		   *
 * NON-INFRINGEMENT.  IN NO EVENT SHALL XGI AND/OR			   *
 * ITS SUPPLIERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,	   *
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,	   *
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER	   *
 * DEALINGS IN THE SOFTWARE.						   *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xf86.h"
#include "xf86_OSproc.h"
#include "compiler.h"
#include "xf86PciInfo.h"
#include "xf86Pci.h"
#include "xf86fbman.h"
#include "regionstr.h"

#include "xf86xv.h"
#include "xf86xvmc.h"
#include <X11/extensions/Xv.h>
#include <X11/extensions/XvMC.h>
#include "dixstruct.h"
#include "fourcc.h"

#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_version.h"
#include "xgi_driver.h"
#include "xgi_video.h"
#include "xgi_hwmc.h"

#include "xgi_debug.h"

int     XGIXvMCCreateContext(ScrnInfoPtr pScrn, XvMCContextPtr pContext,
                             int *numPriv, long **priv );
void    XGIXvMCDestroyContext(ScrnInfoPtr pScrn, XvMCContextPtr pContext);
int     XGIXvMCCreateSurface(ScrnInfoPtr pScrn, XvMCSurfacePtr pSurface,
                             int *numPriv, long **priv );
void    XGIXvMCDestroySurface(ScrnInfoPtr pScrn, XvMCSurfacePtr pSurface);
int     XGIXvMCCreateSubpicture(ScrnInfoPtr pScrn, XvMCSubpicturePtr pSurface,
                               int *numPriv, long **priv );
void    XGIXvMCDestroySubpicture(ScrnInfoPtr pScrn, XvMCSubpicturePtr pSurface);

static int YV12SubpictureIndexList[2] =
{
    FOURCC_IA44,
    FOURCC_AI44
};

static XF86MCImageIDList YV12SubpictureList =
{
    2,
    YV12SubpictureIndexList
};

static XF86MCSurfaceInfoRec XGI_YV12_MPEG2_Surface =
{
    FOURCC_YV12,
    XVMC_CHROMA_FORMAT_420,
    0,
    720,
    576,
    720,
    576,
    XVMC_MPEG_2 | XVMC_MOCOMP,    /* XVMC_MOCOMP | XVMC_IDCT */
    XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING |
    XVMC_INTRA_UNSIGNED | XVMC_BACKEND_SUBPICTURE,
    &YV12SubpictureList
};

static XF86MCSurfaceInfoRec XGI_YV12_MPEG1_Surface =
{
    FOURCC_YV12,
    XVMC_CHROMA_FORMAT_420,
    0,
    720,
    576,
    720,
    576,
    XVMC_MPEG_1,
    XVMC_OVERLAID_SURFACE | XVMC_SUBPICTURE_INDEPENDENT_SCALING |
    XVMC_INTRA_UNSIGNED | XVMC_BACKEND_SUBPICTURE,
    &YV12SubpictureList
};

static XF86MCSurfaceInfoPtr ppXGISurface[2] =
{
    (XF86MCSurfaceInfoPtr)&XGI_YV12_MPEG2_Surface,
    (XF86MCSurfaceInfoPtr)&XGI_YV12_MPEG1_Surface
};

/* List of subpicture types that we support */
static XF86ImageRec ia44_subpicture = XVIMAGE_IA44;
static XF86ImageRec ai44_subpicture = XVIMAGE_AI44;

static XF86ImagePtr XGISubpictureList[2] =
{
    (XF86ImagePtr)&ia44_subpicture,
    (XF86ImagePtr)&ai44_subpicture
};

/* Fill in the device dependent adaptor record.
 * This is named "XGI Video Overlay" because this code falls under the
 * XV extenstion, the name must match or it won't be used.
 *
 * Surface and Subpicture - see above
 * Function pointers to functions below
 */
static XF86MCAdaptorRec pAdaptor =
{
    "XGI Video Overlay",        /* name */
    2,                          /* num_surfaces */
    ppXGISurface,               /* surfaces */
    2,                          /* num_subpictures */
    XGISubpictureList,          /* subpictures */
    (xf86XvMCCreateContextProcPtr)XGIXvMCCreateContext,
    (xf86XvMCDestroyContextProcPtr)XGIXvMCDestroyContext,
    (xf86XvMCCreateSurfaceProcPtr)XGIXvMCCreateSurface,
    (xf86XvMCDestroySurfaceProcPtr)XGIXvMCDestroySurface,
    (xf86XvMCCreateSubpictureProcPtr)XGIXvMCCreateSubpicture,
    (xf86XvMCDestroySubpictureProcPtr)XGIXvMCDestroySubpicture
};

static XF86MCAdaptorPtr ppAdaptor[1] =
{
    (XF86MCAdaptorPtr)&pAdaptor
};

/**************************************************************************
 *
 *  XGIInitMC
 *
 *  Initialize the hardware motion compenstation extention for this
 *  hardware. The initialization routines want the address of the pointers
 *  to the structures, not the address of the structures. This means we
 *  allocate (or create static?) the pointer memory and pass that
 *  address. This seems a little convoluted.
 *
 *  We need to allocate memory for the device depended adaptor record.
 *  This is what holds the pointers to all our device functions.
 *
 *  We need to map the overlay registers into the drm.
 *
 *  We need to map the surfaces into the drm.
 *
 *  Inputs:
 *    Screen pointer
 *
 *  Outputs:
 *    None, this calls the device independent screen initialization
 *    function.
 *
 *  Revisions:
 *
 **************************************************************************/

Bool XGIInitMC(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr      pXGI = XGIPTR(pScrn);
    int         i;

    /* Clear the Surface Allocation */
    for(i = 0; i < XGI_MAX_SURFACES; i++)
    {
        pXGI->surfID[i] = 0;
    }

    pXGI->spID = 0;

    return xf86XvMCScreenInit(pScreen, 1, ppAdaptor);
}

/**************************************************************************
 *
 *  XGIXvMCCreateContext
 *
 *  Some info about the private data:
 *
 *  Set *numPriv to the number of 32bit words that make up the size of
 *  of the data that priv will point to.
 *
 *  *priv = (long *) calloc (elements, sizeof(element))
 *  *numPriv = (elements * sizeof(element)) >> 2;
 *
 **************************************************************************/

int XGIXvMCCreateContext(ScrnInfoPtr pScrn, XvMCContextPtr pContext,
                         int *numPriv, long **priv )
{
    XGIPtr                  pXGI = XGIPTR(pScrn);
    XGIXvMCCreateContextPtr pXGIContext = NULL;
    int                     i, ret = 0;

    XGITRACE(("XGIXvMCCreateContext \n"));

    pXGI->directRenderingEnabled = TRUE;
    if (!pXGI->directRenderingEnabled)
    {
        xf86DrvMsg(X_ERROR, pScrn->scrnIndex,
                   "XGIXvMCCreateContext: Cannot use XvMC without DRI!\n");
        return BadAlloc;
    }

    *priv = calloc(1, sizeof(XGIXvMCCreateContextRec));
    if (!*priv)
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "Allocate XGIXvMCCreateContextRec fails\n");
        *numPriv = 0;
        return BadAlloc;
    }
    pXGIContext = (XGIXvMCCreateContextRec *)*priv;

    *numPriv = sizeof(XGIXvMCCreateContextRec) >> 2;

    pXGI->xvmcContext = *pXGIContext;    /* store it */
    pXGIContext->IOAddr =
#ifdef XSERVER_LIBPCIACCESS
	pXGI->pPciInfo->regions[1].base_addr
#else
	pXGI->IOAddr
#endif
	;
    pXGIContext->fbAddr = pScrn->memPhysBase;
    pXGIContext->IOSize = XGI_MMIO_SIZE;
    pXGIContext->fbSize = pXGI->fbSize;

    return Success;
}

int XGIXvMCCreateSurface(ScrnInfoPtr pScrn,
                         XvMCSurfacePtr pSurface,
                         int *numPriv, long **priv )
{
    XGIPtr                  pXGI = XGIPTR(pScrn);
    XGIXvMCCreateSurfacePtr pXGISurface = NULL;
    XvMCContextPtr          pContext = pSurface->context;
    struct xgi_mem_alloc    alloc;
    int                     surfSize, surfPitch, pitchAlign;
    int                     i, ret = 0;

    XGITRACE(("XGIXvMCCreateSurface \n"));

    xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,"surface_type_id = %x width = %d, height = %d\n",
               pContext->surface_type_id, pContext->width, pContext->height);

    *priv = (long *)calloc(1, sizeof(XGIXvMCCreateSurfaceRec));
    if (!*priv)
    {
        xf86DrvMsg(X_ERROR, pScrn->scrnIndex,
                   "Failed to allocate memory for XGIXvMCCreateSurfaceRec!\n");
        *numPriv = 0;
        return BadAlloc;
    }
    memset(*priv, 0, sizeof(XGIXvMCCreateSurfaceRec));

    pitchAlign = 31;
    surfPitch = (pContext->width + pitchAlign) & ~pitchAlign;
    surfSize = surfPitch * pContext->height + surfPitch * (pContext->height >> 1);

    pXGISurface = (XGIXvMCCreateSurfaceRec *)*priv;

    /*size in DWORD unit*/
    *numPriv = sizeof(XGIXvMCCreateSurfaceRec) >> 2;

    for (i = 0; i < XGI_MAX_SURFACES; i++)
    {
        if (!pXGI->surfID[i])
        {
            alloc.location = XGI_MEMLOC_LOCAL;
            alloc.size = surfSize;

	    ret = drmCommandWriteRead(pXGI->drm_fd, DRM_XGI_ALLOC, &alloc,
				      sizeof(alloc));
            if (ret < 0) {
                xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "FB memory allocate ioctl failed !\n");
                return BadAlloc;
            }
            else {
                pXGISurface->index = i;
                pXGISurface->pitch  = surfPitch;
                pXGISurface->size   = surfSize;
                pXGISurface->hwAddr = alloc.hw_addr;

                pXGISurface->offsetY = pXGISurface->hwAddr;
                pXGISurface->offsetV = pXGISurface->offsetY
                                       + pXGISurface->pitch * pContext->height;
                pXGISurface->offsetU = pXGISurface->offsetV
                                       + pXGISurface->pitch * pContext->height / 4;

                pXGI->surfID[i] = pSurface->surface_id;
                pXGI->xvmcSurface[i] = *pXGISurface;

                xf86DrvMsg(pScrn->scrnIndex, X_CONFIG," index: %d surfPitch: %d surfHwAddr %lx\n",
                           i, surfPitch, (unsigned long)alloc.hw_addr);
                return Success;
            }
        }
    }

    return BadAlloc;
}

int XGIXvMCCreateSubpicture(ScrnInfoPtr pScrn,
                            XvMCSubpicturePtr pSubpicture,
                            int *numPriv, long **priv)
{
    XGIPtr                  pXGI = XGIPTR(pScrn);
    XGIPortPtr              pXGIPort = pXGI->pAdaptor->pPortPrivates[0].ptr;
    XvMCContextPtr          pContext = pSubpicture->context;
    XGIXvMCSubpicturePtr    pXGISubpicture = NULL;
    struct xgi_mem_alloc    alloc;
    int                     surfSize, surfPitch, pitchAlign;
    int                     i = 0, ret = 0;

    XGITRACE(("XGIXvMCCreateSubpicture \n"));

    xf86DrvMsg(pScrn->scrnIndex, X_CONFIG, "subpicture_id = %x xvimage_id = %x width = %d, height = %d\n",
               (unsigned int)pSubpicture->subpicture_id, pSubpicture->xvimage_id, pSubpicture->width, pSubpicture->height);

    *priv = (long *)calloc(1, sizeof(XGIXvMCSubpictureRec));
    if (!*priv)
    {
        xf86DrvMsg(X_ERROR, pScrn->scrnIndex,
                   "XGIXvMCCreateSubpicture: Unable to allocate memory!\n");
        *numPriv = 0;
        return (BadAlloc);
    }
    memset(*priv, 0, sizeof(XGIXvMCSubpictureRec));

    *numPriv = sizeof(XGIXvMCSubpictureRec) >> 2;

    pitchAlign = 31;
    surfPitch = (pSubpicture->width + pitchAlign) & ~pitchAlign;
    surfSize = surfPitch * pSubpicture->height * 2; /* two subpicture surface */
    pXGISubpicture = (XGIXvMCSubpictureRec *)*priv;

    if (!pXGI->spID)
    {
        alloc.location = XGI_MEMLOC_LOCAL;
        alloc.size = surfSize;
        ret = drmCommandWriteRead(pXGI->drm_fd, DRM_XGI_ALLOC, &alloc,
				  sizeof(alloc));
        if (ret < 0) {
            memset(*priv, 0, sizeof(XGIXvMCSubpictureRec));
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "FB memory allocate ioctl failed !\n");
            return BadAlloc;
        }
        else {
            pXGISubpicture->pitch  = surfPitch;
            pXGISubpicture->size   = alloc.size;
            pXGISubpicture->hwAddr = alloc.hw_addr;

            pXGI->spID = pSubpicture->subpicture_id;
            pXGI->xvmcSubpic = *pXGISubpicture;
            memset(pXGISubpicture->hwAddr + pXGI->fbBase, i, surfSize);
            xf86DrvMsg(pScrn->scrnIndex, X_CONFIG,"pitch: %d size: %x hwAddr %lx\n",
                       surfPitch, pXGISubpicture->size, (unsigned long)pXGISubpicture->hwAddr);
        }
    }

    pXGIPort->spMode   = pSubpicture->xvimage_id;
    pXGIPort->spWidth  = pSubpicture->width;
    pXGIPort->spHeight = pSubpicture->height;
    pXGIPort->spPitch  = surfPitch;
    return Success;
}

void XGIXvMCDestroyContext (ScrnInfoPtr pScrn, XvMCContextPtr pContext)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

    XGITRACE(("XGIXvMCDestroyContext \n"));

    return;
}

void XGIXvMCDestroySurface (ScrnInfoPtr pScrn, XvMCSurfacePtr pSurface)
{
    XGIPtr                  pXGI = XGIPTR(pScrn);
    XGIXvMCCreateSurfacePtr pXGISurface = NULL;
    unsigned long           surfBusAddr = 0;
    int                     i, ret = 0;

    XGITRACE(("XGIXvMCDestroySurface \n"));

    for (i = 0; i < XGI_MAX_SURFACES; i++)
    {
        if (pXGI->surfID[i] == pSurface->surface_id)
        {
            pXGI->surfID[i] = 0;
            pXGISurface = &(pXGI->xvmcSurface[i]);

            if (pXGISurface != NULL)
            {
                surfBusAddr = pXGISurface->hwAddr;
                ret = drmCommandWrite(pXGI->drm_fd, DRM_XGI_FREE, 
				      &surfBusAddr, sizeof(surfBusAddr));
                if (ret < 0) {
                    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
			       "FB memory allocate failed!\n");
                    return;
                }
                memset(pXGISurface, 0, sizeof(XGIXvMCCreateSurfaceRec));
            }
            return;
        }
    }
    return;
}

void XGIXvMCDestroySubpicture (ScrnInfoPtr pScrn, XvMCSubpicturePtr pSubpicture)
{
    XGIPtr                  pXGI = XGIPTR(pScrn);
    XGIXvMCSubpicturePtr    pXGISubpicture = NULL;
    unsigned long           surfBusAddr = 0;
    int                     i, ret = 0;

    XGITRACE(("XGIXvMCDestroySurface \n"));

    if (pXGI->spID == pSubpicture->subpicture_id) {
        pXGI->spID = 0;
        pXGISubpicture = &(pXGI->xvmcSubpic);
        surfBusAddr = pXGISubpicture->hwAddr;

        memset(pXGISubpicture, 0, sizeof(XGIXvMCSubpictureRec));

        ret = drmCommandWrite(pXGI->drm_fd, DRM_XGI_FREE, &surfBusAddr,
			      sizeof(surfBusAddr));
        if (ret < 0) {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "FB memory free failed!\n");
        }
    }

    return;
}
