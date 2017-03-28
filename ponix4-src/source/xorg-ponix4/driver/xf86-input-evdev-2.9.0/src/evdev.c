/*
 * Copyright © 2004-2008 Red Hat, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without
 * fee, provided that the above copyright notice appear in all copies
 * and that both that copyright notice and this permission notice
 * appear in supporting documentation, and that the name of Red Hat
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  Red
 * Hat makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
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
 * Authors:
 *	Kristian Høgsberg (krh@redhat.com)
 *	Adam Jackson (ajax@redhat.com)
 *	Peter Hutterer (peter.hutterer@redhat.com)
 *	Oliver McFadden (oliver.mcfadden@nokia.com)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "evdev.h"
#include "axis_labels.h"

#include <X11/keysym.h>
#include <X11/extensions/XI.h>

#include <linux/version.h>
#include <sys/stat.h>
#include <libudev.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <xf86.h>
#include <xf86Xinput.h>
#include <exevents.h>
#include <xorgVersion.h>
#include <xkbsrv.h>

#include <X11/Xatom.h>
#include <evdev-properties.h>
#include <xserver-properties.h>
#ifdef MULTITOUCH
#include <mtdev-plumbing.h>
#endif

#ifndef XI_PROP_PRODUCT_ID
#define XI_PROP_PRODUCT_ID "Device Product ID"
#endif

#ifndef XI_PROP_VIRTUAL_DEVICE
#define XI_PROP_VIRTUAL_DEVICE "Virtual Device"
#endif

/* removed from server, purge when dropping support for server 1.10 */
#define XI86_SEND_DRAG_EVENTS   0x08

#define ArrayLength(a) (sizeof(a) / (sizeof((a)[0])))

#define MIN_KEYCODE 8
#define GLYPHS_PER_KEY 2
#define AltMask		Mod1Mask
#define NumLockMask	Mod2Mask
#define AltLangMask	Mod3Mask
#define KanaMask	Mod4Mask
#define ScrollLockMask	Mod5Mask

#define CAPSFLAG	1
#define NUMFLAG		2
#define SCROLLFLAG	4
#define MODEFLAG	8
#define COMPOSEFLAG	16

#ifndef ABS_MT_SLOT
#define ABS_MT_SLOT 0x2f
#endif

#ifndef ABS_MT_TRACKING_ID
#define ABS_MT_TRACKING_ID 0x39
#endif

#ifndef XI86_SERVER_FD
#define XI86_SERVER_FD 0x20
#endif

static const char *evdevDefaults[] = {
    "XkbRules",     "evdev",
    "XkbModel",     "pc104", /* the right model for 'us' */
    "XkbLayout",    "us",
    NULL
};

/* Any of those triggers a proximity event */
static int proximity_bits[] = {
        BTN_TOOL_PEN,
        BTN_TOOL_RUBBER,
        BTN_TOOL_BRUSH,
        BTN_TOOL_PENCIL,
        BTN_TOOL_AIRBRUSH,
        BTN_TOOL_FINGER,
        BTN_TOOL_MOUSE,
        BTN_TOOL_LENS,
};

static int EvdevOn(DeviceIntPtr);
static int EvdevCache(InputInfoPtr pInfo);
static void EvdevKbdCtrl(DeviceIntPtr device, KeybdCtrl *ctrl);
static int EvdevSwitchMode(ClientPtr client, DeviceIntPtr device, int mode);
static BOOL EvdevGrabDevice(InputInfoPtr pInfo, int grab, int ungrab);
static void EvdevSetCalibration(InputInfoPtr pInfo, int num_calibration, int calibration[4]);
static int EvdevOpenDevice(InputInfoPtr pInfo);
static void EvdevCloseDevice(InputInfoPtr pInfo);

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms);
static void EvdevInitOneAxisLabel(EvdevPtr pEvdev, int mapped_axis,
                                  const char **labels, int label_idx, Atom *atoms);
static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms);
static void EvdevInitProperty(DeviceIntPtr dev);
static int EvdevSetProperty(DeviceIntPtr dev, Atom atom,
                            XIPropertyValuePtr val, BOOL checkonly);
static Atom prop_product_id;
static Atom prop_invert;
static Atom prop_calibration;
static Atom prop_swap;
static Atom prop_axis_label;
static Atom prop_btn_label;
static Atom prop_device;
static Atom prop_virtual;
static Atom prop_scroll_dist;

static int EvdevSwitchMode(ClientPtr client, DeviceIntPtr device, int mode)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (pEvdev->flags & EVDEV_RELATIVE_EVENTS)
    {
        if (mode == Relative)
            return Success;
        else
            return XI_BadMode;
    }

    switch (mode) {
        case Absolute:
            pEvdev->flags &= ~EVDEV_RELATIVE_MODE;
            break;

        case Relative:
            pEvdev->flags |= EVDEV_RELATIVE_MODE;
            break;

        default:
            return XI_BadMode;
    }

    return Success;
}

static inline int EvdevBitIsSet(const unsigned long *array, int bit)
{
    return !!(array[bit / LONG_BITS] & (1LL << (bit % LONG_BITS)));
}

static inline void EvdevSetBit(unsigned long *array, int bit)
{
    array[bit / LONG_BITS] |= (1LL << (bit % LONG_BITS));
}

static int
EvdevGetMajorMinor(InputInfoPtr pInfo)
{
    struct stat st;

    if (fstat(pInfo->fd, &st) == -1)
    {
        xf86IDrvMsg(pInfo, X_ERROR, "stat failed (%s). cannot check for duplicates.\n",
                    strerror(errno));
        return 0;
    }

    return st.st_rdev;
}

/**
 * Return TRUE if one of the devices we know about has the same min/maj
 * number.
 */
static BOOL
EvdevIsDuplicate(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    InputInfoPtr d;

    nt_list_for_each_entry(d, xf86FirstLocalDevice(), next)
    {
        EvdevPtr e;

        if (strcmp(d->drv->driverName, "evdev") != 0)
            continue;

        e = (EvdevPtr)d->private;
        if (e != pEvdev &&
            e->min_maj &&
            e->min_maj == pEvdev->min_maj)
            return TRUE;
    }

    return FALSE;
}

static BOOL
EvdevDeviceIsVirtual(const char* devicenode)
{
    struct udev *udev = NULL;
    struct udev_device *device = NULL;
    struct stat st;
    int rc = FALSE;
    const char *devpath;

    udev = udev_new();
    if (!udev)
        goto out;

    if (stat(devicenode, &st) == -1)
        goto out;

    device = udev_device_new_from_devnum(udev, 'c', st.st_rdev);

    if (!device)
        goto out;


    devpath = udev_device_get_devpath(device);
    if (!devpath)
        goto out;

    if (strstr(devpath, "LNXSYSTM"))
        rc = TRUE;

out:
    udev_device_unref(device);
    udev_unref(udev);
    return rc;
}

#ifndef HAVE_SMOOTH_SCROLLING
static int wheel_up_button = 4;
static int wheel_down_button = 5;
static int wheel_left_button = 6;
static int wheel_right_button = 7;
#endif

static EventQueuePtr
EvdevNextInQueue(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->num_queue >= EVDEV_MAXQUEUE)
    {
        LogMessageVerbSigSafe(X_WARNING, 0, "dropping event due to full queue!\n");
        return NULL;
    }

    pEvdev->num_queue++;
    return &pEvdev->queue[pEvdev->num_queue - 1];
}

void
EvdevQueueKbdEvent(InputInfoPtr pInfo, struct input_event *ev, int value)
{
    int code = ev->code + MIN_KEYCODE;
    EventQueuePtr pQueue;

    /* Filter all repeated events from device.
       We'll do softrepeat in the server, but only since 1.6 */
    if (value == 2)
        return;

    if ((pQueue = EvdevNextInQueue(pInfo)))
    {
        pQueue->type = EV_QUEUE_KEY;
        pQueue->detail.key = code;
        pQueue->val = value;
    }
}

void
EvdevQueueButtonEvent(InputInfoPtr pInfo, int button, int value)
{
    EventQueuePtr pQueue;

    if ((pQueue = EvdevNextInQueue(pInfo)))
    {
        pQueue->type = EV_QUEUE_BTN;
        pQueue->detail.key = button;
        pQueue->val = value;
    }
}

void
EvdevQueueProximityEvent(InputInfoPtr pInfo, int value)
{
    EventQueuePtr pQueue;
    if ((pQueue = EvdevNextInQueue(pInfo)))
    {
        pQueue->type = EV_QUEUE_PROXIMITY;
        pQueue->detail.key = 0;
        pQueue->val = value;
    }
}

#ifdef MULTITOUCH
void
EvdevQueueTouchEvent(InputInfoPtr pInfo, unsigned int touch, ValuatorMask *mask,
                     uint16_t evtype)
{
    EventQueuePtr pQueue;
    if ((pQueue = EvdevNextInQueue(pInfo)))
    {
        pQueue->type = EV_QUEUE_TOUCH;
        pQueue->detail.touch = touch;
        valuator_mask_copy(pQueue->touchMask, mask);
        pQueue->val = evtype;
    }
}
#endif

/**
 * Post button event right here, right now.
 * Interface for MB emulation since these need to post immediately.
 */
void
EvdevPostButtonEvent(InputInfoPtr pInfo, int button, enum ButtonAction act)
{
    xf86PostButtonEvent(pInfo->dev, Relative, button,
                        (act == BUTTON_PRESS) ? 1 : 0, 0, 0);
}

void
EvdevQueueButtonClicks(InputInfoPtr pInfo, int button, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        EvdevQueueButtonEvent(pInfo, button, 1);
        EvdevQueueButtonEvent(pInfo, button, 0);
    }
}

static void
EvdevSwapAbsValuators(EvdevPtr pEvdev, ValuatorMask *mask)
{
    int i;
    int swapped_isset[2] = {0, 0};
    int swapped_values[2];

    if (!pEvdev->swap_axes)
        return;

    for(i = 0; i <= 1; i++) {
        if (valuator_mask_isset(mask, i)) {
            const struct input_absinfo *abs1 =
                libevdev_get_abs_info(pEvdev->dev, i);
            const struct input_absinfo *abs2 =
                libevdev_get_abs_info(pEvdev->dev, 1 - i);

            swapped_isset[1 - i] = 1;
            swapped_values[1 - i] =
                xf86ScaleAxis(valuator_mask_get(mask, i),
                              abs2->maximum, abs2->minimum,
                              abs1->maximum, abs1->minimum);
        }
    }

    for (i = 0; i <= 1; i++) {
        if (swapped_isset[i])
            valuator_mask_set(mask, i, swapped_values[i]);
        else
            valuator_mask_unset(mask, i);
    }
}

static void
EvdevApplyCalibration(EvdevPtr pEvdev, ValuatorMask *mask)
{
    int i;

    for (i = 0; i <= 1; i++) {
        const struct input_absinfo *abs;
        int val;
        int calib_min;
        int calib_max;

        if (!valuator_mask_isset(mask, i))
            continue;

        val = valuator_mask_get(mask, i);
        abs = libevdev_get_abs_info(pEvdev->dev, i);

        if (i == 0) {
            calib_min = pEvdev->calibration.min_x;
            calib_max = pEvdev->calibration.max_x;
        } else {
            calib_min = pEvdev->calibration.min_y;
            calib_max = pEvdev->calibration.max_y;
        }

        if (pEvdev->flags & EVDEV_CALIBRATED)
            val = xf86ScaleAxis(val, abs->maximum, abs->minimum,
                                calib_max, calib_min);

        if ((i == 0 && pEvdev->invert_x) || (i == 1 && pEvdev->invert_y))
            val = (abs->maximum - val + abs->minimum);

        valuator_mask_set(mask, i, val);
    }
}

/**
 * Take the valuators and process them accordingly.
 */
static void
EvdevProcessValuators(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    int *delta = pEvdev->delta;

    if (pEvdev->abs_queued) {
        /* convert to relative motion for touchpads */
        if (pEvdev->flags & EVDEV_RELATIVE_MODE) {
            if (pEvdev->in_proximity) {
                if (valuator_mask_isset(pEvdev->vals, 0))
                {
                    if (valuator_mask_isset(pEvdev->old_vals, 0))
                        delta[REL_X] = valuator_mask_get(pEvdev->vals, 0) -
                            valuator_mask_get(pEvdev->old_vals, 0);
                    valuator_mask_set(pEvdev->old_vals, 0,
                            valuator_mask_get(pEvdev->vals, 0));
                }
                if (valuator_mask_isset(pEvdev->vals, 1))
                {
                    if (valuator_mask_isset(pEvdev->old_vals, 1))
                        delta[REL_Y] = valuator_mask_get(pEvdev->vals, 1) -
                            valuator_mask_get(pEvdev->old_vals, 1);
                    valuator_mask_set(pEvdev->old_vals, 1,
                            valuator_mask_get(pEvdev->vals, 1));
                }
            } else {
                valuator_mask_zero(pEvdev->old_vals);
            }
            valuator_mask_zero(pEvdev->vals);
            pEvdev->abs_queued = 0;
            pEvdev->rel_queued = 1;
        } else {
            int val;
            if (valuator_mask_fetch(pEvdev->vals, 0, &val))
                valuator_mask_set(pEvdev->old_vals, 0, val);
            if (valuator_mask_fetch(pEvdev->vals, 1, &val))
                valuator_mask_set(pEvdev->old_vals, 1, val);
        }
    }

    if (pEvdev->rel_queued) {
        int tmp;
        int i;

        if (pEvdev->swap_axes) {
            tmp = pEvdev->delta[REL_X];
            pEvdev->delta[REL_X] = pEvdev->delta[REL_Y];
            pEvdev->delta[REL_Y] = tmp;
            if (pEvdev->delta[REL_X] == 0)
                valuator_mask_unset(pEvdev->vals, REL_X);
            if (pEvdev->delta[REL_Y] == 0)
                valuator_mask_unset(pEvdev->vals, REL_Y);
        }
        if (pEvdev->invert_x)
            pEvdev->delta[REL_X] *= -1;
        if (pEvdev->invert_y)
            pEvdev->delta[REL_Y] *= -1;


        Evdev3BEmuProcessRelMotion(pInfo,
                                   pEvdev->delta[REL_X],
                                   pEvdev->delta[REL_Y]);

        for (i = 0; i < REL_CNT; i++)
        {
            int map = pEvdev->rel_axis_map[i];
            if (pEvdev->delta[i] && map != -1)
                valuator_mask_set(pEvdev->vals, map, pEvdev->delta[i]);
        }
    }
    /*
     * Some devices only generate valid abs coords when BTN_TOOL_PEN is
     * pressed.  On wacom tablets, this means that the pen is in
     * proximity of the tablet.  After the pen is removed, BTN_TOOL_PEN is
     * released, and a (0, 0) absolute event is generated.  Checking
     * pEvdev->in_proximity here lets us ignore that event.  pEvdev is
     * initialized to 1 so devices that don't use this scheme still
     * just works.
     */
    else if (pEvdev->abs_queued && pEvdev->in_proximity) {
        EvdevSwapAbsValuators(pEvdev, pEvdev->vals);
        EvdevApplyCalibration(pEvdev, pEvdev->vals);
        Evdev3BEmuProcessAbsMotion(pInfo, pEvdev->vals);
    }
}

static void
EvdevProcessProximityEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    EvdevPtr pEvdev = pInfo->private;

    if (!pEvdev->use_proximity)
        return;

    pEvdev->prox_queued = 1;

    EvdevQueueProximityEvent(pInfo, ev->value);
}

/**
 * Proximity handling is rather weird because of tablet-specific issues.
 * Some tablets, notably Wacoms, send a 0/0 coordinate in the same EV_SYN as
 * the out-of-proximity notify. We need to ignore those, hence we only
 * actually post valuator events when we're in proximity.
 *
 * Other tablets send the x/y coordinates, then EV_SYN, then the proximity
 * event. For those, we need to remember x/y to post it when the proximity
 * comes.
 *
 * If we're not in proximity and we get valuator events, remember that, they
 * won't be posted though. If we move into proximity without valuators, use
 * the last ones we got and let the rest of the code post them.
 */
static int
EvdevProcessProximityState(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    int prox_state = 0;
    int i;

    /* Does this device have any proximity axes? */
    if (!pEvdev->prox)
        return 0;

    /* no proximity change in the queue */
    if (!pEvdev->prox_queued)
    {
        if (pEvdev->abs_queued && !pEvdev->in_proximity)
            for (i = 0; i < valuator_mask_size(pEvdev->vals); i++)
                if (valuator_mask_isset(pEvdev->vals, i))
                    valuator_mask_set(pEvdev->prox, i,
                                      valuator_mask_get(pEvdev->vals, i));
        return 0;
    }

    for (i = 0; i < pEvdev->num_queue; i++)
    {
        if (pEvdev->queue[i].type == EV_QUEUE_PROXIMITY)
        {
            prox_state = pEvdev->queue[i].val;
            break;
        }
    }

    if ((prox_state && !pEvdev->in_proximity) ||
        (!prox_state && pEvdev->in_proximity))
    {
        /* We're about to go into/out of proximity but have no abs events
         * within the EV_SYN. Use the last coordinates we have. */
        for (i = 0; i < valuator_mask_size(pEvdev->prox); i++)
            if (!valuator_mask_isset(pEvdev->vals, i) &&
                valuator_mask_isset(pEvdev->prox, i))
                valuator_mask_set(pEvdev->vals, i,
                                  valuator_mask_get(pEvdev->prox, i));
        valuator_mask_zero(pEvdev->prox);

        pEvdev->abs_queued = valuator_mask_size(pEvdev->vals);
    }

    pEvdev->in_proximity = prox_state;
    return 1;
}

/**
 * Take a button input event and process it accordingly.
 */
static void
EvdevProcessButtonEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    unsigned int button;
    int value;
    EvdevPtr pEvdev = pInfo->private;

    button = EvdevUtilButtonEventToButtonNumber(pEvdev, ev->code);

    /* Get the signed value, earlier kernels had this as unsigned */
    value = ev->value;

    /* Handle drag lock */
    if (EvdevDragLockFilterEvent(pInfo, button, value))
        return;

    if (EvdevWheelEmuFilterButton(pInfo, button, value))
        return;

    if (EvdevMBEmuFilterEvent(pInfo, button, value))
        return;

    if (button)
        EvdevQueueButtonEvent(pInfo, button, value);
    else
        EvdevQueueKbdEvent(pInfo, ev, value);
}

/**
 * Take the relative motion input event and process it accordingly.
 */
static void
EvdevProcessRelativeMotionEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    int value;
    EvdevPtr pEvdev = pInfo->private;
    int map;

    /* Get the signed value, earlier kernels had this as unsigned */
    value = ev->value;

    switch (ev->code) {
#ifndef HAVE_SMOOTH_SCROLLING
        case REL_WHEEL:
            if (value > 0)
                EvdevQueueButtonClicks(pInfo, wheel_up_button, value);
            else if (value < 0)
                EvdevQueueButtonClicks(pInfo, wheel_down_button, -value);
            break;

        case REL_DIAL:
        case REL_HWHEEL:
            if (value > 0)
                EvdevQueueButtonClicks(pInfo, wheel_right_button, value);
            else if (value < 0)
                EvdevQueueButtonClicks(pInfo, wheel_left_button, -value);
            break;
        /* We don't post wheel events as axis motion. */
#endif
        default:
            /* Ignore EV_REL events if we never set up for them. */
            if (!(pEvdev->flags & EVDEV_RELATIVE_EVENTS) &&
                    ev->code != REL_WHEEL && ev->code != REL_DIAL &&
                    ev->code != REL_HWHEEL)
                return;

            /* Handle mouse wheel emulation */
            if (EvdevWheelEmuFilterMotion(pInfo, ev))
                return;

            pEvdev->rel_queued = 1;
            pEvdev->delta[ev->code] += value;
            map = pEvdev->rel_axis_map[ev->code];
            valuator_mask_set(pEvdev->vals, map, value);
            break;
    }
}

#ifdef MULTITOUCH
static void
EvdevProcessTouch(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    int type;

    if (pEvdev->cur_slot < 0 || !pEvdev->mt_mask)
        return;

    /* If the ABS_MT_SLOT is the first event we get after EV_SYN, skip this */
    if (pEvdev->slot_state == SLOTSTATE_EMPTY)
        return;

    if (pEvdev->slot_state == SLOTSTATE_CLOSE)
        type = XI_TouchEnd;
    else if (pEvdev->slot_state == SLOTSTATE_OPEN)
        type = XI_TouchBegin;
    else
        type = XI_TouchUpdate;


    EvdevSwapAbsValuators(pEvdev, pEvdev->mt_mask);
    EvdevApplyCalibration(pEvdev, pEvdev->mt_mask);

    EvdevQueueTouchEvent(pInfo, pEvdev->cur_slot, pEvdev->mt_mask, type);

    pEvdev->slot_state = SLOTSTATE_EMPTY;

    valuator_mask_zero(pEvdev->mt_mask);
}

static int
num_slots(EvdevPtr pEvdev)
{
    int value;

    if (pEvdev->mtdev)
        value = pEvdev->mtdev->caps.slot.maximum + 1;
    else
        value = libevdev_get_num_slots(pEvdev->dev);

    /* If we don't know how many slots there are, assume at least 10 */
    return value > 1 ? value : 10;
}

static int
last_mt_vals_slot(EvdevPtr pEvdev)
{
    int value = pEvdev->cur_slot;

    return value < num_slots(pEvdev) ? value : -1;
}

static void
EvdevProcessTouchEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    EvdevPtr pEvdev = pInfo->private;
    int map;

    if (!pEvdev->mtdev &&
        !libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_SLOT))
        return;

    if (ev->code == ABS_MT_SLOT) {
        EvdevProcessTouch(pInfo);
        pEvdev->cur_slot = ev->value;
    } else
    {
        int slot_index = last_mt_vals_slot(pEvdev);

        if (pEvdev->slot_state == SLOTSTATE_EMPTY)
            pEvdev->slot_state = SLOTSTATE_UPDATE;
        if (ev->code == ABS_MT_TRACKING_ID) {
            if (ev->value >= 0) {
                pEvdev->slot_state = SLOTSTATE_OPEN;

                if (slot_index >= 0)
                    valuator_mask_copy(pEvdev->mt_mask,
                                       pEvdev->last_mt_vals[slot_index]);
                else
                    LogMessageVerbSigSafe(X_WARNING, 0,
                                "%s: Attempted to copy values from out-of-range "
                                "slot, touch events may be incorrect.\n",
                                pInfo->name);
            } else
                pEvdev->slot_state = SLOTSTATE_CLOSE;
        } else {
            map = pEvdev->abs_axis_map[ev->code];
            valuator_mask_set(pEvdev->mt_mask, map, ev->value);
            if (slot_index >= 0)
                valuator_mask_set(pEvdev->last_mt_vals[slot_index], map,
                                  ev->value);
        }
    }
}
#else
#define EvdevProcessTouch(pInfo)
#define EvdevProcessTouchEvent(pInfo, ev)
#endif /* MULTITOUCH */

/**
 * Take the absolute motion input event and process it accordingly.
 */
static void
EvdevProcessAbsoluteMotionEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    int value;
    EvdevPtr pEvdev = pInfo->private;
    int map;

    /* Get the signed value, earlier kernels had this as unsigned */
    value = ev->value;

    /* Ignore EV_ABS events if we never set up for them. */
    if (!(pEvdev->flags & EVDEV_ABSOLUTE_EVENTS))
        return;

    if (ev->code > ABS_MAX)
        return;

    if (EvdevWheelEmuFilterMotion(pInfo, ev))
        return;

    if (ev->code >= ABS_MT_SLOT) {
        EvdevProcessTouchEvent(pInfo, ev);
        pEvdev->abs_queued = 1;
    } else if (!pEvdev->mt_mask) {
        map = pEvdev->abs_axis_map[ev->code];
        valuator_mask_set(pEvdev->vals, map, value);
        pEvdev->abs_queued = 1;
    }
}

/**
 * Take the key press/release input event and process it accordingly.
 */
static void
EvdevProcessKeyEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    int value, i;
    EvdevPtr pEvdev = pInfo->private;

    /* Get the signed value, earlier kernels had this as unsigned */
    value = ev->value;

    /* don't repeat mouse buttons */
    if (ev->code >= BTN_MOUSE && ev->code < KEY_OK)
        if (value == 2)
            return;

    for (i = 0; i < ArrayLength(proximity_bits); i++)
    {
        if (ev->code == proximity_bits[i])
        {
            EvdevProcessProximityEvent(pInfo, ev);
            return;
        }
    }

    switch (ev->code) {
        case BTN_TOUCH:
            /* For devices that have but don't use proximity, use
             * BTN_TOUCH as the proximity notifier */
            if (!pEvdev->use_proximity)
                pEvdev->in_proximity = value ? ev->code : 0;
            if (!(pEvdev->flags & (EVDEV_TOUCHSCREEN | EVDEV_TABLET)) ||
                pEvdev->mt_mask)
                break;
            /* Treat BTN_TOUCH from devices that only have BTN_TOUCH as
             * BTN_LEFT. */
            ev->code = BTN_LEFT;
            /* Intentional fallthrough! */

        default:
            EvdevProcessButtonEvent(pInfo, ev);
            break;
    }
}

/**
 * Post the relative motion events.
 */
void
EvdevPostRelativeMotionEvents(InputInfoPtr pInfo, int num_v, int first_v,
                              int v[MAX_VALUATORS])
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->rel_queued) {
        xf86PostMotionEventM(pInfo->dev, Relative, pEvdev->vals);
    }
}

/**
 * Post the absolute motion events.
 */
void
EvdevPostAbsoluteMotionEvents(InputInfoPtr pInfo, int num_v, int first_v,
                              int v[MAX_VALUATORS])
{
    EvdevPtr pEvdev = pInfo->private;

    /*
     * Some devices only generate valid abs coords when BTN_TOOL_PEN is
     * pressed.  On wacom tablets, this means that the pen is in
     * proximity of the tablet.  After the pen is removed, BTN_TOOL_PEN is
     * released, and a (0, 0) absolute event is generated.  Checking
     * pEvdev->in_proximity here lets us ignore that event.
     * pEvdev->in_proximity is initialized to 1 so devices that don't use
     * this scheme still just work.
     */
    if (pEvdev->abs_queued && pEvdev->in_proximity) {
        xf86PostMotionEventM(pInfo->dev, Absolute, pEvdev->vals);
    }
}

static void
EvdevPostProximityEvents(InputInfoPtr pInfo, int which, int num_v, int first_v,
                                  int v[MAX_VALUATORS])
{
    int i;
    EvdevPtr pEvdev = pInfo->private;

    for (i = 0; pEvdev->prox_queued && i < pEvdev->num_queue; i++) {
        switch (pEvdev->queue[i].type) {
            case EV_QUEUE_KEY:
            case EV_QUEUE_BTN:
#ifdef MULTITOUCH
            case EV_QUEUE_TOUCH:
#endif
                break;
            case EV_QUEUE_PROXIMITY:
                if (pEvdev->queue[i].val == which)
                    xf86PostProximityEventP(pInfo->dev, which, first_v, num_v,
                            v + first_v);
                break;
        }
    }
}

/**
 * Post the queued key/button events.
 */
static void EvdevPostQueuedEvents(InputInfoPtr pInfo, int num_v, int first_v,
                                  int v[MAX_VALUATORS])
{
    int i;
    EvdevPtr pEvdev = pInfo->private;

    for (i = 0; i < pEvdev->num_queue; i++) {
        switch (pEvdev->queue[i].type) {
        case EV_QUEUE_KEY:
            xf86PostKeyboardEvent(pInfo->dev, pEvdev->queue[i].detail.key,
                                  pEvdev->queue[i].val);
            break;
        case EV_QUEUE_BTN:
            if (Evdev3BEmuFilterEvent(pInfo,
                                      pEvdev->queue[i].detail.key,
                                      pEvdev->queue[i].val))
                break;

            if (pEvdev->abs_queued && pEvdev->in_proximity) {
                xf86PostButtonEventP(pInfo->dev, Absolute, pEvdev->queue[i].detail.key,
                                     pEvdev->queue[i].val, first_v, num_v,
                                     v + first_v);

            } else
                xf86PostButtonEvent(pInfo->dev, Relative, pEvdev->queue[i].detail.key,
                                    pEvdev->queue[i].val, 0, 0);
            break;
        case EV_QUEUE_PROXIMITY:
            break;
#ifdef MULTITOUCH
        case EV_QUEUE_TOUCH:
            xf86PostTouchEvent(pInfo->dev, pEvdev->queue[i].detail.touch,
                               pEvdev->queue[i].val, 0,
                               pEvdev->queue[i].touchMask);
            break;
#endif
        }
    }
}

/**
 * Take the synchronization input event and process it accordingly; the motion
 * notify events are sent first, then any button/key press/release events.
 */
static void
EvdevProcessSyncEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    int i;
    int num_v = 0, first_v = 0;
    int v[MAX_VALUATORS] = {};
    EvdevPtr pEvdev = pInfo->private;

    EvdevProcessProximityState(pInfo);

    EvdevProcessValuators(pInfo);
    EvdevProcessTouch(pInfo);

    EvdevPostProximityEvents(pInfo, TRUE, num_v, first_v, v);
    EvdevPostRelativeMotionEvents(pInfo, num_v, first_v, v);
    EvdevPostAbsoluteMotionEvents(pInfo, num_v, first_v, v);
    EvdevPostQueuedEvents(pInfo, num_v, first_v, v);
    EvdevPostProximityEvents(pInfo, FALSE, num_v, first_v, v);

    memset(pEvdev->delta, 0, sizeof(pEvdev->delta));
    for (i = 0; i < ArrayLength(pEvdev->queue); i++)
    {
        EventQueuePtr queue = &pEvdev->queue[i];
        queue->detail.key = 0;
        queue->type = 0;
        queue->val = 0;
        /* don't reset the touchMask */
    }

    if (pEvdev->vals)
        valuator_mask_zero(pEvdev->vals);
    pEvdev->num_queue = 0;
    pEvdev->abs_queued = 0;
    pEvdev->rel_queued = 0;
    pEvdev->prox_queued = 0;

}

/**
 * Process the events from the device; nothing is actually posted to the server
 * until an EV_SYN event is received.
 */
static void
EvdevProcessEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    switch (ev->type) {
        case EV_REL:
            EvdevProcessRelativeMotionEvent(pInfo, ev);
            break;
        case EV_ABS:
            EvdevProcessAbsoluteMotionEvent(pInfo, ev);
            break;
        case EV_KEY:
            EvdevProcessKeyEvent(pInfo, ev);
            break;
        case EV_SYN:
            EvdevProcessSyncEvent(pInfo, ev);
            break;
    }
}

#undef ABS_X_VALUE
#undef ABS_Y_VALUE
#undef ABS_VALUE

static void
EvdevFreeMasks(EvdevPtr pEvdev)
{
#ifdef MULTITOUCH
    int i;
#endif

    valuator_mask_free(&pEvdev->vals);
    valuator_mask_free(&pEvdev->old_vals);
    valuator_mask_free(&pEvdev->prox);
#ifdef MULTITOUCH
    valuator_mask_free(&pEvdev->mt_mask);
    if (pEvdev->last_mt_vals)
    {
        for (i = 0; i < libevdev_get_num_slots(pEvdev->dev); i++)
            valuator_mask_free(&pEvdev->last_mt_vals[i]);
        free(pEvdev->last_mt_vals);
        pEvdev->last_mt_vals = NULL;
    }
    for (i = 0; i < EVDEV_MAXQUEUE; i++)
        valuator_mask_free(&pEvdev->queue[i].touchMask);
#endif
}

#ifdef MULTITOUCH
static void
EvdevHandleMTDevEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    EvdevPtr pEvdev = pInfo->private;

    mtdev_put_event(pEvdev->mtdev, ev);
    if (libevdev_event_is_code(ev, EV_SYN, SYN_REPORT)) {
        while (!mtdev_empty(pEvdev->mtdev)) {
            struct input_event e;
            mtdev_get_event(pEvdev->mtdev, &e);
            EvdevProcessEvent(pInfo, &e);
        }
    }
}
#endif

static void
EvdevReadInput(InputInfoPtr pInfo)
{
    int rc = 0;
    EvdevPtr pEvdev = pInfo->private;
    struct input_event ev;

    do {
        rc = libevdev_next_event(pEvdev->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc < 0) {
            if (rc == -ENODEV) /* May happen after resume */
                xf86RemoveEnabledDevice(pInfo);
            else if (rc != -EAGAIN)
                LogMessageVerbSigSafe(X_ERROR, 0, "%s: Read error: %s\n", pInfo->name,
                                       strerror(-rc));
            break;
        } else if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
#ifdef MULTITOUCH
            if (pEvdev->mtdev)
                EvdevHandleMTDevEvent(pInfo, &ev);
            else
#endif
                EvdevProcessEvent(pInfo, &ev);
        }
        else { /* SYN_DROPPED */
            rc = libevdev_next_event(pEvdev->dev, LIBEVDEV_READ_FLAG_SYNC, &ev);
            while (rc == LIBEVDEV_READ_STATUS_SYNC) {
#ifdef MULTITOUCH
                if (pEvdev->mtdev)
                    EvdevHandleMTDevEvent(pInfo, &ev);
                else
#endif
                    EvdevProcessEvent(pInfo, &ev);
                rc = libevdev_next_event(pEvdev->dev, LIBEVDEV_READ_FLAG_SYNC, &ev);
            }
        }
    } while (rc == LIBEVDEV_READ_STATUS_SUCCESS);
}

static void
EvdevPtrCtrlProc(DeviceIntPtr device, PtrCtrl *ctrl)
{
    /* Nothing to do, dix handles all settings */
}

static void
EvdevKbdCtrl(DeviceIntPtr device, KeybdCtrl *ctrl)
{
    static struct { int xbit, code; } bits[] = {
        { CAPSFLAG,	LED_CAPSL },
        { NUMFLAG,	LED_NUML },
        { SCROLLFLAG,	LED_SCROLLL },
        { MODEFLAG,	LED_KANA },
        { COMPOSEFLAG,	LED_COMPOSE }
    };

    InputInfoPtr pInfo;
    struct input_event ev[ArrayLength(bits) + 1];
    int i;

    memset(ev, 0, sizeof(ev));

    pInfo = device->public.devicePrivate;
    for (i = 0; i < ArrayLength(bits); i++) {
        ev[i].type = EV_LED;
        ev[i].code = bits[i].code;
        ev[i].value = (ctrl->leds & bits[i].xbit) > 0;
    }

    ev[i].type = EV_SYN;
    ev[i].code = SYN_REPORT;
    ev[i].value = 0;

    write(pInfo->fd, ev, sizeof ev);
}

static int
EvdevAddKeyClass(DeviceIntPtr device)
{
    int rc = Success;
    XkbRMLVOSet rmlvo = {0};
    InputInfoPtr pInfo;

    pInfo = device->public.devicePrivate;

    /* sorry, no rules change allowed for you */
    xf86ReplaceStrOption(pInfo->options, "xkb_rules", "evdev");
    rmlvo.rules = xf86SetStrOption(pInfo->options, "xkb_rules", NULL);
    rmlvo.model = xf86SetStrOption(pInfo->options, "xkb_model", NULL);
    rmlvo.layout = xf86SetStrOption(pInfo->options, "xkb_layout", NULL);
    rmlvo.variant = xf86SetStrOption(pInfo->options, "xkb_variant", NULL);
    rmlvo.options = xf86SetStrOption(pInfo->options, "xkb_options", NULL);

    if (!InitKeyboardDeviceStruct(device, &rmlvo, NULL, EvdevKbdCtrl))
        rc = !Success;

    XkbFreeRMLVOSet(&rmlvo, FALSE);

    return rc;
}

#ifdef MULTITOUCH
/* MT axes are counted twice - once as ABS_X (which the kernel keeps for
 * backwards compatibility), once as ABS_MT_POSITION_X. So we need to keep a
 * mapping of those axes to make sure we only count them once
 */
struct mt_axis_mappings {
    int mt_code;
    int code;
    Bool needs_mapping; /* TRUE if both code and mt_code are present */
    int mapping;        /* Logical mapping of 'code' axis */
};

static struct mt_axis_mappings mt_axis_mappings[] = {
    {ABS_MT_POSITION_X, ABS_X},
    {ABS_MT_POSITION_Y, ABS_Y},
    {ABS_MT_PRESSURE, ABS_PRESSURE},
    {ABS_MT_DISTANCE, ABS_DISTANCE},
};
#endif

/**
 * return TRUE if the axis is not one we should count as true axis
 */
static int
is_blacklisted_axis(int axis)
{
    switch(axis)
    {
        case ABS_MT_SLOT:
        case ABS_MT_TRACKING_ID:
            return TRUE;
        default:
            return FALSE;
    }
}


static int
EvdevAddAbsValuatorClass(DeviceIntPtr device, int want_scroll_axes)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int num_axes = 0, axis, i = 0;
    int num_mt_axes = 0, /* number of MT-only axes */
        num_mt_axes_total = 0; /* total number of MT axes, including
                                  double-counted ones, excluding blacklisted */
    Atom *atoms;
    int mapping = 0;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (!libevdev_has_event_type(pEvdev->dev, EV_ABS))
        goto out;

    for (i = 0; i < ABS_MAX; i++)
        if (libevdev_has_event_code(pEvdev->dev, EV_ABS, i))
            num_axes++;

    if (num_axes < 1)
        goto out;

#ifdef MULTITOUCH
    for (axis = ABS_MT_SLOT; axis < ABS_MAX; axis++)
    {
        if (libevdev_has_event_code(pEvdev->dev, EV_ABS, axis))
        {
            int j;
            Bool skip = FALSE;

            for (j = 0; j < ArrayLength(mt_axis_mappings); j++)
            {
                if (mt_axis_mappings[j].mt_code == axis &&
                    libevdev_has_event_code(pEvdev->dev, EV_ABS, mt_axis_mappings[j].code))
                {
                    mt_axis_mappings[j].needs_mapping = TRUE;
                    skip = TRUE;
                }
            }

            if (!is_blacklisted_axis(axis))
            {
                num_mt_axes_total++;
                if (!skip)
                    num_mt_axes++;
            }
            num_axes--;
        }
    }

    /* device only has mt-axes. the kernel should give us ABS_X etc for
       backwards compat but some devices don't have it. */
    if (num_axes == 0 && num_mt_axes > 0) {
        xf86IDrvMsg(pInfo, X_ERROR,
                    "found only multitouch-axes. That shouldn't happen.\n");
        goto out;
    }

#endif

#ifdef HAVE_SMOOTH_SCROLLING
    if (want_scroll_axes && libevdev_has_event_type(pEvdev->dev, EV_REL))
    {
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_WHEEL))
            num_axes++;
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_HWHEEL))
            num_axes++;
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_DIAL))
            num_axes++;
    }
#endif

    if (num_axes + num_mt_axes > MAX_VALUATORS) {
        xf86IDrvMsg(pInfo, X_WARNING, "found %d axes, limiting to %d.\n", num_axes, MAX_VALUATORS);
        num_axes = MAX_VALUATORS;
    }

    if (num_axes < 1 && num_mt_axes_total < 1) {
        xf86Msg(X_WARNING, "%s: no absolute or touch axes found.\n",
                device->name);
        return !Success;
    }

    pEvdev->num_vals = num_axes;
    if (num_axes > 0) {
        pEvdev->vals = valuator_mask_new(num_axes);
        pEvdev->old_vals = valuator_mask_new(num_axes);
        if (!pEvdev->vals || !pEvdev->old_vals) {
            xf86IDrvMsg(pInfo, X_ERROR, "failed to allocate valuator masks.\n");
            goto out;
        }
    }
#ifdef MULTITOUCH
    if (num_mt_axes_total > 0) {
        int nslots = num_slots(pEvdev);

        pEvdev->num_mt_vals = num_mt_axes_total;
        pEvdev->mt_mask = valuator_mask_new(num_mt_axes_total);
        if (!pEvdev->mt_mask) {
            xf86Msg(X_ERROR, "%s: failed to allocate MT valuator mask.\n",
                    device->name);
            goto out;
        }

        pEvdev->last_mt_vals = calloc(nslots, sizeof(ValuatorMask *));
        if (!pEvdev->last_mt_vals) {
            xf86IDrvMsg(pInfo, X_ERROR,
                        "%s: failed to allocate MT last values mask array.\n",
                        device->name);
            goto out;
        }

        for (i = 0; i < nslots; i++) {
            pEvdev->last_mt_vals[i] = valuator_mask_new(num_mt_axes_total);
            if (!pEvdev->last_mt_vals[i]) {
                xf86IDrvMsg(pInfo, X_ERROR,
                            "%s: failed to allocate MT last values mask.\n",
                            device->name);
                goto out;
            }
        }

        for (i = 0; i < EVDEV_MAXQUEUE; i++) {
            pEvdev->queue[i].touchMask =
                valuator_mask_new(num_mt_axes_total);
            if (!pEvdev->queue[i].touchMask) {
                xf86Msg(X_ERROR, "%s: failed to allocate MT valuator masks for "
                        "evdev event queue.\n", device->name);
                goto out;
            }
        }
    }
#endif
    atoms = malloc((pEvdev->num_vals + num_mt_axes) * sizeof(Atom));

    i = 0;
    for (axis = ABS_X; i < MAX_VALUATORS && axis <= ABS_MAX; axis++) {
#ifdef MULTITOUCH
        int j;
#endif
        pEvdev->abs_axis_map[axis] = -1;
        if (!libevdev_has_event_code(pEvdev->dev, EV_ABS, axis) ||
            is_blacklisted_axis(axis))
            continue;

        mapping = i;

#ifdef MULTITOUCH
        for (j = 0; j < ArrayLength(mt_axis_mappings); j++)
        {
            if (mt_axis_mappings[j].code == axis)
                mt_axis_mappings[j].mapping = mapping;
            else if (mt_axis_mappings[j].mt_code == axis &&
                    mt_axis_mappings[j].needs_mapping)
                mapping = mt_axis_mappings[j].mapping;
        }
#endif
        pEvdev->abs_axis_map[axis] = mapping;
        if (mapping == i)
            i++;
    }

#ifdef HAVE_SMOOTH_SCROLLING
    if (want_scroll_axes)
    {
        mapping++; /* continue from abs axis mapping */

        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_HWHEEL))
            pEvdev->rel_axis_map[REL_HWHEEL] = mapping++;
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_DIAL))
            pEvdev->rel_axis_map[REL_DIAL] = mapping++;
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_WHEEL))
            pEvdev->rel_axis_map[REL_WHEEL] = mapping++;
    }
#endif

    EvdevInitAxesLabels(pEvdev, Absolute, pEvdev->num_vals + num_mt_axes, atoms);

    if (!InitValuatorClassDeviceStruct(device, num_axes + num_mt_axes, atoms,
                                       GetMotionHistorySize(), Absolute)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize valuator class device.\n");
        goto out;
    }

#ifdef MULTITOUCH
    if (num_mt_axes_total > 0)
    {
        int num_touches = 0;
        int mode = pEvdev->flags & EVDEV_TOUCHPAD ?
            XIDependentTouch : XIDirectTouch;

        num_touches = num_slots(pEvdev);

        if (!InitTouchClassDeviceStruct(device, num_touches, mode,
                                        num_mt_axes_total)) {
            xf86Msg(X_ERROR, "%s: failed to initialize touch class device.\n",
                    device->name);
            goto out;
        }

        for (i = 0; i < num_touches; i++) {
            for (axis = ABS_MT_TOUCH_MAJOR; axis < ABS_MAX; axis++) {
                if (pEvdev->abs_axis_map[axis] >= 0) {
                    int val = pEvdev->mtdev ? 0 : libevdev_get_current_slot(pEvdev->dev);
                    /* XXX: read initial values from mtdev when it adds support
                     *      for doing so. */
                    valuator_mask_set(pEvdev->last_mt_vals[i],
                                      pEvdev->abs_axis_map[axis], val);
                }
            }
        }
    }
#endif

    for (axis = ABS_X; axis < ABS_MT_SLOT; axis++) {
        const struct input_absinfo *abs;
        int axnum = pEvdev->abs_axis_map[axis];
        int resolution = 0;

        if (axnum == -1)
            continue;

        abs = libevdev_get_abs_info(pEvdev->dev, axis);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 30)
        /* Kernel provides units/mm, X wants units/m */
        resolution = abs->resolution * 1000;
#endif

        xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   abs->minimum,
                                   abs->maximum,
                                   resolution, 0, resolution, Absolute);
        xf86InitValuatorDefaults(device, axnum);
    }

#ifdef MULTITOUCH
    for (axis = ABS_MT_TOUCH_MAJOR; axis <= ABS_MAX; axis++) {
        const struct input_absinfo *abs;
        int axnum = pEvdev->abs_axis_map[axis];
        int resolution = 0;
        int j;
        BOOL skip = FALSE;

        if (axnum < 0)
            continue;

        abs = libevdev_get_abs_info(pEvdev->dev, axis);

        for (j = 0; j < ArrayLength(mt_axis_mappings); j++)
            if (mt_axis_mappings[j].mt_code == axis &&
                    mt_axis_mappings[j].needs_mapping)
            {
                skip = TRUE;
                break;
            }

        /* MT axis is mapped, don't set up twice */
        if (skip)
            continue;

        resolution = abs->resolution * 1000;

        xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   abs->minimum,
                                   abs->maximum,
                                   resolution, 0, resolution,
                                   Absolute);
    }
#endif

#ifdef HAVE_SMOOTH_SCROLLING
    if (want_scroll_axes)
    {
        int idx;
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_WHEEL))
        {
            idx = REL_WHEEL;
            xf86InitValuatorAxisStruct(device,
                                       pEvdev->rel_axis_map[idx],
                                       atoms[pEvdev->rel_axis_map[idx]],
                                       NO_AXIS_LIMITS, NO_AXIS_LIMITS,
                                       0, 0, 0, Relative);
            SetScrollValuator(device, pEvdev->rel_axis_map[idx],
                              SCROLL_TYPE_VERTICAL,
                              -pEvdev->smoothScroll.vert_delta,
                              SCROLL_FLAG_PREFERRED);
        }

        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_HWHEEL))
        {
            idx = REL_HWHEEL;
            xf86InitValuatorAxisStruct(device,
                                       pEvdev->rel_axis_map[idx],
                                       atoms[pEvdev->rel_axis_map[idx]],
                                       NO_AXIS_LIMITS, NO_AXIS_LIMITS,
                                       0, 0, 0, Relative);
            SetScrollValuator(device, pEvdev->rel_axis_map[idx],
                              SCROLL_TYPE_HORIZONTAL,
                              pEvdev->smoothScroll.horiz_delta,
                              SCROLL_FLAG_NONE);
        }

        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_DIAL))
        {
            idx = REL_DIAL;
            xf86InitValuatorAxisStruct(device,
                                       pEvdev->rel_axis_map[idx],
                                       atoms[pEvdev->rel_axis_map[idx]],
                                       NO_AXIS_LIMITS, NO_AXIS_LIMITS,
                                       0, 0, 0, Relative);
            SetScrollValuator(device, pEvdev->rel_axis_map[idx],
                              SCROLL_TYPE_HORIZONTAL,
                              pEvdev->smoothScroll.dial_delta,
                              SCROLL_FLAG_NONE);
        }
    }
#endif

    free(atoms);

    for (i = 0; i < ArrayLength(proximity_bits); i++)
    {
        if (!pEvdev->use_proximity)
            break;

        if (libevdev_has_event_code(pEvdev->dev, EV_KEY, proximity_bits[i]))
        {
            InitProximityClassDeviceStruct(device);
            pEvdev->prox = valuator_mask_new(num_axes);
            if (!pEvdev->prox) {
                xf86IDrvMsg(pInfo, X_ERROR,
                            "failed to allocate proximity valuator " "mask.\n");
                goto out;
            }
            break;
        }
    }

    if (!InitPtrFeedbackClassDeviceStruct(device, EvdevPtrCtrlProc)) {
        xf86IDrvMsg(pInfo, X_ERROR,
                    "failed to initialize pointer feedback class device.\n");
        goto out;
    }

    if (pEvdev->flags & EVDEV_TOUCHPAD)
        pEvdev->flags |= EVDEV_RELATIVE_MODE;
    else
        pEvdev->flags &= ~EVDEV_RELATIVE_MODE;

    if (xf86FindOption(pInfo->options, "Mode"))
    {
        char *mode;
        mode = xf86SetStrOption(pInfo->options, "Mode", NULL);
        if (!strcasecmp("absolute", mode))
            pEvdev->flags &= ~EVDEV_RELATIVE_MODE;
        else if (!strcasecmp("relative", mode))
            pEvdev->flags |= EVDEV_RELATIVE_MODE;
        else
            xf86IDrvMsg(pInfo, X_INFO, "unknown mode, use default\n");
        free(mode);
    }

    return Success;

out:
    EvdevFreeMasks(pEvdev);
    return !Success;
}

static int
EvdevSetScrollValuators(DeviceIntPtr device)
{
#ifdef HAVE_SMOOTH_SCROLLING
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int axnum;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    axnum = pEvdev->rel_axis_map[REL_WHEEL];
    if (axnum != -1) {
        SetScrollValuator(device, axnum, SCROLL_TYPE_VERTICAL,
                          -pEvdev->smoothScroll.vert_delta,
                          SCROLL_FLAG_PREFERRED);
    }

    axnum = pEvdev->rel_axis_map[REL_DIAL];
    if (axnum != -1) {
        SetScrollValuator(device, axnum, SCROLL_TYPE_HORIZONTAL,
                          pEvdev->smoothScroll.dial_delta,
                          SCROLL_FLAG_NONE);
    }

    axnum = pEvdev->rel_axis_map[REL_HWHEEL];
    if (axnum != -1) {
        SetScrollValuator(device, axnum, SCROLL_TYPE_HORIZONTAL,
                          pEvdev->smoothScroll.horiz_delta,
                          SCROLL_FLAG_NONE);
    }
#endif

    return Success;
}

static int
EvdevAddRelValuatorClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int num_axes = 0, axis, map, i = 0;
    Atom *atoms;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (!libevdev_has_event_type(pEvdev->dev, EV_REL))
        goto out;

    for (i = 0; i < REL_MAX; i++)
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, i))
            num_axes++;
    if (num_axes < 1)
        goto out;

#ifndef HAVE_SMOOTH_SCROLLING
    /* Wheels are special, we post them as button events. So let's ignore them
     * in the axes list too */
    if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_WHEEL))
        num_axes--;
    if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_HWHEEL))
        num_axes--;
    if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_DIAL))
        num_axes--;

    if (num_axes <= 0)
        goto out;
#endif

    if (num_axes > MAX_VALUATORS) {
        xf86IDrvMsg(pInfo, X_WARNING, "found %d axes, limiting to %d.\n", num_axes, MAX_VALUATORS);
        num_axes = MAX_VALUATORS;
    }

    pEvdev->num_vals = num_axes;
    if (num_axes > 0) {
        pEvdev->vals = valuator_mask_new(num_axes);
        if (!pEvdev->vals)
            goto out;
    }
    atoms = malloc(pEvdev->num_vals * sizeof(Atom));

    for (axis = REL_X, map = 0; map < MAX_VALUATORS && axis <= REL_MAX; axis++)
    {
        pEvdev->rel_axis_map[axis] = -1;
#ifndef HAVE_SMOOTH_SCROLLING
        /* We don't post wheel events, so ignore them here too */
        if (axis == REL_WHEEL || axis == REL_HWHEEL || axis == REL_DIAL)
            continue;
#endif
        if (!libevdev_has_event_code(pEvdev->dev, EV_REL, axis))
            continue;
        pEvdev->rel_axis_map[axis] = map;
        map++;
    }

    EvdevInitAxesLabels(pEvdev, Relative, pEvdev->num_vals, atoms);

    if (!InitValuatorClassDeviceStruct(device, num_axes, atoms,
                                       GetMotionHistorySize(), Relative)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize valuator class device.\n");
        goto out;
    }

    if (!InitPtrFeedbackClassDeviceStruct(device, EvdevPtrCtrlProc)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize pointer feedback class "
                "device.\n");
        goto out;
    }

    for (axis = REL_X; axis <= REL_MAX; axis++)
    {
        int axnum = pEvdev->rel_axis_map[axis];

        if (axnum == -1)
            continue;
        xf86InitValuatorAxisStruct(device, axnum, atoms[axnum], -1, -1, 1, 0, 1,
                                   Relative);
        xf86InitValuatorDefaults(device, axnum);
    }

    EvdevSetScrollValuators(device);

    free(atoms);

    return Success;

out:
    valuator_mask_free(&pEvdev->vals);
    return !Success;
}

static int
EvdevAddButtonClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    Atom *labels;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    labels = malloc(pEvdev->num_buttons * sizeof(Atom));
    EvdevInitButtonLabels(pEvdev, pEvdev->num_buttons, labels);

    if (!InitButtonClassDeviceStruct(device, pEvdev->num_buttons, labels,
                                     pEvdev->btnmap))
        return !Success;

    free(labels);
    return Success;
}

/**
 * Init the button mapping for the device. By default, this is a 1:1 mapping,
 * i.e. Button 1 maps to Button 1, Button 2 to 2, etc.
 *
 * If a mapping has been specified, the mapping is the default, with the
 * user-defined ones overwriting the defaults.
 * i.e. a user-defined mapping of "3 2 1" results in a mapping of 3 2 1 4 5 6 ...
 *
 * Invalid button mappings revert to the default.
 *
 * Note that index 0 is unused, button 0 does not exist.
 * This mapping is initialised for all devices, but only applied if the device
 * has buttons (in EvdevAddButtonClass).
 */
static void
EvdevInitButtonMapping(InputInfoPtr pInfo)
{
    int         i, nbuttons     = 1;
    char       *mapping         = NULL;
    EvdevPtr    pEvdev          = pInfo->private;

    /* Check for user-defined button mapping */
    if ((mapping = xf86CheckStrOption(pInfo->options, "ButtonMapping", NULL)))
    {
        char    *map, *s = NULL;
        int     btn = 0;

        xf86IDrvMsg(pInfo, X_CONFIG, "ButtonMapping '%s'\n", mapping);
        map = mapping;
        do
        {
            btn = strtol(map, &s, 10);

            if (s == map || btn < 0 || btn > EVDEV_MAXBUTTONS)
            {
                xf86IDrvMsg(pInfo, X_ERROR,
                            "... Invalid button mapping. Using defaults\n");
                nbuttons = 1; /* ensure defaults start at 1 */
                break;
            }

            pEvdev->btnmap[nbuttons++] = btn;
            map = s;
        } while (s && *s != '\0' && nbuttons < EVDEV_MAXBUTTONS);
        free(mapping);
    }

    for (i = nbuttons; i < ArrayLength(pEvdev->btnmap); i++)
        pEvdev->btnmap[i] = i;

}

static void
EvdevInitAnyValuators(DeviceIntPtr device, EvdevPtr pEvdev)
{
    InputInfoPtr pInfo = device->public.devicePrivate;
    int rel_success = FALSE;

    if (pEvdev->flags & EVDEV_RELATIVE_EVENTS &&
        EvdevAddRelValuatorClass(device) == Success)
    {
        rel_success = TRUE;
        xf86IDrvMsg(pInfo, X_INFO, "initialized for relative axes.\n");
    }
    if (pEvdev->flags & EVDEV_ABSOLUTE_EVENTS &&
        EvdevAddAbsValuatorClass(device, !rel_success) == Success)
        xf86IDrvMsg(pInfo, X_INFO, "initialized for absolute axes.\n");
}

static void
EvdevInitAbsValuators(DeviceIntPtr device, EvdevPtr pEvdev)
{
    InputInfoPtr pInfo = device->public.devicePrivate;

    if (EvdevAddAbsValuatorClass(device, TRUE) == Success) {
        xf86IDrvMsg(pInfo, X_INFO,"initialized for absolute axes.\n");
    } else {
        xf86IDrvMsg(pInfo, X_ERROR,"failed to initialize for absolute axes.\n");
        pEvdev->flags &= ~EVDEV_ABSOLUTE_EVENTS;
    }
}

static void
EvdevInitRelValuators(DeviceIntPtr device, EvdevPtr pEvdev)
{
    InputInfoPtr pInfo = device->public.devicePrivate;
    int has_abs_axes = pEvdev->flags & EVDEV_ABSOLUTE_EVENTS;

    if (EvdevAddRelValuatorClass(device) == Success) {

        xf86IDrvMsg(pInfo, X_INFO,"initialized for relative axes.\n");

        if (has_abs_axes) {
            xf86IDrvMsg(pInfo, X_WARNING,"ignoring absolute axes.\n");
            pEvdev->flags &= ~EVDEV_ABSOLUTE_EVENTS;
        }

    } else {
        xf86IDrvMsg(pInfo, X_ERROR,"failed to initialize for relative axes.\n");

        pEvdev->flags &= ~EVDEV_RELATIVE_EVENTS;

        if (has_abs_axes)
            EvdevInitAbsValuators(device, pEvdev);
    }
}

static void
EvdevInitTouchDevice(DeviceIntPtr device, EvdevPtr pEvdev)
{
    InputInfoPtr pInfo = device->public.devicePrivate;

    if (pEvdev->flags & EVDEV_RELATIVE_EVENTS) {
        xf86IDrvMsg(pInfo, X_WARNING, "touchpads, tablets and touchscreens "
                    "ignore relative axes.\n");
        pEvdev->flags &= ~EVDEV_RELATIVE_EVENTS;
    }

    EvdevInitAbsValuators(device, pEvdev);
}

static int
EvdevInit(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (pEvdev->flags & EVDEV_KEYBOARD_EVENTS)
	EvdevAddKeyClass(device);
    if (pEvdev->flags & EVDEV_BUTTON_EVENTS)
	EvdevAddButtonClass(device);

    /* We don't allow relative and absolute axes on the same device. The
     * reason is that some devices (MS Optical Desktop 2000) register both
     * rel and abs axes for x/y.
     *
     * The abs axes register min/max; this min/max then also applies to the
     * relative device (the mouse) and caps it at 0..255 for both axes.
     * So, unless you have a small screen, you won't be enjoying it much;
     * consequently, absolute axes are generally ignored.
     *
     * However, currenly only a device with absolute axes can be registered
     * as a touch{pad,screen}. Thus, given such a device, absolute axes are
     * used and relative axes are ignored.
     */

    if (pEvdev->flags & (EVDEV_UNIGNORE_RELATIVE | EVDEV_UNIGNORE_ABSOLUTE))
        EvdevInitAnyValuators(device, pEvdev);
    else if (pEvdev->flags & (EVDEV_TOUCHPAD | EVDEV_TOUCHSCREEN | EVDEV_TABLET))
        EvdevInitTouchDevice(device, pEvdev);
    else if (pEvdev->flags & EVDEV_RELATIVE_EVENTS)
        EvdevInitRelValuators(device, pEvdev);
    else if (pEvdev->flags & EVDEV_ABSOLUTE_EVENTS)
        EvdevInitAbsValuators(device, pEvdev);

    /* We drop the return value, the only time we ever want the handlers to
     * unregister is when the device dies. In which case we don't have to
     * unregister anyway */
    EvdevInitProperty(device);
    XIRegisterPropertyHandler(device, EvdevSetProperty, NULL, NULL);
    EvdevMBEmuInitProperty(device);
    Evdev3BEmuInitProperty(device);
    EvdevWheelEmuInitProperty(device);
    EvdevDragLockInitProperty(device);
    EvdevAppleInitProperty(device);

    return Success;
}

/**
 * Init all extras (wheel emulation, etc.) and grab the device.
 */
static int
EvdevOn(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int rc = Success;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;
    /* after PreInit fd is still open */
    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        return rc;

    EvdevGrabDevice(pInfo, 1, 0);

    xf86FlushInput(pInfo->fd);
    xf86AddEnabledDevice(pInfo);
    EvdevMBEmuOn(pInfo);
    Evdev3BEmuOn(pInfo);
    pEvdev->flags |= EVDEV_INITIALIZED;
    device->public.on = TRUE;

    return Success;
}


static int
EvdevProc(DeviceIntPtr device, int what)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    switch (what)
    {
    case DEVICE_INIT:
	return EvdevInit(device);

    case DEVICE_ON:
        return EvdevOn(device);

    case DEVICE_OFF:
        if (pEvdev->flags & EVDEV_INITIALIZED)
        {
            EvdevMBEmuFinalize(pInfo);
            Evdev3BEmuFinalize(pInfo);
        }
        if (pInfo->fd != -1)
        {
            EvdevGrabDevice(pInfo, 0, 1);
            xf86RemoveEnabledDevice(pInfo);
            EvdevCloseDevice(pInfo);
        }
        pEvdev->min_maj = 0;
        pEvdev->flags &= ~EVDEV_INITIALIZED;
	device->public.on = FALSE;
	break;

    case DEVICE_CLOSE:
	xf86IDrvMsg(pInfo, X_INFO, "Close\n");
        EvdevCloseDevice(pInfo);
        EvdevFreeMasks(pEvdev);
        pEvdev->min_maj = 0;
	break;

    default:
        return BadValue;
    }

    return Success;
}

/**
 * Get as much information as we can from the fd and cache it.
 *
 * @return Success if the information was cached, or !Success otherwise.
 */
static int
EvdevCache(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    int i;

    /*
     * Do not try to validate absinfo data since it is not expected
     * to be static, always refresh it in evdev structure.
     */
    for (i = ABS_X; i <= ABS_MAX; i++) {
        if (libevdev_has_event_code(pEvdev->dev, EV_ABS, i)) {
            const struct input_absinfo *abs = libevdev_get_abs_info(pEvdev->dev, i);
            xf86IDrvMsgVerb(pInfo, X_PROBED, 6, "absolute axis %#x [%d..%d]\n",
                            i, abs->minimum, abs->maximum);
        }
    }

    return Success;
}

/**
 * Issue an EVIOCGRAB on the device file, either as a grab or to ungrab, or
 * both. Return TRUE on success, otherwise FALSE. Failing the release is a
 * still considered a success, because it's not as if you could do anything
 * about it.
 */
static BOOL
EvdevGrabDevice(InputInfoPtr pInfo, int grab, int ungrab)
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->grabDevice)
    {
        int rc;
        if (grab && (rc = libevdev_grab(pEvdev->dev, LIBEVDEV_GRAB)) < 0) {
            xf86IDrvMsg(pInfo, X_WARNING, "Grab failed (%s)\n",
                        strerror(-rc));
            return FALSE;
        } else if (ungrab && (rc = libevdev_grab(pEvdev->dev, LIBEVDEV_UNGRAB)) < 0)
            xf86IDrvMsg(pInfo, X_WARNING, "Release failed (%s)\n",
                        strerror(-rc));
    }

    return TRUE;
}

/**
 * Some devices only have other axes (e.g. wheels), but we
 * still need x/y for these. The server relies on devices having
 * x/y as axes 0/1 and core/XI 1.x clients expect it too (#44655)
 */
static void
EvdevForceXY(InputInfoPtr pInfo, int mode)
{
    EvdevPtr pEvdev = pInfo->private;

    xf86IDrvMsg(pInfo, X_INFO, "Forcing %s x/y axes to exist.\n",
                (mode == Relative) ? "relative" : "absolute");

    if (mode == Relative)
    {
        libevdev_enable_event_code(pEvdev->dev, EV_REL, REL_X, NULL);
        libevdev_enable_event_code(pEvdev->dev, EV_REL, REL_Y, NULL);
    } else if (mode == Absolute)
    {
        struct input_absinfo abs;

        abs.minimum = 0;
        abs.maximum = 1000;
        abs.value = 0;
        abs.fuzz = 0;
        abs.flat = 0;
        abs.resolution = 0;

        libevdev_enable_event_code(pEvdev->dev, EV_ABS, ABS_X, &abs);
        libevdev_enable_event_code(pEvdev->dev, EV_ABS, ABS_Y, &abs);
    }
}

static int
EvdevProbe(InputInfoPtr pInfo)
{
    int i, has_rel_axes, has_abs_axes, has_keys, num_buttons, has_scroll;
    int has_lmr; /* left middle right */
    int has_mt; /* multitouch */
    int ignore_abs = 0, ignore_rel = 0;
    EvdevPtr pEvdev = pInfo->private;
    int rc = 1;

    xf86IDrvMsg(pInfo, X_PROBED, "Vendor %#hx Product %#hx\n",
                libevdev_get_id_vendor(pEvdev->dev),
                libevdev_get_id_product(pEvdev->dev));

    /* Trinary state for ignoring axes:
       - unset: do the normal thing.
       - TRUE: explicitly ignore them.
       - FALSE: unignore axes, use them at all cost if they're present.
     */
    if (xf86FindOption(pInfo->options, "IgnoreRelativeAxes"))
    {
        if (xf86SetBoolOption(pInfo->options, "IgnoreRelativeAxes", FALSE))
            ignore_rel = TRUE;
        else
            pEvdev->flags |= EVDEV_UNIGNORE_RELATIVE;

    }
    if (xf86FindOption(pInfo->options, "IgnoreAbsoluteAxes"))
    {
        if (xf86SetBoolOption(pInfo->options, "IgnoreAbsoluteAxes", FALSE))
           ignore_abs = TRUE;
        else
            pEvdev->flags |= EVDEV_UNIGNORE_ABSOLUTE;
    }

    has_rel_axes = FALSE;
    has_abs_axes = FALSE;
    has_keys = FALSE;
    has_scroll = FALSE;
    has_lmr = FALSE;
    has_mt = FALSE;
    num_buttons = 0;

    /* count all buttons */
    for (i = BTN_MISC; i < BTN_JOYSTICK; i++)
    {
        int mapping = 0;
        if (libevdev_has_event_code(pEvdev->dev, EV_KEY, i))
        {
            mapping = EvdevUtilButtonEventToButtonNumber(pEvdev, i);
            if (mapping > num_buttons)
                num_buttons = mapping;
        }
    }

    has_lmr = libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_LEFT) ||
              libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_MIDDLE) ||
              libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_RIGHT);

    if (num_buttons)
    {
        pEvdev->flags |= EVDEV_BUTTON_EVENTS;
        pEvdev->num_buttons = num_buttons;
        xf86IDrvMsg(pInfo, X_PROBED, "Found %d mouse buttons\n", num_buttons);
    }

    for (i = 0; i < REL_MAX; i++) {
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, i)) {
            has_rel_axes = TRUE;
            break;
        }
    }

    if (has_rel_axes) {
        if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_WHEEL) ||
            libevdev_has_event_code(pEvdev->dev, EV_REL, REL_HWHEEL) ||
            libevdev_has_event_code(pEvdev->dev, EV_REL, REL_DIAL)) {
            xf86IDrvMsg(pInfo, X_PROBED, "Found scroll wheel(s)\n");
            has_scroll = TRUE;
            if (!num_buttons)
                xf86IDrvMsg(pInfo, X_INFO,
                            "Forcing buttons for scroll wheel(s)\n");
            num_buttons = (num_buttons < 3) ? 7 : num_buttons + 4;
            pEvdev->num_buttons = num_buttons;
        }

        if (!ignore_rel)
        {
            xf86IDrvMsg(pInfo, X_PROBED, "Found relative axes\n");
            pEvdev->flags |= EVDEV_RELATIVE_EVENTS;

            if (libevdev_has_event_code(pEvdev->dev, EV_REL, REL_X) &&
                libevdev_has_event_code(pEvdev->dev, EV_REL, REL_Y)) {
                xf86IDrvMsg(pInfo, X_PROBED, "Found x and y relative axes\n");
            } else if (!libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_X) ||
                       !libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_Y))
                EvdevForceXY(pInfo, Relative);
        } else {
            xf86IDrvMsg(pInfo, X_INFO, "Relative axes present but ignored.\n");
            has_rel_axes = FALSE;
        }
    }

    for (i = 0; i < ABS_MAX; i++) {
        if (libevdev_has_event_code(pEvdev->dev, EV_ABS, i)) {
            has_abs_axes = TRUE;
            break;
        }
    }

#ifdef MULTITOUCH
    for (i = ABS_MT_SLOT; i < ABS_MAX; i++) {
        if (libevdev_has_event_code(pEvdev->dev, EV_ABS, i)) {
            has_mt = TRUE;
            break;
        }
    }
#endif

    if (ignore_abs && has_abs_axes)
    {
        xf86IDrvMsg(pInfo, X_INFO, "Absolute axes present but ignored.\n");
        has_abs_axes = FALSE;
    } else if (has_abs_axes) {
        xf86IDrvMsg(pInfo, X_PROBED, "Found absolute axes\n");
        pEvdev->flags |= EVDEV_ABSOLUTE_EVENTS;

        if (has_mt) {
            xf86IDrvMsg(pInfo, X_PROBED, "Found absolute multitouch axes\n");
            if (num_buttons == 0) {
                if (libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_JOYSTICK)) {
                    xf86IDrvMsg(pInfo, X_INFO, "Device is a Joystick with MT without buttons. Ignoring it.\n");
                    goto out;
                } else {
                    xf86IDrvMsg(pInfo, X_INFO, "No buttons found, faking one.\n");
                    num_buttons = 1;
                    pEvdev->num_buttons = num_buttons;
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
                }
            }
        }

        if ((libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_X) &&
             libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_Y))) {
            xf86IDrvMsg(pInfo, X_PROBED, "Found x and y absolute axes\n");
            if (libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_TOOL_PEN) ||
                libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_STYLUS) ||
                libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_STYLUS2))
            {
                xf86IDrvMsg(pInfo, X_PROBED, "Found absolute tablet.\n");
                pEvdev->flags |= EVDEV_TABLET;
                if (!pEvdev->num_buttons)
                {
                    pEvdev->num_buttons = 7; /* LMR + scroll wheels */
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
                }
            } else if (libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_PRESSURE) ||
                       libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_TOUCH)) {
                if (has_lmr || libevdev_has_event_code(pEvdev->dev, EV_KEY, BTN_TOOL_FINGER)) {
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchpad.\n");
                    pEvdev->flags |= EVDEV_TOUCHPAD;
                } else {
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchscreen\n");
                    pEvdev->flags |= EVDEV_TOUCHSCREEN;
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
                }
            } else if (!(libevdev_has_event_code(pEvdev->dev, EV_REL, REL_X) &&
                         libevdev_has_event_code(pEvdev->dev, EV_REL, REL_Y)) && has_lmr) {
                    /* some touchscreens use BTN_LEFT rather than BTN_TOUCH */
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchscreen\n");
                    pEvdev->flags |= EVDEV_TOUCHSCREEN;
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
            }
        } else {
#ifdef MULTITOUCH
            if (!libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_POSITION_X) ||
                !libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_POSITION_Y))
#endif
                EvdevForceXY(pInfo, Absolute);
        }



    }

    for (i = 0; i < BTN_MISC; i++) {
        if (libevdev_has_event_code(pEvdev->dev, EV_KEY, i)) {
            xf86IDrvMsg(pInfo, X_PROBED, "Found keys\n");
            pEvdev->flags |= EVDEV_KEYBOARD_EVENTS;
            has_keys = TRUE;
            break;
        }
    }

    if (has_rel_axes || has_abs_axes)
    {
        char *str;
        int num_calibration = 0, calibration[4] = { 0, 0, 0, 0 };

        pEvdev->invert_x = xf86SetBoolOption(pInfo->options, "InvertX", FALSE);
        pEvdev->invert_y = xf86SetBoolOption(pInfo->options, "InvertY", FALSE);
        pEvdev->swap_axes = xf86SetBoolOption(pInfo->options, "SwapAxes", FALSE);

        str = xf86CheckStrOption(pInfo->options, "Calibration", NULL);
        if (str) {
            num_calibration = sscanf(str, "%d %d %d %d",
                    &calibration[0], &calibration[1],
                    &calibration[2], &calibration[3]);
            free(str);
            if (num_calibration == 4)
                EvdevSetCalibration(pInfo, num_calibration, calibration);
            else
                xf86IDrvMsg(pInfo, X_ERROR,
                            "Insufficient calibration factors (%d). Ignoring calibration\n",
                            num_calibration);
        }
    }

    if (has_rel_axes || has_abs_axes || num_buttons) {
        pInfo->flags |= XI86_SEND_DRAG_EVENTS;
	if (pEvdev->flags & EVDEV_TOUCHPAD) {
	    xf86IDrvMsg(pInfo, X_INFO, "Configuring as touchpad\n");
	    pInfo->type_name = XI_TOUCHPAD;
	    pEvdev->use_proximity = 0;
	} else if (pEvdev->flags & EVDEV_TABLET) {
	    xf86IDrvMsg(pInfo, X_INFO, "Configuring as tablet\n");
	    pInfo->type_name = XI_TABLET;
        } else if (pEvdev->flags & EVDEV_TOUCHSCREEN) {
            xf86IDrvMsg(pInfo, X_INFO, "Configuring as touchscreen\n");
            pInfo->type_name = XI_TOUCHSCREEN;
	} else {
            if (!libevdev_has_event_code(pEvdev->dev, EV_REL, REL_X) ||
                !libevdev_has_event_code(pEvdev->dev, EV_REL, REL_Y))
                EvdevForceXY(pInfo, Relative);
	    xf86IDrvMsg(pInfo, X_INFO, "Configuring as mouse\n");
	    pInfo->type_name = XI_MOUSE;
	}

        rc = 0;
    }

    if (has_keys) {
        xf86IDrvMsg(pInfo, X_INFO, "Configuring as keyboard\n");
        pInfo->type_name = XI_KEYBOARD;
        rc = 0;
    }

    if (has_scroll &&
        (has_rel_axes || has_abs_axes || num_buttons || has_keys))
    {
        xf86IDrvMsg(pInfo, X_INFO, "Adding scrollwheel support\n");
        pEvdev->flags |= EVDEV_BUTTON_EVENTS;
        pEvdev->flags |= EVDEV_RELATIVE_EVENTS;

#ifdef HAVE_SMOOTH_SCROLLING
        pEvdev->smoothScroll.vert_delta =
            xf86SetIntOption(pInfo->options, "VertScrollDelta", 1);
        pEvdev->smoothScroll.horiz_delta =
            xf86SetIntOption(pInfo->options, "HorizScrollDelta", 1);
        pEvdev->smoothScroll.dial_delta =
            xf86SetIntOption(pInfo->options, "DialDelta", 1);
#endif
    }

out:
    if (rc)
        xf86IDrvMsg(pInfo, X_WARNING, "Don't know how to use device\n");

    return rc;
}

static void
EvdevSetCalibration(InputInfoPtr pInfo, int num_calibration, int calibration[4])
{
    EvdevPtr pEvdev = pInfo->private;

    if (num_calibration == 0) {
        pEvdev->flags &= ~EVDEV_CALIBRATED;
        pEvdev->calibration.min_x = 0;
        pEvdev->calibration.max_x = 0;
        pEvdev->calibration.min_y = 0;
        pEvdev->calibration.max_y = 0;
    } else if (num_calibration == 4) {
        pEvdev->flags |= EVDEV_CALIBRATED;
        pEvdev->calibration.min_x = calibration[0];
        pEvdev->calibration.max_x = calibration[1];
        pEvdev->calibration.min_y = calibration[2];
        pEvdev->calibration.max_y = calibration[3];
    }
}

#ifdef MULTITOUCH
/**
 * Open an mtdev device for this device. mtdev is a bit too generous with
 * memory usage, so only do so for multitouch protocol A devices.
 *
 * @return FALSE on error, TRUE if mtdev was initiated or the device doesn't
 * need it
 */
static Bool
EvdevOpenMTDev(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;

    if (pEvdev->mtdev) {
        pEvdev->cur_slot = pEvdev->mtdev->caps.slot.value;
        return TRUE;
    } else if (libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_SLOT)) {
        pEvdev->cur_slot = libevdev_get_current_slot(pEvdev->dev);
        return TRUE;
    }

    if (pInfo->fd < 0) {
        xf86Msg(X_ERROR, "%s: Bug. fd < 0\n", pInfo->name);
        return FALSE;
    }

    if (!libevdev_has_event_type(pEvdev->dev, EV_ABS))
        return TRUE;

    /* don't need mtdev for protocol B devices */
    if (libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_SLOT))
        return TRUE;

    if (!libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_POSITION_X) ||
        !libevdev_has_event_code(pEvdev->dev, EV_ABS, ABS_MT_POSITION_Y))
        return TRUE;

    xf86IDrvMsg(pInfo, X_INFO, "Using mtdev for this device\n");
    pEvdev->mtdev = mtdev_new_open(pInfo->fd);
    if (pEvdev->mtdev)
        pEvdev->cur_slot = pEvdev->mtdev->caps.slot.value;
    else {
        xf86Msg(X_ERROR, "%s: Couldn't open mtdev device\n", pInfo->name);
        EvdevCloseDevice(pInfo);
        return FALSE;
    }

    return TRUE;
}
#endif

static int
EvdevOpenDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    char *device = pEvdev->device;

    if (!device)
    {
        device = xf86CheckStrOption(pInfo->options, "Device", NULL);
        if (!device) {
            xf86IDrvMsg(pInfo, X_ERROR, "No device specified.\n");
            return BadValue;
        }

        pEvdev->device = device;
        xf86IDrvMsg(pInfo, X_CONFIG, "Device: \"%s\"\n", device);
    }

    if (!(pInfo->flags & XI86_SERVER_FD) && pInfo->fd < 0)
    {
        do {
            pInfo->fd = open(device, O_RDWR | O_NONBLOCK, 0);
        } while (pInfo->fd < 0 && errno == EINTR);
    }

    if (pInfo->fd < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "Unable to open evdev device \"%s\".\n", device);
        return BadValue;
    }

    if (libevdev_get_fd(pEvdev->dev) != -1) {
        struct input_event ev;

        libevdev_change_fd(pEvdev->dev, pInfo->fd);
        /* re-sync libevdev's view of the device, but
           we don't care about the actual events here */
        libevdev_next_event(pEvdev->dev, LIBEVDEV_READ_FLAG_FORCE_SYNC, &ev);
        while (libevdev_next_event(pEvdev->dev, LIBEVDEV_READ_FLAG_SYNC, &ev) == LIBEVDEV_READ_STATUS_SYNC)
            ;
    } else {
        int rc = libevdev_set_fd(pEvdev->dev, pInfo->fd);
        if (rc < 0) {
            xf86IDrvMsg(pInfo, X_ERROR, "Unable to query fd: %s\n", strerror(-rc));
            return BadValue;
        }
    }

    /* Check major/minor of device node to avoid adding duplicate devices. */
    pEvdev->min_maj = EvdevGetMajorMinor(pInfo);
    if (EvdevIsDuplicate(pInfo))
    {
        xf86IDrvMsg(pInfo, X_WARNING, "device file is duplicate. Ignoring.\n");
        EvdevCloseDevice(pInfo);
        return BadMatch;
    }

#ifdef MULTITOUCH
    if (!EvdevOpenMTDev(pInfo)) {
        xf86Msg(X_ERROR, "%s: Couldn't open mtdev device\n", pInfo->name);
        EvdevCloseDevice(pInfo);
        return BadValue;
    }
#endif

    return Success;
}

static void
EvdevCloseDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    if (!(pInfo->flags & XI86_SERVER_FD) && pInfo->fd >= 0)
    {
        close(pInfo->fd);
        pInfo->fd = -1;
    }

#ifdef MULTITOUCH
    if (pEvdev->mtdev)
    {
        mtdev_close_delete(pEvdev->mtdev);
        pEvdev->mtdev = NULL;
    }
#endif

}


static void
EvdevUnInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
    EvdevPtr pEvdev = pInfo ? pInfo->private : NULL;
    if (pEvdev)
    {
        /* Release string allocated in EvdevOpenDevice. */
        free(pEvdev->device);
        pEvdev->device = NULL;

        free(pEvdev->type_name);
        pEvdev->type_name = NULL;

        libevdev_free(pEvdev->dev);
    }
    xf86DeleteInput(pInfo, flags);
}

static EvdevPtr
EvdevAlloc(InputInfoPtr pInfo)
{
    int i;
    EvdevPtr pEvdev = calloc(sizeof(EvdevRec), 1);

    if (!pEvdev)
        return NULL;

    pEvdev->dev = libevdev_new();
    if (!pEvdev->dev) {
        free(pEvdev);
        return NULL;
    }

    /*
     * We initialize pEvdev->in_proximity to 1 so that device that doesn't use
     * proximity will still report events.
     */
    pEvdev->in_proximity = 1;
    pEvdev->use_proximity = 1;

#ifdef MULTITOUCH
    pEvdev->cur_slot = -1;
#endif

    for (i = 0; i < ArrayLength(pEvdev->rel_axis_map); i++)
        pEvdev->rel_axis_map[i] = -1;
    for (i = 0; i < ArrayLength(pEvdev->abs_axis_map); i++)
        pEvdev->abs_axis_map[i] = -1;

    pEvdev->rel_axis_map[0] = 0;
    pEvdev->rel_axis_map[1] = 1;

    pEvdev->type_name = NULL;

    return pEvdev;
}

static int
EvdevPreInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
    EvdevPtr pEvdev;
    int rc = BadAlloc;

    if (!(pEvdev = EvdevAlloc(pInfo)))
        goto error;

    pInfo->private = pEvdev;
    pInfo->type_name = "UNKNOWN";
    pInfo->device_control = EvdevProc;
    pInfo->read_input = EvdevReadInput;
    pInfo->switch_mode = EvdevSwitchMode;

    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        goto error;

    /* Grabbing the event device stops in-kernel event forwarding. In other
       words, it disables rfkill and the "Macintosh mouse button emulation".
       Note that this needs a server that sets the console to RAW mode. */
    pEvdev->grabDevice = xf86CheckBoolOption(pInfo->options, "GrabDevice", 0);

    /* If grabDevice is set, ungrab immediately since we only want to grab
     * between DEVICE_ON and DEVICE_OFF. If we never get DEVICE_ON, don't
     * hold a grab. */

    if (!EvdevGrabDevice(pInfo, 1, 1))
    {
        xf86IDrvMsg(pInfo, X_WARNING, "Device may already be configured.\n");
        rc = BadMatch;
        goto error;
    }

    EvdevInitButtonMapping(pInfo);

    if (EvdevCache(pInfo) || EvdevProbe(pInfo)) {
        rc = BadMatch;
        goto error;
    }

    /* Overwrite type_name with custom-defined one (#62831).
       Note: pInfo->type_name isn't freed so we need to manually do this
     */
    pEvdev->type_name = xf86SetStrOption(pInfo->options,
                                         "TypeName",
                                         pInfo->type_name);
    pInfo->type_name = pEvdev->type_name;

    if (pEvdev->flags & EVDEV_BUTTON_EVENTS)
    {
        EvdevMBEmuPreInit(pInfo);
        Evdev3BEmuPreInit(pInfo);
        EvdevWheelEmuPreInit(pInfo);
        EvdevDragLockPreInit(pInfo);
    }

    return Success;

error:
    EvdevCloseDevice(pInfo);
    return rc;
}

_X_EXPORT InputDriverRec EVDEV = {
    1,
    "evdev",
    NULL,
    EvdevPreInit,
    EvdevUnInit,
    NULL,
    evdevDefaults,
#ifdef XI86_DRV_CAP_SERVER_FD
    XI86_DRV_CAP_SERVER_FD
#endif
};

static void
EvdevUnplug(pointer	p)
{
}

static pointer
EvdevPlug(pointer	module,
          pointer	options,
          int		*errmaj,
          int		*errmin)
{
    xf86AddInputDriver(&EVDEV, module, 0);
    return module;
}

static XF86ModuleVersionInfo EvdevVersionRec =
{
    "evdev",
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    PACKAGE_VERSION_MAJOR, PACKAGE_VERSION_MINOR, PACKAGE_VERSION_PATCHLEVEL,
    ABI_CLASS_XINPUT,
    ABI_XINPUT_VERSION,
    MOD_CLASS_XINPUT,
    {0, 0, 0, 0}
};

_X_EXPORT XF86ModuleData evdevModuleData =
{
    &EvdevVersionRec,
    EvdevPlug,
    EvdevUnplug
};


/* Return an index value for a given button event code
 * returns 0 on non-button event.
 */
unsigned int
EvdevUtilButtonEventToButtonNumber(EvdevPtr pEvdev, int code)
{
    switch (code)
    {
        /* Mouse buttons */
        case BTN_LEFT:
            return 1;
        case BTN_MIDDLE:
            return 2;
        case BTN_RIGHT:
            return 3;
        case BTN_SIDE ... BTN_JOYSTICK - 1:
            return 8 + code - BTN_SIDE;

        /* Generic buttons */
        case BTN_0 ... BTN_2:
            return 1 + code - BTN_0;
        case BTN_3 ... BTN_MOUSE - 1:
            return 8 + code - BTN_3;

        /* Tablet stylus buttons */
        case BTN_TOUCH ... BTN_STYLUS2:
            return 1 + code - BTN_TOUCH;

        /* The rest */
        default:
            /* Ignore */
            return 0;
    }
}

static void EvdevInitOneAxisLabel(EvdevPtr pEvdev, int mapped_axis,
                                  const char **labels, int label_idx, Atom *atoms)
{
    Atom atom;

    if (mapped_axis == -1)
        return;

    atom = XIGetKnownProperty(labels[label_idx]);
    if (!atom) /* Should not happen */
        return;

    atoms[mapped_axis] = atom;
}

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms)
{
    int axis;

    memset(atoms, 0, natoms * sizeof(Atom));

    /* rel[0] and [1] are always mapped, so we get the rel labels. if we
       have abs x/y, the labels will be overwritten with the right one */
    for (axis = 0; axis < ArrayLength(rel_labels); axis++)
        EvdevInitOneAxisLabel(pEvdev, pEvdev->rel_axis_map[axis], rel_labels, axis, atoms);

    for (axis = 0; axis < ArrayLength(abs_labels); axis++)
        EvdevInitOneAxisLabel(pEvdev, pEvdev->abs_axis_map[axis], abs_labels, axis, atoms);
}

static void EvdevInitButtonLabels(EvdevPtr pEvdev, int natoms, Atom *atoms)
{
    Atom atom;
    int button, bmap;

    /* First, make sure all atoms are initialized */
    atom = XIGetKnownProperty(BTN_LABEL_PROP_BTN_UNKNOWN);
    for (button = 0; button < natoms; button++)
        atoms[button] = atom;

    for (button = BTN_MISC; button < BTN_JOYSTICK; button++)
    {
        if (libevdev_has_event_code(pEvdev->dev, EV_KEY, button))
        {
            int group = (button % 0x100)/16;
            int idx = button - ((button/16) * 16);

            if (!btn_labels[group][idx])
                continue;

            atom = XIGetKnownProperty(btn_labels[group][idx]);
            if (!atom)
                continue;

            /* Props are 0-indexed, button numbers start with 1 */
            bmap = EvdevUtilButtonEventToButtonNumber(pEvdev, button) - 1;
            atoms[bmap] = atom;
        }
    }

    /* wheel buttons, hardcoded anyway */
    if (natoms > 3)
        atoms[3] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_WHEEL_UP);
    if (natoms > 4)
        atoms[4] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_WHEEL_DOWN);
    if (natoms > 5)
        atoms[5] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_HWHEEL_LEFT);
    if (natoms > 6)
        atoms[6] = XIGetKnownProperty(BTN_LABEL_PROP_BTN_HWHEEL_RIGHT);
}

static void
EvdevInitProperty(DeviceIntPtr dev)
{
    InputInfoPtr pInfo  = dev->public.devicePrivate;
    EvdevPtr     pEvdev = pInfo->private;
    int          rc;
    char         *device_node;

    CARD32       product[2];

    prop_product_id = MakeAtom(XI_PROP_PRODUCT_ID, strlen(XI_PROP_PRODUCT_ID), TRUE);
    product[0] = libevdev_get_id_vendor(pEvdev->dev);
    product[1] = libevdev_get_id_product(pEvdev->dev);
    rc = XIChangeDeviceProperty(dev, prop_product_id, XA_INTEGER, 32,
                                PropModeReplace, 2, product, FALSE);
    if (rc != Success)
        return;

    XISetDevicePropertyDeletable(dev, prop_product_id, FALSE);

    /* Device node property */
    device_node = strdup(pEvdev->device);
    prop_device = MakeAtom(XI_PROP_DEVICE_NODE,
                           strlen(XI_PROP_DEVICE_NODE), TRUE);
    rc = XIChangeDeviceProperty(dev, prop_device, XA_STRING, 8,
                                PropModeReplace,
                                strlen(device_node), device_node,
                                FALSE);
    free(device_node);

    if (rc != Success)
        return;

    if (EvdevDeviceIsVirtual(pEvdev->device))
    {
        BOOL virtual = 1;
        prop_virtual = MakeAtom(XI_PROP_VIRTUAL_DEVICE,
                                strlen(XI_PROP_VIRTUAL_DEVICE), TRUE);
        rc = XIChangeDeviceProperty(dev, prop_virtual, XA_INTEGER, 8,
                                    PropModeReplace, 1, &virtual, FALSE);
        XISetDevicePropertyDeletable(dev, prop_virtual, FALSE);
    }


    XISetDevicePropertyDeletable(dev, prop_device, FALSE);

    if (pEvdev->flags & (EVDEV_RELATIVE_EVENTS | EVDEV_ABSOLUTE_EVENTS))
    {
        BOOL invert[2];
        invert[0] = pEvdev->invert_x;
        invert[1] = pEvdev->invert_y;

        prop_invert = MakeAtom(EVDEV_PROP_INVERT_AXES, strlen(EVDEV_PROP_INVERT_AXES), TRUE);

        rc = XIChangeDeviceProperty(dev, prop_invert, XA_INTEGER, 8,
                PropModeReplace, 2,
                invert, FALSE);
        if (rc != Success)
            return;

        XISetDevicePropertyDeletable(dev, prop_invert, FALSE);

        prop_calibration = MakeAtom(EVDEV_PROP_CALIBRATION,
                strlen(EVDEV_PROP_CALIBRATION), TRUE);
        if (pEvdev->flags & EVDEV_CALIBRATED) {
            int calibration[4];

            calibration[0] = pEvdev->calibration.min_x;
            calibration[1] = pEvdev->calibration.max_x;
            calibration[2] = pEvdev->calibration.min_y;
            calibration[3] = pEvdev->calibration.max_y;

            rc = XIChangeDeviceProperty(dev, prop_calibration, XA_INTEGER,
                    32, PropModeReplace, 4, calibration,
                    FALSE);
        } else if (pEvdev->flags & EVDEV_ABSOLUTE_EVENTS) {
            rc = XIChangeDeviceProperty(dev, prop_calibration, XA_INTEGER,
                    32, PropModeReplace, 0, NULL,
                    FALSE);
        }
        if (rc != Success)
            return;

        XISetDevicePropertyDeletable(dev, prop_calibration, FALSE);

        prop_swap = MakeAtom(EVDEV_PROP_SWAP_AXES,
                strlen(EVDEV_PROP_SWAP_AXES), TRUE);

        rc = XIChangeDeviceProperty(dev, prop_swap, XA_INTEGER, 8,
                PropModeReplace, 1, &pEvdev->swap_axes, FALSE);
        if (rc != Success)
            return;

        XISetDevicePropertyDeletable(dev, prop_swap, FALSE);

        /* Axis labelling */
        if ((pEvdev->num_vals > 0) && (prop_axis_label = XIGetKnownProperty(AXIS_LABEL_PROP)))
        {
            int mode;
            int num_axes = pEvdev->num_vals + pEvdev->num_mt_vals;
            Atom atoms[num_axes];

            if (pEvdev->flags & EVDEV_ABSOLUTE_EVENTS)
                mode = Absolute;
            else if (pEvdev->flags & EVDEV_RELATIVE_EVENTS)
                mode = Relative;
            else {
                xf86IDrvMsg(pInfo, X_ERROR, "BUG: mode is neither absolute nor relative\n");
                mode = Absolute;
            }

            EvdevInitAxesLabels(pEvdev, mode, num_axes, atoms);
            XIChangeDeviceProperty(dev, prop_axis_label, XA_ATOM, 32,
                                   PropModeReplace, num_axes, atoms, FALSE);
            XISetDevicePropertyDeletable(dev, prop_axis_label, FALSE);
        }
        /* Button labelling */
        if ((pEvdev->num_buttons > 0) && (prop_btn_label = XIGetKnownProperty(BTN_LABEL_PROP)))
        {
            Atom atoms[EVDEV_MAXBUTTONS];
            EvdevInitButtonLabels(pEvdev, EVDEV_MAXBUTTONS, atoms);
            XIChangeDeviceProperty(dev, prop_btn_label, XA_ATOM, 32,
                                   PropModeReplace, pEvdev->num_buttons, atoms, FALSE);
            XISetDevicePropertyDeletable(dev, prop_btn_label, FALSE);
        }

#ifdef HAVE_SMOOTH_SCROLLING
        {
            int smooth_scroll_values[3] = {
                pEvdev->smoothScroll.vert_delta,
                pEvdev->smoothScroll.horiz_delta,
                pEvdev->smoothScroll.dial_delta
            };
            prop_scroll_dist = MakeAtom(EVDEV_PROP_SCROLL_DISTANCE,
                                        strlen(EVDEV_PROP_SCROLL_DISTANCE), TRUE);
            XIChangeDeviceProperty(dev, prop_scroll_dist, XA_INTEGER, 32,
                                   PropModeReplace, 3, smooth_scroll_values, FALSE);
            XISetDevicePropertyDeletable(dev, prop_scroll_dist, FALSE);
        }
#endif

    }

}

static int
EvdevSetProperty(DeviceIntPtr dev, Atom atom, XIPropertyValuePtr val,
                 BOOL checkonly)
{
    InputInfoPtr pInfo  = dev->public.devicePrivate;
    EvdevPtr     pEvdev = pInfo->private;

    if (atom == prop_invert)
    {
        BOOL* data;
        if (val->format != 8 || val->size != 2 || val->type != XA_INTEGER)
            return BadMatch;

        if (!checkonly)
        {
            data = (BOOL*)val->data;
            pEvdev->invert_x = data[0];
            pEvdev->invert_y = data[1];
        }
    } else if (atom == prop_calibration)
    {
        if (val->format != 32 || val->type != XA_INTEGER)
            return BadMatch;
        if (val->size != 4 && val->size != 0)
            return BadMatch;

        if (!checkonly)
            EvdevSetCalibration(pInfo, val->size, val->data);
    } else if (atom == prop_swap)
    {
        if (val->format != 8 || val->type != XA_INTEGER || val->size != 1)
            return BadMatch;

        if (!checkonly)
            pEvdev->swap_axes = *((BOOL*)val->data);
    } else if (atom == prop_scroll_dist)
    {
        if (val->format != 32 || val->type != XA_INTEGER || val->size != 3)
            return BadMatch;

        if (!checkonly) {
            int *data = (int *)val->data;
            pEvdev->smoothScroll.vert_delta = data[0];
            pEvdev->smoothScroll.horiz_delta = data[1];
            pEvdev->smoothScroll.dial_delta = data[2];
            EvdevSetScrollValuators(dev);
        }
    } else if (atom == prop_axis_label || atom == prop_btn_label ||
               atom == prop_product_id || atom == prop_device ||
               atom == prop_virtual)
        return BadAccess; /* Read-only properties */

    return Success;
}
