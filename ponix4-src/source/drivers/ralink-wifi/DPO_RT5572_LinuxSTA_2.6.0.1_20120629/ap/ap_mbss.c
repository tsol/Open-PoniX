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


#ifdef MBSS_SUPPORT

#define MODULE_MBSS
#include "rt_config.h"


/* --------------------------------- Public -------------------------------- */
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

extern void RtmpOSNetDevProtect(
	IN BOOLEAN lock_it);

VOID MBSS_Init(
	IN PRTMP_ADAPTER 				pAd,
	IN RTMP_OS_NETDEV_OP_HOOK		*pNetDevOps)
{
#define MBSS_MAX_DEV_NUM	32
	PNET_DEV pDevNew;
	INT32 IdBss, MaxNumBss;
	INT status;
	RTMP_OS_NETDEV_OP_HOOK	netDevHook;

	/* sanity check to avoid redundant virtual interfaces are created */
	if (pAd->FlgMbssInit != FALSE)
		return;
	/* End of if */


	/* init */
	MaxNumBss = pAd->ApCfg.BssidNum;
	if (MaxNumBss > MAX_MBSSID_NUM(pAd))
		MaxNumBss = MAX_MBSSID_NUM(pAd);
	/* End of if */


	/* first IdBss must not be 0 (BSS0), must be 1 (BSS1) */
	for(IdBss=FIRST_MBSSID; IdBss<MAX_MBSSID_NUM(pAd); IdBss++)
		pAd->ApCfg.MBSSID[IdBss].MSSIDDev = NULL;
	/* End of for */

    /* create virtual network interface */
	for(IdBss=FIRST_MBSSID; IdBss<MaxNumBss; IdBss++)
	{
		UINT32 MC_RowID = 0, IoctlIF = 0;
#ifdef MULTIPLE_CARD_SUPPORT
		MC_RowID = pAd->MC_RowID;
#endif /* MULTIPLE_CARD_SUPPORT */
#ifdef HOSTAPD_SUPPORT
		IoctlIF = pAd->IoctlIF;
#endif /* HOSTAPD_SUPPORT */
		pDevNew = RtmpOSNetDevCreate(MC_RowID, &IoctlIF, INT_MBSSID, IdBss, sizeof(PRTMP_ADAPTER), INF_MBSSID_DEV_NAME);
#ifdef HOSTAPD_SUPPORT
		pAd->IoctlIF = IoctlIF;
#endif /* HOSTAPD_SUPPORT */
		if (pDevNew == NULL)
		{
			/* allocation fail, exit */
			pAd->ApCfg.BssidNum = IdBss; /* re-assign new MBSS number */
			DBGPRINT(RT_DEBUG_ERROR,
                     ("Allocate network device fail (MBSS)...\n"));
			break;
		}
		else
		{
			DBGPRINT(RT_DEBUG_TRACE, ("Register MBSSID IF (%s)\n", RTMP_OS_NETDEV_GET_DEVNAME(pDevNew)));
		}

		RTMP_OS_NETDEV_SET_PRIV(pDevNew, pAd);
		
		/* init operation functions and flags */
		NdisCopyMemory(&netDevHook, pNetDevOps, sizeof(netDevHook));

		netDevHook.priv_flags = INT_MBSSID;			/* We are virtual interface */
		netDevHook.needProtcted = TRUE;

		/* Init MAC address of virtual network interface */
		NdisMoveMemory(&netDevHook.devAddr[0], &pAd->ApCfg.MBSSID[IdBss].Bssid[0], MAC_ADDR_LEN);

		/* backup our virtual network interface */
		pAd->ApCfg.MBSSID[IdBss].MSSIDDev = pDevNew;
		
		/* register this device to OS */
		status = RtmpOSNetDevAttach(pAd->OpMode, pDevNew, &netDevHook);

	}

	pAd->FlgMbssInit = TRUE;

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
VOID MBSS_Remove(
	IN PRTMP_ADAPTER 	pAd)
{
	MULTISSID_STRUCT *pMbss;
	UINT IdBss;



	for(IdBss=FIRST_MBSSID; IdBss<MAX_MBSSID_NUM(pAd); IdBss++)
	{
		pMbss = &pAd->ApCfg.MBSSID[IdBss];
		RtmpOSNetDevProtect(1);
		if (pMbss->MSSIDDev)
		{
			RtmpOSNetDevDetach(pMbss->MSSIDDev);
			RtmpOSNetDevProtect(0);
			RtmpOSNetDevFree(pMbss->MSSIDDev);

			RtmpOSNetDevProtect(1);
			/* clear it as NULL to prevent latter access error */
			pMbss->MSSIDDev = NULL;
		}
		RtmpOSNetDevProtect(0);
	}
} /* End of RT28xx_MBSS_Remove */


/*
========================================================================
Routine Description:
    Get multiple bss idx.

Arguments:
	pAd				points to our adapter
	pDev			which WLAN network interface

Return Value:
    0: close successfully
    otherwise: close fail

Note:
========================================================================
*/
INT32 RT28xx_MBSS_IdxGet(
	IN PRTMP_ADAPTER	pAd,
	IN PNET_DEV			pDev)
{
	INT32 BssId = -1;
	INT32 IdBss;


	for(IdBss=0; IdBss<pAd->ApCfg.BssidNum; IdBss++)
	{
		if (pAd->ApCfg.MBSSID[IdBss].MSSIDDev == pDev)
		{
			BssId = IdBss;
			break;
		}
	}

	return BssId;
}


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
INT MBSS_Open(
	IN	PNET_DEV		pDev)
{
	PRTMP_ADAPTER pAd;
	INT BssId;

	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	BssId = RT28xx_MBSS_IdxGet(pAd, pDev);
    if (BssId < 0)
        return -1;
    
    pAd->ApCfg.MBSSID[BssId].bBcnSntReq = TRUE;
	return 0;
} /* End of MBSS_Open */


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
INT MBSS_Close(
	IN	PNET_DEV		pDev)
{
	PRTMP_ADAPTER pAd;
	INT BssId;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	BssId = RT28xx_MBSS_IdxGet(pAd, pDev);
    if (BssId < 0)
        return -1;

	RTMP_OS_NETDEV_STOP_QUEUE(pDev);

	/* kick out all stas behind the Bss */
	MbssKickOutStas(pAd, BssId, REASON_DISASSOC_INACTIVE);

	pAd->ApCfg.MBSSID[BssId].bBcnSntReq = FALSE;

	APMakeAllBssBeacon(pAd);
	APUpdateAllBeaconFrame(pAd);

	return 0;
} /* End of MBSS_Close */


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
int MBSS_PacketSend(
	IN	PNDIS_PACKET				pPktSrc, 
	IN	PNET_DEV					pDev,
	IN	RTMP_NET_PACKET_TRANSMIT	Func)
{
    RTMP_ADAPTER     *pAd;
    MULTISSID_STRUCT *pMbss;
    PNDIS_PACKET     pPkt = (PNDIS_PACKET)pPktSrc;
    INT              IdBss;


	pAd = RTMP_OS_NETDEV_GET_PRIV(pDev);
	ASSERT(pAd);


	if ((RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_BSS_SCAN_IN_PROGRESS)) ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RADIO_OFF))          ||
		(RTMP_TEST_FLAG(pAd, fRTMP_ADAPTER_RESET_IN_PROGRESS)))
	{
		/* wlan is scanning/disabled/reset */
		RELEASE_NDIS_PACKET(pAd, pPkt, NDIS_STATUS_FAILURE);
		return 0;
	} /* End of if */


    /* 0 is main BSS, dont handle it here */
    /* FIRST_MBSSID = 1 */
    pMbss = pAd->ApCfg.MBSSID;

    for(IdBss=FIRST_MBSSID; IdBss<pAd->ApCfg.BssidNum; IdBss++)
    {
        /* find the device in our MBSS list */
        if (pMbss[IdBss].MSSIDDev == pDev)
		{
/*			NdisZeroMemory((PUCHAR)&(RTPKT_TO_OSPKT(pPktSrc))->cb[CB_OFF], 15); */
			NdisZeroMemory((PUCHAR)(GET_OS_PKT_CB(pPktSrc) + CB_OFF), 15);
            RTMP_SET_PACKET_NET_DEVICE_MBSSID(pPktSrc, IdBss);
/*			SET_OS_PKT_NETDEV(pPktSrc, pDev); */
		 

            /* transmit the packet */
            return Func(pPktSrc);
		}
	}

    /* can not find the BSS so discard the packet */
	RELEASE_NDIS_PACKET(pAd, pPkt, NDIS_STATUS_FAILURE);

    return 0;
} /* End of MBSS_PacketSend */


#endif /* MBSS_SUPPORT */

/* End of ap_mbss.c */
