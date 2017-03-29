/*
 * Copyright (C) 2003-2006 by XGI Technology, Taiwan.
 * (c) Copyright IBM Corporation 2008
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* All drivers should typically include these */
#include "xf86.h"
#include "xf86_OSproc.h"
#include <xf86i2c.h>
#include <xf86Crtc.h>

#include "xgi.h"
#include "xgi_regs.h"
#include "xg47_mode.h"

static void xg47_crtc_destroy(xf86CrtcPtr output);

static void xg47_crtc_dpms(xf86CrtcPtr crtc, int mode);

static Bool xg47_crtc_lock(xf86CrtcPtr crtc);

static void xg47_crtc_unlock(xf86CrtcPtr crtc);

static Bool xg47_crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
    DisplayModePtr adjusted_mode);

static void xg47_crtc_prepare(xf86CrtcPtr crtc);

static void xg47_crtc_commit(xf86CrtcPtr crtc);

static void xg47_crtc_save(xf86CrtcPtr crtc);
static void xg47_crtc_restore(xf86CrtcPtr crtc);
static void xg47_crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
    DisplayModePtr adjusted_mode, int x, int y);

struct xg47_crtc_data {
    struct xgi_regs regs;
};


static const xf86CrtcFuncsRec xg47_crtc_funcs = {
    .dpms = xg47_crtc_dpms,
    .save = xg47_crtc_save,
    .restore = xg47_crtc_restore,
    .lock = xg47_crtc_lock,
    .unlock = xg47_crtc_unlock,
    .mode_fixup = xg47_crtc_mode_fixup,
    .prepare = xg47_crtc_prepare,
    .mode_set = xg47_crtc_mode_set,
    .commit = xg47_crtc_commit,
    .destroy = xg47_crtc_destroy
};


void
xg47_crtc_save(xf86CrtcPtr crtc)
{
    XGIPtr  pXGI = XGIPTR(crtc->scrn);
    struct xg47_crtc_data *data = crtc->driver_private;
    vgaHWPtr pVgaHW = VGAHWPTR(crtc->scrn);

    xg47_mode_save(crtc->scrn, pVgaHW, & data->regs);
}


void
xg47_crtc_restore(xf86CrtcPtr crtc)
{
    XGIPtr  pXGI = XGIPTR(crtc->scrn);
    struct xg47_crtc_data *data = crtc->driver_private;
    vgaHWPtr pVgaHW = VGAHWPTR(crtc->scrn);

    xg47_mode_restore(crtc->scrn, pVgaHW, & data->regs);
}


void
xg47_crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
                   DisplayModePtr adjusted_mode, int x, int y)
{
    XGIPtr  pXGI = XGIPTR(crtc->scrn);
    struct xgi_regs regs;
    vgaHWPtr pVgaHW = VGAHWPTR(crtc->scrn);


    /* Initialise the ModeReg values */
    if (!vgaHWInit(crtc->scrn, mode))
        return;

    crtc->scrn->vtSema = TRUE;

    SetModeCRTC1(pXGI, mode, &regs);
    SetColorDAC(pXGI, crtc->scrn->bitsPerPixel, &regs);

    /* Unprotect registers */
    OUTB(0x3C4, 0x11);
    uint8_t protect = INB(0x3C5);
    OUTB(0x3C5, 0x92);

    xg47_mode_restore(crtc->scrn, &pVgaHW->ModeReg, &regs);

    /* Protect registers */
    OUTB(0x3C4, 0x11);
    OUTB(0x3C5, protect);
    return;
}


void
xg47_crtc_dpms(xf86CrtcPtr crtc, int mode)
{
    (void) crtc;
    (void) mode;
}


Bool
xg47_crtc_lock(xf86CrtcPtr crtc)
{
    (void) crtc;
    return FALSE;
}

void
xg47_crtc_unlock(xf86CrtcPtr crtc)
{
    (void) crtc;
}


Bool
xg47_crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
                     DisplayModePtr adjusted_mode)
{
    (void) crtc;
    (void) mode;
    (void) adjusted_mode;
    return TRUE;
}


void
xg47_crtc_prepare(xf86CrtcPtr crtc)
{
    crtc->funcs->dpms(crtc, DPMSModeOff);
}


void
xg47_crtc_commit(xf86CrtcPtr crtc)
{
    crtc->funcs->dpms(crtc, DPMSModeOn);
}


void
xg47_crtc_destroy(xf86CrtcPtr crtc)
{
    if (crtc->driver_private) {
        free (crtc->driver_private);
    }
}


void
xg47_CrtcInit(ScrnInfoPtr pScrn, unsigned dev_type)
{
    xf86CrtcPtr crtc;
    struct xg47_crtc_data *data;


    data = calloc(sizeof(*data), 1);
    if (data == NULL) {
        return;
    }

    crtc = xf86CrtcCreate(pScrn, & xg47_crtc_funcs);
    if (crtc == NULL) {
        free(data);
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, 
                   "Unable to create CRTC structure.\n");
        return;
    }

    crtc->driver_private = data;
}
