/*
 * Copyright (c) 2012 Qualcomm Atheros, Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <linux/pci_regs.h>
#include <linux/mii.h>

#include "alc_hw.h"



/*
 * get permanent mac address
 *    0: success
 *    non-0:fail
 */
u16 l1c_get_perm_macaddr(struct alx_hw *hw, u8 *addr)
{
	u32 val, otp_ctrl, otp_flag, mac0, mac1;
	u16 i;
	u16 phy_val;

	/* get it from register first */
	alx_mem_r32(hw, L1C_STAD0, &mac0);
	alx_mem_r32(hw, L1C_STAD1, &mac1);

	*(u32 *)(addr + 2) = LX_SWAP_DW(mac0);
	*(u16 *)addr = (u16)LX_SWAP_W((u16)mac1);

	if (macaddr_valid(addr))
		return 0;

	alx_mem_r32(hw, L1C_TWSI_DBG, &val);
	alx_mem_r32(hw, L1C_EFUSE_CTRL2, &otp_ctrl);
	alx_mem_r32(hw, L1C_MASTER, &otp_flag);

	if ((val & L1C_TWSI_DBG_DEV_EXIST) != 0 ||
	    (otp_flag & L1C_MASTER_OTP_FLG) != 0) {
		/* nov-memory exist, do software-autoload */
		/* enable OTP_CLK for L1C */
		if (hw->pci_devid == L1C_DEV_ID ||
		    hw->pci_devid == L2C_DEV_ID) {
			if ((otp_ctrl & L1C_EFUSE_CTRL2_CLK_EN) != 0) {
				alx_mem_w32(hw, L1C_EFUSE_CTRL2,
					    otp_ctrl | L1C_EFUSE_CTRL2_CLK_EN);
				udelay(5);
			}
		}
		/* raise voltage temporally for L2CB/L1D */
		if (hw->pci_devid == L2CB_DEV_ID ||
		    hw->pci_devid == L2CB2_DEV_ID) {
			/* clear bit[7] of debugport 00 */
			l1c_read_phydbg(hw, true, L1C_MIIDBG_ANACTRL,
					&phy_val);
			l1c_write_phydbg(hw, true, L1C_MIIDBG_ANACTRL,
					 phy_val & ~L1C_ANACTRL_HB_EN);
			/* set bit[3] of debugport 3B */
			l1c_read_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					&phy_val);
			l1c_write_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					 phy_val | L1C_VOLT_CTRL_SWLOWEST);
			udelay(20);
		}
		/* do load */
		alx_mem_r32(hw, L1C_SLD, &val);
		alx_mem_w32(hw, L1C_SLD, val | L1C_SLD_START);
		for (i = 0; i < L1C_SLD_MAX_TO; i++) {
			mdelay(1);
			alx_mem_r32(hw, L1C_SLD, &val);
			if ((val & L1C_SLD_START) == 0)
				break;
		}
		/* disable OTP_CLK for L1C */
		if (hw->pci_devid == L1C_DEV_ID ||
		    hw->pci_devid == L2C_DEV_ID) {
			alx_mem_w32(hw, L1C_EFUSE_CTRL2,
				    otp_ctrl & ~L1C_EFUSE_CTRL2_CLK_EN);
			udelay(5);
		}
		/* low voltage */
		if (hw->pci_devid == L2CB_DEV_ID ||
		    hw->pci_devid == L2CB2_DEV_ID) {
			/* set bit[7] of debugport 00 */
			l1c_read_phydbg(hw, true, L1C_MIIDBG_ANACTRL,
					&phy_val);
			l1c_write_phydbg(hw, true, L1C_MIIDBG_ANACTRL,
					 phy_val | L1C_ANACTRL_HB_EN);
			/* clear bit[3] of debugport 3B */
			l1c_read_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					&phy_val);
			l1c_write_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					 phy_val & ~L1C_VOLT_CTRL_SWLOWEST);
			udelay(20);
		}
		if (i == L1C_SLD_MAX_TO)
			goto out;
	} else {
		if (hw->pci_devid == L1C_DEV_ID ||
		    hw->pci_devid == L2C_DEV_ID) {
			alx_mem_w32(hw, L1C_EFUSE_CTRL2,
				    otp_ctrl & ~L1C_EFUSE_CTRL2_CLK_EN);
			udelay(5);
		}
	}

	alx_mem_r32(hw, L1C_STAD0, &mac0);
	alx_mem_r32(hw, L1C_STAD1, &mac1);

	*(u32 *)(addr + 2) = LX_SWAP_DW(mac0);
	*(u16 *)addr = (u16)LX_SWAP_W((u16)mac1);

	if (macaddr_valid(addr))
		return 0;

out:
	return LX_ERR_ALOAD;
}

/*
 * reset mac & dma
 * return
 *     0: success
 *     non-0:fail
 */
u16 l1c_reset_mac(struct alx_hw *hw)
{
	u32 val, mrst_val;
	u16 ret;
	u16 i;

	/* disable all interrupts, RXQ/TXQ */
	alx_mem_w32(hw, L1C_IMR, 0);
	alx_mem_w32(hw, L1C_ISR, L1C_ISR_DIS);

	ret = l1c_enable_mac(hw, false, 0);
	if (ret != 0)
		return ret;
	/* reset whole mac safely. OOB is meaningful for L1D only  */
	alx_mem_r32(hw, L1C_MASTER, &mrst_val);
	mrst_val |= L1C_MASTER_OOB_DIS;
	alx_mem_w32(hw, L1C_MASTER, mrst_val | L1C_MASTER_DMA_MAC_RST);

	/* make sure it's idle */
	for (i = 0; i < L1C_DMA_MAC_RST_TO; i++) {
		alx_mem_r32(hw, L1C_MASTER, &val);
		if ((val & L1C_MASTER_DMA_MAC_RST) == 0)
			break;
		udelay(20);
	}
	if (i == L1C_DMA_MAC_RST_TO)
		return LX_ERR_RSTMAC;
	/* keep the old value */
	alx_mem_w32(hw, L1C_MASTER, mrst_val & ~L1C_MASTER_DMA_MAC_RST);

	/* driver control speed/duplex, hash-alg */
	alx_mem_r32(hw, L1C_MAC_CTRL, &val);
	alx_mem_w32(hw, L1C_MAC_CTRL, val | L1C_MAC_CTRL_WOLSPED_SWEN);

	/* clk switch setting */
	alx_mem_r32(hw, L1C_SERDES, &val);
	switch (hw->pci_devid) {
	case L2CB_DEV_ID:
		alx_mem_w32(hw, L1C_SERDES, val & ~L1C_SERDES_PHYCLK_SLWDWN);
		break;
	case L2CB2_DEV_ID:
	case L1D2_DEV_ID:
		alx_mem_w32(hw, L1C_SERDES,
			    val | L1C_SERDES_PHYCLK_SLWDWN |
			    L1C_SERDES_MACCLK_SLWDWN);
		break;
	default:
		/* the defalut value of default product is OFF */;
	}

	return 0;
}

/* reset phy
 * return
 *    0: success
 *    non-0:fail
 */
u16 l1c_reset_phy(struct alx_hw *hw, bool pws_en, bool az_en, bool ptp_en)
{
	u32 val;
	u16 i, phy_val;

	ptp_en = ptp_en;

	/* reset PHY core */
	alx_mem_r32(hw, L1C_PHY_CTRL, &val);
	val &= ~(L1C_PHY_CTRL_DSPRST_OUT | L1C_PHY_CTRL_IDDQ |
		 L1C_PHY_CTRL_GATE_25M | L1C_PHY_CTRL_POWER_DOWN |
		 L1C_PHY_CTRL_CLS);
	val |= L1C_PHY_CTRL_RST_ANALOG;

	if (pws_en)
		val |= (L1C_PHY_CTRL_HIB_PULSE | L1C_PHY_CTRL_HIB_EN);
	else
		val &= ~(L1C_PHY_CTRL_HIB_PULSE | L1C_PHY_CTRL_HIB_EN);

	alx_mem_w32(hw, L1C_PHY_CTRL, val);
	udelay(10); /* 5us is enough */
	alx_mem_w32(hw, L1C_PHY_CTRL, val | L1C_PHY_CTRL_DSPRST_OUT);

	/* delay 800us */
	for (i = 0; i < L1C_PHY_CTRL_DSPRST_TO; i++)
		udelay(10);

	/* switch clock */
	if (hw->pci_devid == L2CB_DEV_ID) {
		l1c_read_phydbg(hw, true, L1C_MIIDBG_CFGLPSPD, &phy_val);
		/* clear bit13 */
		l1c_write_phydbg(hw, true, L1C_MIIDBG_CFGLPSPD,
				 phy_val & ~L1C_CFGLPSPD_RSTCNT_CLK125SW);
	}

	/* fix tx-half-amp issue */
	if (hw->pci_devid == L2CB_DEV_ID || hw->pci_devid == L2CB2_DEV_ID) {
		l1c_read_phydbg(hw, true, L1C_MIIDBG_CABLE1TH_DET, &phy_val);
		l1c_write_phydbg(hw, true, L1C_MIIDBG_CABLE1TH_DET,
				 phy_val | L1C_CABLE1TH_DET_EN); /* set bit15 */
	}

	if (pws_en) {
		/* clear bit[3] of debugport 3B to 0,
		 * lower voltage to save power */
		if (hw->pci_devid == L2CB_DEV_ID ||
		    hw->pci_devid == L2CB2_DEV_ID) {
			l1c_read_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					&phy_val);
			l1c_write_phydbg(hw, true, L1C_MIIDBG_VOLT_CTRL,
					 phy_val & ~L1C_VOLT_CTRL_SWLOWEST);
		}
		/* power saving config */
		l1c_write_phydbg(hw, true, L1C_MIIDBG_LEGCYPS,
				 (hw->pci_devid == L1D_DEV_ID ||
				  hw->pci_devid == L1D2_DEV_ID) ?
				 L1D_LEGCYPS_DEF : L1C_LEGCYPS_DEF);
		/* hib */
		l1c_write_phydbg(hw, true, L1C_MIIDBG_SYSMODCTRL,
				 L1C_SYSMODCTRL_IECHOADJ_DEF);
	} else {
		/*dis powersaving */
		l1c_read_phydbg(hw, true, L1C_MIIDBG_LEGCYPS, &phy_val);
		l1c_write_phydbg(hw, true, L1C_MIIDBG_LEGCYPS,
				 phy_val & ~L1C_LEGCYPS_EN);
		/* disable hibernate */
		l1c_read_phydbg(hw, true, L1C_MIIDBG_HIBNEG, &phy_val);
		l1c_write_phydbg(hw, true, L1C_MIIDBG_HIBNEG,
				 phy_val & ~L1C_HIBNEG_PSHIB_EN);
	}

	/* az is only for l2cbv2 / l1dv1 /l1dv2 */
	if (hw->pci_devid == L1D_DEV_ID ||
	    hw->pci_devid == L1D2_DEV_ID ||
	    hw->pci_devid == L2CB2_DEV_ID) {
		if (az_en) {
			switch (hw->pci_devid) {
			case L2CB2_DEV_ID:
				alx_mem_w32(hw, L1C_LPI_DECISN_TIMER,
					    L1C_LPI_DESISN_TIMER_L2CB);
				/* az enable 100M */
				l1c_write_phy(hw, true, L1C_MIIEXT_ANEG, true,
					      L1C_MIIEXT_LOCAL_EEEADV,
					      L1C_LOCAL_EEEADV_100BT);
				/* az long wake threshold */
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL5,
					      L1C_AZCTRL5_WAKE_LTH_L2CB);
				/* az short wake threshold */
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL4,
					      L1C_AZCTRL4_WAKE_STH_L2CB);

				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_CLDCTRL3,
					      L1C_CLDCTRL3_L2CB);

				/* bit7 set to 0, otherwise ping fail */
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_CLDCTRL7,
					      L1C_CLDCTRL7_L2CB);

				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL2,
					      L1C_AZCTRL2_L2CB);
				break;

			case L1D_DEV_ID:
				l1c_write_phydbg(hw, true,
				    L1C_MIIDBG_AZ_ANADECT, L1C_AZ_ANADECT_DEF);
				phy_val = hw->long_cable ? L1C_CLDCTRL3_L1D :
					  (L1C_CLDCTRL3_L1D &
					   ~(L1C_CLDCTRL3_BP_CABLE1TH_DET_GT |
					     L1C_CLDCTRL3_AZ_DISAMP));
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_CLDCTRL3, phy_val);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL,
					      L1C_AZCTRL_L1D);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL2,
					      L1C_AZCTRL2_L2CB);
				break;

			case L1D2_DEV_ID:
				l1c_write_phydbg(hw, true,
						 L1C_MIIDBG_AZ_ANADECT,
						 L1C_AZ_ANADECT_DEF);
				phy_val = hw->long_cable ? L1C_CLDCTRL3_L1D :
					  (L1C_CLDCTRL3_L1D &
					   ~L1C_CLDCTRL3_BP_CABLE1TH_DET_GT);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_CLDCTRL3, phy_val);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL,
					      L1C_AZCTRL_L1D);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL2,
					      L1C_AZCTRL2_L1D2);
				l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
					      L1C_MIIEXT_AZCTRL6,
					      L1C_AZCTRL6_L1D2);
				break;
			}
		} else {
			alx_mem_r32(hw, L1C_LPI_CTRL, &val);
			alx_mem_w32(hw, L1C_LPI_CTRL, val & ~L1C_LPI_CTRL_EN);
			l1c_write_phy(hw, true, L1C_MIIEXT_ANEG, true,
				      L1C_MIIEXT_LOCAL_EEEADV, 0);
			l1c_write_phy(hw, true, L1C_MIIEXT_PCS, true,
				      L1C_MIIEXT_CLDCTRL3, L1C_CLDCTRL3_L2CB);
		}
	}

	/* other debug port need to set */
	l1c_write_phydbg(hw, true, L1C_MIIDBG_ANACTRL, L1C_ANACTRL_DEF);
	l1c_write_phydbg(hw, true, L1C_MIIDBG_SRDSYSMOD, L1C_SRDSYSMOD_DEF);
	l1c_write_phydbg(hw, true, L1C_MIIDBG_TST10BTCFG, L1C_TST10BTCFG_DEF);
	/* L1c, L2c, L1d, L2cb  link fail inhibit
	   timer issue of L1c UNH-IOL test fail, set bit7*/
	l1c_write_phydbg(hw, true, L1C_MIIDBG_TST100BTCFG,
			 L1C_TST100BTCFG_DEF | L1C_TST100BTCFG_LITCH_EN);

	/* set phy interrupt mask */
	l1c_write_phy(hw, false, 0, true,
		      L1C_MII_IER, L1C_IER_LINK_UP | L1C_IER_LINK_DOWN);

	return 0;
}


/* reset pcie
 * just reset pcie relative registers (pci command, clk, aspm...)
 * return
 *    0:success
 *    non-0:fail
 */
u16 l1c_reset_pcie(struct alx_hw *hw, bool l0s_en, bool l1_en)
{
	u32 val;
	u16 val16;
	u16 ret;

	/* Workaround for PCI problem when BIOS sets MMRBC incorrectly. */
	alx_cfg_r16(hw, PCI_COMMAND, &val16);
	if ((val16 & (PCI_COMMAND_IO |
		      PCI_COMMAND_MEMORY |
		      PCI_COMMAND_MASTER)) == 0 ||
	    (val16 & PCI_COMMAND_INTX_DISABLE) != 0) {
		val16 = (u16)((val16 | (PCI_COMMAND_IO |
					PCI_COMMAND_MEMORY |
					PCI_COMMAND_MASTER))
			      & ~PCI_COMMAND_INTX_DISABLE);
		alx_cfg_w16(hw, PCI_COMMAND, val16);
	}

	/* Clear any PowerSaving Settings */
	alx_cfg_w16(hw, L1C_PM_CSR, 0);

	/* close write attr for some registes */
	alx_mem_r32(hw, L1C_LTSSM_CTRL, &val);
	alx_mem_w32(hw, L1C_LTSSM_CTRL, val & ~L1C_LTSSM_WRO_EN);

	/* mask some pcie error bits */
	alx_mem_r32(hw, L1C_UE_SVRT, &val);
	val &= ~(L1C_UE_SVRT_DLPROTERR | L1C_UE_SVRT_FCPROTERR);
	alx_mem_w32(hw, L1C_UE_SVRT, val);

	/* pclk */
	alx_mem_r32(hw, L1C_MASTER, &val);
	val &= ~L1C_MASTER_PCLKSEL_SRDS;
	alx_mem_w32(hw, L1C_MASTER, val);

	/* Set 1000 bit 2, only used for L1c/L2c , WOL usage */
	if (hw->pci_devid == L1C_DEV_ID || hw->pci_devid == L2C_DEV_ID) {
		alx_mem_r32(hw, L1C_PPHY_MISC1, &val);
		alx_mem_w32(hw, L1C_PPHY_MISC1, val | L1C_PPHY_MISC1_RCVDET);
	} else { /* other device should set bit 5 of reg1400 for WOL */
		if ((val & L1C_MASTER_WAKEN_25M) == 0)
			alx_mem_w32(hw, L1C_MASTER, val | L1C_MASTER_WAKEN_25M);
	}
	/* l2cb 1.0*/
	if (hw->pci_devid == L2CB_DEV_ID && hw->pci_revid == L2CB_V10) {
		alx_mem_r32(hw, L1C_PPHY_MISC2, &val);
		FIELD_SETL(val, L1C_PPHY_MISC2_L0S_TH,
			   L1C_PPHY_MISC2_L0S_TH_L2CB1);
		FIELD_SETL(val, L1C_PPHY_MISC2_CDR_BW,
			   L1C_PPHY_MISC2_CDR_BW_L2CB1);
		alx_mem_w32(hw, L1C_PPHY_MISC2, val);
		/* extend L1 sync timer, this will use more power,
		 * only for L2cb v1.0*/
		if (!hw->aps_en) {
			alx_mem_r32(hw, L1C_LNK_CTRL, &val);
			alx_mem_w32(hw, L1C_LNK_CTRL,
				    val | L1C_LNK_CTRL_EXTSYNC);
		}
	}

	/* l2cbv1.x & l1dv1.x */
	if (hw->pci_devid == L2CB_DEV_ID || hw->pci_devid == L1D_DEV_ID) {
		alx_mem_r32(hw, L1C_PMCTRL, &val);
		alx_mem_w32(hw, L1C_PMCTRL, val | L1C_PMCTRL_L0S_BUFSRX_EN);
		/* clear vendor message for L1d & L2cb */
		alx_mem_r32(hw, L1C_DMA_DBG, &val);
		alx_mem_w32(hw, L1C_DMA_DBG, val & ~L1C_DMA_DBG_VENDOR_MSG);
	}

	/* hi-tx-perf */
	if (hw->hi_txperf) {
		alx_mem_r32(hw, L1C_PPHY_MISC1, &val);
		FIELD_SETL(val, L1C_PPHY_MISC1_NFTS,
			   L1C_PPHY_MISC1_NFTS_HIPERF);
		alx_mem_w32(hw, L1C_PPHY_MISC1, val);
	}
	/* l0s, l1 setting */
	ret = l1c_enable_aspm(hw, l0s_en, l1_en, 0);

	udelay(10);

	return ret;
}


/* disable/enable MAC/RXQ/TXQ
 * en
 *    true:enable
 *    false:disable
 * return
 *    0:success
 *    non-0-fail
 */
u16 l1c_enable_mac(struct alx_hw *hw, bool en, u16 en_ctrl)
{
	u32 rxq, txq, mac, val;
	u16 i;

	alx_mem_r32(hw, L1C_RXQ0, &rxq);
	alx_mem_r32(hw, L1C_TXQ0, &txq);
	alx_mem_r32(hw, L1C_MAC_CTRL, &mac);

	if (en) { /* enable */
		alx_mem_w32(hw, L1C_RXQ0, rxq | L1C_RXQ0_EN);
		alx_mem_w32(hw, L1C_TXQ0, txq | L1C_TXQ0_EN);
		if ((en_ctrl & LX_MACSPEED_1000) != 0) {
			FIELD_SETL(mac, L1C_MAC_CTRL_SPEED,
				   L1C_MAC_CTRL_SPEED_1000);
		} else {
			FIELD_SETL(mac, L1C_MAC_CTRL_SPEED,
				   L1C_MAC_CTRL_SPEED_10_100);
		}

		test_set_or_clear(mac, en_ctrl, LX_MACDUPLEX_FULL,
				  L1C_MAC_CTRL_FULLD);

		/* rx filter */
		test_set_or_clear(mac, en_ctrl, LX_FLT_PROMISC,
				  L1C_MAC_CTRL_PROMISC_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_MULTI_ALL,
				  L1C_MAC_CTRL_MULTIALL_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_BROADCAST,
				  L1C_MAC_CTRL_BRD_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_DIRECT,
				  L1C_MAC_CTRL_RX_EN);
		test_set_or_clear(mac, en_ctrl, LX_FC_TXEN,
				  L1C_MAC_CTRL_TXFC_EN);
		test_set_or_clear(mac, en_ctrl, LX_FC_RXEN,
				  L1C_MAC_CTRL_RXFC_EN);
		test_set_or_clear(mac, en_ctrl, LX_VLAN_STRIP,
				  L1C_MAC_CTRL_VLANSTRIP);
		test_set_or_clear(mac, en_ctrl, LX_LOOPBACK,
				  L1C_MAC_CTRL_LPBACK_EN);
		test_set_or_clear(mac, en_ctrl, LX_SINGLE_PAUSE,
				  L1C_MAC_CTRL_SPAUSE_EN);
		test_set_or_clear(mac, en_ctrl, LX_ADD_FCS,
				  (L1C_MAC_CTRL_PCRCE | L1C_MAC_CTRL_CRCE));

		alx_mem_w32(hw, L1C_MAC_CTRL, mac | L1C_MAC_CTRL_TX_EN);
	} else { /* disable mac */
		alx_mem_w32(hw, L1C_RXQ0, rxq & ~L1C_RXQ0_EN);
		alx_mem_w32(hw, L1C_TXQ0, txq & ~L1C_TXQ0_EN);

		/* waiting for rxq/txq be idle */
		for (i = 0; i < L1C_DMA_MAC_RST_TO; i++) {/* wait atmost 1ms */
			alx_mem_r32(hw, L1C_MAC_STS, &val);
			if ((val & (L1C_MAC_STS_TXQ_BUSY |
				    L1C_MAC_STS_RXQ_BUSY)) == 0) {
				break;
			}
			udelay(20);
		}
		if (L1C_DMA_MAC_RST_TO == i)
			return LX_ERR_RSTMAC;
		/* stop mac tx/rx */
		alx_mem_w32(hw, L1C_MAC_CTRL,
			    mac & ~(L1C_MAC_CTRL_RX_EN | L1C_MAC_CTRL_TX_EN));

		for (i = 0; i < L1C_DMA_MAC_RST_TO; i++) {
			alx_mem_r32(hw, L1C_MAC_STS, &val);
			if ((val & L1C_MAC_STS_IDLE) == 0)
				break;
			udelay(10);
		}
		if (L1C_DMA_MAC_RST_TO == i)
			return LX_ERR_RSTMAC;
	}

	return 0;
}


/* enable/disable aspm support
 * that will change settings for phy/mac/pcie
 */
u16 l1c_enable_aspm(struct alx_hw *hw, bool l0s_en, bool l1_en, u8 lnk_stat)
{
	u32 pmctrl;
	bool linkon;

	linkon = (lnk_stat == LX_LC_10H || lnk_stat == LX_LC_10F ||
		  lnk_stat == LX_LC_100H || lnk_stat == LX_LC_100F ||
		  lnk_stat == LX_LC_1000F) ? true : false;

	alx_mem_r32(hw, L1C_PMCTRL, &pmctrl);
	pmctrl &= ~(L1C_PMCTRL_L0S_EN |
		    L1C_PMCTRL_L1_EN |
		    L1C_PMCTRL_ASPM_FCEN);
	FIELD_SETL(pmctrl, L1C_PMCTRL_LCKDET_TIMER,
		   L1C_PMCTRL_LCKDET_TIMER_DEF);

	/* l1 timer */
	if (hw->pci_devid == L2CB2_DEV_ID || hw->pci_devid == L1D2_DEV_ID) {
		pmctrl &= ~L1D_PMCTRL_TXL1_AFTER_L0S;
		FIELD_SETL(pmctrl, L1D_PMCTRL_L1_TIMER,
			   (lnk_stat == LX_LC_100H ||
			    lnk_stat == LX_LC_100F ||
			    lnk_stat == LX_LC_1000F) ?
			   L1D_PMCTRL_L1_TIMER_16US : 1);
	} else {
		FIELD_SETL(pmctrl, L1C_PMCTRL_L1_TIMER,
			   (lnk_stat == LX_LC_100H ||
			    lnk_stat == LX_LC_100F ||
			    lnk_stat == LX_LC_1000F) ?
			   ((hw->pci_devid == L2CB_DEV_ID) ?
			    L1C_PMCTRL_L1_TIMER_L2CB1 : L1C_PMCTRL_L1_TIMER_DEF
			   ) : 1);
	}
	if (l0s_en) { /* on/off l0s only if bios/system enable l0s */
		pmctrl |= (L1C_PMCTRL_L0S_EN | L1C_PMCTRL_ASPM_FCEN);
	}
	if (l1_en) { /* on/off l1 only if bios/system enable l1 */
		pmctrl |= (L1C_PMCTRL_L1_EN | L1C_PMCTRL_ASPM_FCEN);
	}

	if (hw->pci_devid == L2CB_DEV_ID || hw->pci_devid == L1D_DEV_ID ||
	    hw->pci_devid == L2CB2_DEV_ID || hw->pci_devid == L1D2_DEV_ID) {
		/* If the pm_request_l1 time exceeds the value of this timer,
		   it will enter L0s instead of L1 for this ASPM request.*/
		FIELD_SETL(pmctrl, L1C_PMCTRL_L1REQ_TO,
			   L1C_PMCTRL_L1REG_TO_DEF);

		pmctrl |= L1C_PMCTRL_RCVR_WT_1US    |   /* wait 1us not 2ms */
			  L1C_PMCTRL_L1_SRDSRX_PWD  |   /* pwd serdes */
			  L1C_PMCTRL_L1_CLKSW_EN;
		pmctrl &= ~(L1C_PMCTRL_L1_SRDS_EN   |
			    L1C_PMCTRL_L1_SRDSPLL_EN|
			    L1C_PMCTRL_L1_BUFSRX_EN |
			    L1C_PMCTRL_SADLY_EN     |
			    L1C_PMCTRL_HOTRST_WTEN);
		/* disable l0s if linkdown or l2cbv1.x */
		if (!linkon ||
		    (!hw->aps_en && hw->pci_devid == L2CB_DEV_ID)) {
			pmctrl &= ~L1C_PMCTRL_L0S_EN;
		}
	} else { /* l1c */
		FIELD_SETL(pmctrl, L1C_PMCTRL_L1_TIMER, 0);
		if (linkon) {
			pmctrl |= L1C_PMCTRL_L1_SRDS_EN     |
				  L1C_PMCTRL_L1_SRDSPLL_EN  |
				  L1C_PMCTRL_L1_BUFSRX_EN;
			pmctrl &= ~(L1C_PMCTRL_L1_SRDSRX_PWD|
				    L1C_PMCTRL_L1_CLKSW_EN  |
				    L1C_PMCTRL_L0S_EN       |
				    L1C_PMCTRL_L1_EN);
		} else {
			pmctrl |= L1C_PMCTRL_L1_CLKSW_EN;
			pmctrl &= ~(L1C_PMCTRL_L1_SRDS_EN   |
				    L1C_PMCTRL_L1_SRDSPLL_EN|
				    L1C_PMCTRL_L1_BUFSRX_EN |
				    L1C_PMCTRL_L0S_EN);
		}
	}

	alx_mem_w32(hw, L1C_PMCTRL, pmctrl);

	return 0;
}


/* initialize phy for speed / flow control
 * lnk_cap
 *    if autoNeg, is link capability to tell the peer
 *    if force mode, is forced speed/duplex
 */
u16 l1c_init_phy_spdfc(struct alx_hw *hw, bool auto_neg,
		       u8 lnk_cap, bool fc_en)
{
	u16 adv, giga, cr;
	u32 val;
	u16 ret;

	/* clear flag */
	l1c_write_phy(hw, false, 0, false, L1C_MII_DBG_ADDR, 0);
	alx_mem_r32(hw, L1C_DRV, &val);
	FIELD_SETL(val, LX_DRV_PHY, 0);

	if (auto_neg) {
		adv = L1C_ADVERTISE_DEFAULT_CAP & ~L1C_ADVERTISE_SPEED_MASK;
		giga = L1C_GIGA_CR_1000T_DEFAULT_CAP &
		       ~L1C_GIGA_CR_1000T_SPEED_MASK;
		val |= LX_DRV_PHY_AUTO;
		if (!fc_en)
			adv &= ~(ADVERTISE_PAUSE_CAP | ADVERTISE_PAUSE_ASYM);
		else
			val |= LX_DRV_PHY_FC;
		if ((LX_LC_10H & lnk_cap) != 0) {
			adv |= ADVERTISE_10HALF;
			val |= LX_DRV_PHY_10;
		}
		if ((LX_LC_10F & lnk_cap) != 0) {
			adv |= ADVERTISE_10HALF |
			       ADVERTISE_10FULL;
			val |= LX_DRV_PHY_10 | LX_DRV_PHY_DUPLEX;
		}
		if ((LX_LC_100H & lnk_cap) != 0) {
			adv |= ADVERTISE_100HALF;
			val |= LX_DRV_PHY_100;
		}
		if ((LX_LC_100F & lnk_cap) != 0) {
			adv |= ADVERTISE_100HALF |
			       ADVERTISE_100FULL;
			val |= LX_DRV_PHY_100 | LX_DRV_PHY_DUPLEX;
		}
		if ((LX_LC_1000F & lnk_cap) != 0) {
			giga |= L1C_GIGA_CR_1000T_FD_CAPS;
			val |= LX_DRV_PHY_1000 | LX_DRV_PHY_DUPLEX;
		}

		ret = l1c_write_phy(hw, false, 0, false, MII_ADVERTISE, adv);
		ret = l1c_write_phy(hw, false, 0, false, MII_CTRL1000, giga);

		cr = BMCR_RESET | BMCR_ANENABLE | BMCR_ANRESTART;
		ret = l1c_write_phy(hw, false, 0, false, MII_BMCR, cr);
	} else { /* force mode */
		cr = BMCR_RESET;
		switch (lnk_cap) {
		case LX_LC_10H:
			val |= LX_DRV_PHY_10;
			break;
		case LX_LC_10F:
			cr |= BMCR_FULLDPLX;
			val |= LX_DRV_PHY_10 | LX_DRV_PHY_DUPLEX;
			break;
		case LX_LC_100H:
			cr |= BMCR_SPEED100;
			val |= LX_DRV_PHY_100;
			break;
		case LX_LC_100F:
			cr |= BMCR_SPEED100 | BMCR_FULLDPLX;
			val |= LX_DRV_PHY_100 | LX_DRV_PHY_DUPLEX;
			break;
		default:
			return LX_ERR_PARM;
		}
		ret = l1c_write_phy(hw, false, 0, false, MII_BMCR, cr);
	}

	if (!ret) {
		l1c_write_phy(hw, false, 0, false, L1C_MII_DBG_ADDR,
			      LX_PHY_INITED);
	}
	alx_mem_w32(hw, L1C_DRV, val);

	return ret;
}


/* do power saving setting befor enter suspend mode
 * NOTE:
 *    1. phy link must be established before calling this function
 *    2. wol option (pattern,magic,link,etc.) is configed before call it.
 */
u16 l1c_powersaving(struct alx_hw *hw, u8 wire_spd, bool wol_en,
		    bool mac_txen, bool mac_rxen, bool pws_en)
{
	u32 master_ctrl, mac_ctrl, phy_ctrl;
	u16 pm_ctrl, ret = 0;

	master_ctrl = 0;
	mac_ctrl = 0;
	phy_ctrl = 0;

	pws_en = pws_en;

	alx_mem_r32(hw, L1C_MASTER, &master_ctrl);
	master_ctrl &= ~L1C_MASTER_PCLKSEL_SRDS;

	alx_mem_r32(hw, L1C_MAC_CTRL, &mac_ctrl);
	/* 10/100 half */
	FIELD_SETL(mac_ctrl, L1C_MAC_CTRL_SPEED,  L1C_MAC_CTRL_SPEED_10_100);
	mac_ctrl &= ~(L1C_MAC_CTRL_FULLD |
		      L1C_MAC_CTRL_RX_EN |
		      L1C_MAC_CTRL_TX_EN);

	alx_mem_r32(hw, L1C_PHY_CTRL, &phy_ctrl);
	phy_ctrl &= ~(L1C_PHY_CTRL_DSPRST_OUT | L1C_PHY_CTRL_CLS);
	/* if (pws_en) */
	phy_ctrl |= (L1C_PHY_CTRL_RST_ANALOG | L1C_PHY_CTRL_HIB_PULSE |
		     L1C_PHY_CTRL_HIB_EN);

	if (wol_en) { /* enable rx packet or tx packet */
		if (mac_rxen)
			mac_ctrl |= (L1C_MAC_CTRL_RX_EN | L1C_MAC_CTRL_BRD_EN);
		if (mac_txen)
			mac_ctrl |= L1C_MAC_CTRL_TX_EN;
		if (LX_LC_1000F == wire_spd) {
			FIELD_SETL(mac_ctrl, L1C_MAC_CTRL_SPEED,
				   L1C_MAC_CTRL_SPEED_1000);
		}
		if (LX_LC_10F == wire_spd || LX_LC_100F == wire_spd ||
		    LX_LC_100F == wire_spd) {
			mac_ctrl |= L1C_MAC_CTRL_FULLD;
		}
		phy_ctrl |= L1C_PHY_CTRL_DSPRST_OUT;
		ret = l1c_write_phy(hw, false, 0, false,
				    L1C_MII_IER, L1C_IER_LINK_UP);
	} else {
		master_ctrl |= L1C_MASTER_PCLKSEL_SRDS;
		ret = l1c_write_phy(hw, false, 0, false, L1C_MII_IER, 0);
		phy_ctrl |= (L1C_PHY_CTRL_IDDQ | L1C_PHY_CTRL_POWER_DOWN);
	}
	alx_mem_w32(hw, L1C_MASTER, master_ctrl);
	alx_mem_w32(hw, L1C_MAC_CTRL, mac_ctrl);
	alx_mem_w32(hw, L1C_PHY_CTRL, phy_ctrl);

	/* set PME_EN ?? */
	if (wol_en) {
		alx_cfg_r16(hw, L1C_PM_CSR, &pm_ctrl);
		pm_ctrl |= L1C_PM_CSR_PME_EN;
		alx_cfg_w16(hw, L1C_PM_CSR, pm_ctrl);
	}

	return ret;
}


/* read phy register */
u16 l1c_read_phy(struct alx_hw *hw, bool ext, u8 dev, bool fast,
		 u16 reg, u16 *data)
{
	u32 val;
	u16 clk_sel, i, ret = 0;

	*data = 0;
	clk_sel = fast ?
		  (u16)L1C_MDIO_CLK_SEL_25MD4 : (u16)L1C_MDIO_CLK_SEL_25MD128;

	if (ext) {
		val = FIELDL(L1C_MDIO_EXTN_DEVAD, dev) |
		      FIELDL(L1C_MDIO_EXTN_REG, reg);
		alx_mem_w32(hw, L1C_MDIO_EXTN, val);

		val = L1C_MDIO_SPRES_PRMBL |
		      FIELDL(L1C_MDIO_CLK_SEL, clk_sel) |
		      L1C_MDIO_START |
		      L1C_MDIO_MODE_EXT |
		      L1C_MDIO_OP_READ;
	} else {
		val = L1C_MDIO_SPRES_PRMBL |
		      FIELDL(L1C_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1C_MDIO_REG, reg) |
		      L1C_MDIO_START |
		      L1C_MDIO_OP_READ;
	}

	alx_mem_w32(hw, L1C_MDIO, val);

	for (i = 0; i < L1C_MDIO_MAX_AC_TO; i++) {
		alx_mem_r32(hw, L1C_MDIO, &val);
		if ((val & L1C_MDIO_BUSY) == 0) {
			*data = (u16)FIELD_GETX(val, L1C_MDIO_DATA);
			break;
		}
		udelay(10);
	}
	if (L1C_MDIO_MAX_AC_TO == i)
		ret = LX_ERR_MIIBUSY;

	return ret;
}

/* write phy register */
u16 l1c_write_phy(struct alx_hw *hw, bool ext, u8 dev, bool fast,
		  u16 reg, u16 data)
{
	u32 val;
	u16 clk_sel, i, ret = 0;

	clk_sel = fast ?
	    (u16)L1C_MDIO_CLK_SEL_25MD4 : (u16)L1C_MDIO_CLK_SEL_25MD128;

	if (ext) {
		val = FIELDL(L1C_MDIO_EXTN_DEVAD, dev) |
		      FIELDL(L1C_MDIO_EXTN_REG, reg);
		alx_mem_w32(hw, L1C_MDIO_EXTN, val);

		val = L1C_MDIO_SPRES_PRMBL |
		      FIELDL(L1C_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1C_MDIO_DATA, data) |
		      L1C_MDIO_START |
		      L1C_MDIO_MODE_EXT;
	} else {
		val = L1C_MDIO_SPRES_PRMBL |
		      FIELDL(L1C_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1C_MDIO_REG, reg) |
		      FIELDL(L1C_MDIO_DATA, data) |
		      L1C_MDIO_START;
	}

	alx_mem_w32(hw, L1C_MDIO, val);

	for (i = 0; i < L1C_MDIO_MAX_AC_TO; i++) {
		alx_mem_r32(hw, L1C_MDIO, &val);
		if ((val & L1C_MDIO_BUSY) == 0)
			break;
		udelay(10);
	}

	if (L1C_MDIO_MAX_AC_TO == i)
		ret = LX_ERR_MIIBUSY;

	return ret;
}

u16 l1c_read_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 *data)
{
	u16 ret;

	ret = l1c_write_phy(hw, false, 0, fast, L1C_MII_DBG_ADDR, reg);
	ret = l1c_read_phy(hw, false, 0, fast, L1C_MII_DBG_DATA, data);

	return ret;
}

u16 l1c_write_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 data)
{
	u16 ret;

	ret = l1c_write_phy(hw, false, 0, fast, L1C_MII_DBG_ADDR, reg);
	ret = l1c_write_phy(hw, false, 0, fast, L1C_MII_DBG_DATA, data);

	return ret;
}


/*
 * initialize mac basically
 *  most of hi-feature no init
 *      MAC/PHY should be reset before call this function
 *  smb_timer : million-second
 *  int_mod   : micro-second
 *  disable RSS as default
 */
u16 l1c_init_mac(struct alx_hw *hw, u8 *addr, u32 txmem_hi,
		 u32 *tx_mem_lo, u8 tx_qnum, u16 txring_sz,
		 u32 rxmem_hi, u32 rfdmem_lo, u32 rrdmem_lo,
		 u16 rxring_sz, u16 rxbuf_sz, u16 smb_timer,
		 u16 mtu, u16 int_mod, bool hash_legacy)
{
	u32 val;
	u16 val16;
	u8 dmar_len;

	/* set mac-address */
	val = *(u32 *)(addr + 2);
	alx_mem_w32(hw, L1C_STAD0, LX_SWAP_DW(val));
	val = *(u16 *)addr ;
	alx_mem_w32(hw, L1C_STAD1, LX_SWAP_W((u16)val));

	/* clear multicast hash table, algrithm */
	alx_mem_w32(hw, L1C_HASH_TBL0, 0);
	alx_mem_w32(hw, L1C_HASH_TBL1, 0);
	alx_mem_r32(hw, L1C_MAC_CTRL, &val);
	if (hash_legacy)
		val |= L1C_MAC_CTRL_MHASH_ALG_HI5B;
	else
		val &= ~L1C_MAC_CTRL_MHASH_ALG_HI5B;
	alx_mem_w32(hw, L1C_MAC_CTRL, val);

	/* clear any wol setting/status */
	alx_mem_r32(hw, L1C_WOL0, &val);
	alx_mem_w32(hw, L1C_WOL0, 0);

	/* clk gating */
	alx_mem_w32(hw, L1C_CLK_GATE, (hw->pci_devid == L1D_DEV_ID) ? 0 :
		       (L1C_CLK_GATE_DMAR | L1C_CLK_GATE_DMAW |
			L1C_CLK_GATE_TXQ  | L1C_CLK_GATE_RXQ  |
			L1C_CLK_GATE_TXMAC));

	/* descriptor ring base memory */
	alx_mem_w32(hw, L1C_TX_BASE_ADDR_HI, txmem_hi);
	alx_mem_w32(hw, L1C_TPD_RING_SZ, txring_sz);
	switch (tx_qnum) {
	case 2:
		alx_mem_w32(hw, L1C_TPD_PRI1_ADDR_LO, tx_mem_lo[1]);
		/* fall through */
	case 1:
		alx_mem_w32(hw, L1C_TPD_PRI0_ADDR_LO, tx_mem_lo[0]);
		break;
	default:
		return LX_ERR_PARM;
	}
	alx_mem_w32(hw, L1C_RX_BASE_ADDR_HI, rxmem_hi);
	alx_mem_w32(hw, L1C_RFD_ADDR_LO, rfdmem_lo);
	alx_mem_w32(hw, L1C_RRD_ADDR_LO, rrdmem_lo);
	alx_mem_w32(hw, L1C_RFD_BUF_SZ, rxbuf_sz);
	alx_mem_w32(hw, L1C_RRD_RING_SZ, rxring_sz);
	alx_mem_w32(hw, L1C_RFD_RING_SZ, rxring_sz);
	alx_mem_w32(hw, L1C_SMB_TIMER, smb_timer * 500UL);

	if (hw->pci_devid == L2CB_DEV_ID) {
		/* revise SRAM configuration */
		alx_mem_w32(hw, L1C_SRAM5, L1C_SRAM_RXF_LEN_L2CB1);
		alx_mem_w32(hw, L1C_SRAM7, L1C_SRAM_TXF_LEN_L2CB1);
		alx_mem_w32(hw, L1C_SRAM4, L1C_SRAM_RXF_HT_L2CB1);
		alx_mem_w32(hw, L1C_SRAM0, L1C_SRAM_RFD_HT_L2CB1);
		alx_mem_w32(hw, L1C_SRAM6, L1C_SRAM_TXF_HT_L2CB1);
		alx_mem_w32(hw, L1C_SRAM2, L1C_SRAM_TRD_HT_L2CB1);
		alx_mem_w32(hw, L1C_TXQ2, 0); /* TX watermark, goto L1 state.*/
		alx_mem_w32(hw, L1C_RXQ3, 0); /* RXD threshold. */
	}
	alx_mem_w32(hw, L1C_SRAM9, L1C_SRAM_LOAD_PTR);

	/* int moduration */
	alx_mem_r32(hw, L1C_MASTER, &val);
	val |= L1C_MASTER_IRQMOD2_EN | L1C_MASTER_IRQMOD1_EN |
	    L1C_MASTER_SYSALVTIMER_EN;  /* sysalive */
	alx_mem_w32(hw, L1C_MASTER, val);
	/* set Interrupt Moderator Timer (max interrupt per sec)
	 * we use seperate time for rx/tx */
	alx_mem_w32(hw, L1C_IRQ_MODU_TIMER,
		    FIELDL(L1C_IRQ_MODU_TIMER1, int_mod) |
		    FIELDL(L1C_IRQ_MODU_TIMER2, int_mod >> 1));

	/* tpd threshold to trig int */
	alx_mem_w32(hw, L1C_TINT_TPD_THRSHLD, (u32)txring_sz / 3);
	alx_mem_w32(hw, L1C_TINT_TIMER, int_mod * 2);
	/* re-send int */
	alx_mem_w32(hw, L1C_INT_RETRIG, L1C_INT_RETRIG_TO);

	/* mtu */
	alx_mem_w32(hw, L1C_MTU, (u32)(mtu + 4 + 4)); /* crc + vlan */

	/* txq */
	if ((mtu + 8) < L1C_TXQ1_JUMBO_TSO_TH)
		val = (u32)(mtu + 8 + 7); /* 7 for QWORD align */
	else
		val = L1C_TXQ1_JUMBO_TSO_TH;
	alx_mem_w32(hw, L1C_TXQ1, val >> 3);

	alx_mem_r32(hw, L1C_DEV_CTRL, &val);
	dmar_len = (u8)FIELD_GETX(val, L1C_DEV_CTRL_MAXRRS);
	/* if BIOS had changed the default dma read max length,
	 * restore it to default value */
	if (dmar_len < L1C_DEV_CTRL_MAXRRS_MIN) {
		FIELD_SETL(val, L1C_DEV_CTRL_MAXRRS, L1C_DEV_CTRL_MAXRRS_MIN);
		alx_mem_w32(hw, L1C_DEV_CTRL, val);
		dmar_len = L1C_DEV_CTRL_MAXRRS_MIN;
	}
	val = FIELDL(L1C_TXQ0_TPD_BURSTPREF, L1C_TXQ0_TPD_BURSTPREF_DEF) |
	      L1C_TXQ0_MODE_ENHANCE |
	      L1C_TXQ0_LSO_8023_EN |
	      L1C_TXQ0_SUPT_IPOPT |
	      FIELDL(L1C_TXQ0_TXF_BURST_PREF,
		     (hw->pci_devid == L2CB_DEV_ID ||
		      hw->pci_devid == L2CB2_DEV_ID) ?
		     L1C_TXQ0_TXF_BURST_PREF_L2CB :
		     L1C_TXQ0_TXF_BURST_PREF_DEF);
	alx_mem_w32(hw, L1C_TXQ0, val);

	/* fc */
	alx_mem_r32(hw, L1C_SRAM5, &val);
	val = FIELD_GETX(val, L1C_SRAM_RXF_LEN) << 3; /* bytes */
	if (val > L1C_SRAM_RXF_LEN_8K) {
		val16 = L1C_MTU_STD_ALGN;
		val = (val - (2 * L1C_MTU_STD_ALGN + L1C_MTU_MIN));
	} else {
		val16 = L1C_MTU_STD_ALGN;
		val = (val - L1C_MTU_STD_ALGN);
	}
	alx_mem_w32(hw, L1C_RXQ2,
		    FIELDL(L1C_RXQ2_RXF_XOFF_THRESH, val16 >> 3) |
		    FIELDL(L1C_RXQ2_RXF_XON_THRESH, val >> 3));
	/* rxq */
	val = FIELDL(L1C_RXQ0_NUM_RFD_PREF, L1C_RXQ0_NUM_RFD_PREF_DEF) |
	    L1C_RXQ0_IPV6_PARSE_EN;
	if (mtu > L1C_MTU_JUMBO_TH)
		val |= L1C_RXQ0_CUT_THRU_EN;
	if ((hw->pci_devid & 1) != 0) {
		FIELD_SETL(val, L1C_RXQ0_ASPM_THRESH,
			   (hw->pci_devid == L1D2_DEV_ID) ?
			   L1C_RXQ0_ASPM_THRESH_NO :
			   L1C_RXQ0_ASPM_THRESH_100M);
	}
	alx_mem_w32(hw, L1C_RXQ0, val);

	/* rfd producer index */
	alx_mem_w32(hw, L1C_RFD_PIDX, (u32)rxring_sz - 1);

	/* DMA */
	val = FIELDL(L1C_DMA_RORDER_MODE, L1C_DMA_RORDER_MODE_OUT) |
	      L1C_DMA_RREQ_PRI_DATA |
	      FIELDL(L1C_DMA_RREQ_BLEN, dmar_len) |
	      FIELDL(L1C_DMA_WDLY_CNT, L1C_DMA_WDLY_CNT_DEF) |
	      FIELDL(L1C_DMA_RDLY_CNT, L1C_DMA_RDLY_CNT_DEF) ;
	alx_mem_w32(hw, L1C_DMA, val);

	return 0;
}


u16 l1c_get_phy_config(struct alx_hw *hw)
{
	u32 val;
	u16 phy_val;

	alx_mem_r32(hw, L1C_PHY_CTRL, &val);
	if ((val & L1C_PHY_CTRL_DSPRST_OUT) == 0) { /* phy in rst */
		return LX_DRV_PHY_UNKNOWN;
	}

	alx_mem_r32(hw, L1C_DRV, &val);
	val = FIELD_GETX(val, LX_DRV_PHY);
	if (LX_DRV_PHY_UNKNOWN == val)
		return LX_DRV_PHY_UNKNOWN;

	l1c_read_phy(hw, false, 0, false, L1C_MII_DBG_ADDR, &phy_val);

	if (LX_PHY_INITED == phy_val)
		return (u16) val;

	return LX_DRV_PHY_UNKNOWN;
}

