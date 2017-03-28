/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2007, Ralink Technology, Inc.
 *
 * This program is free software; you can redistribute it and/or modify  * 
 * it under the terms of the GNU General Public License as published by  * 
 * the Free Software Foundation; either version 2 of the License, or     * 
 * (at your option) any later version.                                   * 
 *                                                                       * 
 * This program is distributed in the hope that it will be useful,       * 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        * 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         * 
 * GNU General Public License for more details.                          * 
 *                                                                       * 
 * You should have received a copy of the GNU General Public License     * 
 * along with this program; if not, write to the                         * 
 * Free Software Foundation, Inc.,                                       * 
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             * 
 *                                                                       * 
 *************************************************************************

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
#ifdef RT2870
	{USB_DEVICE(0x148F,0x2770)}, /* Ralink */
	{USB_DEVICE(0x148F,0x2870)}, /* Ralink */
	{USB_DEVICE(0x07B8,0x2870)}, /* AboCom */
	{USB_DEVICE(0x07B8,0x2770)}, /* AboCom */
	{USB_DEVICE(0x0DF6,0x0039)}, /* Sitecom 2770 */
	{USB_DEVICE(0x0DF6,0x003F)}, /* Sitecom 2770 */
	{USB_DEVICE(0x083A,0x7512)}, /* Arcadyan 2770 */
	{USB_DEVICE(0x0789,0x0162)}, /* Logitec 2870 */
	{USB_DEVICE(0x0789,0x0163)}, /* Logitec 2870 */
	{USB_DEVICE(0x0789,0x0164)}, /* Logitec 2870 */
	{USB_DEVICE(0x177f,0x0302)}, /* lsusb */
	{USB_DEVICE(0x0B05,0x1731)}, /* Asus */
	{USB_DEVICE(0x0B05,0x1732)}, /* Asus */
	{USB_DEVICE(0x0B05,0x1742)}, /* Asus */
	{USB_DEVICE(0x0DF6,0x0017)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002B)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002C)}, /* Sitecom */
	{USB_DEVICE(0x0DF6,0x002D)}, /* Sitecom */
	{USB_DEVICE(0x14B2,0x3C06)}, /* Conceptronic */
	{USB_DEVICE(0x14B2,0x3C28)}, /* Conceptronic */
	{USB_DEVICE(0x2019,0xED06)}, /* Planex Communications, Inc. */
	{USB_DEVICE(0x07D1,0x3C09)}, /* D-Link */
	{USB_DEVICE(0x07D1,0x3C11)}, /* D-Link */
	{USB_DEVICE(0x14B2,0x3C07)}, /* AL */
	{USB_DEVICE(0x050D,0x8053)}, /* Belkin */
	{USB_DEVICE(0x14B2,0x3C23)}, /* Airlink */
	{USB_DEVICE(0x14B2,0x3C27)}, /* Airlink */
	{USB_DEVICE(0x07AA,0x002F)}, /* Corega */
	{USB_DEVICE(0x07AA,0x003C)}, /* Corega */
	{USB_DEVICE(0x07AA,0x003F)}, /* Corega */
	{USB_DEVICE(0x1044,0x800B)}, /* Gigabyte */
	{USB_DEVICE(0x15A9,0x0006)}, /* Sparklan */
	{USB_DEVICE(0x083A,0xB522)}, /* SMC */
	{USB_DEVICE(0x083A,0xA618)}, /* SMC */
	{USB_DEVICE(0x083A,0x8522)}, /* Arcadyan */
	{USB_DEVICE(0x083A,0x7522)}, /* Arcadyan */
	{USB_DEVICE(0x0CDE,0x0022)}, /* ZCOM */
	{USB_DEVICE(0x0586,0x3416)}, /* Zyxel */
	{USB_DEVICE(0x0CDE,0x0025)}, /* Zyxel */
	{USB_DEVICE(0x1740,0x9701)}, /* EnGenius */
	{USB_DEVICE(0x1740,0x9702)}, /* EnGenius */
	{USB_DEVICE(0x0471,0x200f)}, /* Philips */
	{USB_DEVICE(0x14B2,0x3C25)}, /* Draytek */
	{USB_DEVICE(0x13D3,0x3247)}, /* AzureWave */
	{USB_DEVICE(0x083A,0x6618)}, /* Accton */
	{USB_DEVICE(0x15c5,0x0008)}, /* Amit */
	{USB_DEVICE(0x0E66,0x0001)}, /* Hawking */
	{USB_DEVICE(0x0E66,0x0003)}, /* Hawking */
	{USB_DEVICE(0x129B,0x1828)}, /* Siemens */
	{USB_DEVICE(0x157E,0x300E)},	/* U-Media */
	{USB_DEVICE(0x050d,0x805c)},	
	{USB_DEVICE(0x050d,0x815c)},
	{USB_DEVICE(0x1482,0x3C09)}, /* Abocom*/
	{USB_DEVICE(0x14B2,0x3C09)}, /* Alpha */
	{USB_DEVICE(0x04E8,0x2018)}, /* samsung linkstick2 */
	{USB_DEVICE(0x1690,0x0740)}, /* Askey */
	{USB_DEVICE(0x5A57,0x0280)}, /* Zinwell */
	{USB_DEVICE(0x5A57,0x0282)}, /* Zinwell */
	{USB_DEVICE(0x7392,0x7718)},
	{USB_DEVICE(0x7392,0x7717)},
	{USB_DEVICE(0x1737,0x0070)}, /* Linksys WUSB100 */
	{USB_DEVICE(0x1737,0x0071)}, /* Linksys WUSB600N */
	{USB_DEVICE(0x0411,0x00e8)}, /* Buffalo WLI-UC-G300N*/
	{USB_DEVICE(0x050d,0x815c)}, /* Belkin F5D8053 */
	{USB_DEVICE(0x100D,0x9031)}, /* Motorola 2770 */
	{USB_DEVICE(0x0DB0,0x6899)},
#endif /* RT2870*/
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
	{USB_DEVICE(0x0930,0x0A07)}, /* TOSHIBA */
#endif /* RT35xx */
	{ }/* Terminating entry */
};

INT const rtusb_usb_id_len = sizeof(rtusb_dev_id) / sizeof(USB_DEVICE_ID);

MODULE_DEVICE_TABLE(usb, rtusb_dev_id);
