/*
 * Copyright © 2011 Red Hat, Inc.
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
 *	Peter Hutterer (peter.hutterer@redhat.com)
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <evdev.h>
#include <evdev-properties.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <exevents.h>
#include <xf86.h>
#include <xf86Xinput.h>
#include <X11/Xatom.h>

/* Apple-specific controls.
 *
 * On Apple keyboards, the multimedia function keys are overlaid with the F
 * keys. F1 is also BrightnessDown, F10 is Mute, etc. The kernel provides a
 * tweak to enable/disable this.
 *
 * /sys/module/hid_apple/parameters/fnmode
 *     0 .. keyboard sends Fx keys, fn is disabled
 *     1 .. keyboard sends multimedia keys, fn sends Fx keys
 *     2 .. keyboard sends Fx keys, fn sends multimedia keys
 *
 * We only handle 1 and 2, don't care about 0. If fnmode is found to be on
 * 0, we force it to 2 instead.
 */

/* In this file: fkeymode refers to the evdev-specific enums and parameters,
 * fnmode refers to the fnmode parameter exposed by the kernel. fnmode is
 * apple-specific */
#define FNMODE_PATH "/sys/module/hid_apple/parameters/fnmode"

/* Taken from the kernel */
#define USB_VENDOR_ID_APPLE                             0x05ac
#define USB_DEVICE_ID_APPLE_ALU_MINI_ANSI               0x021d
#define USB_DEVICE_ID_APPLE_ALU_MINI_ISO                0x021e
#define USB_DEVICE_ID_APPLE_ALU_MINI_JIS                0x021f
#define USB_DEVICE_ID_APPLE_ALU_ANSI                    0x0220
#define USB_DEVICE_ID_APPLE_ALU_ISO                     0x0221
#define USB_DEVICE_ID_APPLE_ALU_JIS                     0x0222
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_ANSI           0x022c
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_ISO            0x022d
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_JIS            0x022e
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_ANSI      0x0239
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_ISO       0x023a
#define USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_JIS       0x023b

static int EvdevAppleGetProperty (DeviceIntPtr dev, Atom property);
static int EvdevAppleSetProperty(DeviceIntPtr dev, Atom atom,
                      XIPropertyValuePtr val, BOOL checkonly);

static Atom prop_fkeymode;
static Bool fnmode_readonly; /* set if we can only read fnmode */

struct product_table
{
    unsigned int vendor;
    unsigned int product;
} apple_keyboard_table[] = {
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_MINI_ANSI},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_MINI_ISO},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_MINI_JIS},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_ANSI},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_ISO},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_JIS},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_ANSI},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_ISO},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_JIS},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_ANSI},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_ISO},
    { USB_VENDOR_ID_APPLE, USB_DEVICE_ID_APPLE_ALU_WIRELESS_2009_JIS},
    { 0, 0}
};

/**
 * @return TRUE if the device matches a product in the given product table,
 *         FALSE otherwise
 */
static Bool product_check(const struct product_table *t, int vendor, int product)
{
    while (t->vendor)
    {
        if (vendor == t->vendor && product == t->product)
            return TRUE;
        t++;
    }

    return FALSE;
}

/**
 * @return 0 on success, -1 otherwise (check errno)
 */
static int
set_fnmode(enum fkeymode fkeymode)
{
    int fd;
    char mode;
    int bytes_written;

    if (fkeymode == FKEYMODE_UNKNOWN)
    {
        errno = EINVAL; /* silly you */
        return -1;
    }

    fd = open(FNMODE_PATH, O_WRONLY);
    if (fd < 0)
        return -1;

    mode = (fkeymode == FKEYMODE_FKEYS) ? '2' : '1';

    bytes_written = write(fd, &mode, 1);
    close(fd);

    return (bytes_written == 1) ? 0 : -1;
}

/**
 * Get the current value of fnmode. If fnmode is found to be on 0, we set it
 * to 2 in the process. Yes, quite daring, I know. I live on the edge.
 *
 * @return The current setting of fnmode or FKEYMODE_UNKNOWN on error (check
 * errno)
 */
static enum fkeymode
get_fnmode(void)
{
    int fd;
    char retvalue;

    fd = open(FNMODE_PATH, O_RDWR);
    if (fd < 0 && errno == EACCES)
    {
        fnmode_readonly = TRUE;
        fd = open(FNMODE_PATH, O_RDONLY);
    }

    if (fd < 0)
        goto err;

    if (read(fd, &retvalue, 1) != 1)
        goto err;

    if (retvalue != '0' && retvalue != '1' && retvalue != '2')
    {
        xf86Msg(X_ERROR, "Invalid fnmode value: %c\n", retvalue);
        errno = EINVAL;
        goto err;
    }

    close(fd);

    /* we don't want 0, switch to 2 */
    if (retvalue == '0')
    {
        if (fnmode_readonly)
            xf86Msg(X_WARNING, "fnmode is disabled and read-only. Fn key will"
                    "not toggle to multimedia keys.\n");
        else
            set_fnmode(FKEYMODE_FKEYS);
    }


    return retvalue == '1' ? FKEYMODE_MMKEYS : FKEYMODE_FKEYS;

err:
    if (fd >= 0)
        close(fd);
    return FKEYMODE_UNKNOWN;
}

/**
 * Set the property value to fkeymode. If the property doesn't exist,
 * initialize it.
 */
static void set_fkeymode_property(InputInfoPtr pInfo, enum fkeymode fkeymode)
{
    DeviceIntPtr dev = pInfo->dev;
    BOOL init = FALSE;
    char data;

    switch(fkeymode)
    {
        case FKEYMODE_FKEYS: data = 0; break;
        case FKEYMODE_MMKEYS: data = 1; break;
        case FKEYMODE_UNKNOWN:
            xf86IDrvMsg(pInfo, X_ERROR, "Failed to get fnmode (%s)\n", strerror(errno));
            return;
    }

    if (!prop_fkeymode) {
        init = TRUE;
        prop_fkeymode = MakeAtom(EVDEV_PROP_FUNCTION_KEYS, strlen(EVDEV_PROP_FUNCTION_KEYS), TRUE);
    }

    /* Don't send an event if we're initializing the property */
    XIChangeDeviceProperty(dev, prop_fkeymode, XA_INTEGER, 8,
                           PropModeReplace, 1, &data, !init);

    if (init)
    {
        XISetDevicePropertyDeletable(dev, prop_fkeymode, FALSE);
        XIRegisterPropertyHandler(dev, EvdevAppleSetProperty, EvdevAppleGetProperty, NULL);
    }
}


/**
 * Called when a client reads the property state.
 * Update with current kernel state, it may have changed behind our back.
 */
static int
EvdevAppleGetProperty (DeviceIntPtr dev, Atom property)
{
    if (property == prop_fkeymode)
    {
        InputInfoPtr pInfo  = dev->public.devicePrivate;
        EvdevPtr     pEvdev = pInfo->private;
        enum fkeymode fkeymode;

        fkeymode = get_fnmode();
        if (fkeymode != pEvdev->fkeymode) {
            /* set internal copy first, so we don't write to the file in
             * SetProperty handler */
            pEvdev->fkeymode = fkeymode;
            set_fkeymode_property(pInfo, fkeymode);
        }
    }
    return Success;
}

static int
EvdevAppleSetProperty(DeviceIntPtr dev, Atom atom,
                      XIPropertyValuePtr val, BOOL checkonly)
{
    InputInfoPtr pInfo  = dev->public.devicePrivate;
    EvdevPtr pEvdev = pInfo->private;

    if (atom == prop_fkeymode)
    {
        CARD8 v = *(CARD8*)val->data;

        if (val->format != 8 || val->type != XA_INTEGER)
            return BadMatch;

        if (fnmode_readonly)
            return BadAccess;

        if (v > 1)
            return BadValue;

        if (!checkonly)
        {
            if ((!v && pEvdev->fkeymode != FKEYMODE_FKEYS) ||
                (v && pEvdev->fkeymode != FKEYMODE_MMKEYS))
            {
                pEvdev->fkeymode = v ? FKEYMODE_MMKEYS : FKEYMODE_FKEYS;
                set_fnmode(pEvdev->fkeymode);
            }
        }
    }

    return Success;
}

void
EvdevAppleInitProperty(DeviceIntPtr dev)
{
    InputInfoPtr pInfo  = dev->public.devicePrivate;
    EvdevPtr     pEvdev = pInfo->private;
    enum fkeymode fkeymode;

    if (!product_check(apple_keyboard_table,
                       libevdev_get_id_vendor(pEvdev->dev),
                       libevdev_get_id_product(pEvdev->dev)))
        return;

    fkeymode = get_fnmode();
    pEvdev->fkeymode = fkeymode;
    set_fkeymode_property(pInfo, fkeymode);
}
