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
 *    Luugi Marsan
 *
 * Sources:
 *    xf86-video-intel, mga_dacG.c
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/* All drivers should typically include these */
#include "xf86.h"
#include "xf86_OSproc.h"

/* Drivers for PCI hardware need this */
#include "xf86PciInfo.h"

/* Drivers that need to access the PCI config space directly need this */
#include "xf86Pci.h"

#include "mga_reg.h"
#include "mga.h"
#include "mga_macros.h"
#include "mga_dacG.h"

typedef struct {
    long clock;

    CARD32 c2ctl;
    CARD32 c2hparam;
    CARD32 c2hsync;
    CARD32 c2vparam;
    CARD32 c2vsync;
    CARD32 c2startadd0;
    CARD32 c2offset;
    CARD32 c2datactl;

    /* DAC registers */
    CARD8 sync_ctl;
    CARD8 pwr_ctl;
} MgaCrtcState, *MgaCrtcStatePtr;

typedef struct {
    MgaCrtcState saved_state;
} MgaCrtcDataRec, *MgaCrtcDataPtr;

static void crtc_dpms(xf86CrtcPtr crtc, int mode);
static void crtc_save(xf86CrtcPtr crtc);
static void crtc_restore(xf86CrtcPtr crtc);
static Bool crtc_lock(xf86CrtcPtr crtc);
static void crtc_unlock(xf86CrtcPtr crtc);
static Bool crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
                            DisplayModePtr adjusted_mode);
static void crtc_prepare(xf86CrtcPtr crtc);
static void crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
                          DisplayModePtr adjusted_mode, int x, int y);
static void crtc_commit(xf86CrtcPtr crtc);
static void crtc_destroy(xf86CrtcPtr crtc);

static const xf86CrtcFuncsRec crtc_funcs = {
    .dpms = crtc_dpms,
    .save = crtc_save,
    .restore = crtc_restore,
    .lock = crtc_lock,
    .unlock = crtc_unlock,
    .mode_fixup = crtc_mode_fixup,
    .prepare = crtc_prepare,
    .mode_set = crtc_mode_set,
    .commit = crtc_commit,
    .destroy = crtc_destroy
};

static void
state_set(xf86CrtcPtr crtc, MgaCrtcStatePtr state,
          DisplayModePtr mode, int x, int y)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);

    /* This code will only work for G450/G550, so we can just
     * copy the clock.
     */
    state->clock = mode->Clock;

    state->c2ctl = INREG(MGAREG_C2CTL);
    state->c2ctl |= MGAREG_C2CTL_C2_EN;

    state->c2ctl &= ~MGAREG_C2CTL_CRTCDACSEL_MASK;
    state->c2ctl |= MGAREG_C2CTL_CRTCDACSEL_CRTC1;

    state->c2hparam = (((mode->HDisplay - 8) << 16) |
                        (mode->HTotal - 8));
    state->c2hsync = (((mode->HSyncEnd - 8) << 16) |
                       (mode->HSyncStart - 8));

    state->c2vparam = (((mode->VDisplay - 1) << 16) |
                        (mode->VTotal - 1));
    state->c2vsync = (((mode->VSyncEnd - 1) << 16) |
                       (mode->VSyncStart - 1));

    state->c2startadd0 = pMga->DstOrg;
    state->c2startadd0 += (y * crtc->scrn->displayWidth + x) *
                          crtc->scrn->bitsPerPixel / 8;
    state->c2startadd0 &= 0x1ffffc0;

    state->c2offset = crtc->scrn->displayWidth;

    state->c2datactl = INREG(MGAREG_C2DATACTL);
    state->c2datactl &= 0xffffff00;

    state->c2ctl &= 0xff1fffff;

    switch (crtc->scrn->bitsPerPixel) {
    case 15:
        state->c2ctl |= 0x200000;
        state->c2offset *= 2;
        break;
    case 16:
        state->c2ctl |= 0x400000;
        state->c2offset *= 2;
        break;
    case 32:
        state->c2ctl |= 0x800000;
        state->c2offset *= 4;
    }
}

static void
state_restore(xf86CrtcPtr crtc, MgaCrtcStatePtr state)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);

    if (state->clock)
        MGAG450SetPLLFreq(crtc->scrn, MGA_VIDEO_PLL, state->clock);

    OUTREG(MGAREG_C2HPARAM, state->c2hparam);
    OUTREG(MGAREG_C2HSYNC, state->c2hsync);
    OUTREG(MGAREG_C2VPARAM, state->c2vparam);
    OUTREG(MGAREG_C2VSYNC, state->c2vsync);
    OUTREG(MGAREG_C2STARTADD0, state->c2startadd0);
    OUTREG(MGAREG_C2OFFSET, state->c2offset);
    OUTREG(MGAREG_C2DATACTL, state->c2datactl);
    OUTREG(MGAREG_C2CTL, state->c2ctl);
}

static void
state_save(xf86CrtcPtr crtc, MgaCrtcStatePtr state)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);

    state->clock = MGAG450SavePLLFreq(crtc->scrn, MGA_VIDEO_PLL);
    state->sync_ctl = inMGAdac(MGA1064_SYNC_CTL);

    // output?
    //state->pwr_ctl = inMGAdac(MGA1064_PWR_CTL);

    /* the crtc2 mode registers are read only, so we cannot
     * save them here.
     */
}

static void
crtc_dpms(xf86CrtcPtr crtc, int mode)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);
    CARD32 c2ctl;
    CARD8 misc, gen, pwr_ctl, mask;

    if (mode == DPMSModeOn) {
        /* FIXME:
         * What exactly are we doing here? :)
         * Not sure if this actually belongs to the DAC2 output.
         * Also need to set DAC2 sync polarity (see mga_dh.c).
         */

        /* We don't use MISC synch pol, must be 0*/
        misc = inMGAdreg(MGAREG_MISC_READ);
        misc &= ~MGAREG_MISC_HSYNCPOL;
        misc &= ~MGAREG_MISC_VSYNCPOL;
        OUTREG8(MGAREG_MISC_WRITE, misc);

        /* Set Rset to 0.7 V */
        gen = inMGAdac(MGA1064_GEN_IO_CTL);
        gen &= ~0x40;
        outMGAdac(MGA1064_GEN_IO_CTL, gen);

        gen = inMGAdac( MGA1064_GEN_IO_DATA);
        gen &= ~0x40;
        outMGAdac(MGA1064_GEN_IO_DATA, gen);
    }

    c2ctl = INREG(MGAREG_C2CTL);

    if (mode == DPMSModeOn)
        c2ctl &= ~MGAREG_C2CTL_PIXCLKDIS_DISABLE;
    else
        c2ctl |= MGAREG_C2CTL_PIXCLKDIS_DISABLE;

    OUTREG(MGAREG_C2CTL, c2ctl);

    pwr_ctl = inMGAdac(MGA1064_PWR_CTL);

    /* FIXME:
     * Maybe enable VID_PLL in crtc_prepare(), since it needs to be
     * enabled if we want to set the video pll m/n/p values.
     */
    pwr_ctl |= MGA1064_PWR_CTL_VID_PLL_EN;

    mask = MGA1064_PWR_CTL_CFIFO_EN;

    if (mode == DPMSModeOn)
        outMGAdac(MGA1064_PWR_CTL, pwr_ctl | mask);
    else
        outMGAdac(MGA1064_PWR_CTL, pwr_ctl & ~mask);
}

static Bool
crtc_lock(xf86CrtcPtr crtc)
{
#ifdef XF86DRI
    return MGADRILock(crtc->scrn);
#else
    return FALSE;
#endif
}

static void
crtc_unlock(xf86CrtcPtr crtc)
{
#ifdef XF86DRI
    MGADRIUnlock(crtc->scrn);
#endif
}

static void
crtc_save(xf86CrtcPtr crtc)
{
    MgaCrtcDataPtr data = MGACRTCDATAPTR(crtc);

    state_save(crtc, &data->saved_state);
}

static void
crtc_restore(xf86CrtcPtr crtc)
{
    MgaCrtcDataPtr data = MGACRTCDATAPTR(crtc);
    MGAPtr pMga = MGAPTR(crtc->scrn);

    outMGAdac(MGA1064_SYNC_CTL, data->saved_state.sync_ctl);
}

static Bool
crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
                DisplayModePtr adjusted_mode)
{
    return TRUE;
}

static void
crtc_prepare(xf86CrtcPtr crtc)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);
    CARD32 val;

    /* According to the docs, we should drop the _C2_EN bit
     * before programming CRTC2.
     */
    val = INREG(MGAREG_C2CTL);
    val &= ~MGAREG_C2CTL_C2_EN;
    OUTREG(MGAREG_C2CTL, val);

    /* Set CRTC2 to use the video PLL */
    val &= ~MGAREG_C2CTL_PIXCLKSEL_MASK;
    val &= ~MGAREG_C2CTL_PIXCLKSELH_MASK;
    val |= MGAREG_C2CTL_PIXCLKSEL_VIDEOPLL;
    OUTREG(MGAREG_C2CTL, val);

    crtc->funcs->dpms(crtc, DPMSModeOff);
}

static void
crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
              DisplayModePtr adjusted_mode, int x, int y)
{
    MGAPtr pMga = MGAPTR(crtc->scrn);
    MgaCrtcState state;

    memset (&state, 0, sizeof (state));

    state_set(crtc, &state, mode, x, y);
    state_restore(crtc, &state);

    /* FIXME:
     * Don't use magic numbers here. The bits in SYNC_CTL were
     * documented in mga_dh.c.
     *
     * Also I'm not sure whether this is the best place to set SYNC_CTL.
     */
    outMGAdac(MGA1064_SYNC_CTL, 0xcc);
}

static void
crtc_commit(xf86CrtcPtr crtc)
{
    crtc->funcs->dpms(crtc, DPMSModeOn);

    /* We don't have to enable _C2_EN here,
     * because state_restore() already did that for us.
     */
}

static void
crtc_destroy(xf86CrtcPtr crtc)
{
    if (crtc->driver_private)
        xfree (crtc->driver_private);
}

Bool
MgaGCrtc2Init(ScrnInfoPtr scrn)
{
    MGAPtr pMga = MGAPTR(scrn);
    xf86CrtcPtr crtc;
    MgaCrtcDataPtr data;

    /* This code only works for G450/G550 */
    if (!MGAISGx50(pMga))
        return FALSE;

    data = xnfcalloc (sizeof (MgaCrtcDataRec), 1);
    if (!data)
        return FALSE;

    crtc = xf86CrtcCreate (scrn, &crtc_funcs);
    if (!crtc) {
        xfree(data);
        return FALSE;
    }

    crtc->driver_private = data;

    return TRUE;
}
