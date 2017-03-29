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

#ifndef __WIL6210_H__
#define __WIL6210_H__

#include <linux/netdevice.h>
#include <linux/wireless.h>
#include <net/cfg80211.h>

#define WIL_NAME "wil6210"
#define PCI_VENDOR_ID_WIL6210 (0x1ae9)
#define PCI_DEVICE_ID_WIL6210 (0x0301)

/**
 * extract bits [@b0:@b1] (inclusive) from the value @x
 * it should be @b0 <= @b1, or result is incorrect
 */
static inline u32 GET_BITS(u32 x, int b0, int b1)
{
	return (x >> b0) & ((1 << (b1 - b0 + 1)) - 1);
}

#define WIL6210_MEM_SIZE (2*1024*1024UL)

#define WIL6210_TX_QUEUES (4)

#define WIL6210_RX_RING_SIZE (128)
#define WIL6210_TX_RING_SIZE (128)
#define WIL6210_MAX_TX_RINGS (24)

struct wil6210_mbox_ring {
	u32 base;
	u16 unused;
	u16 size;
	u32 tail;
	u32 head;
} __packed;

struct wil6210_mbox_ring_desc {
	u32 sync;
	u32 addr;
} __packed;

/* at HOST_OFF_WIL6210_MBOX_CTL */
struct wil6210_mbox_ctl {
	struct wil6210_mbox_ring tx;
	struct wil6210_mbox_ring rx;
} __packed;

struct wil6210_mbox_hdr {
	u16 seq;
	u16 ctx;
	u16 type;
	u8 flags;
	u8 len; /* payload, bytes after this header */
} __packed;

/* max. value for wil6210_mbox_hdr.len */
#define MAX_MBOXITEM_SIZE   (120)

struct wil6210_mbox_hdr_wmi {
	u16 reserved0;
	u16 id;
	u16 info1;
	u16 reserved1;
} __packed;

struct pending_wmi_event {
	struct list_head list;
	struct {
		struct wil6210_mbox_hdr mbox_hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		u8 data[0];
	} __packed event;
};

union vring_desc;

struct vring {
	dma_addr_t pa;
	union vring_desc *va; /* vring_desc[size] */
	u16 size; /* number of vring_desc elements */
	u32 swtail;
	u32 swhead;
	u32 hwtail; /* write here to inform hw */
	void **ctx; /* void *ctx[size] - software context */
};

enum { /* for wil6210_priv.status */
	wil_status_fwready = 0,
	wil_status_fwconnected,
	wil_status_dontscan,
	wil_status_irqen, /* FIXME: interrupts enabled - for debug */
};

struct pci_dev;

struct wil6210_stats {
	u16 last_mcs_rx;
	u16 bf_mcs; /* last BF, used for Tx */
};

struct wil6210_priv {
	struct pci_dev *pdev;
	int n_msi;
	struct wireless_dev *wdev;
	void __iomem *csr;
	unsigned long status;
	/* profile */
	u32 monitor_flags;
	struct ieee80211_channel *monitor_chan;
	/* cached ISR registers */
	u32 isr_rx;
	u32 isr_tx;
	u32 isr_misc;
	/* mailbox related */
	struct mutex wmi_mutex;
	struct wil6210_mbox_ctl mbox_ctl;
	struct completion wmi_ready;
	u16 wmi_seq;
	u16 reply_id; /**< wait for this WMI event */
	void *reply_buf;
	u8 reply_size;
	struct workqueue_struct *wmi_wq; /* for deferred calls */
	struct work_struct wmi_event_worker;
	struct workqueue_struct *wmi_wq_conn; /* for connect worker */
	struct work_struct wmi_connect_worker;
	struct timer_list connect_timer;
	int pending_connect_cid;
	struct list_head pending_wmi_ev;
	spinlock_t wmi_ev_lock;

	/* DMA related */
	struct vring vring_rx;
	struct vring vring_tx[WIL6210_MAX_TX_RINGS];
	u8 dst_addr[WIL6210_MAX_TX_RINGS][ETH_ALEN];
	/* scan */
	struct cfg80211_scan_request *scan_request;

	struct mutex mutex;
	/* statistics */
	struct wil6210_stats stats;
	/* debugfs */
	struct dentry *debug;
	struct debugfs_blob_wrapper fw_code_blob;
	struct debugfs_blob_wrapper fw_data_blob;
	struct debugfs_blob_wrapper fw_peri_blob;
	struct debugfs_blob_wrapper uc_code_blob;
	struct debugfs_blob_wrapper uc_data_blob;
	struct debugfs_blob_wrapper rgf_blob;
};

#define wil_to_wiphy(i) (i->wdev->wiphy)
#define wil_to_dev(i) (wiphy_dev(wil_to_wiphy(i)))
#define wiphy_to_wil(w) (struct wil6210_priv *)(wiphy_priv(w))
#define wil_to_wdev(i) (i->wdev)
#define wdev_to_wil(w) (struct wil6210_priv *)(wdev_priv(w))
#define wil_to_ndev(i) (wil_to_wdev(i)->netdev)
#define ndev_to_wil(n) (wdev_to_wil(n->ieee80211_ptr))

#define wil_info(wil, fmt, arg...) netdev_info(wil_to_ndev(wil), fmt, ##arg)
#define wil_err(wil, fmt, arg...) netdev_err(wil_to_ndev(wil), fmt, ##arg)

void memcpy_fromio_32(void *dst, const volatile void __iomem *src,
		size_t count);
void memcpy_toio_32(volatile void __iomem *dst, const void *src, size_t count);

void *wil_if_alloc(struct device *dev, void __iomem *csr);
void wil_if_free(struct wil6210_priv *wil);
int wil_if_add(struct wil6210_priv *wil);
void wil_if_remove(struct wil6210_priv *wil);
int wil_priv_init(struct wil6210_priv *wil);
void wil_priv_deinit(struct wil6210_priv *wil);
int wil_reset(struct wil6210_priv *wil);
void wil_link_on(struct wil6210_priv *wil);
void wil_link_off(struct wil6210_priv *wil);
int wil_up(struct wil6210_priv *wil);
int wil_down(struct wil6210_priv *wil);

void __iomem *wmi_buffer(struct wil6210_priv *wil, u32 ptr);
void __iomem *wmi_addr(struct wil6210_priv *wil, u32 ptr);
int wmi_read_hdr(struct wil6210_priv *wil, u32 ptr,
		struct wil6210_mbox_hdr *hdr);
int wmi_send_cmd(struct wil6210_priv *wil, void *buf);
void wmi_recv_cmd(struct wil6210_priv *wil);
int wmi_call(struct wil6210_priv *wil, void *buf,
		u16 reply_id, void *reply, u8 reply_size, int to_msec);
void wmi_connect_worker(struct work_struct *work);
void wmi_event_worker(struct work_struct *work);
void wmi_event_flush(struct wil6210_priv *wil);

int wil6210_init_irq(struct wil6210_priv *wil, int irq);
void wil6210_fini_irq(struct wil6210_priv *wil, int irq);
void wil6210_disable_irq(struct wil6210_priv *wil);
void wil6210_enable_irq(struct wil6210_priv *wil);

int wil6210_debugfs_init(struct wil6210_priv *wil);
void wil6210_debugfs_remove(struct wil6210_priv *wil);

int wil6210_set_mac_address(struct wil6210_priv *wil, void *addr);
int wil6210_set_bcon(struct wil6210_priv *wil, int bi, u16 wmi_nettype);
void wil6210_disconnect(struct wil6210_priv *wil, void *bssid);

int rx_init(struct wil6210_priv *wil);
void rx_fini(struct wil6210_priv *wil);

/* TX API */
int vring_init_tx(struct wil6210_priv *wil, int id,
		int size, int cid, int tid);
void vring_fini_tx(struct wil6210_priv *wil, int id);

netdev_tx_t wil_start_xmit(struct sk_buff *skb, struct net_device *ndev);
void tx_complete(struct wil6210_priv *wil, int ringid);

/* RX API */
void rx_handle(struct wil6210_priv *wil);

int iftype_nl2wmi(enum nl80211_iftype type);

#endif /* __WIL6210_H__ */
