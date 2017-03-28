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


#ifndef __DRS_EXTR_H__
#define __DRS_EXTR_H__


/* group rate switch */
#ifdef NEW_RATE_ADAPT_SUPPORT

extern UCHAR RateSwitchTable11N3S_Group[];

/* MACRO */
#define RTMP_DRS_GROUP_ENABLE(__pAd, __pEntry)								\
{																			\
	MlmeSetMcsGroup((__pAd), (__pEntry));									\
	(__pEntry)->lastRateIdx = 1;											\
	(__pEntry)->fewPktsCnt = 0;												\
	(__pEntry)->perThrdAdj = PER_THRD_ADJ;									\
	(__pEntry)->useNewRateAdapt = 1;										\
}

#define RTMP_DRS_GROUP_IS_USED(__pAd, __pEntry)								\
	(((__pAd)->chipCap.DrsMethod == RTMP_CHIP_DRS_GROUP) &&					\
	((__pEntry)->useNewRateAdapt == 1))

#define RTMP_DRS_GROUP_TABLE_3S_SELECT(__ppTable, __pTableSize, __pInitTxRateIdx)	\
{																			\
	*(__ppTable) = RateSwitchTable11N3S_Group;								\
	*(__pTableSize) = RateSwitchTable11N3S_Group[0];						\
	*(__pInitTxRateIdx) = RateSwitchTable11N3S_Group[1];					\
}

#define RTMP_DRS_GROUP_TABLE_2S_SELECT(__ppTable, __pTableSize, __pInitTxRateIdx)	\
{																			\
	*(__ppTable) = RateSwitchTable11N3S_Group;								\
	*(__pTableSize) = RateSwitchTable11N3S_Group[0];						\
	*(__pInitTxRateIdx) = 0x0f;												\
}

#define RTMP_DRS_GROUP_TABLE_1S_SELECT(__ppTable, __pTableSize, __pInitTxRateIdx)	\
{																			\
	*(__ppTable) = RateSwitchTable11N3S_Group;								\
	*(__pTableSize) = RateSwitchTable11N3S_Group[0];						\
	*(__pInitTxRateIdx) = 0x07;												\
}

#define RTMP_DRS_GROUP_ALG_IS_SELECTED(__pTable)							\
	((__pTable) == RateSwitchTable11N3S_Group)

#define RTMP_DRS_GROUP_TX_RATE_INIT(__pEntry, __pTable, __pTxRate)			\
	(__pTxRate) = (PRTMP_TX_RATE_SWITCH) &(__pTable)[((__pEntry)->CurrTxRateIndex+1)*10];

#define PER_THRD_ADJ			1
#define FEW_PKTS_CNT_THRD		1

/* FUNCTION */

#ifdef CONFIG_STA_SUPPORT
VOID StaQuickResponeForRateUpExecAdapt(
	IN PRTMP_ADAPTER			pAd,
	IN ULONG					i);
#endif /* CONFIG_STA_SUPPORT */

VOID MlmeSetMcsGroup(
	IN PRTMP_ADAPTER 			pAd,
	OUT PMAC_TABLE_ENTRY		pEntry);

INT	Set_UseNewRateAdapt_Proc(
	IN	PRTMP_ADAPTER			pAd, 
	IN	PSTRING					arg);

INT	Set_PerThrdAdj_Proc(
	IN	PRTMP_ADAPTER			pAd,
	IN	PSTRING					arg);

VOID MlmeDynamicTxRateSwitchingAdapt(
	IN PRTMP_ADAPTER pAd,
	IN ULONG i,
	IN ULONG TxSuccess,
	IN ULONG TxRetransmit,
	IN ULONG TxFailCount);

#else

#define RTMP_DRS_GROUP_ENABLE(__pAd, __pEntry)

#endif /* NEW_RATE_ADAPT_SUPPORT */


/* normal rate switch */
#define RTMP_DRS_ALG_INIT(__pAd, __Alg)										\
	(__pAd)->chipCap.DrsMethod = __Alg;

#define RTMP_DRS_ENTRY_INIT(__pAd, __pEntry)								\
	if ((__pAd)->chipCap.DrsMethod == RTMP_CHIP_DRS_GROUP)					\
	{																		\
		RTMP_DRS_GROUP_ENABLE(__pAd, __pEntry);								\
	}																		\
	else																	\
	{																		\
		/* default rate switch parameters */								\
		(__pEntry)->useNewRateAdapt = 0;									\
	}

#endif /* __DRS_EXTR_H__ */

/* End of drs_extr.h */
