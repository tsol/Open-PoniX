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


#if defined(DFS_SUPPORT) || defined(CARRIER_DETECTION_SUPPORT)

#include "rt_config.h"

INT	Set_RadarShow_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg)
{

#ifdef CARRIER_DETECTION_SUPPORT
	printk("pAd->CommonCfg.CarrierDetect.CD_State = %d\n", pAd->CommonCfg.CarrierDetect.CD_State);
	printk("pAd->CommonCfg.CarrierDetect.criteria = %d\n", pAd->CommonCfg.CarrierDetect.criteria);
	printk("pAd->CommonCfg.CarrierDetect.Delta = %d\n", pAd->CommonCfg.CarrierDetect.delta);
	printk("pAd->CommonCfg.CarrierDetect.DivFlag = %d\n", pAd->CommonCfg.CarrierDetect.div_flag);
	printk("pAd->CommonCfg.CarrierDetect.Threshold = %d(0x%x)\n", pAd->CommonCfg.CarrierDetect.threshold, pAd->CommonCfg.CarrierDetect.threshold);
#endif /* CARRIER_DETECTION_SUPPORT */

	return TRUE;
}

/*
	========================================================================

	Routine Description:
		Radar channel check routine

	Arguments:
		pAd 	Pointer to our adapter

	Return Value:
		TRUE	need to do radar detect
		FALSE	need not to do radar detect

	========================================================================
*/
BOOLEAN RadarChannelCheck(
	IN PRTMP_ADAPTER	pAd,
	IN UCHAR			Ch)
{
	INT 	i;
	BOOLEAN result = FALSE;

	for (i=0; i<pAd->ChannelListNum; i++)
	{
		if (Ch == pAd->ChannelList[i].Channel)
		{
			result = pAd->ChannelList[i].DfsReq;
			break;
		}
	}

	return result;
}

/*
       ========================================================================
       Routine Description:
               Control CCK_MRC Status
       Arguments:
               pAd     Pointer to our adapter
       Return Value:

       ========================================================================
*/
VOID CckMrcStatusCtrl(IN PRTMP_ADAPTER pAd)
{
}


/*
       ========================================================================
       Routine Description:
               Enhance DFS/CS when using GLRT.
       Arguments:
               pAd     Pointer to our adapter
       Return Value:

       ========================================================================
*/
VOID RadarGLRTCompensate(IN PRTMP_ADAPTER pAd)
{
}
#endif /*defined(DFS_SUPPORT) || defined(CARRIER_DETECTION_SUPPORT) */

