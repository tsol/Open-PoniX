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

#include "alf_hw.h"


/* get permanent mac address from
 *    0: success
 *    non-0:fail
 */
u16 l1f_get_perm_macaddr(struct alx_hw *hw, u8 *addr)
{
	u32 val, mac0, mac1;
	u16 flag, i;

#define INTN_LOADED 0x1
#define EXTN_LOADED 0x2

	flag = 0;
	val = 0;

read_mcadr:

	/* get it from register first */
	alx_mem_r32(hw, L1F_STAD0, &mac0);
	alx_mem_r32(hw, L1F_STAD1, &mac1);

	*(u32 *)(addr + 2) = LX_SWAP_DW(mac0);
	*(u16 *)addr = (u16)LX_SWAP_W((u16)mac1);

	if (macaddr_valid(addr))
		return 0;

	if ((flag & INTN_LOADED) == 0) {
		/* load from efuse ? */
		for (i = 0; i < L1F_SLD_MAX_TO; i++) {
			alx_mem_r32(hw, L1F_SLD, &val);
			if ((val & (L1F_SLD_STAT | L1F_SLD_START)) == 0)
				break;
			mdelay(1);
		}
		if (i == L1F_SLD_MAX_TO)
			goto out;
		alx_mem_w32(hw, L1F_SLD, val | L1F_SLD_START);
		for (i = 0; i < L1F_SLD_MAX_TO; i++) {
			mdelay(1);
			alx_mem_r32(hw, L1F_SLD, &val);
			if ((val & L1F_SLD_START) == 0)
				break;
		}
		if (i == L1F_SLD_MAX_TO)
			goto out;
		flag |= INTN_LOADED;
		goto read_mcadr;
	}

	if ((flag & EXTN_LOADED) == 0) {
		alx_mem_r32(hw, L1F_EFLD, &val);
		if ((val & (L1F_EFLD_F_EXIST | L1F_EFLD_E_EXIST)) != 0) {
			/* load from eeprom/flash ? */
			for (i = 0; i < L1F_SLD_MAX_TO; i++) {
				alx_mem_r32(hw, L1F_EFLD, &val);
				if ((val & (L1F_EFLD_STAT |
					    L1F_EFLD_START)) == 0) {
					break;
				}
				mdelay(1);
			}
			if (i == L1F_SLD_MAX_TO)
				goto out;
			alx_mem_w32(hw, L1F_EFLD, val | L1F_EFLD_START);
			for (i = 0; i < L1F_SLD_MAX_TO; i++) {
				mdelay(1);
				alx_mem_r32(hw, L1F_EFLD, &val);
				if ((val & L1F_EFLD_START) == 0)
					break;
			}
			if (i == L1F_SLD_MAX_TO)
				goto out;
			flag |= EXTN_LOADED;
			goto read_mcadr;
		}
	}

out:
	return LX_ERR_ALOAD;
}


/* reset mac & dma
 * return
 *     0: success
 *     non-0:fail
 */
u16 l1f_reset_mac(struct alx_hw *hw)
{
	u32 val, pmctrl = 0;
	u16 ret;
	u16 i;
	u8 rev = (u8)(FIELD_GETX(hw->pci_revid, L1F_PCI_REVID));

	/* disable all interrupts, RXQ/TXQ */
	alx_mem_w32(hw, L1F_MSIX_MASK, BIT_ALL); /* ???? msi-x */
	alx_mem_w32(hw, L1F_IMR, 0);
	alx_mem_w32(hw, L1F_ISR, L1F_ISR_DIS);

	ret = l1f_enable_mac(hw, false, 0);
	if (ret != 0)
		return ret;

	/* mac reset workaroud */
	alx_mem_w32(hw, L1F_RFD_PIDX, 1);

	/* dis l0s/l1 before mac reset */
	if ((rev == L1F_REV_A0 || rev == L1F_REV_A1) &&
	    (hw->pci_revid & L1F_PCI_REVID_WTH_CR) != 0) {
		alx_mem_r32(hw, L1F_PMCTRL, &pmctrl);
		if ((pmctrl & (L1F_PMCTRL_L1_EN | L1F_PMCTRL_L0S_EN)) != 0) {
			alx_mem_w32(hw, L1F_PMCTRL,
				    pmctrl & ~(L1F_PMCTRL_L1_EN |
					       L1F_PMCTRL_L0S_EN));
		}
	}

	/* reset whole mac safely */
	alx_mem_r32(hw, L1F_MASTER, &val);
	alx_mem_w32(hw, L1F_MASTER,
		    val | L1F_MASTER_DMA_MAC_RST | L1F_MASTER_OOB_DIS);

	/* make sure it's real idle */
	udelay(10);
	for (i = 0; i < L1F_DMA_MAC_RST_TO; i++) {
		alx_mem_r32(hw, L1F_RFD_PIDX, &val);
		if (val == 0)
			break;
		udelay(10);
	}
	for (; i < L1F_DMA_MAC_RST_TO; i++) {
		alx_mem_r32(hw, L1F_MASTER, &val);
		if ((val & L1F_MASTER_DMA_MAC_RST) == 0)
			break;
		udelay(10);
	}
	if (i == L1F_DMA_MAC_RST_TO)
		return LX_ERR_RSTMAC;
	udelay(10);

	if ((rev == L1F_REV_A0 || rev == L1F_REV_A1) &&
	    (hw->pci_revid & L1F_PCI_REVID_WTH_CR) != 0) {
		/* set L1F_MASTER_PCLKSEL_SRDS (affect by soft-rst, PERST) */
		alx_mem_w32(hw, L1F_MASTER, val | L1F_MASTER_PCLKSEL_SRDS);
		/* resoter l0s / l1 */
		if ((pmctrl & (L1F_PMCTRL_L1_EN | L1F_PMCTRL_L0S_EN)) != 0)
			alx_mem_w32(hw, L1F_PMCTRL, pmctrl);
	}

	/* clear Internal OSC settings, switching OSC by hw itself,
	 * disable isoloate for A0 */
	alx_mem_r32(hw, L1F_MISC3, &val);
	alx_mem_w32(hw, L1F_MISC3,
		    (val & ~L1F_MISC3_25M_BY_SW) | L1F_MISC3_25M_NOTO_INTNL);
	alx_mem_r32(hw, L1F_MISC, &val);
	val &= ~L1F_MISC_INTNLOSC_OPEN;
	if (rev == L1F_REV_A0 || rev == L1F_REV_A1)
		val &= ~L1F_MISC_ISO_EN;
	alx_mem_w32(hw, L1F_MISC, val);
	udelay(20);

	/* driver control speed/duplex, hash-alg */
	alx_mem_r32(hw, L1F_MAC_CTRL, &val);
	alx_mem_w32(hw, L1F_MAC_CTRL, val | L1F_MAC_CTRL_WOLSPED_SWEN);

	/* clk sw */
	alx_mem_r32(hw, L1F_SERDES, &val);
	alx_mem_w32(hw, L1F_SERDES,
		    val | L1F_SERDES_MACCLK_SLWDWN | L1F_SERDES_PHYCLK_SLWDWN);

	return 0;
}

/* reset phy
 * return
 *    0: success
 *    non-0:fail
 */
u16 l1f_reset_phy(struct alx_hw *hw, bool pws_en, bool az_en, bool ptp_en)
{
	u32 val;
	u16 i, phy_val;

	az_en = az_en;
	ptp_en = ptp_en;

	/* reset PHY core */
	alx_mem_r32(hw, L1F_PHY_CTRL, &val);
	val &= ~(L1F_PHY_CTRL_DSPRST_OUT | L1F_PHY_CTRL_IDDQ |
		 L1F_PHY_CTRL_GATE_25M | L1F_PHY_CTRL_POWER_DOWN |
		 L1F_PHY_CTRL_CLS);
	val |= L1F_PHY_CTRL_RST_ANALOG;

	if (pws_en)
		val |= (L1F_PHY_CTRL_HIB_PULSE | L1F_PHY_CTRL_HIB_EN);
	else
		val &= ~(L1F_PHY_CTRL_HIB_PULSE | L1F_PHY_CTRL_HIB_EN);
	alx_mem_w32(hw, L1F_PHY_CTRL, val);
	udelay(10); /* 5us is enough */
	alx_mem_w32(hw, L1F_PHY_CTRL, val | L1F_PHY_CTRL_DSPRST_OUT);

	for (i = 0; i < L1F_PHY_CTRL_DSPRST_TO; i++) { /* delay 800us */
		udelay(10);
	}

	/* ???? phy power saving */

	l1f_write_phydbg(hw, true,
			 L1F_MIIDBG_TST10BTCFG, L1F_TST10BTCFG_DEF);
	l1f_write_phydbg(hw, true, L1F_MIIDBG_SRDSYSMOD, L1F_SRDSYSMOD_DEF);
	l1f_write_phydbg(hw, true,
			 L1F_MIIDBG_TST100BTCFG, L1F_TST100BTCFG_DEF);
	l1f_write_phydbg(hw, true, L1F_MIIDBG_ANACTRL, L1F_ANACTRL_DEF);
	l1f_read_phydbg(hw, true, L1F_MIIDBG_GREENCFG2, &phy_val);
	l1f_write_phydbg(hw, true, L1F_MIIDBG_GREENCFG2,
			 phy_val & ~L1F_GREENCFG2_GATE_DFSE_EN);
	/* rtl8139c, 120m */
	l1f_write_phy(hw, true, L1F_MIIEXT_ANEG, true,
		      L1F_MIIEXT_NLP78, L1F_MIIEXT_NLP78_120M_DEF);

	/* set phy interrupt mask */
	l1f_write_phy(hw, false, 0, true,
		      L1F_MII_IER, L1F_IER_LINK_UP | L1F_IER_LINK_DOWN);


	/* TODO *****???? */
	return 0;
}


/* reset pcie
 * just reset pcie relative registers (pci command, clk, aspm...)
 * return
 *    0:success
 *    non-0:fail
 */
u16 l1f_reset_pcie(struct alx_hw *hw, bool l0s_en, bool l1_en)
{
	u32 val;
	u16 val16;
	u16 ret;
	u8 rev = (u8)(FIELD_GETX(hw->pci_revid, L1F_PCI_REVID));

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
	alx_cfg_w16(hw, L1F_PM_CSR, 0);

	/* deflt val of PDLL D3PLLOFF */
	alx_mem_r32(hw, L1F_PDLL_TRNS1, &val);
	alx_mem_w32(hw, L1F_PDLL_TRNS1, val & ~L1F_PDLL_TRNS1_D3PLLOFF_EN);

	/* mask some pcie error bits */
	alx_mem_r32(hw, L1F_UE_SVRT, &val);
	val &= ~(L1F_UE_SVRT_DLPROTERR | L1F_UE_SVRT_FCPROTERR);
	alx_mem_w32(hw, L1F_UE_SVRT, val);

	/* wol 25M  & pclk */
	alx_mem_r32(hw, L1F_MASTER, &val);
	if ((rev == L1F_REV_A0 || rev == L1F_REV_A1) &&
	    (hw->pci_revid & L1F_PCI_REVID_WTH_CR) != 0) {
		if ((val & L1F_MASTER_WAKEN_25M) == 0 ||
		    (val & L1F_MASTER_PCLKSEL_SRDS) == 0) {
			alx_mem_w32(hw, L1F_MASTER,
				    val | L1F_MASTER_PCLKSEL_SRDS |
				    L1F_MASTER_WAKEN_25M);
		}
	} else {
		if ((val & L1F_MASTER_WAKEN_25M) == 0 ||
		    (val & L1F_MASTER_PCLKSEL_SRDS) != 0) {
			alx_mem_w32(hw, L1F_MASTER,
				    (val & ~L1F_MASTER_PCLKSEL_SRDS) |
				    L1F_MASTER_WAKEN_25M);
		}
	}

	/* l0s, l1 setting */
	ret = l1f_enable_aspm(hw, l0s_en, l1_en, 0);

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
u16 l1f_enable_mac(struct alx_hw *hw, bool en, u16 en_ctrl)
{
	u32 rxq, txq, mac, val;
	u16 i;

	alx_mem_r32(hw, L1F_RXQ0, &rxq);
	alx_mem_r32(hw, L1F_TXQ0, &txq);
	alx_mem_r32(hw, L1F_MAC_CTRL, &mac);

	if (en) { /* enable */
		alx_mem_w32(hw, L1F_RXQ0, rxq | L1F_RXQ0_EN);
		alx_mem_w32(hw, L1F_TXQ0, txq | L1F_TXQ0_EN);
		if ((en_ctrl & LX_MACSPEED_1000) != 0) {
			FIELD_SETL(mac, L1F_MAC_CTRL_SPEED,
				   L1F_MAC_CTRL_SPEED_1000);
		} else {
			FIELD_SETL(mac, L1F_MAC_CTRL_SPEED,
				   L1F_MAC_CTRL_SPEED_10_100);
		}

		test_set_or_clear(mac, en_ctrl, LX_MACDUPLEX_FULL,
				  L1F_MAC_CTRL_FULLD);
		/* rx filter */
		test_set_or_clear(mac, en_ctrl, LX_FLT_PROMISC,
				  L1F_MAC_CTRL_PROMISC_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_MULTI_ALL,
				  L1F_MAC_CTRL_MULTIALL_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_BROADCAST,
				  L1F_MAC_CTRL_BRD_EN);
		test_set_or_clear(mac, en_ctrl, LX_FLT_DIRECT,
				  L1F_MAC_CTRL_RX_EN);
		test_set_or_clear(mac, en_ctrl, LX_FC_TXEN,
				  L1F_MAC_CTRL_TXFC_EN);
		test_set_or_clear(mac, en_ctrl, LX_FC_RXEN,
				  L1F_MAC_CTRL_RXFC_EN);
		test_set_or_clear(mac, en_ctrl, LX_VLAN_STRIP,
				  L1F_MAC_CTRL_VLANSTRIP);
		test_set_or_clear(mac, en_ctrl, LX_LOOPBACK,
				  L1F_MAC_CTRL_LPBACK_EN);
		test_set_or_clear(mac, en_ctrl, LX_SINGLE_PAUSE,
				  L1F_MAC_CTRL_SPAUSE_EN);
		test_set_or_clear(mac, en_ctrl, LX_ADD_FCS,
				  (L1F_MAC_CTRL_PCRCE | L1F_MAC_CTRL_CRCE));

		alx_mem_w32(hw, L1F_MAC_CTRL, mac | L1F_MAC_CTRL_TX_EN);
	} else { /* disable mac */
		alx_mem_w32(hw, L1F_RXQ0, rxq & ~L1F_RXQ0_EN);
		alx_mem_w32(hw, L1F_TXQ0, txq & ~L1F_TXQ0_EN);

		/* waiting for rxq/txq be idle */
		udelay(40);

		/* stop mac tx/rx */
		alx_mem_w32(hw, L1F_MAC_CTRL,
			    mac & ~(L1F_MAC_CTRL_RX_EN | L1F_MAC_CTRL_TX_EN));

		for (i = 0; i < L1F_DMA_MAC_RST_TO; i++) {
			alx_mem_r32(hw, L1F_MAC_STS, &val);
			if ((val & L1F_MAC_STS_IDLE) == 0)
				break;
			udelay(10);
		}
		if (L1F_DMA_MAC_RST_TO == i)
			return LX_ERR_RSTMAC;
	}

	return 0;
}

/* enable/disable aspm support
 * that will change settings for phy/mac/pcie
 */
u16 l1f_enable_aspm(struct alx_hw *hw, bool l0s_en, bool l1_en, u8 lnk_stat)
{
	u32 pmctrl;
	u8 rev = (u8)(FIELD_GETX(hw->pci_revid, L1F_PCI_REVID));

	lnk_stat = lnk_stat;


	alx_mem_r32(hw, L1F_PMCTRL, &pmctrl);

	/* ????default */
	FIELD_SETL(pmctrl, L1F_PMCTRL_LCKDET_TIMER,
		   L1F_PMCTRL_LCKDET_TIMER_DEF);
	pmctrl |= L1F_PMCTRL_RCVR_WT_1US    |   /* wait 1us */
		  L1F_PMCTRL_L1_CLKSW_EN    |   /* pcie clk sw */
		  L1F_PMCTRL_L1_SRDSRX_PWD  ;   /* pwd serdes ????default */
	/* ????default */
	FIELD_SETL(pmctrl, L1F_PMCTRL_L1REQ_TO, L1F_PMCTRL_L1REG_TO_DEF);
	FIELD_SETL(pmctrl, L1F_PMCTRL_L1_TIMER, L1F_PMCTRL_L1_TIMER_16US);
	pmctrl &= ~(L1F_PMCTRL_L1_SRDS_EN |
		    L1F_PMCTRL_L1_SRDSPLL_EN |
		    L1F_PMCTRL_L1_BUFSRX_EN |
		    L1F_PMCTRL_SADLY_EN |       /* ???default */
		    L1F_PMCTRL_HOTRST_WTEN|
		    L1F_PMCTRL_L0S_EN |
		    L1F_PMCTRL_L1_EN |
		    L1F_PMCTRL_ASPM_FCEN |
		    L1F_PMCTRL_TXL1_AFTER_L0S |
		    L1F_PMCTRL_RXL1_AFTER_L0S
		    );
	if ((rev == L1F_REV_A0 || rev == L1F_REV_A1) &&
	    (hw->pci_revid & L1F_PCI_REVID_WTH_CR) != 0) {
		pmctrl |= L1F_PMCTRL_L1_SRDS_EN | L1F_PMCTRL_L1_SRDSPLL_EN;
	}

	/* on/off l0s only if bios/system enable l0s */
	if (/* sysl0s_en && */ l0s_en)
		pmctrl |= (L1F_PMCTRL_L0S_EN | L1F_PMCTRL_ASPM_FCEN);
	/* on/off l1 only if bios/system enable l1 */
	if (/* sysl1_en && */ l1_en)
		pmctrl |= (L1F_PMCTRL_L1_EN | L1F_PMCTRL_ASPM_FCEN);

	alx_mem_w32(hw, L1F_PMCTRL, pmctrl);

	return 0;
}


/* initialize phy for speed / flow control
 * lnk_cap
 *    if autoNeg, is link capability to tell the peer
 *    if force mode, is forced speed/duplex
 */
u16 l1f_init_phy_spdfc(struct alx_hw *hw, bool auto_neg,
		       u8 lnk_cap, bool fc_en)
{
	u16 adv, giga, cr;
	u32 val;
	u16 ret;

	/* clear flag */
	l1f_write_phy(hw, false, 0, false, L1F_MII_DBG_ADDR, 0);
	alx_mem_r32(hw, L1F_DRV, &val);
	FIELD_SETL(val, LX_DRV_PHY, 0);

	if (auto_neg) {
		adv = L1F_ADVERTISE_DEFAULT_CAP & ~L1F_ADVERTISE_SPEED_MASK;
		giga = L1F_GIGA_CR_1000T_DEFAULT_CAP &
			~L1F_GIGA_CR_1000T_SPEED_MASK;
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
			giga |= L1F_GIGA_CR_1000T_FD_CAPS;
			val |= LX_DRV_PHY_1000 | LX_DRV_PHY_DUPLEX;
		}

		ret = l1f_write_phy(hw, false, 0, false, MII_ADVERTISE, adv);
		ret = l1f_write_phy(hw, false, 0, false, MII_CTRL1000, giga);

		cr = BMCR_RESET | BMCR_ANENABLE | BMCR_ANRESTART;
		ret = l1f_write_phy(hw, false, 0, false, MII_BMCR, cr);
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
		ret = l1f_write_phy(hw, false, 0, false, MII_BMCR, cr);
	}

	if (!ret) {
		l1f_write_phy(hw, false, 0, false,
			      L1F_MII_DBG_ADDR, LX_PHY_INITED);
	}
	alx_mem_w32(hw, L1F_DRV, val);

	return ret;
}


/* do power saving setting befor enter suspend mode
 * NOTE:
 *    1. phy link must be established before calling this function
 *    2. wol option (pattern,magic,link,etc.) is configed before call it.
 */
u16 l1f_powersaving(struct alx_hw *hw,
		    u8 wire_spd,
		    bool wol_en,
		    bool mactx_en,
		    bool macrx_en,
		    bool pws_en)
{
	u32 master_ctrl, mac_ctrl, phy_ctrl, val;
	u16 pm_ctrl, ret = 0;

	master_ctrl = 0;
	mac_ctrl = 0;
	phy_ctrl = 0;

	pws_en = pws_en;

	alx_mem_r32(hw, L1F_MASTER, &master_ctrl);
	master_ctrl &= ~L1F_MASTER_PCLKSEL_SRDS;

	alx_mem_r32(hw, L1F_MAC_CTRL, &mac_ctrl);
	/* 10/100 half */
	FIELD_SETL(mac_ctrl, L1F_MAC_CTRL_SPEED,  L1F_MAC_CTRL_SPEED_10_100);
	mac_ctrl &= ~(L1F_MAC_CTRL_FULLD |
		      L1F_MAC_CTRL_RX_EN |
		      L1F_MAC_CTRL_TX_EN);

	alx_mem_r32(hw, L1F_PHY_CTRL, &phy_ctrl);
	phy_ctrl &= ~(L1F_PHY_CTRL_DSPRST_OUT | L1F_PHY_CTRL_CLS);
	/* if (pws_en) { */
	phy_ctrl |= (L1F_PHY_CTRL_RST_ANALOG | L1F_PHY_CTRL_HIB_PULSE |
		     L1F_PHY_CTRL_HIB_EN);

	if (wol_en) { /* enable rx packet or tx packet */
		if (macrx_en)
			mac_ctrl |= (L1F_MAC_CTRL_RX_EN | L1F_MAC_CTRL_BRD_EN);
		if (mactx_en)
			mac_ctrl |= L1F_MAC_CTRL_TX_EN;
		if (LX_LC_1000F == wire_spd) {
			FIELD_SETL(mac_ctrl, L1F_MAC_CTRL_SPEED,
				   L1F_MAC_CTRL_SPEED_1000);
		}
		if (LX_LC_10F == wire_spd ||
		    LX_LC_100F == wire_spd ||
		    LX_LC_100F == wire_spd) {
			mac_ctrl |= L1F_MAC_CTRL_FULLD;
		}
		phy_ctrl |= L1F_PHY_CTRL_DSPRST_OUT;
		ret = l1f_write_phy(hw, false, 0, false, L1F_MII_IER,
				    L1F_IER_LINK_UP);
	} else {
		ret = l1f_write_phy(hw, false, 0, false, L1F_MII_IER, 0);
		phy_ctrl |= (L1F_PHY_CTRL_IDDQ | L1F_PHY_CTRL_POWER_DOWN);
	}
	alx_mem_w32(hw, L1F_MASTER, master_ctrl);
	alx_mem_w32(hw, L1F_MAC_CTRL, mac_ctrl);
	alx_mem_w32(hw, L1F_PHY_CTRL, phy_ctrl);

	/* set val of PDLL D3PLLOFF */
	alx_mem_r32(hw, L1F_PDLL_TRNS1, &val);
	alx_mem_w32(hw, L1F_PDLL_TRNS1, val | L1F_PDLL_TRNS1_D3PLLOFF_EN);

	/* set PME_EN */
	if (wol_en) {
		alx_cfg_r16(hw, L1F_PM_CSR, &pm_ctrl);
		pm_ctrl |= L1F_PM_CSR_PME_EN;
		alx_cfg_w16(hw, L1F_PM_CSR, pm_ctrl);
	}

	return ret;
}


/* read phy register */
u16 l1f_read_phy(struct alx_hw *hw, bool ext, u8 dev, bool fast,
		 u16 reg, u16 *data)
{
	u32 val;
	u16 clk_sel, i, ret = 0;

	*data = 0;
	clk_sel = fast ?
	    (u16)L1F_MDIO_CLK_SEL_25MD4 : (u16)L1F_MDIO_CLK_SEL_25MD128;

	if (ext) {
		val = FIELDL(L1F_MDIO_EXTN_DEVAD, dev) |
		      FIELDL(L1F_MDIO_EXTN_REG, reg);
		alx_mem_w32(hw, L1F_MDIO_EXTN, val);

		val = L1F_MDIO_SPRES_PRMBL |
		      FIELDL(L1F_MDIO_CLK_SEL, clk_sel) |
		      L1F_MDIO_START |
		      L1F_MDIO_MODE_EXT |
		      L1F_MDIO_OP_READ;
	} else {
		val = L1F_MDIO_SPRES_PRMBL |
		      FIELDL(L1F_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1F_MDIO_REG, reg) |
		      L1F_MDIO_START |
		      L1F_MDIO_OP_READ;
	}

	alx_mem_w32(hw, L1F_MDIO, val);

	for (i = 0; i < L1F_MDIO_MAX_AC_TO; i++) {
		alx_mem_r32(hw, L1F_MDIO, &val);
		if ((val & L1F_MDIO_BUSY) == 0) {
			*data = (u16)FIELD_GETX(val, L1F_MDIO_DATA);
			break;
		}
		udelay(10);
	}

	if (L1F_MDIO_MAX_AC_TO == i)
		ret = LX_ERR_MIIBUSY;

	return ret;
}

/* write phy register */
u16 l1f_write_phy(struct alx_hw *hw, bool ext, u8 dev, bool fast,
		  u16 reg, u16 data)
{
	u32 val;
	u16 clk_sel, i, ret = 0;

	clk_sel = fast ?
	    (u16)L1F_MDIO_CLK_SEL_25MD4 : (u16)L1F_MDIO_CLK_SEL_25MD128;

	if (ext) {
		val = FIELDL(L1F_MDIO_EXTN_DEVAD, dev) |
		      FIELDL(L1F_MDIO_EXTN_REG, reg);
		alx_mem_w32(hw, L1F_MDIO_EXTN, val);

		val = L1F_MDIO_SPRES_PRMBL |
		      FIELDL(L1F_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1F_MDIO_DATA, data) |
		      L1F_MDIO_START |
		      L1F_MDIO_MODE_EXT;
	} else {
		val = L1F_MDIO_SPRES_PRMBL |
		      FIELDL(L1F_MDIO_CLK_SEL, clk_sel) |
		      FIELDL(L1F_MDIO_REG, reg) |
		      FIELDL(L1F_MDIO_DATA, data) |
		      L1F_MDIO_START;
	}

	alx_mem_w32(hw, L1F_MDIO, val);

	for (i = 0; i < L1F_MDIO_MAX_AC_TO; i++) {
		alx_mem_r32(hw, L1F_MDIO, &val);
		if ((val & L1F_MDIO_BUSY) == 0)
			break;
		udelay(10);
	}

	if (L1F_MDIO_MAX_AC_TO == i)
		ret = LX_ERR_MIIBUSY;

	return ret;
}

u16 l1f_read_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 *data)
{
	u16 ret;

	ret = l1f_write_phy(hw, false, 0, fast, L1F_MII_DBG_ADDR, reg);
	ret = l1f_read_phy(hw, false, 0, fast, L1F_MII_DBG_DATA, data);

	return ret;
}

u16 l1f_write_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 data)
{
	u16 ret;

	ret = l1f_write_phy(hw, false, 0, fast, L1F_MII_DBG_ADDR, reg);
	ret = l1f_write_phy(hw, false, 0, fast, L1F_MII_DBG_DATA, data);

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
u16 l1f_init_mac(struct alx_hw *hw, u8 *addr, u32 txmem_hi,
		 u32 *tx_mem_lo, u8 tx_qnum, u16 txring_sz,
		 u32 rxmem_hi, u32 rfdmem_lo, u32 rrdmem_lo,
		 u16 rxring_sz, u16 rxbuf_sz, u16 smb_timer,
		 u16 mtu, u16 int_mod, bool hash_legacy)
{
	u32 val;
	u16 val16, devid;
	u8 dmar_len;

	alx_cfg_r16(hw, PCI_DEVICE_ID, &devid);

	/* set mac-address */
	val = *(u32 *)(addr + 2);
	alx_mem_w32(hw, L1F_STAD0, LX_SWAP_DW(val));
	val = *(u16 *)addr ;
	alx_mem_w32(hw, L1F_STAD1, LX_SWAP_W((u16)val));

	/* clear multicast hash table, algrithm */
	alx_mem_w32(hw, L1F_HASH_TBL0, 0);
	alx_mem_w32(hw, L1F_HASH_TBL1, 0);
	alx_mem_r32(hw, L1F_MAC_CTRL, &val);
	if (hash_legacy)
		val |= L1F_MAC_CTRL_MHASH_ALG_HI5B;
	else
		val &= ~L1F_MAC_CTRL_MHASH_ALG_HI5B;
	alx_mem_w32(hw, L1F_MAC_CTRL, val);

	/* clear any wol setting/status */
	alx_mem_r32(hw, L1F_WOL0, &val);
	alx_mem_w32(hw, L1F_WOL0, 0);

	/* clk gating */
	alx_mem_w32(hw, L1F_CLK_GATE,
		    (FIELD_GETX(hw->pci_revid, L1F_PCI_REVID) == L1F_REV_B0) ?
		     L1F_CLK_GATE_ALL_B0 : L1F_CLK_GATE_ALL_A0);

	/* idle timeout to switch clk_125M */
	if (FIELD_GETX(hw->pci_revid, L1F_PCI_REVID) == L1F_REV_B0) {
		alx_mem_w32(hw, L1F_IDLE_DECISN_TIMER,
			    L1F_IDLE_DECISN_TIMER_DEF);
	}

	/* descriptor ring base memory */
	alx_mem_w32(hw, L1F_TX_BASE_ADDR_HI, txmem_hi);
	alx_mem_w32(hw, L1F_TPD_RING_SZ, txring_sz);
	switch (tx_qnum) {
	case 4:
		alx_mem_w32(hw, L1F_TPD_PRI3_ADDR_LO, tx_mem_lo[3]);
		/* fall through */
	case 3:
		alx_mem_w32(hw, L1F_TPD_PRI2_ADDR_LO, tx_mem_lo[2]);
		/* fall through */
	case 2:
		alx_mem_w32(hw, L1F_TPD_PRI1_ADDR_LO, tx_mem_lo[1]);
		/* fall through */
	case 1:
		alx_mem_w32(hw, L1F_TPD_PRI0_ADDR_LO, tx_mem_lo[0]);
		break;
	default:
		return LX_ERR_PARM;
	}
	alx_mem_w32(hw, L1F_RX_BASE_ADDR_HI, rxmem_hi);
	alx_mem_w32(hw, L1F_RFD_ADDR_LO, rfdmem_lo);
	alx_mem_w32(hw, L1F_RRD_ADDR_LO, rrdmem_lo);
	alx_mem_w32(hw, L1F_RFD_BUF_SZ, rxbuf_sz);
	alx_mem_w32(hw, L1F_RRD_RING_SZ, rxring_sz);
	alx_mem_w32(hw, L1F_RFD_RING_SZ, rxring_sz);
	alx_mem_w32(hw, L1F_SMB_TIMER, smb_timer * 500UL);
	alx_mem_w32(hw, L1F_SRAM9, L1F_SRAM_LOAD_PTR);

	/* int moduration */
	alx_mem_r32(hw, L1F_MASTER, &val);
/*    val = (val & ~L1F_MASTER_IRQMOD2_EN) | */
	val = val | L1F_MASTER_IRQMOD2_EN |
		    L1F_MASTER_IRQMOD1_EN |
		    L1F_MASTER_SYSALVTIMER_EN;  /* sysalive */
	alx_mem_w32(hw, L1F_MASTER, val);
	alx_mem_w32(hw, L1F_IRQ_MODU_TIMER,
		    FIELDL(L1F_IRQ_MODU_TIMER1, int_mod >> 1));

	/* tpd threshold to trig int */
	alx_mem_w32(hw, L1F_TINT_TPD_THRSHLD, (u32)txring_sz / 3);
	alx_mem_w32(hw, L1F_TINT_TIMER, int_mod);
	/* re-send int */
	alx_mem_w32(hw, L1F_INT_RETRIG, L1F_INT_RETRIG_TO);

	/* mtu */
	alx_mem_w32(hw, L1F_MTU, (u32)(mtu + 4 + 4)); /* crc + vlan */
	if (mtu > L1F_MTU_JUMBO_TH) {
		alx_mem_r32(hw, L1F_MAC_CTRL, &val);
		alx_mem_w32(hw, L1F_MAC_CTRL, val & ~L1F_MAC_CTRL_FAST_PAUSE);
	}

	/* txq */
	if ((mtu + 8) < L1F_TXQ1_JUMBO_TSO_TH)
		val = (u32)(mtu + 8 + 7) >> 3; /* 7 for QWORD align */
	else
		val = L1F_TXQ1_JUMBO_TSO_TH >> 3;
	alx_mem_w32(hw, L1F_TXQ1, val | L1F_TXQ1_ERRLGPKT_DROP_EN);
	alx_mem_r32(hw, L1F_DEV_CTRL, &val);
	dmar_len = (u8)FIELD_GETX(val, L1F_DEV_CTRL_MAXRRS);
	/* if BIOS had changed the default dma read max length,
	 * restore it to default value */
	if (dmar_len < L1F_DEV_CTRL_MAXRRS_MIN) {
		FIELD_SETL(val, L1F_DEV_CTRL_MAXRRS, L1F_DEV_CTRL_MAXRRS_MIN);
		alx_mem_w32(hw, L1F_DEV_CTRL, val);
	}
	val = FIELDL(L1F_TXQ0_TPD_BURSTPREF, L1F_TXQ_TPD_BURSTPREF_DEF) |
	      L1F_TXQ0_MODE_ENHANCE |
	      L1F_TXQ0_LSO_8023_EN |
	      L1F_TXQ0_SUPT_IPOPT |
	      FIELDL(L1F_TXQ0_TXF_BURST_PREF, L1F_TXQ_TXF_BURST_PREF_DEF);
	alx_mem_w32(hw, L1F_TXQ0, val);
	val = FIELDL(L1F_HQTPD_Q1_NUMPREF, L1F_TXQ_TPD_BURSTPREF_DEF) |
	      FIELDL(L1F_HQTPD_Q2_NUMPREF, L1F_TXQ_TPD_BURSTPREF_DEF) |
	      FIELDL(L1F_HQTPD_Q3_NUMPREF, L1F_TXQ_TPD_BURSTPREF_DEF) |
	      L1F_HQTPD_BURST_EN;
	alx_mem_w32(hw, L1F_HQTPD, val);

	/* rxq */
	alx_mem_r32(hw, L1F_SRAM5, &val);
	val = FIELD_GETX(val, L1F_SRAM_RXF_LEN) << 3; /* bytes */
	if (val > L1F_SRAM_RXF_LEN_8K) {
		val16 = L1F_MTU_STD_ALGN >> 3;
		val = (val - (2 * L1F_MTU_STD_ALGN + L1F_MTU_MIN)) >> 3;
	} else {
		val16 = L1F_MTU_STD_ALGN >> 3;
		val = (val - L1F_MTU_STD_ALGN) >> 3;
	}
	alx_mem_w32(hw, L1F_RXQ2,
		    FIELDL(L1F_RXQ2_RXF_XOFF_THRESH, val16) |
		    FIELDL(L1F_RXQ2_RXF_XON_THRESH, val));
	val = FIELDL(L1F_RXQ0_NUM_RFD_PREF, L1F_RXQ0_NUM_RFD_PREF_DEF) |
	      FIELDL(L1F_RXQ0_RSS_MODE, L1F_RXQ0_RSS_MODE_DIS) |
	      FIELDL(L1F_RXQ0_IDT_TBL_SIZE, L1F_RXQ0_IDT_TBL_SIZE_DEF) |
	      L1F_RXQ0_RSS_HSTYP_ALL |
	      L1F_RXQ0_RSS_HASH_EN |
	      L1F_RXQ0_IPV6_PARSE_EN;
	if (mtu > L1F_MTU_JUMBO_TH)
		val |= L1F_RXQ0_CUT_THRU_EN;
	if ((devid & 1) != 0) {
		FIELD_SETL(val, L1F_RXQ0_ASPM_THRESH,
			   L1F_RXQ0_ASPM_THRESH_100M);
	}
	alx_mem_w32(hw, L1F_RXQ0, val);

	/* rfd producer index */
	alx_mem_w32(hw, L1F_RFD_PIDX, (u32)rxring_sz - 1);

	/* DMA */
	alx_mem_r32(hw, L1F_DMA, &val);
	val = FIELDL(L1F_DMA_RORDER_MODE, L1F_DMA_RORDER_MODE_OUT) |
	      L1F_DMA_RREQ_PRI_DATA |
	      FIELDL(L1F_DMA_RREQ_BLEN, dmar_len) |
	      FIELDL(L1F_DMA_WDLY_CNT, L1F_DMA_WDLY_CNT_DEF) |
	      FIELDL(L1F_DMA_RDLY_CNT, L1F_DMA_RDLY_CNT_DEF) |
	      FIELDL(L1F_DMA_RCHNL_SEL, hw->dma_chnl - 1);
	alx_mem_w32(hw, L1F_DMA, val);

	return 0;
}


u16 l1f_get_phy_config(struct alx_hw *hw)
{
	u32 val;
	u16 phy_val;

	alx_mem_r32(hw, L1F_PHY_CTRL, &val);

	/* phy in rst */
	if ((val & L1F_PHY_CTRL_DSPRST_OUT) == 0)
		return LX_DRV_PHY_UNKNOWN;

	alx_mem_r32(hw, L1F_DRV, &val);
	val = FIELD_GETX(val, LX_DRV_PHY);

	if (LX_DRV_PHY_UNKNOWN == val)
		return LX_DRV_PHY_UNKNOWN;

	l1f_read_phy(hw, false, 0, false, L1F_MII_DBG_ADDR, &phy_val);

	if (LX_PHY_INITED == phy_val)
		return (u16) val;

	return LX_DRV_PHY_UNKNOWN;
}

