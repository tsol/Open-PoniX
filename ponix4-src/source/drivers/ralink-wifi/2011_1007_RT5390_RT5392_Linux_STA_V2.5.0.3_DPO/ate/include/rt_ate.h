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


#ifndef __RT_ATE_H__
#define __RT_ATE_H__

#if defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) || defined(RT5592)
#define	TXPowerEnMask		0xA8;
#define	RXPowerEnMask		0x54;
#endif /* defined(RT5370) || defined(RT5372) || defined(RT5390) || defined(RT5392) || defined(RT5592) */

#ifdef RTMP_INTERNAL_TX_ALC
#define EEPROM_TSSI_ENABLE 0x36
#define EEPROM_TSSI_MODE_EXTEND 0x76

#define ATE_MDSM_NORMAL_TX_POWER						0x00
#define ATE_MDSM_DROP_TX_POWER_BY_6dBm					0x01
#define ATE_MDSM_DROP_TX_POWER_BY_12dBm					0x02
#define ATE_MDSM_ADD_TX_POWER_BY_6dBm					0x03
#define ATE_MDSM_BBP_R1_STATIC_TX_POWER_CONTROL_MASK	0x03
#endif /* RTMP_INTERNAL_TX_ALC */

#define	LEN_OF_ARG 16
#define ATE_ON(_p)              (((_p)->ate.Mode) != ATE_STOP)
#define TX_METHOD_0 0 /* Early chipsets must be applied this original TXCONT/TXCARR/TXCARS mechanism. */
#define TX_METHOD_1 1 /* Default TXCONT/TXCARR/TXCARS mechanism is TX_METHOD_1 */

#define ATE_MAC_TX_ENABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 2);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_TX_DISABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 2);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_RX_ENABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 3);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

#define ATE_MAC_RX_DISABLE(_A, _I, _pV)	\
{										\
	RTMP_IO_READ32(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 3);				\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));	\
}

/* Set MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 1. */
#define ATE_MAC_TX_CTS_ENABLE(_A, _I, _pV)	\
{											\
	RTMP_IO_READ32(_A, _I, _pV);			\
	(*(_pV)) |= (1 << 4);					\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));		\
}

/* Clear MAC_SYS_CTRL(0x1004) Continuous Tx Production Test (bit4) = 0. */
#define ATE_MAC_TX_CTS_DISABLE(_A, _I, _pV)	\
{											\
	RTMP_IO_READ32(_A, _I, _pV);			\
	(*(_pV)) &= ~(1 << 4);					\
	RTMP_IO_WRITE32(_A, _I, (*(_pV)));		\
}

/* Clear BBP R22 to reset Tx Mode (bit7~bit0) = 0. */
#define ATE_BBP_RESET_TX_MODE(_A, _I, _pV)			\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= ~(0xFF);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Set BBP R22 to start Continuous Tx Mode (bit7) = 1. */
#define ATE_BBP_START_CTS_TX_MODE(_A, _I, _pV)		\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 7);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Clear BBP R22 to stop Continuous Tx Mode (bit7) = 0. */
#define ATE_BBP_STOP_CTS_TX_MODE(_A, _I, _pV)		\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 7);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Set BBP R24 to send out Continuous Tx sin wave (bit0) = 1. */
#define ATE_BBP_CTS_TX_SIN_WAVE_ENABLE(_A, _I, _pV)	\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) |= (1 << 0);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/* Clear BBP R24 to stop Continuous Tx sin wave (bit0) = 0. */
#define ATE_BBP_CTS_TX_SIN_WAVE_DISABLE(_A, _I, _pV)	\
{													\
	ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV);		\
	(*(_pV)) &= ~(1 << 0);							\
	ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, (*(_pV)));	\
}

/*
==========================================================================
	Description:
		This routine sets initial value of VGA in the RX chain.
		AGC is the abbreviation of "Automatic Gain Controller",
		while VGA (Variable Gain Amplifier) is a part of AGC loop.
		(i.e., VGA + level detector + feedback loop = AGC)

    Return:
        VOID
==========================================================================
*/
#define ATE_CHIP_RX_VGA_GAIN_INIT(__pAd)								\
	if (__pAd->ate.pChipStruct->RxVGAInit != NULL)	\
		__pAd->ate.pChipStruct->RxVGAInit(__pAd)

#ifdef RTMP_MAC_PCI
NTSTATUS	ATE_RT3562WriteBBPR66(
	IN	PRTMP_ADAPTER	pAd,
	IN	UCHAR			Value);

#define ATEPCIReadBBPRegister(_A, _I, _pV)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             j, k;                               \
    for (j=0; j<MAX_BUSY_COUNT; j++)                    \
    {                                                   \
        RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word);     \
        if (BbpCsr.field.Busy == BUSY)                  \
        {                                               \
            continue;                                   \
        }                                               \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 1;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, BBP_CSR_CFG, BbpCsr.word);     \
        for (k=0; k<MAX_BUSY_COUNT; k++)                \
        {                                               \
            RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word); \
            if (BbpCsr.field.Busy == IDLE)              \
                break;                                  \
        }                                               \
        if ((BbpCsr.field.Busy == IDLE) &&              \
            (BbpCsr.field.RegNum == _I))                \
        {                                               \
            *(_pV) = (UCHAR)BbpCsr.field.Value;         \
            break;                                      \
        }                                               \
    }                                                   \
    if (BbpCsr.field.Busy == BUSY)                      \
    {                                                   \
        DBGPRINT_ERR(("BBP read R%d fail\n", _I));      \
        *(_pV) = (_A)->BbpWriteLatch[_I];               \
    }                                                   \
}

#define ATEPCIWriteBBPRegister(_A, _I, _V)        \
{                                                       \
    BBP_CSR_CFG_STRUC  BbpCsr;                             \
    int             BusyCnt;                            \
    for (BusyCnt=0; BusyCnt<MAX_BUSY_COUNT; BusyCnt++)  \
    {                                                   \
        RTMP_IO_READ32(_A, BBP_CSR_CFG, &BbpCsr.word);     \
        if (BbpCsr.field.Busy == BUSY)                  \
            continue;                                   \
        BbpCsr.word = 0;                                \
        BbpCsr.field.fRead = 0;                         \
        BbpCsr.field.BBP_RW_MODE = 1;                         \
        BbpCsr.field.Busy = 1;                          \
        BbpCsr.field.Value = _V;                        \
        BbpCsr.field.RegNum = _I;                       \
        RTMP_IO_WRITE32(_A, BBP_CSR_CFG, BbpCsr.word);     \
        (_A)->BbpWriteLatch[_I] = _V;                   \
        break;                                          \
    }                                                   \
    if (BusyCnt == MAX_BUSY_COUNT)                      \
    {                                                   \
        DBGPRINT_ERR(("BBP write R%d fail\n", _I));     \
    }                                                   \
}

#define ATE_BBP_IO_READ8_BY_REG_ID(_A, _I, _pV)		ATEPCIReadBBPRegister(_A, _I, _pV)

#define ATE_BBP_IO_WRITE8_BY_REG_ID(_A, _I, _V)		ATEPCIWriteBBPRegister(_A, _I, _V)

#endif /* RTMP_MAC_PCI */


INT DefaultATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index);





#if defined(RT28xx) || defined(RT2880)
VOID RT28xxATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd);

INT RT28xxATETxPwrHandler(
	IN PRTMP_ADAPTER pAd,
	IN char index);
#endif /* defined(RT28xx) || defined(RT2880) */


#ifdef RALINK_QA
VOID ATE_QA_Statistics(
	IN PRTMP_ADAPTER		pAd,
	IN PRXWI_STRUC			pRxWI,
	IN PRT28XX_RXD_STRUC    p28xxRxD,
	IN PHEADER_802_11		pHeader);
	
INT RtmpDoAte(
	IN	PRTMP_ADAPTER	pAd, 
	IN	RTMP_IOCTL_INPUT_STRUCT	*wrq,
	IN	PSTRING	wrq_name);

INT Set_TxStop_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_RxStop_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

#ifdef DBG
INT Set_EERead_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_EEWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_BBPRead_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_BBPWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_RFWrite_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* DBG */ 
#endif /* RALINK_QA */


#ifdef RTMP_RF_RW_SUPPORT
#define ATE_RF_IO_READ8_BY_REG_ID(_A, _I, _pV)     RT30xxReadRFRegister(_A, _I, _pV)
#define ATE_RF_IO_WRITE8_BY_REG_ID(_A, _I, _V)     RT30xxWriteRFRegister(_A, _I, _V)
#endif /* RTMP_RF_RW_SUPPORT */

#ifdef RALINK_QA
#define SYNC_CHANNEL_WITH_QA(_A, _pV)\
	if ((_A->bQATxStart == TRUE) || (_A->bQARxStart == TRUE))\
	{\
		return;\
	}\
	else\
		*_pV = _A->Channel
#else
#define SYNC_CHANNEL_WITH_QA(_A, _pV)\
	*_pV = _A->Channel
#endif /* RALINK_QA */

VOID rt_ee_read_all(
	IN  PRTMP_ADAPTER   pAd,
	OUT USHORT *Data);

VOID rt_ee_write_all(
	IN  PRTMP_ADAPTER   pAd,
	IN  USHORT *Data);

VOID rt_ee_write_bulk(
	IN  PRTMP_ADAPTER	pAd,
	IN  USHORT *Data,
	IN  USHORT offset,
	IN  USHORT length);

INT Set_ATE_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_DA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_SA_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_BSSID_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_CHANNEL_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_INIT_CHAN_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT Set_ADCDump_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);
#ifdef RTMP_INTERNAL_TX_ALC
INT Set_ATE_TSSI_CALIBRATION_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT Set_ATE_TSSI_CALIBRATION_EX_Proc(
	IN      PRTMP_ADAPTER   pAd,
	IN      PSTRING                 arg);


#if defined(RT3350) || defined(RT3352)
CHAR InsertTssi(
	IN UCHAR InChannel,
	IN UCHAR Channel0,
	IN UCHAR Channel1,
	IN CHAR Tssi0,
	IN CHAR Tssi1);

INT RT335xATETssiCalibrationExtend(
	IN struct _RTMP_ADAPTER		*pAd,
	IN PSTRING					arg);
#endif /* defined(RT3350) || defined(RT3352) */

CHAR ATEGetDesiredTSSI(
	IN PRTMP_ADAPTER		pAd);

#endif /* RTMP_INTERNAL_TX_ALC */

#ifdef HW_ANTENNA_DIVERSITY_SUPPORT
INT Set_ATE_DIV_ANTENNA_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT Set_ATE_DIV_ANTENNA_CALIBRATION_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);
#endif /* HW_ANTENNA_DIVERSITY_SUPPORT */

#ifdef RTMP_TEMPERATURE_COMPENSATION
INT Set_ATE_READ_EXTERNAL_TSSI_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);
#endif /* RTMP_TEMPERATURE_COMPENSATION */

INT	Set_ATE_TX_POWER0_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TX_POWER1_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

#ifdef DOT11N_SS3_SUPPORT
INT	Set_ATE_TX_POWER2_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);
#endif /* DOT11N_SS3_SUPPORT */

INT	Set_ATE_TX_Antenna_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT	Set_ATE_RX_Antenna_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

#ifdef RT3350
INT	Set_ATE_PA_Bias_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* RT3350 */

INT	Default_Set_ATE_TX_FREQ_OFFSET_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);





#if defined(RT28xx) || defined(RT2880)
INT	RT28xx_Set_ATE_TX_FREQ_OFFSET_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* defined(RT28xx) || defined(RT2880) */


INT	Set_ATE_TX_FREQ_OFFSET_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Default_Set_ATE_TX_BW_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);





#if defined(RT28xx) || defined(RT2880)
INT	RT28xx_Set_ATE_TX_BW_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* defined(RT28xx) || defined(RT2880) */


INT	Set_ATE_TX_BW_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TX_LENGTH_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TX_COUNT_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TX_MCS_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT	Set_ATE_TX_MODE_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT	Set_ATE_TX_GI_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);


INT	Set_ATE_RX_FER_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Read_RF_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

#ifndef RTMP_RF_RW_SUPPORT
INT Set_ATE_Write_RF1_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Write_RF2_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Write_RF3_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Write_RF4_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* RTMP_RF_RW_SUPPORT */

INT Set_ATE_Load_E2P_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Read_E2P_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

#ifdef LED_CONTROL_SUPPORT
#endif /* LED_CONTROL_SUPPORT */

INT	Set_ATE_AUTO_ALC_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_IPG_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_Payload_Proc(
    IN  PRTMP_ADAPTER   pAd, 
    IN  PSTRING         arg);

#ifdef TXBF_SUPPORT
INT	Set_ATE_TXBF_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TXSOUNDING_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_TXBF_DIVCAL_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT	Set_ATE_TXBF_LNACAL_Proc(
	IN	PRTMP_ADAPTER	pAd,
	IN	PSTRING			arg);

INT Set_ATE_TXBF_INIT_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_TXBF_CAL_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_TXBF_GOLDEN_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_TXBF_VERIFY_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_TXBF_VERIFY_NoComp_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT Set_ATE_ForceBBP_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);
#endif /* TXBF_SUPPORT */

INT	Set_ATE_Show_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

INT	Set_ATE_Help_Proc(
	IN	PRTMP_ADAPTER	pAd, 
	IN	PSTRING			arg);

VOID DefaultATEAsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd);


VOID ATEAsicAdjustTxPower(
	IN PRTMP_ADAPTER pAd);

VOID ATESampleRssi(
	IN PRTMP_ADAPTER	pAd,
	IN PRXWI_STRUC		pRxWI);	

#ifdef RTMP_MAC_PCI
INT TxDmaBusy(
	IN PRTMP_ADAPTER pAd);

INT RxDmaBusy(
	IN PRTMP_ADAPTER pAd);

VOID RtmpDmaEnable(
	IN PRTMP_ADAPTER pAd,
	IN INT Enable);

INT ATESetUpFrame(
	IN PRTMP_ADAPTER pAd,
	IN UINT32 TxIdx);
#endif /* RTMP_MAC_PCI */


#ifdef RT33xx
INT Set_ATE_TX_EVM_CALIBRATION_Show_Proc(
 IN PRTMP_ADAPTER pAd,
 IN PSTRING   arg);
 
INT Set_ATE_TX_EVM_CALIBRATION_Proc(
 IN PRTMP_ADAPTER pAd,
 IN PSTRING   arg);
 
INT Set_ATE_TX_EVM_CALIBRATION_Fill_Proc(
 IN PRTMP_ADAPTER pAd,
 IN PSTRING   arg);
#endif /* RT33xx */

NDIS_STATUS ChipStructAssign(
 IN	PRTMP_ADAPTER pAd);

NDIS_STATUS ATEInit(
 IN PRTMP_ADAPTER pAd);

#ifdef RALINK_QA
VOID ReadQATxTypeFromBBP(
 IN	PRTMP_ADAPTER	pAd);
#endif /* RALINK_QA */

NDIS_STATUS ATEBBPWriteWithRxChain(
 IN RTMP_ADAPTER *pAd,
 IN UCHAR bbpId,
 IN CHAR bbpVal,
 IN RX_CHAIN_IDX rx_ch_idx);





#if defined(RT28xx) || defined(RT2880)
VOID RT28xxATERxVGAInit(
 IN PRTMP_ADAPTER		pAd);
#endif /* defined(RT28xx) || defined(RT2880) */


VOID  ATEPeriodicExec(
	IN PVOID SystemSpecific1, 
	IN PVOID FunctionContext, 
	IN PVOID SystemSpecific2, 
	IN PVOID SystemSpecific3);

VOID ATEAsicSetTxRxPath(
    IN PRTMP_ADAPTER pAd);

VOID RtmpRfIoWrite(
	IN PRTMP_ADAPTER pAd);

VOID ATEAsicSwitchChannel(
    IN PRTMP_ADAPTER pAd);

VOID BbpSoftReset(
	IN PRTMP_ADAPTER pAd);


#endif /* __RT_ATE_H__ */

