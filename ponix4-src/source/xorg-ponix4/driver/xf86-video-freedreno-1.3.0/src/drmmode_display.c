/*
 * Copyright © 2007, 2013 Red Hat, Inc.
 * Copyright © 2008 Maarten Maathuis
 *
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
 *    Dave Airlie <airlied@redhat.com>
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "xorgVersion.h"

#include "msm.h"
#include "xf86Crtc.h"
#include "xf86drmMode.h"
#include "xf86DDC.h"
#include "xf86Cursor.h"
#include "xf86RandR12.h"
#include "X11/Xatom.h"

#include "dixstruct.h"
#include "scrnintstr.h"
#include "fb.h"
#include "xf86cmap.h"
#include "shadowfb.h"

#include "region.h"

#include <X11/extensions/randr.h>

#ifdef HAVE_XEXTPROTO_71
#include <X11/extensions/dpmsconst.h>
#else
#define DPMS_SERVER
#include <X11/extensions/dpms.h>
#endif

#include <sys/ioctl.h>
#include <errno.h>
#include <libudev.h>


static Bool drmmode_xf86crtc_resize(ScrnInfoPtr pScrn, int width, int height);

typedef struct {
	int fd;
	uint32_t fb_id;
	drmModeResPtr mode_res;
	int cpp;
	drmEventContext event_context;
#ifdef HAVE_LIBUDEV
	struct udev_monitor *uevent_monitor;
#endif
} drmmode_rec, *drmmode_ptr;

typedef struct {
	drmmode_ptr drmmode;
	drmModeCrtcPtr mode_crtc;
	struct fd_bo *cursor;
	struct fd_bo *rotate_bo;
	int rotate_pitch;
	PixmapPtr rotate_pixmap;
	uint32_t rotate_fb_id;
	Bool cursor_visible;
} drmmode_crtc_private_rec, *drmmode_crtc_private_ptr;

typedef struct {
	drmModePropertyPtr mode_prop;
	int index; /* Index within the kernel-side property arrays for
		    * this connector. */
	int num_atoms; /* if range prop, num_atoms == 1; if enum prop,
			* num_atoms == num_enums + 1 */
	Atom *atoms;
} drmmode_prop_rec, *drmmode_prop_ptr;

typedef struct {
	drmmode_ptr drmmode;
	int output_id;
	drmModeConnectorPtr mode_output;
	drmModeEncoderPtr mode_encoder;
	drmModePropertyBlobPtr edid_blob;
	int num_props;
	drmmode_prop_ptr props;
} drmmode_output_private_rec, *drmmode_output_private_ptr;

typedef struct {
	drmmode_ptr drmmode;
	unsigned old_fb_id;
	int flip_count;
	void *event_data;
	unsigned int fe_frame;
	unsigned int fe_tv_sec;
	unsigned int fe_tv_usec;
} drmmode_flipdata_rec, *drmmode_flipdata_ptr;

typedef struct {
	drmmode_flipdata_ptr flipdata;
	Bool dispatch_me;
} drmmode_flipevtcarrier_rec, *drmmode_flipevtcarrier_ptr;

static void drmmode_output_dpms(xf86OutputPtr output, int mode);

static drmmode_ptr
drmmode_from_scrn(ScrnInfoPtr scrn)
{
	if (scrn) {
		xf86CrtcConfigPtr conf = XF86_CRTC_CONFIG_PTR(scrn);
		drmmode_crtc_private_ptr crtc = conf->crtc[0]->driver_private;

		return crtc->drmmode;
	}

	return NULL;
}

static PixmapPtr
drmmode_pixmap_wrap(ScreenPtr pScreen, int width, int height, int depth,
		int bpp, int pitch, struct fd_bo *bo, void *data)
{
	MSMPtr pMsm = MSMPTR_FROM_SCREEN(pScreen);
	PixmapPtr ppix;

	if (!pMsm->NoAccel)
		data = NULL;

	ppix = pScreen->CreatePixmap(pScreen, 0, 0, depth, 0);
	if (!ppix)
		return NULL;

	pScreen->ModifyPixmapHeader(ppix, width, height, depth, bpp,
			pitch, data);
	msm_set_pixmap_bo(ppix, bo);

	return ppix;
}

static void
drmmode_ConvertFromKMode(ScrnInfoPtr scrn, drmModeModeInfo *kmode,
		DisplayModePtr	mode)
{
	memset(mode, 0, sizeof(DisplayModeRec));
	mode->status = MODE_OK;

	mode->Clock = kmode->clock;

	mode->HDisplay = kmode->hdisplay;
	mode->HSyncStart = kmode->hsync_start;
	mode->HSyncEnd = kmode->hsync_end;
	mode->HTotal = kmode->htotal;
	mode->HSkew = kmode->hskew;

	mode->VDisplay = kmode->vdisplay;
	mode->VSyncStart = kmode->vsync_start;
	mode->VSyncEnd = kmode->vsync_end;
	mode->VTotal = kmode->vtotal;
	mode->VScan = kmode->vscan;

	mode->Flags = kmode->flags; //& FLAG_BITS;
	mode->name = strdup(kmode->name);

	if (kmode->type & DRM_MODE_TYPE_DRIVER)
		mode->type = M_T_DRIVER;
	if (kmode->type & DRM_MODE_TYPE_PREFERRED)
		mode->type |= M_T_PREFERRED;
	xf86SetModeCrtc (mode, scrn->adjustFlags);
}

static void
drmmode_ConvertToKMode(ScrnInfoPtr scrn, drmModeModeInfo *kmode,
		DisplayModePtr mode)
{
	memset(kmode, 0, sizeof(*kmode));

	kmode->clock = mode->Clock;
	kmode->hdisplay = mode->HDisplay;
	kmode->hsync_start = mode->HSyncStart;
	kmode->hsync_end = mode->HSyncEnd;
	kmode->htotal = mode->HTotal;
	kmode->hskew = mode->HSkew;

	kmode->vdisplay = mode->VDisplay;
	kmode->vsync_start = mode->VSyncStart;
	kmode->vsync_end = mode->VSyncEnd;
	kmode->vtotal = mode->VTotal;
	kmode->vscan = mode->VScan;

	kmode->flags = mode->Flags; //& FLAG_BITS;
	if (mode->name)
		strncpy(kmode->name, mode->name, DRM_DISPLAY_MODE_LEN);
	kmode->name[DRM_DISPLAY_MODE_LEN-1] = 0;

}

static void
drmmode_crtc_dpms(xf86CrtcPtr drmmode_crtc, int mode)
{

}

static void
drmmode_fbcon_copy(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	ExaDriverPtr exa = pMsm->pExa;
	drmmode_ptr drmmode = NULL;
	xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(pScrn);
	drmModeFBPtr fb;
	unsigned w = pScrn->virtualX, h = pScrn->virtualY, bpp = pScrn->bitsPerPixel;
	uint32_t fbcon_id = 0;
	struct fd_bo *fbcon_bo;
	PixmapPtr fbcon_pix, scanout_pix;
	void *ptr;
	int i;

	for (i = 0; i < config->num_crtc; i++) {
		drmmode_crtc_private_ptr crtc = config->crtc[0]->driver_private;
		if (crtc->mode_crtc->buffer_id) {
			fbcon_id = crtc->mode_crtc->buffer_id;
			drmmode = crtc->drmmode;
		}
	}

	if (!fbcon_id)
		goto fallback;

	fb = drmModeGetFB(drmmode->fd, fbcon_id);
	if (!fb) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"Failed to retrieve fbcon fb: id %d\n", fbcon_id);
		goto fallback;
	}

	if (fb->depth != pScrn->depth || fb->width != w || fb->height != h) {
		drmModeFreeFB(fb);
		goto fallback;
	}

	fbcon_bo = fd_bo_from_handle(pMsm->dev, fb->handle,
			fb->height * fb->pitch);
	if (!fbcon_bo) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"Failed to retrieve fbcon buffer: handle=0x%08x\n",
				fb->handle);
		drmModeFreeFB(fb);
		goto fallback;
	}

	fbcon_pix = drmmode_pixmap_wrap(pScreen, fb->width, fb->height,
			fb->depth, fb->bpp, fb->pitch, fbcon_bo, NULL);
	drmModeFreeFB(fb);
	if (!fbcon_pix) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"Failed to create pixmap for fbcon contents\n");
		goto fallback;
	}

	scanout_pix = drmmode_pixmap_wrap(pScreen, w, h, pScrn->depth,
			bpp, pScrn->displayWidth * bpp / 8, pMsm->scanout, NULL);
	if (!scanout_pix) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"Failed to init scanout pixmap for fbcon mirror\n");
		pScreen->DestroyPixmap(fbcon_pix);
		goto fallback;
	}

	/* we could use CopyArea() and get the normal sw fallbacks, but
	 * then we need to do things in terms of drawables and regions.
	 * So meh, we have XA, let's not worry too much about it.
	 */
	if (!exa->PrepareCopy(fbcon_pix, scanout_pix, 0, 0, GXcopy, ~0))
		goto fallback;
	exa->Copy(scanout_pix, 0, 0, 0, 0, w, h);
	exa->DoneCopy(scanout_pix);

	/* shouldn't really be needed, the PrepareAccess below will do this,
	 * but for good measure:
	 */
	MSMFlushAccel(pScreen);

	/* wait for completion before continuing, avoids seeing a momentary
	 * flash of "corruption" on occasion
	 */
	exa->PrepareAccess(scanout_pix, EXA_PREPARE_SRC);
	exa->FinishAccess(scanout_pix, EXA_PREPARE_SRC);

	pScreen->DestroyPixmap(scanout_pix);
	pScreen->DestroyPixmap(fbcon_pix);
	pScreen->canDoBGNoneRoot = TRUE;

	return;

fallback:
	ptr = fd_bo_map(pMsm->scanout);
	if (!ptr)
		return;
	memset(ptr, 0x00, fd_bo_size(pMsm->scanout));
}

static Bool
drmmode_set_mode_major(xf86CrtcPtr crtc, DisplayModePtr mode,
		Rotation rotation, int x, int y)
{
	ScrnInfoPtr pScrn = crtc->scrn;
	MSMPtr pMsm = MSMPTR(pScrn);
	xf86CrtcConfigPtr   xf86_config = XF86_CRTC_CONFIG_PTR(crtc->scrn);
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;
	uint32_t *output_ids;
	int output_count = 0;
	int ret = TRUE;
	int i;
	int fb_id;
	drmModeModeInfo kmode;

	if (drmmode->fb_id == 0) {
		int pitch = MSMAlignedStride(pScrn->virtualX,
				pScrn->bitsPerPixel);
		ret = drmModeAddFB(drmmode->fd, pScrn->virtualX, pScrn->virtualY,
				pScrn->depth, pScrn->bitsPerPixel, pitch,
				fd_bo_handle(pMsm->scanout), &drmmode->fb_id);
		if (ret) {
			xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
					"Error adding FB for scanout: %s\n",
					strerror(-ret));
			return FALSE;
		}
		pScrn->displayWidth = pitch / (pScrn->bitsPerPixel >> 3);
	}

	if (!xf86CrtcRotate(crtc))
		return FALSE;

	output_ids = calloc(sizeof(uint32_t), xf86_config->num_output);
	if (!output_ids)
		return FALSE;

	for (i = 0; i < xf86_config->num_output; i++) {
		xf86OutputPtr output = xf86_config->output[i];
		drmmode_output_private_ptr drmmode_output;

		if (output->crtc != crtc)
			continue;

		drmmode_output = output->driver_private;
		output_ids[output_count] =
				drmmode_output->mode_output->connector_id;
		output_count++;
	}

	drmmode_ConvertToKMode(crtc->scrn, &kmode, mode);

	fb_id = drmmode->fb_id;
	if (drmmode_crtc->rotate_fb_id) {
		fb_id = drmmode_crtc->rotate_fb_id;
		x = 0;
		y = 0;
	}

	ret = drmModeSetCrtc(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id,
			fb_id, x, y, output_ids, output_count, &kmode);
	free(output_ids);

	if (ret) {
		xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
				"failed to set mode: %s\n", strerror(-ret));
		return FALSE;
	}

	/* Work around some xserver stupidity */
	for (i = 0; i < xf86_config->num_output; i++) {
		xf86OutputPtr output = xf86_config->output[i];

		if (output->crtc != crtc)
			continue;

		drmmode_output_dpms(output, DPMSModeOn);
	}

	crtc->funcs->gamma_set(crtc, crtc->gamma_red, crtc->gamma_green,
			crtc->gamma_blue, crtc->gamma_size);

	if (pMsm->HWCursor)
		xf86_reload_cursors(crtc->scrn->pScreen);

	return TRUE;
}

static void
drmmode_set_cursor_position (xf86CrtcPtr crtc, int x, int y)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;

	drmModeMoveCursor(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id, x, y);
}

static void
convert_cursor(CARD32 *dst, CARD32 *src, int dw, int sw)
{
	int i, j;

	for (j = 0;  j < sw; j++) {
		for (i = 0; i < sw; i++) {
			dst[j * dw + i] = src[j * sw + i];
		}
	}
}

static Bool
drmmode_load_cursor_argb_check(xf86CrtcPtr crtc, CARD32 *image)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	struct fd_bo *cursor = drmmode_crtc->cursor;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;
	void *ptr = fd_bo_map(cursor);
	int ret = 0;

	convert_cursor(ptr, image, 64, 64);

	if (drmmode_crtc->cursor_visible) {
		ret = drmModeSetCursor(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id,
				fd_bo_handle(cursor), 64, 64);
	}

	return ret == 0;
}

static void
drmmode_load_cursor_argb(xf86CrtcPtr crtc, CARD32 *image)
{
	drmmode_load_cursor_argb_check(crtc, image);
}

static void
drmmode_hide_cursor (xf86CrtcPtr crtc)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;

	drmModeSetCursor(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id,
			0, 64, 64);
	drmmode_crtc->cursor_visible = FALSE;
}

static void
drmmode_show_cursor (xf86CrtcPtr crtc)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;

	drmModeSetCursor(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id,
			fd_bo_handle(drmmode_crtc->cursor), 64, 64);
	drmmode_crtc->cursor_visible = TRUE;
}

static void *
drmmode_crtc_shadow_allocate(xf86CrtcPtr crtc, int width, int height)
{
	ScrnInfoPtr pScrn = crtc->scrn;
	MSMPtr pMsm = MSMPTR(pScrn);
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;
	int ret, pitch, size;
	void *ptr;

	pitch = MSMAlignedStride(width, 32);
	size = pitch * height;

	drmmode_crtc->rotate_pitch = pitch;
	drmmode_crtc->rotate_bo = fd_bo_new(pMsm->dev, size,
			DRM_FREEDRENO_GEM_TYPE_KMEM);
	if (!drmmode_crtc->rotate_bo) {
		xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
				"Couldn't allocate shadow memory for rotated CRTC\n");
		return NULL;
	}

	ptr = fd_bo_map(drmmode_crtc->rotate_bo);
	if (!ptr) {
		xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
				"Couldn't get virtual address of shadow scanout\n");
		fd_bo_del(drmmode_crtc->rotate_bo);
		drmmode_crtc->rotate_bo = NULL;
		return NULL;
	}

	ret = drmModeAddFB(drmmode->fd, width, height, crtc->scrn->depth,
			crtc->scrn->bitsPerPixel, drmmode_crtc->rotate_pitch,
			fd_bo_handle(drmmode_crtc->rotate_bo),
			&drmmode_crtc->rotate_fb_id);
	if (ret) {
		xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
				"Error adding FB for shadow scanout: %s\n",
				strerror(-ret));
		fd_bo_del(drmmode_crtc->rotate_bo);
		drmmode_crtc->rotate_bo = NULL;
		return NULL;
	}

	return ptr;
}

static PixmapPtr
drmmode_crtc_shadow_create(xf86CrtcPtr crtc, void *data, int width, int height)
{
	ScrnInfoPtr pScrn = crtc->scrn;
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	PixmapPtr rotate_pixmap;

	if (!data)
		data = drmmode_crtc_shadow_allocate (crtc, width, height);

	rotate_pixmap = drmmode_pixmap_wrap(pScrn->pScreen, width, height,
			pScrn->depth, pScrn->bitsPerPixel,
			drmmode_crtc->rotate_pitch,
			drmmode_crtc->rotate_bo, data);

	drmmode_crtc->rotate_pixmap = rotate_pixmap;
	return drmmode_crtc->rotate_pixmap;
}

static void
drmmode_crtc_shadow_destroy(xf86CrtcPtr crtc, PixmapPtr rotate_pixmap, void *data)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;

	if (rotate_pixmap)
		FreeScratchPixmapHeader(rotate_pixmap);

	if (data) {
		drmModeRmFB(drmmode->fd, drmmode_crtc->rotate_fb_id);
		drmmode_crtc->rotate_fb_id = 0;
		fd_bo_del(drmmode_crtc->rotate_bo);
		drmmode_crtc->rotate_bo = NULL;
		drmmode_crtc->rotate_pixmap = NULL;
	}
}

static void
drmmode_gamma_set(xf86CrtcPtr crtc, CARD16 *red, CARD16 *green, CARD16 *blue,
		int size)
{
	drmmode_crtc_private_ptr drmmode_crtc = crtc->driver_private;
	drmmode_ptr drmmode = drmmode_crtc->drmmode;
	int ret;

	ret = drmModeCrtcSetGamma(drmmode->fd, drmmode_crtc->mode_crtc->crtc_id,
			size, red, green, blue);
	if (ret != 0) {
		xf86DrvMsg(crtc->scrn->scrnIndex, X_ERROR,
				"failed to set gamma: %s\n", strerror(-ret));
	}
}

static const xf86CrtcFuncsRec drmmode_crtc_funcs = {
		.dpms = drmmode_crtc_dpms,
		.set_mode_major = drmmode_set_mode_major,
		.set_cursor_position = drmmode_set_cursor_position,
		.show_cursor = drmmode_show_cursor,
		.hide_cursor = drmmode_hide_cursor,
#if XORG_VERSION_CURRENT >= XORG_VERSION_NUMERIC(1,15,99,902,0)
		.load_cursor_argb_check = drmmode_load_cursor_argb_check,
#endif
		.load_cursor_argb = drmmode_load_cursor_argb,
		.shadow_create = drmmode_crtc_shadow_create,
		.shadow_allocate = drmmode_crtc_shadow_allocate,
		.shadow_destroy = drmmode_crtc_shadow_destroy,
		.gamma_set = drmmode_gamma_set,
};


static void
drmmode_crtc_init(ScrnInfoPtr pScrn, drmmode_ptr drmmode, int num)
{
	MSMPtr pMsm = MSMPTR(pScrn);
	xf86CrtcPtr crtc;
	drmmode_crtc_private_ptr drmmode_crtc;

	crtc = xf86CrtcCreate(pScrn, &drmmode_crtc_funcs);
	if (crtc == NULL)
		return;

	drmmode_crtc = xnfcalloc(sizeof(drmmode_crtc_private_rec), 1);
	drmmode_crtc->mode_crtc = drmModeGetCrtc(drmmode->fd,
			drmmode->mode_res->crtcs[num]);
	drmmode_crtc->drmmode = drmmode;

	drmmode_crtc->cursor = fd_bo_new(pMsm->dev, 64*64*4,
			DRM_FREEDRENO_GEM_TYPE_KMEM);
	assert(drmmode_crtc->cursor);

	crtc->driver_private = drmmode_crtc;

	return;
}

static xf86OutputStatus
drmmode_output_detect(xf86OutputPtr output)
{
	/* go to the hw and retrieve a new output struct */
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	xf86OutputStatus status;
	drmModeFreeConnector(drmmode_output->mode_output);

	drmmode_output->mode_output =
			drmModeGetConnector(drmmode->fd, drmmode_output->output_id);

	if (!drmmode_output->mode_output)
		return XF86OutputStatusDisconnected;

	switch (drmmode_output->mode_output->connection) {
	case DRM_MODE_CONNECTED:
		status = XF86OutputStatusConnected;
		break;
	case DRM_MODE_DISCONNECTED:
		status = XF86OutputStatusDisconnected;
		break;
	default:
	case DRM_MODE_UNKNOWNCONNECTION:
		status = XF86OutputStatusUnknown;
		break;
	}
	return status;
}

static Bool
drmmode_output_mode_valid(xf86OutputPtr output, DisplayModePtr mode)
{
	if (mode->type & M_T_DEFAULT)
		/* Default modes are harmful here. */
		return MODE_BAD;

	return MODE_OK;
}

static DisplayModePtr
drmmode_output_get_modes(xf86OutputPtr output)
{
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmModeConnectorPtr koutput = drmmode_output->mode_output;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	int i;
	DisplayModePtr Modes = NULL, Mode;
	drmModePropertyPtr props;
	xf86MonPtr ddc_mon = NULL;

	if (!koutput)
		return NULL;

	/* look for an EDID property */
	for (i = 0; i < koutput->count_props; i++) {
		props = drmModeGetProperty(drmmode->fd, koutput->props[i]);
		if (!props || !(props->flags & DRM_MODE_PROP_BLOB))
			continue;

		if (!strcmp(props->name, "EDID")) {
			if (drmmode_output->edid_blob)
				drmModeFreePropertyBlob(drmmode_output->edid_blob);
			drmmode_output->edid_blob =
					drmModeGetPropertyBlob(drmmode->fd,
							koutput->prop_values[i]);
		}
		drmModeFreeProperty(props);
	}

	if (drmmode_output->edid_blob) {
		ddc_mon = xf86InterpretEDID(output->scrn->scrnIndex,
				drmmode_output->edid_blob->data);
		if (ddc_mon && drmmode_output->edid_blob->length > 128)
			ddc_mon->flags |= MONITOR_EDID_COMPLETE_RAWDATA;
	}
	xf86OutputSetEDID(output, ddc_mon);

	/* modes should already be available */
	for (i = 0; i < koutput->count_modes; i++) {
		Mode = xnfalloc(sizeof(DisplayModeRec));

		drmmode_ConvertFromKMode(output->scrn, &koutput->modes[i],
				Mode);
		Modes = xf86ModesAdd(Modes, Mode);

	}
	return Modes;
}

static void
drmmode_output_destroy(xf86OutputPtr output)
{
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	int i;

	if (drmmode_output->edid_blob)
		drmModeFreePropertyBlob(drmmode_output->edid_blob);
	for (i = 0; i < drmmode_output->num_props; i++) {
		drmModeFreeProperty(drmmode_output->props[i].mode_prop);
		free(drmmode_output->props[i].atoms);
	}
	drmModeFreeConnector(drmmode_output->mode_output);
	free(drmmode_output);
	output->driver_private = NULL;
}

static void
drmmode_output_dpms(xf86OutputPtr output, int mode)
{
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmModeConnectorPtr koutput = drmmode_output->mode_output;
	drmModePropertyPtr props;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	int mode_id = -1, i;

	for (i = 0; i < koutput->count_props; i++) {
		props = drmModeGetProperty(drmmode->fd, koutput->props[i]);
		if (props && (props->flags && DRM_MODE_PROP_ENUM)) {
			if (!strcmp(props->name, "DPMS")) {
				mode_id = koutput->props[i];
				drmModeFreeProperty(props);
				break;
			}
			drmModeFreeProperty(props);
		}
	}

	if (mode_id < 0)
		return;

	drmModeConnectorSetProperty(drmmode->fd, koutput->connector_id,
			mode_id, mode);
}

static Bool
drmmode_property_ignore(drmModePropertyPtr prop)
{
	if (!prop)
		return TRUE;
	/* ignore blob prop */
	if (prop->flags & DRM_MODE_PROP_BLOB)
		return TRUE;
	/* ignore standard property */
	if (!strcmp(prop->name, "EDID") ||
			!strcmp(prop->name, "DPMS"))
		return TRUE;

	return FALSE;
}

static void
drmmode_output_create_resources(xf86OutputPtr output)
{
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmModeConnectorPtr mode_output = drmmode_output->mode_output;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	drmModePropertyPtr drmmode_prop;
	uint32_t value;
	int i, j, err;

	drmmode_output->props = calloc(mode_output->count_props, sizeof(drmmode_prop_rec));
	if (!drmmode_output->props)
		return;

	drmmode_output->num_props = 0;
	for (i = 0, j = 0; i < mode_output->count_props; i++) {
		drmmode_prop = drmModeGetProperty(drmmode->fd, mode_output->props[i]);
		if (drmmode_property_ignore(drmmode_prop)) {
			drmModeFreeProperty(drmmode_prop);
			continue;
		}
		drmmode_output->props[j].mode_prop = drmmode_prop;
		drmmode_output->props[j].index = i;
		drmmode_output->num_props++;
		j++;
	}

	for (i = 0; i < drmmode_output->num_props; i++) {
		drmmode_prop_ptr p = &drmmode_output->props[i];
		drmmode_prop = p->mode_prop;

		value = drmmode_output->mode_output->prop_values[p->index];

		if (drmmode_prop->flags & DRM_MODE_PROP_RANGE) {
			INT32 range[2];

			p->num_atoms = 1;
			p->atoms = calloc(p->num_atoms, sizeof(Atom));
			if (!p->atoms)
				continue;
			p->atoms[0] = MakeAtom(drmmode_prop->name, strlen(drmmode_prop->name), TRUE);
			range[0] = drmmode_prop->values[0];
			range[1] = drmmode_prop->values[1];
			err = RRConfigureOutputProperty(output->randr_output, p->atoms[0],
					FALSE, TRUE,
					drmmode_prop->flags & DRM_MODE_PROP_IMMUTABLE ? TRUE : FALSE,
							2, range);
			if (err != 0) {
				xf86DrvMsg(output->scrn->scrnIndex, X_ERROR,
						"RRConfigureOutputProperty error, %d\n", err);
			}
			err = RRChangeOutputProperty(output->randr_output, p->atoms[0],
					XA_INTEGER, 32, PropModeReplace, 1,
					&value, FALSE, FALSE);
			if (err != 0) {
				xf86DrvMsg(output->scrn->scrnIndex, X_ERROR,
						"RRChangeOutputProperty error, %d\n", err);
			}
		} else if (drmmode_prop->flags & DRM_MODE_PROP_ENUM) {
			p->num_atoms = drmmode_prop->count_enums + 1;
			p->atoms = calloc(p->num_atoms, sizeof(Atom));
			if (!p->atoms)
				continue;
			p->atoms[0] = MakeAtom(drmmode_prop->name, strlen(drmmode_prop->name), TRUE);
			for (j = 1; j <= drmmode_prop->count_enums; j++) {
				struct drm_mode_property_enum *e = &drmmode_prop->enums[j-1];
				p->atoms[j] = MakeAtom(e->name, strlen(e->name), TRUE);
			}
			err = RRConfigureOutputProperty(output->randr_output, p->atoms[0],
					FALSE, FALSE,
					drmmode_prop->flags & DRM_MODE_PROP_IMMUTABLE ? TRUE : FALSE,
							p->num_atoms - 1, (INT32 *)&p->atoms[1]);
			if (err != 0) {
				xf86DrvMsg(output->scrn->scrnIndex, X_ERROR,
						"RRConfigureOutputProperty error, %d\n", err);
			}
			for (j = 0; j < drmmode_prop->count_enums; j++)
				if (drmmode_prop->enums[j].value == value)
					break;
			/* there's always a matching value */
			err = RRChangeOutputProperty(output->randr_output, p->atoms[0],
					XA_ATOM, 32, PropModeReplace, 1, &p->atoms[j+1], FALSE, FALSE);
			if (err != 0) {
				xf86DrvMsg(output->scrn->scrnIndex, X_ERROR,
						"RRChangeOutputProperty error, %d\n", err);
			}
		}
	}
}

static Bool
drmmode_output_set_property(xf86OutputPtr output, Atom property,
		RRPropertyValuePtr value)
{
	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	int i, ret;

	for (i = 0; i < drmmode_output->num_props; i++) {
		drmmode_prop_ptr p = &drmmode_output->props[i];

		if (p->atoms[0] != property)
			continue;

		if (p->mode_prop->flags & DRM_MODE_PROP_RANGE) {
			uint32_t val;

			if (value->type != XA_INTEGER || value->format != 32 ||
					value->size != 1)
				return FALSE;
			val = *(uint32_t *)value->data;

			ret = drmModeConnectorSetProperty(drmmode->fd, drmmode_output->output_id,
					p->mode_prop->prop_id, (uint64_t)val);

			if (ret)
				return FALSE;

			return TRUE;

		} else if (p->mode_prop->flags & DRM_MODE_PROP_ENUM) {
			Atom	atom;
			const char	*name;
			int		j;

			if (value->type != XA_ATOM || value->format != 32 || value->size != 1)
				return FALSE;
			memcpy(&atom, value->data, 4);
			name = NameForAtom(atom);

			/* search for matching name string, then set its value down */
			for (j = 0; j < p->mode_prop->count_enums; j++) {
				if (!strcmp(p->mode_prop->enums[j].name, name)) {
					ret = drmModeConnectorSetProperty(drmmode->fd,
							drmmode_output->output_id,
							p->mode_prop->prop_id,
							p->mode_prop->enums[j].value);

					if (ret)
						return FALSE;

					return TRUE;
				}
			}

			return FALSE;
		}
	}

	return TRUE;
}

static Bool
drmmode_output_get_property(xf86OutputPtr output, Atom property)
{

	drmmode_output_private_ptr drmmode_output = output->driver_private;
	drmmode_ptr drmmode = drmmode_output->drmmode;
	uint32_t value;
	int err, i;

	if (output->scrn->vtSema) {
		drmModeFreeConnector(drmmode_output->mode_output);
		drmmode_output->mode_output =
				drmModeGetConnector(drmmode->fd, drmmode_output->output_id);
	}

	if (!drmmode_output->mode_output)
		return FALSE;

	for (i = 0; i < drmmode_output->num_props; i++) {
		drmmode_prop_ptr p = &drmmode_output->props[i];
		if (p->atoms[0] != property)
			continue;

		value = drmmode_output->mode_output->prop_values[p->index];

		if (p->mode_prop->flags & DRM_MODE_PROP_RANGE) {
			err = RRChangeOutputProperty(output->randr_output,
					property, XA_INTEGER, 32,
					PropModeReplace, 1, &value,
					FALSE, FALSE);

			return !err;
		} else if (p->mode_prop->flags & DRM_MODE_PROP_ENUM) {
			int		j;

			/* search for matching name string, then set its value down */
			for (j = 0; j < p->mode_prop->count_enums; j++) {
				if (p->mode_prop->enums[j].value == value)
					break;
			}

			err = RRChangeOutputProperty(output->randr_output, property,
					XA_ATOM, 32, PropModeReplace, 1,
					&p->atoms[j+1], FALSE, FALSE);

			return !err;
		}
	}

	return FALSE;
}

static const xf86OutputFuncsRec drmmode_output_funcs = {
		.create_resources = drmmode_output_create_resources,
		.dpms = drmmode_output_dpms,
		.detect = drmmode_output_detect,
		.mode_valid = drmmode_output_mode_valid,
		.get_modes = drmmode_output_get_modes,
		.set_property = drmmode_output_set_property,
		.get_property = drmmode_output_get_property,
		.destroy = drmmode_output_destroy
};

static int subpixel_conv_table[7] = {
		0, SubPixelUnknown,
		SubPixelHorizontalRGB,
		SubPixelHorizontalBGR,
		SubPixelVerticalRGB,
		SubPixelVerticalBGR,
		SubPixelNone };

const char *output_names[] = {
		"None",
		"VGA",
		"DVI-I",
		"DVI-D",
		"DVI-A",
		"Composite",
		"SVIDEO",
		"LVDS",
		"CTV",
		"DIN",
		"DP",
		"HDMI",
		"HDMI",
		"TV",
		"eDP",
};
#define NUM_OUTPUT_NAMES (sizeof(output_names) / sizeof(output_names[0]))

static void
drmmode_output_init(ScrnInfoPtr pScrn, drmmode_ptr drmmode, int num)
{
	xf86OutputPtr output;
	drmModeConnectorPtr koutput;
	drmModeEncoderPtr kencoder;
	drmmode_output_private_ptr drmmode_output;
	char name[32];

	koutput = drmModeGetConnector(drmmode->fd,
			drmmode->mode_res->connectors[num]);
	if (!koutput)
		return;

	kencoder = drmModeGetEncoder(drmmode->fd, koutput->encoders[0]);
	if (!kencoder) {
		drmModeFreeConnector(koutput);
		return;
	}

	if (koutput->connector_type >= NUM_OUTPUT_NAMES)
		snprintf(name, 32, "Unknown%d-%d", koutput->connector_type,
				koutput->connector_type_id);
	else
		snprintf(name, 32, "%s-%d",
				output_names[koutput->connector_type],
				koutput->connector_type_id);

	output = xf86OutputCreate (pScrn, &drmmode_output_funcs, name);
	if (!output) {
		drmModeFreeEncoder(kencoder);
		drmModeFreeConnector(koutput);
		return;
	}

	drmmode_output = calloc(sizeof(drmmode_output_private_rec), 1);
	if (!drmmode_output) {
		xf86OutputDestroy(output);
		drmModeFreeConnector(koutput);
		drmModeFreeEncoder(kencoder);
		return;
	}

	drmmode_output->output_id = drmmode->mode_res->connectors[num];
	drmmode_output->mode_output = koutput;
	drmmode_output->mode_encoder = kencoder;
	drmmode_output->drmmode = drmmode;
	output->mm_width = koutput->mmWidth;
	output->mm_height = koutput->mmHeight;

	output->subpixel_order = subpixel_conv_table[koutput->subpixel];
	output->driver_private = drmmode_output;

	output->possible_crtcs = kencoder->possible_crtcs;
	output->possible_clones = kencoder->possible_clones;

	output->interlaceAllowed = TRUE;
	output->doubleScanAllowed = TRUE;
}

static Bool
drmmode_xf86crtc_resize(ScrnInfoPtr pScrn, int width, int height)
{
	xf86CrtcConfigPtr xf86_config = XF86_CRTC_CONFIG_PTR(pScrn);
	ScreenPtr screen = xf86ScrnToScreen(pScrn);
	MSMPtr pMsm = MSMPTR(pScrn);
	drmmode_crtc_private_ptr drmmode_crtc = NULL;
	drmmode_ptr drmmode = NULL;
	uint32_t old_width, old_height, old_pitch, old_fb_id = 0;
	struct fd_bo *old_bo = NULL;
	int ret, i, pitch, size;
	PixmapPtr ppix;
	void *ptr;

	pitch = MSMAlignedStride(width, 32);
	size = pitch * height;

	if (xf86_config->num_crtc) {
		drmmode_crtc = xf86_config->crtc[0]->driver_private;
		drmmode = drmmode_crtc->drmmode;
	}

	DEBUG_MSG("resize called %d %d\n", width, height);

	if ((pScrn->virtualX == width) &&
			(pScrn->virtualY == height) &&
			pMsm->scanout && drmmode->fb_id)
		return TRUE;

	old_width = pScrn->virtualX;
	old_height = pScrn->virtualY;
	old_pitch = pScrn->displayWidth;
	if (drmmode)
		old_fb_id = drmmode->fb_id;
	old_bo = pMsm->scanout;

	pMsm->scanout = fd_bo_new(pMsm->dev, size,
			DRM_FREEDRENO_GEM_TYPE_KMEM);

	if (!pMsm->scanout)
		goto fail;

	pScrn->virtualX = width;
	pScrn->virtualY = height;
	pScrn->displayWidth = pitch / (pScrn->bitsPerPixel >> 3);

	ptr = fd_bo_map(pMsm->scanout);

	if (drmmode) {
		ret = drmModeAddFB(drmmode->fd, width, height, pScrn->depth,
				pScrn->bitsPerPixel, pitch, fd_bo_handle(pMsm->scanout),
				&drmmode->fb_id);
		if (ret)
			goto fail;
	}

	if (!old_fb_id) {
		drmmode_fbcon_copy(screen);
	} else {
		memset(ptr, 0x00, fd_bo_size(pMsm->scanout));
	}

	/* NOTE do everything that could fail before this point,
	 * otherwise you could end up w/ screen pixmap pointing
	 * at the wrong scanout bo
	 */
	ppix = screen->GetScreenPixmap(screen);
	if (ppix) {
		screen->ModifyPixmapHeader(ppix, width, height, -1, -1, pitch, ptr);
		msm_set_pixmap_bo(ppix, pMsm->scanout);
#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 9
		pScrn->pixmapPrivate.ptr = ppix->devPrivate.ptr;
#endif
	}

	for (i = 0; i < xf86_config->num_crtc; i++) {
		xf86CrtcPtr crtc = xf86_config->crtc[i];

		if (!crtc->enabled)
			continue;

		drmmode_set_mode_major(crtc, &crtc->mode,
				crtc->rotation, crtc->x, crtc->y);
	}

	if (old_fb_id)
		drmModeRmFB(drmmode->fd, old_fb_id);
	if (old_bo)
		fd_bo_del(old_bo);

	return TRUE;

 fail:
	pMsm->scanout = old_bo;
	pScrn->virtualX = old_width;
	pScrn->virtualY = old_height;
	pScrn->displayWidth = old_pitch;
	if (drmmode)
		drmmode->fb_id = old_fb_id;

	return FALSE;
}

static const xf86CrtcConfigFuncsRec drmmode_xf86crtc_config_funcs = {
		drmmode_xf86crtc_resize
};

Bool drmmode_pre_init(ScrnInfoPtr pScrn, int fd, int cpp)
{
	drmmode_ptr drmmode;
	int i;

	drmmode = xnfalloc(sizeof *drmmode);
	drmmode->fd = fd;
	drmmode->fb_id = 0;

	xf86CrtcConfigInit(pScrn, &drmmode_xf86crtc_config_funcs);

	drmmode->cpp = cpp;
	drmmode->mode_res = drmModeGetResources(drmmode->fd);
	if (!drmmode->mode_res)
		return FALSE;

	xf86CrtcSetSizeRange(pScrn, 320, 200, drmmode->mode_res->max_width,
			drmmode->mode_res->max_height);

	if (!xf86SetDepthBpp(pScrn, 0, 0, 0,
			Support24bppFb | Support32bppFb |
			SupportConvert32to24 | SupportConvert24to32)) {
		ERROR_MSG("Unable to set bitdepth");
		free(drmmode);
		return FALSE;
	}

	if (!drmmode->mode_res->count_connectors ||
			!drmmode->mode_res->count_crtcs) {
		drmModeFreeResources(drmmode->mode_res);
		free(drmmode);
		goto done;
	}

	for (i = 0; i < drmmode->mode_res->count_crtcs; i++) {
		if (!xf86IsEntityShared(pScrn->entityList[0]) ||
				(pScrn->confScreen->device->screen == i))
			drmmode_crtc_init(pScrn, drmmode, i);
	}

	for (i = 0; i < drmmode->mode_res->count_connectors; i++)
		drmmode_output_init(pScrn, drmmode, i);

done:

	xf86InitialConfiguration(pScrn, TRUE);

	return TRUE;
}

void
drmmode_adjust_frame(ScrnInfoPtr scrn, int x, int y)
{
	xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(scrn);
	xf86OutputPtr output = config->output[config->compat_output];
	xf86CrtcPtr crtc = output->crtc;

	if (!crtc || !crtc->enabled)
		return;

	drmmode_set_mode_major(crtc, &crtc->mode, crtc->rotation, x, y);
}

void
drmmode_remove_fb(ScrnInfoPtr pScrn)
{
	xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(pScrn);
	xf86CrtcPtr crtc = NULL;
	drmmode_crtc_private_ptr drmmode_crtc;
	drmmode_ptr drmmode;

	if (config && config->num_crtc)
		crtc = config->crtc[0];
	if (!crtc)
		return;

	drmmode_crtc = crtc->driver_private;
	drmmode = drmmode_crtc->drmmode;

	if (drmmode->fb_id)
		drmModeRmFB(drmmode->fd, drmmode->fb_id);
	drmmode->fb_id = 0;
}

int
drmmode_cursor_init(ScreenPtr pScreen)
{
	int flags = HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
			HARDWARE_CURSOR_SOURCE_MASK_INTERLEAVE_32 |
			HARDWARE_CURSOR_ARGB;

	return xf86_cursors_init(pScreen, 64, 64, flags);
}

Bool
drmmode_page_flip(DrawablePtr draw, PixmapPtr back, void *priv)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(draw->pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	struct fd_bo *back_bo = msm_get_pixmap_bo(back);
	xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(pScrn);
	drmmode_crtc_private_ptr crtc = config->crtc[0]->driver_private;
	drmmode_ptr mode = crtc->drmmode;
	drmmode_flipdata_ptr flipdata;
	drmmode_flipevtcarrier_ptr flipcarrier;
	unsigned int ref_crtc_hw_id = 0;
	int ret, i, old_fb_id, emitted = 0;

	old_fb_id = mode->fb_id;
	ret = drmModeAddFB(mode->fd, pScrn->virtualX, pScrn->virtualY,
			pScrn->depth, pScrn->bitsPerPixel,
			pScrn->displayWidth * pScrn->bitsPerPixel / 8,
			fd_bo_handle(back_bo), &mode->fb_id);
	if (ret) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"add fb failed: %s\n", strerror(errno));
		return FALSE;
	}

	flipdata = calloc(1, sizeof(drmmode_flipdata_rec));
	if (!flipdata) {
		xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
				"flip queue: data alloc failed.\n");
		goto error_undo;
	}

	flipdata->event_data = priv;
	flipdata->drmmode = mode;

	for (i = 0; i < config->num_crtc; i++) {
		crtc = config->crtc[i]->driver_private;

		if (!config->crtc[i]->enabled)
			continue;

		if (ref_crtc_hw_id == 0)
			ref_crtc_hw_id = (1 << i);

		flipdata->flip_count++;

		flipcarrier = calloc(1, sizeof(drmmode_flipevtcarrier_rec));
		if (!flipcarrier) {
			xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
					"flip queue: carrier alloc failed.\n");
			if (emitted == 0)
				free(flipdata);
			goto error_undo;
		}

		/* Only the reference crtc will finally deliver its page flip
		 * completion event. All other crtc's events will be discarded.
		 */
		flipcarrier->dispatch_me = ((1 << i) == ref_crtc_hw_id);
		flipcarrier->flipdata = flipdata;

		ret = drmModePageFlip(mode->fd, crtc->mode_crtc->crtc_id,
				mode->fb_id, DRM_MODE_PAGE_FLIP_EVENT,
				flipcarrier);
		if (ret) {
			xf86DrvMsg(pScrn->scrnIndex, X_WARNING,
					"flip queue failed: %s\n", strerror(errno));

			free(flipcarrier);
			if (emitted == 0)
				free(flipdata);
			goto error_undo;
		}

		emitted++;
	}

	/* Will release old fb after all crtc's completed flip. */
	flipdata->old_fb_id = old_fb_id;

	pMsm->scanout = back_bo;

	return TRUE;

error_undo:
	drmModeRmFB(mode->fd, mode->fb_id);
	mode->fb_id = old_fb_id;
	return FALSE;
}

#ifdef HAVE_LIBUDEV
static void
drmmode_handle_uevents(ScrnInfoPtr scrn)
{
	drmmode_ptr drmmode = drmmode_from_scrn(scrn);
	struct udev_device *dev;

	dev = udev_monitor_receive_device(drmmode->uevent_monitor);
	if (!dev)
		return;

	RRGetInfo(xf86ScrnToScreen(scrn), TRUE);
	udev_device_unref(dev);
}
#endif

static void
drmmode_uevent_init(ScrnInfoPtr scrn)
{
#ifdef HAVE_LIBUDEV
	drmmode_ptr drmmode = drmmode_from_scrn(scrn);
	struct udev *u;
	struct udev_monitor *mon;

	u = udev_new();
	if (!u)
		return;
	mon = udev_monitor_new_from_netlink(u, "udev");
	if (!mon) {
		udev_unref(u);
		return;
	}

	if (udev_monitor_filter_add_match_subsystem_devtype(mon,
			"drm",
			"drm_minor") < 0 ||
			udev_monitor_enable_receiving(mon) < 0) {
		udev_monitor_unref(mon);
		udev_unref(u);
		return;
	}

	AddGeneralSocket(udev_monitor_get_fd(mon));
	drmmode->uevent_monitor = mon;
#endif
}

static void
drmmode_uevent_fini(ScrnInfoPtr scrn)
{
#ifdef HAVE_LIBUDEV
	drmmode_ptr drmmode = drmmode_from_scrn(scrn);

	if (drmmode->uevent_monitor) {
		struct udev *u = udev_monitor_get_udev(drmmode->uevent_monitor);

		RemoveGeneralSocket(udev_monitor_get_fd(drmmode->uevent_monitor));
		udev_monitor_unref(drmmode->uevent_monitor);
		udev_unref(u);
	}
#endif
}

static void
drmmode_flip_handler(int fd, unsigned int frame, unsigned int tv_sec,
		unsigned int tv_usec, void *event_data)
{
	drmmode_flipevtcarrier_ptr flipcarrier = event_data;
	drmmode_flipdata_ptr flipdata = flipcarrier->flipdata;
	drmmode_ptr drmmode = flipdata->drmmode;

	/* Is this the event whose info shall be delivered to higher level? */
	if (flipcarrier->dispatch_me) {
		/* Yes: Cache msc, ust for later delivery. */
		flipdata->fe_frame = frame;
		flipdata->fe_tv_sec = tv_sec;
		flipdata->fe_tv_usec = tv_usec;
	}
	free(flipcarrier);

	/* Last crtc completed flip? */
	flipdata->flip_count--;
	if (flipdata->flip_count > 0)
		return;

	/* Release framebuffer */
	drmModeRmFB(drmmode->fd, flipdata->old_fb_id);

	if (flipdata->event_data) {
		/* Deliver cached msc, ust from reference crtc to flip event handler */
		MSMDRI2SwapComplete(flipdata->event_data, flipdata->fe_frame,
				flipdata->fe_tv_sec, flipdata->fe_tv_usec);
	}

	free(flipdata);
}

static void
drmmode_wakeup_handler(pointer data, int err, pointer p)
{
	ScrnInfoPtr scrn = data;
	drmmode_ptr drmmode = drmmode_from_scrn(scrn);
	fd_set *read_mask = p;

	if (scrn == NULL || err < 0)
		return;

	if (FD_ISSET(drmmode->fd, read_mask))
		drmHandleEvent(drmmode->fd, &drmmode->event_context);

#ifdef HAVE_LIBUDEV
	if (FD_ISSET(udev_monitor_get_fd(drmmode->uevent_monitor), read_mask))
		drmmode_handle_uevents(scrn);
#endif
}

void
drmmode_wait_for_event(ScrnInfoPtr pScrn)
{
	drmmode_ptr drmmode = drmmode_from_scrn(pScrn);
	drmHandleEvent(drmmode->fd, &drmmode->event_context);
}

Bool
drmmode_screen_init(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	drmmode_ptr drmmode = drmmode_from_scrn(pScrn);
	MSMPtr pMsm = MSMPTR(pScrn);
	int pitch, size;

	/* NOTE: we need an initial scanout buffer, in case no attached
	 * display:
	 */
	pitch = MSMAlignedStride(pScrn->virtualX, pScrn->bitsPerPixel);
	size = pitch * pScrn->virtualY;
	DEBUG_MSG("initial scanout buffer: %dx%d@%d (size=%d, pitch=%d)",
		pScrn->virtualX, pScrn->virtualY, pScrn->bitsPerPixel,
		size, pitch);
	pMsm->scanout = fd_bo_new(pMsm->dev, size,
			DRM_FREEDRENO_GEM_TYPE_KMEM);
	if (!pMsm->scanout) {
		ERROR_MSG("Error allocating scanout buffer");
		return FALSE;
	}

	drmmode_uevent_init(pScrn);

	/* Plug in a pageflip completion event handler */
	drmmode->event_context.version = DRM_EVENT_CONTEXT_VERSION;
	drmmode->event_context.page_flip_handler = drmmode_flip_handler;

	AddGeneralSocket(drmmode->fd);

	/* Register a wakeup handler to get informed on DRM events */
	RegisterBlockAndWakeupHandlers((BlockHandlerProcPtr)NoopDDA,
			drmmode_wakeup_handler, pScrn);

	return TRUE;
}

void
drmmode_screen_fini(ScreenPtr pScreen)
{
	ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
	MSMPtr pMsm = MSMPTR(pScrn);
	drmmode_ptr drmmode = drmmode_from_scrn(pScrn);

	drmmode_uevent_fini(pScrn);

	/* Register a wakeup handler to get informed on DRM events */
	RemoveBlockAndWakeupHandlers((BlockHandlerProcPtr)NoopDDA,
			drmmode_wakeup_handler, pScrn);
	RemoveGeneralSocket(drmmode->fd);

	drmmode_remove_fb(pScrn);
	fd_bo_del(pMsm->scanout);
	pMsm->scanout = NULL;
}
