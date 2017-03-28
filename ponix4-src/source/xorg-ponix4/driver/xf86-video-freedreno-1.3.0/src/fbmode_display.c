/* msm-output.c
 *
 * Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#ifdef HAVE_XEXTPROTO_71
#include <X11/extensions/dpmsconst.h>
#else
#define DPMS_SERVER
#include <X11/extensions/dpms.h>
#endif

#include <sys/ioctl.h>
#include "xf86.h"
#include "xf86Crtc.h"
#include "xf86_OSlib.h"
#include "msm.h"

#include <linux/fb.h>
#include <linux/ioctl.h>
#include <linux/msm_kgsl.h>
#include <linux/msm_mdp.h>

typedef enum
{
	MSM_MDP_VERSION_22,
	MSM_MDP_VERSION_31,
	MSM_MDP_VERSION_40,
} MSMChipType;

typedef struct {
	/* File descriptor for the framebuffer device */
	int fd;

	/* Fixed and var strutures from the framebuffer */
	struct fb_fix_screeninfo fixed_info;
	struct fb_var_screeninfo mode_info;

	/* Pointer to the mapped framebuffer memory */
	void *fbmem;

	/* Processor identifier */
	MSMChipType chipID;

	/* Default mode for X */
	DisplayModeRec default_mode;
	int HWCursorState;
	int defaultVsync;
	PixmapPtr rotatedPixmap;
} fbmode_rec, *fbmode_ptr;

#define MSM_CURSOR_WIDTH 64
#define MSM_CURSOR_HEIGHT 64

static void MSMGetDefaultMode(fbmode_ptr fbmode);


static fbmode_ptr
fbmode_from_scrn(ScrnInfoPtr scrn)
{
	if (scrn) {
		xf86CrtcConfigPtr conf = XF86_CRTC_CONFIG_PTR(scrn);
		return conf->crtc[0]->driver_private;
	}

	return NULL;
}

static void
MSMCrtcGammaSet(xf86CrtcPtr crtc,
		CARD16 *red, CARD16 *green, CARD16 *blue, int size)
{
	/* This is a new function that gets called by the DI code  */

}

static void
MSMCrtcDPMS(xf86CrtcPtr crtc, int mode)
{
	/* TODO: Implement DPMS */
}

static Bool
MSMCrtcLock(xf86CrtcPtr crtc)
{
	return TRUE;
}

static void
MSMCrtcUnlock(xf86CrtcPtr crtc)
{
}

static void
MSMCrtcPrepare(xf86CrtcPtr crtc)
{
	/* Blank the display before we change modes? */
}

static Bool
MSMCrtcModeFixup(xf86CrtcPtr crtc, DisplayModePtr mode,
		DisplayModePtr adjmode)
{
	fbmode_ptr fbmode = crtc->driver_private;

	if (mode->HDisplay > fbmode->mode_info.xres_virtual ||
			mode->VDisplay > fbmode->mode_info.yres_virtual)
		return FALSE;

	return TRUE;
}

static void
MSMCrtcModeSet(xf86CrtcPtr crtc, DisplayModePtr mode,
		DisplayModePtr adjmode, int x, int y)
{
	int ret;
	fbmode_ptr fbmode = crtc->driver_private;
	ScrnInfoPtr pScrn = crtc->scrn;
	struct fb_var_screeninfo var;
	int htotal, vtotal;

	memcpy(&var, &fbmode->mode_info, sizeof(var));

	htotal = var.xres + var.right_margin + var.hsync_len + var.left_margin;

	var.xres = adjmode->HDisplay;
	var.right_margin = adjmode->HSyncStart - adjmode->HDisplay;
	var.hsync_len = adjmode->HSyncEnd - adjmode->HSyncStart;
	var.left_margin = adjmode->HTotal - adjmode->HSyncEnd;

	vtotal = var.yres + var.lower_margin + var.vsync_len + var.upper_margin;

	var.yres = adjmode->VDisplay;
	var.lower_margin = adjmode->VSyncStart - adjmode->VDisplay;
	var.vsync_len = adjmode->VSyncEnd - adjmode->VSyncStart;
	var.upper_margin = adjmode->VTotal - adjmode->VSyncEnd;

	if (vtotal != adjmode->VTotal || htotal != adjmode->HTotal)
		var.pixclock = fbmode->defaultVsync * adjmode->HTotal * adjmode->VTotal;
	/*crtc->rotatedData!= NULL indicates that rotation has been requested
   and shadow framebuffer has been allocated, so change the yoffset to make
   the shadow framebuffer as visible screen. */
	var.yoffset = (crtc->rotatedData && crtc->rotation != 1) ? var.yres : 0;

	ret = ioctl(fbmode->fd, FBIOPUT_VSCREENINFO, &var);

	if (ret)
		xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Unable to change the mode: %m\n");
	else {
		/* Refresh the changed settings from the driver */
		if (crtc->scrn->pScreen)
			xf86_reload_cursors(crtc->scrn->pScreen);
		ioctl(fbmode->fd, FBIOGET_VSCREENINFO, &fbmode->mode_info);
	}
}

static void
MSMCrtcCommit(xf86CrtcPtr crtc)
{
}

static void *
MSMCrtcShadowAllocate(xf86CrtcPtr crtc, int width, int height)
{
	fbmode_ptr fbmode = crtc->driver_private;
	/* (pMsm->fixed_info.line_length * pMsm->mode_info.yres) is the size of
    original framebuufer. As buffer is already preallocated by kernel, so just
    return the memory address after the end of original framebuffer as the
    starting address of the shadow framebuffer.*/
	memset((char*)(fbmode->fbmem + fbmode->mode_info.yres *
			fbmode->fixed_info.line_length), 0, fbmode->mode_info.yres *
			fbmode->fixed_info.line_length );
	return (fbmode->fbmem + fbmode->mode_info.yres * fbmode->fixed_info.line_length);
}

static PixmapPtr
MSMCrtcShadowCreate(xf86CrtcPtr crtc, void *data, int width, int height)
{
	fbmode_ptr fbmode = crtc->driver_private;
	ScrnInfoPtr pScrn = crtc->scrn;
	PixmapPtr pNewPixmap = NULL;
	if (!data)
		data = MSMCrtcShadowAllocate(crtc, width, height);
	/*The pitch, width and size of the rotated pixmap has to be the same as
    those of the display framebuffer*/
	pNewPixmap = GetScratchPixmapHeader(pScrn->pScreen,
			fbmode->mode_info.xres, fbmode->mode_info.yres,
			pScrn->depth, pScrn->bitsPerPixel,
			fbmode->fixed_info.line_length, data);
	if (!pNewPixmap)
		xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
				"Unable to allocate shadow pixmap for rotation\n");
	fbmode->rotatedPixmap = pNewPixmap;
	return pNewPixmap;
}

static void
MSMCrtcShadowDestroy(xf86CrtcPtr crtc, PixmapPtr pPixmap, void *data)
{
	fbmode_ptr fbmode = crtc->driver_private;
	fbmode->rotatedPixmap = NULL;
	if (pPixmap)
		FreeScratchPixmapHeader(pPixmap);
}

#ifdef MSMFB_CURSOR
static void
_init_cursor(fbmode_ptr fbmode, struct fb_cursor *cursor)
{
	memset(cursor, 0, sizeof(*cursor));

	/* This is a workaround for a buggy kernel */

	cursor->image.width = MSM_CURSOR_WIDTH;
	cursor->image.height = MSM_CURSOR_HEIGHT;
	cursor->image.depth = 32;

	cursor->enable = fbmode->HWCursorState;
}
#endif

void
_enable_cursor(fbmode_ptr fbmode, Bool enable)
{
#ifdef MSMFB_CURSOR
	struct fb_cursor cursor;

	_init_cursor(fbmode, &cursor);

	fbmode->HWCursorState = cursor.enable = (enable == TRUE) ? 1 : 0;

	if (ioctl(fbmode->fd, MSMFB_CURSOR, &cursor))
		ErrorF("%s: Error calling MSMBF_CURSOR\n", __FUNCTION__);
#endif
}

Bool
fbmode_cursor_init(ScreenPtr pScreen)
{
#ifdef MSMFB_CURSOR
	return xf86_cursors_init(pScreen, MSM_CURSOR_WIDTH, MSM_CURSOR_HEIGHT,
			HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
			HARDWARE_CURSOR_INVERT_MASK |
			HARDWARE_CURSOR_AND_SOURCE_WITH_MASK |
			HARDWARE_CURSOR_SOURCE_MASK_INTERLEAVE_32 |
			HARDWARE_CURSOR_ARGB);
#else
	return FALSE;
#endif
}

static void
MSMCrtcSetCursorPosition(xf86CrtcPtr crtc, int x, int y)
{
#ifdef MSMFB_CURSOR
	fbmode_ptr fbmode = crtc->driver_private;
	struct fb_cursor cursor;

	_init_cursor(fbmode, &cursor);

	if (x < 0)
		x = 0;

	if (y < 0)
		y = 0;

	cursor.set |= FB_CUR_SETPOS;
	cursor.image.dx = x;
	cursor.image.dy = y;

	if (ioctl(fbmode->fd, MSMFB_CURSOR, &cursor))
		ErrorF("%s: Error calling MSMBF_CURSOR\n", __FUNCTION__);
#endif
}

static void
MSMCrtcShowCursor(xf86CrtcPtr crtc)
{
	_enable_cursor(crtc->driver_private, TRUE);
}

static void
MSMCrtcHideCursor(xf86CrtcPtr crtc)
{
	_enable_cursor(crtc->driver_private, FALSE);
}

static Bool
MSMCrtcLoadCursorARGBCheck(xf86CrtcPtr crtc, CARD32 * image)
{
#ifdef MSMFB_CURSOR
	fbmode_ptr fbmode = crtc->driver_private;
	struct fb_cursor cursor;

	_init_cursor(fbmode, &cursor);

	cursor.set |= FB_CUR_SETIMAGE;
	cursor.image.data = (char *)image;

	/* BLEND_TRANSP_EN off */
	cursor.image.bg_color = 0xFFFFFFFF;

	/* Per pixel alpha on */
	cursor.image.fg_color = 0;

	if (ioctl(fbmode->fd, MSMFB_CURSOR, &cursor)) {
		ErrorF("%s: Error calling MSMBF_CURSOR\n", __FUNCTION__);
		return FALSE;
	}
	return TRUE;
#else
	return FALSE;
#endif
}

static void
MSMCrtcLoadCursorARGB(xf86CrtcPtr crtc, CARD32 * image)
{
	MSMCrtcLoadCursorARGBCheck(crtc, image);
}


static const xf86CrtcFuncsRec MSMCrtcFuncs = {
		.dpms = MSMCrtcDPMS,
		.lock = MSMCrtcLock,
		.unlock = MSMCrtcUnlock,
		.mode_fixup = MSMCrtcModeFixup,
		.prepare = MSMCrtcPrepare,
		.mode_set = MSMCrtcModeSet,
		.commit = MSMCrtcCommit,
		.shadow_create = MSMCrtcShadowCreate,
		.shadow_allocate = MSMCrtcShadowAllocate,
		.shadow_destroy = MSMCrtcShadowDestroy,
		.set_cursor_position = MSMCrtcSetCursorPosition,
		.show_cursor = MSMCrtcShowCursor,
		.hide_cursor = MSMCrtcHideCursor,
#if XORG_VERSION_CURRENT >= XORG_VERSION_NUMERIC(1,15,99,902,0)
		.load_cursor_argb_check = MSMCrtcLoadCursorARGBCheck,
#endif
		.load_cursor_argb = MSMCrtcLoadCursorARGB,
		.gamma_set = MSMCrtcGammaSet,
		.destroy = NULL, /* XXX */
};

static void
fbmode_crtc_init(ScrnInfoPtr pScrn, fbmode_ptr fbmode)
{
	xf86CrtcPtr crtc = xf86CrtcCreate(pScrn, &MSMCrtcFuncs);
	crtc->driver_private = fbmode;
}


static void
MSMOutputCreateResources(xf86OutputPtr output)
{
	/* No custom properties are supported */
}

static Bool
MSMOutputSetProperty(xf86OutputPtr output, Atom property,
		RRPropertyValuePtr value)
{
	/* No custom properties are supported */
	return TRUE;
}

static void
MSMOutputDPMS(xf86OutputPtr output, int mode)
{
	/* DPMS is handled at the CRTC */
}

static void
MSMOutputPrepare(xf86OutputPtr output)
{
}

static void
MSMOutputCommit(xf86OutputPtr output)
{
}

static void
MSMOutputSave(xf86OutputPtr output)
{
}

static void
MSMOutputRestore(xf86OutputPtr output)
{
}

static int
MSMOutputModeValid(xf86OutputPtr output, DisplayModePtr pMode)
{
	return MODE_OK;
}

static Bool
MSMOutputModeFixup(xf86OutputPtr output, DisplayModePtr mode,
		DisplayModePtr adjmode)
{
	return TRUE;
}

static void
MSMOutputModeSet(xf86OutputPtr output, DisplayModePtr mode,
		DisplayModePtr adjmode)
{
	/* Nothing to do on the output side */
}

static xf86OutputStatus
MSMOutputDetect(xf86OutputPtr output)
{
	return XF86OutputStatusConnected;
}

static DisplayModePtr
MSMOutputGetModes(xf86OutputPtr output)
{
	fbmode_ptr fbmode = output->driver_private;
	MSMGetDefaultMode(fbmode);
	return xf86DuplicateMode(&fbmode->default_mode);
}

static void
MSMOutputDestroy(xf86OutputPtr output)
{
}

static const xf86OutputFuncsRec MSMOutputFuncs = {
		.create_resources = MSMOutputCreateResources,
		.dpms = MSMOutputDPMS,
		.save = MSMOutputSave,
		.restore = MSMOutputRestore,
		.mode_valid = MSMOutputModeValid,
		.mode_fixup = MSMOutputModeFixup,
		.prepare = MSMOutputPrepare,
		.mode_set = MSMOutputModeSet,
		.commit = MSMOutputCommit,
		.detect = MSMOutputDetect,
		.get_modes = MSMOutputGetModes,
		.set_property = MSMOutputSetProperty,
		.destroy = MSMOutputDestroy
};

static void
fbmode_output_init(ScrnInfoPtr pScrn, fbmode_ptr fbmode)
{
	xf86CrtcConfigPtr xf86_config = XF86_CRTC_CONFIG_PTR(pScrn);
	xf86OutputPtr output;

	/*For primary display*/
	output = xf86OutputCreate(pScrn, &MSMOutputFuncs, "default");

	output->interlaceAllowed = FALSE;
	output->doubleScanAllowed = FALSE;

	/* FIXME: Set monitor size here? */
	output->possible_crtcs = 1;
	output->driver_private = fbmode;
	output->crtc = xf86_config->crtc[0];
}

/* Get the current mode from the framebuffer mode and
 * convert it into xfree86 timings
 */
static void
MSMGetDefaultMode(fbmode_ptr fbmode)
{
	char name[32];
	sprintf(name, "%dx%d", fbmode->mode_info.xres, fbmode->mode_info.yres);

	fbmode->default_mode.name = strdup(name);

	if (fbmode->default_mode.name == NULL)
		fbmode->default_mode.name = "";

	fbmode->default_mode.next = &fbmode->default_mode;
	fbmode->default_mode.prev = &fbmode->default_mode;
	fbmode->default_mode.type |= M_T_BUILTIN | M_T_PREFERRED;

	fbmode->default_mode.HDisplay = fbmode->mode_info.xres;
	fbmode->default_mode.HSyncStart =
			fbmode->default_mode.HDisplay + fbmode->mode_info.right_margin;
	fbmode->default_mode.HSyncEnd =
			fbmode->default_mode.HSyncStart + fbmode->mode_info.hsync_len;
	fbmode->default_mode.HTotal =
			fbmode->default_mode.HSyncEnd + fbmode->mode_info.left_margin;

	fbmode->default_mode.VDisplay = fbmode->mode_info.yres;
	fbmode->default_mode.VSyncStart =
			fbmode->default_mode.VDisplay + fbmode->mode_info.lower_margin;
	fbmode->default_mode.VSyncEnd =
			fbmode->default_mode.VSyncStart + fbmode->mode_info.vsync_len;
	fbmode->default_mode.VTotal =
			fbmode->default_mode.VSyncEnd + fbmode->mode_info.upper_margin;

	/* The clock number we get is not the actual pixclock for the display,
	 * which automagically updates at a fixed rate.  There is no good way
	 * to automatically figure out the fixed rate, so we use a config
	 * value */

	fbmode->default_mode.Clock = (fbmode->defaultVsync *
			fbmode->default_mode.HTotal *
			fbmode->default_mode.VTotal) / 1000;

	fbmode->default_mode.CrtcHDisplay = fbmode->default_mode.HDisplay;
	fbmode->default_mode.CrtcHSyncStart = fbmode->default_mode.HSyncStart;
	fbmode->default_mode.CrtcHSyncEnd = fbmode->default_mode.HSyncEnd;
	fbmode->default_mode.CrtcHTotal = fbmode->default_mode.HTotal;

	fbmode->default_mode.CrtcVDisplay = fbmode->default_mode.VDisplay;
	fbmode->default_mode.CrtcVSyncStart = fbmode->default_mode.VSyncStart;
	fbmode->default_mode.CrtcVSyncEnd = fbmode->default_mode.VSyncEnd;
	fbmode->default_mode.CrtcVTotal = fbmode->default_mode.VTotal;

	fbmode->default_mode.CrtcHAdjusted = FALSE;
	fbmode->default_mode.CrtcVAdjusted = FALSE;
}

static Bool
MSMCrtcResize(ScrnInfoPtr pScrn, int width, int height)
{
	MSMPtr pMsm = MSMPTR(pScrn);
	int	oldx = pScrn->virtualX;
	int	oldy = pScrn->virtualY;
	ScreenPtr   screen = screenInfo.screens[pScrn->scrnIndex];
	PixmapPtr ppix;

	if (oldx == width && oldy == height)
		return TRUE;

	pScrn->virtualX = width;
	pScrn->virtualY = height;
	pScrn->displayWidth = width;

	ppix = screen->GetScreenPixmap(screen);
	if (ppix) {
		msm_set_pixmap_bo(ppix, pMsm->scanout);
		screen->ModifyPixmapHeader(ppix, width, height,
				pScrn->depth, pScrn->bitsPerPixel,
				pScrn->displayWidth * (pScrn->bitsPerPixel / 8),
				NULL);
#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 9
		pScrn->pixmapPrivate.ptr = ppix->devPrivate.ptr;
#endif
	}

	return TRUE;
}

static const xf86CrtcConfigFuncsRec MSMCrtcConfigFuncs = {
		MSMCrtcResize,
};

Bool fbmode_pre_init(ScrnInfoPtr pScrn)
{
	MSMPtr pMsm = MSMPTR(pScrn);

	fbmode_ptr fbmode;
	EntityInfoPtr pEnt;
	const char *dev;
	int mdpver, panelid;
	int depth, fbbpp;

	fbmode = calloc(1, sizeof(*fbmode));

	pEnt = xf86GetEntityInfo(pScrn->entityList[0]);

	/* Open the FB device specified by the user */
	dev = xf86FindOptionValue(pEnt->device->options, "fb");

	fbmode->fd = open(dev, O_RDWR, 0);

	if (fbmode->fd < 0) {
		ERROR_MSG("Opening '%s' failed: %s", dev, strerror(errno));
		free(fbmode);
		return FALSE;
	}
	/* Unblank the screen if it was previously blanked */
	ioctl(fbmode->fd, FBIOBLANK, FB_BLANK_UNBLANK);

	/* Make sure the software refresher is on */
	if(pMsm->SWRefresher){
		ioctl(fbmode->fd, MSMFB_RESUME_SW_REFRESHER, 0);
	}

	/* Get the fixed info (par) structure */

	if (ioctl(fbmode->fd, FBIOGET_FSCREENINFO, &fbmode->fixed_info)) {
		ERROR_MSG("Unable to read hardware info from %s: %s",
				dev, strerror(errno));
		free(fbmode);
		return FALSE;
	}

	/* Parse the ID and figure out what version of the MDP and what
	 * panel ID we have - default to the MDP3 */

	fbmode->chipID = MSM_MDP_VERSION_31;

	if (sscanf(fbmode->fixed_info.id, "msmfb%d_%x", &mdpver, &panelid) < 2) {
		WARNING_MSG("Unable to determine the MDP version - assume 3.1");
	}
	else {
		switch (mdpver) {
		case 22:
			fbmode->chipID = MSM_MDP_VERSION_22;
			break;
		case 31:
			fbmode->chipID = MSM_MDP_VERSION_31;
			break;
		case 40:
			fbmode->chipID = MSM_MDP_VERSION_40;
			break;
		default:
			WARNING_MSG("Unable to determine the MDP version - assume 3.1");
			break;
		}
	}

	/* FIXME:  If we want to parse the panel type, it happens here */

	/* Setup memory */

	/* FIXME:  This is where we will be in close communication with
	 * the fbdev driver to allocate memory.   In the mean time, we
	 * just reuse the framebuffer memory */

	pScrn->videoRam = fbmode->fixed_info.smem_len;

	/* Get the current screen setting */
	if (ioctl(fbmode->fd, FBIOGET_VSCREENINFO, &fbmode->mode_info)) {
		ERROR_MSG("Unable to read the current mode from %s: %s",
				dev, strerror(errno));

		free(fbmode);
		return FALSE;
	}

	/* msm-fb is made of fail.. need to pan otherwise backlight
	 * driver doesn't get kicked and we end up with backlight off.
	 * Makes perfect sense.
	 */
	fbmode->mode_info.yoffset = 1;
	if (ioctl(fbmode->fd, FBIOPAN_DISPLAY, &fbmode->mode_info)) {
		ERROR_MSG("could not pan on %s: %s", dev, strerror(errno));
	}
	/* we have to do this twice because if we were previously
	 * panned to offset 1, then the first FBIOPAN_DISPLAY wouldn't
	 * do anything.
	 */
	fbmode->mode_info.yoffset = 0;
	if (ioctl(fbmode->fd, FBIOPAN_DISPLAY, &fbmode->mode_info)) {
		ERROR_MSG("could not pan on %s: %s", dev, strerror(errno));
	}

	switch(fbmode->mode_info.bits_per_pixel) {
	case 16:
		depth = 16;
		fbbpp = 16;
		break;
	case 24:
	case 32:
		depth = 24;
		fbbpp = 32;
		break;
	default:
		ERROR_MSG("The driver can only support 16bpp and 24bpp output");
		free(fbmode);
		return FALSE;
	}

	if (!xf86SetDepthBpp(pScrn, depth, 0, fbbpp,
			Support24bppFb | Support32bppFb |
			SupportConvert32to24 | SupportConvert24to32)) {
		ERROR_MSG("Unable to set bitdepth");
		free(fbmode);
		return FALSE;
	}

	/* Set the color information in the mode structure to be set when the
      screen initializes.  This might seem like a redundant step, but
      at least on the 8650A, the default color setting is RGBA, not ARGB,
      so setting the color information here insures that the framebuffer
      mode is what we expect */

	switch(pScrn->depth) {
	case 16:
		fbmode->mode_info.bits_per_pixel = 16;
		fbmode->mode_info.red.offset = 11;
		fbmode->mode_info.green.offset = 5;
		fbmode->mode_info.blue.offset = 0;
		fbmode->mode_info.red.length = 5;
		fbmode->mode_info.green.length = 6;
		fbmode->mode_info.blue.length = 5;
		fbmode->mode_info.red.msb_right = 0;
		fbmode->mode_info.green.msb_right = 0;
		fbmode->mode_info.blue.msb_right = 0;
		fbmode->mode_info.transp.offset = 0;
		fbmode->mode_info.transp.length = 0;
		break;
	case 24:
	case 32:
		fbmode->mode_info.bits_per_pixel = 32;
		fbmode->mode_info.red.offset = 16;
		fbmode->mode_info.green.offset = 8;
		fbmode->mode_info.blue.offset = 0;
		fbmode->mode_info.blue.length = 8;
		fbmode->mode_info.green.length = 8;
		fbmode->mode_info.red.length = 8;
		fbmode->mode_info.blue.msb_right = 0;
		fbmode->mode_info.green.msb_right = 0;
		fbmode->mode_info.red.msb_right = 0;
		fbmode->mode_info.transp.offset = 24;
		fbmode->mode_info.transp.length = 8;
		break;
	default:
		ERROR_MSG("The driver can only support 16bpp and 24bpp output");
		free(fbmode);
		return FALSE;
	}

	/* DefaultVsync - default 60 */
	fbmode->defaultVsync =
		xf86CheckIntOption(pEnt->device->options, "DefaultVsync", 60);

	xf86CrtcConfigInit(pScrn, &MSMCrtcConfigFuncs);
	fbmode_crtc_init(pScrn, fbmode);

	xf86CrtcSetSizeRange(pScrn,200,200,2048,2048);

	/* Setup the output */
	fbmode_output_init(pScrn, fbmode);

	if (!xf86InitialConfiguration(pScrn, FALSE)) {
		ERROR_MSG("configuration failed");
		free(fbmode);
		return FALSE;
	}

	/* Set up the virtual size */

	pScrn->virtualX = pScrn->display->virtualX > fbmode->mode_info.xres ?
			pScrn->display->virtualX : fbmode->mode_info.xres;

	pScrn->virtualY = pScrn->display->virtualY > fbmode->mode_info.yres ?
			pScrn->display->virtualY : fbmode->mode_info.yres;

	if (pScrn->virtualX > fbmode->mode_info.xres_virtual)
		pScrn->virtualX = fbmode->mode_info.xres_virtual;

	if (pScrn->virtualY > fbmode->mode_info.yres_virtual)
		pScrn->virtualY = fbmode->mode_info.yres_virtual;

	/* displayWidth is the width of the line in pixels */

	/* The framebuffer driver should always report the line length,
	 * but in case it doesn't, we can calculate it ourselves */

	if (fbmode->fixed_info.line_length) {
		pScrn->displayWidth = fbmode->fixed_info.line_length;
	} else {
		pScrn->displayWidth = fbmode->mode_info.xres_virtual *
				fbmode->mode_info.bits_per_pixel / 8;
	}

	pScrn->displayWidth /= (pScrn->bitsPerPixel / 8);

	/* Set up the view port */
	pScrn->frameX0 = 0;
	pScrn->frameY0 = 0;
	pScrn->frameX1 = fbmode->mode_info.xres;
	pScrn->frameY1 = fbmode->mode_info.yres;

	MSMGetDefaultMode(fbmode);

	/* Make a copy of the mode - this is important, because some
	 * where in the RandR setup, these modes get deleted */

	pScrn->modes = xf86DuplicateMode(&fbmode->default_mode);
	pScrn->currentMode = pScrn->modes;

	/* Set up the colors - this is from fbdevhw, which implies
	 * that it is important for TrueColor and DirectColor modes
	 */
	pScrn->offset.red = fbmode->mode_info.red.offset;
	pScrn->offset.green = fbmode->mode_info.green.offset;
	pScrn->offset.blue = fbmode->mode_info.blue.offset;

	pScrn->mask.red = ((1 << fbmode->mode_info.red.length) - 1)
			 << fbmode->mode_info.red.offset;

	pScrn->mask.green = ((1 << fbmode->mode_info.green.length) - 1)
			 << fbmode->mode_info.green.offset;

	pScrn->mask.blue = ((1 << fbmode->mode_info.blue.length) - 1)
			 << fbmode->mode_info.blue.offset;

	return TRUE;
}

Bool
fbmode_screen_init(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	fbmode_ptr fbmode = fbmode_from_scrn(pScrn);

#if 0
#if defined (MSMFB_GET_PAGE_PROTECTION) && defined (MSMFB_SET_PAGE_PROTECTION)
	/* If the frame buffer can be cached, do so.                                      */
	/* CAUTION: This needs to be done *BEFORE* the mmap() call, or it has no effect.  */
	/* FIXME:  The current page protection should ideally be saved here and restored  */
	/*         when exiting the driver, but there may be little point in doing this   */
	/*         since the XServer typically won't exit for most applications.          */
	{
		const int desired_fb_page_protection = fbmode->FBCache;
		struct mdp_page_protection fb_page_protection;

		// If the kernel supports the FB Caching settings infrastructure,
		// then set the frame buffer cache settings.
		// Otherwise, issue a warning and continue.
		if (ioctl(fbmode->fd, MSMFB_GET_PAGE_PROTECTION, &fb_page_protection)) {
			xf86DrvMsg(scrnIndex, X_WARNING,
					"MSMFB_GET_PAGE_PROTECTION IOCTL: Unable to get current FB cache settings.\n");
		}
		else {
			if (fb_page_protection.page_protection != desired_fb_page_protection) {
				fb_page_protection.page_protection = desired_fb_page_protection;
				if (ioctl(fbmode->fd, MSMFB_SET_PAGE_PROTECTION, &fb_page_protection)) {
					xf86DrvMsg(scrnIndex, X_WARNING,
							"MSMFB_SET_PAGE_PROTECTION IOCTL: Unable to set requested FB cache settings: %s.\n",
							fbCacheStrings[desired_fb_page_protection]);
				}
			}
		}
	}
#endif // defined (MSMFB_GET_PAGE_PROTECTION) && defined (MSMFB_SET_PAGE_PROTECTION)
#endif

	/* Map the framebuffer memory */
	fbmode->fbmem = mmap(NULL, fbmode->fixed_info.smem_len,
			PROT_READ | PROT_WRITE, MAP_SHARED, fbmode->fd, 0);

	/* If we can't map the memory, then this is a short trip */

	if (fbmode->fbmem == MAP_FAILED) {
		ERROR_MSG("Unable to map the framebuffer memory: %s", strerror(errno));
		return FALSE;
	}

	/* Make a buffer object for the framebuffer so that the GPU MMU
	 * can use it
	 */
	pMsm->scanout = fd_bo_from_fbdev(pMsm->pipe, fbmode->fd, fbmode->fixed_info.smem_len);

	return TRUE;
}

void
fbmode_screen_fini(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	fbmode_ptr fbmode = fbmode_from_scrn(pScrn);
	munmap(fbmode->fbmem, fbmode->fixed_info.smem_len);
	fd_bo_del(pMsm->scanout);
	pMsm->scanout = NULL;
}
