/* Copyright (C) 2010 - 2012 Broadcom Corporation.
 * Portions Copyright (C) VMware, Inc. 2007-2011. All Rights Reserved.
 */

#define TG3_NETQ_WAIT_EVENT_TIMEOUT	msecs_to_jiffies(100)
#define TG3_VRQ_CHANGE_TIMEOUT_US	10000
#define TG3_VRQ_FLUSH_TIMEOUT_US	50000
#define TG3_VRQ_MAX_NUM_TX_QS(tp) \
	(tg3_flag((tp), ENABLE_TSS) ? (tp)->irq_cnt - 2 : 0)
#define TG3_VRQ_MAX_NUM_RX_QS(tp)	(tp->irq_cnt - 1)

static void tg3_vmware_timer(struct tg3 *tp)
{
	/*
	 * If the driver says this NIC is in promiscuous mode, the hardware
	 * better agree with it. See PR 88299 for more details, but there
	 * are cases (eg. bcm5704 rev10) that the set promiscuous bit is
	 * lost for no reason and this seems to be the least intrusive
	 * workaround for that issue (likely a hardware bug).
	 *
	 * This is happening on other cards as well (BCM5700 rev04) PR 97750
	 * Also, on some cards (5703) we have seen other bits getting messed up
	 */
	if (netif_carrier_ok(tp->dev)) {
		u32 rx_mode = tr32(MAC_RX_MODE);
		if (!(rx_mode & RX_MODE_PROMISC) && (rx_mode != tp->rx_mode)) {
			/*
			 * We love to warn the users every time there is such a
			 * register reset, but we do not want to do it forever
			 * given some NICs are really that bad (PR 105488).
			 */
			if (tp->vmware.rx_mode_reset_counter < 200) {
				netdev_info(tp->dev, "%s: rx_mode "
					    "0x%x(%s)=>0x%x",
					    tp->dev->name, rx_mode,
				       rx_mode & RX_MODE_PROMISC ? "on" : "off",
					    tp->rx_mode);
				tp->vmware.rx_mode_reset_counter++;
			}
			tw32_f(MAC_RX_MODE, tp->rx_mode);
		}
	}

	/*
	 * we've seen ssome 5703's (rev 10) which don't seen to
	 * generate an interrupt on link-up state changes. bug 89197.
	 */
	if (!netif_carrier_ok(tp->dev) &&
	    !(tg3_flag(tp, USE_LINKCHG_REG)) &&
	    !(tg3_flag(tp, POLL_SERDES))) {
		struct tg3_hw_status *sblk = tp->napi[0].hw_status;
		if (sblk->status & SD_STATUS_LINK_CHG) {
			sblk->status = SD_STATUS_UPDATED |
			       (sblk->status & ~SD_STATUS_LINK_CHG);
			tg3_setup_phy(tp, 0);
		}
	}
}

static netdev_features_t tg3_vmware_tune_tso(struct tg3 *tp,
					     netdev_features_t features)
{
	/* VMWare does not have skb_gso_segment() to workaround TSO_BUG */
	if (tg3_flag(tp, TSO_BUG))
		features &= ~(NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_TSO_ECN);

#if defined(TG3_INBOX)
	/*
	 * VMWare does not see significant performance
	 * increases with TSO enabled.
	 */
	features &= ~(NETIF_F_TSO | NETIF_F_TSO6 | NETIF_F_TSO_ECN);
	tg3_flag_clear(tp, TSO_CAPABLE);
	tg3_flag_clear(tp, TSO_BUG);
#endif /* TG3_INBOX */

	return features;
}

/* The following debug buffers and exported routines are used by GDB to access
 * * tg3 hardware registers when doing live debug over serial port. */
#define DBG_BUF_SZ  128

static u32 tg3_dbg_buf[DBG_BUF_SZ];

void tg3_dbg_read32(struct net_device *dev, u32 off, u32 len)
{
	struct tg3 *tp = netdev_priv(dev);
	u32 *buf = tg3_dbg_buf;

	memset(tg3_dbg_buf, 0, sizeof(tg3_dbg_buf));

	if (off & 0x3) {
		len += off & 0x3;
		off &= ~0x3;
	}

	if (off >= TG3_REG_BLK_SIZE)
		return;

	if (len & 0x3)
		len = (len + 3) & ~3;

	if (len > DBG_BUF_SZ)
		len = DBG_BUF_SZ;

	if (off + len > TG3_REG_BLK_SIZE)
		len = TG3_REG_BLK_SIZE - off;

	while (len > 0) {
		*buf = tr32(off);
		buf++;
		off += 4;
		len -= 4;
	}
}
EXPORT_SYMBOL(tg3_dbg_read32);

void tg3_dbg_write32(struct net_device *dev, u32 off, u32 val)
{
	struct tg3 *tp = netdev_priv(dev);

	if (off & 0x3)
		return;

	tw32(off, val);
}
EXPORT_SYMBOL(tg3_dbg_write32);

#if !defined(TG3_VMWARE_BMAPILNX_DISABLE)
static int
tg3_vmware_ioctl_cim(struct net_device *dev, struct ifreq *ifr)
{
	struct tg3 *tp = netdev_priv(dev);
	void __user *useraddr = ifr->ifr_data;
	struct brcm_vmware_ioctl_req req;
	int rc = 0;
	u32 val;

	if (copy_from_user(&req, useraddr, sizeof(req))) {
		netdev_err(dev, "%s: could not copy "
				"from user tg3_ioctl_req\n", __func__);
		return -EFAULT;
	}

	switch (req.cmd) {
	case BRCM_VMWARE_CIM_CMD_ENABLE_NIC:
		netdev_info(dev, "%s: enable NIC\n", __func__);

		rc = tg3_open(tp->dev);
		break;
	case BRCM_VMWARE_CIM_CMD_DISABLE_NIC:
		netdev_info(dev, "%s: disable NIC\n", __func__);

		rc = tg3_close(tp->dev);
		break;
	case BRCM_VMWARE_CIM_CMD_REG_READ: {
		struct brcm_vmware_ioctl_reg_read_req *rd_req;

		rd_req = &req.cmd_req.reg_read_req;
		if (0x7ffc < rd_req->reg_offset) {
			netdev_err(dev, "%s: %s: "
					"out of range: req reg: 0x%x\n",
				   __func__, "reg read", rd_req->reg_offset);
			rc = -EINVAL;
			break;
		}

		if (rd_req->reg_offset & 0x3) {
			netdev_err(dev, "%s: %s: "
				   "offset not dword aligned: req reg: 0x%x\n",
				   __func__, "reg read", rd_req->reg_offset);
			rc = -EINVAL;
			break;
		}

		switch (rd_req->reg_access_type) {
		case BRCM_VMWARE_REG_ACCESS_DIRECT:
			val = tr32(rd_req->reg_offset);
			break;
		case BRCM_VMWARE_REG_ACCESS_PCI_CFG:
			pci_read_config_dword(tp->pdev,
					      rd_req->reg_offset, &val);
			break;
		default:
			netdev_err(dev, "%s: %s: "
				  "invalid access method: access type: 0x%x\n",
				   __func__, "reg read",
				   rd_req->reg_access_type);
			rc = -EINVAL;
			break;
		}

		req.cmd_req.reg_read_req.reg_value = val;
		netdev_info(dev, "%s: %s: reg: 0x%x value:0x%x",
			    __func__, "reg read", rd_req->reg_offset,
			    rd_req->reg_value);

		break;
	} case BRCM_VMWARE_CIM_CMD_REG_WRITE: {
		struct brcm_vmware_ioctl_reg_write_req *wr_req;

		wr_req = &req.cmd_req.reg_write_req;
		if (0x7ffc < wr_req->reg_offset) {
			netdev_err(dev, "%s: %s: "
					"out of range: req reg: 0x%x\n",
				   __func__, "reg write", wr_req->reg_offset);
			rc = -EINVAL;
			break;
		}

		if (wr_req->reg_offset & 0x3) {
			netdev_err(dev, "%s: %s: "
				   "offset not dword aligned: req reg: 0x%x\n",
				   __func__, "reg write", wr_req->reg_offset);
			rc = -EINVAL;
			break;
		}

		switch (wr_req->reg_access_type) {
		case BRCM_VMWARE_REG_ACCESS_DIRECT:
			tw32(wr_req->reg_offset, wr_req->reg_value);
			break;
		case BRCM_VMWARE_REG_ACCESS_PCI_CFG:
			pci_write_config_dword(tp->pdev, wr_req->reg_offset,
					       wr_req->reg_value);
			break;
		default:
			netdev_err(dev, "%s: %s: "
				  "invalid access method: access type: 0x%x\n",
				   __func__, "reg write",
				   wr_req->reg_access_type);
			rc = -EINVAL;
			break;
		}

		netdev_info(dev, "%s: %s: reg: 0x%x value:0x%x",
			    __func__, "reg write", wr_req->reg_offset,
			    wr_req->reg_value);

		break;
	} case BRCM_VMWARE_CIM_CMD_GET_NIC_PARAM:
		netdev_info(dev, "%s: get NIC param\n", __func__);

		req.cmd_req.get_nic_param_req.mtu = dev->mtu;
		memcpy(req.cmd_req.get_nic_param_req.current_mac_addr,
		       dev->dev_addr,
		       sizeof(req.cmd_req.get_nic_param_req.current_mac_addr));
		break;
	case BRCM_VMWARE_CIM_CMD_GET_NIC_STATUS:
		netdev_info(dev, "%s: get NIC status\n", __func__);

		req.cmd_req.get_nic_status_req.nic_status = netif_running(dev);
		break;
	default:
		netdev_err(dev, "%s: unknown req.cmd: 0x%x\n",
			   __func__, req.cmd);
		rc = -EINVAL;
	}

	if (rc == 0 &&
	    copy_to_user(useraddr, &req, sizeof(req))) {
		netdev_err(dev, "%s: couldn't copy to user tg3_ioctl_req\n",
			   __func__);
		return -EFAULT;
	}

	return rc;
}
#endif  /* TG3_VMWARE_BMAPILNX */

#ifdef TG3_VMWARE_NETQ_ENABLE
static void tg3_vmware_fetch_stats(struct tg3 *tp)
{
	int i;
	u32 addr;
	struct tg3_napi *tnapi = &tp->napi[0];

	tnapi->netq.stats.rx_bytes_hw   += tr32(0x9d0);
	tnapi->netq.stats.rx_errors_hw  += tr32(0x9d4);
	tnapi->netq.stats.rx_packets_hw += tr32(0x9d8);

	for (i = 1, addr = 0xa00; i < 16; i++, addr += 0x20) {
		tnapi = &tp->napi[i];

		tnapi->netq.stats.tx_bytes         += tr32(addr + 0x00);
		tnapi->netq.stats.tx_ucast_packets += tr32(addr + 0x04);
		tnapi->netq.stats.tx_mcast_packets += tr32(addr + 0x08);
		tnapi->netq.stats.tx_bcast_packets += tr32(addr + 0x0c);

		tnapi->netq.stats.rx_bytes_hw   += tr32(addr + 0x10);
		tnapi->netq.stats.rx_errors_hw  += tr32(addr + 0x14);
		tnapi->netq.stats.rx_packets_hw += tr32(addr + 0x18);
	}
}

static void tg3_set_prod_bdinfo(struct tg3 *tp, u32 bdinfo_addr,
				dma_addr_t mapping, u32 maxlen_flags)
{
	tw32(bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_HIGH,
	     ((u64) mapping >> 32));
	tw32(bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_LOW,
	     ((u64) mapping & 0xffffffff));
	tw32(bdinfo_addr + TG3_BDINFO_MAXLEN_FLAGS, maxlen_flags);
	/* Leave the nic addr field alone */
}

static void tg3_rx_prod_rcb_disable(struct tg3 *tp, u32 bdinfo_addr)
{
	tw32(bdinfo_addr + TG3_BDINFO_MAXLEN_FLAGS, BDINFO_FLAGS_DISABLED);
	tw32(bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_HIGH, 0);
	tw32(bdinfo_addr + TG3_BDINFO_HOST_ADDR + TG3_64BIT_REG_LOW, 0);
}

static void tg3_disable_prod_rcbs(struct tg3 *tp, u32 ring)
{
	struct tg3_rx_prodring_set *tpr;
	u32 offset;

	if (!(tg3_flag(tp, IOV_CAPABLE)))
		return;

	tpr = &tp->napi[ring].prodring;
	offset = RCVDBDI_JMB_BD_RING1 + (ring - 1) * 2 * TG3_BDINFO_SIZE;

	/* Disable the jumbo ring */
	tg3_rx_prod_rcb_disable(tp, offset);
	offset += TG3_BDINFO_SIZE;

	/* Disable the standard ring */
	tg3_rx_prod_rcb_disable(tp, offset);
}

static void tg3_setup_prod_rcbs(struct tg3 *tp, u32 ring)
{
	struct tg3_rx_prodring_set *tpr;
	u32 offset;

	if (!(tg3_flag(tp, ENABLE_IOV)))
		return;

	tpr = &tp->napi[ring].prodring;
	offset = RCVDBDI_JMB_BD_RING1 + (ring - 1) * 2 * TG3_BDINFO_SIZE;

	if (tg3_flag(tp, JUMBO_RING_ENABLE)) {
		tg3_set_prod_bdinfo(tp, offset, tpr->rx_jmb_mapping,
		       (TG3_RX_JMB_MAX_SIZE_5717 << BDINFO_FLAGS_MAXLEN_SHIFT) |
			      BDINFO_FLAGS_USE_EXT_RECV);
		tpr->rx_jmb_prod_idx = tp->rx_jumbo_pending;
		tw32_rx_mbox(tpr->rx_jmb_mbox, tpr->rx_jmb_prod_idx);
	}

	offset += TG3_BDINFO_SIZE;

	tg3_set_prod_bdinfo(tp, offset, tpr->rx_std_mapping,
	(TG3_RX_STD_MAX_SIZE_5717 << BDINFO_FLAGS_MAXLEN_SHIFT) |
		(TG3_RX_STD_DMA_SZ << 2));
	tpr->rx_std_prod_idx = tp->rx_pending;
	tw32_rx_mbox(tpr->rx_std_mbox, tpr->rx_std_prod_idx);
}

static void tg3_setup_prod_mboxes(struct tg3 *tp, u32 ring)
{
	struct tg3_rx_prodring_set *tpr = &tp->napi[ring].prodring;

	if (!ring) {
		tpr->rx_std_mbox = TG3_RX_STD_PROD_IDX_REG;
		tpr->rx_jmb_mbox = TG3_RX_JMB_PROD_IDX_REG;
		return;
	}

	tpr->rx_std_mbox = MAILBOX_RCV_STD_PROD_IDX_RING1 + (ring - 1) * 4;
	if (ring % 2)
		tpr->rx_std_mbox -= 4;
	else
		tpr->rx_std_mbox += 4;

	if (ring < 12)
		tpr->rx_jmb_mbox = MAILBOX_RCV_JUMBO_PROD_IDX_RING1 +
				   (ring - 1) * 4;
	else
		tpr->rx_jmb_mbox = MAILBOX_RCV_JMB_PROD_IDX_RING12 +
				   (ring - 12) * 4;

	if (ring % 2)
		tpr->rx_jmb_mbox -= 4;
	else
		tpr->rx_jmb_mbox += 4;
}

static int
tg3_netq_get_netqueue_features(vmknetddi_queueop_get_features_args_t *args)
{
	struct tg3 *tp = netdev_priv(args->netdev);

	args->features = VMKNETDDI_QUEUEOPS_FEATURE_RXQUEUES;
	if (tg3_flag(tp, ENABLE_TSS))
		args->features |= VMKNETDDI_QUEUEOPS_FEATURE_TXQUEUES;
	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_get_queue_count(vmknetddi_queueop_get_queue_count_args_t *args)
{
	struct tg3 *tp = netdev_priv(args->netdev);

	if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_RX) {
		args->count = TG3_VRQ_MAX_NUM_RX_QS(tp);

		netdev_info(tp->dev, "Using %d RX NetQ rings\n", args->count);

		return VMKNETDDI_QUEUEOPS_OK;
	} else if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_TX) {
		args->count = TG3_VRQ_MAX_NUM_TX_QS(tp);

		netdev_info(tp->dev, "Using %d TX NetQ rings\n", args->count);

		return VMKNETDDI_QUEUEOPS_OK;
	} else {
		netdev_err(tp->dev, "Counting queue: invalid queue type\n");

		return VMKNETDDI_QUEUEOPS_ERR;
	}
}

static int
tg3_netq_get_filter_count(vmknetddi_queueop_get_filter_count_args_t *args)
{
	/* Only support 1 Mac filter per queue */
	args->count = 1;
	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_alloc_tx_queue(struct net_device *netdev,
			 vmknetddi_queueops_queueid_t *p_qid,
			 u16 *queue_mapping)
{
	struct tg3 *tp = netdev_priv(netdev);
	int i;

	if (tp->vmware.netq.n_tx_queues_allocated >= TG3_VRQ_MAX_NUM_TX_QS(tp))
		return VMKNETDDI_QUEUEOPS_ERR;

	for (i = 1; i < 1 + TG3_VRQ_MAX_NUM_TX_QS(tp); i++) {
		struct tg3_napi *tnapi = &tp->napi[i + 1];
		if (!(tnapi->netq.flags & TG3_NETQ_TXQ_ALLOCATED)) {
			tnapi->netq.flags |= TG3_NETQ_TXQ_ALLOCATED;
			tp->vmware.netq.n_tx_queues_allocated++;
			*p_qid = VMKNETDDI_QUEUEOPS_MK_TX_QUEUEID(i);
			*queue_mapping = i;

			tw32(HOSTCC_TXCOL_TICKS_VEC1 + i * 0x18,
			     tp->coal.tx_coalesce_usecs);
			tw32(HOSTCC_TXMAX_FRAMES_VEC1 + i * 0x18,
			     tp->coal.tx_max_coalesced_frames);
			tw32(HOSTCC_TXCOAL_MAXF_INT_VEC1 + i * 0x18,
			     tp->coal.tx_max_coalesced_frames_irq);

			netdev_info(tp->dev, "TX NetQ allocated on %d\n", i);
			return VMKNETDDI_QUEUEOPS_OK;
		}
	}

	netdev_err(tp->dev, "No free tx queues found!\n");
	return VMKNETDDI_QUEUEOPS_ERR;
}

static int
tg3_netq_alloc_rx_queue(struct net_device *netdev,
			 vmknetddi_queueops_queueid_t *p_qid,
			 struct napi_struct **napi_p)
{
	int i;
	struct tg3 *tp = netdev_priv(netdev);

	if (tp->vmware.netq.n_rx_queues_allocated >= TG3_VRQ_MAX_NUM_RX_QS(tp)) {
		netdev_err(tp->dev, "RX Q alloc: No queues available!\n" );
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	for (i = 1; i < TG3_VRQ_MAX_NUM_RX_QS(tp) + 1; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];
		if (!(tnapi->netq.flags & TG3_NETQ_RXQ_ALLOCATED)) {
			tnapi->netq.flags |= TG3_NETQ_RXQ_ALLOCATED;
			tp->vmware.netq.n_rx_queues_allocated++;
			*p_qid = VMKNETDDI_QUEUEOPS_MK_RX_QUEUEID(i);
			*napi_p = &tnapi->napi;

			netdev_info(tp->dev, "RX NetQ allocated on %d\n", i);
			return VMKNETDDI_QUEUEOPS_OK;
		}
	}
	netdev_err(tp->dev, "No free rx queues found!\n");
	return VMKNETDDI_QUEUEOPS_ERR;
}

static int
tg3_netq_alloc_queue(vmknetddi_queueop_alloc_queue_args_t *args)
{
	struct net_device *netdev = args->netdev;
	struct tg3 *tp = netdev_priv(netdev);

	if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_TX) {
		return tg3_netq_alloc_tx_queue(args->netdev, &args->queueid,
					     &args->queue_mapping);
	} else if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_RX) {
		return tg3_netq_alloc_rx_queue(args->netdev, &args->queueid,
						&args->napi);
	} else {
		netdev_err(tp->dev, "Trying to alloc invalid queue type: %x\n",
			   args->type);
		return VMKNETDDI_QUEUEOPS_ERR;
	}
}

static void tg3_netq_txq_free(struct tg3 *tp, int qid)
{
	struct tg3_napi *tnapi = &tp->napi[qid + 1];

	tnapi->netq.flags &= ~TG3_NETQ_TXQ_ALLOCATED;
	tp->vmware.netq.n_tx_queues_allocated--;

	/* Don't sit on tx packet completions.
	 * Send them up as soon as they are ready.
	 */
	tw32(HOSTCC_TXCOL_TICKS_VEC1 + qid * 0x18, 0);
	tw32(HOSTCC_TXMAX_FRAMES_VEC1 + qid * 0x18, 1);
	tw32(HOSTCC_TXCOAL_MAXF_INT_VEC1 + qid * 0x18, 1);
}

static int
tg3_netq_free_tx_queue(struct net_device *netdev,
		       vmknetddi_queueops_queueid_t qid)
{
	struct tg3 *tp = netdev_priv(netdev);
	struct tg3_napi *tnapi;

	u16 index = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(qid);
	if (index == 0 || index > TG3_VRQ_MAX_NUM_TX_QS(tp)) {
		netdev_err(tp->dev,
			   "Trying to free invalid tx queue: %d\n", index);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	tnapi = &tp->napi[index + 1];

	if (!(tnapi->netq.flags & TG3_NETQ_TXQ_ALLOCATED)) {
		netdev_info(tp->dev,
			    "Trying to free unallocated tx queue: %d\n", index);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	tg3_netq_txq_free(tp, index);

	if (tnapi->tx_cons != tnapi->tx_prod) {
		netdev_warn(tp->dev,
			    "Timeout submitting free NetQ TX Queue: %x\n",
			    index);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	netdev_info(tp->dev, "Free NetQ TX Queue: %d\n", index);

	return VMKNETDDI_QUEUEOPS_OK;
}

static void
tg3_netq_disable_queue(struct tg3 *tp, int qid)
{
	int i;
	struct tg3_napi *tnapi = &tp->napi[qid];
	u32 val;

	tnapi->netq.flags &= ~TG3_NETQ_RXQ_ENABLED;

	tg3_disable_prod_rcbs(tp, qid);

	/* Disable the VRQ */
	tw32(MAC_VRQ_ENABLE, tr32(MAC_VRQ_ENABLE) & ~(1 << qid));

	/* Poll for acknowledgement from the hardware. */
	for (i = 0; i < TG3_VRQ_CHANGE_TIMEOUT_US / 10; i++) {
		if (!(tr32(VRQ_STATUS) & (1 << qid)))
			break;
		udelay(10);
	}

	if (i == TG3_VRQ_CHANGE_TIMEOUT_US / 10)
		netdev_warn(tp->dev,
			    "Timeout performing initial queue disable: %x\n",
			    qid);

#if 0
	/* Force a DMA of all remaining rx packets in this VRQ */
	tw32(HOSTCC_MODE,
	     tp->coalesce_mode | HOSTCC_MODE_ENABLE | tnapi->coal_now);

	/* There isn't a reliable way to tell that all the rx packets have
	 * made it to the host.  The status tag can change before all packets
	 * have drained.  The best we can do is wait a little bit.
	 */
	msleep(100);
#endif

	tw32(HOSTCC_PARAM_SET_RESET, 1 << qid);

	tw32_rx_mbox(tnapi->prodring.rx_std_mbox, 0);
	tw32_rx_mbox(tnapi->prodring.rx_jmb_mbox, 0);
	tw32_rx_mbox(tnapi->consmbox, 0);
	tnapi->rx_rcb_ptr = 0;

	tw32(HOSTCC_RXCOL_TICKS_VEC1 + (qid - 1) * 0x18, 0);
	tw32(HOSTCC_RXMAX_FRAMES_VEC1 + (qid - 1) * 0x18, 0);
	tw32(HOSTCC_RXCOAL_MAXF_INT_VEC1 + (qid - 1) * 0x18, 0);

	val = ~(1 << qid) << 15;
	val |= VRQ_FLUSH_ENABLE | VRQ_FLUSH_RESET_ENABLE |
	       VRQ_FLUSH_STATUPDT_INT_ENABLE | VRQ_FLUSH_DISCARD_PKT_ENABLE |
	       VRQ_FLUSH_SW_FLUSH;
	tw32(VRQ_FLUSH_CTRL, val);

	/* Poll for acknowledgement from the hardware. */
	for (i = 0; i < TG3_VRQ_FLUSH_TIMEOUT_US / 10; i++) {
		if (!(tr32(VRQ_FLUSH_CTRL) & VRQ_FLUSH_SW_FLUSH))
			break;
		udelay(10);
	}
	if (tr32(VRQ_FLUSH_CTRL) & VRQ_FLUSH_SW_FLUSH) {
		netdev_warn(tp->dev,
			    "Timeout flushing hardware queue: %x\n",
			    qid);
	}

	/* Force a status block update to refresh all the producer
	 * and consumer indexes.  Also, we want to make sure we
	 * use the right tag when reenabling interrupts.
	 */
	tw32_f(HOSTCC_MODE, tp->coalesce_mode |
	       HOSTCC_MODE_ENABLE | HOSTCC_MODE_NOINT_ON_NOW |
	       tnapi->coal_now);

	for (i = 0; i < 500; i++) {
		if (tnapi->hw_status->rx_jumbo_consumer == 0 &&
		    tnapi->hw_status->rx_consumer == 0 &&
		    tnapi->hw_status->rx_mini_consumer == 0 &&
		    tnapi->hw_status->idx[0].rx_producer == 0)
			break;
		mdelay(1);
	}

	if (i == 500) {
		netdev_warn(tp->dev,
			    "%d: Timeout waiting for final status block update.\n",
			    qid);
	}

	/* Disable the VRQ mapper for this vector */
	tw32(MAC_VRQMAP_2H + qid * 8, 0);

	/* Disable the perfect match term */
	tw32(MAC_VRQMAP_1H + qid * 8, 0);

	/* Disable the VLAN match term */
	tw32(MAC_VRQFLT_FLTSET + qid * 4, 0);
	tw32(MAC_VRQFLT_CFG + qid * 4, 0);
	tw32(MAC_VRQFLT_PTRN + qid * 4, 0);

	/* Remove the perfect match filter. */
	if (qid < 4) {
		tw32(MAC_ADDR_0_HIGH + (qid * 8), tr32(MAC_ADDR_0_HIGH));
		tw32(MAC_ADDR_0_LOW + (qid * 8), tr32(MAC_ADDR_0_LOW));
	} else {
		tw32(MAC_VRQ_PMATCH_HI_5 + (qid - 4) * 8,
		     tr32(MAC_ADDR_0_HIGH));
		tw32(MAC_VRQ_PMATCH_LO_5 + (qid - 4) * 8,
		     tr32(MAC_ADDR_0_LOW));
	}

	tg3_rx_prodring_free(tp, &tnapi->prodring);

	tnapi->last_tag = tnapi->hw_status->status_tag;
	tnapi->last_irq_tag = tnapi->last_tag;

	tnapi->chk_msi_cnt = 0;
	tnapi->last_rx_cons = 0;
	tnapi->last_tx_cons = 0;
}

static int
tg3_netq_free_rx_queue(struct net_device *netdev,
		       vmknetddi_queueops_queueid_t qid)
{
	struct tg3 *tp = netdev_priv(netdev);
	struct tg3_napi *tnapi;

	u16 index = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(qid);
	if (index == 0 || index > TG3_VRQ_MAX_NUM_RX_QS(tp)) {
		netdev_err(tp->dev,
			   "Trying to free invalid rx queue: %d\n", index);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	tnapi = &tp->napi[index];
	if (!(tnapi->netq.flags & TG3_NETQ_RXQ_ALLOCATED)) {
		netdev_warn(tp->dev,
			    "Attempt to free a queue that is already free: %x\n",
			    qid);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	if (tnapi->netq.flags & TG3_NETQ_RXQ_ENABLED) {
		napi_disable(&tnapi->napi);
		tw32_mailbox_f(tnapi->int_mbox, 1);

		tg3_netq_disable_queue(tp, index);

		napi_enable(&tnapi->napi);
		tg3_int_reenable(tnapi);

#ifdef BCM_HAS_NEW_IRQ_SIG
		tg3_msi(0, tnapi);
#else
		tg3_msi(0, tnapi, 0);
#endif
	}

	tnapi->netq.flags &= ~TG3_NETQ_RXQ_ALLOCATED;
	tp->vmware.netq.n_rx_queues_allocated--;

	netdev_info(tp->dev, "Free NetQ RX Queue: %x\n", index);

	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_free_queue(vmknetddi_queueop_free_queue_args_t *args)
{
	struct tg3 *tp = netdev_priv(args->netdev);
	if (VMKNETDDI_QUEUEOPS_IS_TX_QUEUEID(args->queueid)) {
		return tg3_netq_free_tx_queue(args->netdev, args->queueid);
	} else if (VMKNETDDI_QUEUEOPS_IS_RX_QUEUEID(args->queueid)) {
		return tg3_netq_free_rx_queue(args->netdev, args->queueid);
	} else {
		netdev_err(tp->dev, "free netq: invalid queue type\n");
		return VMKNETDDI_QUEUEOPS_ERR;
	}
}

static int
tg3_netq_get_queue_vector(vmknetddi_queueop_get_queue_vector_args_t *args)
{
	int qid;
	struct tg3 *tp = netdev_priv(args->netdev);

	qid = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(args->queueid);

	if (VMKNETDDI_QUEUEOPS_IS_TX_QUEUEID(args->queueid)) {
		if (qid > tp->irq_cnt - 2) {
			netdev_err(tp->dev,
				   "Attempt to get vector for "
				   "invalid TX queue ID 0x%x\n",
				   qid);
			return VMKNETDDI_QUEUEOPS_ERR;
		}
		qid++;
	} else if (VMKNETDDI_QUEUEOPS_IS_RX_QUEUEID(args->queueid)) {
		if (qid > tp->irq_cnt - 1) {
			netdev_err(tp->dev,
				   "Attempt to get vector for "
				   "invalid RX queue ID 0x%x\n",
				   qid);
			return VMKNETDDI_QUEUEOPS_ERR;
		}
	} else {
		netdev_err(tp->dev,
			   "Attempt to get vector for invalid "
			   "queue type, ID 0x%x\n",
			   qid);
	}

	args->vector = tp->napi[qid].irq_vec;

	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_get_default_queue(vmknetddi_queueop_get_default_queue_args_t *args)
{
	struct tg3 *tp = netdev_priv(args->netdev);

	if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_RX) {
		args->queueid = VMKNETDDI_QUEUEOPS_MK_RX_QUEUEID(0);
		args->napi = &tp->napi[0].napi;
		return VMKNETDDI_QUEUEOPS_OK;
	} else if (args->type == VMKNETDDI_QUEUEOPS_QUEUE_TYPE_TX) {
		args->queueid = VMKNETDDI_QUEUEOPS_MK_TX_QUEUEID(0);
		args->queue_mapping = 0;
		return VMKNETDDI_QUEUEOPS_OK;
	} else
		return VMKNETDDI_QUEUEOPS_ERR;
}

static int
tg3_netq_remove_rx_filter(vmknetddi_queueop_remove_rx_filter_args_t *args)
{
	struct tg3 *tp = netdev_priv(args->netdev);
	struct tg3_napi *tnapi;
	u16 qid = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(args->queueid);
	u16 fid = VMKNETDDI_QUEUEOPS_FILTERID_VAL(args->filterid);

	if (!VMKNETDDI_QUEUEOPS_IS_RX_QUEUEID(args->queueid)) {
		netdev_err(tp->dev, "0X%x is not a valid QID\n", qid );
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	if (qid == 0 || qid > TG3_VRQ_MAX_NUM_RX_QS(tp)) {
		netdev_err(tp->dev, "QID 0X%x is out of range\n", qid );
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	tnapi = &tp->napi[qid];

	if (!(tnapi->netq.flags & TG3_NETQ_RXQ_ENABLED)) {
		netdev_err(tp->dev, "Filter not allocated on QID %d\n", qid );
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	/* Only support one Mac filter per queue */
	if (fid != qid) {
		netdev_err(tp->dev, "Invalid filter ID (0x%x) on QID %d\n", fid, qid );
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	napi_disable(&tnapi->napi);
	tw32_mailbox_f(tnapi->int_mbox, 1);

	tg3_netq_disable_queue(tp, qid);

	napi_enable(&tnapi->napi);
	tg3_int_reenable(tnapi);

#ifdef BCM_HAS_NEW_IRQ_SIG
	tg3_msi(0, tnapi);
#else
	tg3_msi(0, tnapi, 0);
#endif

	netdev_info(tp->dev, "NetQ remove RX filter: %d\n", qid);

	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_apply_rx_filter(vmknetddi_queueop_apply_rx_filter_args_t *args)
{
	u8 *macaddr = NULL;
	struct tg3_napi *tnapi;
	struct tg3 *tp = netdev_priv(args->netdev);
	u16 qid = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(args->queueid);
	vmknetddi_queueops_filter_class_t class;
	DECLARE_MAC_BUF(mac);
	u32 val;
	u16 vlan_id = 0;

	if (!VMKNETDDI_QUEUEOPS_IS_RX_QUEUEID(args->queueid)) {
		netdev_err(tp->dev, "Invalid NetQ RX ID: %x\n", args->queueid);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	if (qid == 0 || qid > TG3_VRQ_MAX_NUM_RX_QS(tp)) {
		netdev_err(tp->dev,
			   "Applying filter with invalid RX NetQ %d ID\n", qid);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	tnapi = &tp->napi[qid];

	if (!(tnapi->netq.flags & TG3_NETQ_RXQ_ALLOCATED)) {
		netdev_err(tp->dev, "RX NetQ %d not allocated\n", qid);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	if (tnapi->netq.flags & TG3_NETQ_RXQ_ENABLED) {
		netdev_err(tp->dev, "RX NetQ %d already enabled\n", qid);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	class = vmknetddi_queueops_get_filter_class(&args->filter);
	switch (class) {
	case VMKNETDDI_QUEUEOPS_FILTER_VLAN:
	case VMKNETDDI_QUEUEOPS_FILTER_MACADDR:
	case VMKNETDDI_QUEUEOPS_FILTER_VLANMACADDR:
		break;
	default:
		netdev_err(tp->dev, "Received invalid RX NetQ filter: %x\n",
			   class);
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	if (class == VMKNETDDI_QUEUEOPS_FILTER_MACADDR ||
	    class == VMKNETDDI_QUEUEOPS_FILTER_VLANMACADDR)
		macaddr = (void *)vmknetddi_queueops_get_filter_macaddr(&args->filter);

	if (class == VMKNETDDI_QUEUEOPS_FILTER_VLAN ||
	    class == VMKNETDDI_QUEUEOPS_FILTER_VLANMACADDR)
		vlan_id = vmknetddi_queueops_get_filter_vlanid(&args->filter);

	/* Populate the producer rings with skbs */
	if (tg3_rx_prodring_alloc(tp, &tnapi->prodring)) {
		netdev_err(tp->dev, "Failed to allocate queue buffers!\n");
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	/* Make the producer rings available to the hardware */
	tg3_setup_prod_rcbs(tp, qid);

	if (macaddr) {
		/* Program the perfect match filter. */
		if (qid < 4) {
			val = (macaddr[0] << 8) | macaddr[1];
			tw32(MAC_ADDR_0_HIGH + (qid * 8), val);

			val = ((macaddr[2] << 24) | (macaddr[3] << 16) |
			       (macaddr[4] <<  8) | (macaddr[5] <<  0));
			tw32(MAC_ADDR_0_LOW + (qid * 8), val);
		} else {
			val = (macaddr[0] << 8)  | macaddr[1];
			tw32(MAC_VRQ_PMATCH_HI_5 + (qid - 4) * 8, val);

			val = (macaddr[2] << 24) | (macaddr[3] << 16) |
				  (macaddr[4] <<  8) | (macaddr[5] <<  0);
			tw32(MAC_VRQ_PMATCH_LO_5 + (qid - 4) * 8, val);
		}

		/* Tell the hardware which perfect match filter to use. */
		val = MAC_VRQMAP_1H_PTA_PFEN | qid;
		tw32(MAC_VRQMAP_1H + qid * 8, val);
	}

	if (vlan_id) {
		val = MAC_VRQFLT_PTRN_VLANID | (htons(vlan_id) << 16);
		tw32(MAC_VRQFLT_PTRN + qid * 4, val);

		tw32(MAC_VRQFLT_FLTSET + qid * 4, 1 << qid);

		val = MAC_VRQFLT_ELEM_EN | MAC_VRQFLT_HDR_VLAN;
		tw32(MAC_VRQFLT_CFG + qid * 4, val);

		val = MAC_VRQMAP_2H_PTA_VFEN | (1 << qid);;
	} else
		val = 0;

	if (macaddr && vlan_id)
		val |= MAC_VRQMAP_2H_PTA_AND;
	else
		val |= MAC_VRQMAP_2H_PTA_OR;

	val |= MAC_VRQMAP_2H_PTA_EN;
	tw32(MAC_VRQMAP_2H + qid * 8, val);

	/* Enable the VRQ */
	val = tr32(MAC_VRQ_ENABLE);
	tw32(MAC_VRQ_ENABLE, val | (1 << qid));

	tw32(HOSTCC_RXCOL_TICKS_VEC1 + (qid - 1) * 0x18,
	     tp->coal.rx_coalesce_usecs);
	tw32(HOSTCC_RXMAX_FRAMES_VEC1 + (qid - 1) * 0x18,
	     tp->coal.rx_max_coalesced_frames);
	tw32(HOSTCC_RXCOAL_MAXF_INT_VEC1 + (qid - 1) * 0x18,
	     tp->coal.rx_max_coalesced_frames_irq);

	tnapi->netq.flags |= TG3_NETQ_RXQ_ENABLED;

	/*  Apply RX filter code here */
	args->filterid = VMKNETDDI_QUEUEOPS_MK_FILTERID(qid);

	netdev_info(tp->dev, "NetQ set RX Filter: %d [%s %d]\n", qid,
		    macaddr ? print_mac(mac, macaddr) : "00:00:00:00:00:00",
		    vmknetddi_queueops_get_filter_vlanid(&args->filter));

	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netq_get_queue_stats(vmknetddi_queueop_get_stats_args_t *args)
{
	u16 qid = VMKNETDDI_QUEUEOPS_QUEUEID_VAL(args->queueid);
	struct tg3_napi *tnapi;
	struct tg3 *tp = netdev_priv(args->netdev);
	struct net_device_stats *netstats;

	tnapi = &tp->napi[qid];
	netstats = &tnapi->netq.net_stats;

	netstats->rx_packets = tnapi->netq.stats.rx_packets_sw;
	netstats->rx_bytes = tnapi->netq.stats.rx_bytes_sw;
	netstats->rx_errors = tnapi->netq.stats.rx_errors_sw;
	netstats->rx_crc_errors = tnapi->netq.stats.rx_crc_errors;
	netstats->rx_frame_errors = tnapi->netq.stats.rx_frame_errors;
	netstats->tx_packets = tnapi->netq.stats.tx_ucast_packets +
			       tnapi->netq.stats.tx_mcast_packets +
			       tnapi->netq.stats.tx_bcast_packets;
	netstats->tx_bytes = tnapi->netq.stats.tx_bytes;

	args->stats = netstats;

	return VMKNETDDI_QUEUEOPS_OK;
}

static int
tg3_netqueue_ops(vmknetddi_queueops_op_t op, void *args)
{
	struct tg3 *tp;

	if (op == VMKNETDDI_QUEUEOPS_OP_GET_VERSION)
		return vmknetddi_queueops_version(
			(vmknetddi_queueop_get_version_args_t *)args);

	tp = netdev_priv(((vmknetddi_queueop_get_features_args_t *)args)->netdev);

	if (!(tg3_flag(tp, USING_MSIX)) ||
	    !(tg3_flag(tp, INIT_COMPLETE)) ||
	    tp->irq_cnt < 2) {
		netdev_err(tp->dev, "Device not ready for netq ops!\n");
		return VMKNETDDI_QUEUEOPS_ERR;
	}

	switch (op) {
	case VMKNETDDI_QUEUEOPS_OP_GET_FEATURES:
		return tg3_netq_get_netqueue_features(
			(vmknetddi_queueop_get_features_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_GET_QUEUE_COUNT:
		return tg3_netq_get_queue_count(
			(vmknetddi_queueop_get_queue_count_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_GET_FILTER_COUNT:
		return tg3_netq_get_filter_count(
			(vmknetddi_queueop_get_filter_count_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_ALLOC_QUEUE:
		return tg3_netq_alloc_queue(
			(vmknetddi_queueop_alloc_queue_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_FREE_QUEUE:
		return tg3_netq_free_queue(
			(vmknetddi_queueop_free_queue_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_GET_QUEUE_VECTOR:
		return tg3_netq_get_queue_vector(
			(vmknetddi_queueop_get_queue_vector_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_GET_DEFAULT_QUEUE:
		return tg3_netq_get_default_queue(
			(vmknetddi_queueop_get_default_queue_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_APPLY_RX_FILTER:
		return tg3_netq_apply_rx_filter(
			(vmknetddi_queueop_apply_rx_filter_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_REMOVE_RX_FILTER:
		return tg3_netq_remove_rx_filter(
			(vmknetddi_queueop_remove_rx_filter_args_t *)args);

	case VMKNETDDI_QUEUEOPS_OP_GET_STATS:
		return tg3_netq_get_queue_stats(
			(vmknetddi_queueop_get_stats_args_t *)args);

	/*  Unsupported for now */
	default:
		break;
	}

	netdev_warn(to->dev, "Unhandled NETQUEUE OP %d\n", op);

	return VMKNETDDI_QUEUEOPS_ERR;
}

static void tg3_netq_init(struct tg3 *tp)
{
	struct tg3_netq_napi *tnetq;

	if (tg3_netq_force[tp->vmware.netq.index] >= 0) {
		tp->rxq_req = tg3_netq_force[tp->vmware.netq.index] + 1;

		tp->txq_req = tg3_netq_force[tp->vmware.netq.index];
		tp->txq_req = min(tp->txq_req, tp->txq_max);
		tp->txq_req = max_t(u32, tp->txq_req, 1);
	}

	if (!tg3_netq_force[tp->vmware.netq.index])
		return;

	tnetq = &tp->napi[0].netq;
	tnetq->flags |= TG3_NETQ_RXQ_ALLOCATED | TG3_NETQ_RXQ_ENABLED;

	tnetq->flags |= TG3_NETQ_TXQ_ALLOCATED;
	tnetq = &tp->napi[1].netq;
	tnetq->flags |= TG3_NETQ_TXQ_ALLOCATED;

	VMKNETDDI_REGISTER_QUEUEOPS(tp->dev, tg3_netqueue_ops);
	netdev_info(tp->dev, "VMware NetQueue Ops is registered\n");
}

static void tg3_netq_free_all_qs(struct tg3 *tp)
{
	int i;

	for (i = 1; i < tp->irq_max; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		if (i != 1 && (tnapi->netq.flags & TG3_NETQ_TXQ_ALLOCATED))
			tg3_netq_txq_free(tp, i - 1);

		if (tnapi->netq.flags & TG3_NETQ_RXQ_ALLOCATED) {
			if (tnapi->netq.flags & TG3_NETQ_RXQ_ENABLED)
				tg3_netq_disable_queue(tp, i);

			tnapi->netq.flags &= ~TG3_NETQ_RXQ_ALLOCATED;
			tp->vmware.netq.n_rx_queues_allocated--;
		}
	}
}

static void tg3_netq_invalidate_state(struct tg3 *tp)
{
	if (!(tg3_flag(tp, ENABLE_IOV)))
		return;

	tg3_netq_free_all_qs(tp);
	vmknetddi_queueops_invalidate_state(tp->dev);
}

static void tg3_netq_restore(struct tg3 *tp)
{
	if (!(tg3_flag(tp, IOV_CAPABLE)))
		return;

	/* Enable the VRQs */
	tw32(MAC_VRQ_ENABLE, MAC_VRQ_ENABLE_DFLT_VRQ);
}

static void tg3_netq_limit_dflt_queue_counts(struct tg3 *tp)
{
	if (!tg3_flag(tp, IOV_CAPABLE))
		return;

	/* If the number of rx and tx queues was not formally
	 * requested, artificially cap the number of queues
	 * to ease system resource consumption.
	 */
	if (!tp->rxq_req) {
		/* Allocated 9 queues (8 + 1) by default. */
		tp->rxq_cnt = min_t(u32, tp->rxq_cnt + 1, 9);
	}

	if (!tp->txq_req) {
		tp->txq_cnt = min_t(u32, tp->txq_cnt, 8);
	}
}

static u32 tg3_netq_tune_vector_count(struct tg3 *tp)
{
	u32 irqcnt = max(tp->rxq_cnt, tp->txq_cnt);

	if (irqcnt > 1 && tp->txq_cnt > tp->rxq_cnt - 1)
		irqcnt++;

	return irqcnt;
}

static int tg3_netq_stats_size(struct tg3 *tp)
{
	int size = TG3_NUM_STATS;

	if (!tg3_flag(tp, ENABLE_IOV))
		return size;

	size += ARRAY_SIZE(tg3_vmware_ethtool_stats_keys) * tp->irq_cnt;

	return size;
}

static void tg3_netq_stats_get_strings(struct tg3 *tp, u8 *buf)
{
	int i;

	for (i = 0; i < tp->irq_cnt; i++) {
		sprintf(buf, "[%d]: rx_packets (sw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_packets (hw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_bytes (sw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_bytes (hw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_errors (sw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_errors (hw)", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_crc_errors", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: rx_frame_errors", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: tx_bytes", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: tx_ucast_packets", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: tx_mcast_packets", i);
		buf += ETH_GSTRING_LEN;
		sprintf(buf, "[%d]: tx_bcast_packets", i);
		buf += ETH_GSTRING_LEN;
	}
}

static void tg3_netq_stats_get(struct tg3 *tp, u64 *tmp_stats)
{
	int i;

	if (!tg3_flag(tp, ENABLE_IOV))
		return;

	/* Copy over the NetQ specific statistics */
	for (i = 0; i < tp->irq_cnt; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		memcpy(tmp_stats, &tnapi->netq.stats,
		       sizeof(struct tg3_netq_stats));
		tmp_stats += TG3_NETQ_NUM_STATS;
	}
}

static void tg3_netq_stats_clear(struct tg3 *tp)
{
	int i;

	for (i = 0; i < tp->irq_max; i++) {
		struct tg3_napi *tnapi = &tp->napi[i];

		memset(&tnapi->netq.stats, 0,
		       sizeof(struct tg3_netq_stats));
	}
}
#endif /* TG3_VMWARE_NETQ_ENABLE */
