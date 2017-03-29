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

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/hardirq.h>
#include <net/ieee80211_radiotap.h>
#include <linux/if_arp.h>
#include <linux/moduleparam.h>

#include "wil6210.h"
#include "wmi.h"
#include "txrx.h"
#include "wil6210_rgf.h"

static bool rtap_include_phy_info;
module_param(rtap_include_phy_info, bool, S_IRUGO);
MODULE_PARM_DESC(rtap_include_phy_info,
		" Include PHY info in the radiotap header, default - no");

static inline int vring_is_empty(struct vring *vring)
{
	return vring->swhead == vring->swtail;
}

static inline u32 vring_next_tail(struct vring *vring)
{
	return (vring->swtail+1)%vring->size;
}

static inline void vring_advance_head(struct vring *vring)
{
	vring->swhead = (vring->swhead+1)%vring->size;
}

static inline int vring_is_full(struct vring *vring)
{
	return vring_next_tail(vring) == vring->swhead;
}

static int vring_alloc(struct wil6210_priv *wil, struct vring *vring)
{
	struct device *dev = wil_to_dev(wil);
	size_t sz = vring->size * sizeof(vring->va[0]);
	int i;
	BUILD_BUG_ON(sizeof(vring->va[0]) != 32);
	wil_info(wil, "%s()\n", __func__);
	vring->swhead = 0;
	vring->swtail = 0;
	vring->ctx = kzalloc(vring->size * sizeof(vring->ctx[0]), GFP_KERNEL);
	if (!vring->ctx) {
		wil_err(wil, "vring_alloc [%d] failed to alloc ctx mem\n",
				vring->size);
		vring->va = NULL;
		return -ENOMEM;
	}
	vring->va = dma_alloc_coherent(dev, sz, &vring->pa, GFP_KERNEL);
	if (!vring->va) {
		wil_err(wil, "vring_alloc [%d] failed to alloc DMA mem\n",
				vring->size);
		kfree(vring->ctx);
		vring->ctx = NULL;
		return -ENOMEM;
	}
	/* initially, all descriptors are SW owned
	 * For Tx and Rx, ownership bit is at the same location, thus
	 * we can use any
	 */
	for (i = 0; i < vring->size; i++) {
		struct vring_tx_desc *d = &(vring->va[i].tx);
		d->dma.status = TX_DMA_STATUS_DU;
	}
	wil_info(wil, "vring[%d] 0x%p:0x%016llx 0x%p\n", vring->size,
			vring->va, (unsigned long long)vring->pa, vring->ctx);
	return 0;
}

static void vring_free(struct wil6210_priv *wil, struct vring *vring, int tx)
{
	struct device *dev = wil_to_dev(wil);
	size_t sz = vring->size * sizeof(vring->va[0]);
	wil_info(wil, "%s()\n", __func__);
	while (!vring_is_empty(vring)) {
		if (tx) {
			struct vring_tx_desc *d = &vring->va[vring->swtail].tx;
			dma_addr_t pa = d->dma.addr_low |
					((u64)d->dma.addr_high << 32);
			struct sk_buff *skb = vring->ctx[vring->swtail];
			if (skb) {
				dma_unmap_single(dev, pa, d->dma.length,
						DMA_TO_DEVICE);
				dev_kfree_skb_any(skb);
				vring->ctx[vring->swtail] = NULL;
			} else {
				dma_unmap_page(dev, pa, d->dma.length,
						DMA_TO_DEVICE);
			}
			vring->swtail = vring_next_tail(vring);
		} else { /* rx */
			struct vring_rx_desc *d = &vring->va[vring->swtail].rx;
			dma_addr_t pa = d->dma.addr_low |
					((u64)d->dma.addr_high << 32);
			struct sk_buff *skb = vring->ctx[vring->swhead];
			dma_unmap_single(dev, pa, d->dma.length,
					DMA_FROM_DEVICE);
			kfree_skb(skb);
			vring_advance_head(vring);
		}
	}
	dma_free_coherent(dev, sz, vring->va, vring->pa);
	kfree(vring->ctx);
	vring->pa = 0;
	vring->va = NULL;
	vring->ctx = NULL;
}

/**
 * Allocate one skb for Rx VRING
 *
 * Safe to call from IRQ
 */
static int vring_alloc_skb(struct wil6210_priv *wil,
		struct vring *vring, u32 i, int headroom)
{
	struct device *dev = wil_to_dev(wil);
	unsigned int sz = RX_BUF_LEN;
	struct vring_rx_desc *d = &(vring->va[i].rx);
	dma_addr_t pa;
	/* TODO align */
	struct sk_buff *skb = dev_alloc_skb(sz + headroom);
	if (unlikely(!skb))
		return -ENOMEM;
	skb_reserve(skb, headroom);
	skb_put(skb, sz);
	pa = dma_map_single(dev, skb->data, skb->len, DMA_FROM_DEVICE);
	if (unlikely(dma_mapping_error(dev, pa))) {
		kfree_skb(skb);
		return -ENOMEM;
	}
	d->dma.d0 = BIT(9) | RX_DMA_D0_CMD_DMA_IT;
	d->dma.addr_low = lower_32_bits(pa);
	d->dma.addr_high = (u16)upper_32_bits(pa);
	/* ip_length don't care */
	/* b11 don't care */
	/* error don't care */
	d->dma.status = 0; /* BIT(0) should be 0 for HW_OWNED */
	d->dma.length = sz;
	vring->ctx[i] = skb;
	return 0;
}

/**
 * Adds radiotap header
 *
 * Any error indicated as "Bad FCS"
 *
 * Vendor data for 04:ce:14-1 (Wilocity-1) consists of:
 *  - Rx descriptor: 32 bytes
 *  - Phy info
 */
static void rx_add_radiotap_header(struct wil6210_priv *wil,
				 struct sk_buff *skb,
				 struct vring_rx_desc *d)
{
	struct wil6210_rtap {
		struct ieee80211_radiotap_header rthdr;
		/* fields should be in the order of bits in rthdr.it_present */
		/* flags */
		u8 flags;
		/* channel */
		__le16 chnl_freq __aligned(2);
		__le16 chnl_flags;
		/* MCS */
		u8 mcs_present;
		u8 mcs_flags;
		u8 mcs_index;
	} __packed;
	struct wil6210_rtap_vendor {
		struct wil6210_rtap rtap;
		/* vendor */
		u8 vendor_oui[3] __aligned(2);
		u8 vendor_ns;
		__le16 vendor_skip;
		u8 vendor_data[0];
	} __packed;
	struct wil6210_rtap_vendor *rtap_vendor;
	int rtap_len = sizeof(struct wil6210_rtap);
	int phy_length = 0; /* phy info header size, bytes */
	static char phy_data[128];
	if (rtap_include_phy_info) {
		rtap_len = sizeof(*rtap_vendor) + sizeof(*d);
		/* calculate additional length */
		if (d->dma.status & RX_DMA_STATUS_PHY_INFO) {
			/**
			 * PHY info starts from 8-byte boundary
			 * there are 8-byte lines, last line may be partially
			 * written (HW bug), thus FW configures for last line
			 * to be excessive. Driver skips this last line.
			 */
			int len = min_t(int, 8 + sizeof(phy_data),
					rxdesc_phy_length(d));
			if (len > 8) {
				void *p = skb_tail_pointer(skb);
				void *pa = PTR_ALIGN(p, 8);
				if (skb_tailroom(skb) >= len + (pa - p)) {
					phy_length = len - 8;
					memcpy(phy_data, pa, phy_length);
				}
			}
		}
		rtap_len += phy_length;
	}

	if (skb_headroom(skb) < rtap_len &&
	    pskb_expand_head(skb, rtap_len, 0, GFP_ATOMIC)) {
		wil_err(wil, "Unable to expand headrom to %d\n", rtap_len);
		return;
	}

	rtap_vendor = (void *)skb_push(skb, rtap_len);
	memset(rtap_vendor, 0, rtap_len);

	rtap_vendor->rtap.rthdr.it_version = PKTHDR_RADIOTAP_VERSION;
	rtap_vendor->rtap.rthdr.it_len = cpu_to_le16(rtap_len);
	rtap_vendor->rtap.rthdr.it_present = cpu_to_le32(
			(1 << IEEE80211_RADIOTAP_FLAGS) |
			(1 << IEEE80211_RADIOTAP_CHANNEL) |
			(1 << IEEE80211_RADIOTAP_MCS));
	if (d->dma.status & RX_DMA_STATUS_ERROR)
		rtap_vendor->rtap.flags |= IEEE80211_RADIOTAP_F_BADFCS;

	rtap_vendor->rtap.chnl_freq = cpu_to_le16(wil->monitor_chan ?
			wil->monitor_chan->center_freq : 58320);
	rtap_vendor->rtap.chnl_flags = cpu_to_le16(0);

	rtap_vendor->rtap.mcs_present = IEEE80211_RADIOTAP_MCS_HAVE_MCS;
	rtap_vendor->rtap.mcs_flags = 0;
	rtap_vendor->rtap.mcs_index = rxdesc_mcs(d);

	if (rtap_include_phy_info) {
		rtap_vendor->rtap.rthdr.it_present |= cpu_to_le32(1 <<
				IEEE80211_RADIOTAP_VENDOR_NAMESPACE);
		/* OUI for Wilocity 04:ce:14 */
		rtap_vendor->vendor_oui[0] = 0x04;
		rtap_vendor->vendor_oui[1] = 0xce;
		rtap_vendor->vendor_oui[2] = 0x14;
		rtap_vendor->vendor_ns = 1;
		/* Rx descriptor + PHY data  */
		rtap_vendor->vendor_skip = cpu_to_le16(sizeof(*d) +
				phy_length);
		memcpy(rtap_vendor->vendor_data, d, sizeof(*d));
		memcpy(rtap_vendor->vendor_data + sizeof(*d), phy_data,
				phy_length);
	}
}

/**
 * reap 1 frame from @swhead
 *
 * Safe to call from IRQ
 */
static struct sk_buff *vring_reap_rx(struct wil6210_priv *wil,
		struct vring *vring)
{
	struct device *dev = wil_to_dev(wil);
	struct net_device *ndev = wil_to_ndev(wil);
	struct vring_rx_desc *d;
	struct sk_buff *skb;
	dma_addr_t pa;
	unsigned int sz = RX_BUF_LEN;
	if (vring_is_empty(vring))
		return NULL;
	d = &(vring->va[vring->swhead].rx);
	if (!(d->dma.status & RX_DMA_STATUS_DU)) {
		/* it is not error, we just reached end of Rx done area */
		return NULL;
	}
	pa = d->dma.addr_low | ((u64)d->dma.addr_high << 32);
	skb = vring->ctx[vring->swhead];
	dma_unmap_single(dev, pa, sz, DMA_FROM_DEVICE);
	skb_trim(skb, d->dma.length);
	wil->stats.last_mcs_rx = rxdesc_mcs(d);
	/* use radiotap header only if required */
	if (ndev->type == ARPHRD_IEEE80211_RADIOTAP)
		rx_add_radiotap_header(wil, skb, d);
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "Rx[%3d] : %d bytes\n", vring->swhead, d->dma.length);
	print_hex_dump(KERN_INFO, "Rx ", DUMP_PREFIX_NONE, 32, 4, d
			, sizeof(*d), false);
#endif
	vring_advance_head(vring);
	return skb;
}

/**
 * allocate and fill up to @count buffers in rx ring
 * buffers posted at @swtail
 */
static int rx_refill(struct wil6210_priv *wil, int count)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct vring *v = &wil->vring_rx;
	u32 next_tail;
	int rc = 0;
	int headroom = ndev->type == ARPHRD_IEEE80211_RADIOTAP ?
			WIL6210_RTAP_SIZE : 0;
	for (; next_tail = vring_next_tail(v),
			(next_tail != v->swhead) && (count-- > 0);
			v->swtail = next_tail) {
		rc = vring_alloc_skb(wil, v, v->swtail, headroom);
		if (rc) {
			wil_err(wil, "Error %d in rx_refill[%d]\n",
					rc, v->swtail);
			break;
		}
	}
	iowrite32(v->swtail, wil->csr + HOSTADDR(v->hwtail));
	return rc;
}

static int netif_rx_any(struct sk_buff *skb)
{
	if (in_interrupt())
		return netif_rx(skb);
	else
		return netif_rx_ni(skb);
}

/**
 * Proceed all completed skb's from Rx VRING
 *
 * Safe to call from IRQ
 */
void rx_handle(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct vring *v = &wil->vring_rx;
	struct sk_buff *skb;
	if (!v->va) {
		wil_err(wil, "Rx IRQ while Rx not yet initialized\n");
		return;
	}
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "%s()\n", __func__);
#endif
	while (NULL != (skb = vring_reap_rx(wil, v))) {
#ifdef WIL_DEBUG_TXRX
		print_hex_dump(KERN_INFO, "Rx ", DUMP_PREFIX_OFFSET, 16, 1,
				skb->data, skb_headlen(skb), false);
#endif
		skb_orphan(skb);
		if (wil->wdev->iftype == NL80211_IFTYPE_MONITOR) {
			skb->dev = ndev;
			skb_reset_mac_header(skb);
			skb->ip_summed = CHECKSUM_UNNECESSARY;
			skb->pkt_type = PACKET_OTHERHOST;
			skb->protocol = htons(ETH_P_802_2);

		} else {
			skb->protocol = eth_type_trans(skb, ndev);
		}
		if (likely(netif_rx_any(skb) == NET_RX_SUCCESS)) {
			ndev->stats.rx_packets++;
			ndev->stats.rx_bytes += skb->len;

		} else {
			ndev->stats.rx_dropped++;
		}
	}
	rx_refill(wil, v->size);
}

int rx_init(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	struct vring *vring = &wil->vring_rx;
	int rc;
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_CFG_RX_CHAIN_CMD cfg;
	} __packed wmi_rx_cfg = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 1,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_CFG_RX_CHAIN_CMD),
		},
		.wmi = {
			.id = WMI_CFG_RX_CHAIN_CMDID,
			.info1 = 0,
		},
		.cfg = {
			.action = ADD_RX_CHAIN,
			.sw_ring = {
				.max_mpdu_size = RX_BUF_LEN,
			},
			.mid = 0, /* TODO - what is it? */
			.decap_trans_type = DECAP_TYPE_802_3,
		},
	};
	struct {
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_CFG_RX_CHAIN_DONE_EVENT cfg;
	} __packed wmi_rx_cfg_reply;
	wil_info(wil, "%s()\n", __func__);
	vring->size = WIL6210_RX_RING_SIZE;
	rc = vring_alloc(wil, vring);
	if (rc)
		return rc;
	wmi_rx_cfg.cfg.sw_ring.ring_mem_base = vring->pa;
	wmi_rx_cfg.cfg.sw_ring.ring_size = vring->size;
	if (wdev->iftype == NL80211_IFTYPE_MONITOR) {
		wmi_rx_cfg.cfg.sniffer_mode = 1;
		if (wil->monitor_chan)
			wmi_rx_cfg.cfg.sniffer_channel =
					wil->monitor_chan->hw_value - 1;
		wmi_rx_cfg.cfg.sniffer_phy_info =
				(ndev->type == ARPHRD_IEEE80211_RADIOTAP);
		/* 0 - CP, 1 - DP */
		wmi_rx_cfg.cfg.sniffer_phy =
				(wil->monitor_flags & MONITOR_FLAG_CONTROL) ?
						0 : 1;
	}
	rc = wmi_call(wil, &wmi_rx_cfg, WMI_CFG_RX_CHAIN_DONE_EVENTID,
			&wmi_rx_cfg_reply, sizeof(wmi_rx_cfg_reply), 200);
	if (rc)
		goto err_free;
	vring->hwtail = wmi_rx_cfg_reply.cfg.rx_ring_tail_ptr;
	wil_info(wil, "Rx init: status %d tail 0x%08x\n",
			wmi_rx_cfg_reply.cfg.status, vring->hwtail);
	rc = rx_refill(wil, vring->size);
	if (rc)
		goto err_free;
	return 0;
 err_free:
	vring_free(wil, vring, 0);
	return rc;
}

void rx_fini(struct wil6210_priv *wil)
{
	struct vring *vring = &wil->vring_rx;
	wil_info(wil, "%s()\n", __func__);
	if (vring->va) {
		int rc;
		struct {
			struct wil6210_mbox_hdr hdr;
			struct wil6210_mbox_hdr_wmi wmi;
			struct WMI_CFG_RX_CHAIN_CMD cfg;
		} __packed wmi_rx_cfg = {
			.hdr = {
				.seq = 1,
				.ctx = 1,
				.type = 0,
				.flags = 1,
				.len = sizeof(struct wil6210_mbox_hdr_wmi)
				+ sizeof(struct WMI_CFG_RX_CHAIN_CMD),
			},
			.wmi = {
				.id = WMI_CFG_RX_CHAIN_CMDID,
				.info1 = 0,
			},
			.cfg = {
				.action = DELETE_RX_CHAIN,
				.sw_ring = {
					.max_mpdu_size = RX_BUF_LEN,
				},
			},
		};
		struct {
			struct wil6210_mbox_hdr_wmi wmi;
			struct WMI_CFG_RX_CHAIN_DONE_EVENT cfg;
		} __packed wmi_rx_cfg_reply;
		rc = wmi_call(wil, &wmi_rx_cfg, WMI_CFG_RX_CHAIN_DONE_EVENTID,
				&wmi_rx_cfg_reply, sizeof(wmi_rx_cfg_reply),
				100);
		vring_free(wil, vring, 0);
	}
}

int vring_init_tx(struct wil6210_priv *wil, int id, int size,
		int cid, int tid)
{
	struct wireless_dev *wdev = wil->wdev;
	int rc;
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_VRING_CFG_CMD cfg;
	} __packed wmi_tx_cfg = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 1,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_VRING_CFG_CMD),
		},
		.wmi = {
			.id = WMI_VRING_CFG_CMDID,
			.info1 = 0,
		},
		.cfg = {
			.action = ADD_VRING,
			.sw_ring = {
				.max_mpdu_size = TX_BUF_LEN,
			},
			.ringid = id,
			.cidxtid = (cid & 0xf) | ((tid & 0xf) << 4),
			.encap_trans_type = ENC_TYPE_802_3,
			.mac_ctrl = 0,
			.to_resolution = 0,
			.agg_max_wsize = 0,
			.priority = 0,
			.timeslot_us = 0xfff,
		},
	};
	struct {
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_VRING_CFG_DONE_EVENT cfg;
	} __packed reply;
	struct vring *vring = &wil->vring_tx[id];
	wil_info(wil, "%s(%d)\n", __func__, id);
	if (vring->va) {
		wil_err(wil, "Tx ring [%d] already allocated\n", id);
		rc = -EINVAL;
		goto out;
	}
	vring->size = size;
	rc = vring_alloc(wil, vring);
	if (rc)
		goto out;
	wmi_tx_cfg.cfg.sw_ring.ring_mem_base = vring->pa;
	wmi_tx_cfg.cfg.sw_ring.ring_size = vring->size;
	/* FIXME Firmware works now in PBSS mode(ToDS=0, FromDS=0) */
	switch (wdev->iftype) {
	case NL80211_IFTYPE_STATION:
#if 0
		wmi_tx_cfg.cfg.ds_cfg = STATION_MODE;
#else
		wmi_tx_cfg.cfg.ds_cfg = PBSS_MODE;
#endif
		break;
	case NL80211_IFTYPE_AP:
#if 0
		wmi_tx_cfg.cfg.ds_cfg = AP_MODE;
#else
		wmi_tx_cfg.cfg.ds_cfg = PBSS_MODE;
#endif
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
		wmi_tx_cfg.cfg.ds_cfg = STATION_MODE;
		break;
	case NL80211_IFTYPE_P2P_GO:
		wmi_tx_cfg.cfg.ds_cfg = AP_MODE;
		break;
	default:
		rc = -EOPNOTSUPP;
		goto out_free;

	}
	rc = wmi_call(wil, &wmi_tx_cfg, WMI_VRING_CFG_DONE_EVENTID,
			&reply, sizeof(reply), 100);
	if (rc)
		goto out_free;
	if (reply.cfg.status != VRING_CFG_SUCCESS) {
		wil_err(wil, "Tx config failed, status 0x%02x\n",
				reply.cfg.status);
		goto out_free;
	}
	vring->hwtail = reply.cfg.tx_vring_tail_ptr;
	return 0;
 out_free:
	vring_free(wil, vring, 1);
 out:
	return rc;
}

void vring_fini_tx(struct wil6210_priv *wil, int id)
{
#if 0
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_VRING_CFG_CMD cfg;
	} __packed wmi_tx_cfg = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 1,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_VRING_CFG_CMD),
		},
		.wmi = {
			.id = WMI_VRING_CFG_CMDID,
			.info1 = 0,
		},
		.cfg = {
			.action = DELETE_VRING,
			.sw_ring = {
			},
			.ringid = id,
		},
	};
	struct {
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_VRING_CFG_DONE_EVENT cfg;
	} __packed reply;
#endif
	struct vring *vring = &wil->vring_tx[id];
	if (!vring->va)
		return;
	wil_info(wil, "%s(%d)\n", __func__, id);
#if 0
	wmi_call(wil, &wmi_tx_cfg, WMI_VRING_CFG_DONE_EVENTID,
			&reply, sizeof(reply), 100);
#endif
	vring_free(wil, vring, 1);
}

static struct vring *find_tx_vring(struct wil6210_priv *wil,
		struct sk_buff *skb)
{
	struct vring *v = &wil->vring_tx[0];
	if (v->va)
		return v;
	return NULL;
}

static int tx_desc_map(struct vring_tx_desc *d, dma_addr_t pa, u32 len)
{
	d->dma.addr_low = lower_32_bits(pa);
	d->dma.addr_high = (u16)upper_32_bits(pa);
	d->dma.ip_length = 0;
	/* 0..6: mac_length; 7:ip_version 0-IP6 1-IP4*/
	d->dma.b11 = 0/*14 | BIT(7)*/;
	d->dma.error = 0;
	d->dma.status = 0; /* BIT(0) should be 0 for HW_OWNED */
	d->dma.length = len;
	d->dma.d0 = 0;
	d->mac.d[0] = 0;
	d->mac.d[1] = 0;
	d->mac.d[2] = 0;
	d->mac.ucode_cmd = 0;
#if 0
	/* use MCS 1 */
	d->mac.d[0] |= BIT(MAC_CFG_DESC_TX_DESCRIPTOR_MAC_0_MCS_EN_POS) |
			(1 << MAC_CFG_DESC_TX_DESCRIPTOR_MAC_0_MCS_INDEX_POS);
#endif
#if 1
	/* use dst index 0 */
	d->mac.d[1] |= BIT(MAC_CFG_DESC_TX_DESCRIPTOR_MAC_1_DST_INDEX_EN_POS) |
			(0 << MAC_CFG_DESC_TX_DESCRIPTOR_MAC_1_DST_INDEX_POS);
#endif
#if 0
	d->mac.d[0] |= BIT(MAC_CFG_DESC_TX_DESCRIPTOR_MAC_0_STATUS_EN_POS);
	d->mac.d[0] |= BIT(MAC_CFG_DESC_TX_DESCRIPTOR_MAC_0_INTERRUP_EN_POS);
#endif
	/* translation type:  0 - bypass; 1 - 802.3; 2 - native wifi */
	d->mac.d[2] = BIT(MAC_CFG_DESC_TX_DESCRIPTOR_MAC_2_SNAP_HDR_INSERTION_EN_POS) |
			(1 << MAC_CFG_DESC_TX_DESCRIPTOR_MAC_2_L2_TRANSLATION_TYPE_POS);
	return 0;
}

static int tx_vring(struct wil6210_priv *wil, struct vring *vring,
		struct sk_buff *skb)
{
	struct device *dev = wil_to_dev(wil);
	struct vring_tx_desc *d;
	u32 swhead = vring->swhead;
	u32 swtail = vring->swtail;
	int used = (vring->size + swhead - swtail) % vring->size;
	int avail = vring->size - used - 1;
	int nr_frags = skb_shinfo(skb)->nr_frags;
	int f;
	int vring_index = vring - wil->vring_tx;
	int i = swhead;
	dma_addr_t pa;
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "%s()\n", __func__);
#endif
	if (avail < vring->size/8)
		netif_tx_stop_all_queues(wil_to_ndev(wil));
	if (avail < 1 + nr_frags) {
		wil_err(wil, "Tx ring full. No space for %d fragments\n",
				1 + nr_frags);
		return -ENOMEM;
	}
	d = &(vring->va[i].tx);

	/* FIXME FW can accept only unicast frames for the peer */
	memcpy(skb->data, wil->dst_addr[vring_index], ETH_ALEN);

	pa = dma_map_single(dev, skb->data,
			skb_headlen(skb), DMA_TO_DEVICE);
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "Tx skb %d bytes %p -> %#08llx\n",
			skb_headlen(skb), skb->data, (unsigned long long)pa);
	print_hex_dump(KERN_INFO, "Tx ", DUMP_PREFIX_OFFSET, 16, 1, skb->data
			, skb_headlen(skb), false);
#endif
	if (unlikely(dma_mapping_error(dev, pa)))
		return -EINVAL;
	/* 1-st segment */
	tx_desc_map(d, pa, skb_headlen(skb));
	d->mac.d[2] |= ((nr_frags + 1) <<
		MAC_CFG_DESC_TX_DESCRIPTOR_MAC_2_NUM_OF_DESCRIPTORS_POS);
	vring->ctx[i] = skb;
	/* middle segments */
	for (f = 0; f < nr_frags; f++) {
		const struct skb_frag_struct *frag =
				&skb_shinfo(skb)->frags[f];
		int len = skb_frag_size(frag);
		i = (swhead + f + 1) % vring->size;
		d = &(vring->va[i].tx);
		pa = skb_frag_dma_map(dev, frag, 0, skb_frag_size(frag),
				DMA_TO_DEVICE);
		if (unlikely(dma_mapping_error(dev, pa)))
			goto dma_error;
		tx_desc_map(d, pa, len);
		vring->ctx[i] = NULL;
	}
	/* for the last seg only */
	d->dma.d0 |= BIT(DMA_CFG_DESC_TX_DESCRIPTOR_DMA_0_CMD_EOP_POS);
	d->dma.d0 |= BIT(9);
	d->dma.d0 |= BIT(DMA_CFG_DESC_TX_DESCRIPTOR_DMA_0_CMD_DMA_IT_POS);
	d->dma.d0 |= (vring_index << DMA_CFG_DESC_TX_DESCRIPTOR_DMA_0_QID_POS);
#ifdef WIL_DEBUG_TXRX
	print_hex_dump(KERN_INFO, "Tx ", DUMP_PREFIX_NONE, 32, 4, d
			, sizeof(*d), false);
#endif
	/* advance swhead */
	vring->swhead = (swhead + nr_frags + 1) % vring->size;
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "Tx swhead %d -> %d\n", swhead, vring->swhead);
#endif
	iowrite32(vring->swhead, wil->csr + HOSTADDR(vring->hwtail));
	return 0;
 dma_error:
	/* unmap what we have mapped */
	for (; f > -1; f--) {
		i = (swhead + f + 1) % vring->size;
		d = &(vring->va[i].tx);
		d->dma.status = TX_DMA_STATUS_DU;
		pa = d->dma.addr_low | ((u64)d->dma.addr_high << 32);
		if (vring->ctx[i])
			dma_unmap_single(dev, pa, d->dma.length, DMA_TO_DEVICE);
		else
			dma_unmap_page(dev, pa, d->dma.length, DMA_TO_DEVICE);
	}
	return -EINVAL;
}


netdev_tx_t wil_start_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct wil6210_priv *wil = ndev_to_wil(ndev);
	struct vring *vring;
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "%s()\n", __func__);
#endif
	if (!test_bit(wil_status_fwready, &wil->status)) {
		wil_err(wil, "FW not ready\n");
		goto drop;
	}
	if (!test_bit(wil_status_fwconnected, &wil->status)) {
		wil_err(wil, "FW not connected\n");
		goto drop;
	}
	if (wil->wdev->iftype == NL80211_IFTYPE_MONITOR) {
		wil_err(wil, "Xmit in monitor mode not supported\n");
		goto drop;
	}
	/* find vring */
	vring = find_tx_vring(wil, skb);
	if (!vring) {
		wil_err(wil, "No Tx VRING available\n");
		goto drop;
	}
	/* set up vring entry */
	switch (tx_vring(wil, vring, skb)) {
	case 0:
		ndev->stats.tx_packets++;
		ndev->stats.tx_bytes += skb->len;
		return NETDEV_TX_OK;
		break;
	case -ENOMEM:
		return NETDEV_TX_BUSY;
	default:
		; /* goto drop; */
		break;
	}
 drop:
	netif_tx_stop_all_queues(ndev);
	ndev->stats.tx_dropped++;
	dev_kfree_skb_any(skb);
	return NET_XMIT_DROP;
}

/**
 * Clean up transmitted skb's from the Tx VRING
 *
 * Safe to call from IRQ
 */
void tx_complete(struct wil6210_priv *wil, int ringid)
{
	struct device *dev = wil_to_dev(wil);
	struct vring *vring = &wil->vring_tx[ringid];
	if (!vring->va) {
		wil_err(wil, "Tx irq[%d]: vring not initialized\n", ringid);
		return;
	}
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "%s(%d)\n", __func__, ringid);
#endif
	while (!vring_is_empty(vring)) {
		struct vring_tx_desc *d = &vring->va[vring->swtail].tx;
		dma_addr_t pa;
		struct sk_buff *skb;
		if (!(d->dma.status & TX_DMA_STATUS_DU))
			break;
#ifdef WIL_DEBUG_TXRX
		wil_info(wil, "Tx[%3d] : %d bytes, status 0x%02x err 0x%02x\n",
				vring->swtail, d->dma.length, d->dma.status,
				d->dma.error);
		print_hex_dump(KERN_INFO, "TxC ", DUMP_PREFIX_NONE, 32, 4, d
				, sizeof(*d), false);
#endif
		pa = d->dma.addr_low | ((u64)d->dma.addr_high << 32);
		skb = vring->ctx[vring->swtail];
		if (skb) {
			dma_unmap_single(dev, pa, d->dma.length, DMA_TO_DEVICE);
			dev_kfree_skb_any(skb);
			vring->ctx[vring->swtail] = NULL;
		} else {
			dma_unmap_page(dev, pa, d->dma.length, DMA_TO_DEVICE);
		}
		d->dma.addr_low = 0;
		d->dma.addr_high = 0;
		d->dma.length = 0;
		d->dma.status = TX_DMA_STATUS_DU;
		vring->swtail = (vring->swtail + 1) % vring->size;
	}
	{
		u32 swhead = vring->swhead;
		u32 swtail = vring->swtail;
		int used = (vring->size + swhead - swtail) % vring->size;
		int avail = vring->size - used - 1;
		if (avail > vring->size/4)
			netif_tx_wake_all_queues(wil_to_ndev(wil));
	}
}
