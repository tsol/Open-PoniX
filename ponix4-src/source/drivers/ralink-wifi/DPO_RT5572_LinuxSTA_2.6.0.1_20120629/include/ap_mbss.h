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


#ifndef MODULE_MBSS

#define MBSS_EXTERN    extern

#else

#define MBSS_EXTERN

#endif /* MODULE_MBSS */


/*
	For MBSS, the phy mode is different;
	So MBSS_PHY_MODE_RESET() can help us to adjust the correct mode &
	maximum MCS for the BSS.
*/
#define MBSS_PHY_MODE_RESET(__BssId, __HtPhyMode)				\
	{															\
		UCHAR __PhyMode = pAd->ApCfg.MBSSID[__BssId].PhyMode;	\
		if ((__PhyMode == PHY_11B) &&							\
			(__HtPhyMode.field.MODE != MODE_CCK))				\
		{														\
			__HtPhyMode.field.MODE = MODE_CCK;					\
			__HtPhyMode.field.MCS = 3;							\
		}														\
		else if ((__PhyMode <= PHY_11G) &&						\
				(__PhyMode != PHY_11B) &&						\
				(__HtPhyMode.field.MODE != MODE_OFDM))			\
		{														\
			__HtPhyMode.field.MODE = MODE_OFDM;					\
			__HtPhyMode.field.MCS = 7;							\
		}														\
	}


/* Public function list */
INT	Show_MbssInfo_Display_Proc(
	IN	PRTMP_ADAPTER				pAd,
	IN	PSTRING						arg);

VOID MBSS_Init(
	IN PRTMP_ADAPTER				pAd,
	IN RTMP_OS_NETDEV_OP_HOOK		*pNetDevOps);

VOID MBSS_Remove(
	IN PRTMP_ADAPTER				pAd);

INT MBSS_Open(
	IN	PNET_DEV					pDev);

INT MBSS_Close(
	IN	PNET_DEV					pDev);

INT32 RT28xx_MBSS_IdxGet(
	IN PRTMP_ADAPTER	pAd,
	IN PNET_DEV			pDev);

/* End of ap_mbss.h */
