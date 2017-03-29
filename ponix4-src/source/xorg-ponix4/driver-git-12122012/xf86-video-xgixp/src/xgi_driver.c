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

/* X and server generic header files */
#include "xf86.h"
#include "xf86_OSproc.h"
#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 6
#include "xf86RAC.h"
#include "xf86Resources.h"
#endif
#include "xf86cmap.h"
#include "xf86xv.h"
#include <xf86i2c.h>
#include <xf86Crtc.h>
#include <xf86RandR12.h>
#include "vbe.h"

/* PCI vendor/device definitions */
#include "xf86PciInfo.h"

/* fbdevhw & vgahw */
#include "fbdevhw.h"
#include "vgaHW.h"
#include "dixstruct.h"

/* initialise a SW cursor */
#include "mipointer.h"

/* colormap initialization */
#include "micmap.h"

#include "xorgVersion.h"

#include "xgi_debug.h"

//#define FB_ACCESS_WRAPPER
#if X_BYTE_ORDER == X_BIG_ENDIAN
#include "wfbrename.h"
#endif
#include "fb.h"

/* Driver data structures */
#include "xgi.h"
#include "xgi_regs.h"
#include "xgi_version.h"
#include "xgi_option.h"
#include "xgi_misc.h"
#include "xgi_bios.h"
#include "xgi_dga.h"
#include "xgi_cursor.h"
#include "xgi_shadow.h"
#include "xgi_video.h"
#include "xgi_misc.h"
#include "xgi_driver.h"
#include "xgi_hwmc.h"
#include "xg47_mode.h"
#include "xg47_misc.h"
#include "xg47_accel.h"
#include "xg47_cursor.h"
#include "xg47_regs.h"
#include "xg47_cmdlist.h"
#include "xg47_i2c.h"

#include <byteswap.h>

/* Jong 09/20/2006; support dual view */
extern int		g_DualViewMode;

/* Jong 10/04/2006; support different resolutions for dual view */
/* DisplayModePtr g_pCurrentModeOfFirstView=0; */

/* Jong 09/06/2006; support dual view */
#ifdef XGIDUALVIEW
static int	XGIEntityIndex = -1;
#endif

/* Jong 05/25/2006 */
#define DBG_FLOW        1

#define FB_MANAGED_BY_X 12*1024*1024

#define XGI_XVMC

static Bool xg47_crtc_config_resize(ScrnInfoPtr scrn, int width, int height);
static xf86MonPtr get_configured_monitor(ScrnInfoPtr pScrn, int index);
static XGIPtr XGIGetRec(ScrnInfoPtr pScrn);
static void     XGIIdentify(int flags);
#ifdef XSERVER_LIBPCIACCESS
static Bool XGIPciProbe(DriverPtr drv, int entity_num, struct pci_device *dev,
    intptr_t match_data);
#else
static Bool     XGIProbe(DriverPtr drv, int flags);
#endif
static Bool     XGIPreInit(ScrnInfoPtr pScrn, int flags);
static Bool     XGIScreenInit(SCREEN_INIT_ARGS_DECL);
static Bool     XGIEnterVT(VT_FUNC_ARGS_DECL);
static void     XGILeaveVT(VT_FUNC_ARGS_DECL);
static Bool     XGICloseScreen(CLOSE_SCREEN_ARGS_DECL);
static Bool     XGISaveScreen(ScreenPtr pScreen, int mode);
/* Optional functions */
static void     XGIFreeScreen(FREE_SCREEN_ARGS_DECL);
static int      XGIValidMode(SCRN_ARG_TYPE arg, DisplayModePtr mode, Bool verbose,
                             int flags);

/* Internally used functions */
static Bool     XGIMapMMIO(ScrnInfoPtr pScrn);
static void XGIUnmapMMIO(ScrnInfoPtr pScrn);
static Bool     XGIMapFB(ScrnInfoPtr pScrn);
static void XGIUnmapFB(ScrnInfoPtr pScrn);
static void XGIUnmapMem(ScrnInfoPtr pScrn);

static void     XGISave(ScrnInfoPtr pScrn);
static void     XGIRestore(ScrnInfoPtr pScrn);
static Bool     XGIModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);

static void     XGIBlockHandler(BLOCKHANDLER_ARGS_DECL);

static void xg47_write_memory_func(void *dst, uint32_t value, int size);
static uint32_t xg47_read_memory_func(const void *src, int size);
static void xg47_setup_fb_wrap(ReadMemoryProcPtr *read_ptr, 
    WriteMemoryProcPtr *write_ptr, DrawablePtr pDraw);
static void xg47_finish_fb_wrap(DrawablePtr pDraw);


static const char *vgahwSymbols[] = {
    "vgaHWBlankScreen",
    "vgaHWddc1SetSpeedWeak",
    "vgaHWFreeHWRec",
    "vgaHWGetHWRec",
    "vgaHWGetIOBase",
    "vgaHWGetIndex",
    "vgaHWInit",
    "vgaHWLock",
    "vgaHWMapMem",
    "vgaHWProtect",
    "vgaHWRestore",
    "vgaHWSave",
    "vgaHWSaveScreen",
    "vgaHWSetMmioFuncs",
    "vgaHWUnlock",
    NULL
};

static const char *ddcSymbols[] = {
    "xf86DoEDID_DDC1",
    "xf86DoEDID_DDC2",
    "xf86PrintEDID",
    "xf86SetDDCproperties",
    NULL
};

static const char *i2cSymbols[] = {
    "xf86CreateI2CBusRec",
    "xf86I2CBusInit",
    NULL
};

static const char *fbSymbols[] = {
#if X_BYTE_ORDER == X_BIG_ENDIAN
    "wfbPictureInit",
    "wfbScreenInit",
#else
    "fbPictureInit",
    "fbScreenInit",
#endif
    NULL
};

static const char *xaaSymbols[] = {
    "XAACreateInfoRec",
    "XAAestroyInfoRec",
    "XAAFillSolidRects",
    "XAAGetCopyROP",
    "XAAGetPatternROP", 
    "XAAInit",
    "XAAScreenIndex",
    "XAAStippleScanlineFuncLSBFirst",
    "XAAOverlayFBfuncs",
    "XAACachePlanarMonoStipple",
    "XAAHelpPatternROP",
    NULL
};

static const char *ramdacSymbols[] = {
    "xf86CreateCursorInfoRec",
    "xf86DestroyCursorInfoRec",
    "xf86InitCursor",
    NULL
};

static const char *drmSymbols[] = {
    "drmGetInterruptFromBusID",
    "drmCtlInstHandler",
    "drmCtlUninstHandler",
    "drmCommandNone",
    "drmCommandRead",
    "drmCommandWrite",
    "drmCommandWriteRead",
    "drmFreeVersion",
    "drmGetLibVersion",
    "drmGetVersion",
    "drmMap",
    "drmUnmap",
    NULL
};

static const char *driSymbols[] = {
    "DRICloseScreen",
    "DRICreateInfoRec",
    "DRIDestroyInfoRec",
    "DRIFinishScreenInit",
    "DRIGetContext",
    "DRIGetDeviceInfo",
    "DRIGetSAREAPrivate",
    "DRILock",
    "DRIQueryVersion",
    "DRIScreenInit",
    "DRIUnlock",
    "DRICreatePCIBusID",
    NULL
};

static const char *vbeSymbols[] = {
    "VBEInit",
    "VBEDPMSSet",
    "vbeDoEDID",
    "vbeFree",
    NULL
};

static const char *int10Symbols[] = {
    "xf86ExecX86int10",
    "xf86FreeInt10",
    "xf86InitInt10",
    NULL
};

static const char *shadowSymbols[] = {
    "shadowInit",
    NULL
};

static const char *fbdevHWSymbols[] = {
    "fbdevHWInit",
    "fbdevHWUseBuildinMode",
    "fbdevHWGetLineLength",
    "fbdevHWGetVidmem",
    "fbdevHWDPMSSet",
    /* colormap */
    "fbdevHWLoadPaletteWeak",
    /* ScrnInfo hooks */
    "fbdevHWAdjustFrameWeak",
    "fbdevHWEnterVT",
    "fbdevHWLeaveVT",
    "fbdevHWModeInit",
    "fbdevHWRestore",
    "fbdevHWSave",
    "fbdevHWSwitchModeWeak",
    "fbdevHWValidModeWeak",
    "fbdevHWMapMMIO",
    "fbdevHWMapVidmem",
    "fbdevHWUnmapMMIO",
    "fbdevHWUnmapVidmem",
    NULL
};

#ifdef XSERVER_LIBPCIACCESS
#define XGI_DEVICE_MATCH(d, i) \
    { 0x18ca, (d), PCI_MATCH_ANY, PCI_MATCH_ANY, 0, 0, (i) }

static const struct pci_id_match xgi_device_match[] = {
    XGI_DEVICE_MATCH(PCI_CHIP_0047, XG47),

    { 0, 0, 0 },
};
#endif

static SymTabRec XGIChipsets[] = {
    { XG47,         "Volari 8300"   },
    { -1,           NULL            }
};

#ifndef XSERVER_LIBPCIACCESS
static PciChipsets XGIPciChipsets[] = {
    { XG47,             PCI_CHIP_0047,    RES_SHARED_VGA },
    { -1,               -1,               RES_UNDEFINED  }
};
#endif

static const xf86CrtcConfigFuncsRec xg47_crtc_config_funcs = {
    .resize = xg47_crtc_config_resize
};

/* Clock Limits */
static int PixelClockLimit8bpp[] = {
    267000, /* Volari 8300 */
};

static int PixelClockLimit16bpp[] = {
    267000, /* Volari 8300 */
};

static int PixelClockLimit24bpp[] = {
    267000, /* Volari 8300 */
};

static int PixelClockLimit32bpp[] = {
    267000, /* Volari 8300 */
};

/*
 * This contains the functions needed by the server after loading the driver
 * module.  It must be supplied, and gets passed back by the SetupProc
 * function in the dynamic case.  In the static case, a reference to this
 * is compiled in, and this requires that the name of this DriverRec be
 * an upper-case version of the driver name.
 */

_X_EXPORT DriverRec XGI = {
    XGI_VERSION_CURRENT,
    XGI_DRIVER_NAME,
    XGIIdentify,
#ifdef XSERVER_LIBPCIACCESS
    NULL,
#else
    XGIProbe,
#endif
    XGIAvailableOptions,
    NULL,
    0,
    NULL,

#ifdef XSERVER_LIBPCIACCESS
    xgi_device_match,
    XGIPciProbe
#endif
};

static MODULESETUPPROTO(XGISetup);

/* Module loader interface for subsidiary driver module */
static XF86ModuleVersionInfo XGIVersionRec =
{
    XGI_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    XGI_VERSION_MAJOR, XGI_VERSION_MINOR, XGI_VERSION_PATCH,
    ABI_CLASS_VIDEODRV,
    ABI_VIDEODRV_VERSION,
    MOD_CLASS_VIDEODRV,
    {0,0,0,0}
};

_X_EXPORT XF86ModuleData xgixpModuleData =
{
    &XGIVersionRec,
    XGISetup,
    NULL
};

/*
 * XGISetup --
 *
 * This function is called every time the module is loaded.
 */

static pointer XGISetup(pointer module,
                        pointer options,
                        int *errorMajor,
                        int *errorMinor)
{
    static Bool isInited = FALSE;
    pointer result;

    /* This module should be loaded only once, but check to be sure. */

    if (!isInited) {
        /*
         * Modules that this driver always requires may be loaded
         * here by calling LoadSubModule().
         */
        isInited = TRUE;
        xf86AddDriver(&XGI, module, 1);

        if (!LoadSubModule(module, 
#if X_BYTE_ORDER == X_BIG_ENDIAN
			   "wfb",
#else
			   "fb",
#endif
			   NULL, NULL, NULL, NULL,
                           NULL, NULL)) {
            return NULL;
        }

        if (!LoadSubModule(module, "vgahw", NULL, NULL, NULL, NULL,
                           NULL, NULL)) {
            return NULL;
        }

        /*
         * The return value must be non-NULL on success even though
         * there is no TearDownProc.
         */
        result = (pointer)TRUE;
    } else {
        if (errorMajor)
            *errorMajor = LDR_ONCEONLY;
        result = NULL;
    }

    return result;
}


static void XGIIdentify(int flags)
{
    xf86PrintChipsets(XGI_NAME, "driver for XGI chipsets", XGIChipsets);
}

#ifdef XSERVER_LIBPCIACCESS
static Bool XGIPciProbe(DriverPtr drv, int entity_num,
                        struct pci_device *dev, intptr_t match_data)
{
    ScrnInfoPtr pScrn;

    pScrn = xf86ConfigPciEntity(NULL, 0, entity_num, NULL,
                                RES_SHARED_VGA, NULL, NULL, NULL, NULL);
    if (pScrn != NULL) {
	XGIPtr pXGI;
#ifdef XGIDUALVIEW
        EntityInfoPtr pEnt;
#endif

        pScrn->driverVersion = XGI_VERSION_CURRENT;
        pScrn->driverName    = XGI_DRIVER_NAME;
        pScrn->name          = XGI_NAME;
        pScrn->PreInit       = XGIPreInit;
        pScrn->ScreenInit    = XGIScreenInit;
        pScrn->SwitchMode    = XGISwitchMode;
        pScrn->AdjustFrame   = XG47AdjustFrame;
        pScrn->EnterVT       = XGIEnterVT;
        pScrn->LeaveVT       = XGILeaveVT;
        pScrn->FreeScreen    = XGIFreeScreen;
        pScrn->ValidMode     = XGIValidMode;

        pXGI = XGIGetRec(pScrn);
        if (pXGI == NULL) {
            return FALSE;
        }

        pXGI->pPciInfo = dev;
        pXGI->chipset = match_data;


        /* Jong 09/06/2006; support dual view */
#ifdef XGIDUALVIEW
        pEnt = xf86GetEntityInfo(entity_num);
        if (g_DualViewMode == 1) {
            XGIEntityPtr pXGIEntity = NULL;
            DevUnion  *pEntityPrivate;

            xf86SetEntitySharable(entity_num);

            if (XGIEntityIndex < 0) {
                XGIEntityIndex = xf86AllocateEntityPrivateIndex();
            }

            pEntityPrivate = xf86GetEntityPrivate(pEnt->index, XGIEntityIndex);
            if (!pEntityPrivate->ptr) {
                pEntityPrivate->ptr = xnfcalloc(sizeof(XGIEntityRec), 1);
                pXGIEntity = pEntityPrivate->ptr;
                memset(pXGIEntity, 0, sizeof(XGIEntityRec));
                pXGIEntity->lastInstance = -1;
            } else {
                pXGIEntity = pEntityPrivate->ptr;
            }

            pXGIEntity->lastInstance++;
            xf86SetEntityInstanceForScreen(pScrn, pScrn->entityList[0],
                                           pXGIEntity->lastInstance);
        }
#endif /* XGIDUALVIEW */
    }

    return (pScrn != NULL);
}
#else
static Bool XGIProbe(DriverPtr drv, int flags)
{
    int     i;
    int     *usedChips = NULL;
    int     numDevSections;
    int     numUsed;
    Bool    foundScreen = FALSE;
    GDevPtr *devSections;

    /*
     * Find the config file Device sections that match this
     * driver, and return if there are none.
     */
    if ((numDevSections = xf86MatchDevice(XGI_DRIVER_NAME, &devSections)) <= 0)
    {
        return FALSE;
    }

    /*
     * While we're VGA-dependent, can really only have one such instance, but
     * we'll ignore that.
    */

    /*
     * We need to probe the hardware first.  We then need to see how this
     * fits in with what is given in the config file, and allow the config
     * file info to override any contradictions.
    */

    /*
     * Since this is a PCI card, "probing" just amounts to checking
     * the PCI data that the server has already collected.  If there
     * is none, return.
     *
     * Although the config file is allowed to override things, it
     * is reasonable to not allow it to override the detection
     * of no PCI video cards.
     *
     * The provided xf86MatchPciInstances() helper takes care of
     * the details.
     */

    if (xf86GetPciVideoInfo() == NULL)
    {
        return FALSE;
    }
    numUsed = xf86MatchPciInstances(XGI_NAME, PCI_VENDOR_XGI,
                                    XGIChipsets, XGIPciChipsets, devSections,
                                    numDevSections, drv, &usedChips);

    if (numUsed <=0)
        return FALSE;

	/* Jong 09/28/2006; support dual view mode */
    if (numUsed >= 2)
		g_DualViewMode=1;

    if (numUsed > 0)
    {
        if (flags & PROBE_DETECT)
            foundScreen = TRUE;
        else for (i = 0; i < numUsed; i++)
        {
            ScrnInfoPtr pScrn = NULL;
            /* ScrnInfoPtr pScrn = xf86AllocateScreen(drv, 0); */

            if ((pScrn = xf86ConfigPciEntity(pScrn, flags, usedChips[i],
                                             XGIPciChipsets, NULL, NULL,
                                             NULL, NULL, NULL)))
            {
                /* Allocate a ScrnInfoRec */
                /* Fill in what we can of the ScrnInfoRec */
                pScrn->driverVersion = XGI_VERSION_CURRENT;
                pScrn->driverName    = XGI_DRIVER_NAME;
                pScrn->name          = XGI_NAME;
                pScrn->Probe         = XGIProbe;
                pScrn->PreInit       = XGIPreInit;
                pScrn->ScreenInit    = XGIScreenInit;
                pScrn->SwitchMode    = XGISwitchMode;
                pScrn->AdjustFrame   = XG47AdjustFrame;
                pScrn->EnterVT       = XGIEnterVT;
                pScrn->LeaveVT       = XGILeaveVT;
                pScrn->FreeScreen    = XGIFreeScreen;
                pScrn->ValidMode     = XGIValidMode;
                foundScreen = TRUE;
                /* add screen to entity */
            }

/* Jong 09/06/2006; support dual view */
#ifdef XGIDUALVIEW
			if(g_DualViewMode == 1)
			{
			   XGIEntityPtr pXGIEntity = NULL;
			   DevUnion  *pEntityPrivate;

			   xf86SetEntitySharable(usedChips[i]); 

			   if(XGIEntityIndex < 0) {
					XGIEntityIndex = xf86AllocateEntityPrivateIndex();
			   }

			   pEntityPrivate = xf86GetEntityPrivate(pScrn->entityList[0], XGIEntityIndex);

			   if(!pEntityPrivate->ptr) {
				  pEntityPrivate->ptr = xnfcalloc(sizeof(XGIEntityRec), 1);
				  pXGIEntity = pEntityPrivate->ptr;
				  memset(pXGIEntity, 0, sizeof(XGIEntityRec));
				  pXGIEntity->lastInstance = -1;
			   } else {
				  pXGIEntity = pEntityPrivate->ptr;
			   }

			   pXGIEntity->lastInstance++;
			   xf86SetEntityInstanceForScreen(pScrn, pScrn->entityList[0],
							pXGIEntity->lastInstance);
#endif /* XGIDUALVIEW */
			}
        }
        free(usedChips);
    }

    free(devSections);
    return foundScreen;
}
#endif

static XGIPtr XGIGetRec(ScrnInfoPtr pScrn)
{
    /*
     * Allocate an XGIRec, and hook it into pScrn->driverPrivate.
     * pScrn->driverPrivate is initialised to NULL, so we can check if
     * the allocation has already been done.
     */
    if (pScrn->driverPrivate == NULL) {
	XGIPtr pXGI = xnfcalloc(sizeof(XGIRec), 1);

	pScrn->driverPrivate = pXGI;
	pXGI->pScrn = pScrn;
    }

    return (XGIPtr) pScrn->driverPrivate;
}

static void XGIFreeRec(ScrnInfoPtr pScrn)
{

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (pScrn->driverPrivate == NULL)
    {
        return;
    }
    free(pScrn->driverPrivate);
    pScrn->driverPrivate = NULL;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

}

/**
 * Memory map the MMIO region.  Used during pre-init.
 */
static Bool XGIMapMMIO(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    int err = 0;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!pXGI->IOBase) {
        if (pXGI->isFBDev) {
            pXGI->IOBase = fbdevHWMapMMIO(pScrn);
            err = (pXGI->IOBase == NULL);
        }
        else {
            /* Map a virtual address IOBase from physical address IOAddr
             * for MMIO
             */
#ifdef XSERVER_LIBPCIACCESS
            err = pci_device_map_region(pXGI->pPciInfo, 1, TRUE);
            pXGI->IOBase = pXGI->pPciInfo->regions[1].memory;
#else
            pXGI->IOBase = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_MMIO,
                                         pXGI->pciTag, pXGI->IOAddr, 
                                         XGI_MMIO_SIZE);
            err = (pXGI->IOBase == NULL);
#endif
        }
    }


    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "IO Map at 0x%p\n", pXGI->IOBase);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return (err == 0);
}


/**
 * Unmap the MMIO region.
 * 
 * \sa XGIUnmapMem, XGIUnmapFB
 */
static void XGIUnmapMMIO(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!pXGI->IOBase)
    {
        if (pXGI->isFBDev) {
            fbdevHWUnmapMMIO(pScrn);
        }
        else {
#ifdef XSERVER_LIBPCIACCESS
            pci_device_unmap_region(pXGI->pPciInfo, 1);
#else
            xf86UnMapVidMem(pScrn->scrnIndex, pXGI->IOBase, XGI_MMIO_SIZE);
#endif
        }

        pXGI->IOBase = NULL;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif
}

/**
 * Memory map the frame buffer.  Used by pre-init.
 */
static Bool XGIMapFB(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    int err = 0;


#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!pXGI->fbBase) {
        if (pXGI->isFBDev) {
            pXGI->fbBase = fbdevHWMapVidmem(pScrn);
            err = (pXGI->fbBase == NULL);
        }
        else {
            /* Make sure that the fbSize has been set (after
             * XGIPreInitMemory has been called) before attempting the
             * mapping.
             */
            if (pXGI->fbSize != 0) {
#ifdef XSERVER_LIBPCIACCESS
                err = pci_device_map_region(pXGI->pPciInfo, 0, TRUE);
                pXGI->fbBase = pXGI->pPciInfo->regions[0].memory;
#else
                pXGI->fbBase = xf86MapPciMem(pScrn->scrnIndex,
                                             VIDMEM_FRAMEBUFFER,
                                             pXGI->pciTag,
                                             pXGI->fbAddr,
                                             pXGI->fbSize);
                err = (pXGI->fbBase == NULL);
#endif

                xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                           "Frame Buffer Map at 0x%p\n", pXGI->fbBase);
            }
        }
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return (err == 0);
}


/**
 * Unmap the frame buffer from process address space.
 *
 * \sa XGIUnmapMem, XGIUnmapMMIO
 */
static void XGIUnmapFB(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!pXGI->fbBase) {
        if (pXGI->isFBDev) {
            fbdevHWUnmapVidmem(pScrn);
        }
        else {
            xf86UnMapVidMem(pScrn->scrnIndex, pXGI->fbBase, pXGI->fbSize);
        }

	pXGI->fbBase = NULL;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif
}


/*
 * Unmap the MMIO region and the frame buffer.
 */
static void XGIUnmapMem(ScrnInfoPtr pScrn)
{
#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    XGIUnmapMMIO(pScrn);
    XGIUnmapFB(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif
}


/*
 * Compute log base 2 of val
 */
int XGIMinBits(int val)
{
    int  bits;

    if (!val) return 1;
    for (bits = 0; val; val >>= 1, ++bits);
    return bits;
}

/*
 * This is called by XGIPreInit to set up the default visual.
 */
static Bool XGIPreInitVisual(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!xf86SetDepthBpp(pScrn, 8, 8, 8, (Support24bppFb
                                        | Support32bppFb
                                        | SupportConvert24to32)))
    {
        return FALSE;
    }

    /* Check that the returned depth is one we support */
    switch (pScrn->depth)
    {
    case 1:
    case 4:
    case 8:
        if (pScrn->bitsPerPixel != pScrn->depth)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Given depth (%d)/ fbbpp (%d) is not supported by this driver\n",
                       pScrn->depth, pScrn->bitsPerPixel);
            return FALSE;
        }
        break;
    case 15:
    case 16:
        if (pScrn->bitsPerPixel != 16)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Given depth (%d)/ fbbpp (%d) is not supported by this driver\n",
                       pScrn->depth, pScrn->bitsPerPixel);
            return FALSE;
        }
        break;
    case 24:
        if ((pScrn->bitsPerPixel != 24) && (pScrn->bitsPerPixel != 32))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Given depth (%d)/ fbbpp (%d) is not supported by this driver\n",
                       pScrn->depth, pScrn->bitsPerPixel);
            return FALSE;
        }
        break;
    case 32:
        if (pScrn->bitsPerPixel != 32)
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Given depth (%d)/ fbbpp (%d) is not supported by this driver\n",
                       pScrn->depth, pScrn->bitsPerPixel);
            return FALSE;
        }
        break;
    default:
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "Given depth (%d) is not supported by this driver\n",
                   pScrn->depth);
        return FALSE;
    }

    /* Print out the depth/bpp that was set */
    xf86PrintDepthBpp(pScrn);

    /* Get pixmap format */
    pXGI->pix24bpp = xf86GetBppFromDepth(pScrn, pScrn->depth);

    /* Set the default visual. */
    if (!xf86SetDefaultVisual(pScrn, -1))   return FALSE;

    /* We don't currently support DirectColor at > 8bpp */
    if (pScrn->depth > 8 && pScrn->defaultVisual != TrueColor)
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Given default visual"
                   " (%s) is not supported at depth %d\n",
                   xf86GetVisualName(pScrn->defaultVisual), pScrn->depth);
        return FALSE;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/**
 * Called by \c XGIPreInit to handle all color weight issues
 */
static Bool XGIPreInitWeight(ScrnInfoPtr pScrn)
{
    XGIPtr  pXGI = XGIPTR(pScrn);


    /* Save flag for 6 bit DAC to use for setting CRTC registers.  Otherwise
     * use an 8 bit DAC, even if xf86SetWeight sets pScrn->rgbBits to some
     * value other than 8.
     */

    if (pScrn->depth > 8) {
        /* The defaults are OK for us */
        rgb defaultWeight = {0, 0, 0};
        rgb defaultMask = {0, 0, 0};

	pXGI->isDac8bits = FALSE;

        if (!xf86SetWeight(pScrn, defaultWeight, defaultMask)) {
            return FALSE;
        }
    } else {
	pXGI->isDac8bits = xf86ReturnOptValBool(pXGI->pOptionInfo,
						OPTION_DAC_8BIT, FALSE);
        pScrn->rgbBits = (pXGI->isDac8bits) ? 8 : 6;
    }

    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "Using %d bits per RGB (%d bit DAC)\n",
               pScrn->rgbBits, pXGI->isDac8bits ? 8 : 6);

    return TRUE;
}

static Bool XGIPreInitInt10(ScrnInfoPtr pScrn)
{
    XGIPtr   pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!xf86LoadSubModule(pScrn, "vbe")
        || !xf86LoadSubModule(pScrn, "int10")) {
        return FALSE;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/*
 * This is called by XGIPreInit to handle config file overrides for things
 * like chipset and memory regions.  Also determine memory size and type.
 * If memory type ever needs an override, put it in this routine.
 */
static Bool XGIPreInitConfig(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
#ifndef XSERVER_LIBPCIACCESS
    EntityInfoPtr   pEnt = pXGI->pEnt;
    GDevPtr         pDev  = pEnt->device;
#endif
    MessageType     from;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* Chipset */
    from = X_PROBED;
    if (pScrn->chipset == NULL)
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "ChipID 0x%04X is not recognised\n", pXGI->chipset);
        return FALSE;
    }

    if (pXGI->chipset < 0)
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "chipset \"%s\" is not recognised\n", pScrn->chipset);
        return FALSE;
    }

    xf86DrvMsg(pScrn->scrnIndex, from,
               "chipset: \"%s\" (chipID = 0x%04x)\n",
                pScrn->chipset,
                pXGI->chipset);

#ifndef XSERVER_LIBPCIACCESS
    /* Determine frame buffer base address from PCI configuration space.
     * It's a physical address 
     */
    from = X_PROBED;
    pXGI->fbAddr = pXGI->pPciInfo->memBase[0] & 0xFFFFFFF0; 

    if (pDev->MemBase) {
        xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                   "Linear address override, using 0x%08x instead of 0x%08x\n",
                    pDev->MemBase,
                    pXGI->fbAddr);
        pXGI->fbAddr = pDev->MemBase;
        from = X_CONFIG;
    } else if (!pXGI->fbAddr) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "No valid linear framebuffer address\n");
        return FALSE;
    }

    xf86DrvMsg(pScrn->scrnIndex, from, "Linear framebuffer at 0x%lx\n",
               pXGI->fbAddr);


    /* Base address of MMIO from PCI configuration space.
     * It's a physical address 
     */
    from = X_PROBED;
    pXGI->IOAddr = pXGI->pPciInfo->memBase[1] & 0xFFFFFFF0;

    if (pDev->IOBase) {
        xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                   "MMIO address override, using 0x%08x instead of 0x%08x\n",
                   pDev->IOBase,
                   pXGI->IOAddr);
        pXGI->IOAddr = pDev->IOBase;
        from = X_CONFIG;
    } else if (!pXGI->IOAddr) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid MMIO address\n");
        return FALSE;
    }
    xf86DrvMsg(pScrn->scrnIndex, X_PROBED,"IO registers at 0x%lx\n", pXGI->IOAddr);
#endif

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

static void XGIPreInitDDC(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    xf86MonPtr pMon = get_configured_monitor(pScrn, pXGI->pEnt->index);

    xf86SetDDCproperties(pScrn, xf86PrintEDID(pMon));
}

/*
 * This is called by XGIPreInit to initialize gamma correction.
 */
static Bool XGIPreInitGamma(ScrnInfoPtr pScrn)
{
    Gamma defaultGamma = {0.0, 0.0, 0.0};

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!xf86SetGamma(pScrn, defaultGamma)) return FALSE;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}


/* This is called by XGIPreInit to validate modes
 * and compute parameters for all of the valid modes.
 */
static Bool XGIPreInitModes(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    ClockRangePtr   pClockRange;

    int             modesFound;
    char            *mod = NULL;
    MessageType     from = X_PROBED;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* there is programmable clock */
    pScrn->progClock = TRUE;

    /* Set the min pixel clock */
    pXGI->minClock = 12000;      /* XXX Guess, need to check this */
    xf86DrvMsg(pScrn->scrnIndex, X_DEFAULT, "Min pixel clock is %d MHz\n", pXGI->minClock / 1000);

    /*
     * If the user has specified ramdac speed in the XF86Config
     * file, we respect that setting.
     */
    if (pXGI->pEnt->device->dacSpeeds[0])
    {
        int speed = 0;

        switch (pScrn->bitsPerPixel)
        {
        case 8:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP8];
            break;
        case 16:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP16];
            break;
        case 24:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP24];
            break;
        case 32:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP32];
            break;
        }
        if (speed == 0)
            pXGI->maxClock = pXGI->pEnt->device->dacSpeeds[0];
        else
            pXGI->maxClock = speed;
        from = X_CONFIG;
    }
    else
    {
        switch (pScrn->bitsPerPixel)
        {
        case 16:
            pXGI->maxClock = PixelClockLimit16bpp[pXGI->chipset];
            break;
        case 24:
            pXGI->maxClock = PixelClockLimit24bpp[pXGI->chipset];
            break;
        case 32:
            pXGI->maxClock = PixelClockLimit32bpp[pXGI->chipset];
            break;
        default:
            pXGI->maxClock = PixelClockLimit8bpp[pXGI->chipset];
            break;
        }
    }
    xf86DrvMsg(pScrn->scrnIndex, from, "Max pixel clock is %d MHz\n", pXGI->maxClock / 1000);

    /*
     * Setup the ClockRanges, which describe what clock ranges are available,
     * and what sort of modes they can be used for.
     */
    pClockRange = xnfcalloc(sizeof(ClockRange), 1);
    pClockRange->next = NULL;

    pClockRange->minClock = pXGI->minClock;
    pClockRange->maxClock = pXGI->maxClock;
    pClockRange->clockIndex = -1;                /* programmable */
    pClockRange->interlaceAllowed = TRUE;
    pClockRange->doubleScanAllowed = FALSE;      /* XXX check this */

    /*
     * xf86ValidateModes will check that the mode HTotal and VTotal values
     * don't exceed the chipset's limit if pScrn->maxHValue and
     * pScrn->maxVValue are set. Since our XGIValidMode() already takes
     * care of this, we don't worry about setting them here.
     */

    if ((pScrn->depth < 8) || (pScrn->bitsPerPixel == 24))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_PROBED,
                   "Disabling Engine due to 24bpp or < 8bpp.\n");
        pXGI->noAccel = TRUE;
    }


    if (!xf86RandR12PreInit(pScrn)) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "RandR initialization failure.\n");
        return FALSE;
    }

    /*
     * Initialise the CRTC fields for the modes.  This driver expects
     * vertical values to be halved for interlaced modes.
     */
    xf86SetCrtcForModes(pScrn, 0);

    /* Set the current mode to the first in the list. */
    pScrn->currentMode = pScrn->modes;

    /* Print the list of modes being used. */
    xf86PrintModes(pScrn);

    /* Set the DPI */
    xf86SetDpi(pScrn, 0, 0);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/*
 * This is called by XGIPreInit to initialize the hardware cursor.
 */
static Bool XGIPreInitCursor(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_SW_CURSOR, FALSE))
    {
        if (!xf86LoadSubModule(pScrn, "ramdac")) return FALSE;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/*
 * This is called by XGIPreInit to initialize hardware acceleration.
 */
static Bool XGIPreInitAccel(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!xf86ReturnOptValBool(pXGI->pOptionInfo, OPTION_NOACCEL, FALSE)) {
        /* Will cause segmentation fault when loading at second time. 
         * FirstView = 0 when in single view.
         */
        if (!g_DualViewMode || pXGI->FirstView) 
            if (!xf86LoadSubModule(pScrn, "xaa")) return FALSE;
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

static Bool XGIPreInitMemory(ScrnInfoPtr pScrn)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    char            *chipset = NULL;
    MessageType     from;


    XG47GetFramebufferSize(pXGI);

    /* Save offset of frame buffer for setting destination and source base in
     * acceleration functions.
     * 
     * On XP5, base address must be less than 64MB=0x4000000.
     */
    pScrn->fbOffset = 0;
#ifdef XGIDUALVIEW
    if (g_DualViewMode) {
        /* Use half of the memory available for each view */
        pXGI->freeFbSize /= 2;
 
        if (!pXGI->FirstView) {
            pScrn->fbOffset = (pXGI->freeFbSize >= 0x4000000)
                ? (pXGI->freeFbSize - 1024) : pXGI->freeFbSize; 
        }
    }
#endif

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* Determine RAM Type */
    switch (pXGI->chipset) {
    case XG47:
        pXGI->isDDRAM = TRUE;
        pXGI->frequency = NTSC;
        chipset = "XG47";
        break;
    default:
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No support for \"%s\"\n", pScrn->chipset);
        return FALSE;
    }

    xf86DrvMsg(pScrn->scrnIndex, X_PROBED, "Found %s chip\n", chipset);


    /* Determine video memory
     */
    if (pXGI->pEnt->device->videoRam != 0) {
        pScrn->videoRam = pXGI->pEnt->device->videoRam;
        from = X_CONFIG;
    }
    else if (pXGI->chipset == XG47) {
        pScrn->videoRam = (pXGI->biosFbSize / 1024);
        from = X_PROBED;
    }

    xf86DrvMsg(pScrn->scrnIndex, from, "VideoRAM: %u KByte\n",
               pScrn->videoRam);


#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}
static void XGIPreInitClock(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);
    MessageType     from = X_PROBED;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* Set the min pixel clock */
    pXGI->minClock = 12000;      /* XXX Guess, need to check this */
    xf86DrvMsg(pScrn->scrnIndex, X_DEFAULT, "Min pixel clock is %d MHz\n", pXGI->minClock / 1000);

    /*
     * If the user has specified ramdac speed in the XF86Config
     * file, we respect that setting.
     */
    if (pXGI->pEnt->device->dacSpeeds[0])
    {
        int speed = 0;

        switch (pScrn->bitsPerPixel)
        {
        case 8:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP8];
            break;
        case 16:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP16];
            break;
        case 24:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP24];
            break;
        case 32:
            speed = pXGI->pEnt->device->dacSpeeds[DAC_BPP32];
            break;
        }
        if (speed == 0)
            pXGI->maxClock = pXGI->pEnt->device->dacSpeeds[0];
        else
            pXGI->maxClock = speed;
        from = X_CONFIG;
    }
    else
    {
        switch (pScrn->bitsPerPixel)
        {
        case 16:
            pXGI->maxClock = PixelClockLimit16bpp[pXGI->chipset];
            break;
        case 24:
            pXGI->maxClock = PixelClockLimit24bpp[pXGI->chipset];
            break;
        case 32:
            pXGI->maxClock = PixelClockLimit32bpp[pXGI->chipset];
            break;
        default:
            pXGI->maxClock = PixelClockLimit8bpp[pXGI->chipset];
            break;
        }
    }
    xf86DrvMsg(pScrn->scrnIndex, from, "Max pixel clock is %d MHz\n", pXGI->maxClock / 1000);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

}

static Bool XGIPreInitShadow(ScrnInfoPtr pScrn)
{
    XGIPtr          pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* Load shadow if needed */
    if (pXGI->isShadowFB)
    {
        if (!xf86LoadSubModule(pScrn, "shadow"))
        {
            XGIFreeRec(pScrn);
            return FALSE;
        }
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

xf86MonPtr get_configured_monitor(ScrnInfoPtr pScrn, int index)
{
    XGIPtr pXGI = XGIPTR(pScrn);
    xf86MonPtr pMon = NULL;


    if (xf86LoadSubModule(pScrn, "i2c")) {
	if (!xg47_InitI2C(pScrn)) {
	    xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		       "I2C initialization failed!\n");
	}
    } else {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "Failed to load i2c module\n");
    }


    if (!xf86LoadSubModule(pScrn, "ddc")) {
        return NULL;
    }

    if (pXGI->pI2C != NULL) {
	pMon = xf86DoEDID_DDC2(XF86_SCRN_ARG(pScrn), pXGI->pI2C);
    }

    if (pMon == NULL) {
        pMon = xf86DoEDID_DDC1(XF86_SCRN_ARG(pScrn), vgaHWddc1SetSpeedWeak(),
                               XG47DDCRead);
    }

    if ((pMon == NULL) && (pXGI->pVbe != NULL)) {
        pMon = vbeDoEDID(pXGI->pVbe, NULL);
    }

    return pMon;
}


/* XGIPreInit is called once at server startup. */
Bool XGIPreInit(ScrnInfoPtr pScrn, int flags)
{
    XGIPtr           pXGI;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    /* Check the number of entities registered for the screen against the expected
     * number (most drivers expect only one).The entity information for
     * each of them should be retrieved by calling xf86GetEntityInfo()).
     * And check for the correct bus type and that none of the sharable
     * resources registered during the Probe phase was rejected.
     */

    /* Access to resources for the entities that can be controlled in a
     * device-independent way are enabled before this function is called.
     * If the driver needs to access any resources that it has disabled in
     * an EntityInit() function that it registered, then it may enable them
     * here providing that it disables them before this function returns.
     */
    if (pScrn->numEntities != 1) return FALSE;

    /* Allocate the XGIRec driverPrivate */

    pXGI = XGIGetRec(pScrn);
    if (pXGI == NULL) return FALSE;


    /* Get the entity, and make sure it is PCI. */
    pXGI->pEnt = xf86GetEntityInfo(pScrn->entityList[0]);

#ifndef XSERVER_LIBPCIACCESS
    if (pXGI->pEnt->resources) return FALSE;
#endif
    if (pXGI->pEnt->location.type != BUS_PCI)   goto fail;

/* Jong 09/06/2006; support dual view */
/* pXGI->pEntityPrivate hasn't been defined */
#ifdef XGIDUALVIEW
    if (xf86IsEntityShared(pScrn->entityList[0])) {
        pXGI->pEntityPrivate = xf86GetEntityPrivate(pScrn->entityList[0], XGIEntityIndex)->ptr;

        /* If PrimInit is not done, assume DVI.  Otherwise assume CRT.
         */
        pXGI->FirstView = (!xf86IsPrimInitDone(pScrn->entityList[0]));
    }
#endif


#ifndef XSERVER_LIBPCIACCESS
    pXGI->pPciInfo = xf86GetPciInfoForEntity(pXGI->pEnt->index);
    pXGI->pciTag = pciTag(pXGI->pPciInfo->bus,
                          pXGI->pPciInfo->device,
                          pXGI->pPciInfo->func);
    xf86DrvMsg(pScrn->scrnIndex, X_INFO,
               "PCI bus %d card %d func %d\n",
               pXGI->pPciInfo->bus,
               pXGI->pPciInfo->device,
               pXGI->pPciInfo->func);

    pXGI->chipset = pXGI->pEnt->chipset;
#endif
    pScrn->chipset = (char *)xf86TokenToString(XGIChipsets, pXGI->chipset);

    pXGI->isFBDev = FALSE;

    /* Fill in the monitor field, just Set pScrn->monitor */
    pScrn->monitor = pScrn->confScreen->monitor;

    if (flags & PROBE_DETECT) {
        get_configured_monitor(pScrn, pXGI->pEnt->index);
        return TRUE;
    }

    if (!vgaHWGetHWRec(pScrn))
    {
        XGIFreeRec(pScrn);
        return FALSE;
    }
    vgaHWSetStdFuncs(VGAHWPTR(pScrn));

    /* All additional resources that the screen needs must be registered here.
     * This should be done with xf86RegisterResources(). If some of the fixed resources
     * registered in the Probe phase are not needed or not decoded by the hardware
     * when in the OPERATING server state, their status should be updated with
     * xf86SetOperatingState().
     */

#ifndef XSERVER_LIBPCIACCESS
    /* Register the PCI-assigned resources. */
    if (xf86RegisterResources(pXGI->pEnt->index, NULL, ResExclusive))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "xf86RegisterResources() found resource conflicts\n");
        return FALSE;
    }
#endif
    /*
     * Set the depth/bpp.  Use the globally preferred depth/bpp.  If the driver
     * has special default depth/bpp requirements, the defaults should be specified
     * here explicitly. Our default depth is 8, so pass it to the helper function.
     * We support both 24bpp and 32bpp framebuffer layouts. Our preference for
     * depth 24 is 24bpp, so tell it that too. This sets pScrn->display also.
     */
    if (!XGIPreInitVisual(pScrn))   goto fail;

    /*
     * xf86SetWeight() and xf86SetDefaultVisual() must be called to
     * set weight/mask/offset for depth > 8 after pScrn->display is initialised.
     */
    if (!XGIPreInitWeight(pScrn))           goto fail;

    if (!XGIPreInitGamma(pScrn))            goto fail;

    if (!XGIProcessOptions(pScrn))          goto fail;

    if (!XGIPreInitConfig(pScrn))           goto fail;
    if (pXGI->isFBDev)
    {

	/* Jong 07/07/2006; has error - _dl_catch_error() from /lib/ld-linux.so.2 ???? */
        if (!xf86LoadSubModule(pScrn, "fbdevhw")) return FALSE;

        /* check for linux framebuffer device */
        if (!fbdevHWInit(pScrn, pXGI->pPciInfo, NULL)) return FALSE;
        pScrn->SwitchMode   = fbdevHWSwitchModeWeak();
        pScrn->AdjustFrame  = fbdevHWAdjustFrameWeak();
        pScrn->ValidMode    = fbdevHWValidModeWeak();
    }

    /* Enable MMIO */
    if (!pXGI->noMMIO) {
        if (!XGIMapMMIO(pScrn)) {
            goto fail;
        }
    }

    if (!XGIPreInitInt10(pScrn))            goto fail;
    if (!XGIPreInitMemory(pScrn))           goto fail;

    /* pScrn->videoRam is determined by XGIPreInitMemory() */
    pXGI->fbSize = pScrn->videoRam * 1024;


    pScrn->progClock = TRUE;

    /* Allocate an xf86CrtcConfig
     */
    xf86CrtcConfigInit(pScrn, & xg47_crtc_config_funcs);
    xf86CrtcSetSizeRange(pScrn, 320, 200, 1920, 1200);


    if (!XGIMapFB(pScrn))                   goto fail;

    XGIPreInitDDC(pScrn);

    xg47_CrtcInit(pScrn, ST_DISP_CRT);
    xg47_PreInitOutputs(pScrn);

    if (!XGIPreInitModes(pScrn))            goto fail;

    if (!XGIPreInitCursor(pScrn))           goto fail;
    if (!xf86InitialConfiguration(pScrn, FALSE)) {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid modes found.\n");
        return FALSE;
    }

    if (!XGIPreInitAccel(pScrn))            goto fail;

    if(!XGIPreInitShadow(pScrn))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                   "shadow initialization failed!\n");
    }


    /* Decide which operations need to be placed under resource access control.
     * The classes of operations are the frame buffer operations (RAC_FB),
     * the pointer operations (RAC_CURSOR), the viewport change operations (RAC_VIEWPORT)
     * and the colormap operations (RAC_COLORMAP). Any operation that requires resources
     * which might be disabled during OPERATING state should be set to use RAC.
     * This can be specified separately for memory and IO resources (the racMemFlags
     * and racIoFlags fields of the ScrnInfoRec respectively).
     */
#ifndef XSERVER_LIBPCIACCESS
    pScrn->racMemFlags = RAC_FB | RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;

    if (pXGI->isMmioOnly)
    {
        pScrn->racIoFlags = 0;
    }
    else
    {
        pScrn->racIoFlags = RAC_FB | RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;
    }
#endif
/* Jong 09/11/2006; support dual view */
#ifdef XGIDUALVIEW
    xf86SetPrimInitDone(pScrn->entityList[0]);
#endif

    return TRUE;
fail:
    XGIUnmapMem(pScrn);

    if (pXGI->pVbe) {
        vbeFree(pXGI->pVbe);
        pXGI->pVbe = NULL;
        pXGI->pInt10 = NULL;
    }

    /* Free int10 info */
    if (pXGI->pInt10) {
        xf86FreeInt10(pXGI->pInt10);
        pXGI->pInt10 = NULL;
    }
    vgaHWFreeHWRec(pScrn);

    XGIFreeRec(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return FALSE;
}

static void XGIBlockHandler(BLOCKHANDLER_ARGS_DECL)
{
    SCREEN_PTR(arg);
    ScrnInfoPtr    pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr         pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    /* xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__); */
#endif

    pScreen->BlockHandler = pXGI->BlockHandler;
    (*pScreen->BlockHandler)(BLOCKHANDLER_ARGS);
    pScreen->BlockHandler = XGIBlockHandler;

#ifdef XvExtension
    if(pXGI->VideoTimerCallback)
    {
        UpdateCurrentTime();
        (*pXGI->VideoTimerCallback)(pScrn, currentTime.milliseconds);
    }
#endif

#if DBG_FLOW
    /* xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__); */
#endif

}

/* XGISave:Save the current video state. This could be called from ChipScreenInit() and
 * (possibly) ChipEnterVT().This will only be saving pre-server states or states before
 * returning to the server. There is only one current saved state per screen and it is
 * stored in private storage in the screen.
 */
static void XGISave(ScrnInfoPtr pScrn)
{
    vgaRegPtr   pVgaReg = &VGAHWPTR(pScrn)->SavedReg;
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIRegPtr   pXGIReg = &pXGI->savedReg;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (pXGI->isFBDev) {
        fbdevHWSave(pScrn);
        return;
    }

    xg47_mode_save(pScrn, pVgaReg, pXGIReg);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

}

/*
 * Restore the initial (text) mode.
 */
/* Restore the original video state. This could be called from the ChipLeaveVT()
 * and ChipCloseScreen() functions.
 * XGIRestore: Restores the saved state from the private storage.
 * Usually only used for restoring text modes.
 */

static void XGIRestore(ScrnInfoPtr pScrn)
{
    XGIPtr      pXGI = XGIPTR(pScrn);
    XGIRegPtr   pXGIReg = &pXGI->savedReg;
    vgaRegPtr   pVgaReg = &VGAHWPTR(pScrn)->SavedReg;

    xg47_mode_restore(pScrn, pVgaReg, pXGIReg);
#if !defined(__powerpc__)
    vgaHWRestore(pScrn, pVgaReg, VGA_SR_FONTS);
#endif
}

Bool XGIFBManagerInit(ScreenPtr pScreen)
{
    ScrnInfoPtr     pScrn = xf86ScreenToScrn(pScreen);
    XGIPtr          pXGI = XGIPTR(pScrn);
    BoxRec          availFBArea;
    CARD16          temp;
    Bool            ret;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    availFBArea.x1 = 0;
    availFBArea.y1 = 0;
    availFBArea.x2 = pScrn->displayWidth;

    temp = (pXGI->fbSize - XGI_CURSOR_BUF_SIZE) / (pScrn->displayWidth * pScrn->bitsPerPixel / 8);

    availFBArea.y2 = (temp > 2047) ? 2047 : temp;


    /* XAA uses FB manager for its pixmap cahe */
    ret = xf86InitFBManager(pScreen, &availFBArea);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return ret;
}

/* Called at the start of each server generation. */
Bool XGIScreenInit(SCREEN_INIT_ARGS_DECL)
{
    /* First Get the ScrnInfoRec */
    ScrnInfoPtr pScrn  = xf86ScreenToScrn(pScreen);
    XGIPtr      pXGI = XGIPTR(pScrn);
    vgaHWPtr    pVgaHW = VGAHWPTR(pScrn);
    VisualPtr   pVisual;

    int         width, height, displayWidth;
    Bool        retValue;
    CARD8       *pFBStart;
    int         visualMask;


    ErrorF("XGI-XGIScreenInit()...\n");

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    XGITRACE(("XGIScreenInit %x %d\n", pScrn->memPhysBase, pScrn->fbOffset));

    pXGI->directRenderingEnabled = XGIDRIScreenInit(pScreen);
    if (!pXGI->directRenderingEnabled) {
        goto fail;
    }

    pScrn->memPhysBase =
#ifdef XSERVER_LIBPCIACCESS
        (pXGI->pPciInfo->regions[0].base_addr & ~0x0F)
#else
        (pXGI->pPciInfo->memBase[0] & 0xFFFFFFF0);
#endif
        + pScrn->fbOffset;

    /*
     * If using the vgahw module, its data structures and related
     * things are typically initialised/mapped here.
     * Map the VGA memory and get the VGA IO base
     */
    if (IsPrimaryCard)
    {
        if (!vgaHWMapMem(pScrn))
            return FALSE;
    }

    if (!pXGI->noMMIO) {
        if (!XGIMapMMIO(pScrn)) {
            goto fail;
        }

        /* Initialize the MMIO vgahw functions */
        vgaHWSetMmioFuncs(pVgaHW, pXGI->IOBase, 0);
    }

    if (!XGIMapFB(pScrn))           goto fail;

    /*
     * Save the current video card state. Enough state must be
     * saved so that the original state can later be restored.
     */
    XGISave(pScrn);

#ifdef XGI_DUMP
    XGIDumpRegisterValue(pScrn);
#endif

    /* Darken the screen for aesthetic reasons and set the viewport.
     */
    XGISaveScreen(pScreen, SCREEN_SAVER_ON);
    pScrn->AdjustFrame(ADJUST_FRAME_ARGS(pScrn, pScrn->frameX0, pScrn->frameY0));


    /*
     * The next step is to setup the screen's visuals, and initialise the
     * framebuffer code.  In cases where the framebuffer's default choices
     * for things like visual layouts and bits per RGB are OK, this may be
     * as simple as calling the framebuffer's ScreenInit() function.
     * If not, the visuals will need to be setup before calling
     * a fb ScreenInit() function and fixed up after.
     *
     * For most PC hardware at depths >= 8, the defaults that cfb uses
     * are not appropriate.  In this driver, we fixup the visuals after.
     */

    /* Reset the visual list */
    miClearVisualTypes();

    /*
     * Setup the visuals we supported.  This driver only supports
     * TrueColor for bpp > 8, so the default set of visuals isn't
     * acceptable.  To deal with this, call miSetVisualTypes with
     * the appropriate visual mask.
     */
    if (!xf86SetDefaultVisual(pScrn, -1))
	goto fail;

    if (pScrn->bitsPerPixel > 8)
        visualMask = miGetDefaultVisualMask(pScrn->depth);
    else
        visualMask = TrueColorMask;

    if (!miSetVisualTypes(pScrn->depth, visualMask, pScrn->rgbBits,
                          pScrn->defaultVisual))
    {
        goto fail;
    }

    if (!miSetPixmapDepths())   goto fail;

    /* FIXME - we don't do shadowfb for < 4 */
    pScrn->displayWidth = pScrn->virtualX;
    displayWidth = pScrn->virtualX;

    if (pXGI->rotate)
    {
        height = pScrn->virtualX;
        width = pScrn->virtualY;
    }
    else
    {
        width = pScrn->virtualX;
        height = pScrn->virtualY;
    }

    if (pXGI->isShadowFB)
    {
        pXGI->shadowPitch = BitmapBytePad(pScrn->bitsPerPixel * width);
        pXGI->pShadow = malloc(pXGI->shadowPitch * height);
        displayWidth = pXGI->shadowPitch / (pScrn->bitsPerPixel >> 3);
        pFBStart = pXGI->pShadow;
    }
    else
    {
        pXGI->isShadowFB = FALSE;
        pXGI->pShadow = NULL;
        pFBStart = pXGI->fbBase;
    }
    /*
     * Initialise the framebuffer.
     * Call the framebuffer layer's ScreenInit function,
     * and fill in other pScreen fields.
     */

pScrn->xDpi = 72;
pScrn->yDpi = 72;
pScrn->pScreen = pScreen;
#if X_BYTE_ORDER == X_BIG_ENDIAN
    retValue = wfbScreenInit(pScreen, pFBStart, width, height,
			     pScrn->xDpi, pScrn->yDpi, displayWidth,
			     pScrn->bitsPerPixel,
			     xg47_setup_fb_wrap, xg47_finish_fb_wrap);
#else
    retValue = fbScreenInit(pScreen, pFBStart, width, height,
			    pScrn->xDpi, pScrn->yDpi, displayWidth,
			    pScrn->bitsPerPixel);
#endif

    if (!retValue)  goto fail;

    /*
     * Set initial black & white colourmap indices.
     */
    xf86SetBlackWhitePixels(pScreen);

    /* Override the default mask/offset settings */
    if (pScrn->bitsPerPixel > 8) {
        /* Fixup RGB ordering */
        pVisual = pScreen->visuals + pScreen->numVisuals;
        while (--pVisual >= pScreen->visuals) {
            if ((pVisual->class | DynamicClass) == DirectColor) {
                pVisual->offsetRed = pScrn->offset.red;
                pVisual->offsetGreen = pScrn->offset.green;
                pVisual->offsetBlue = pScrn->offset.blue;
                pVisual->redMask = pScrn->mask.red;
                pVisual->greenMask = pScrn->mask.green;
                pVisual->blueMask = pScrn->mask.blue;
            }
        }
    }

    /* must be after RGB order fixed */
#if X_BYTE_ORDER == X_BIG_ENDIAN
    wfbPictureInit(pScreen, 0, 0);
#else
    fbPictureInit(pScreen, 0, 0);
#endif

    if (!XGIFBManagerInit(pScreen))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "FB Manager init failed \n");
    }

    xf86SetBackingStore(pScreen);
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After xf86SetBackingStore()\n");

    if (!pXGI->isShadowFB)
    {
        XGIDGAInit(pScreen);
    }

    /* This is an ugly hack.  For reasons that I do not understand, if the
     * mode is not set before calling XGIDRIFinishScreenInit, acceleration
     * will not work.  I assume that there is some register setting that
     * should be done in the kernel but is only done in the mode setting code.
     */
    XG47_NativeModeInit(pScrn, pScrn->currentMode);

    if (pXGI->directRenderingEnabled) {
        pXGI->directRenderingEnabled = XGIDRIFinishScreenInit(pScreen);
	if (!pXGI->directRenderingEnabled) {
	    /* Eventually we should just disable acceleration here.  We'll
	     * also need to call XG47EnableMMIO.
	     */
	    goto fail;
	}
    }

    /* 2D accel Initialize */
    if (!pXGI->noAccel) {
        pXGI->noAccel = !XG47AccelInit(pScreen);
        if (pXGI->noAccel) {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Acceleration initialization failed\n");
        }
    }

    if (!pXGI->noAccel) {
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Acceleration enabled\n");
    } else {
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Acceleration disabled\n");
    }


    if (!XGIEnterVT(VT_FUNC_ARGS))
	goto fail;

    /* Set Silken Mouse */
    xf86SetSilkenMouse(pScreen);
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After xf86SetSilkenMouse()\n");

    /*
     * Hardware cursor setup.  This example is for
     * the mi software cursor.
     */
    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After miDCInitialize()\n");

    if (pXGI->isHWCursor) {
        if (XGIHWCursorInit(pScreen)) {
            int width, height;

            xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                       "Using hardware cursor (scanline %d)\n",
                       (int) pXGI->cursorStart / (int) pScrn->displayWidth);
            if (xf86QueryLargestOffscreenArea(pScreen, &width, &height,
                                              0, 0, 0)) {
                xf86DrvMsg(pScrn->scrnIndex, X_INFO,
                           "Largest offscreen area available: %d x %d\n",
                           width, height);
            }
        } else {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Hardware cursor initialization failed\n");
            pXGI->isHWCursor = FALSE;
        }
    }

    if (!pXGI->isHWCursor) {
        pXGI->cursorStart = 0;
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Using software cursor\n");
    }


    /* Colormap setup
     */
    if (!miCreateDefColormap(pScreen)) return FALSE;
    if (!xf86HandleColormaps(pScreen, 256, pXGI->isDac8bits ? 8 : 6,
                             ((pXGI->isFBDev)
                              ? fbdevHWLoadPaletteWeak() : XG47LoadPalette),
                             XG47SetOverscan,
                             (CMAP_RELOAD_ON_MODE_SWITCH 
                              | CMAP_PALETTED_TRUECOLOR))) {
        return FALSE;
    }
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After xf86HandleColormaps()\n");

    /* shadow frame buffer */
    if (pXGI->isShadowFB)
    {
        if (pXGI->rotate)
        {
            if (!pXGI->PointerMoved)
            {
                pXGI->PointerMoved = pScrn->PointerMoved;
                pScrn->PointerMoved = XGIPointerMoved;
            }
            switch (pScrn->bitsPerPixel)
            {
                case 8:    pXGI->RefreshArea = XGIRefreshArea8; break;
                case 16:   pXGI->RefreshArea = XGIRefreshArea16; break;
                case 24:   pXGI->RefreshArea = XGIRefreshArea24; break;
                case 32:   pXGI->RefreshArea = XGIRefreshArea32; break;
            }
        }
        else
        {
            pXGI->RefreshArea = XGIRefreshArea;
        }
        shadowInit(pScreen, XGIShadowUpdate, 0);
    }
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After pXGI->isShadowFB\n");

    /* DPMS setup */
    xf86DPMSInit(pScreen, xf86DPMSSet, 0);

    /* XV extension */
#ifdef XvExtension
    XGIInitVideo(pScreen); 

    PDEBUG(ErrorF("*-*Jong-XGIInitVideo-End\n"));
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After XvExtension\n");
#endif
#ifdef XGI_XVMC
    PDEBUG(ErrorF("*-*Jong-XGIInitMC-Begin\n"));
    XGIInitMC(pScreen);
    PDEBUG(ErrorF("*-*Jong-XGIInitMC-End\n"));
    XGIDebug(DBG_FUNCTION, "[DBG] Jong 06142006-After XGI_XVMC\n");
#endif

    /* Provide SaveScreen */
    pScreen->SaveScreen  = XGISaveScreen;

    /* Wrap CloseScreen */
    pXGI->CloseScreen    = pScreen->CloseScreen;
    pScreen->CloseScreen = XGICloseScreen;

    xf86CrtcScreenInit(pScreen);

    PDEBUG(ErrorF("*-*Jong-After-XGIInitMC-1\n"));
    /* Report any unused options (only for the first generation) */
    if (serverGeneration == 1)
    {
        xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);
    }

    PDEBUG(ErrorF("Jong-After-XGIInitMC-2\n"));
    pXGI->BlockHandler = pScreen->BlockHandler;
    pScreen->BlockHandler = XGIBlockHandler;

    PDEBUG(ErrorF("Jong-After-XGIInitMC-3\n"));
#ifdef XGI_DUMP
    XGIDumpRegisterValue(pScrn);
#endif

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
fail:
    if (!pXGI->noMMIO && !pXGI->directRenderingEnabled) {
	XG47DisableMMIO(pScrn);
    }
    PDEBUG(ErrorF("Jong-After-XGIInitMC-7\n"));
    XGIUnmapMem(pScrn);
    PDEBUG(ErrorF("Jong-After-XGIInitMC-8\n"));

    if (pXGI->pVbe) {
        vbeFree(pXGI->pVbe);
        pXGI->pVbe = NULL;
        pXGI->pInt10 = NULL;
    }

    PDEBUG(ErrorF("Jong-After-XGIInitMC-10\n"));
    /* Free int10 info */
    if (pXGI->pInt10) {
        xf86FreeInt10(pXGI->pInt10);
        pXGI->pInt10 = NULL;
    }
    PDEBUG(ErrorF("Jong-After-XGIInitMC-11\n"));
    vgaHWFreeHWRec(pScrn);

    PDEBUG(ErrorF("Jong-After-XGIInitMC-12\n"));
    XGIFreeRec(pScrn);

    PDEBUG(ErrorF("Jong-After-XGIInitMC-13\n"));
    return FALSE;
}

/* Usually mandatory */
/* When a SwitchMode event is received, XGISwitchMode() is called (when it exists).
 * Initialises the new mode for the screen identified by index.
 * The viewport may need to be adjusted also.
 */
Bool XGISwitchMode(SWITCH_MODE_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    return XGIModeInit(pScrn, mode);
}


/*
 * This is called when VT switching back to the X server. Its job is
 * to reinitialise the video mode.
 */
static Bool XGIEnterVT(VT_FUNC_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    XGIPtr      pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (!pXGI->noAccel) {
        int ret;
        static const struct xgi_state_info stateInfo = { 0, 1 };

        /* reset KD cmdlist status */
        ret = drmCommandWrite(pXGI->drm_fd, DRM_XGI_STATE_CHANGE, 
                              (void *) &stateInfo, sizeof(stateInfo));
        if (ret < 0) {
            return FALSE;
        }
    }
    else if (!pXGI->noMMIO) {
        XG47EnableMMIO(pScrn);
    }

    if (pXGI->isFBDev) {
        if (!fbdevHWEnterVT(VT_FUNC_ARGS))
            return FALSE;
    } else {
        /* Should we re-save the text mode on each VT enter? */
        if (!XGIModeInit(pScrn, pScrn->currentMode))
            return FALSE;
    }

    if (!pXGI->noAccel) {
        /* reset 2D cmdlist status */
        xg47_Reset(pXGI->cmdList);
    }

    pScrn->AdjustFrame(ADJUST_FRAME_ARGS(pScrn, pScrn->frameX0, pScrn->frameY0));

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/*
 * This is called when VT switching away from the X server.  Its job is
 * to restore the previous (text) mode.
 *
 * We may wish to remap video/MMIO memory too.
 */

/* Mandatory */
static void XGILeaveVT(VT_FUNC_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);
    XGIPtr      pXGI = XGIPTR(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (pXGI == NULL) {
#if DBG_FLOW
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n",
                   __func__, __FILE__, __LINE__);
#endif
        return;
    }

    /* Jong 11/09/2006; only call once */
    if ((g_DualViewMode == 1) && (pXGI->FirstView == 0)) {
#if DBG_FLOW
        xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n",
                   __func__, __FILE__, __LINE__);
#endif
        return;
    }

    if (pXGI->isFBDev) {
        fbdevHWLeaveVT(VT_FUNC_ARGS);
    } else {
        XGIRestore(pScrn);
    }

    if (!pXGI->noAccel) {
        int ret;
        static const struct xgi_state_info stateInfo = { 1, 0 };


        /* reset KD cmdlist status */
        ret = drmCommandWrite(pXGI->drm_fd, DRM_XGI_STATE_CHANGE,
                              (void *) &stateInfo, sizeof(stateInfo));
        if (ret < 0) {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Notify kernel to change state (G==>C)\n");
        }
    } else if (!pXGI->noMMIO) {
        XG47DisableMMIO(pScrn);
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n",
               __func__, __FILE__, __LINE__);
#endif
}

/*
 * This is called at the end of each server generation.  It restores the
 * original (text) mode.  It should really also unmap the video memory too.
 */

/* Mandatory */
static Bool XGICloseScreen(CLOSE_SCREEN_ARGS_DECL)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    vgaHWPtr    pVgaHW = VGAHWPTR(pScrn);
    XGIPtr      pXGI = XGIPTR(pScrn);
    Bool        result;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    if (pXGI->pCursorInfo) {
	XG47HWCursorCleanup(pScreen);
        xf86DestroyCursorInfoRec(pXGI->pCursorInfo);
        pXGI->pCursorInfo = NULL;
    }

    /* Do only one time; otherwise will cause system hang 
     */
    if ((g_DualViewMode == 0 ) || (pScreen->myNum == 1)) {
#ifdef HAVE_XAA_H
	if (pXGI->pXaaInfo) {
	    XG47AccelExit(pScreen);
	    XAADestroyInfoRec(pXGI->pXaaInfo);

	    pXGI->pXaaInfo = NULL;
	}
#endif

    }

    /* Restore the saved video state and unmap the memory regions. */
    if (pScrn->vtSema)
    {
        XGIRestore(pScrn);
    }

    vgaHWLock(pVgaHW);
    if (pXGI->directRenderingEnabled) {
        XGIDRICloseScreen(pScreen);
    }
    else if (!pXGI->noMMIO) {
        XG47DisableMMIO(pScrn);
    }

    XGIUnmapMem(pScrn);

    if (pXGI->pShadow)
    {
        free(pXGI->pShadow);
        pXGI->pShadow = NULL;
    }

    if (pXGI->pDgaModes)
    {
        free(pXGI->pDgaModes);
        pXGI->pDgaModes = NULL;
    }

    if (pXGI->pAdaptor)
    {
        /* free(pXGI->pAdaptor->pPortPrivates[0].ptr); */
        xf86XVFreeVideoAdaptorRec(pXGI->pAdaptor);
        pXGI->pAdaptor = NULL;
    }
    /* The ScrnInfoRec's vtSema field should be set to FALSE
     * once the video HW state has been restored.
     */
    pScrn->vtSema = FALSE;

    if (pXGI->BlockHandler)
        pScreen->BlockHandler = pXGI->BlockHandler;

    if (pXGI->pVbe) {
        vbeFree(pXGI->pVbe);
        pXGI->pVbe = NULL;
        pXGI->pInt10 = NULL;
    }

    if (pXGI->pInt10) {
        xf86FreeInt10(pXGI->pInt10);
        pXGI->pInt10 = NULL;
    }


    /* Before freeing the per-screen driver data the saved CloseScreen
     * value should be restored to pScreen->CloseScreen, and that function
     * should be called after freeing the data.
     */
    pScreen->CloseScreen = pXGI->CloseScreen;

    result = (*pScreen->CloseScreen)(CLOSE_SCREEN_ARGS);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return result;
}

static Bool XGISaveScreen(ScreenPtr pScreen, int mode)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    Bool        unblank;

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    unblank = xf86IsUnblank(mode);

    if (unblank)
        SetTimeSinceLastInputEvent();

    if ((pScrn != NULL) && pScrn->vtSema) {
        vgaHWBlankScreen(pScrn, unblank);
    }

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    return TRUE;
}

/* Optional */
/* Free up any per-generation data structures */
/* Free any driver-allocated data that may have been allocated up to
 * and including an unsuccessful ChipScreenInit() call. This would predominantly
 * be data allocated by ChipPreInit() that persists across server generations.
 * It would include the driverPrivate, and any ``privates'' entries that modules
 * may have allocated.
 */
static void XGIFreeScreen(FREE_SCREEN_ARGS_DECL)
{
    SCRN_INFO_PTR(arg);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "++ Enter %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

    XGITRACE(("XGIFreeScreen\n"));
    if (xf86LoaderCheckSymbol("vgaHWFreeHWRec"))
    {
        vgaHWFreeHWRec(pScrn);
    }
    XGIFreeRec(pScrn);

#if DBG_FLOW
    xf86DrvMsg(pScrn->scrnIndex, X_INFO, "-- Leave %s() %s:%d\n", __FUNCTION__, __FILE__, __LINE__);
#endif

}

/* Optional */
/* Checks if a mode is suitable for the selected chipset. */
/* This function may also modify the effective timings and clock of the passed mode.
 * These have been stored in the mode's Crtc* and SynthClock elements, and have
 * already been adjusted for interlacing, doublescanning, multiscanning and clock
 * multipliers and dividers. The function should not modify any other mode field,
 * unless it wants to modify the mode timings reported to the user by xf86PrintModes().

 * The function is called once for every mode in the XF86Config Monitor section
 * assigned to the screen, with flags set to MODECHECK_INITIAL. It is subsequently
 * called for every mode in the XF86Config Display subsection assigned to the screen,
 * with flags set to MODECHECK_FINAL. In the second case, the mode will have successfully
 * passed all other tests. In addition, the ScrnInfoRec's virtualX, virtualY and
 * displayWidth fields will have been set as if the mode to be validated were to
 * be the last mode accepted.
 *
 * In effect, calls with MODECHECK_INITIAL are intended for checks that do not
 * depend on any mode other than the one being validated, while calls with
 * MODECHECK_FINAL are intended for checks that may involve more than one mode.
 */
static int XGIValidMode(SCRN_ARG_TYPE arg, DisplayModePtr mode, Bool verbose, int flags)
{
    SCRN_INFO_PTR(arg);
    XGIPtr          pXGI = XGIPTR(pScrn);
    int             ret;


    ret = XG47ValidMode(pScrn, mode);

    /* This driver only uses the programmable clock mode.
     */
    mode->ClockIndex = 0x02;


    return ret;
}


static Bool XGIModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    xf86SetSingleMode(pScrn, mode, RR_Rotate_0);
    vgaHWProtect(pScrn, FALSE);
    return TRUE;
}


Bool
xg47_crtc_config_resize(ScrnInfoPtr scrn, int width, int height)
{
    scrn->virtualX = width;
    scrn->virtualY = height;
 
    return TRUE;
}


#if X_BYTE_ORDER == X_BIG_ENDIAN
uint32_t
xg47_read_memory_swap_func(const void *src, int size)
{
    switch (size) {
    case 1: {
	const uint8_t *const ptr = src;
	return *ptr;
    }

    case 2: {
	const uint16_t *const ptr = src;
	return bswap_16(*ptr);
    }

    case 4: {
	const uint32_t *const ptr = src;
	return bswap_32(*ptr);
    }
    }

    return 0;
}


void
xg47_write_memory_swap_func(void *dst, uint32_t value, int size)
{
    switch (size) {
    case 1: {
	uintptr_t addr = (uintptr_t) dst;
	uint8_t *const ptr = (uint8_t *)(addr ^ 3);

	*ptr = value;
	break;
    }

    case 2: {
	uint16_t *const ptr = dst;
	*ptr = bswap_16(value);
	break;
    }

    case 4: {
	uint32_t *const ptr = dst;
	*ptr = bswap_32(value);
	break;
    }
    }
}


uint32_t
xg47_read_memory_func(const void *src, int size)
{
    switch (size) {
    case 1: {
#if 0
	uintptr_t addr = (uintptr_t) src;
	const uint8_t *const ptr = (uint8_t *)(addr ^ 3);
#else
	const uint8_t *const ptr = (const uint8_t *) src;
#endif
	return *ptr;
    }

    case 2: {
	const uint16_t *const ptr = src;
	return *ptr;
    }

    case 4: {
	const uint32_t *const ptr = src;
	return *ptr;
    }
    }

    return 0;
}


void
xg47_write_memory_func(void *dst, uint32_t value, int size)
{
    switch (size) {
    case 1: {
	uint8_t *const ptr = dst;
	*ptr = value;
	break;
    }

    case 2: {
	uint16_t *const ptr = dst;
	*ptr = value;
	break;
    }

    case 4: {
	uint32_t *const ptr = dst;
	*ptr = value;
	break;
    }
    }
}


void
xg47_setup_fb_wrap(ReadMemoryProcPtr *read_ptr, 
		   WriteMemoryProcPtr *write_ptr,
		   DrawablePtr pDraw)
{
    switch (pDraw->type) {
    case DRAWABLE_WINDOW:
#ifdef DRAWABLE_BUFFER
    case DRAWABLE_BUFFER:
#endif
	*read_ptr = xg47_read_memory_swap_func;
	*write_ptr = xg47_write_memory_swap_func;
	break;
	
    default:
	*read_ptr = xg47_read_memory_func;
	*write_ptr = xg47_write_memory_func;
	break;
    }
}


void
xg47_finish_fb_wrap(DrawablePtr pDraw)
{
    (void) pDraw;
}
#endif /* X_BYTE_ORDER == X_BIG_ENDIAN */
