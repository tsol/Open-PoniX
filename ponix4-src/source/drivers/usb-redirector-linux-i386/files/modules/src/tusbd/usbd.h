/*
 *
 *  Copyright (C) 2007-2014 SimplyCore, LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/usb.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/version.h>
#include <linux/kref.h>
#include <linux/device.h>
#include <linux/compat.h>
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#define KERNEL_GT(lIllll,llIIIl,lllIlIl)			(LINUX_VERSION_CODE > KERNEL_VERSION(\
(lIllll),(llIIIl),(lllIlIl)))
#define KERNEL_LT(lIllll,llIIIl,lllIlIl)			(LINUX_VERSION_CODE < KERNEL_VERSION(\
(lIllll),(llIIIl),(lllIlIl)))
#define KERNEL_EQ(lIllll,llIIIl,lllIlIl)			(LINUX_VERSION_CODE == KERNEL_VERSION\
((lIllll),(llIIIl),(lllIlIl)))
#define KERNEL_GT_EQ(lIllll,llIIIl,lllIlIl)			(LINUX_VERSION_CODE >= \
KERNEL_VERSION((lIllll),(llIIIl),(lllIlIl)))
#define KERNEL_LT_EQ(lIllll,llIIIl,lllIlIl)			(LINUX_VERSION_CODE <= \
KERNEL_VERSION((lIllll),(llIIIl),(lllIlIl)))
#if defined(RHEL_RELEASE_CODE) 
#define RHEL_RELEASE_GT(lIllll,llIIIl) 		(RHEL_RELEASE_CODE > \
RHEL_RELEASE_VERSION((lIllll),(llIIIl)))
#define RHEL_RELEASE_LT(lIllll,llIIIl)		(RHEL_RELEASE_CODE < \
RHEL_RELEASE_VERSION((lIllll),(llIIIl)))
#define RHEL_RELEASE_EQ(lIllll,llIIIl)		(RHEL_RELEASE_CODE == \
RHEL_RELEASE_VERSION((lIllll),(llIIIl)))
#define RHEL_RELEASE_GT_EQ(lIllll,llIIIl)		(RHEL_RELEASE_CODE >= \
RHEL_RELEASE_VERSION((lIllll),(llIIIl)))
#define RHEL_RELEASE_LT_EQ(lIllll,llIIIl)		(RHEL_RELEASE_CODE <= \
RHEL_RELEASE_VERSION((lIllll),(llIIIl)))
#else
#define RHEL_RELEASE_GT(lIllll,llIIIl) 		(0x1710+1947-0x1eab)
#define RHEL_RELEASE_LT(lIllll,llIIIl)		(0x1ae4+2695-0x256b)
#define RHEL_RELEASE_EQ(lIllll,llIIIl)		(0x48f+4539-0x164a)
#define RHEL_RELEASE_GT_EQ(lIllll,llIIIl)		(0x877+5689-0x1eb0)
#define RHEL_RELEASE_LT_EQ(lIllll,llIIIl)		(0x1d8+4174-0x1226)
#endif
#include "../public/pubstt2.h"
#include "../public/apitypes.h"
#include "../public/pubuniprotocol.h"
#include "../public/interface.h"
#include "../public/public_devices.h"
#include "driver.h"
#include "fifo.h"
#include "usbdcdev.h"
#include "usb_device.h"
#include "vhci_device.h"
#include "minor_control.h"
#include "minor_device.h"
#include "minor_vhci.h"
#include "utils.h"
#include "urb_chain.h"
#define IIIIlll(IIIIlIll, IlIIlIll...) printk( KERN_DEBUG \
"\x75\x73\x62\x64\x20\x5b\x25\x30\x39\x75\x5d\x3a\x20" IIIIlIll, \
jiffies_to_msecs(jiffies), ## IlIIlIll)
#ifdef _USBD_DEBUG_BUILD_
#define IIIll(IIIIlIll, IlIIlIll...) printk( KERN_DEBUG \
"\x75\x73\x62\x64\x20\x5b\x25\x30\x39\x75\x5d\x3a\x20" IIIIlIll, \
jiffies_to_msecs(jiffies), ## IlIIlIll)
#else
#define IIIll(IIIIlIll, IlIIlIll...)
#endif
#if KERNEL_LT_EQ((0x17dc+3680-0x263a),(0x8cb+1379-0xe28),(0x13a+173-0x1d0))
typedef unsigned long uintptr_t;
#endif

