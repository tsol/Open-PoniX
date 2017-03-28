/*
 *************************************************************************
 * Ralink Tech Inc.
 * 5F., No.36, Taiyuan St., Jhubei City,
 * Hsinchu County 302,
 * Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2010, Ralink Technology, Inc.
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
 *************************************************************************/


#define RTMP_MODULE_OS

#ifdef MBSS_SUPPORT

#define MODULE_MBSS
/*#include "rt_config.h" */
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"


/* --------------------------------- Public -------------------------------- */
NET_DEV_STATS *RT28xx_get_ether_stats(
    IN  PNET_DEV net_dev);

/*
========================================================================
Routine Description:
    Initialize Multi-BSS function.

Arguments:
    pAd				points to our adapter
    pDevMain		points to the main BSS network interface

Return Value:
    None

Note:
	1. Only create and initialize virtual network interfaces.
	2. No main network interface here.
	3. If you down ra0 and modify the BssNum of RT2860AP.dat/RT2870AP.dat,
		it will not work! You must rmmod rt2860ap.ko and lsmod rt2860ap.ko again.
========================================================================
*/
VOID RT28xx_MBSS_Init(
	IN VOID 				*pAd,
	IN PNET_DEV				pDevMain)
{
	RTMP_OS_NETDEV_OP_HOOK	netDevHook;

	NdisZeroMemory(&netDevHook, sizeof(netDevHook));
	netDevHook.open = MBSS_VirtualIF_Open;	/* device opem hook point */
	netDevHook.stop = MBSS_VirtualIF_Close;	/* device close hook point */
	netDevHook.xmit = MBSS_VirtualIF_PacketSend;	/* hard transmit hook point */
	netDevHook.ioctl = MBSS_VirtualIF_Ioctl;	/* ioctl hook point */
	netDevHook.get_stats = RT28xx_get_ether_stats;

	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MBSS_INIT,
						0, &netDevHook, 0);

}


/*
========================================================================
Routine Description:
    Remove Multi-BSS network interface.

Arguments:
	pAd			points to our adapter

Return Value:
    None

Note:
    FIRST_MBSSID = 1
    Main BSS is not removed here.
========================================================================
*/
VOID RT28xx_MBSS_Remove(
	IN VOID 			*pAd)
{

	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MBSS_REMOVE, 0, NULL, 0);

} /* End of RT28xx_MBSS_Remove */



/* --------------------------------- Private -------------------------------- */
/*
========================================================================
Routine Description:
    Open a virtual network interface.

Arguments:
	pDev			which WLAN network interface

Return Value:
    0: open successfully
    otherwise: open fail

Note:
========================================================================
*/
INT MBSS_VirtualIF_Open(
	IN	PNET_DEV	pDev)
{
	VOID *pAd;


	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> MBSSVirtualIF_open\n", RTMP_OS_NETDEV_GET_DEVNAME(pDev)));

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);

	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MBSS_OPEN, 0, pDev, 0);

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();

	RTMP_OS_NETDEV_START_QUEUE(pDev);

	return 0;
} /* End of MBSS_VirtualIF_Open */


/*
========================================================================
Routine Description:
    Close a virtual network interface.

Arguments:
    pDev           which WLAN network interface

Return Value:
    0: close successfully
    otherwise: close fail

Note:
========================================================================
*/
INT MBSS_VirtualIF_Close(
	IN	PNET_DEV	pDev)
{
	VOID *pAd;


	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> MBSSVirtualIF_close\n", RTMP_OS_NETDEV_GET_DEVNAME(pDev)));

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	
	RTMP_OS_NETDEV_STOP_QUEUE(pDev);

	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_MBSS_CLOSE, 0, pDev, 0);

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();
	return 0;
} /* End of MBSS_VirtualIF_Close */


/*
========================================================================
Routine Description:
    Send a packet to WLAN.

Arguments:
	pPktSrc			points to our adapter
	pDev			which WLAN network interface

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
INT MBSS_VirtualIF_PacketSend(
	IN PNDIS_PACKET			pPktSrc, 
	IN PNET_DEV				pDev)
{

	MEM_DBG_PKT_ALLOC_INC(pPktSrc);

	if(!(RTMP_OS_NETDEV_STATE_RUNNING(pDev)))
	{
		/* the interface is down */
		RELEASE_NDIS_PACKET(NULL, pPktSrc, NDIS_STATUS_FAILURE);
		return 0;
	} /* End of if */

	return MBSS_PacketSend(pPktSrc, pDev, rt28xx_packet_xmit);
} /* End of MBSS_VirtualIF_PacketSend */


/*
========================================================================
Routine Description:
    IOCTL to WLAN.

Arguments:
	pDev			which WLAN network interface
	pIoCtrl			command information
	Command			command ID

Return Value:
    0: IOCTL successfully
    otherwise: IOCTL fail

Note:
    SIOCETHTOOL     8946    New drivers use this ETHTOOL interface to
                            report link failure activity.
========================================================================
*/
INT MBSS_VirtualIF_Ioctl(
	IN PNET_DEV				pDev, 
	IN OUT VOID 			*pIoCtrl, 
	IN INT 					Command)
{
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	ASSERT(pAd);

	if (!pAd)
		return -EINVAL;

/*	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
		return -ENETDOWN;
	/* End of if */

	/* do real IOCTL */
	return rt28xx_ioctl(pDev, pIoCtrl, Command);
} /* End of MBSS_VirtualIF_Ioctl */

#endif /* MBSS_SUPPORT */

/* End of ap_mbss_inf.c */
