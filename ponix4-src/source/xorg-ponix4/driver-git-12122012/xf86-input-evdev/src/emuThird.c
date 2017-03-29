/*
 * Copyright © 2011 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of the authors
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  The authors make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * THE AUTHORS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* Right mouse button emulation code.
 * Emulates a right button event if the first button is held down for a
 * timeout. If the device moves more than a certain amount before the
 * timeout is over, the emulation is cancelled and a normal button event is
 * generated.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "evdev.h"

#include <X11/Xatom.h>
#include <xf86.h>
#include <xf86Xinput.h>
#include <exevents.h>

#include <evdev-properties.h>

/* Threshold (in device coordinates) for devices to cancel emulation */
#define DEFAULT_MOVE_THRESHOLD 20

static Atom prop_3bemu;         /* Right button emulation on/off property   */
static Atom prop_3btimeout;     /* Right button timeout property            */
static Atom prop_3bbutton;      /* Right button target physical button      */
static Atom prop_3bthreshold;   /* Right button move cancellation threshold */

/* State machine for 3rd button emulation */
enum EmulationState {
    EM3B_OFF,             /* no event      */
    EM3B_PENDING,         /* timer pending */
    EM3B_EMULATING        /* in emulation  */
};

static void
Evdev3BEmuPostButtonEvent(InputInfoPtr pInfo, int button, enum ButtonAction act)
{
    EvdevPtr          pEvdev   = pInfo->private;
    struct emulate3B *emu3B    = &pEvdev->emulate3B;
    int               absolute = Relative;

    /* if we cancel, emit the button down event at our start position,
     * not at the current position. Only for absolute devices though. For
     * relative events, this may be a bit iffy since pointer accel may shoot
     * us back more than we moved and confuse the user.
     */
    if (emu3B->flags & EVDEV_ABSOLUTE_EVENTS)
        absolute = Absolute;

    xf86PostButtonEventP(pInfo->dev, absolute, button,
                         (act == BUTTON_PRESS) ? 1 : 0, 0,
                         (absolute ? 2 : 0), emu3B->startpos);
}


/**
 * Timer function. Post a button down event to the server.
 *
 * @param arg The InputInfoPtr for this device.
 */
CARD32
Evdev3BEmuTimer(OsTimerPtr timer, CARD32 time, pointer arg)
{
    InputInfoPtr      pInfo    = (InputInfoPtr)arg;
    EvdevPtr          pEvdev   = pInfo->private;
    struct emulate3B *emu3B    = &pEvdev->emulate3B;
    int               sigstate = 0;

    sigstate = xf86BlockSIGIO ();
    emu3B->state = EM3B_EMULATING;
    Evdev3BEmuPostButtonEvent(pInfo, emu3B->button, BUTTON_PRESS);
    xf86UnblockSIGIO (sigstate);
    return 0;
}


/**
 * Cancel all emulation, reset the timer and reset deltas.
 */
static void
Evdev3BCancel(InputInfoPtr pInfo)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;

    if (emu3B->state != EM3B_OFF)
    {
        TimerCancel(emu3B->timer);
        emu3B->state = EM3B_OFF;
        memset(emu3B->delta, 0, sizeof(emu3B->delta));
    }

    emu3B->flags = 0;
}

/**
 * Emulate a third button on button press. Note that emulation only triggers
 * on button 1.
 *
 * Return TRUE if event was swallowed by middle mouse button emulation,
 * FALSE otherwise.
 */
BOOL
Evdev3BEmuFilterEvent(InputInfoPtr pInfo, int button, BOOL press)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;
    int               ret    = FALSE;

    if (!emu3B->enabled)
        goto out;

    if (press)
        emu3B->buttonstate |= button;
    else
        emu3B->buttonstate &= ~button;

    /* Any other button pressed? Cancel timer */
    if (button != 1)
    {
        switch (emu3B->state)
        {
            case EM3B_PENDING:
                Evdev3BEmuPostButtonEvent(pInfo, 1, BUTTON_PRESS);
                Evdev3BCancel(pInfo);
                break;
            case EM3B_EMULATING:
                /* We're emulating and now the user pressed a different
                 * button. Just release the emulating one, tell the user to
                 * not do that and get on with life */
                Evdev3BEmuPostButtonEvent(pInfo, emu3B->button, BUTTON_RELEASE);
                Evdev3BCancel(pInfo);
                break;
            default:
                break;
        }
        goto out;
    }

    /* Don't emulate if any other button is down */
    if ((emu3B->buttonstate & ~0x1) != 0)
        goto out;

    /* Release event → cancel, send press and release now. */
    if (!press)
    {
        switch(emu3B->state)
        {
            case EM3B_PENDING:
                Evdev3BEmuPostButtonEvent(pInfo, 1, BUTTON_PRESS);
                Evdev3BCancel(pInfo);
                break;
            case EM3B_EMULATING:
                Evdev3BEmuPostButtonEvent(pInfo, emu3B->button, BUTTON_RELEASE);
                Evdev3BCancel(pInfo);
                ret = TRUE;
                break;
            default:
                break;
        }

        goto out;
    }

    if (press && emu3B->state == EM3B_OFF)
    {
        emu3B->state = EM3B_PENDING;
        emu3B->timer = TimerSet(emu3B->timer, 0, emu3B->timeout,
                                Evdev3BEmuTimer, pInfo);
        ret = TRUE;
        goto out;
    }

out:
    return ret;
}

/**
 * Handle absolute x/y motion. If the motion is above the threshold, cancel
 * emulation.
 */
void
Evdev3BEmuProcessAbsMotion(InputInfoPtr pInfo, ValuatorMask *vals)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;
    int               cancel = FALSE;
    int               axis   = 0;

    if (emu3B->state != EM3B_PENDING)
    {
        if (valuator_mask_isset(vals, 0))
            emu3B->startpos[0] = valuator_mask_get(vals, 0);
        if (valuator_mask_isset(vals, 1))
            emu3B->startpos[1] = valuator_mask_get(vals, 1);

        return;
    }

    if ((emu3B->flags & EVDEV_ABSOLUTE_EVENTS) == 0)
        emu3B->flags |= EVDEV_ABSOLUTE_EVENTS;

    while (axis <= 1 && !cancel)
    {
        if (valuator_mask_isset(vals, axis))
        {
            int delta = valuator_mask_get(vals, axis) - emu3B->startpos[axis];
            if (abs(delta) > emu3B->threshold)
                cancel = TRUE;
        }
        axis++;
    }

    if (cancel)
    {
        Evdev3BEmuPostButtonEvent(pInfo, 1, BUTTON_PRESS);
        Evdev3BCancel(pInfo);
    }
}

/**
 * Handle relative x/y motion. If the motion is above the threshold, cancel
 * emulation.
 */
void
Evdev3BEmuProcessRelMotion(InputInfoPtr pInfo, int dx, int dy)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;

    if (emu3B->state != EM3B_PENDING)
        return;

    emu3B->delta[0] += dx;
    emu3B->delta[1] += dy;
    emu3B->flags |= EVDEV_RELATIVE_EVENTS;

    if (abs(emu3B->delta[0]) > emu3B->threshold ||
        abs(emu3B->delta[1]) > emu3B->threshold)
    {
        Evdev3BEmuPostButtonEvent(pInfo, 1, BUTTON_PRESS);
        Evdev3BCancel(pInfo);
    }
}

void
Evdev3BEmuPreInit(InputInfoPtr pInfo)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;

    emu3B->enabled = xf86SetBoolOption(pInfo->options,
                                       "EmulateThirdButton",
                                       FALSE);
    emu3B->timeout = xf86SetIntOption(pInfo->options,
                                      "EmulateThirdButtonTimeout",
                                      1000);
    emu3B->button = xf86SetIntOption(pInfo->options,
                                      "EmulateThirdButtonButton",
                                      3);
    /* FIXME: this should be auto-configured based on axis ranges */
    emu3B->threshold = xf86SetIntOption(pInfo->options,
                                         "EmulateThirdButtonMoveThreshold",
                                         DEFAULT_MOVE_THRESHOLD);
    /* allocate now so we don't allocate in the signal handler */
    emu3B->timer = TimerSet(NULL, 0, 0, NULL, NULL);
}

void
Evdev3BEmuOn(InputInfoPtr pInfo)
{
    /* This function just exists for symmetry in evdev.c */
}

void
Evdev3BEmuFinalize(InputInfoPtr pInfo)
{
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;

    TimerFree(emu3B->timer);
    emu3B->timer = NULL;
}

static int
Evdev3BEmuSetProperty(DeviceIntPtr dev, Atom atom, XIPropertyValuePtr val,
                      BOOL checkonly)
{
    InputInfoPtr      pInfo  = dev->public.devicePrivate;
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;

    if (atom == prop_3bemu)
    {
        if (val->format != 8 || val->size != 1 || val->type != XA_INTEGER)
            return BadMatch;

        if (!checkonly)
            emu3B->enabled = *((BOOL*)val->data);

    } else if (atom == prop_3btimeout)
    {
        if (val->format != 32 || val->size != 1 || val->type != XA_INTEGER)
            return BadMatch;

        if (!checkonly)
            emu3B->timeout = *((CARD32*)val->data);

    } else if (atom == prop_3bbutton)
    {
        if (val->format != 8 || val->size != 1 || val->type != XA_INTEGER)
            return BadMatch;

        if (!checkonly)
            emu3B->button = *((CARD8*)val->data);
    } else if (atom == prop_3bthreshold)
    {
        if (val->format != 32 || val->size != 1 || val->type != XA_INTEGER)
            return BadMatch;

        if (!checkonly)
            emu3B->threshold = *((CARD32*)val->data);
    }


    return Success;
}

/**
 * Initialise properties for third button emulation
 */
void
Evdev3BEmuInitProperty(DeviceIntPtr dev)
{
    InputInfoPtr      pInfo  = dev->public.devicePrivate;
    EvdevPtr          pEvdev = pInfo->private;
    struct emulate3B *emu3B  = &pEvdev->emulate3B;
    int               rc;

    if (!dev->button) /* don't init prop for keyboards */
        return;

    /* third button emulation on/off */
    prop_3bemu = MakeAtom(EVDEV_PROP_THIRDBUTTON, strlen(EVDEV_PROP_THIRDBUTTON), TRUE);
    rc = XIChangeDeviceProperty(dev, prop_3bemu, XA_INTEGER, 8,
                                PropModeReplace, 1,
                                &emu3B->enabled,
                                FALSE);
    if (rc != Success)
        return;

    XISetDevicePropertyDeletable(dev, prop_3bemu, FALSE);

    /* third button emulation timeout */
    prop_3btimeout = MakeAtom(EVDEV_PROP_THIRDBUTTON_TIMEOUT,
                              strlen(EVDEV_PROP_THIRDBUTTON_TIMEOUT),
                              TRUE);
    rc = XIChangeDeviceProperty(dev, prop_3btimeout, XA_INTEGER, 32, PropModeReplace, 1,
                                &emu3B->timeout, FALSE);

    if (rc != Success)
        return;

    XISetDevicePropertyDeletable(dev, prop_3btimeout, FALSE);

    /* third button emulation button to be triggered  */
    prop_3bbutton = MakeAtom(EVDEV_PROP_THIRDBUTTON_BUTTON,
                             strlen(EVDEV_PROP_THIRDBUTTON_BUTTON),
                             TRUE);
    rc = XIChangeDeviceProperty(dev, prop_3bbutton, XA_INTEGER, 8, PropModeReplace, 1,
                                &emu3B->button, FALSE);

    if (rc != Success)
        return;

    XISetDevicePropertyDeletable(dev, prop_3bbutton, FALSE);

    /* third button emulation movement threshold */
    prop_3bthreshold = MakeAtom(EVDEV_PROP_THIRDBUTTON_THRESHOLD,
                                strlen(EVDEV_PROP_THIRDBUTTON_THRESHOLD),
                                TRUE);
    rc = XIChangeDeviceProperty(dev, prop_3bthreshold, XA_INTEGER, 32, PropModeReplace, 1,
                                &emu3B->threshold, FALSE);

    if (rc != Success)
        return;

    XISetDevicePropertyDeletable(dev, prop_3bthreshold, FALSE);

    XIRegisterPropertyHandler(dev, Evdev3BEmuSetProperty, NULL, NULL);
}
