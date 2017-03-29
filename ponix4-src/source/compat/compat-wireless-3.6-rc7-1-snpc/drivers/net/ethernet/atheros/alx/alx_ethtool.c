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

#include <linux/netdevice.h>
#include <linux/ethtool.h>
#include <linux/slab.h>

#include "alx.h"
#include "alx_hwcom.h"

#ifdef ETHTOOL_OPS_COMPAT
#include "alx_compat_ethtool.c"
#endif


static int alx_get_settings(struct net_device *netdev,
			    struct ethtool_cmd *ecmd)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	u32 link_speed = hw->link_speed;
	bool link_up = hw->link_up;

	ecmd->supported = (SUPPORTED_10baseT_Half  |
			   SUPPORTED_10baseT_Full  |
			   SUPPORTED_100baseT_Half |
			   SUPPORTED_100baseT_Full |
			   SUPPORTED_Autoneg       |
			   SUPPORTED_TP);
	if (CHK_HW_FLAG(GIGA_CAP))
		ecmd->supported |= SUPPORTED_1000baseT_Full;

	ecmd->advertising = ADVERTISED_TP;

	ecmd->advertising |= ADVERTISED_Autoneg;
	ecmd->advertising |= hw->autoneg_advertised;

	ecmd->port = PORT_TP;
	ecmd->phy_address = 0;
	ecmd->autoneg = AUTONEG_ENABLE;
	ecmd->transceiver = XCVR_INTERNAL;

	if (!in_interrupt()) {
		hw->cbs.check_phy_link(hw, &link_speed, &link_up);
		hw->link_speed = link_speed;
		hw->link_up = link_up;
	}

	if (link_up) {
		switch (link_speed) {
		case ALX_LINK_SPEED_10_HALF:
			ethtool_cmd_speed_set(ecmd, SPEED_10);
			ecmd->duplex = DUPLEX_HALF;
			break;
		case ALX_LINK_SPEED_10_FULL:
			ethtool_cmd_speed_set(ecmd, SPEED_10);
			ecmd->duplex = DUPLEX_FULL;
			break;
		case ALX_LINK_SPEED_100_HALF:
			ethtool_cmd_speed_set(ecmd, SPEED_100);
			ecmd->duplex = DUPLEX_HALF;
			break;
		case ALX_LINK_SPEED_100_FULL:
			ethtool_cmd_speed_set(ecmd, SPEED_100);
			ecmd->duplex = DUPLEX_FULL;
			break;
		case ALX_LINK_SPEED_1GB_FULL:
			ethtool_cmd_speed_set(ecmd, SPEED_1000);
			ecmd->duplex = DUPLEX_FULL;
			break;
		default:
			ecmd->speed = -1;
			ecmd->duplex = -1;
			break;
		}
	} else {
		ethtool_cmd_speed_set(ecmd, -1);
		ecmd->duplex = -1;
	}

	return 0;
}


static int alx_set_settings(struct net_device *netdev,
			    struct ethtool_cmd *ecmd)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	u32 advertised, old;
	int error = 0;

	while (CHK_ADPT_FLAG(1, STATE_RESETTING))
		msleep(20);
	SET_ADPT_FLAG(1, STATE_RESETTING);

	old = hw->autoneg_advertised;
	advertised = 0;
	if (ecmd->autoneg == AUTONEG_ENABLE) {
		advertised = ALX_LINK_SPEED_DEFAULT;
	} else {
		u32 speed = ethtool_cmd_speed(ecmd);
		if (speed == SPEED_1000) {
			if (ecmd->duplex != DUPLEX_FULL) {
				dev_warn(&adpt->pdev->dev,
					 "1000M half is invalid\n");
				CLI_ADPT_FLAG(1, STATE_RESETTING);
				return -EINVAL;
			}
			advertised = ALX_LINK_SPEED_1GB_FULL;
		} else if (speed == SPEED_100) {
			if (ecmd->duplex == DUPLEX_FULL)
				advertised = ALX_LINK_SPEED_100_FULL;
			else
				advertised = ALX_LINK_SPEED_100_HALF;
		} else {
			if (ecmd->duplex == DUPLEX_FULL)
				advertised = ALX_LINK_SPEED_10_FULL;
			else
				advertised = ALX_LINK_SPEED_10_HALF;
		}
	}

	if (hw->autoneg_advertised == advertised) {
		CLI_ADPT_FLAG(1, STATE_RESETTING);
		return error;
	}

	error = hw->cbs.setup_phy_link_speed(hw, advertised, true,
			!hw->disable_fc_autoneg);
	if (error) {
		dev_err(&adpt->pdev->dev,
			"setup link failed with code %d\n", error);
		hw->cbs.setup_phy_link_speed(hw, old, true,
				!hw->disable_fc_autoneg);
	}
	CLI_ADPT_FLAG(1, STATE_RESETTING);
	return error;
}


static void alx_get_pauseparam(struct net_device *netdev,
			       struct ethtool_pauseparam *pause)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;


	if (hw->disable_fc_autoneg ||
	    hw->cur_fc_mode == alx_fc_none)
		pause->autoneg = 0;
	else
		pause->autoneg = 1;

	if (hw->cur_fc_mode == alx_fc_rx_pause) {
		pause->rx_pause = 1;
	} else if (hw->cur_fc_mode == alx_fc_tx_pause) {
		pause->tx_pause = 1;
	} else if (hw->cur_fc_mode == alx_fc_full) {
		pause->rx_pause = 1;
		pause->tx_pause = 1;
	}
}


static int alx_set_pauseparam(struct net_device *netdev,
			      struct ethtool_pauseparam *pause)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	enum alx_fc_mode req_fc_mode;
	bool disable_fc_autoneg;
	int retval;

	while (CHK_ADPT_FLAG(1, STATE_RESETTING))
		msleep(20);
	SET_ADPT_FLAG(1, STATE_RESETTING);

	req_fc_mode        = hw->req_fc_mode;
	disable_fc_autoneg = hw->disable_fc_autoneg;


	if (pause->autoneg != AUTONEG_ENABLE)
		disable_fc_autoneg = true;
	else
		disable_fc_autoneg = false;

	if ((pause->rx_pause && pause->tx_pause) || pause->autoneg)
		req_fc_mode = alx_fc_full;
	else if (pause->rx_pause && !pause->tx_pause)
		req_fc_mode = alx_fc_rx_pause;
	else if (!pause->rx_pause && pause->tx_pause)
		req_fc_mode = alx_fc_tx_pause;
	else if (!pause->rx_pause && !pause->tx_pause)
		req_fc_mode = alx_fc_none;
	else
		return -EINVAL;

	if ((hw->req_fc_mode != req_fc_mode) ||
	    (hw->disable_fc_autoneg != disable_fc_autoneg)) {
		hw->req_fc_mode = req_fc_mode;
		hw->disable_fc_autoneg = disable_fc_autoneg;
		if (!hw->disable_fc_autoneg)
			retval = hw->cbs.setup_phy_link(hw,
				hw->autoneg_advertised, true, true);

		if (hw->cbs.config_fc)
			hw->cbs.config_fc(hw);
	}

	CLI_ADPT_FLAG(1, STATE_RESETTING);
	return 0;
}


static u32 alx_get_msglevel(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	return adpt->msg_enable;
}


static void alx_set_msglevel(struct net_device *netdev, u32 data)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	adpt->msg_enable = data;
}


static int alx_get_regs_len(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	return hw->hwreg_sz * sizeof(32);
}


static void alx_get_regs(struct net_device *netdev,
			 struct ethtool_regs *regs, void *buff)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;

	regs->version = 0;

	memset(buff, 0, hw->hwreg_sz * sizeof(u32));
	if (hw->cbs.get_ethtool_regs)
		hw->cbs.get_ethtool_regs(hw, buff);
}


static int alx_get_eeprom_len(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	return hw->eeprom_sz;
}


static int alx_get_eeprom(struct net_device *netdev,
			  struct ethtool_eeprom *eeprom, u8 *bytes)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	bool eeprom_exist = false;
	u32 *eeprom_buff;
	int first_dword, last_dword;
	int retval = 0;
	int i;

	if (eeprom->len == 0)
		return -EINVAL;

	if (hw->cbs.check_nvram)
		hw->cbs.check_nvram(hw, &eeprom_exist);
	if (!eeprom_exist)
		return -EOPNOTSUPP;

	eeprom->magic = adpt->pdev->vendor |
			(adpt->pdev->device << 16);

	first_dword = eeprom->offset >> 2;
	last_dword = (eeprom->offset + eeprom->len - 1) >> 2;

	eeprom_buff = kmalloc(sizeof(u32) *
			(last_dword - first_dword + 1), GFP_KERNEL);
	if (eeprom_buff == NULL)
		return -ENOMEM;

	for (i = first_dword; i < last_dword; i++) {
		if (hw->cbs.read_nvram) {
			retval = hw->cbs.read_nvram(hw, i*4,
					&(eeprom_buff[i-first_dword]));
			if (retval) {
				retval =  -EIO;
				goto out;
			}
		}
	}

	/* Device's eeprom is always little-endian, word addressable */
	for (i = 0; i < last_dword - first_dword; i++)
		le32_to_cpus(&eeprom_buff[i]);

	memcpy(bytes, (u8 *)eeprom_buff + (eeprom->offset & 3), eeprom->len);
out:
	kfree(eeprom_buff);
	return retval;
}


static int alx_set_eeprom(struct net_device *netdev,
			  struct ethtool_eeprom *eeprom, u8 *bytes)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	struct alx_hw *hw = &adpt->hw;
	bool eeprom_exist = false;
	u32 *eeprom_buff;
	u32 *ptr;
	int first_dword, last_dword;
	int retval = 0;
	int i;

	if (eeprom->len == 0)
		return -EINVAL;

	if (hw->cbs.check_nvram)
		hw->cbs.check_nvram(hw, &eeprom_exist);
	if (!eeprom_exist)
		return -EOPNOTSUPP;


	if (eeprom->magic != (adpt->pdev->vendor |
				(adpt->pdev->device << 16)))
		return -EINVAL;

	first_dword = eeprom->offset >> 2;
	last_dword = (eeprom->offset + eeprom->len - 1) >> 2;
	eeprom_buff = kmalloc(ALX_MAX_EEPROM_LEN, GFP_KERNEL);
	if (eeprom_buff == NULL)
		return -ENOMEM;

	ptr = (u32 *)eeprom_buff;

	if (eeprom->offset & 3) {
		/* need read/modify/write of first changed EEPROM word */
		/* only the second byte of the word is being modified */
		if (hw->cbs.read_nvram) {
			retval = hw->cbs.read_nvram(hw, first_dword * 4,
						&(eeprom_buff[0]));
			if (retval) {
				retval = -EIO;
				goto out;
			}
		}
		ptr++;
	}

	if (((eeprom->offset + eeprom->len) & 3)) {
		/* need read/modify/write of last changed EEPROM word */
		/* only the first byte of the word is being modified */
		if (hw->cbs.read_nvram) {
			retval = hw->cbs.read_nvram(hw, last_dword * 4,
				&(eeprom_buff[last_dword - first_dword]));
			if (retval) {
				retval = -EIO;
				goto out;
			}
		}
	}

	/* Device's eeprom is always little-endian, word addressable */
	memcpy(ptr, bytes, eeprom->len);
	for (i = 0; i < last_dword - first_dword + 1; i++) {
		if (hw->cbs.write_nvram) {
			retval = hw->cbs.write_nvram(hw, (first_dword + i) * 4,
						eeprom_buff[i]);
			if (retval) {
				retval = -EIO;
				goto out;
			}
		}
	}
out:
	kfree(eeprom_buff);
	return retval;
}


static void alx_get_drvinfo(struct net_device *netdev,
			    struct ethtool_drvinfo *drvinfo)
{
	struct alx_adapter *adpt = netdev_priv(netdev);

	strlcpy(drvinfo->driver,  alx_drv_name, sizeof(drvinfo->driver));
	strlcpy(drvinfo->fw_version, "alx", 32);
	strlcpy(drvinfo->bus_info, pci_name(adpt->pdev),
		sizeof(drvinfo->bus_info));
	drvinfo->n_stats = 0;
	drvinfo->testinfo_len = 0;
	drvinfo->regdump_len = adpt->hw.hwreg_sz;
	drvinfo->eedump_len = adpt->hw.eeprom_sz;
}


static int alx_wol_exclusion(struct alx_adapter *adpt,
			     struct ethtool_wolinfo *wol)
{
	struct alx_hw *hw = &adpt->hw;
	int retval = 1;

	/* WOL not supported except for the following */
	switch (hw->pci_devid) {
	case ALX_DEV_ID_AR8131:
	case ALX_DEV_ID_AR8132:
	case ALX_DEV_ID_AR8151_V1:
	case ALX_DEV_ID_AR8151_V2:
	case ALX_DEV_ID_AR8152_V1:
	case ALX_DEV_ID_AR8152_V2:
	case ALX_DEV_ID_AR8161:
	case ALX_DEV_ID_AR8162:
		retval = 0;
		break;
	default:
		wol->supported = 0;
	}

	return retval;
}


static void alx_get_wol(struct net_device *netdev,
			struct ethtool_wolinfo *wol)
{
	struct alx_adapter *adpt = netdev_priv(netdev);

	wol->supported = WAKE_MAGIC | WAKE_PHY;
	wol->wolopts = 0;

	if (adpt->wol & ALX_WOL_MAGIC)
		wol->wolopts |= WAKE_MAGIC;
	if (adpt->wol & ALX_WOL_PHY)
		wol->wolopts |= WAKE_PHY;

	netif_info(adpt, wol, adpt->netdev,
		   "wol->wolopts = %x\n", wol->wolopts);
}


static int alx_set_wol(struct net_device *netdev, struct ethtool_wolinfo *wol)
{
	struct alx_adapter *adpt = netdev_priv(netdev);

	if (wol->wolopts & (WAKE_ARP | WAKE_MAGICSECURE |
			    WAKE_UCAST | WAKE_BCAST | WAKE_MCAST))
		return -EOPNOTSUPP;

	if (alx_wol_exclusion(adpt, wol))
		return wol->wolopts ? -EOPNOTSUPP : 0;

	adpt->wol = 0;

	if (wol->wolopts & WAKE_MAGIC)
		adpt->wol |= ALX_WOL_MAGIC;
	if (wol->wolopts & WAKE_PHY)
		adpt->wol |= ALX_WOL_PHY;

	device_set_wakeup_enable(&adpt->pdev->dev, adpt->wol);

	return 0;
}


static int alx_nway_reset(struct net_device *netdev)
{
	struct alx_adapter *adpt = netdev_priv(netdev);
	if (netif_running(netdev))
		alx_reinit_locked(adpt);
	return 0;
}


static const struct ethtool_ops alx_ethtool_ops = {
	.get_settings    = alx_get_settings,
	.set_settings    = alx_set_settings,
	.get_pauseparam  = alx_get_pauseparam,
	.set_pauseparam  = alx_set_pauseparam,
	.get_drvinfo     = alx_get_drvinfo,
	.get_regs_len    = alx_get_regs_len,
	.get_regs        = alx_get_regs,
	.get_wol         = alx_get_wol,
	.set_wol         = alx_set_wol,
	.get_msglevel    = alx_get_msglevel,
	.set_msglevel    = alx_set_msglevel,
	.nway_reset      = alx_nway_reset,
	.get_link        = ethtool_op_get_link,
	.get_eeprom_len  = alx_get_eeprom_len,
	.get_eeprom      = alx_get_eeprom,
	.set_eeprom      = alx_set_eeprom,
};


void alx_set_ethtool_ops(struct net_device *netdev)
{
	SET_ETHTOOL_OPS(netdev, &alx_ethtool_ops);
}
