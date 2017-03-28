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
#include "xf86PciInfo.h"

#include "mga_reg.h"
#include "mga.h"

#define MGAOUTPUTDATAPTR(p) ((MgaOutputDataPtr) ((p)->driver_private))

typedef struct {
    I2CBusPtr ddc_bus;
} MgaOutputDataRec, *MgaOutputDataPtr;

static void output_dac1_dpms(xf86OutputPtr output, int mode);
static void output_dac2_dpms(xf86OutputPtr output, int mode);
static void output_panel1_dpms(xf86OutputPtr output, int mode);
static void output_panel2_dpms(xf86OutputPtr output, int mode);
static void output_save(xf86OutputPtr output);
static void output_restore(xf86OutputPtr output);
static void output_dac2_restore(xf86OutputPtr output);
static void output_panel_restore(xf86OutputPtr output);
static int output_mode_valid(xf86OutputPtr output, DisplayModePtr mode);
static Bool output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
                              DisplayModePtr adjusted_mode);
static void output_prepare(xf86OutputPtr output);
static void output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                            DisplayModePtr adjusted_mode);
static void output_dac2_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                                 DisplayModePtr adjusted_mode);
static void output_panel1_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                                   DisplayModePtr adjusted_mode);
static void output_panel2_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                                   DisplayModePtr adjusted_mode);
static void output_commit(xf86OutputPtr output);
static xf86OutputStatus output_detect(xf86OutputPtr output);
static DisplayModePtr output_get_modes(xf86OutputPtr output);
static void output_destroy(xf86OutputPtr output);

static const xf86OutputFuncsRec output_dac1_funcs = {
    .dpms = output_dac1_dpms,
    .save = output_save,
    .restore = output_restore,
    .mode_valid = output_mode_valid,
    .mode_fixup = output_mode_fixup,
    .prepare = output_prepare,
    .mode_set = output_mode_set,
    .commit = output_commit,
    .detect = output_detect,
    .get_modes = output_get_modes,
    .destroy = output_destroy
};

static const xf86OutputFuncsRec output_dac2_funcs = {
    .dpms = output_dac2_dpms,
    .save = output_save,
    .restore = output_dac2_restore,
    .mode_valid = output_mode_valid,
    .mode_fixup = output_mode_fixup,
    .prepare = output_prepare,
    .mode_set = output_dac2_mode_set,
    .commit = output_commit,
    .detect = output_detect,
    .get_modes = output_get_modes,
    .destroy = output_destroy
};

static const xf86OutputFuncsRec output_panel1_funcs = {
    .dpms = output_panel1_dpms,
    .save = output_save,
    .restore = output_panel_restore,
    .mode_valid = output_mode_valid,
    .mode_fixup = output_mode_fixup,
    .prepare = output_prepare,
    .mode_set = output_panel1_mode_set,
    .commit = output_commit,
    .detect = output_detect,
    .get_modes = output_get_modes,
    .destroy = output_destroy
};

static const xf86OutputFuncsRec output_panel2_funcs = {
    .dpms = output_panel2_dpms,
    .save = output_save,
    .restore = output_panel_restore,
    .mode_valid = output_mode_valid,
    .mode_fixup = output_mode_fixup,
    .prepare = output_prepare,
    .mode_set = output_panel2_mode_set,
    .commit = output_commit,
    .detect = output_detect,
    .get_modes = output_get_modes,
    .destroy = output_destroy
};

static int panel_users = 0;

/* enable/disable primary output. */
static void
output1_dpms(xf86OutputPtr output, int mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 misc_ctl, disp_ctl, mask;

    misc_ctl = inMGAdac(MGA1064_MISC_CTL);
    mask = MGA1064_MISC_CTL_DAC_EN;

    if (mode == DPMSModeOn)
        outMGAdac(MGA1064_MISC_CTL, misc_ctl | mask);
    else
        outMGAdac(MGA1064_MISC_CTL, misc_ctl & ~mask);

    disp_ctl = inMGAdac(MGA1064_DISP_CTL);
    mask = MGA1064_DISP_CTL_DAC1OUTSEL_EN;

    if (mode == DPMSModeOn)
        outMGAdac(MGA1064_DISP_CTL, disp_ctl | mask);
    else
        outMGAdac(MGA1064_DISP_CTL, disp_ctl & ~mask);
}

static void
output_dac1_dpms(xf86OutputPtr output, int mode)
{
    /* XXX
     * . Prefer an implementation that doesn't depend on VGA specifics.
     *
     * . This will only work for the primary output or maybe only for
     *   CRTC1?
     */

    MGAPtr pMga = MGAPTR(output->scrn);

#if 0
    CARD8 val, seq1, crtcext1;

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

    MGAWAITVSYNC();
    MGAWAITBUSY();

    OUTREG8(MGAREG_SEQ_DATA, seq1);

    usleep(20000);

    OUTREG8(MGAREG_CRTCEXT_DATA, crtcext1);
#endif

    output1_dpms(output, mode);
}

static void
output_dac2_dpms(xf86OutputPtr output, int mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 pwr_ctl, mask;

    pwr_ctl = inMGAdac(MGA1064_PWR_CTL);

    mask = MGA1064_PWR_CTL_DAC2_EN;

    /* dropping _RFIFO_EN disables both outputs, so it's probably
     * owned by CRTC1.
     *    MGA1064_PWR_CTL_RFIFO_EN;
     */

    if (mode == DPMSModeOn)
        outMGAdac(MGA1064_PWR_CTL, pwr_ctl | mask);
    else
        outMGAdac(MGA1064_PWR_CTL, pwr_ctl & ~mask);
}

static void
output_panel1_dpms(xf86OutputPtr output, int mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 pwr_ctl, mask;

    pwr_ctl = inMGAdac(MGA1064_PWR_CTL);
    mask = MGA1064_PWR_CTL_PANEL_EN;

    if (mode == DPMSModeOn) {
        panel_users |= 1;
        outMGAdac(MGA1064_PWR_CTL, pwr_ctl | mask);
    } else {
        panel_users &= ~1;

        if (!panel_users)
            outMGAdac(MGA1064_PWR_CTL, pwr_ctl & ~mask);
    }

    output1_dpms(output, mode);
}

static void
output_panel2_dpms(xf86OutputPtr output, int mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 pwr_ctl, mask;

    pwr_ctl = inMGAdac(MGA1064_PWR_CTL);
    mask = MGA1064_PWR_CTL_PANEL_EN;

    if (mode == DPMSModeOn) {
        panel_users |= 2;

        outMGAdac(MGA1064_PWR_CTL, pwr_ctl | mask);
        outMGAdac(MGA1064_DVI_PIPE_CTL, 0x20);
    } else {
        panel_users &= ~2;

        if (!panel_users)
            outMGAdac(MGA1064_PWR_CTL, pwr_ctl & ~mask);

        outMGAdac(MGA1064_DVI_PIPE_CTL, 0x0);
    }
}

static void
output_save(xf86OutputPtr output)
{
}

static void
output_restore(xf86OutputPtr output)
{
}

static void
output_dac2_restore(xf86OutputPtr output)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    CARD8 disp_ctl;

    disp_ctl = inMGAdac(MGA1064_DISP_CTL);
    disp_ctl &= ~MGA1064_DISP_CTL_DAC2OUTSEL_MASK;
    disp_ctl |= MGA1064_DISP_CTL_DAC2OUTSEL_CRTC1;

    outMGAdac(MGA1064_DISP_CTL, disp_ctl);
}

static void
output_panel_restore(xf86OutputPtr output)
{
    MGAPtr pMga = MGAPTR(output->scrn);

    /* Reset DUALDVI register */
    outMGAdac(MGA1064_DVI_PIPE_CTL, 0x0);

    /* Set Panel mode between 20 and 54 MHz */
    outMGAdac(MGA1064_PAN_CTL, 0x7);
}

static int
output_mode_valid(xf86OutputPtr output, DisplayModePtr mode)
{
    return MODE_OK;
}

static Bool
output_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
                   DisplayModePtr adjusted_mode)
{
    return TRUE;
}

static void
output_prepare(xf86OutputPtr output)
{
    output->funcs->dpms(output, DPMSModeOff);
}

static void
output_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                DisplayModePtr adjusted_mode)
{
}

static void
output_dac2_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                     DisplayModePtr adjusted_mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(output->scrn);
    CARD8 disp_ctl;

    disp_ctl = inMGAdac(MGA1064_DISP_CTL);
    disp_ctl &= ~MGA1064_DISP_CTL_DAC2OUTSEL_MASK;

    /* Is this output mapped to CRTC1? */
    if (output->crtc == config->crtc[0])
        disp_ctl |= MGA1064_DISP_CTL_DAC2OUTSEL_CRTC1;
    else
        disp_ctl |= MGA1064_DISP_CTL_DAC2OUTSEL_CRTC2;

    outMGAdac(MGA1064_DISP_CTL, disp_ctl);
}

static void
output_commit(xf86OutputPtr output)
{
    output->funcs->dpms(output, DPMSModeOn);
}

static CARD8
get_pan_ctl_value(xf86OutputPtr output, DisplayModePtr mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);

    /* these tables are based on code from matroxfb,
     * used with permission by Petr Vandrovec.
     */
    static int g450_thresholds[] = {
        45000, 65000, 85000, 105000,
        135000, 160000, 175000, -1
    };
    static int g550_thresholds[] = {
        45000, 55000, 70000, 85000,
        10000, 115000, 125000, -1
    };
    int *ptr = (pMga->Chipset == PCI_CHIP_MGAG550)
        ? g550_thresholds : g450_thresholds;
    CARD8 ret = 0;

    while (*ptr != -1 && *ptr++ < mode->Clock)
        ret += 0x08;

    return ret;
}

static void
output_panel1_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                      DisplayModePtr adjusted_mode)
{
    MGAPtr pMga = MGAPTR(output->scrn);
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR(output->scrn);
    CARD8 disp_ctl;

    outMGAdac(MGA1064_PAN_CTL, get_pan_ctl_value (output, mode));

    disp_ctl = inMGAdac(MGA1064_DISP_CTL);
    disp_ctl &= ~MGA1064_DISP_CTL_PANOUTSEL_MASK;

    /* Is this output mapped to CRTC1? */
    if (output->crtc == config->crtc[0])
        disp_ctl |= MGA1064_DISP_CTL_PANOUTSEL_CRTC1;
    else
        disp_ctl |= MGA1064_DISP_CTL_PANOUTSEL_CRTC2RGB;

    outMGAdac(MGA1064_DISP_CTL, disp_ctl);
}

static void
output_panel2_mode_set(xf86OutputPtr output, DisplayModePtr mode,
                      DisplayModePtr adjusted_mode)
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
MgaGOutputDac1Init(ScrnInfoPtr scrn, Bool number)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86OutputPtr output;
    MgaOutputDataPtr data;
    const char *name = number ? "VGA1" : "VGA";

    data = xnfcalloc(sizeof(MgaOutputDataRec), 1);
    if (!data)
        return NULL;

    output = xf86OutputCreate(scrn, &output_dac1_funcs, name);
    if (!output) {
        xfree(data);
        return NULL;
    }

    output->driver_private = data;

    data->ddc_bus = pMga->DDC_Bus1;

    return output;
}

xf86OutputPtr
MgaGOutputDac2Init(ScrnInfoPtr scrn, Bool number)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86OutputPtr output;
    MgaOutputDataPtr data;
    const char *name = number ? "VGA2" : "VGA";

    data = xnfcalloc(sizeof(MgaOutputDataRec), 1);
    if (!data)
        return NULL;

    output = xf86OutputCreate(scrn, &output_dac2_funcs, name);
    if (!output) {
        xfree(data);
        return NULL;
    }

    output->driver_private = data;

    data->ddc_bus = pMga->DDC_Bus2;

    return output;
}

xf86OutputPtr
MgaGOutputPanel1Init(ScrnInfoPtr scrn, Bool number)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86OutputPtr output;
    MgaOutputDataPtr data;
    const char *name = number ? "DVI1" : "DVI";

    data = xnfcalloc(sizeof(MgaOutputDataRec), 1);
    if (!data)
        return NULL;

    output = xf86OutputCreate(scrn, &output_panel1_funcs, name);
    if (!output) {
        xfree(data);
        return NULL;
    }

    output->driver_private = data;

    data->ddc_bus = pMga->DDC_Bus1;

    return output;
}

xf86OutputPtr
MgaGOutputPanel2Init(ScrnInfoPtr scrn, Bool number)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86OutputPtr output;
    MgaOutputDataPtr data;
    const char *name = number ? "DVI2" : "DVI";

    data = xnfcalloc(sizeof(MgaOutputDataRec), 1);
    if (!data)
        return NULL;

    output = xf86OutputCreate(scrn, &output_panel2_funcs, name);
    if (!output) {
        xfree(data);
        return NULL;
    }

    output->driver_private = data;

    data->ddc_bus = pMga->DDC_Bus2;

    return output;
}
