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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <xserver-properties.h>

#ifndef AXIS_LABELS
#define AXIS_LABELS

/* Aligned with linux/input.h.
   Note that there are holes in the ABS_ range, these are simply replaced with
   MISC here */
static const char* abs_labels[] = {
    AXIS_LABEL_PROP_ABS_X,              /* 0x00 */
    AXIS_LABEL_PROP_ABS_Y,              /* 0x01 */
    AXIS_LABEL_PROP_ABS_Z,              /* 0x02 */
    AXIS_LABEL_PROP_ABS_RX,             /* 0x03 */
    AXIS_LABEL_PROP_ABS_RY,             /* 0x04 */
    AXIS_LABEL_PROP_ABS_RZ,             /* 0x05 */
    AXIS_LABEL_PROP_ABS_THROTTLE,       /* 0x06 */
    AXIS_LABEL_PROP_ABS_RUDDER,         /* 0x07 */
    AXIS_LABEL_PROP_ABS_WHEEL,          /* 0x08 */
    AXIS_LABEL_PROP_ABS_GAS,            /* 0x09 */
    AXIS_LABEL_PROP_ABS_BRAKE,          /* 0x0a */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_HAT0X,          /* 0x10 */
    AXIS_LABEL_PROP_ABS_HAT0Y,          /* 0x11 */
    AXIS_LABEL_PROP_ABS_HAT1X,          /* 0x12 */
    AXIS_LABEL_PROP_ABS_HAT1Y,          /* 0x13 */
    AXIS_LABEL_PROP_ABS_HAT2X,          /* 0x14 */
    AXIS_LABEL_PROP_ABS_HAT2Y,          /* 0x15 */
    AXIS_LABEL_PROP_ABS_HAT3X,          /* 0x16 */
    AXIS_LABEL_PROP_ABS_HAT3Y,          /* 0x17 */
    AXIS_LABEL_PROP_ABS_PRESSURE,       /* 0x18 */
    AXIS_LABEL_PROP_ABS_DISTANCE,       /* 0x19 */
    AXIS_LABEL_PROP_ABS_TILT_X,         /* 0x1a */
    AXIS_LABEL_PROP_ABS_TILT_Y,         /* 0x1b */
    AXIS_LABEL_PROP_ABS_TOOL_WIDTH,     /* 0x1c */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_VOLUME          /* 0x20 */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MISC,           /* undefined */
    AXIS_LABEL_PROP_ABS_MT_TOUCH_MAJOR, /* 0x30 */
    AXIS_LABEL_PROP_ABS_MT_TOUCH_MINOR, /* 0x31 */
    AXIS_LABEL_PROP_ABS_MT_WIDTH_MAJOR, /* 0x32 */
    AXIS_LABEL_PROP_ABS_MT_WIDTH_MINOR, /* 0x33 */
    AXIS_LABEL_PROP_ABS_MT_ORIENTATION, /* 0x34 */
    AXIS_LABEL_PROP_ABS_MT_POSITION_X,  /* 0x35 */
    AXIS_LABEL_PROP_ABS_MT_POSITION_Y,  /* 0x36 */
    AXIS_LABEL_PROP_ABS_MT_TOOL_TYPE,   /* 0x37 */
    AXIS_LABEL_PROP_ABS_MT_BLOB_ID,     /* 0x38 */
    AXIS_LABEL_PROP_ABS_MT_TRACKING_ID, /* 0x39 */
    AXIS_LABEL_PROP_ABS_MT_PRESSURE,    /* 0x3a */
#ifdef AXIS_LABEL_PROP_ABS_MT_DISTANCE
    AXIS_LABEL_PROP_ABS_MT_DISTANCE,    /* 0x3b */
    AXIS_LABEL_PROP_ABS_MT_TOOL_X,      /* 0x3c */
    AXIS_LABEL_PROP_ABS_MT_TOOL_Y,      /* 0x3d */
#endif /* AXIS_LABEL_PROP_ABS_MT_DISTANCE */
};

static const char* rel_labels[] = {
    AXIS_LABEL_PROP_REL_X,
    AXIS_LABEL_PROP_REL_Y,
    AXIS_LABEL_PROP_REL_Z,
    AXIS_LABEL_PROP_REL_RX,
    AXIS_LABEL_PROP_REL_RY,
    AXIS_LABEL_PROP_REL_RZ,
    AXIS_LABEL_PROP_REL_HWHEEL,
    AXIS_LABEL_PROP_REL_DIAL,
    AXIS_LABEL_PROP_REL_WHEEL,
    AXIS_LABEL_PROP_REL_MISC
};

static const char* btn_labels[][16] = {
    { /* BTN_MISC group                 offset 0x100*/
        BTN_LABEL_PROP_BTN_0,           /* 0x00 */
        BTN_LABEL_PROP_BTN_1,           /* 0x01 */
        BTN_LABEL_PROP_BTN_2,           /* 0x02 */
        BTN_LABEL_PROP_BTN_3,           /* 0x03 */
        BTN_LABEL_PROP_BTN_4,           /* 0x04 */
        BTN_LABEL_PROP_BTN_5,           /* 0x05 */
        BTN_LABEL_PROP_BTN_6,           /* 0x06 */
        BTN_LABEL_PROP_BTN_7,           /* 0x07 */
        BTN_LABEL_PROP_BTN_8,           /* 0x08 */
        BTN_LABEL_PROP_BTN_9            /* 0x09 */
    },
    { /* BTN_MOUSE group                offset 0x110 */
        BTN_LABEL_PROP_BTN_LEFT,        /* 0x00 */
        BTN_LABEL_PROP_BTN_RIGHT,       /* 0x01 */
        BTN_LABEL_PROP_BTN_MIDDLE,      /* 0x02 */
        BTN_LABEL_PROP_BTN_SIDE,        /* 0x03 */
        BTN_LABEL_PROP_BTN_EXTRA,       /* 0x04 */
        BTN_LABEL_PROP_BTN_FORWARD,     /* 0x05 */
        BTN_LABEL_PROP_BTN_BACK,        /* 0x06 */
        BTN_LABEL_PROP_BTN_TASK         /* 0x07 */
    },
    { /* BTN_JOYSTICK group             offset 0x120 */
        BTN_LABEL_PROP_BTN_TRIGGER,     /* 0x00 */
        BTN_LABEL_PROP_BTN_THUMB,       /* 0x01 */
        BTN_LABEL_PROP_BTN_THUMB2,      /* 0x02 */
        BTN_LABEL_PROP_BTN_TOP,         /* 0x03 */
        BTN_LABEL_PROP_BTN_TOP2,        /* 0x04 */
        BTN_LABEL_PROP_BTN_PINKIE,      /* 0x05 */
        BTN_LABEL_PROP_BTN_BASE,        /* 0x06 */
        BTN_LABEL_PROP_BTN_BASE2,       /* 0x07 */
        BTN_LABEL_PROP_BTN_BASE3,       /* 0x08 */
        BTN_LABEL_PROP_BTN_BASE4,       /* 0x09 */
        BTN_LABEL_PROP_BTN_BASE5,       /* 0x0a */
        BTN_LABEL_PROP_BTN_BASE6,       /* 0x0b */
        NULL,
        NULL,
        NULL,
        BTN_LABEL_PROP_BTN_DEAD         /* 0x0f */
    },
    { /* BTN_GAMEPAD group              offset 0x130 */
        BTN_LABEL_PROP_BTN_A,           /* 0x00 */
        BTN_LABEL_PROP_BTN_B,           /* 0x01 */
        BTN_LABEL_PROP_BTN_C,           /* 0x02 */
        BTN_LABEL_PROP_BTN_X,           /* 0x03 */
        BTN_LABEL_PROP_BTN_Y,           /* 0x04 */
        BTN_LABEL_PROP_BTN_Z,           /* 0x05 */
        BTN_LABEL_PROP_BTN_TL,          /* 0x06 */
        BTN_LABEL_PROP_BTN_TR,          /* 0x07 */
        BTN_LABEL_PROP_BTN_TL2,         /* 0x08 */
        BTN_LABEL_PROP_BTN_TR2,         /* 0x09 */
        BTN_LABEL_PROP_BTN_SELECT,      /* 0x0a */
        BTN_LABEL_PROP_BTN_START,       /* 0x0b */
        BTN_LABEL_PROP_BTN_MODE,        /* 0x0c */
        BTN_LABEL_PROP_BTN_THUMBL,      /* 0x0d */
        BTN_LABEL_PROP_BTN_THUMBR       /* 0x0e */
    },
    { /* BTN_DIGI group                         offset 0x140 */
        BTN_LABEL_PROP_BTN_TOOL_PEN,            /* 0x00 */
        BTN_LABEL_PROP_BTN_TOOL_RUBBER,         /* 0x01 */
        BTN_LABEL_PROP_BTN_TOOL_BRUSH,          /* 0x02 */
        BTN_LABEL_PROP_BTN_TOOL_PENCIL,         /* 0x03 */
        BTN_LABEL_PROP_BTN_TOOL_AIRBRUSH,       /* 0x04 */
        BTN_LABEL_PROP_BTN_TOOL_FINGER,         /* 0x05 */
        BTN_LABEL_PROP_BTN_TOOL_MOUSE,          /* 0x06 */
        BTN_LABEL_PROP_BTN_TOOL_LENS,           /* 0x07 */
        NULL,
        NULL,
        BTN_LABEL_PROP_BTN_TOUCH,               /* 0x0a */
        BTN_LABEL_PROP_BTN_STYLUS,              /* 0x0b */
        BTN_LABEL_PROP_BTN_STYLUS2,             /* 0x0c */
        BTN_LABEL_PROP_BTN_TOOL_DOUBLETAP,      /* 0x0d */
        BTN_LABEL_PROP_BTN_TOOL_TRIPLETAP       /* 0x0e */
    },
    { /* BTN_WHEEL group                        offset 0x150 */
        BTN_LABEL_PROP_BTN_GEAR_DOWN,           /* 0x00 */
        BTN_LABEL_PROP_BTN_GEAR_UP              /* 0x01 */
    }
};

#endif
