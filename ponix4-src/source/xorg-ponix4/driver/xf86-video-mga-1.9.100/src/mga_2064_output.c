/*
 * Copyright 2007 Tilman Sauerbeck
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software")
 * to deal in the software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * them Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTIBILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *    Tilman Sauerbeck <tilman@code-monkey.de>
 *
 * Sources:
 *    xf86-video-intel
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "xf86.h"
#include "xf86i2c.h"
#include "xf86Crtc.h"

#include "mga_reg.h"
#include "mga.h"

#define MGAOUTPUTDATAPTR(p) ((MgaOutputDataPtr) ((p)->driver_private))

typedef struct {
    I2CBusPtr ddc_bus;
} MgaOutputDataRec, *MgaOutputDataPtr;

static void output_dpms(xf86OutputPtr output, int mode);
static int output_mode_valid(xf86OutputPtr output, DisplayModePtr mode);
static int output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
                             DisplayModePtr adjusted_mode);
static void output_prepare(xf86OutputPtr output);
static void output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                            DisplayModePtr adjusted_mode);
static void output_commit(xf86OutputPtr output);
static xf86OutputStatus output_detect(xf86OutputPtr output);
static DisplayModePtr output_get_modes(xf86OutputPtr output);
static void output_destroy(xf86OutputPtr output);

static const xf86OutputFuncsRec output_funcs = {
    .dpms = output_dpms,
    .mode_valid = output_mode_valid,
    .mode_fixup = output_mode_fixup,
    .prepare = output_prepare,
    .mode_set = output_mode_set,
    .commit = output_commit,
    .detect = output_detect,
    .get_modes = output_get_modes,
    .destroy = output_destroy
};

static void
output_dpms(xf86OutputPtr output, int mode)
{
    /* FIXME
     *
     * Prefer an implementation that doesn't depend on VGA specifics.
     */

    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 seq1, crtcext1;

    OUTREG8(MGAREG_SEQ_INDEX, 0x01); /* Select SEQ1 */
    OUTREG8(MGAREG_CRTCEXT_INDEX, 0x01); /* Select CRTCEXT1 */

    seq1 = INREG8(MGAREG_SEQ_DATA);
    seq1 &= ~MGAREG_SEQ1_SCREEN_OFF;

    crtcext1 = INREG8(MGAREG_CRTCEXT_DATA);
    crtcext1 &= ~MGAREG_CRTCEXT1_HSYNC_OFF;
    crtcext1 &= ~MGAREG_CRTCEXT1_VSYNC_OFF;

    switch (mode) {
    case DPMSModeOn:
        /* nothing to do */
        break;
    case DPMSModeStandby:
        seq1 |= MGAREG_SEQ1_SCREEN_OFF;
        crtcext1 = MGAREG_CRTCEXT1_HSYNC_OFF;
        break;
    case DPMSModeSuspend:
        seq1 |= MGAREG_SEQ1_SCREEN_OFF;
        crtcext1 |= MGAREG_CRTCEXT1_VSYNC_OFF;
        break;
    case DPMSModeOff:
        seq1 |= MGAREG_SEQ1_SCREEN_OFF;
        crtcext1 |= MGAREG_CRTCEXT1_HSYNC_OFF;
        crtcext1 |= MGAREG_CRTCEXT1_VSYNC_OFF;
        break;
    }

    OUTREG8(MGAREG_SEQ_DATA, seq1);
    OUTREG8(MGAREG_CRTCEXT_DATA, crtcext1);
}

static int
output_mode_valid(xf86OutputPtr output, DisplayModePtr mode)
{
    return MODE_OK;
}

static int
output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
                   DisplayModePtr pAdjustedMode)
{
    return TRUE;
}

static void
output_prepare(xf86OutputPtr output)
{
}

static void
output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                DisplayModePtr adjusted_mode)
{
}

static void
output_commit(xf86OutputPtr output)
{
}

static xf86OutputStatus
output_detect(xf86OutputPtr output)
{
    MgaOutputDataPtr data = MGAOUTPUTDATAPTR(output);

    /* 0xa0 is DDC EEPROM address */
    if (xf86I2CProbeAddress(data->ddc_bus, 0xa0))
        return XF86OutputStatusConnected;
    else
        return XF86OutputStatusUnknown;
}

static DisplayModePtr
output_get_modes(xf86OutputPtr output)
{
    MgaOutputDataPtr data = MGAOUTPUTDATAPTR(output);
    xf86MonPtr mon;

    mon = xf86OutputGetEDID(output, data->ddc_bus);
    xf86OutputSetEDID(output, mon);

    return xf86OutputGetEDIDModes(output);
}

static void
output_destroy(xf86OutputPtr output)
{
    if (output->driver_private)
        xfree(output->driver_private);
}

xf86OutputPtr
Mga2064OutputInit(ScrnInfoPtr scrn)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86OutputPtr output;
    MgaOutputDataPtr data;

    data = xnfcalloc(sizeof(MgaOutputDataRec), 1);
    if (!data)
        return NULL;

    output = xf86OutputCreate(scrn, &output_funcs, "VGA");
    if (!output) {
        xfree(data);
        return NULL;
    }

    output->driver_private = data;

    data->ddc_bus = pMga->DDC_Bus1;

    return output;
}
