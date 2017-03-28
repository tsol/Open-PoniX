#include "alsa-autoconf.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5)
#define SND_NEED_USB_SET_INTERFACE
#endif
// #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define SND_NEED_USB_WRAPPER
// #endif
#include "adriver.h"
#include "usbcompat.h"
#include <linux/usb.h>
#include <linux/usb/ch9.h>

#ifndef USB_DT_CS_DEVICE
#define USB_DT_CS_DEVICE		0x21
#define USB_DT_CS_CONFIG		0x22
#define USB_DT_CS_STRING		0x23
#define USB_DT_CS_INTERFACE		0x24
#define USB_DT_CS_ENDPOINT		0x25
#endif

#ifndef USB_ENDPOINT_SYNCTYPE
#define USB_ENDPOINT_SYNCTYPE		0x0c
#define USB_ENDPOINT_SYNC_NONE		(0 << 2)
#define USB_ENDPOINT_SYNC_ASYNC		(1 << 2)
#define USB_ENDPOINT_SYNC_ADAPTIVE	(2 << 2)
#define USB_ENDPOINT_SYNC_SYNC		(3 << 2)
#endif

#ifndef USB_SUBCLASS_VENDOR_SPEC
#define USB_SUBCLASS_VENDOR_SPEC	0xff
#endif

#include <linux/slab.h>

// vim: ft=c
