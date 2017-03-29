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
# include "config.h"
#endif

#include "xf86.h"
#include "xf86PciInfo.h"
#include <xf86i2c.h>
#include <xf86Crtc.h>
#include "fbdevhw.h"

#include "xgi.h"
#include "xgi_regs.h"

struct xg47_crtc_private {
    I2CBusPtr    pI2C;
};


static int xg47_output_mode_valid(xf86OutputPtr output, DisplayModePtr mode);

static Bool xg47_output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
    DisplayModePtr adjusted_mode);

static void xg47_output_prepare(xf86OutputPtr output);

static void xg47_output_commit(xf86OutputPtr output);

static void xg47_output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
    DisplayModePtr adjusted_mode);

static void xg47_vga_dpms(xf86OutputPtr output, int mode);
static void xg47_dvi_dpms(xf86OutputPtr output, int mode);

static xf86OutputStatus xg47_vga_detect(xf86OutputPtr output);
static xf86OutputStatus xg47_dvi_detect(xf86OutputPtr output);

static void xg47_output_save(xf86OutputPtr output);

static void xg47_output_restore(xf86OutputPtr output);

static void xg47_output_destroy(xf86OutputPtr output);

static DisplayModePtr xg47_output_get_modes(xf86OutputPtr output);

static xf86OutputPtr xg47_OutputDac1Init(ScrnInfoPtr scrn, Bool primary);

static xf86OutputPtr xg47_OutputDac2Init(ScrnInfoPtr scrn);

static const xf86OutputFuncsRec xg47_vga_funcs = {
    .dpms = xg47_vga_dpms,
    .save = xg47_output_save,
    .restore = xg47_output_restore,
    .mode_valid = xg47_output_mode_valid,
    .mode_fixup = xg47_output_mode_fixup,
    .prepare = xg47_output_prepare,
    .mode_set = xg47_output_mode_set,
    .commit = xg47_output_commit,
    .detect = xg47_vga_detect,
    .get_modes = xg47_output_get_modes,
    .destroy = xg47_output_destroy
};


static const xf86OutputFuncsRec xg47_dvi_funcs = {
    .dpms = xg47_dvi_dpms,
    .save = xg47_output_save,
    .restore = xg47_output_restore,
    .mode_valid = xg47_output_mode_valid,
    .mode_fixup = xg47_output_mode_fixup,
    .prepare = xg47_output_prepare,
    .mode_set = xg47_output_mode_set,
    .commit = xg47_output_commit,
    .detect = xg47_dvi_detect,
    .get_modes = xg47_output_get_modes,
    .destroy = xg47_output_destroy
};


void
xg47_vga_dpms(xf86OutputPtr output, int mode)
{
    ScrnInfoPtr pScrn = output->scrn;
    XGIPtr  pXGI = XGIPTR(pScrn);


    if (pXGI->isFBDev) {
        fbdevHWDPMSSet(pScrn, mode, 0);
    } else if (pXGI->pVbe) {
        /* I don't know if the bug is in XGI's BIOS or in VBEDPMSSet, but
         * cx must be set to 0 here, or the mode will not be set.
         */
        pXGI->pInt10->cx = 0x0000;
        VBEDPMSSet(pXGI->pVbe, mode);
    } else {
        const uint8_t power_status = (IN3CFB(0x23) & ~0x03) 
            | (mode);
        const uint8_t pm_ctrl = (IN3C5B(0x24) & ~0x01)
            | ((mode == DPMSModeOn) ? 0x01 : 0x00);


        OUT3CFB(0x23, power_status);
        OUT3C5B(0x24, pm_ctrl);
    }
}


void
xg47_dvi_dpms(xf86OutputPtr output, int mode)
{
    ScrnInfoPtr pScrn = output->scrn;
    XGIPtr  pXGI = XGIPTR(pScrn);


    const uint8_t power_status = (IN3CFB(0x26) & ~0x30) 
	| (mode << 4);
        const uint8_t pm_ctrl = (IN3CFB(0x3d) & ~0x01)
            | ((mode == DPMSModeOn) ? 0x01 : 0x00);


    OUT3CFB(0x26, power_status);
    OUT3CFB(0x3d, pm_ctrl);
}


void delay4I2C(XGIPtr pXGI, unsigned times)
{
    unsigned i;

    for (i = 0; i < (0x40 * times); i++) {
	(void) INB(0x3c4);
	(void) INB(0x3c4);
    }
}


/**
 * Wait for vertical retrace on specified CRT
 *
 * \param pXGI  Device information handle
 * \param crt   0 for first CRT, 1 for second CRT
 */
void waitVerticalRetrace(XGIPtr pXGI, int crt)
{
    const unsigned pattern = (crt == 0) ? 0x08 : 0x80;
    unsigned i;

    (void) INB(0x3DA);

    for(i = 0; i < 0xFFFFU; i++) {
        const uint8_t sync = INB(0x3DA);
        if (sync & pattern)
            return;
    }
}


xf86OutputStatus
xg47_vga_detect(xf86OutputPtr output)
{
    XGIPtr pXGI = XGIPTR(output->scrn);
    struct xg47_crtc_private *xg47_output = 
	(struct xg47_crtc_private *) output->driver_private;
    I2CDevPtr i2c_dev;
    Bool attached;


    /* See if there is a display attached that returns an EDID block.
     */
    i2c_dev = xf86I2CFindDev(xg47_output->pI2C, 0xA0);
    if (i2c_dev != NULL) {
	I2CByte data;

	/* If bit 7 of byte 14 is set, then the attached device is digital.
	 * This means that it's not an analog CRT.
	 */
	xf86I2CReadByte(i2c_dev, 14, &data);
	attached = ((data & 0x80) == 0);
    } else {
	uint8_t seq_24;
	uint8_t seq_25;
	uint8_t seq_69;


	/* Enable the DAC.
	 */
	seq_24 = IN3C5B(0x24);
	OUT3C5B(0x24, seq_24 | 0x01);


	/* Wait for the vertical refresh period.
	 */
	waitVerticalRetrace(pXGI, 0);


	/* 3c5.68-69 - Monitor sensing control register for CRT1
	 *    15: Monitor sensing enable
	 * 14:13: reserved
	 *    12: scheme 
	 *        1: apply monitor sensing data during blanking
	 *        0: apply monitor sensing data all the time
	 * 11:10: reserved
	 *   9:0: monitor sensing data
	 *
	 * Registers 6B-6A have the same meaning for CRT2
	 */
	seq_69 = IN3C5B(0x69);
	OUT3C5B(0x69, seq_69 | 0x80);
	

	delay4I2C(pXGI, 300);
	waitVerticalRetrace(pXGI, 0);


	/* 3c5.25 - Monitor sensing register
	 *    7: DAC2 sensing status after anti-jitter filtering.
	 *  6:4: DAC2 red/green/blue sensing status
	 *    3: DAC1 sensing status after anti-jitter filtering.
	 *  2:0: DAC1 red/green/blue sensing status
	 */
	seq_25 = IN3C5B(0x25);
	attached = ((seq_25 & 0x08) != 0);


	/* Restore modified values.
	 */
	OUT3C5B(0x24, seq_24);
	OUT3C5B(0x69, seq_69 & ~0x80);
    }

    return (attached) ? XF86OutputStatusConnected : XF86OutputStatusUnknown;
}


xf86OutputStatus
xg47_dvi_detect(xf86OutputPtr output)
{
    XGIPtr pXGI = XGIPTR(output->scrn);
    struct xg47_crtc_private *xg47_output = 
	(struct xg47_crtc_private *) output->driver_private;
    I2CDevPtr i2c_dev;
    Bool attached = FALSE;


    /* See if there is a display attached that returns an EDID block.
     */
    i2c_dev = xf86I2CFindDev(xg47_output->pI2C, 0xA0);
    if (i2c_dev != NULL) {
	I2CByte data;

	/* If bit 7 of byte 14 is set, then the attached device is digital.
	 * This means that it's not an analog CRT.
	 */
	xf86I2CReadByte(i2c_dev, 14, &data);
	attached = ((data & 0x80) != 0);
    }

    return (attached) ? XF86OutputStatusConnected : XF86OutputStatusUnknown;
}


int
xg47_output_mode_valid(xf86OutputPtr output, DisplayModePtr mode)
{
    return MODE_OK;
}


Bool
xg47_output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
                       DisplayModePtr adjusted_mode)
{
    return TRUE;
}


void
xg47_output_prepare(xf86OutputPtr output)
{
    output->funcs->dpms(output, DPMSModeOff);
}


void
xg47_output_commit(xf86OutputPtr output)
{
    output->funcs->dpms(output, DPMSModeOn);
}


void
xg47_output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                     DisplayModePtr adjusted_mode)
{
}


void
xg47_output_save(xf86OutputPtr output)
{
}


void
xg47_output_restore(xf86OutputPtr output)
{
}


void
xg47_output_destroy(xf86OutputPtr output)
{
    /* No private data yet, so this function doesn't need to do anything.
     */
}


static DisplayModePtr
xg47_output_get_modes(xf86OutputPtr output)
{
    XGIPtr  pXGI = XGIPTR(output->scrn);
    struct xg47_crtc_private *xg47_output = 
	(struct xg47_crtc_private *) output->driver_private;
    xf86MonPtr mon;

    mon = xf86OutputGetEDID(output, xg47_output->pI2C);
    xf86OutputSetEDID(output, mon);

    return xf86OutputGetEDIDModes(output);
}


/**
 */
xf86OutputPtr
xg47_OutputDac1Init(ScrnInfoPtr scrn, Bool primary)
{
    XGIPtr pXGI = XGIPTR(scrn);
    xf86OutputPtr output;
    struct xg47_crtc_private *xg47_output;
    const char *const name = (primary) ? "VGA" : "VGA1";


    output = xf86OutputCreate(scrn, &xg47_vga_funcs, name);
    if (!output) {
        return NULL;
    }

    xg47_output = xnfcalloc(sizeof(*xg47_output), 1);
    xg47_output->pI2C = pXGI->pI2C;

    output->driver_private = xg47_output;
    return output;
}


/**
 */
xf86OutputPtr
xg47_OutputDac2Init(ScrnInfoPtr scrn)
{
    XGIPtr pXGI = XGIPTR(scrn);
    xf86OutputPtr output;
    struct xg47_crtc_private *xg47_output;


    output = xf86OutputCreate(scrn, &xg47_dvi_funcs, "DVI");
    if (!output) {
        return NULL;
    }

    xg47_output = xnfcalloc(sizeof(*xg47_output), 1);
    xg47_output->pI2C = pXGI->pI2C_dvi;

    output->driver_private = xg47_output;
    return output;
}


void
xg47_PreInitOutputs(ScrnInfoPtr scrn)
{
    xf86OutputPtr output;

    output = xg47_OutputDac1Init(scrn, TRUE);
    if (output != NULL) {
        output->possible_crtcs = 0x01;
    }

    output = xg47_OutputDac2Init(scrn);
    if (output != NULL) {
        output->possible_crtcs = 0x01;
    }
}
