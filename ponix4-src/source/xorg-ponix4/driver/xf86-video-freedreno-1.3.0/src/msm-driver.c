/* msm-driver.c
 *
 * Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#include <string.h>
#include <sys/types.h>
#include <grp.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdint.h>

#include "xf86.h"
#include "damage.h"
#include "xf86_OSlib.h"
#include "xf86Crtc.h"

#include "mipointer.h"
#include "micmap.h"
#include "fb.h"
#include "dixstruct.h"

#include "msm.h"
#include "msm-accel.h"
#include "compat-api.h"

#include <drm.h>
#include "xf86drm.h"

#define MSM_NAME        "freedreno"
#define MSM_DRIVER_NAME "freedreno"

#define MSM_VERSION_MAJOR PACKAGE_VERSION_MAJOR
#define MSM_VERSION_MINOR PACKAGE_VERSION_MINOR
#define MSM_VERSION_PATCH PACKAGE_VERSION_PATCHLEVEL

#define MSM_VERSION_CURRENT \
		((MSM_VERSION_MAJOR << 20) |\
				(MSM_VERSION_MINOR << 10) | \
				(MSM_VERSION_PATCH))


/* An aray containing the options that the user can
   configure in xorg.conf
 */

static const OptionInfoRec MSMOptions[] = {
		{OPTION_FB, "fb", OPTV_STRING, {0}, FALSE},
		{OPTION_NOACCEL, "NoAccel", OPTV_BOOLEAN, {0}, FALSE},
		{OPTION_SWCURSOR, "SWCursor", OPTV_BOOLEAN, {0}, FALSE},
		{OPTION_EXAMASK, "examask", OPTV_INTEGER, {0}, FALSE},
		{OPTION_SWREFRESHER, "SWRefresher", OPTV_BOOLEAN, {0}, FALSE},
		{OPTION_VSYNC, "DefaultVsync", OPTV_INTEGER, {0}, FALSE},
		{OPTION_DEBUG, "Debug", OPTV_BOOLEAN, {0}, FALSE},
		{-1, NULL, OPTV_NONE, {0}, FALSE}
};


static Bool MSMEnterVT(VT_FUNC_ARGS_DECL);
static void MSMLeaveVT(VT_FUNC_ARGS_DECL);

Bool msmDebug = TRUE;

static void
MSMBlockHandler (BLOCKHANDLER_ARGS_DECL)
{
	SCREEN_PTR(arg);
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);

	pScreen->BlockHandler = pMsm->BlockHandler;
	(*pScreen->BlockHandler) (BLOCKHANDLER_ARGS);
	pScreen->BlockHandler = MSMBlockHandler;

	if (pScrn->vtSema)
		MSMFlushAccel(pScreen);
}

/*
 * Because we don't use DRI1:
 */

static int
dri_drm_debug_print(const char *format, va_list ap)
{
	xf86VDrvMsgVerb(-1, X_NONE, 3, format, ap);
	return 0;
}

static void
dri_drm_get_perms(gid_t * group, mode_t * mode)
{
	*group = -1;
	*mode = 0666;
}

static drmServerInfo drm_server_info = {
	dri_drm_debug_print,
	xf86LoadKernelModule,
	dri_drm_get_perms,
};

static void
free_msm(MSMPtr pMsm)
{
	if (pMsm->drmFD)
#ifdef XF86_PDEV_SERVER_FD
		if (!(pMsm->pEnt->location.type == BUS_PLATFORM &&
			(pMsm->pEnt->location.id.plat->flags & XF86_PDEV_SERVER_FD)))
#endif
		drmClose(pMsm->drmFD);
	free(pMsm);
}

static Bool
MSMInitDRM(ScrnInfoPtr pScrn)
{
	MSMPtr pMsm = MSMPTR(pScrn);

	drmSetServerInfo(&drm_server_info);

	pMsm->NoKMS = FALSE;

	pMsm->drmFD = drmOpen("msm", NULL);

	if (pMsm->drmFD < 0) {
		pMsm->drmFD = drmOpen("kgsl", NULL);
		pMsm->NoKMS = TRUE;
	}

	if (pMsm->drmFD < 0) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"Unable to open a DRM device\n");
		return FALSE;
	}

	pMsm->dev = fd_device_new(pMsm->drmFD);

	pMsm->deviceName = drmGetDeviceNameFromFd(pMsm->drmFD);

	return TRUE;
}

/* This is the main initialization function for the screen */

static Bool
MSMPreInit(ScrnInfoPtr pScrn, int flags)
{
	MSMPtr pMsm;
	rgb defaultWeight = { 0, 0, 0 };
	Gamma zeros = { 0.0, 0.0, 0.0 };
	unsigned long val;

	DEBUG_MSG("pre-init");

	/* Omit ourselves from auto-probing (which is bound to
	 * fail on our hardware anyway)
	 *
	 * TODO we could probe for drm device..
	 */

	if (flags & PROBE_DETECT) {
		DEBUG_MSG("probe not supported");
		return FALSE;
	}

	if (pScrn->numEntities != 1) {
		DEBUG_MSG("numEntities=%d", pScrn->numEntities);
		return FALSE;
	}

	/* Just use the current monitor specified in the
	 * xorg.conf.  This really means little to us since
	 * we have no choice over which monitor is used,
	 * but X needs this to be set
	 */

	pScrn->monitor = pScrn->confScreen->monitor;

	/* Allocate room for our private data */
	if (pScrn->driverPrivate == NULL)
		pScrn->driverPrivate = xnfcalloc(sizeof(MSMRec), 1);

	pMsm = MSMPTR(pScrn);

	if (pMsm == NULL) {
		ERROR_MSG("Unable to allocate memory");
		return FALSE;
	}

	xf86PrintDepthBpp(pScrn);
	pScrn->rgbBits = 8;

	pScrn->progClock = TRUE;
	pScrn->chipset = MSM_DRIVER_NAME;

	INFO_MSG("MSM/Qualcomm processor");

	pMsm->pEnt = xf86GetEntityInfo(pScrn->entityList[0]);

	if (!MSMInitDRM(pScrn)) {
		ERROR_MSG("Unable to open DRM");
		return FALSE;
	}

	if (pMsm->NoKMS) {
		if (!fbmode_pre_init(pScrn)) {
			ERROR_MSG("fbdev modesetting failed to initialize");
			return FALSE;
		}
	} else {
		if (!drmmode_pre_init(pScrn, pMsm->drmFD, pScrn->bitsPerPixel >> 3)) {
			ERROR_MSG("Kernel modesetting failed to initialize");
			return FALSE;
		}
	}

	xf86CollectOptions(pScrn, NULL);

	pMsm->options = malloc(sizeof(MSMOptions));

	if (pMsm->options == NULL) {
		free_msm(pMsm);
		return FALSE;
	}

	memcpy(pMsm->options, MSMOptions, sizeof(MSMOptions));
	xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, pMsm->options);

	/* Determine if the user wants debug messages turned on: */
	msmDebug = xf86ReturnOptValBool(pMsm->options, OPTION_DEBUG, FALSE);

	/* NoAccel - default FALSE */
	pMsm->NoAccel = xf86ReturnOptValBool(pMsm->options, OPTION_NOACCEL, FALSE);

	/* SWCursor - default FALSE */
	pMsm->HWCursor = !xf86ReturnOptValBool(pMsm->options, OPTION_SWCURSOR, FALSE);

	/* SWRefresher - default TRUE */
	pMsm->SWRefresher = xf86ReturnOptValBool(pMsm->options, OPTION_SWREFRESHER, TRUE);

	if (xf86GetOptValULong(pMsm->options, OPTION_EXAMASK, &val))
		pMsm->examask = val;
	else
		pMsm->examask = ACCEL_DEFAULT;

	INFO_MSG("Option Summary:");
	INFO_MSG("  NoAccel:     %d", pMsm->NoAccel);
	INFO_MSG("  HWCursor:    %d", pMsm->HWCursor);
	INFO_MSG("  examask:     %d", pMsm->examask);
	if (pMsm->NoKMS) {
		const char *fb = xf86GetOptValString(pMsm->options, OPTION_FB);
		INFO_MSG("  fb:          %s", fb);
		INFO_MSG("  SWRefresher: %d", pMsm->SWRefresher);
	}
	INFO_MSG("  Debug:       %d", msmDebug);

	xf86PrintModes(pScrn);

	/* FIXME:  We will probably need to be more exact when setting
	 * the DPI.  For now, we just use the default (96,96 I think) */

	xf86SetDpi(pScrn, 0, 0);

	if (!xf86SetWeight(pScrn, defaultWeight, defaultWeight)) {
		free_msm(pMsm);
		return FALSE;
	}

	/* Initialize default visual */
	if (!xf86SetDefaultVisual(pScrn, -1)) {
		free_msm(pMsm);
		return FALSE;
	}

	if (!xf86SetGamma(pScrn, zeros)) {
		free_msm(pMsm);
		return FALSE;
	}

	INFO_MSG("MSM Options:");
	INFO_MSG(" HW Cursor: %s", pMsm->HWCursor ? "Enabled" : "Disabled");

	return TRUE;
}

static Bool
MSMSaveScreen(ScreenPtr pScreen, int mode)
{
	/* Nothing to do here, yet */
	return TRUE;
}

static Bool
MSMCreateScreenResources(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	PixmapPtr ppix;

	pScreen->CreateScreenResources = pMsm->CreateScreenResources;
	if (!(*pScreen->CreateScreenResources)(pScreen))
		return FALSE;
	pScreen->CreateScreenResources = MSMCreateScreenResources;

	if (!MSMEnterVT(VT_FUNC_ARGS(0)))
		return FALSE;

	ppix = pScreen->GetScreenPixmap(pScreen);
	if (ppix) {
		int pitch = MSMAlignedStride(ppix->drawable.width,
				ppix->drawable.bitsPerPixel);
		pScreen->ModifyPixmapHeader(ppix, ppix->drawable.width,
				ppix->drawable.height, ppix->drawable.depth,
				ppix->drawable.bitsPerPixel, pitch, NULL);
		msm_set_pixmap_bo(ppix, pMsm->scanout);
	}

	return TRUE;
}

static Bool
MSMCloseScreen(CLOSE_SCREEN_ARGS_DECL)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);

	DEBUG_MSG("close screen");

	/* Close DRI2 */
	if (pMsm->dri) {
		MSMDRI2CloseScreen(pScreen);
	}

	/* Close EXA */
	if (pMsm->pExa) {
		exaDriverFini(pScreen);
		free(pMsm->pExa);
		pMsm->pExa = NULL;
	}

	if (pScrn->vtSema) {
		MSMLeaveVT(VT_FUNC_ARGS(0));
		pScrn->vtSema = FALSE;
	}

	if (pMsm->NoKMS) {
		fbmode_screen_fini(pScreen);
	} else {
		drmmode_screen_fini(pScreen);
	}

	pScreen->BlockHandler = pMsm->BlockHandler;
	pScreen->CloseScreen = pMsm->CloseScreen;

	return (*pScreen->CloseScreen)(CLOSE_SCREEN_ARGS);
}

static Bool
MSMScreenInit(SCREEN_INIT_ARGS_DECL)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	int displayWidth;

	DEBUG_MSG("screen-init");

	/* Set up the X visuals */
	miClearVisualTypes();

	/* We only support TrueColor at the moment, and I suspect that is all
	 * we will ever support */

	if (!miSetVisualTypes(pScrn->depth, TrueColorMask,
			pScrn->rgbBits, TrueColor)) {
		ERROR_MSG("Unable to set up the visual for %d BPP", pScrn->bitsPerPixel);
		return FALSE;
	}

	if (!miSetPixmapDepths()) {
		ERROR_MSG("Unable to set the pixmap depth");
		return FALSE;
	}

	/* Set up the X drawing area */

	displayWidth = pScrn->displayWidth;
	if (!displayWidth)
		displayWidth = pScrn->virtualX;

	xf86LoadSubModule(pScrn, "fb");

	if (!fbScreenInit(pScreen, NULL,
			pScrn->virtualX, pScrn->virtualY,
			pScrn->xDpi, pScrn->yDpi,
			displayWidth, pScrn->bitsPerPixel)) {
		ERROR_MSG("fbScreenInit failed");
		return FALSE;
	}

	/* Set up the color information for the visual(s) */

	if (pScrn->bitsPerPixel > 8) {
		VisualPtr visual = pScreen->visuals + pScreen->numVisuals;

		while (--visual >= pScreen->visuals) {
			if ((visual->class | DynamicClass) == DirectColor) {
				visual->offsetRed = pScrn->offset.red;
				visual->offsetGreen = pScrn->offset.green;
				visual->offsetBlue = pScrn->offset.blue;
				visual->redMask = pScrn->mask.red;
				visual->greenMask = pScrn->mask.green;
				visual->blueMask = pScrn->mask.blue;
			}
		}
	}

	/* Set up the Render fallbacks */
	if (!fbPictureInit(pScreen, NULL, 0)) {
		ERROR_MSG("fbPictureInit failed");
		return FALSE;
	}

	/* Set default colors */
	xf86SetBlackWhitePixels(pScreen);

	/* Set up the backing store */
	xf86SetBackingStore(pScreen);

	/* Set up EXA */
	xf86LoadSubModule(pScrn, "exa");

	if (!MSMSetupAccel(pScreen))
		ERROR_MSG("Unable to setup EXA");

	/* Set up the software cursor */
	miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

	/* Try to set up the HW cursor */
	if (pMsm->HWCursor) {
		if (pMsm->NoKMS)
			pMsm->HWCursor = fbmode_cursor_init(pScreen);
		else
			pMsm->HWCursor = drmmode_cursor_init(pScreen);

		if (!pMsm->HWCursor)
			ERROR_MSG("Hardware cursor initialization failed");
	}

	/* Set up the default colormap */

	if (!miCreateDefColormap(pScreen)) {
		ERROR_MSG("miCreateDefColormap failed");
		return FALSE;
	}

	pScreen->SaveScreen = MSMSaveScreen;

	pMsm->CloseScreen = pScreen->CloseScreen;
	pScreen->CloseScreen = MSMCloseScreen;

	pMsm->CreateScreenResources = pScreen->CreateScreenResources;
	pScreen->CreateScreenResources = MSMCreateScreenResources;

	pMsm->BlockHandler = pScreen->BlockHandler;
	pScreen->BlockHandler = MSMBlockHandler;

	if (!xf86CrtcScreenInit(pScreen)) {
		ERROR_MSG("CRTCScreenInit failed");
		return FALSE;
	}

	if (pMsm->NoKMS) {
		if (!fbmode_screen_init(pScreen)) {
			ERROR_MSG("fbmode_screen_init failed");
			return FALSE;
		}
	} else {
		if (!drmmode_screen_init(pScreen)) {
			ERROR_MSG("drmmode_screen_init failed");
			return FALSE;
		}
	}

	return TRUE;
}

static Bool
MSMSwitchMode(SWITCH_MODE_ARGS_DECL)
{
	/* FIXME:  We should only have the one mode, so we shouldn't ever call
	 * this function - regardless, it needs to be stubbed - so what
	 * do we return, TRUE or FALSE? */

	return TRUE;
}

static Bool
MSMEnterVT(VT_FUNC_ARGS_DECL)
{
	SCRN_INFO_PTR(arg);
	MSMPtr pMsm = MSMPTR(pScrn);

	DEBUG_MSG("enter-vt");

	if (!pMsm->NoKMS) {
		int ret;
#ifdef XF86_PDEV_SERVER_FD
		if (!(pMsm->pEnt->location.type == BUS_PLATFORM &&
			(pMsm->pEnt->location.id.plat->flags & XF86_PDEV_SERVER_FD)))
			ret = 0;
		else
#endif
		ret = drmSetMaster(pMsm->drmFD);
		if (ret)
			ERROR_MSG("Unable to get master: %s", strerror(errno));
	}

	/* Set up the mode - this doesn't actually touch the hardware,
	 * but it makes RandR all happy */

	if (!xf86SetDesiredModes(pScrn)) {
		ERROR_MSG("Unable to set the mode");
		return FALSE;
	}

	return TRUE;
}

static void
MSMLeaveVT(VT_FUNC_ARGS_DECL)
{
	SCRN_INFO_PTR(arg);
	MSMPtr pMsm = MSMPTR(pScrn);

	DEBUG_MSG("leave-vt");

	if (!pMsm->NoKMS) {
		int ret;
#ifdef XF86_PDEV_SERVER_FD
		if (!(pMsm->pEnt->location.type == BUS_PLATFORM &&
			(pMsm->pEnt->location.id.plat->flags & XF86_PDEV_SERVER_FD)))
			ret = 0;
		else
#endif
		ret = drmDropMaster(pMsm->drmFD);
		if (ret)
			ERROR_MSG("Unable to drop master: %s", strerror(errno));
	}
}

static void
MSMFreeScreen(FREE_SCREEN_ARGS_DECL)
{
	SCRN_INFO_PTR(arg);
	MSMPtr pMsm = MSMPTR(pScrn);
	free_msm(pMsm);
}

/* ------------------------------------------------------------ */
/* Following is the standard driver setup that probes for the   */
/* hardware and sets up the structures.                         */

static const OptionInfoRec *
MSMAvailableOptions(int chipid, int busid)
{
	return MSMOptions;
}

static void
MSMIdentify(int flags)
{
	xf86Msg(X_INFO, "%s: Video driver for Qualcomm processors\n", MSM_NAME);
}

static Bool
MSMProbe(DriverPtr drv, int flags)
{
	GDevPtr *sections;
	int i, nsects;
	Bool foundScreen = FALSE;
	ScrnInfoPtr pScrn = NULL;

	/* For now, just return false during a probe */

	if (flags & PROBE_DETECT) {
		ErrorF("probe not supported\n");
		return FALSE;
	}

	/* Find all of the device sections in the config */

	nsects = xf86MatchDevice(MSM_NAME, &sections);
	if (nsects <= 0) {
		xf86Msg(X_INFO, "Did not find any matching device "
				"section in configuration file\n");
		if (flags & PROBE_DETECT) {
			/* if we are probing, assume one and lets see if we can
			 * open the device to confirm it is there:
			 */
			nsects = 1;
		} else {
			return FALSE;
		}
	}

	for (i = 0; i < nsects; i++) {
		int entity, drmfd;

		drmfd = drmOpen("msm", NULL);

		if (drmfd < 0) {
			const char *dev;
			int fbdevfd;

			xf86Msg(X_INFO, "No msm DRM/KMS, fallback to fbdev/kgsl\n");

			/* ok, then legacy.. we need an fb too */
			dev = xf86FindOptionValue(sections[i]->options, "fb");

			xf86Msg(X_INFO, "Section %d - looking for %s\n", i, dev);

			if (dev == NULL) {
				xf86Msg(X_WARNING, "no device specified in section %d\n", i);
				continue;
			}

			fbdevfd = open(dev, O_RDWR, 0);
			if (fbdevfd < 0) {
				xf86Msg(X_WARNING, "Could not open fbdev '%s': %s\n",
						dev, strerror(errno));
				continue;
			}
			close(fbdevfd);

			drmfd = drmOpen("kgsl", NULL);
		}

		if (drmfd < 0) {
			xf86Msg(X_WARNING, "Could not open drm: %s\n",
					strerror(errno));
			continue;
		}

		close(drmfd);

		foundScreen = TRUE;

		if (flags & PROBE_DETECT) {
			/* just add the device.. we aren't a PCI device, so
			 * call xf86AddBusDeviceToConfigure() directly
			 */
			xf86AddBusDeviceToConfigure(MSM_DRIVER_NAME,
					BUS_NONE, NULL, i);
			continue;
		}

		entity = xf86ClaimFbSlot(drv, 0, sections[i], TRUE);
		pScrn = xf86ConfigFbEntity(NULL, 0, entity, NULL, NULL, NULL, NULL);

		/* Set up the hooks for the screen */

		pScrn->driverVersion = MSM_VERSION_CURRENT;
		pScrn->driverName = MSM_NAME;
		pScrn->name = MSM_NAME;
		pScrn->Probe = MSMProbe;
		pScrn->PreInit = MSMPreInit;
		pScrn->ScreenInit = MSMScreenInit;
		pScrn->SwitchMode = MSMSwitchMode;
		pScrn->EnterVT = MSMEnterVT;
		pScrn->LeaveVT = MSMLeaveVT;
		pScrn->FreeScreen = MSMFreeScreen;
	}

	free(sections);
	return foundScreen;
}

static Bool
MSMDriverFunc(ScrnInfoPtr scrn, xorgDriverFuncOp op, void *data)
{
	xorgHWFlags *flag;

	switch (op) {
	case GET_REQUIRED_HW_INTERFACES:
		flag = (CARD32 *)data;
		(*flag) = 0;
		return TRUE;
#ifdef XF86_PDEV_SERVER_FD
	case SUPPORTS_SERVER_FDS:
		return TRUE;
#endif
	default:
		return FALSE;
	}
}

#ifdef XSERVER_PLATFORM_BUS
static Bool probe_hw(struct xf86_platform_device *dev)
{
	int fd;

	/* NOTE: for kgsl we still need config file to find fbdev device,
	 * so it will always be probed through the old MSMProbe path.  So
	 * only look for drm/msm here:
	 */

#if XF86_PDEV_SERVER_FD
	if (dev && (dev->flags & XF86_PDEV_SERVER_FD)) {
		drmVersionPtr version;

		fd = xf86_get_platform_device_int_attrib(dev, ODEV_ATTRIB_FD, -1);
		if (fd == -1)
			return FALSE;

		version = drmGetVersion(fd);
		/* make sure we have the right device: */
		if (version && (strcmp(version->name, "msm") == 0)) {
			drmFreeVersion(version);
			return TRUE;
		}

		drmFreeVersion(version);
		return FALSE;
	}
#endif

	fd = drmOpen("msm", NULL);
	if (fd != -1) {
		close(fd);
		return TRUE;
	}

	return FALSE;
}

static Bool
MSMPlatformProbe(DriverPtr driver,
		int entity_num, int flags, struct xf86_platform_device *dev,
		intptr_t match_data)
{
	ScrnInfoPtr pScrn = NULL;
	int scr_flags = 0;

	/* Note: at least for now there is no point in gpu screens.. and
	 * allowing them exposes a bug in older xservers that would result
	 * in the device probed as a gpu screen rather than regular screen
	 * resulting in "No screens found".
	 *
	 * If later there is actually reason to support GPU screens, track
	 * down the first xorg ABI # that contains the fix, and make this
	 * conditional on that or later ABI versions.  Otherwise you will
	 * break things for people with older xservers.
	 *
	if (flags & PLATFORM_PROBE_GPU_SCREEN)
		scr_flags = XF86_ALLOCATE_GPU_SCREEN;
	 */

	if (probe_hw(dev)) {
		pScrn = xf86AllocateScreen(driver, scr_flags);
		xf86AddEntityToScreen(pScrn, entity_num);

		pScrn->driverVersion = MSM_VERSION_CURRENT;
		pScrn->driverName = MSM_NAME;
		pScrn->name = MSM_NAME;
		pScrn->Probe = MSMProbe;
		pScrn->PreInit = MSMPreInit;
		pScrn->ScreenInit = MSMScreenInit;
		pScrn->SwitchMode = MSMSwitchMode;
		pScrn->EnterVT = MSMEnterVT;
		pScrn->LeaveVT = MSMLeaveVT;
		pScrn->FreeScreen = MSMFreeScreen;
	}

	return pScrn != NULL;
}
#endif

_X_EXPORT DriverRec freedrenoDriver = {
		MSM_VERSION_CURRENT,
		MSM_DRIVER_NAME,
		MSMIdentify,
		MSMProbe,
		MSMAvailableOptions,
		NULL,
		0,
		MSMDriverFunc,
		NULL,
		NULL,  /* pci_probe */
#ifdef XSERVER_PLATFORM_BUS
		MSMPlatformProbe,
#endif
};

MODULESETUPPROTO(freedrenoSetup);

/* Versioning information for the module - most of these variables will
   come from config.h generated by ./configure
 */

static XF86ModuleVersionInfo freedrenoVersRec = {
		MSM_DRIVER_NAME,
		MODULEVENDORSTRING,
		MODINFOSTRING1,
		MODINFOSTRING2,
		XORG_VERSION_CURRENT,
		MSM_VERSION_MAJOR, MSM_VERSION_MINOR, MSM_VERSION_PATCH,
		ABI_CLASS_VIDEODRV,
		ABI_VIDEODRV_VERSION,
		NULL,
		{0, 0, 0, 0},
};

_X_EXPORT XF86ModuleData freedrenoModuleData = { &freedrenoVersRec, freedrenoSetup, NULL };

pointer
freedrenoSetup(pointer module, pointer ops, int *errmaj, int *errmin)
{
	static Bool initDone = FALSE;

	if (initDone == FALSE) {
		initDone = TRUE;
		xf86AddDriver(&freedrenoDriver, module, HaveDriverFuncs);

		/* FIXME: Load symbol references here */
		return (pointer) 1;
	} else {
		if (errmaj)
			*errmaj = LDR_ONCEONLY;
		return NULL;
	}
}
