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


#define ALF_REV_ID_AR8161_B0            0x10

/* definition for MSIX */
#define ALF_MSIX_ENTRY_BASE		0x2000
#define ALF_MSIX_ENTRY_SIZE		16
#define ALF_MSIX_MSG_LOADDR_OFF		0
#define ALF_MSIX_MSG_HIADDR_OFF		4
#define ALF_MSIX_MSG_DATA_OFF		8
#define ALF_MSIX_MSG_CTRL_OFF		12

#define ALF_MSIX_INDEX_RXQ0		0
#define ALF_MSIX_INDEX_RXQ1		1
#define ALF_MSIX_INDEX_RXQ2		2
#define ALF_MSIX_INDEX_RXQ3		3
#define ALF_MSIX_INDEX_RXQ4		4
#define ALF_MSIX_INDEX_RXQ5		5
#define ALF_MSIX_INDEX_RXQ6		6
#define ALF_MSIX_INDEX_RXQ7		7
#define ALF_MSIX_INDEX_TXQ0		8
#define ALF_MSIX_INDEX_TXQ1		9
#define ALF_MSIX_INDEX_TXQ2		10
#define ALF_MSIX_INDEX_TXQ3		11
#define ALF_MSIX_INDEX_TIMER		12
#define ALF_MSIX_INDEX_ALERT		13
#define ALF_MSIX_INDEX_SMB		14
#define ALF_MSIX_INDEX_PHY		15


#define ALF_SRAM_BASE		L1F_SRAM0
#define ALF_SRAM(_i, _type) \
		(ALF_SRAM_BASE + ((_i) * sizeof(_type)))

#define ALF_MIB_BASE		L1F_MIB_BASE
#define ALF_MIB(_i, _type) \
		(ALF_MIB_BASE + ((_i) * sizeof(_type)))

/* definition for RSS */
#define ALF_RSS_KEY_BASE	L1F_RSS_KEY0
#define ALF_RSS_IDT_BASE	L1F_RSS_IDT_TBL0
#define ALF_RSS_KEY(_i, _type) \
		(ALF_RSS_KEY_BASE + ((_i) * sizeof(_type)))
#define ALF_RSS_TBL(_i, _type) \
		(L1F_RSS_IDT_TBL0 + ((_i) * sizeof(_type)))


/* NIC */
static int alf_identify_nic(struct alx_hw *hw)
{
	u32 drv;

	if (hw->pci_revid < ALX_REV_ID_AR8161_V2_0)
		return 0;

	/* check from V2_0(b0) to ... */
	switch (hw->pci_revid) {
	default:
		alx_mem_r32(hw, L1F_DRV, &drv);
		if (drv & LX_DRV_DISABLE)
			return -EINVAL;
		break;
	}
	return 0;
}


/* PHY */
static int alf_read_phy_reg(struct alx_hw *hw, u16 reg_addr, u16 *phy_data)
{
	unsigned long  flags;
	int  retval = 0;

	spin_lock_irqsave(&hw->mdio_lock, flags);

	if (l1f_read_phy(hw, false, ALX_MDIO_DEV_TYPE_NORM, false, reg_addr,
			 phy_data)) {
		alx_hw_err(hw, "error when read phy reg\n");
		retval = -EINVAL;
	}

	spin_unlock_irqrestore(&hw->mdio_lock, flags);
	return retval;
}


static int alf_write_phy_reg(struct alx_hw *hw, u16 reg_addr, u16 phy_data)
{
	unsigned long  flags;
	int  retval = 0;

	spin_lock_irqsave(&hw->mdio_lock, flags);

	if (l1f_write_phy(hw, false, ALX_MDIO_DEV_TYPE_NORM, false, reg_addr,
			  phy_data)) {
		alx_hw_err(hw, "error when write phy reg\n");
		retval = -EINVAL;
	}

	spin_unlock_irqrestore(&hw->mdio_lock, flags);
	return retval;
}


static int alf_init_phy(struct alx_hw *hw)
{
	u16 phy_id[2];
	int retval;

	spin_lock_init(&hw->mdio_lock);

	retval = alf_read_phy_reg(hw, MII_PHYSID1, &phy_id[0]);
	if (retval)
		return retval;
	retval = alf_read_phy_reg(hw, MII_PHYSID2, &phy_id[1]);
	if (retval)
		return retval;
	memcpy(&hw->phy_id, phy_id, sizeof(hw->phy_id));

	hw->autoneg_advertised = ALX_LINK_SPEED_1GB_FULL |
				 ALX_LINK_SPEED_10_HALF  |
				 ALX_LINK_SPEED_10_FULL  |
				 ALX_LINK_SPEED_100_HALF |
				 ALX_LINK_SPEED_100_FULL;
	return retval;
}


static int alf_reset_phy(struct alx_hw *hw)
{
	int retval = 0;
	bool pws_en, az_en, ptp_en;

	pws_en = az_en = ptp_en = false;
	CLI_HW_FLAG(PWSAVE_EN);
	CLI_HW_FLAG(AZ_EN);
	CLI_HW_FLAG(PTP_EN);

	if (CHK_HW_FLAG(PWSAVE_CAP)) {
		pws_en = true;
		SET_HW_FLAG(PWSAVE_EN);
	}

	if (CHK_HW_FLAG(AZ_CAP)) {
		az_en = true;
		SET_HW_FLAG(AZ_EN);
	}

	if (CHK_HW_FLAG(PTP_CAP)) {
		ptp_en = true;
		SET_HW_FLAG(PTP_EN);
	}

	alx_hw_info(hw, "reset PHY, pws = %d, az = %d, ptp = %d\n",
		    pws_en, az_en, ptp_en);
	if (l1f_reset_phy(hw, pws_en, az_en, ptp_en)) {
		alx_hw_err(hw, "error when reset phy\n");
		retval = -EINVAL;
	}
	return retval;
}


/* LINK */
static int alf_setup_phy_link(struct alx_hw *hw, u32 speed, bool autoneg,
			      bool fc)
{
	u8 link_cap = 0;
	int retval = 0;

	alx_hw_info(hw, "speed = 0x%x, autoneg = %d\n", speed, autoneg);
	if (speed & ALX_LINK_SPEED_1GB_FULL)
		link_cap |= LX_LC_1000F;

	if (speed & ALX_LINK_SPEED_100_FULL)
		link_cap |= LX_LC_100F;

	if (speed & ALX_LINK_SPEED_100_HALF)
		link_cap |= LX_LC_100H;

	if (speed & ALX_LINK_SPEED_10_FULL)
		link_cap |= LX_LC_10F;

	if (speed & ALX_LINK_SPEED_10_HALF)
		link_cap |= LX_LC_10H;

	if (l1f_init_phy_spdfc(hw, autoneg, link_cap, fc)) {
		alx_hw_err(hw, "error when init phy speed and fc\n");
		retval = -EINVAL;
	}

	return retval;
}


static int alf_setup_phy_link_speed(struct alx_hw *hw, u32 speed,
				    bool autoneg, bool fc)
{
	/*
	 * Clear autoneg_advertised and set new values based on input link
	 * speed.
	 */
	hw->autoneg_advertised = 0;

	if (speed & ALX_LINK_SPEED_1GB_FULL)
		hw->autoneg_advertised |= ALX_LINK_SPEED_1GB_FULL;

	if (speed & ALX_LINK_SPEED_100_FULL)
		hw->autoneg_advertised |= ALX_LINK_SPEED_100_FULL;

	if (speed & ALX_LINK_SPEED_100_HALF)
		hw->autoneg_advertised |= ALX_LINK_SPEED_100_HALF;

	if (speed & ALX_LINK_SPEED_10_FULL)
		hw->autoneg_advertised |= ALX_LINK_SPEED_10_FULL;

	if (speed & ALX_LINK_SPEED_10_HALF)
		hw->autoneg_advertised |= ALX_LINK_SPEED_10_HALF;

	return alf_setup_phy_link(hw, hw->autoneg_advertised,
				  autoneg, fc);
}


static int alf_check_phy_link(struct alx_hw *hw, u32 *speed, bool *link_up)
{
	u16 bmsr, giga;
	int retval;

	alf_read_phy_reg(hw, MII_BMSR, &bmsr);
	retval = alf_read_phy_reg(hw, MII_BMSR, &bmsr);
	if (retval)
		return retval;

	if (!(bmsr & BMSR_LSTATUS)) {
		*link_up = false;
		*speed = ALX_LINK_SPEED_UNKNOWN;
		return 0;
	}
	*link_up = true;

	/* Read PHY Specific Status Register (17) */
	retval = alf_read_phy_reg(hw, L1F_MII_GIGA_PSSR, &giga);
	if (retval)
		return retval;


	if (!(giga & L1F_GIGA_PSSR_SPD_DPLX_RESOLVED)) {
		alx_hw_err(hw, "error for speed duplex resolved\n");
		return -EINVAL;
	}

	switch (giga & L1F_GIGA_PSSR_SPEED) {
	case L1F_GIGA_PSSR_1000MBS:
		if (giga & L1F_GIGA_PSSR_DPLX)
			*speed = ALX_LINK_SPEED_1GB_FULL;
		else
			alx_hw_err(hw, "1000M half is invalid");
		break;
	case L1F_GIGA_PSSR_100MBS:
		if (giga & L1F_GIGA_PSSR_DPLX)
			*speed = ALX_LINK_SPEED_100_FULL;
		else
			*speed = ALX_LINK_SPEED_100_HALF;
		break;
	case L1F_GIGA_PSSR_10MBS:
		if (giga & L1F_GIGA_PSSR_DPLX)
			*speed = ALX_LINK_SPEED_10_FULL;
		else
			*speed = ALX_LINK_SPEED_10_HALF;
		break;
	default:
		*speed = ALX_LINK_SPEED_UNKNOWN;
		retval = -EINVAL;
		break;
	}
	return retval;
}


/*
 * 1. stop_mac
 * 2. reset mac & dma by reg1400(MASTER)
 * 3. control speed/duplex, hash-alg
 * 4. clock switch setting
 */
static int alf_reset_mac(struct alx_hw *hw)
{
	int retval = 0;

	if (l1f_reset_mac(hw)) {
		alx_hw_err(hw, "error when reset mac\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_start_mac(struct alx_hw *hw)
{
	u16 en_ctrl = 0;
	int retval = 0;

	/* set link speed param */
	switch (hw->link_speed) {
	case ALX_LINK_SPEED_1GB_FULL:
		en_ctrl |= LX_MACSPEED_1000;
		/* fall through */
	case ALX_LINK_SPEED_100_FULL:
	case ALX_LINK_SPEED_10_FULL:
		en_ctrl |= LX_MACDUPLEX_FULL;
		break;
	}

	/* set fc param*/
	switch (hw->cur_fc_mode) {
	case alx_fc_full:
		en_ctrl |= LX_FC_RXEN; /* Flow Control RX Enable */
		en_ctrl |= LX_FC_TXEN; /* Flow Control TX Enable */
		break;
	case alx_fc_rx_pause:
		en_ctrl |= LX_FC_RXEN; /* Flow Control RX Enable */
		break;
	case alx_fc_tx_pause:
		en_ctrl |= LX_FC_TXEN; /* Flow Control TX Enable */
		break;
	default:
		break;
	}

	if (hw->fc_single_pause)
		en_ctrl |= LX_SINGLE_PAUSE;

	en_ctrl |= LX_FLT_DIRECT;    /* RX Enable; and TX Always Enable */
	en_ctrl |= LX_FLT_BROADCAST; /* RX Broadcast Enable */
	en_ctrl |= LX_ADD_FCS;

	if (CHK_HW_FLAG(VLANSTRIP_EN))
		en_ctrl |= LX_VLAN_STRIP;

	if (CHK_HW_FLAG(PROMISC_EN))
		en_ctrl |=  LX_FLT_PROMISC;

	if (CHK_HW_FLAG(MULTIALL_EN))
		en_ctrl |= LX_FLT_MULTI_ALL;

	if (CHK_HW_FLAG(LOOPBACK_EN))
		en_ctrl |= LX_LOOPBACK;

	if (l1f_enable_mac(hw, true, en_ctrl)) {
		alx_hw_err(hw, "error when start mac\n");
		retval = -EINVAL;
	}
	return retval;
}


/*
 * 1. stop RXQ (reg15A0) and TXQ (reg1590)
 * 2. stop MAC (reg1480)
 */
static int alf_stop_mac(struct alx_hw *hw)
{
	int retval = 0;

	if (l1f_enable_mac(hw, false, 0)) {
		alx_hw_err(hw, "error when stop mac\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_config_mac(struct alx_hw *hw, u16 rxbuf_sz, u16 rx_qnum,
			  u16 rxring_sz, u16 tx_qnum,  u16 txring_sz)
{
	u8 *addr;
	u32 txmem_hi, txmem_lo[4];
	u32 rxmem_hi, rfdmem_lo, rrdmem_lo;
	u16 smb_timer, mtu_with_eth, int_mod;
	bool hash_legacy;
	int i;
	int retval = 0;

	addr = hw->mac_addr;

	txmem_hi = ALX_DMA_ADDR_HI(hw->tpdma[0]);
	for (i = 0; i < tx_qnum; i++)
		txmem_lo[i] = ALX_DMA_ADDR_LO(hw->tpdma[i]);


	rxmem_hi  = ALX_DMA_ADDR_HI(hw->rfdma[0]);
	rfdmem_lo = ALX_DMA_ADDR_LO(hw->rfdma[0]);
	rrdmem_lo = ALX_DMA_ADDR_LO(hw->rrdma[0]);

	smb_timer = (u16)hw->smb_timer;
	mtu_with_eth = hw->mtu + ALX_ETH_LENGTH_OF_HEADER;
	int_mod = hw->imt;

	hash_legacy = true;

	if (l1f_init_mac(hw, addr, txmem_hi, txmem_lo, tx_qnum, txring_sz,
			 rxmem_hi, rfdmem_lo, rrdmem_lo, rxring_sz, rxbuf_sz,
			 smb_timer, mtu_with_eth, int_mod, hash_legacy)) {
		alx_hw_err(hw, "error when config mac\n");
		retval = -EINVAL;
	}

	return retval;
}


/**
 *  alf_get_mac_addr
 *  @hw: pointer to hardware structure
 **/
static int alf_get_mac_addr(struct alx_hw *hw, u8 *addr)
{
	int retval = 0;

	if (l1f_get_perm_macaddr(hw, addr)) {
		alx_hw_err(hw, "error when get permanent mac address\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_reset_pcie(struct alx_hw *hw, bool l0s_en, bool l1_en)
{
	int retval = 0;

	if (!CHK_HW_FLAG(L0S_CAP))
		l0s_en = false;

	if (l0s_en)
		SET_HW_FLAG(L0S_EN);
	else
		CLI_HW_FLAG(L0S_EN);


	if (!CHK_HW_FLAG(L1_CAP))
		l1_en = false;

	if (l1_en)
		SET_HW_FLAG(L1_EN);
	else
		CLI_HW_FLAG(L1_EN);

	if (l1f_reset_pcie(hw, l0s_en, l1_en)) {
		alx_hw_err(hw, "error when reset pcie\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_config_aspm(struct alx_hw *hw, bool l0s_en, bool l1_en)
{
	int retval = 0;

	if (!CHK_HW_FLAG(L0S_CAP))
		l0s_en = false;

	if (l0s_en)
		SET_HW_FLAG(L0S_EN);
	 else
		CLI_HW_FLAG(L0S_EN);

	if (!CHK_HW_FLAG(L1_CAP))
		l1_en = false;

	if (l1_en)
		SET_HW_FLAG(L1_EN);
	else
		CLI_HW_FLAG(L1_EN);

	if (l1f_enable_aspm(hw, l0s_en, l1_en, 0)) {
		alx_hw_err(hw, "error when enable aspm\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_config_wol(struct alx_hw *hw, u32 wufc)
{
	u32 wol;
	int retval = 0;

	wol = 0;
	/* turn on magic packet event */
	if (wufc & ALX_WOL_MAGIC) {
		wol |= L1F_WOL0_MAGIC_EN | L1F_WOL0_PME_MAGIC_EN;
		/* magic packet maybe Broadcast&multicast&Unicast frame */
		/* mac |= MAC_CTRL_BC_EN; */
	}

	/* turn on link up event */
	if (wufc & ALX_WOL_PHY) {
		wol |=  L1F_WOL0_LINK_EN | L1F_WOL0_PME_LINK;
		/* only link up can wake up */
		retval = alf_write_phy_reg(hw, L1F_MII_IER, L1F_IER_LINK_UP);
	}
	alx_mem_w32(hw, L1F_WOL0, wol);
	return retval;
}


static int alf_config_mac_ctrl(struct alx_hw *hw)
{
	u32 mac;

	alx_mem_r32(hw, L1F_MAC_CTRL, &mac);

	/* enable/disable VLAN tag insert,strip */
	if (CHK_HW_FLAG(VLANSTRIP_EN))
		mac |= L1F_MAC_CTRL_VLANSTRIP;
	else
		mac &= ~L1F_MAC_CTRL_VLANSTRIP;

	if (CHK_HW_FLAG(PROMISC_EN))
		mac |= L1F_MAC_CTRL_PROMISC_EN;
	else
		mac &= ~L1F_MAC_CTRL_PROMISC_EN;

	if (CHK_HW_FLAG(MULTIALL_EN))
		mac |= L1F_MAC_CTRL_MULTIALL_EN;
	else
		mac &= ~L1F_MAC_CTRL_MULTIALL_EN;

	if (CHK_HW_FLAG(LOOPBACK_EN))
		mac |= L1F_MAC_CTRL_LPBACK_EN;
	else
		mac &= ~L1F_MAC_CTRL_LPBACK_EN;

	alx_mem_w32(hw, L1F_MAC_CTRL, mac);
	return 0;
}


static int alf_config_pow_save(struct alx_hw *hw, u32 speed, bool wol_en,
			       bool tx_en, bool rx_en, bool pws_en)
{
	u8 wire_spd = LX_LC_10H;
	int retval = 0;

	switch (speed) {
	case ALX_LINK_SPEED_UNKNOWN:
	case ALX_LINK_SPEED_10_HALF:
		wire_spd = LX_LC_10H;
		break;
	case ALX_LINK_SPEED_10_FULL:
		wire_spd = LX_LC_10F;
		break;
	case ALX_LINK_SPEED_100_HALF:
		wire_spd = LX_LC_100H;
		break;
	case ALX_LINK_SPEED_100_FULL:
		wire_spd = LX_LC_100F;
		break;
	case ALX_LINK_SPEED_1GB_FULL:
		wire_spd = LX_LC_1000F;
		break;
	}

	if (l1f_powersaving(hw, wire_spd, wol_en, tx_en, rx_en, pws_en)) {
		alx_hw_err(hw, "error when set power saving\n");
		retval = -EINVAL;
	}
	return retval;
}


/* RAR, Multicast, VLAN */
static int alf_set_mac_addr(struct alx_hw *hw, u8 *addr)
{
	u32 sta;

	/*
	 * for example: 00-0B-6A-F6-00-DC
	 * 0<-->6AF600DC, 1<-->000B.
	 */

	/* low dword */
	sta = (((u32)addr[2]) << 24) | (((u32)addr[3]) << 16) |
	      (((u32)addr[4]) << 8)  | (((u32)addr[5])) ;
	alx_mem_w32(hw, L1F_STAD0, sta);

	/* hight dword */
	sta = (((u32)addr[0]) << 8) | (((u32)addr[1])) ;
	alx_mem_w32(hw, L1F_STAD1, sta);
	return 0;
}


static int alf_set_mc_addr(struct alx_hw *hw, u8 *addr)
{
	u32 crc32, bit, reg, mta;

	/*
	 * set hash value for a multicast address hash calcu processing.
	 *   1. calcu 32bit CRC for multicast address
	 *   2. reverse crc with MSB to LSB
	 */
	crc32 = ALX_ETH_CRC(addr, ALX_ETH_LENGTH_OF_ADDRESS);

	/*
	 * The HASH Table  is a register array of 2 32-bit registers.
	 * It is treated like an array of 64 bits.  We want to set
	 * bit BitArray[hash_value]. So we figure out what register
	 * the bit is in, read it, OR in the new bit, then write
	 * back the new value.  The register is determined by the
	 * upper 7 bits of the hash value and the bit within that
	 * register are determined by the lower 5 bits of the value.
	 */
	reg = (crc32 >> 31) & 0x1;
	bit = (crc32 >> 26) & 0x1F;

	alx_mem_r32(hw, L1F_HASH_TBL0 + (reg<<2), &mta);
	mta |= (0x1 << bit);
	alx_mem_w32(hw, L1F_HASH_TBL0 + (reg<<2), mta);
	return 0;
}


static int alf_clear_mc_addr(struct alx_hw *hw)
{
	alx_mem_w32(hw, L1F_HASH_TBL0, 0);
	alx_mem_w32(hw, L1F_HASH_TBL1, 0);
	return 0;
}


/* RTX, IRQ */
static int alf_config_tx(struct alx_hw *hw)
{
	u32 wrr;

	alx_mem_r32(hw, L1F_WRR, &wrr);
	switch (hw->wrr_mode) {
	case alx_wrr_mode_none:
		FIELD_SETL(wrr, L1F_WRR_PRI, L1F_WRR_PRI_RESTRICT_NONE);
		break;
	case alx_wrr_mode_high:
		FIELD_SETL(wrr, L1F_WRR_PRI, L1F_WRR_PRI_RESTRICT_HI);
		break;
	case alx_wrr_mode_high2:
		FIELD_SETL(wrr, L1F_WRR_PRI, L1F_WRR_PRI_RESTRICT_HI2);
		break;
	case alx_wrr_mode_all:
		FIELD_SETL(wrr, L1F_WRR_PRI, L1F_WRR_PRI_RESTRICT_ALL);
		break;
	}
	FIELD_SETL(wrr, L1F_WRR_PRI0, hw->wrr_prio0);
	FIELD_SETL(wrr, L1F_WRR_PRI1, hw->wrr_prio1);
	FIELD_SETL(wrr, L1F_WRR_PRI2, hw->wrr_prio2);
	FIELD_SETL(wrr, L1F_WRR_PRI3, hw->wrr_prio3);
	alx_mem_w32(hw, L1F_WRR, wrr);
	return 0;
}


static int alf_config_msix(struct alx_hw *hw, u16 num_intrs,
			   bool msix_en, bool msi_en)
{
	u32 map[2];
	u32 type;
	int msix_idx;

	if (!msix_en)
		goto configure_legacy;

	memset(map, 0, sizeof(map));
	for (msix_idx = 0; msix_idx < num_intrs; msix_idx++) {
		switch (msix_idx) {
		case ALF_MSIX_INDEX_RXQ0:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_RXQ0,
				   ALF_MSIX_INDEX_RXQ0);
			break;
		case ALF_MSIX_INDEX_RXQ1:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_RXQ1,
				   ALF_MSIX_INDEX_RXQ1);
			break;
		case ALF_MSIX_INDEX_RXQ2:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_RXQ2,
				   ALF_MSIX_INDEX_RXQ2);
			break;
		case ALF_MSIX_INDEX_RXQ3:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_RXQ3,
				   ALF_MSIX_INDEX_RXQ3);
			break;
		case ALF_MSIX_INDEX_RXQ4:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_RXQ4,
				   ALF_MSIX_INDEX_RXQ4);
			break;
		case ALF_MSIX_INDEX_RXQ5:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_RXQ5,
				   ALF_MSIX_INDEX_RXQ5);
			break;
		case ALF_MSIX_INDEX_RXQ6:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_RXQ6,
				   ALF_MSIX_INDEX_RXQ6);
			break;
		case ALF_MSIX_INDEX_RXQ7:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_RXQ7,
				   ALF_MSIX_INDEX_RXQ7);
			break;
		case ALF_MSIX_INDEX_TXQ0:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_TXQ0,
				   ALF_MSIX_INDEX_TXQ0);
			break;
		case ALF_MSIX_INDEX_TXQ1:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_TXQ1,
				   ALF_MSIX_INDEX_TXQ1);
			break;
		case ALF_MSIX_INDEX_TXQ2:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_TXQ2,
				   ALF_MSIX_INDEX_TXQ2);
			break;
		case ALF_MSIX_INDEX_TXQ3:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_TXQ3,
				   ALF_MSIX_INDEX_TXQ3);
			break;
		case ALF_MSIX_INDEX_TIMER:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_TIMER,
				   ALF_MSIX_INDEX_TIMER);
			break;
		case ALF_MSIX_INDEX_ALERT:
			FIELD_SETL(map[0], L1F_MSI_MAP_TBL1_ALERT,
				   ALF_MSIX_INDEX_ALERT);
			break;
		case ALF_MSIX_INDEX_SMB:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_SMB,
				   ALF_MSIX_INDEX_SMB);
			break;
		case ALF_MSIX_INDEX_PHY:
			FIELD_SETL(map[1], L1F_MSI_MAP_TBL2_PHY,
				   ALF_MSIX_INDEX_PHY);
			break;
		default:
			break;

		}

	}

	alx_mem_w32(hw, L1F_MSI_MAP_TBL1, map[0]);
	alx_mem_w32(hw, L1F_MSI_MAP_TBL2, map[1]);

	/* 0 to alert, 1 to timer */
	type = (L1F_MSI_ID_MAP_DMAW |
		L1F_MSI_ID_MAP_DMAR |
		L1F_MSI_ID_MAP_PCIELNKDW |
		L1F_MSI_ID_MAP_PCIECERR |
		L1F_MSI_ID_MAP_PCIENFERR |
		L1F_MSI_ID_MAP_PCIEFERR |
		L1F_MSI_ID_MAP_PCIEUR);

	alx_mem_w32(hw, L1F_MSI_ID_MAP, type);
	return 0;

configure_legacy:
	alx_mem_w32(hw, L1F_MSI_MAP_TBL1, 0x0);
	alx_mem_w32(hw, L1F_MSI_MAP_TBL2, 0x0);
	alx_mem_w32(hw, L1F_MSI_ID_MAP, 0x0);
	if (msi_en) {
		u32 msi;
		alx_mem_r32(hw, 0x1920, &msi);
		msi |= 0x10000;
		alx_mem_w32(hw, 0x1920, msi);
	}
	return 0;
}


/*
 * Interrupt
 */
static int alf_ack_phy_intr(struct alx_hw *hw)
{
	u16 isr;
	return alf_read_phy_reg(hw, L1F_MII_ISR, &isr);
}


static int alf_enable_legacy_intr(struct alx_hw *hw)
{
	u16 cmd;

	alx_cfg_r16(hw, PCI_COMMAND, &cmd);
	cmd &= ~PCI_COMMAND_INTX_DISABLE;
	alx_cfg_w16(hw, PCI_COMMAND, cmd);

	alx_mem_w32(hw, L1F_ISR, ~((u32) L1F_ISR_DIS));
	alx_mem_w32(hw, L1F_IMR, hw->intr_mask);
	return 0;
}


static int alf_disable_legacy_intr(struct alx_hw *hw)
{
	alx_mem_w32(hw, L1F_ISR, L1F_ISR_DIS);
	alx_mem_w32(hw, L1F_IMR, 0);
	alx_mem_flush(hw);
	return 0;
}


static int alf_enable_msix_intr(struct alx_hw *hw, u8 entry_idx)
{
	u32 ctrl_reg;

	ctrl_reg = ALF_MSIX_ENTRY_BASE + (entry_idx * ALF_MSIX_ENTRY_SIZE) +
		   ALF_MSIX_MSG_CTRL_OFF;

	alx_mem_w32(hw, ctrl_reg, 0x0);
	alx_mem_flush(hw);
	return 0;
}


static int alf_disable_msix_intr(struct alx_hw *hw, u8 entry_idx)
{
	u32 ctrl_reg;

	ctrl_reg = ALF_MSIX_ENTRY_BASE + (entry_idx * ALF_MSIX_ENTRY_SIZE) +
		   ALF_MSIX_MSG_CTRL_OFF;

	alx_mem_w32(hw, ctrl_reg, 0x1);
	alx_mem_flush(hw);
	return 0;
}


/* RSS */
static int alf_config_rss(struct alx_hw *hw, bool rss_en)
{
	int key_len_by_u8 = sizeof(hw->rss_key);
	int idt_len_by_u32 = sizeof(hw->rss_idt) / sizeof(u32);
	u32 rxq0;
	int i;

	/* Fill out hash function keys */
	for (i = 0; i < key_len_by_u8; i++) {
		alx_mem_w8(hw, ALF_RSS_KEY(i, u8),
			   hw->rss_key[key_len_by_u8 - i - 1]);
	}

	/* Fill out redirection table */
	for (i = 0; i < idt_len_by_u32; i++)
		alx_mem_w32(hw, ALF_RSS_TBL(i, u32), hw->rss_idt[i]);

	alx_mem_w32(hw, L1F_RSS_BASE_CPU_NUM, hw->rss_base_cpu);

	alx_mem_r32(hw, L1F_RXQ0, &rxq0);
	if (hw->rss_hstype & ALX_RSS_HSTYP_IPV4_EN)
		rxq0 |=  L1F_RXQ0_RSS_HSTYP_IPV4_EN;
	else
		rxq0 &=  ~L1F_RXQ0_RSS_HSTYP_IPV4_EN;

	if (hw->rss_hstype & ALX_RSS_HSTYP_TCP4_EN)
		rxq0 |=  L1F_RXQ0_RSS_HSTYP_IPV4_TCP_EN;
	else
		rxq0 &=  ~L1F_RXQ0_RSS_HSTYP_IPV4_TCP_EN;

	if (hw->rss_hstype & ALX_RSS_HSTYP_IPV6_EN)
		rxq0 |=  L1F_RXQ0_RSS_HSTYP_IPV6_EN;
	else
		rxq0 &=  ~L1F_RXQ0_RSS_HSTYP_IPV6_EN;

	if (hw->rss_hstype & ALX_RSS_HSTYP_TCP6_EN)
		rxq0 |=  L1F_RXQ0_RSS_HSTYP_IPV6_TCP_EN;
	else
		rxq0 &=  ~L1F_RXQ0_RSS_HSTYP_IPV6_TCP_EN;

	FIELD_SETL(rxq0, L1F_RXQ0_RSS_MODE, hw->rss_mode);
	FIELD_SETL(rxq0, L1F_RXQ0_IDT_TBL_SIZE, hw->rss_idt_size);

	if (rss_en)
		rxq0 |= L1F_RXQ0_RSS_HASH_EN;
	else
		rxq0 &= ~L1F_RXQ0_RSS_HASH_EN;

	alx_mem_w32(hw, L1F_RXQ0, rxq0);
	return 0;
}


/* fc */
static int alf_get_fc_mode(struct alx_hw *hw, enum alx_fc_mode *mode)
{
	u16 bmsr, giga;
	int i;
	int retval = 0;

	for (i = 0; i < ALX_MAX_SETUP_LNK_CYCLE; i++) {
		alf_read_phy_reg(hw, MII_BMSR, &bmsr);
		alf_read_phy_reg(hw, MII_BMSR, &bmsr);
		if (bmsr & BMSR_LSTATUS) {
			/* Read phy Specific Status Register (17) */
			retval = alf_read_phy_reg(hw, L1F_MII_GIGA_PSSR, &giga);
			if (retval)
				return retval;

			if (!(giga & L1F_GIGA_PSSR_SPD_DPLX_RESOLVED)) {
				alx_hw_err(hw,
					"error for speed duplex resolved\n");
				return -EINVAL;
			}

			if ((giga & L1F_GIGA_PSSR_FC_TXEN) &&
			    (giga & L1F_GIGA_PSSR_FC_RXEN)) {
				*mode = alx_fc_full;
			} else if (giga & L1F_GIGA_PSSR_FC_TXEN) {
				*mode = alx_fc_tx_pause;
			} else if (giga & L1F_GIGA_PSSR_FC_RXEN) {
				*mode = alx_fc_rx_pause;
			} else {
				*mode = alx_fc_none;
			}
			break;
		}
		mdelay(100);
	}

	if (i == ALX_MAX_SETUP_LNK_CYCLE) {
		alx_hw_err(hw, "error when get flow control mode\n");
		retval = -EINVAL;
	}
	return retval;
}


static int alf_config_fc(struct alx_hw *hw)
{
	u32 mac;
	int retval = 0;

	if (hw->disable_fc_autoneg) {
		hw->fc_was_autonegged = false;
		hw->cur_fc_mode = hw->req_fc_mode;
	} else {
		hw->fc_was_autonegged = true;
		retval = alf_get_fc_mode(hw, &hw->cur_fc_mode);
		if (retval)
			return retval;
	}

	alx_mem_r32(hw, L1F_MAC_CTRL, &mac);

	switch (hw->cur_fc_mode) {
	case alx_fc_none: /* 0 */
		mac &= ~(L1F_MAC_CTRL_RXFC_EN | L1F_MAC_CTRL_TXFC_EN);
		break;
	case alx_fc_rx_pause: /* 1 */
		mac &= ~L1F_MAC_CTRL_TXFC_EN;
		mac |= L1F_MAC_CTRL_RXFC_EN;
		break;
	case alx_fc_tx_pause: /* 2 */
		mac |= L1F_MAC_CTRL_TXFC_EN;
		mac &= ~L1F_MAC_CTRL_RXFC_EN;
		break;
	case alx_fc_full: /* 3 */
	case alx_fc_default: /* 4 */
		mac |= (L1F_MAC_CTRL_TXFC_EN | L1F_MAC_CTRL_RXFC_EN);
		break;
	default:
		alx_hw_err(hw, "flow control param set incorrectly\n");
		return -EINVAL;
		break;
	}

	alx_mem_w32(hw, L1F_MAC_CTRL, mac);

	return retval;
}


/*
 * NVRam
 */
static int alf_check_nvram(struct alx_hw *hw, bool *exist)
{
	*exist = false;
	return 0;
}


/* ethtool */
static int alf_get_ethtool_regs(struct alx_hw *hw, void *buff)
{
	int i;
	u32 *val = buff;
	static const u32 reg[] = {
		/* 0 */
		L1F_DEV_CAP, L1F_DEV_CTRL, L1F_LNK_CAP, L1F_LNK_CTRL,
		L1F_UE_SVRT, L1F_EFLD, L1F_SLD, L1F_PPHY_MISC1,
		L1F_PPHY_MISC2, L1F_PDLL_TRNS1,

		/* 10 */
		L1F_TLEXTN_STATS, L1F_EFUSE_CTRL, L1F_EFUSE_DATA, L1F_SPI_OP1,
		L1F_SPI_OP2, L1F_SPI_OP3, L1F_EF_CTRL, L1F_EF_ADDR,
		L1F_EF_DATA, L1F_SPI_ID,

		/* 20 */
		L1F_SPI_CFG_START, L1F_PMCTRL, L1F_LTSSM_CTRL, L1F_MASTER,
		L1F_MANU_TIMER, L1F_IRQ_MODU_TIMER, L1F_PHY_CTRL, L1F_MAC_STS,
		L1F_MDIO, L1F_MDIO_EXTN,

		/* 30 */
		L1F_PHY_STS, L1F_BIST0, L1F_BIST1, L1F_SERDES,
		L1F_LED_CTRL, L1F_LED_PATN, L1F_LED_PATN2, L1F_SYSALV,
		L1F_PCIERR_INST, L1F_LPI_DECISN_TIMER,

		/* 40 */
		L1F_LPI_CTRL, L1F_LPI_WAIT, L1F_HRTBT_VLAN, L1F_HRTBT_CTRL,
		L1F_RXPARSE, L1F_MAC_CTRL, L1F_GAP, L1F_STAD1,
		L1F_LED_CTRL, L1F_HASH_TBL0,

		/* 50 */
		L1F_HASH_TBL1, L1F_HALFD, L1F_DMA, L1F_WOL0,
		L1F_WOL1, L1F_WOL2, L1F_WRR, L1F_HQTPD,
		L1F_CPUMAP1, L1F_CPUMAP2,

		/* 60 */
		L1F_MISC, L1F_RX_BASE_ADDR_HI, L1F_RFD_ADDR_LO, L1F_RFD_RING_SZ,
		L1F_RFD_BUF_SZ, L1F_RRD_ADDR_LO, L1F_RRD_RING_SZ,
		L1F_RFD_PIDX, L1F_RFD_CIDX, L1F_RXQ0,

		/* 70 */
		L1F_RXQ1, L1F_RXQ2, L1F_RXQ3, L1F_TX_BASE_ADDR_HI,
		L1F_TPD_PRI0_ADDR_LO, L1F_TPD_PRI1_ADDR_LO,
		L1F_TPD_PRI2_ADDR_LO, L1F_TPD_PRI3_ADDR_LO,
		L1F_TPD_PRI0_PIDX, L1F_TPD_PRI1_PIDX,

		/* 80 */
		L1F_TPD_PRI2_PIDX, L1F_TPD_PRI3_PIDX, L1F_TPD_PRI0_CIDX,
		L1F_TPD_PRI1_CIDX, L1F_TPD_PRI2_CIDX, L1F_TPD_PRI3_CIDX,
		L1F_TPD_RING_SZ, L1F_TXQ0, L1F_TXQ1, L1F_TXQ2,

		/* 90 */
		L1F_MSI_MAP_TBL1, L1F_MSI_MAP_TBL2, L1F_MSI_ID_MAP,
		L1F_MSIX_MASK, L1F_MSIX_PENDING,
	};

	for (i = 0; i < ARRAY_SIZE(reg); i++)
		alx_mem_r32(hw, reg[i], &val[i]);

	/* SRAM */
	for (i = 0; i < 16; i++)
		alx_mem_r32(hw, ALF_SRAM(i, u32), &val[100 + i]);

	/* RSS */
	for (i = 0; i < 10; i++)
		alx_mem_r32(hw, ALF_RSS_KEY(i, u32), &val[120 + i]);
	for (i = 0; i < 32; i++)
		alx_mem_r32(hw, ALF_RSS_TBL(i, u32), &val[130 + i]);
	alx_mem_r32(hw, L1F_RSS_HASH_VAL,     &val[162]);
	alx_mem_r32(hw, L1F_RSS_HASH_FLAG,    &val[163]);
	alx_mem_r32(hw, L1F_RSS_BASE_CPU_NUM, &val[164]);

	/* MIB */
	for (i = 0; i < 48; i++)
		alx_mem_r32(hw, ALF_MIB(i, u32), &val[170 + i]);
	return 0;
}


/******************************************************************************/
static int alf_set_hw_capabilities(struct alx_hw *hw)
{
	SET_HW_FLAG(L0S_CAP);
	SET_HW_FLAG(L1_CAP);

	if (hw->mac_type == alx_mac_l1f)
		SET_HW_FLAG(GIGA_CAP);

	/* set flags of alx_phy_info */
	SET_HW_FLAG(PWSAVE_CAP);
	return 0;
}


/* alc_set_hw_info */
static int alf_set_hw_infos(struct alx_hw *hw)
{
	hw->rxstat_reg = L1F_MIB_RX_OK;
	hw->rxstat_sz = 0x60;
	hw->txstat_reg = L1F_MIB_TX_OK;
	hw->txstat_sz = 0x68;

	hw->rx_prod_reg[0] = L1F_RFD_PIDX;
	hw->rx_cons_reg[0] = L1F_RFD_CIDX;

	hw->tx_prod_reg[0] = L1F_TPD_PRI0_PIDX;
	hw->tx_cons_reg[0] = L1F_TPD_PRI0_CIDX;
	hw->tx_prod_reg[1] = L1F_TPD_PRI1_PIDX;
	hw->tx_cons_reg[1] = L1F_TPD_PRI1_CIDX;
	hw->tx_prod_reg[2] = L1F_TPD_PRI2_PIDX;
	hw->tx_cons_reg[2] = L1F_TPD_PRI2_CIDX;
	hw->tx_prod_reg[3] = L1F_TPD_PRI3_PIDX;
	hw->tx_cons_reg[3] = L1F_TPD_PRI3_CIDX;

	hw->hwreg_sz = 0x200;
	hw->eeprom_sz = 0;

	return 0;
}


/*
 *  alf_init_hw_callbacks
 */
int alf_init_hw_callbacks(struct alx_hw *hw)
{
	/* NIC */
	hw->cbs.identify_nic   = &alf_identify_nic;
	/* MAC */
	hw->cbs.reset_mac      = &alf_reset_mac;
	hw->cbs.start_mac      = &alf_start_mac;
	hw->cbs.stop_mac       = &alf_stop_mac;
	hw->cbs.config_mac     = &alf_config_mac;
	hw->cbs.get_mac_addr   = &alf_get_mac_addr;
	hw->cbs.set_mac_addr   = &alf_set_mac_addr;
	hw->cbs.set_mc_addr    = &alf_set_mc_addr;
	hw->cbs.clear_mc_addr  = &alf_clear_mc_addr;

	/* PHY */
	hw->cbs.init_phy          = &alf_init_phy;
	hw->cbs.reset_phy         = &alf_reset_phy;
	hw->cbs.read_phy_reg      = &alf_read_phy_reg;
	hw->cbs.write_phy_reg     = &alf_write_phy_reg;
	hw->cbs.check_phy_link    = &alf_check_phy_link;
	hw->cbs.setup_phy_link    = &alf_setup_phy_link;
	hw->cbs.setup_phy_link_speed = &alf_setup_phy_link_speed;

	/* Interrupt */
	hw->cbs.ack_phy_intr		= &alf_ack_phy_intr;
	hw->cbs.enable_legacy_intr	= &alf_enable_legacy_intr;
	hw->cbs.disable_legacy_intr	= &alf_disable_legacy_intr;
	hw->cbs.enable_msix_intr	= &alf_enable_msix_intr;
	hw->cbs.disable_msix_intr	= &alf_disable_msix_intr;

	/* Configure */
	hw->cbs.config_tx	= &alf_config_tx;
	hw->cbs.config_fc	= &alf_config_fc;
	hw->cbs.config_rss	= &alf_config_rss;
	hw->cbs.config_msix	= &alf_config_msix;
	hw->cbs.config_wol	= &alf_config_wol;
	hw->cbs.config_aspm	= &alf_config_aspm;
	hw->cbs.config_mac_ctrl	= &alf_config_mac_ctrl;
	hw->cbs.config_pow_save	= &alf_config_pow_save;
	hw->cbs.reset_pcie	= &alf_reset_pcie;

	/* NVRam */
	hw->cbs.check_nvram	= &alf_check_nvram;

	/* Others */
	hw->cbs.get_ethtool_regs = alf_get_ethtool_regs;

	alf_set_hw_capabilities(hw);
	alf_set_hw_infos(hw);

	alx_hw_info(hw, "HW Flags = 0x%x\n", hw->flags);
	return 0;
}

