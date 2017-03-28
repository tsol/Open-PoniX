/****************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 * (c) Copyright 2002, Ralink Technology, Inc.
 *
 * All rights reserved. Ralink's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Ralink Tech. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Ralink Technology, Inc. is obtained.
 ****************************************************************************

    Module Name:
    rtusb_dev_id.c

    Abstract:

    Revision History:
    Who        When          What
    ---------  ----------    ----------------------------------------------
 */

#define RTMP_MODULE_OS

/*#include "rt_config.h"*/
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"

/* module table */
USB_DEVICE_ID rtusb_dev_id[] = {
#ifdef RT35xx
	{USB_DEVICE(0x148F,0x3572)}, /* Ralink 3572 */
	{USB_DEVICE(0x1740,0x9801)}, /* EnGenius 3572 */
	{USB_DEVICE(0x0DF6,0x0041)}, /* Sitecom 3572 */
	{USB_DEVICE(0x0DF6,0x0042)},
	{USB_DEVICE(0x04BB,0x0944)}, /* I-O DATA 3572 */
	{USB_DEVICE(0x1690,0x0740)}, /* 3572 */
	{USB_DEVICE(0x1690,0x0744)}, /* 3572 */
	{USB_DEVICE(0x5A57,0x0284)}, /* Zinwell 3572 */
	{USB_DEVICE(0x167B,0x4001)}, /* 3572 */
	{USB_DEVICE(0x1690,0x0764)}, /* 3572 */ 
	{USB_DEVICE(0x0930,0x0A07)}, /* TOSHIBA */
	{USB_DEVICE(0x1690,0x0761)}, /* Askey */
	{USB_DEVICE(0x13B1,0x002F)}, /* Cisco LinkSys AE1000 */
	{USB_DEVICE(0x1737,0x0079)}, /* Cisco LinkSys WUSB600N */
#endif /* RT35xx */
#ifdef RT3573
	{USB_DEVICE(0x148F,0x3573)}, /* Ralink 3573 */
	{USB_DEVICE(0x7392,0x7733)}, /* Edimax */
	{USB_DEVICE(0x0846,0x9012)}, /* Netgear WNDA4100 N900*/
#endif /* RT3573 */
	{ }/* Terminating entry */
};

INT const rtusb_usb_id_len = sizeof(rtusb_dev_id) / sizeof(USB_DEVICE_ID);

MODULE_DEVICE_TABLE(usb, rtusb_dev_id);
