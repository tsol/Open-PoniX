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

#ifndef XI_PROP_PRODUCT_ID
#define XI_PROP_PRODUCT_ID "Device Product ID"
#endif

#ifndef XI_PROP_VIRTUAL_DEVICE
#define XI_PROP_VIRTUAL_DEVICE "Virtual Device"
#endif

/* removed from server, purge when dropping support for server 1.10 */
#define XI86_SEND_DRAG_EVENTS   0x08

#ifndef MAXDEVICES
#include <inputstr.h> /* for MAX_DEVICES */
#define MAXDEVICES MAX_DEVICES
#endif

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

static const char *evdevDefaults[] = {
    "XkbRules",     "evdev",
    "XkbModel",     "evdev",
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
static void EvdevInitOneAxisLabel(EvdevPtr pEvdev, int axis,
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

/* All devices the evdev driver has allocated and knows about.
 * MAXDEVICES is safe as null-terminated array, as two devices (VCP and VCK)
 * cannot be used by evdev, leaving us with a space of 2 at the end. */
static EvdevPtr evdev_devices[MAXDEVICES] = {NULL};

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

static size_t EvdevCountBits(unsigned long *array, size_t nlongs)
{
    unsigned int i;
    size_t count = 0;

    for (i = 0; i < nlongs; i++) {
        unsigned long x = array[i];

        while (x > 0)
        {
            count += (x & 0x1);
            x >>= 1;
        }
    }
    return count;
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
    EvdevPtr* dev   = evdev_devices;

    if (pEvdev->min_maj)
    {
        while(*dev)
        {
            if ((*dev) != pEvdev &&
                (*dev)->min_maj &&
                (*dev)->min_maj == pEvdev->min_maj)
                return TRUE;
            dev++;
        }
    }
    return FALSE;
}

/**
 * Add to internal device list.
 */
static void
EvdevAddDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    EvdevPtr* dev = evdev_devices;

    while(*dev)
        dev++;

    *dev = pEvdev;
}

/**
 * Remove from internal device list.
 */
static void
EvdevRemoveDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    EvdevPtr *dev   = evdev_devices;
    int count       = 0;

    while(*dev)
    {
        count++;
        if (*dev == pEvdev)
        {
            memmove(dev, dev + 1,
                    sizeof(evdev_devices) - (count * sizeof(EvdevPtr)));
            break;
        }
        dev++;
    }
}


static void
SetXkbOption(InputInfoPtr pInfo, const char *name, char **option)
{
    char *s;

    if ((s = xf86SetStrOption(pInfo->options, name, NULL))) {
        if (!s[0]) {
            free(s);
            *option = NULL;
        } else {
            *option = s;
        }
    }
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

    stat(devicenode, &st);
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

/**
 * Take the valuators and process them accordingly.
 */
static void
EvdevProcessValuators(InputInfoPtr pInfo)
{
    int tmp;
    EvdevPtr pEvdev = pInfo->private;
    int *delta = pEvdev->delta;

    /* convert to relative motion for touchpads */
    if (pEvdev->abs_queued && (pEvdev->flags & EVDEV_RELATIVE_MODE)) {
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
    }

    if (pEvdev->rel_queued) {
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
            int map = pEvdev->axis_map[i];
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
        int i;

        if (pEvdev->swap_axes) {
            int swapped_isset[2] = {0, 0};
            int swapped_values[2];

            for(i = 0; i <= 1; i++)
                if (valuator_mask_isset(pEvdev->vals, i)) {
                    swapped_isset[1 - i] = 1;
                    swapped_values[1 - i] =
                        xf86ScaleAxis(valuator_mask_get(pEvdev->vals, i),
                                      pEvdev->absinfo[1 - i].maximum,
                                      pEvdev->absinfo[1 - i].minimum,
                                      pEvdev->absinfo[i].maximum,
                                      pEvdev->absinfo[i].minimum);
                }

            for (i = 0; i <= 1; i++)
                if (swapped_isset[i])
                    valuator_mask_set(pEvdev->vals, i, swapped_values[i]);
                else
                    valuator_mask_unset(pEvdev->vals, i);
        }

        for (i = 0; i <= 1; i++) {
            int val;
            int calib_min;
            int calib_max;

            if (!valuator_mask_isset(pEvdev->vals, i))
                continue;

            val = valuator_mask_get(pEvdev->vals, i);

            if (i == 0) {
                calib_min = pEvdev->calibration.min_x;
                calib_max = pEvdev->calibration.max_x;
            } else {
                calib_min = pEvdev->calibration.min_y;
                calib_max = pEvdev->calibration.max_y;
            }

            if (pEvdev->flags & EVDEV_CALIBRATED)
                val = xf86ScaleAxis(val, pEvdev->absinfo[i].maximum,
                                    pEvdev->absinfo[i].minimum, calib_max,
                                    calib_min);

            if ((i == 0 && pEvdev->invert_x) || (i == 1 && pEvdev->invert_y))
                val = (pEvdev->absinfo[i].maximum - val +
                       pEvdev->absinfo[i].minimum);

            valuator_mask_set(pEvdev->vals, i, val);
        }
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
            if (!(pEvdev->flags & EVDEV_RELATIVE_EVENTS))
                return;

            /* Handle mouse wheel emulation */
            if (EvdevWheelEmuFilterMotion(pInfo, ev))
                return;

            pEvdev->rel_queued = 1;
            pEvdev->delta[ev->code] += value;
            map = pEvdev->axis_map[ev->code];
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


    EvdevQueueTouchEvent(pInfo, pEvdev->cur_slot, pEvdev->mt_mask, type);

    pEvdev->slot_state = SLOTSTATE_EMPTY;

    valuator_mask_zero(pEvdev->mt_mask);
}

static int
num_slots(EvdevPtr pEvdev)
{
    int value = pEvdev->absinfo[ABS_MT_SLOT].maximum -
                pEvdev->absinfo[ABS_MT_SLOT].minimum + 1;

    /* If we don't know how many slots there are, assume at least 10 */
    return value > 1 ? value : 10;
}

static int
last_mt_vals_slot(EvdevPtr pEvdev)
{
    int value = pEvdev->cur_slot - pEvdev->absinfo[ABS_MT_SLOT].minimum;

    return value < num_slots(pEvdev) ? value : -1;
}

static void
EvdevProcessTouchEvent(InputInfoPtr pInfo, struct input_event *ev)
{
    EvdevPtr pEvdev = pInfo->private;
    int map;

    if (!pEvdev->mtdev)
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
            map = pEvdev->axis_map[ev->code];
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
        map = pEvdev->axis_map[ev->code];
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
        for (i = 0; i < num_slots(pEvdev); i++)
            valuator_mask_free(&pEvdev->last_mt_vals[i]);
        free(pEvdev->last_mt_vals);
        pEvdev->last_mt_vals = NULL;
    }
    for (i = 0; i < EVDEV_MAXQUEUE; i++)
        valuator_mask_free(&pEvdev->queue[i].touchMask);
#endif
}

/* just a magic number to reduce the number of reads */
#define NUM_EVENTS 16

static void
EvdevReadInput(InputInfoPtr pInfo)
{
    struct input_event ev[NUM_EVENTS];
    int i, len = sizeof(ev);

    while (len == sizeof(ev))
    {
#ifdef MULTITOUCH
        EvdevPtr pEvdev = pInfo->private;

        if (pEvdev->mtdev)
            len = mtdev_get(pEvdev->mtdev, pInfo->fd, ev, NUM_EVENTS) *
                sizeof(struct input_event);
        else
#endif
            len = read(pInfo->fd, &ev, sizeof(ev));

        if (len <= 0)
        {
            if (errno == ENODEV) /* May happen after resume */
                xf86RemoveEnabledDevice(pInfo);
            else if (errno != EAGAIN)
                LogMessageVerbSigSafe(X_ERROR, 0, "%s: Read error: %s\n", pInfo->name,
                                       strerror(errno));
            break;
        }

        /* The kernel promises that we always only read a complete
         * event, so len != sizeof ev is an error. */
        if (len % sizeof(ev[0])) {
            LogMessageVerbSigSafe(X_ERROR, 0, "%s: Read error: %s\n", pInfo->name, strerror(errno));
            break;
        }

        for (i = 0; i < len/sizeof(ev[0]); i++)
            EvdevProcessEvent(pInfo, &ev[i]);
    }
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
    struct input_event ev[ArrayLength(bits)];
    int i;

    memset(ev, 0, sizeof(ev));

    pInfo = device->public.devicePrivate;
    for (i = 0; i < ArrayLength(bits); i++) {
        ev[i].type = EV_LED;
        ev[i].code = bits[i].code;
        ev[i].value = (ctrl->leds & bits[i].xbit) > 0;
    }

    write(pInfo->fd, ev, sizeof ev);
}

static int
EvdevAddKeyClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    /* sorry, no rules change allowed for you */
    xf86ReplaceStrOption(pInfo->options, "xkb_rules", "evdev");
    SetXkbOption(pInfo, "xkb_rules", &pEvdev->rmlvo.rules);
    SetXkbOption(pInfo, "xkb_model", &pEvdev->rmlvo.model);
    if (!pEvdev->rmlvo.model)
        SetXkbOption(pInfo, "XkbModel", &pEvdev->rmlvo.model);
    SetXkbOption(pInfo, "xkb_layout", &pEvdev->rmlvo.layout);
    if (!pEvdev->rmlvo.layout)
        SetXkbOption(pInfo, "XkbLayout", &pEvdev->rmlvo.layout);
    SetXkbOption(pInfo, "xkb_variant", &pEvdev->rmlvo.variant);
    if (!pEvdev->rmlvo.variant)
        SetXkbOption(pInfo, "XkbVariant", &pEvdev->rmlvo.variant);
    SetXkbOption(pInfo, "xkb_options", &pEvdev->rmlvo.options);
    if (!pEvdev->rmlvo.options)
        SetXkbOption(pInfo, "XkbOptions", &pEvdev->rmlvo.options);

    if (!InitKeyboardDeviceStruct(device, &pEvdev->rmlvo, NULL, EvdevKbdCtrl))
        return !Success;

    return Success;
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
EvdevAddAbsValuatorClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int num_axes, axis, i = 0;
    int num_mt_axes = 0, /* number of MT-only axes */
        num_mt_axes_total = 0; /* total number of MT axes, including
                                  double-counted ones, excluding blacklisted */
    Atom *atoms;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (!EvdevBitIsSet(pEvdev->bitmask, EV_ABS))
        goto out;

    num_axes = EvdevCountBits(pEvdev->abs_bitmask, NLONGS(ABS_MAX));
    if (num_axes < 1)
        goto out;

#ifdef MULTITOUCH
    for (axis = ABS_MT_SLOT; axis < ABS_MAX; axis++)
    {
        if (EvdevBitIsSet(pEvdev->abs_bitmask, axis))
        {
            int j;
            Bool skip = FALSE;

            for (j = 0; j < ArrayLength(mt_axis_mappings); j++)
            {
                if (mt_axis_mappings[j].mt_code == axis &&
                    BitIsOn(pEvdev->abs_bitmask, mt_axis_mappings[j].code))
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
        pEvdev->num_mt_vals = num_mt_axes_total;
        pEvdev->mt_mask = valuator_mask_new(num_mt_axes_total);
        if (!pEvdev->mt_mask) {
            xf86Msg(X_ERROR, "%s: failed to allocate MT valuator mask.\n",
                    device->name);
            goto out;
        }

        pEvdev->last_mt_vals = calloc(num_slots(pEvdev), sizeof(ValuatorMask *));
        if (!pEvdev->last_mt_vals) {
            xf86IDrvMsg(pInfo, X_ERROR,
                        "%s: failed to allocate MT last values mask array.\n",
                        device->name);
            goto out;
        }

        for (i = 0; i < num_slots(pEvdev); i++) {
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
        int mapping;
        pEvdev->axis_map[axis] = -1;
        if (!EvdevBitIsSet(pEvdev->abs_bitmask, axis) ||
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
        pEvdev->axis_map[axis] = mapping;
        if (mapping == i)
            i++;
    }

    EvdevInitAxesLabels(pEvdev, Absolute, pEvdev->num_vals + num_mt_axes, atoms);

    if (!InitValuatorClassDeviceStruct(device, num_axes + num_mt_axes, atoms,
                                       GetMotionHistorySize(), Absolute)) {
        xf86IDrvMsg(pInfo, X_ERROR, "failed to initialize valuator class device.\n");
        goto out;
    }

#ifdef MULTITOUCH
    if (pEvdev->mtdev && num_mt_axes_total > 0)
    {
        int num_touches = 0;
        int mode = pEvdev->flags & EVDEV_TOUCHPAD ?
            XIDependentTouch : XIDirectTouch;

        if (pEvdev->mtdev->caps.slot.maximum > 0)
            num_touches = pEvdev->mtdev->caps.slot.maximum -
                          pEvdev->mtdev->caps.slot.minimum + 1;

        if (!InitTouchClassDeviceStruct(device, num_touches, mode,
                                        num_mt_axes_total)) {
            xf86Msg(X_ERROR, "%s: failed to initialize touch class device.\n",
                    device->name);
            goto out;
        }

        for (i = 0; i < num_slots(pEvdev); i++) {
            for (axis = ABS_MT_TOUCH_MAJOR; axis < ABS_MAX; axis++) {
                if (pEvdev->axis_map[axis] >= 0) {
                    /* XXX: read initial values from mtdev when it adds support
                     *      for doing so. */
                    valuator_mask_set(pEvdev->last_mt_vals[i],
                                      pEvdev->axis_map[axis], 0);
                }
            }
        }
    }
#endif

    for (axis = ABS_X; axis < ABS_MT_SLOT; axis++) {
        int axnum = pEvdev->axis_map[axis];
        int resolution = 0;

        if (axnum == -1)
            continue;

#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 30)
        /* Kernel provides units/mm, X wants units/m */
        if (pEvdev->absinfo[axis].resolution)
            resolution = pEvdev->absinfo[axis].resolution * 1000;
#endif

        xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   pEvdev->absinfo[axis].minimum,
                                   pEvdev->absinfo[axis].maximum,
                                   resolution, 0, resolution, Absolute);
        xf86InitValuatorDefaults(device, axnum);
    }

#ifdef MULTITOUCH
    for (axis = ABS_MT_TOUCH_MAJOR; axis <= ABS_MAX; axis++) {
        int axnum = pEvdev->axis_map[axis];
        int resolution = 0;
        int j;
        BOOL skip = FALSE;

        if (axnum < 0)
            continue;

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

        if (pEvdev->absinfo[axis].resolution)
            resolution = pEvdev->absinfo[axis].resolution * 1000;

        xf86InitValuatorAxisStruct(device, axnum,
                                   atoms[axnum],
                                   pEvdev->absinfo[axis].minimum,
                                   pEvdev->absinfo[axis].maximum,
                                   resolution, 0, resolution,
                                   Absolute);
    }
#endif

    free(atoms);

    for (i = 0; i < ArrayLength(proximity_bits); i++)
    {
        if (!pEvdev->use_proximity)
            break;

        if (EvdevBitIsSet(pEvdev->key_bitmask, proximity_bits[i]))
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
EvdevAddRelValuatorClass(DeviceIntPtr device)
{
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;
    int num_axes, axis, i = 0;
    Atom *atoms;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    if (!EvdevBitIsSet(pEvdev->bitmask, EV_REL))
        goto out;

    num_axes = EvdevCountBits(pEvdev->rel_bitmask, NLONGS(REL_MAX));
    if (num_axes < 1)
        goto out;

#ifndef HAVE_SMOOTH_SCROLLING
    /* Wheels are special, we post them as button events. So let's ignore them
     * in the axes list too */
    if (EvdevBitIsSet(pEvdev->rel_bitmask, REL_WHEEL))
        num_axes--;
    if (EvdevBitIsSet(pEvdev->rel_bitmask, REL_HWHEEL))
        num_axes--;
    if (EvdevBitIsSet(pEvdev->rel_bitmask, REL_DIAL))
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

    for (axis = REL_X; i < MAX_VALUATORS && axis <= REL_MAX; axis++)
    {
        pEvdev->axis_map[axis] = -1;
#ifndef HAVE_SMOOTH_SCROLLING
        /* We don't post wheel events, so ignore them here too */
        if (axis == REL_WHEEL || axis == REL_HWHEEL || axis == REL_DIAL)
            continue;
#endif
        if (!EvdevBitIsSet(pEvdev->rel_bitmask, axis))
            continue;
        pEvdev->axis_map[axis] = i;
        i++;
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
        int axnum = pEvdev->axis_map[axis];

        if (axnum == -1)
            continue;
        xf86InitValuatorAxisStruct(device, axnum, atoms[axnum], -1, -1, 1, 0, 1,
                                   Relative);
        xf86InitValuatorDefaults(device, axnum);
#ifdef HAVE_SMOOTH_SCROLLING
        if (axis == REL_WHEEL)
            SetScrollValuator(device, axnum, SCROLL_TYPE_VERTICAL, -1.0, SCROLL_FLAG_PREFERRED);
        else if (axis == REL_DIAL)
            SetScrollValuator(device, axnum, SCROLL_TYPE_VERTICAL, -1.0, SCROLL_FLAG_NONE);
        else if (axis == REL_HWHEEL)
            SetScrollValuator(device, axnum, SCROLL_TYPE_HORIZONTAL, 1.0, SCROLL_FLAG_NONE);
#endif
    }

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

    if (pEvdev->flags & EVDEV_RELATIVE_EVENTS &&
        EvdevAddRelValuatorClass(device) == Success)
        xf86IDrvMsg(pInfo, X_INFO, "initialized for relative axes.\n");
    if (pEvdev->flags & EVDEV_ABSOLUTE_EVENTS &&
        EvdevAddAbsValuatorClass(device) == Success)
        xf86IDrvMsg(pInfo, X_INFO, "initialized for absolute axes.\n");
}

static void
EvdevInitAbsValuators(DeviceIntPtr device, EvdevPtr pEvdev)
{
    InputInfoPtr pInfo = device->public.devicePrivate;

    if (EvdevAddAbsValuatorClass(device) == Success) {
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
    int i;
    InputInfoPtr pInfo;
    EvdevPtr pEvdev;

    pInfo = device->public.devicePrivate;
    pEvdev = pInfo->private;

    /* clear all axis_map entries */
    for(i = 0; i < max(ABS_CNT,REL_CNT); i++)
      pEvdev->axis_map[i]=-1;

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
        EvdevRemoveDevice(pInfo);
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
    int i, len;
    struct input_id id;

    char name[1024]                  = {0};
    unsigned long bitmask[NLONGS(EV_CNT)]      = {0};
    unsigned long key_bitmask[NLONGS(KEY_CNT)] = {0};
    unsigned long rel_bitmask[NLONGS(REL_CNT)] = {0};
    unsigned long abs_bitmask[NLONGS(ABS_CNT)] = {0};
    unsigned long led_bitmask[NLONGS(LED_CNT)] = {0};


    if (ioctl(pInfo->fd, EVIOCGID, &id) < 0)
    {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGID failed: %s\n", strerror(errno));
        goto error;
    }

    pEvdev->id_vendor = id.vendor;
    pEvdev->id_product = id.product;

    if (ioctl(pInfo->fd, EVIOCGNAME(sizeof(name) - 1), name) < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGNAME failed: %s\n", strerror(errno));
        goto error;
    }

    strcpy(pEvdev->name, name);

    len = ioctl(pInfo->fd, EVIOCGBIT(0, sizeof(bitmask)), bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        goto error;
    }

    memcpy(pEvdev->bitmask, bitmask, len);

    len = ioctl(pInfo->fd, EVIOCGBIT(EV_REL, sizeof(rel_bitmask)), rel_bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        goto error;
    }

    memcpy(pEvdev->rel_bitmask, rel_bitmask, len);

    len = ioctl(pInfo->fd, EVIOCGBIT(EV_ABS, sizeof(abs_bitmask)), abs_bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        goto error;
    }

    memcpy(pEvdev->abs_bitmask, abs_bitmask, len);

    len = ioctl(pInfo->fd, EVIOCGBIT(EV_LED, sizeof(led_bitmask)), led_bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        goto error;
    }

    memcpy(pEvdev->led_bitmask, led_bitmask, len);

    /*
     * Do not try to validate absinfo data since it is not expected
     * to be static, always refresh it in evdev structure.
     */
    for (i = ABS_X; i <= ABS_MAX; i++) {
        if (EvdevBitIsSet(abs_bitmask, i)) {
            len = ioctl(pInfo->fd, EVIOCGABS(i), &pEvdev->absinfo[i]);
            if (len < 0) {
                xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGABSi(%d) failed: %s\n",
                            i, strerror(errno));
                goto error;
            }
            xf86IDrvMsgVerb(pInfo, X_PROBED, 6, "absolute axis %#x [%d..%d]\n",
                            i, pEvdev->absinfo[i].maximum, pEvdev->absinfo[i].minimum);
        }
    }

    len = ioctl(pInfo->fd, EVIOCGBIT(EV_KEY, sizeof(key_bitmask)), key_bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        goto error;
    }

    /* Copy the data so we have reasonably up-to-date info */
    memcpy(pEvdev->key_bitmask, key_bitmask, len);

    return Success;

error:
    return !Success;

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
        if (grab && ioctl(pInfo->fd, EVIOCGRAB, (void *)1)) {
            xf86IDrvMsg(pInfo, X_WARNING, "Grab failed (%s)\n",
                        strerror(errno));
            return FALSE;
        } else if (ungrab && ioctl(pInfo->fd, EVIOCGRAB, (void *)0))
            xf86IDrvMsg(pInfo, X_WARNING, "Release failed (%s)\n",
                        strerror(errno));
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
        EvdevSetBit(pEvdev->rel_bitmask, REL_X);
        EvdevSetBit(pEvdev->rel_bitmask, REL_Y);
    } else if (mode == Absolute)
    {
        EvdevSetBit(pEvdev->abs_bitmask, ABS_X);
        EvdevSetBit(pEvdev->abs_bitmask, ABS_Y);
        pEvdev->absinfo[ABS_X].minimum = 0;
        pEvdev->absinfo[ABS_X].maximum = 1000;
        pEvdev->absinfo[ABS_X].value = 0;
        pEvdev->absinfo[ABS_X].resolution = 0;
        pEvdev->absinfo[ABS_Y].minimum = 0;
        pEvdev->absinfo[ABS_Y].maximum = 1000;
        pEvdev->absinfo[ABS_Y].value = 0;
        pEvdev->absinfo[ABS_Y].resolution = 0;
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
                pEvdev->id_vendor, pEvdev->id_product);

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
        if (EvdevBitIsSet(pEvdev->key_bitmask, i))
        {
            mapping = EvdevUtilButtonEventToButtonNumber(pEvdev, i);
            if (mapping > num_buttons)
                num_buttons = mapping;
        }
    }

    has_lmr = EvdevBitIsSet(pEvdev->key_bitmask, BTN_LEFT) ||
                EvdevBitIsSet(pEvdev->key_bitmask, BTN_MIDDLE) ||
                EvdevBitIsSet(pEvdev->key_bitmask, BTN_RIGHT);

    if (num_buttons)
    {
        pEvdev->flags |= EVDEV_BUTTON_EVENTS;
        pEvdev->num_buttons = num_buttons;
        xf86IDrvMsg(pInfo, X_PROBED, "Found %d mouse buttons\n", num_buttons);
    }

    for (i = 0; i < REL_MAX; i++) {
        if (EvdevBitIsSet(pEvdev->rel_bitmask, i)) {
            has_rel_axes = TRUE;
            break;
        }
    }

    if (has_rel_axes) {
        if (EvdevBitIsSet(pEvdev->rel_bitmask, REL_WHEEL) ||
            EvdevBitIsSet(pEvdev->rel_bitmask, REL_HWHEEL) ||
            EvdevBitIsSet(pEvdev->rel_bitmask, REL_DIAL)) {
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

            if (EvdevBitIsSet(pEvdev->rel_bitmask, REL_X) &&
                EvdevBitIsSet(pEvdev->rel_bitmask, REL_Y)) {
                xf86IDrvMsg(pInfo, X_PROBED, "Found x and y relative axes\n");
            } else if (!EvdevBitIsSet(pEvdev->abs_bitmask, ABS_X) ||
                       !EvdevBitIsSet(pEvdev->abs_bitmask, ABS_Y))
                EvdevForceXY(pInfo, Relative);
        } else {
            xf86IDrvMsg(pInfo, X_INFO, "Relative axes present but ignored.\n");
            has_rel_axes = FALSE;
        }
    }

    for (i = 0; i < ABS_MAX; i++) {
        if (EvdevBitIsSet(pEvdev->abs_bitmask, i)) {
            has_abs_axes = TRUE;
            break;
        }
    }

#ifdef MULTITOUCH
    for (i = ABS_MT_SLOT; i < ABS_MAX; i++) {
        if (EvdevBitIsSet(pEvdev->abs_bitmask, i)) {
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

        if (has_mt)
            xf86IDrvMsg(pInfo, X_PROBED, "Found absolute multitouch axes\n");

        if ((EvdevBitIsSet(pEvdev->abs_bitmask, ABS_X) &&
             EvdevBitIsSet(pEvdev->abs_bitmask, ABS_Y))) {
            xf86IDrvMsg(pInfo, X_PROBED, "Found x and y absolute axes\n");
            if (EvdevBitIsSet(pEvdev->key_bitmask, BTN_TOOL_PEN) ||
                EvdevBitIsSet(pEvdev->key_bitmask, BTN_STYLUS) ||
                EvdevBitIsSet(pEvdev->key_bitmask, BTN_STYLUS2))
            {
                xf86IDrvMsg(pInfo, X_PROBED, "Found absolute tablet.\n");
                pEvdev->flags |= EVDEV_TABLET;
                if (!pEvdev->num_buttons)
                {
                    pEvdev->num_buttons = 7; /* LMR + scroll wheels */
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
                }
            } else if (EvdevBitIsSet(pEvdev->abs_bitmask, ABS_PRESSURE) ||
                EvdevBitIsSet(pEvdev->key_bitmask, BTN_TOUCH)) {
                if (has_lmr || EvdevBitIsSet(pEvdev->key_bitmask, BTN_TOOL_FINGER)) {
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchpad.\n");
                    pEvdev->flags |= EVDEV_TOUCHPAD;
                } else {
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchscreen\n");
                    pEvdev->flags |= EVDEV_TOUCHSCREEN;
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
                }
            } else if (!(EvdevBitIsSet(pEvdev->rel_bitmask, REL_X) &&
                         EvdevBitIsSet(pEvdev->rel_bitmask, REL_Y)) && has_lmr) {
                    /* some touchscreens use BTN_LEFT rather than BTN_TOUCH */
                    xf86IDrvMsg(pInfo, X_PROBED, "Found absolute touchscreen\n");
                    pEvdev->flags |= EVDEV_TOUCHSCREEN;
                    pEvdev->flags |= EVDEV_BUTTON_EVENTS;
            }
        } else {
#ifdef MULTITOUCH
            if (!EvdevBitIsSet(pEvdev->abs_bitmask, ABS_MT_POSITION_X) ||
                !EvdevBitIsSet(pEvdev->abs_bitmask, ABS_MT_POSITION_Y))
#endif
                EvdevForceXY(pInfo, Absolute);
        }



    }

    for (i = 0; i < BTN_MISC; i++) {
        if (EvdevBitIsSet(pEvdev->key_bitmask, i)) {
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
            if (!EvdevBitIsSet(pEvdev->rel_bitmask, REL_X) ||
                !EvdevBitIsSet(pEvdev->rel_bitmask, REL_Y))
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
    }

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
 * memory usage, so only do so for devices with multitouch bits set.
 *
 * @return FALSE on error, TRUE if mtdev was initiated or the device doesn't
 * need it
 */
static Bool
EvdevOpenMTDev(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    unsigned long bitmask[NLONGS(EV_CNT)]      = {0};
    unsigned long abs_bitmask[NLONGS(ABS_CNT)] = {0};
    int len;

    if (pEvdev->mtdev) {
        pEvdev->cur_slot = pEvdev->mtdev->caps.slot.value;
        return TRUE;
    }

    if (pInfo->fd < 0) {
        xf86Msg(X_ERROR, "%s: Bug. fd < 0\n", pInfo->name);
        return FALSE;
    }

    /* Use ioctl here, this may be called before EvdevCache */
    len = ioctl(pInfo->fd, EVIOCGBIT(0, sizeof(bitmask)), bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        return FALSE;
    }

    if (!EvdevBitIsSet(bitmask, EV_ABS))
        return TRUE;

    len = ioctl(pInfo->fd, EVIOCGBIT(EV_ABS, sizeof(abs_bitmask)), abs_bitmask);
    if (len < 0) {
        xf86IDrvMsg(pInfo, X_ERROR, "ioctl EVIOCGBIT failed: %s\n",
                    strerror(errno));
        return FALSE;
    }

    if (!EvdevBitIsSet(abs_bitmask, ABS_MT_POSITION_X) ||
        !EvdevBitIsSet(abs_bitmask, ABS_MT_POSITION_Y))
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

    if (pInfo->fd < 0)
    {
        do {
            pInfo->fd = open(device, O_RDWR | O_NONBLOCK, 0);
        } while (pInfo->fd < 0 && errno == EINTR);

        if (pInfo->fd < 0) {
            xf86IDrvMsg(pInfo, X_ERROR, "Unable to open evdev device \"%s\".\n", device);
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
        return FALSE;
    }
#endif

    return Success;
}

static void
EvdevCloseDevice(InputInfoPtr pInfo)
{
    EvdevPtr pEvdev = pInfo->private;
    if (pInfo->fd >= 0)
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
        /* Release strings allocated in EvdevAddKeyClass. */
        XkbFreeRMLVOSet(&pEvdev->rmlvo, FALSE);
        /* Release string allocated in EvdevOpenDevice. */
        free(pEvdev->device);
        pEvdev->device = NULL;
    }
    xf86DeleteInput(pInfo, flags);
}

static int
EvdevPreInit(InputDriverPtr drv, InputInfoPtr pInfo, int flags)
{
    EvdevPtr pEvdev;
    int rc = BadAlloc;

    if (!(pEvdev = calloc(sizeof(EvdevRec), 1)))
        goto error;

    pInfo->private = pEvdev;
    pInfo->type_name = "UNKNOWN";
    pInfo->device_control = EvdevProc;
    pInfo->read_input = EvdevReadInput;
    pInfo->switch_mode = EvdevSwitchMode;

    rc = EvdevOpenDevice(pInfo);
    if (rc != Success)
        goto error;

#ifdef MULTITOUCH
    pEvdev->cur_slot = -1;
#endif

    /*
     * We initialize pEvdev->in_proximity to 1 so that device that doesn't use
     * proximity will still report events.
     */
    pEvdev->in_proximity = 1;
    pEvdev->use_proximity = 1;

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

    EvdevAddDevice(pInfo);

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
    evdevDefaults
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

static void EvdevInitOneAxisLabel(EvdevPtr pEvdev, int axis,
                                  const char **labels, int label_idx, Atom *atoms)
{
    Atom atom;

    if (pEvdev->axis_map[axis] == -1)
        return;

    atom = XIGetKnownProperty(labels[label_idx]);
    if (!atom) /* Should not happen */
        return;

    atoms[pEvdev->axis_map[axis]] = atom;
}

static void EvdevInitAxesLabels(EvdevPtr pEvdev, int mode, int natoms, Atom *atoms)
{
    int axis;
    const char **labels;
    int labels_len = 0;

    if (mode == Absolute)
    {
        labels     = abs_labels;
        labels_len = ArrayLength(abs_labels);
    } else if (mode == Relative)
    {
        labels     = rel_labels;
        labels_len = ArrayLength(rel_labels);
    } else
        return;

    memset(atoms, 0, natoms * sizeof(Atom));

    /* Now fill the ones we know */
    for (axis = 0; axis < labels_len; axis++)
        EvdevInitOneAxisLabel(pEvdev, axis, labels, axis, atoms);
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
        if (EvdevBitIsSet(pEvdev->key_bitmask, button))
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
    product[0] = pEvdev->id_vendor;
    product[1] = pEvdev->id_product;
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
    } else if (atom == prop_axis_label || atom == prop_btn_label ||
               atom == prop_product_id || atom == prop_device ||
               atom == prop_virtual)
        return BadAccess; /* Read-only properties */

    return Success;
}
