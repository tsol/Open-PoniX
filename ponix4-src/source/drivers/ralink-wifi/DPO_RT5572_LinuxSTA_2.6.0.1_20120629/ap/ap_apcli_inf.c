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

#ifdef APCLI_SUPPORT

/*#include "rt_config.h" */
#include "rtmp_comm.h"
#include "rt_os_util.h"
#include "rt_os_net.h"


/*
========================================================================
Routine Description:
    Init AP-Client function.

Arguments:
    pAd            points to our adapter
    main_dev_p      points to the main BSS network interface

Return Value:
    None

Note:
	1. Only create and initialize virtual network interfaces.
	2. No main network interface here.
========================================================================
*/
VOID RT28xx_ApCli_Init(
	IN VOID 				*pAd,
	IN PNET_DEV				main_dev_p)
{

	RTMP_OS_NETDEV_OP_HOOK	netDevOpHook;

	/* init operation functions */
	NdisZeroMemory(&netDevOpHook, sizeof(RTMP_OS_NETDEV_OP_HOOK));
	netDevOpHook.open = ApCli_VirtualIF_Open;
	netDevOpHook.stop = ApCli_VirtualIF_Close;
	netDevOpHook.xmit = ApCli_VirtualIF_PacketSend;
	netDevOpHook.ioctl = ApCli_VirtualIF_Ioctl;
	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_APC_INIT,
						0, &netDevOpHook, 0);
}

/*
========================================================================
Routine Description:
    Open a virtual network interface.

Arguments:
    dev_p           which WLAN network interface

Return Value:
    0: open successfully
    otherwise: open fail

Note:
========================================================================
*/
INT ApCli_VirtualIF_Open(
	IN PNET_DEV		dev_p)
{
/*	UCHAR ifIndex; */
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(dev_p);
	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(dev_p)));

	if (VIRTUAL_IF_UP(pAd) != 0)
		return -1;

	/* increase MODULE use count */
	RT_MOD_INC_USE_COUNT();


	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_APC_OPEN, 0, dev_p, 0);

	return 0;
} /* End of ApCli_VirtualIF_Open */


/*
========================================================================
Routine Description:
    Close a virtual network interface.

Arguments:
    dev_p           which WLAN network interface

Return Value:
    0: close successfully
    otherwise: close fail

Note:
========================================================================
*/
INT ApCli_VirtualIF_Close(
	IN	PNET_DEV	dev_p)
{
/*	UCHAR ifIndex; */
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(dev_p);
	ASSERT(pAd);

	DBGPRINT(RT_DEBUG_TRACE, ("%s: ===> %s\n", __FUNCTION__, RTMP_OS_NETDEV_GET_DEVNAME(dev_p)));


	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_APC_CLOSE, 0, dev_p, 0);

	VIRTUAL_IF_DOWN(pAd);

	RT_MOD_DEC_USE_COUNT();

	return 0;
} /* End of ApCli_VirtualIF_Close */


/*
========================================================================
Routine Description:
    Send a packet to WLAN.

Arguments:
    skb_p           points to our adapter
    dev_p           which WLAN network interface

Return Value:
    0: transmit successfully
    otherwise: transmit fail

Note:
========================================================================
*/
INT ApCli_VirtualIF_PacketSend(
	IN PNDIS_PACKET 	pPktSrc, 
	IN PNET_DEV			pDev)
{

	MEM_DBG_PKT_ALLOC_INC(pPktSrc);

	if(!(RTMP_OS_NETDEV_STATE_RUNNING(pDev)))
	{
		/* the interface is down */
		RELEASE_NDIS_PACKET(NULL, pPktSrc, NDIS_STATUS_FAILURE);
		return 0;
	} /* End of if */

	return APC_PacketSend(pPktSrc, pDev, rt28xx_packet_xmit);
} /* End of ApCli_VirtualIF_PacketSend */


/*
========================================================================
Routine Description:
    IOCTL to WLAN.

Arguments:
    dev_p           which WLAN network interface
    rq_p            command information
    cmd             command ID

Return Value:
    0: IOCTL successfully
    otherwise: IOCTL fail

Note:
    SIOCETHTOOL     8946    New drivers use this ETHTOOL interface to
                            report link failure activity.
========================================================================
*/
INT ApCli_VirtualIF_Ioctl(
	IN PNET_DEV				dev_p, 
	IN OUT VOID 			*rq_p, 
	IN INT 					cmd)
{
	VOID *pAd;

	pAd = RTMP_OS_NETDEV_GET_PRIV(dev_p);
	ASSERT(pAd);

/*	if (!RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_INTERRUPT_IN_USE)) */
	if (RTMP_DRIVER_IOCTL_SANITY_CHECK(pAd, NULL) != NDIS_STATUS_SUCCESS)
		return -ENETDOWN;

	/* do real IOCTL */
	return (rt28xx_ioctl(dev_p, rq_p, cmd));
} /* End of ApCli_VirtualIF_Ioctl */


/*
========================================================================
Routine Description:
    Remove ApCli-BSS network interface.

Arguments:
    pAd            points to our adapter

Return Value:
    None

Note:
========================================================================
*/
VOID RT28xx_ApCli_Remove(
	IN VOID *pAd)
{
/*	UINT index; */


	RTMP_AP_IoctlHandle(pAd, NULL, CMD_RTPRIV_IOCTL_APC_REMOVE, 0, NULL, 0);


}

#endif /* APCLI_SUPPORT */
