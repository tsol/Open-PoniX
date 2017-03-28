/*
 * Copyright © 2004-2008 Red Hat, Inc.
 * Copyright © 2008 University of South Australia
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
 *	Peter Hutterer (peter@cs.unisa.edu.au)
 *	Oliver McFadden (oliver.mcfadden@nokia.com)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef EVDEV_H
#define EVDEV_H

#include <linux/input.h>
#include <linux/types.h>

#include <xorg-server.h>
#include <xf86Xinput.h>
#include <xf86_OSproc.h>
#include <xkbstr.h>

#ifdef MULTITOUCH
#include <mtdev.h>
#endif

#include <libevdev/libevdev.h>

#ifndef EV_CNT /* linux 2.6.23 kernels and earlier lack _CNT defines */
#define EV_CNT (EV_MAX+1)
#endif
#ifndef KEY_CNT
#define KEY_CNT (KEY_MAX+1)
#endif
#ifndef REL_CNT
#define REL_CNT (REL_MAX+1)
#endif
#ifndef ABS_CNT
#define ABS_CNT (ABS_MAX+1)
#endif
#ifndef LED_CNT
#define LED_CNT (LED_MAX+1)
#endif

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) >= 14
#define HAVE_SMOOTH_SCROLLING 1
#endif

#if GET_ABI_MAJOR(ABI_XINPUT_VERSION) < 18
#define LogMessageVerbSigSafe xf86MsgVerb
#endif

#define EVDEV_MAXBUTTONS 32
#define EVDEV_MAXQUEUE 32

/* evdev flags */
#define EVDEV_KEYBOARD_EVENTS	(1 << 0)
#define EVDEV_BUTTON_EVENTS	(1 << 1)
#define EVDEV_RELATIVE_EVENTS	(1 << 2)
#define EVDEV_ABSOLUTE_EVENTS	(1 << 3)
#define EVDEV_TOUCHPAD		(1 << 4)
#define EVDEV_INITIALIZED	(1 << 5) /* WheelInit etc. called already? */
#define EVDEV_TOUCHSCREEN	(1 << 6)
#define EVDEV_CALIBRATED	(1 << 7) /* run-time calibrated? */
#define EVDEV_TABLET		(1 << 8) /* device looks like a tablet? */
#define EVDEV_UNIGNORE_ABSOLUTE (1 << 9) /* explicitly unignore abs axes */
#define EVDEV_UNIGNORE_RELATIVE (1 << 10) /* explicitly unignore rel axes */
#define EVDEV_RELATIVE_MODE	(1 << 11) /* Force relative events for devices with absolute axes */

#ifndef MAX_VALUATORS
#define MAX_VALUATORS 36
#endif

#ifndef XI_PROP_DEVICE_NODE
#define XI_PROP_DEVICE_NODE "Device Node"
#endif

#define LONG_BITS (sizeof(long) * 8)

/* Number of longs needed to hold the given number of bits */
#define NLONGS(x) (((x) + LONG_BITS - 1) / LONG_BITS)

/* Function key mode */
enum fkeymode {
    FKEYMODE_UNKNOWN = 0,
    FKEYMODE_FKEYS,       /* function keys send function keys */
    FKEYMODE_MMKEYS,      /* function keys send multimedia keys */
};

enum SlotState {
    SLOTSTATE_OPEN = 8,
    SLOTSTATE_CLOSE,
    SLOTSTATE_UPDATE,
    SLOTSTATE_EMPTY,
};

enum ButtonAction {
    BUTTON_RELEASE = 0,
    BUTTON_PRESS = 1
};

/* axis specific data for wheel emulation */
typedef struct {
    int up_button;
    int down_button;
    int traveled_distance;
} WheelAxis, *WheelAxisPtr;

/* Event queue used to defer keyboard/button events until EV_SYN time. */
typedef struct {
    enum {
        EV_QUEUE_KEY,	/* xf86PostKeyboardEvent() */
        EV_QUEUE_BTN,	/* xf86PostButtonEvent() */
        EV_QUEUE_PROXIMITY, /* xf86PostProximityEvent() */
#ifdef MULTITOUCH
        EV_QUEUE_TOUCH,	/*xf86PostTouchEvent() */
#endif
    } type;
    union {
        int key;	/* May be either a key code or button number. */
#ifdef MULTITOUCH
        unsigned int touch; /* Touch ID */
#endif
    } detail;
    int val;	/* State of the key/button/touch; pressed or released. */
#ifdef MULTITOUCH
    ValuatorMask *touchMask;
#endif
} EventQueueRec, *EventQueuePtr;

typedef struct {
    struct libevdev *dev;

    char *device;
    int grabDevice;         /* grab the event device? */

    int num_vals;           /* number of valuators */
    int num_mt_vals;        /* number of multitouch valuators */
    int abs_axis_map[ABS_CNT]; /* Map evdev ABS_* to index */
    int rel_axis_map[REL_CNT]; /* Map evdev REL_* to index */
    ValuatorMask *vals;     /* new values coming in */
    ValuatorMask *old_vals; /* old values for calculating relative motion */
    ValuatorMask *prox;     /* last values set while not in proximity */
    ValuatorMask *mt_mask;
    ValuatorMask **last_mt_vals;
    int cur_slot;
    enum SlotState slot_state;
#ifdef MULTITOUCH
    struct mtdev *mtdev;
#endif

    int flags;
    int in_proximity;           /* device in proximity */
    int use_proximity;          /* using the proximity bit? */
    int num_buttons;            /* number of buttons */
    BOOL swap_axes;
    BOOL invert_x;
    BOOL invert_y;

    int delta[REL_CNT];
    unsigned int abs_queued, rel_queued, prox_queued;

    /* Middle mouse button emulation */
    struct {
        BOOL                enabled;
        BOOL                pending;     /* timer waiting? */
        int                 buttonstate; /* phys. button state */
        int                 state;       /* state machine (see bt3emu.c) */
        Time                expires;     /* time of expiry */
        Time                timeout;
    } emulateMB;
    /* Third mouse button emulation */
    struct emulate3B {
        BOOL                enabled;
        BOOL                state;       /* current state */
        Time                timeout;     /* timeout until third button press */
        int                 buttonstate; /* phys. button state */
        int                 button;      /* phys button to emit */
        int                 threshold;   /* move threshold in dev coords */
        OsTimerPtr          timer;
        int                 delta[2];    /* delta x/y, accumulating */
        int                 startpos[2]; /* starting pos for abs devices */
        int                 flags;       /* remember if we had rel or abs movement */
    } emulate3B;
    struct {
	int                 meta;           /* meta key to lock any button */
	BOOL                meta_state;     /* meta_button state */
	unsigned int        lock_pair[EVDEV_MAXBUTTONS];  /* specify a meta/lock pair */
	BOOL                lock_state[EVDEV_MAXBUTTONS]; /* state of any locked buttons */
    } dragLock;
    struct {
        BOOL                enabled;
        int                 button;
        int                 button_state;
        int                 inertia;
        WheelAxis           X;
        WheelAxis           Y;
        Time                expires;     /* time of expiry */
        Time                timeout;
    } emulateWheel;
    struct {
        int                 vert_delta;
        int                 horiz_delta;
        int                 dial_delta;
    } smoothScroll;
    /* run-time calibration */
    struct {
        int                 min_x;
        int                 max_x;
        int                 min_y;
        int                 max_y;
    } calibration;

    unsigned char btnmap[32];           /* config-file specified button mapping */

    int reopen_attempts; /* max attempts to re-open after read failure */
    int reopen_left;     /* number of attempts left to re-open the device */
    OsTimerPtr reopen_timer;

    /* minor/major number */
    dev_t min_maj;

    /* Event queue used to defer keyboard/button events until EV_SYN time. */
    int                     num_queue;
    EventQueueRec           queue[EVDEV_MAXQUEUE];

    enum fkeymode           fkeymode;

    char *type_name;
} EvdevRec, *EvdevPtr;

/* Event posting functions */
void EvdevQueueKbdEvent(InputInfoPtr pInfo, struct input_event *ev, int value);
void EvdevQueueButtonEvent(InputInfoPtr pInfo, int button, int value);
void EvdevQueueProximityEvent(InputInfoPtr pInfo, int value);
#ifdef MULTITOUCH
void EvdevQueueTouchEvent(InputInfoPtr pInfo, unsigned int touch,
                          ValuatorMask *mask, uint16_t type);
#endif
void EvdevPostButtonEvent(InputInfoPtr pInfo, int button, enum ButtonAction act);
void EvdevQueueButtonClicks(InputInfoPtr pInfo, int button, int count);
void EvdevPostRelativeMotionEvents(InputInfoPtr pInfo, int num_v, int first_v,
				   int v[MAX_VALUATORS]);
void EvdevPostAbsoluteMotionEvents(InputInfoPtr pInfo, int num_v, int first_v,
				   int v[MAX_VALUATORS]);
unsigned int EvdevUtilButtonEventToButtonNumber(EvdevPtr pEvdev, int code);

/* Middle Button emulation */
int  EvdevMBEmuTimer(InputInfoPtr);
BOOL EvdevMBEmuFilterEvent(InputInfoPtr, int, BOOL);
void EvdevMBEmuWakeupHandler(pointer, int, pointer);
void EvdevMBEmuBlockHandler(pointer, struct timeval**, pointer);
void EvdevMBEmuPreInit(InputInfoPtr);
void EvdevMBEmuOn(InputInfoPtr);
void EvdevMBEmuFinalize(InputInfoPtr);

/* Third button emulation */
CARD32 Evdev3BEmuTimer(OsTimerPtr timer, CARD32 time, pointer arg);
BOOL Evdev3BEmuFilterEvent(InputInfoPtr, int, BOOL);
void Evdev3BEmuPreInit(InputInfoPtr pInfo);
void Evdev3BEmuOn(InputInfoPtr);
void Evdev3BEmuFinalize(InputInfoPtr);
void Evdev3BEmuProcessRelMotion(InputInfoPtr pInfo, int dx, int dy);
void Evdev3BEmuProcessAbsMotion(InputInfoPtr pInfo, ValuatorMask *vals);

/* Mouse Wheel emulation */
void EvdevWheelEmuPreInit(InputInfoPtr pInfo);
BOOL EvdevWheelEmuFilterButton(InputInfoPtr pInfo, unsigned int button, int value);
BOOL EvdevWheelEmuFilterMotion(InputInfoPtr pInfo, struct input_event *pEv);

/* Draglock code */
void EvdevDragLockPreInit(InputInfoPtr pInfo);
BOOL EvdevDragLockFilterEvent(InputInfoPtr pInfo, unsigned int button, int value);

void EvdevMBEmuInitProperty(DeviceIntPtr);
void Evdev3BEmuInitProperty(DeviceIntPtr);
void EvdevWheelEmuInitProperty(DeviceIntPtr);
void EvdevDragLockInitProperty(DeviceIntPtr);
void EvdevAppleInitProperty(DeviceIntPtr);
#endif
