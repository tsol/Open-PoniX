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


#include "rt_config.h"

extern UCHAR	CISCO_OUI[];

extern UCHAR	WPA_OUI[];
extern UCHAR	RSN_OUI[];
extern UCHAR	WME_INFO_ELEM[];
extern UCHAR	WME_PARM_ELEM[];
extern UCHAR	RALINK_OUI[];

extern UCHAR 	BROADCOM_OUI[]; 
extern UCHAR    WPS_OUI[];

/* 
    ==========================================================================
    Description:
        MLME message sanity check
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
 */

BOOLEAN PeerAssocReqCmmSanity(
    IN PRTMP_ADAPTER pAd, 
	IN BOOLEAN isReassoc,
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2,
    OUT USHORT *pCapabilityInfo, 
    OUT USHORT *pListenInterval, 
    OUT PUCHAR pApAddr,
    OUT UCHAR *pSsidLen,
    OUT char *Ssid,
    OUT UCHAR *pRatesLen,
    OUT UCHAR Rates[],
    OUT UCHAR *RSN,
    OUT UCHAR *pRSNLen,
    OUT BOOLEAN *pbWmmCapable,
	OUT ULONG  *pRalinkIe,
	OUT EXT_CAP_INFO_ELEMENT *pExtCapInfo,
    OUT UCHAR		 *pHtCapabilityLen,
    OUT HT_CAPABILITY_IE *pHtCapability)
{
    CHAR			*Ptr;
    PFRAME_802_11	Fr = (PFRAME_802_11)Msg;
    PEID_STRUCT		eid_ptr;
    UCHAR			Sanity = 0;
    UCHAR			WPA1_OUI[4] = { 0x00, 0x50, 0xF2, 0x01 };
    UCHAR			WPA2_OUI[3] = { 0x00, 0x0F, 0xAC };
    MAC_TABLE_ENTRY *pEntry = (MAC_TABLE_ENTRY *)NULL;

    /* to prevent caller from using garbage output value */
	*pSsidLen     = 0;
    *pRatesLen    = 0;
    *pRSNLen      = 0;
    *pbWmmCapable = FALSE;
    *pRalinkIe    = 0;
    *pHtCapabilityLen= 0;	

    COPY_MAC_ADDR(pAddr2, &Fr->Hdr.Addr2);

	pEntry = MacTableLookup(pAd, pAddr2);
	if (pEntry == NULL)
		return FALSE;




    Ptr = (PCHAR)Fr->Octet;

    NdisMoveMemory(pCapabilityInfo, &Fr->Octet[0], 2);
    NdisMoveMemory(pListenInterval, &Fr->Octet[2], 2);

    if (isReassoc) 
	{
		NdisMoveMemory(pApAddr, &Fr->Octet[4], 6);
		eid_ptr = (PEID_STRUCT) &Fr->Octet[10];
	}
	else
	{
		eid_ptr = (PEID_STRUCT) &Fr->Octet[4];
	}


    /* get variable fields from payload and advance the pointer */
    while (((UCHAR *)eid_ptr + eid_ptr->Len + 1) < ((UCHAR *)Fr + MsgLen))
    {
        switch(eid_ptr->Eid)
        {
            case IE_SSID:
				if (((Sanity&0x1) == 1))
					break;

                if ((eid_ptr->Len <= MAX_LEN_OF_SSID))
                {
                    Sanity |= 0x01;
                    NdisMoveMemory(Ssid, eid_ptr->Octet, eid_ptr->Len);
                    *pSsidLen = eid_ptr->Len;
                    DBGPRINT(RT_DEBUG_TRACE,
						("PeerAssocReqSanity - SsidLen = %d  \n", *pSsidLen));
                }
                else
                {
                    DBGPRINT(RT_DEBUG_TRACE,
						("PeerAssocReqSanity - wrong IE_SSID\n"));
                    return FALSE;
                }
                break;

            case IE_SUPP_RATES:
                if ((eid_ptr->Len <= MAX_LEN_OF_SUPPORTED_RATES) &&
					(eid_ptr->Len > 0))
                {
                    Sanity |= 0x02;
                    NdisMoveMemory(Rates, eid_ptr->Octet, eid_ptr->Len);

                    DBGPRINT(RT_DEBUG_TRACE,
						("PeerAssocReqSanity - IE_SUPP_RATES., Len=%d. "
						"Rates[0]=%x\n", eid_ptr->Len, Rates[0]));
                    DBGPRINT(RT_DEBUG_TRACE,
						("Rates[1]=%x %x %x %x %x %x %x\n",
						Rates[1], Rates[2], Rates[3],
						Rates[4], Rates[5], Rates[6],
						Rates[7]));

                    *pRatesLen = eid_ptr->Len;
                }
                else
                {
					UCHAR RateDefault[8] = \
							{ 0x82, 0x84, 0x8b, 0x96, 0x12, 0x24, 0x48, 0x6c };

                	/* HT rate not ready yet. return true temporarily. rt2860c */
                    /*DBGPRINT(RT_DEBUG_TRACE, ("PeerAssocReqSanity - wrong IE_SUPP_RATES\n")); */
                    Sanity |= 0x02;
                    *pRatesLen = 8;
					NdisMoveMemory(Rates, RateDefault, 8);

                    DBGPRINT(RT_DEBUG_TRACE,
						("PeerAssocReqSanity - wrong IE_SUPP_RATES., Len=%d\n",
						eid_ptr->Len));
                }
                break;

            case IE_EXT_SUPP_RATES:
                if (eid_ptr->Len + *pRatesLen <= MAX_LEN_OF_SUPPORTED_RATES)
                {
                    NdisMoveMemory(&Rates[*pRatesLen], eid_ptr->Octet,
									eid_ptr->Len);
                    *pRatesLen = (*pRatesLen) + eid_ptr->Len;
                }
                else
                {
                    NdisMoveMemory(&Rates[*pRatesLen], eid_ptr->Octet,
									MAX_LEN_OF_SUPPORTED_RATES - (*pRatesLen));
                    *pRatesLen = MAX_LEN_OF_SUPPORTED_RATES;
                }
                break;
                
            case IE_HT_CAP:
			if (eid_ptr->Len >= sizeof(HT_CAPABILITY_IE))
			{
				NdisMoveMemory(pHtCapability, eid_ptr->Octet, SIZE_HT_CAP_IE);

				*(USHORT *)(&pHtCapability->HtCapInfo) = \
							cpu2le16(*(USHORT *)(&pHtCapability->HtCapInfo));

#ifdef UNALIGNMENT_SUPPORT
				{
					EXT_HT_CAP_INFO extHtCapInfo;

					NdisMoveMemory((PUCHAR)(&extHtCapInfo), (PUCHAR)(&pHtCapability->ExtHtCapInfo), sizeof(EXT_HT_CAP_INFO));
					*(USHORT *)(&extHtCapInfo) = cpu2le16(*(USHORT *)(&extHtCapInfo));
					NdisMoveMemory((PUCHAR)(&pHtCapability->ExtHtCapInfo), (PUCHAR)(&extHtCapInfo), sizeof(EXT_HT_CAP_INFO));		
				}
#else				
				*(USHORT *)(&pHtCapability->ExtHtCapInfo) = \
							cpu2le16(*(USHORT *)(&pHtCapability->ExtHtCapInfo));
#endif /* UNALIGNMENT_SUPPORT */

				*pHtCapabilityLen = SIZE_HT_CAP_IE;
				Sanity |= 0x10;
				DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - IE_HT_CAP\n"));
			}
			else
			{
				DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - wrong IE_HT_CAP.eid_ptr->Len = %d\n", eid_ptr->Len));
			}
				
		break;
		case IE_EXT_CAPABILITY:
			if (eid_ptr->Len >= sizeof(EXT_CAP_INFO_ELEMENT))
			{
				NdisMoveMemory(pExtCapInfo, eid_ptr->Octet, sizeof(EXT_CAP_INFO_ELEMENT));
				DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - IE_EXT_CAPABILITY!\n"));
			}

			break;

            case IE_WPA:    /* same as IE_VENDOR_SPECIFIC */
            case IE_WPA2:

				if (NdisEqualMemory(eid_ptr->Octet, WPS_OUI, 4))
				{
				    break;
				}

				/* Handle Atheros and Broadcom draft 11n STAs */
				if (NdisEqualMemory(eid_ptr->Octet, BROADCOM_OUI, 3))
				{
					switch (eid_ptr->Octet[3])
					{
						case 0x33: 
							if ((eid_ptr->Len-4) == sizeof(HT_CAPABILITY_IE))
							{
								NdisMoveMemory(pHtCapability, &eid_ptr->Octet[4], SIZE_HT_CAP_IE);

								*(USHORT *)(&pHtCapability->HtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->HtCapInfo));
#ifdef UNALIGNMENT_SUPPORT
								{
									EXT_HT_CAP_INFO extHtCapInfo;

									NdisMoveMemory((PUCHAR)(&extHtCapInfo), (PUCHAR)(&pHtCapability->ExtHtCapInfo), sizeof(EXT_HT_CAP_INFO));
									*(USHORT *)(&extHtCapInfo) = cpu2le16(*(USHORT *)(&extHtCapInfo));
									NdisMoveMemory((PUCHAR)(&pHtCapability->ExtHtCapInfo), (PUCHAR)(&extHtCapInfo), sizeof(EXT_HT_CAP_INFO));		
								}
#else				
								*(USHORT *)(&pHtCapability->ExtHtCapInfo) = cpu2le16(*(USHORT *)(&pHtCapability->ExtHtCapInfo));
#endif /* UNALIGNMENT_SUPPORT */

								*pHtCapabilityLen = SIZE_HT_CAP_IE;
							}
							break;
						
						default:
							/* ignore other cases */
							break;
					}
				}

                if (NdisEqualMemory(eid_ptr->Octet, RALINK_OUI, 3) && (eid_ptr->Len == 7))
                {
                    /**pRalinkIe = eid_ptr->Octet[3]; */
					if (eid_ptr->Octet[3] != 0)
                    	*pRalinkIe = eid_ptr->Octet[3];
        			else
        				*pRalinkIe = 0xf0000000; /* Set to non-zero value (can't set bit0-2) to represent this is Ralink Chip. So at linkup, we will set ralinkchip flag. */
                    break;
                }
                
                /* WMM_IE */
                if (NdisEqualMemory(eid_ptr->Octet, WME_INFO_ELEM, 6) && (eid_ptr->Len == 7))
                {
                    *pbWmmCapable = TRUE;

#ifdef UAPSD_SUPPORT
					if (pEntry)
					{
						UAPSD_AssocParse(pAd,
									pEntry, (UINT8 *)&eid_ptr->Octet[6],
									pAd->ApCfg.MBSSID[\
										pEntry->apidx].UapsdInfo.bAPSDCapable);
					}
#endif /* UAPSD_SUPPORT */

                    break;
                }

                if (pAd->ApCfg.MBSSID[pEntry->apidx].AuthMode < Ndis802_11AuthModeWPA)
                    break;
                
                /* 	If this IE did not begins with 00:0x50:0xf2:0x01,  
                	it would be proprietary. So we ignore it. */
                if (!NdisEqualMemory(eid_ptr->Octet, WPA1_OUI, sizeof(WPA1_OUI))
                    && !NdisEqualMemory(&eid_ptr->Octet[2], WPA2_OUI, sizeof(WPA2_OUI)))
                {
                    DBGPRINT(RT_DEBUG_TRACE, ("Not RSN IE, maybe WMM IE!!!\n"));
                    break;                          
                }
                
                if (/*(eid_ptr->Len <= MAX_LEN_OF_RSNIE) &&*/ (eid_ptr->Len >= MIN_LEN_OF_RSNIE))
                {
					hex_dump("Received RSNIE in Assoc-Req", (UCHAR *)eid_ptr, eid_ptr->Len + 2);
                    
					/* Copy whole RSNIE context */
                    NdisMoveMemory(RSN, eid_ptr, eid_ptr->Len + 2);
					*pRSNLen=eid_ptr->Len + 2;

                }
                else
                {
                    *pRSNLen=0;
                    DBGPRINT(RT_DEBUG_TRACE, ("PeerAssocReqSanity - missing IE_WPA(%d)\n",eid_ptr->Len));
                    return FALSE;
                }               
                break;




            default:
                break;
        }

        eid_ptr = (PEID_STRUCT)((UCHAR*)eid_ptr + 2 + eid_ptr->Len);        
    }

	if ((Sanity&0x3) != 0x03)	 
    {
        DBGPRINT(RT_DEBUG_WARN, ("PeerAssocReqSanity - missing mandatory field\n"));
        return FALSE;
    }
    else
    {
        DBGPRINT(RT_DEBUG_TRACE, ("PeerAssocReqSanity - success\n"));
        return TRUE;
    }
}




/* 
    ==========================================================================
    Description:
        MLME message sanity check
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
 */
BOOLEAN PeerDisassocReqSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum,
    OUT USHORT *Reason) 
{
    PFRAME_802_11 Fr = (PFRAME_802_11)Msg;

    COPY_MAC_ADDR(pAddr2, &Fr->Hdr.Addr2);
	*SeqNum = Fr->Hdr.Sequence;
    NdisMoveMemory(Reason, &Fr->Octet[0], 2);

    return TRUE;
}


/* 
    ==========================================================================
    Description:
        MLME message sanity check
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
 */
BOOLEAN PeerDeauthReqSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr2, 
    OUT	UINT16	*SeqNum, 
    OUT USHORT *Reason) 
{
    PFRAME_802_11 Fr = (PFRAME_802_11)Msg;

    COPY_MAC_ADDR(pAddr2, &Fr->Hdr.Addr2);
	*SeqNum = Fr->Hdr.Sequence;
    NdisMoveMemory(Reason, &Fr->Octet[0], 2);

    return TRUE;
}


/* 
    ==========================================================================
    Description:
        MLME message sanity check
    Return:
        TRUE if all parameters are OK, FALSE otherwise
    ==========================================================================
 */
BOOLEAN APPeerAuthSanity(
    IN PRTMP_ADAPTER pAd, 
    IN VOID *Msg, 
    IN ULONG MsgLen, 
    OUT PUCHAR pAddr1, 
    OUT PUCHAR pAddr2, 
    OUT USHORT *Alg, 
    OUT USHORT *Seq, 
    OUT USHORT *Status, 
    CHAR *ChlgText
    ) 
{
    PFRAME_802_11 Fr = (PFRAME_802_11)Msg;

	COPY_MAC_ADDR(pAddr1,  &Fr->Hdr.Addr1);		/* BSSID */
    COPY_MAC_ADDR(pAddr2,  &Fr->Hdr.Addr2);		/* SA */
    NdisMoveMemory(Alg,    &Fr->Octet[0], 2);
    NdisMoveMemory(Seq,    &Fr->Octet[2], 2);
    NdisMoveMemory(Status, &Fr->Octet[4], 2);

    if (*Alg == AUTH_MODE_OPEN) 
    {
        if (*Seq == 1 || *Seq == 2) 
        {
            return TRUE;
        } 
        else 
        {
            DBGPRINT(RT_DEBUG_TRACE, ("APPeerAuthSanity fail - wrong Seg# (=%d)\n", *Seq));
            return FALSE;
        }
    } 
    else if (*Alg == AUTH_MODE_KEY) 
    {
        if (*Seq == 1 || *Seq == 4) 
        {
            return TRUE;
        } 
        else if (*Seq == 2 || *Seq == 3) 
        {
            NdisMoveMemory(ChlgText, &Fr->Octet[8], CIPHER_TEXT_LEN);
            return TRUE;
        } 
        else 
        {
            DBGPRINT(RT_DEBUG_TRACE, ("APPeerAuthSanity fail - wrong Seg# (=%d)\n", *Seq));
            return FALSE;
        }
    } 
    else 
    {
        DBGPRINT(RT_DEBUG_TRACE, ("APPeerAuthSanity fail - wrong algorithm (=%d)\n", *Alg));
        return FALSE;
    }

	return TRUE;
}


