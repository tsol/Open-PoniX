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

#include <linux/pci.h>
#include <linux/io.h>
#include <linux/list.h>
#include <linux/etherdevice.h>

#include "wil6210.h"
#include "wil6210_rgf.h"
#include "wmi.h"

/**
 * WMI event receiving - theory of operations
 *
 * When firmware about to report WMI event, it fills memory area
 * in the mailbox and raises misc. IRQ. Thread interrupt handler invoked for
 * the misc IRQ, function @wmi_recv_cmd called by thread IRQ handler.
 *
 * @wmi_recv_cmd reads event, allocates memory chunk  and attaches it to the
 * event list @wil->pending_wmi_ev. Then, work queue @wil->wmi_wq wakes up
 * and handles events within the @wmi_event_worker. Every event get detached
 * from list, processed and deleted.
 *
 * Purpose for this mechanism is to release IRQ thread; otherwise,
 * if WMI event handling involves another WMI command flow, this 2-nd flow
 * won't be completed because of blocked IRQ thread.
 */

/**
 * Addressing - theory of operations
 *
 * There are several buses present on the WIL6210 card.
 * Same memory areas are visible at different address on
 * the different busses. There are 3 main bus masters:
 *  - MAC CPU (ucode)
 *  - User CPU (firmware)
 *  - AHB (host)
 *
 * On the PCI bus, there is one BAR (BAR0) of 2Mb size, exposing
 * AHB addresses starting from 0x880000
 *
 * Internally, firmware uses addresses that allows faster access but
 * are invisible from the host. To read from these addresses, alternative
 * AHB address must be used.
 *
 * Memory mapping
 * Linker address         PCI/Host address
 *                        0x880000 .. 0xa80000  2Mb BAR0
 * 0x800000 .. 0x807000   0x900000 .. 0x907000  28k DCCM
 * 0x840000 .. 0x857000   0x908000 .. 0x91f000  92k PERIPH
 */

/**
 * @fw_mapping provides memory remapping table
 */
static const struct {
	u32 from; /* linker address - from, inclusive */
	u32 to;   /* linker address - to, exclusive */
	u32 host; /* PCI/Host address */
} fw_mapping[] = {
	{0x000000, 0x040000, 0x8c0000}, /* FW code RAM 256k */
	{0x800000, 0x808000, 0x900000}, /* FW data RAM 32k */
	{0x840000, 0x860000, 0x908000}, /* peripheral data RAM 128k/96k used */
	{0x880000, 0x88a000, 0x880000}, /* various RGF */
	{0x8c0000, 0x92a000, 0x8c0000}, /* trivial mapping for upper area */
	/*
	 * 920000..928000 ucode code RAM
	 * 928000..92a000 ucode data RAM
	 */
};

/**
 * return AHB address for given firmware/ucode internal (linker) address
 * @x - internal address
 * If address have no valid AHB mapping, return 0
 */
static u32 wmi_addr_remap(u32 x)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(fw_mapping); i++) {
		if ((x >= fw_mapping[i].from) && (x < fw_mapping[i].to))
			return x + fw_mapping[i].host - fw_mapping[i].from;
	}
	return 0;
}

/**
 * Check address validity for WMI buffer; remap if needed
 * @ptr - internal (linker) fw/ucode address
 *
 * Valid buffer should be DWORD aligned
 *
 * return address for accessing buffer from the host;
 * if buffer is not valid, return NULL.
 */
void __iomem *wmi_buffer(struct wil6210_priv *wil, u32 ptr)
{
	u32 off;
	if (ptr % 4)
		return NULL;
	ptr = wmi_addr_remap(ptr);
	if (ptr < WIL6210_FW_HOST_OFF)
		return NULL;
	off = HOSTADDR(ptr);
	if (off > WIL6210_MEM_SIZE - 4)
		return NULL;
	return wil->csr + off;
}

/**
 * Check address validity
 */
void __iomem *wmi_addr(struct wil6210_priv *wil, u32 ptr)
{
	u32 off;
	if (ptr % 4)
		return NULL;
	if (ptr < WIL6210_FW_HOST_OFF)
		return NULL;
	off = HOSTADDR(ptr);
	if (off > WIL6210_MEM_SIZE - 4)
		return NULL;
	return wil->csr + off;
}

int wmi_read_hdr(struct wil6210_priv *wil, u32 ptr,
		struct wil6210_mbox_hdr *hdr)
{
	void __iomem *src = wmi_buffer(wil, ptr);
	if (!src)
		return -EINVAL;
	memcpy_fromio_32(hdr, src, sizeof(*hdr));
	return 0;
}

static int __wmi_send_cmd(struct wil6210_priv *wil, void *buf)
{
	struct wil6210_mbox_hdr *hdr = buf;
	struct wil6210_mbox_ring *r = &wil->mbox_ctl.tx;
	struct wil6210_mbox_ring_desc d_head;
	u32 next_head;
	void __iomem *dst;
	void __iomem *head = wmi_addr(wil, r->head);
	int retry;
	wil_info(wil, "%s()\n", __func__);
	might_sleep();
	if (!test_bit(wil_status_fwready, &wil->status)) {
		wil_err(wil, "FW not ready\n");
		return -EAGAIN;
	}
	if (!head) {
		wil_err(wil, "WMI head is garbage: 0x%08x\n", r->head);
		return -EINVAL;
	}
	/* read Tx head */
	memcpy_fromio_32(&d_head, head
			, sizeof(struct wil6210_mbox_ring_desc));
	if (d_head.sync != 0) {
		wil_err(wil, "WMI head busy\n");
		return -EBUSY;
	}
	/* next head */
	next_head = r->base + ((r->head - r->base
			+ sizeof(struct wil6210_mbox_ring_desc)) % r->size);
	wil_info(wil, "Head 0x%08x -> 0x%08x\n"
			, r->head, next_head);
	/* wait till FW finish with previous command */
	for (retry = 5; retry > 0; retry--) {
		r->tail = ioread32(wil->csr + HOST_MBOX
				+ offsetof(struct wil6210_mbox_ctl, tx.tail));
		if (next_head != r->tail)
			break;
		msleep(20);
	}
	if (next_head == r->tail) {
		wil_err(wil, "WMI ring full\n");
		return -EBUSY;
	}
	dst = wmi_buffer(wil, d_head.addr);
	if (!dst) {
		wil_err(wil, "invalid WMI buffer: 0x%08x\n", d_head.addr);
		return -EINVAL;
	}
	hdr->seq = ++wil->wmi_seq;
	/* set command */
	if ((hdr->type == 0) &&  /* TODO: define */
		(hdr->len >= sizeof(struct wil6210_mbox_hdr_wmi))) {
		struct wil6210_mbox_hdr_wmi *wmi = (void *)(&hdr[1]);
		wil_info(wil, "WMI command 0x%04x\n", wmi->id);
	}
	print_hex_dump(KERN_INFO, "cmd ", DUMP_PREFIX_OFFSET, 16, 1, buf
			, sizeof(*hdr) + hdr->len, true);
	memcpy_toio_32(dst, buf, sizeof(*hdr) + hdr->len);
	/* mark entry as full */
	iowrite32(1, wil->csr + HOSTADDR(r->head)
			+ offsetof(struct wil6210_mbox_ring_desc, sync));
	/* advance next ptr */
	iowrite32(r->head = next_head, wil->csr + HOST_MBOX
			+ offsetof(struct wil6210_mbox_ctl, tx.head));

	/* interrupt to FW */
	iowrite32(SW_INT_MBOX, wil->csr + HOST_SW_INT);

	return 0;
}

int wmi_send_cmd(struct wil6210_priv *wil, void *buf)
{
	int rc;
	mutex_lock(&wil->wmi_mutex);
	rc = __wmi_send_cmd(wil, buf);
	mutex_unlock(&wil->wmi_mutex);
	return rc;
}

/*=== Event handlers ===*/
static void wmi_evt_ready(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	struct WMI_READY_EVENT *evt = d;
	wil_info(wil, "FW ver. %d; MAC %pM\n",
			evt->sw_version, evt->macaddr);
	if (!is_valid_ether_addr(ndev->dev_addr)) {
		memcpy(ndev->dev_addr, evt->macaddr, ETH_ALEN);
		memcpy(ndev->perm_addr, evt->macaddr, ETH_ALEN);
	}
	snprintf(wdev->wiphy->fw_version, sizeof(wdev->wiphy->fw_version),
			"%d", evt->sw_version);
}

static void wmi_evt_fw_ready(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	wil_info(wil, "WMI: FW ready\n");
	set_bit(wil_status_fwready, &wil->status);
	/* reuse wmi_ready for the firmware ready indication */
	complete(&wil->wmi_ready);
}

static void wmi_evt_rx_mgmt(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	struct WMI_RX_MGMT_PACKET_EVENT *data = d;
	struct wiphy *wiphy = wil_to_wiphy(wil);
	struct ieee80211_mgmt *rx_mgmt_frame =
			(struct ieee80211_mgmt *)data->payload;
	int ch_no = data->channel+1;
	u32 freq = ieee80211_channel_to_frequency(ch_no,
			IEEE80211_BAND_60GHZ);
	struct ieee80211_channel *channel = ieee80211_get_channel(wiphy, freq);
	/* TODO convert LE to CPU */
	s32 signal = data->rssi; /* TODO */
	__le16 fc = rx_mgmt_frame->frame_control;
	wil_info(wil, "MGMT: channel %d MCS %d RSSI %d\n",
			data->channel, data->mcs, data->rssi);
	wil_info(wil, "status 0x%08x len %d\n",
			data->status, data->length);
	wil_info(wil, "qid %d mid %d cid %d\n",
			data->qid, data->mid, data->cid);
	if (!channel) {
		wil_err(wil, "Frame on unsupported channel\n");
		return;
	}
	if (ieee80211_is_beacon(fc) || ieee80211_is_probe_resp(fc)) {
		struct cfg80211_bss *bss;
		u64 tsf = rx_mgmt_frame->u.beacon.timestamp;
		u16 cap = rx_mgmt_frame->u.beacon.capab_info;
		u16 bi = rx_mgmt_frame->u.beacon.beacon_int;
		const u8 *ie_buf = rx_mgmt_frame->u.beacon.variable;
		size_t ie_len = data->length -
				offsetof(struct ieee80211_mgmt,
						u.beacon.variable);
		/* Hack to work around wrong cap. info TODO: remove when fixed*/
		if (!(cap & WLAN_CAPABILITY_ESS)) {
			wil_info(wil, "No ESS bit in capabitity. Set it.\n");
			cap |= WLAN_CAPABILITY_ESS;
		}
		if (cap & WLAN_CAPABILITY_IBSS) {
			wil_info(wil, "IBSS bit in capabitity. Clear it.\n");
			cap &= ~WLAN_CAPABILITY_IBSS;
		}
		bss = cfg80211_inform_bss(wiphy, channel, rx_mgmt_frame->bssid,
				tsf, cap, bi, ie_buf, ie_len,
				signal, GFP_KERNEL);
		if (bss) {
			wil_info(wil, "Added BSS %pM\n",
					rx_mgmt_frame->bssid);
			cfg80211_put_bss(bss);
		} else {
			wil_err(wil, "cfg80211_inform_bss() failed\n");
		}
	}
}

static void wmi_evt_scan_complete(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	if (wil->scan_request) {
		struct WMI_SCAN_COMPLETE_EVENT *data = d;
		bool aborted = (data->status != 0);
		wil_info(wil, "SCAN_COMPLETE(0x%08x)\n",
				data->status);
		cfg80211_scan_done(wil->scan_request, aborted);
		wil->scan_request = NULL;
	} else {
		wil_err(wil, "SCAN_COMPLETE while not scanning\n");
	}
}

static void wmi_evt_connect(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	struct WMI_CONNECT_EVENT *evt = d;

	if (len < sizeof(*evt)) {
		wil_err(wil, "Connect event too short : %d bytes\n", len);
		return;
	}
	if (len != sizeof(*evt) + evt->beaconIeLen + evt->assocReqLen
			+ evt->assocRespLen) {
		wil_err(wil,
			"Connect event corrupted : %d != %d + %d + %d + %d\n",
				len, (int)sizeof(*evt), evt->beaconIeLen,
				evt->assocReqLen, evt->assocRespLen);
		return;
	}
	wil_info(wil, "Connect %pM channel [%d] cid %d\n",
			evt->bssid, evt->channel, evt->cid);
	print_hex_dump(KERN_INFO, "connect AI : ", DUMP_PREFIX_OFFSET, 16, 1,
			evt->assocInfo, len - sizeof(*evt), true);
	if ((wdev->iftype == NL80211_IFTYPE_STATION) ||
			(wdev->iftype == NL80211_IFTYPE_P2P_CLIENT)) {
		/* capinfo + listen interval */
		u8 assoc_req_ie_offset = sizeof(u16) + sizeof(u16);
		/* capinfo + status code +  associd */
		u8 assoc_resp_ie_offset = sizeof(u16) + sizeof(u16) +
				sizeof(u16);
		u8 *assoc_req_ie = &evt->assocInfo[evt->beaconIeLen
				+ assoc_req_ie_offset];
		u8 *assoc_resp_ie = &evt->assocInfo[evt->beaconIeLen
				+ evt->assocReqLen + assoc_resp_ie_offset];

		u8 assoc_req_ielen = evt->assocReqLen - assoc_req_ie_offset;
		u8 assoc_resp_ielen = evt->assocRespLen - assoc_resp_ie_offset;
		if (wdev->sme_state != CFG80211_SME_CONNECTING) {
			wil_err(wil, "Not in connecting state\n");
			return;
		}
		if (evt->assocReqLen < assoc_req_ie_offset) {
			wil_info(wil, "No Assoc. Req. info\n");
			assoc_req_ie = NULL;
			assoc_req_ielen = 0;
		}
		if (evt->assocRespLen < assoc_resp_ie_offset) {
			wil_info(wil, "No Assoc. Resp. info\n");
			assoc_resp_ie = NULL;
			assoc_resp_ielen = 0;
		}
		del_timer_sync(&wil->connect_timer);
		cfg80211_connect_result(ndev, evt->bssid,
				assoc_req_ie, assoc_req_ielen,
				assoc_resp_ie, assoc_resp_ielen,
				WLAN_STATUS_SUCCESS, GFP_KERNEL);

	}
	set_bit(wil_status_fwconnected, &wil->status);

	/* FIXME FW can transmit only ucast frames to peer */
	/* FIXME real ring_id instead of hard coded 0 */
	memcpy(wil->dst_addr[0], evt->bssid, ETH_ALEN);

	wil->pending_connect_cid = evt->cid;
	queue_work(wil->wmi_wq_conn, &wil->wmi_connect_worker);
}

static void wmi_evt_disconnect(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	struct WMI_DISCONNECT_EVENT *evt = d;
	wil_info(wil, "Disconnect %pM reason %d proto %d wmi\n",
			evt->bssid,
			evt->protocolReasonStatus, evt->disconnectReason);
	wil6210_disconnect(wil, evt->bssid);
	clear_bit(wil_status_dontscan, &wil->status);
}

static void wmi_evt_notify(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	struct WMI_NOTIFY_REQ_DONE_EVENT *evt = d;
	union {
		u64 tsf;
		u32 tsf_part[2];
	} tsf;
	if (len < sizeof(*evt)) {
		wil_err(wil, "Short NOTIFY event\n");
		return;
	}
	tsf.tsf_part[0] = evt->tsf_lo;
	tsf.tsf_part[1] = evt->tsf_hi;

	wil_info(wil, "Link status, MCS %d TSF 0x%016llx\n"
			"BF status 0x%08x metric 0x%08x\n"
			"Tx Tpt %d goodput %d Rx goodput %d\n"
			"Sectors(rx:tx) my %d:%d peer %d:%d\n",
			evt->bf_mcs, tsf.tsf,
			evt->bf_status, evt->bf_metric,
			evt->tx_tpt, evt->tx_gput, evt->rx_gput,
			evt->my_rx_sector, evt->my_tx_sector,
			evt->peer_rx_sector, evt->peer_tx_sector);
	wil->stats.bf_mcs = evt->bf_mcs;
}

static const struct {
	int eventid;
	void (*handler)(struct wil6210_priv *wil, int eventid,
			void *data, int data_len);
} wmi_evt_handlers[] = {
	{WMI_READY_EVENTID,          wmi_evt_ready},
	{WMI_FW_READY_EVENTID,       wmi_evt_fw_ready},
	{WMI_RX_MGMT_PACKET_EVENTID, wmi_evt_rx_mgmt},
	{WMI_SCAN_COMPLETE_EVENTID,  wmi_evt_scan_complete},
	{WMI_CONNECT_EVENTID,        wmi_evt_connect},
	{WMI_DISCONNECT_EVENTID,     wmi_evt_disconnect},
	{WMI_NOTIFY_REQ_DONE_EVENTID,     wmi_evt_notify},
};

void wmi_recv_cmd(struct wil6210_priv *wil)
{
	struct wil6210_mbox_ring_desc d_tail;
	struct wil6210_mbox_hdr hdr;
	struct wil6210_mbox_ring *r = &wil->mbox_ctl.rx;
	struct pending_wmi_event *evt;
	u8 *cmd;
	void __iomem *src;
	unsigned long flags;
	wil_info(wil, "%s()\n", __func__);
	for (;;) {
		r->head = ioread32(wil->csr + HOST_MBOX
				+ offsetof(struct wil6210_mbox_ctl, rx.head));
		if (r->tail == r->head)
			return;
		/* read cmd from tail */
		memcpy_fromio_32(&d_tail,
				wil->csr + HOSTADDR(r->tail),
				sizeof(struct wil6210_mbox_ring_desc));
		if (d_tail.sync == 0) {
			wil_err(wil, "Mbox evt not owned by FW?\n");
			return;
		}
		if (0 != wmi_read_hdr(wil, d_tail.addr, &hdr)) {
			wil_err(wil, "Mbox evt at 0x%08x?\n",
					d_tail.addr);
			return;
		}
		src = wmi_buffer(wil, d_tail.addr)
			+ sizeof(struct wil6210_mbox_hdr);
		evt = kmalloc(ALIGN(offsetof(struct pending_wmi_event,
				event.wmi) + hdr.len, 4), GFP_KERNEL);
		if (!evt) {
			wil_err(wil, "kmalloc for WMI event (%d) failed\n",
					hdr.len);
			return;
		}
		evt->event.mbox_hdr = hdr;
		cmd = (void *)&evt->event.wmi;
		memcpy_fromio_32(cmd, src, hdr.len);
		/* mark entry as empty */
		iowrite32(0, wil->csr + HOSTADDR(r->tail)
			+ offsetof(struct wil6210_mbox_ring_desc, sync));
		/* indicate */
		wil_info(wil, "Mbox evt %04x %04x %04x %02x %02x\n",
			hdr.seq, hdr.ctx, hdr.type, hdr.flags, hdr.len);
		if ((hdr.type == 0) &&  /* TODO: define */
			(hdr.len >= sizeof(struct wil6210_mbox_hdr_wmi))) {
			wil_info(wil, "WMI event 0x%04x\n",
					evt->event.wmi.id);
		}
		print_hex_dump(KERN_INFO, "evt ", DUMP_PREFIX_OFFSET, 16, 1,
				&evt->event.mbox_hdr, sizeof(hdr) + hdr.len,
				true);
		/* advance tail */
		r->tail = r->base + ((r->tail - r->base
			+ sizeof(struct wil6210_mbox_ring_desc)) % r->size);
		iowrite32(r->tail, wil->csr + HOST_MBOX
				+ offsetof(struct wil6210_mbox_ctl, rx.tail));
		/* add to the pending list */
		spin_lock_irqsave(&wil->wmi_ev_lock, flags);
		list_add_tail(&evt->list, &wil->pending_wmi_ev);
		spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);
		{
			int q =	queue_work(wil->wmi_wq,
					&wil->wmi_event_worker);
			wil_info(wil, "queue_work -> %d\n", q);
		}
	}
}

int wmi_call(struct wil6210_priv *wil, void *buf,
		u16 reply_id, void *reply, u8 reply_size, int to_msec)
{
	int rc;
	int remain;
	struct wil6210_mbox_wmi {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
	} __packed * wmi = buf;
	mutex_lock(&wil->wmi_mutex);
	rc = __wmi_send_cmd(wil, buf);
	if (rc)
		goto out;
	wil->reply_id = reply_id;
	wil->reply_buf = reply;
	wil->reply_size = reply_size;
	remain = wait_for_completion_timeout(&wil->wmi_ready,
			msecs_to_jiffies(to_msec));
	if (0 == remain) {
		wil_err(wil, "wmi_call(0x%04x->0x%04x) timeout %d msec\n",
				wmi->wmi.id, reply_id, to_msec);
		rc = -ETIME;
	} else {
		wil_info(wil,
			"wmi_call(0x%04x->0x%04x) completed in %d msec\n",
			wmi->wmi.id, reply_id,
			to_msec - jiffies_to_msecs(remain));
	}
	wil->reply_id = 0;
	wil->reply_buf = NULL;
	wil->reply_size = 0;
 out:
	mutex_unlock(&wil->wmi_mutex);
	return rc;
}

int wil6210_set_mac_address(struct wil6210_priv *wil, void *addr)
{
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_SET_MAC_ADDRESS_CMD mac;
	} __packed cmd = {
		.hdr = {
			.seq = 1,
			.ctx = 0,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_SET_MAC_ADDRESS_CMD),
		},
		.wmi = {
			.id = WMI_SET_MAC_ADDRESS_CMDID,
			.info1 = 0,
		},
		.mac = {
		},
	};
	memcpy(cmd.mac.macaddr, addr, ETH_ALEN);
	wil_info(wil, "Set MAC %pM\n", addr);
	return wmi_send_cmd(wil, &cmd);
}

int wil6210_set_bcon(struct wil6210_priv *wil, int bi, u16 wmi_nettype)
{
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_SET_BEACON_INT_CMD bcon;
	} __packed cmd = {
		.hdr = {
			.seq = 1,
			.ctx = 0,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_SET_BEACON_INT_CMD),
		},
		.wmi = {
			.id = WMI_SET_BEACON_INT_CMDID,
			.info1 = 0,
		},
		.bcon = {
			.bcon_interval = bi,
			.network_type = wmi_nettype,
		},
	};
	return wmi_send_cmd(wil, &cmd);
}

void wmi_event_flush(struct wil6210_priv *wil)
{
	struct pending_wmi_event *evt, *t;
	wil_info(wil, "%s()\n", __func__);
	list_for_each_entry_safe(evt, t, &wil->pending_wmi_ev, list) {
		list_del(&evt->list);
		kfree(evt);
	}
}

static bool wmi_evt_call_handler(struct wil6210_priv *wil, int id,
		void *d, int len)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(wmi_evt_handlers); i++) {
		if (wmi_evt_handlers[i].eventid == id) {
			wmi_evt_handlers[i].handler(wil, id, d, len);
			return true;
		}
	}
	return false;
}

static void wmi_event_handle(struct wil6210_priv *wil,
		struct wil6210_mbox_hdr *hdr)
{
	wil_info(wil, "%s()\n", __func__);
	if ((hdr->type == 0) &&  /* TODO: define */
		(hdr->len >= sizeof(struct wil6210_mbox_hdr_wmi))) {
		struct wil6210_mbox_hdr_wmi *wmi = (void *)(&hdr[1]);
		void *evt_data = (void *)(&wmi[1]);
		/* check if someone waits for this event */
		if (wil->reply_id && wil->reply_id == wmi->id) {
			if (wil->reply_buf) {
				memcpy(wil->reply_buf, wmi,
					min(hdr->len, wil->reply_size));
			} else {
				wmi_evt_call_handler(wil, wmi->id, evt_data,
						hdr->len - sizeof(*wmi));
			}
			wil_info(wil, "Complete WMI 0x%04x\n",
					wmi->id);
			complete(&wil->wmi_ready);
			return;
		}
		/* unsolicited event */
		/* search for handler */
		if (!wmi_evt_call_handler(wil, wmi->id, evt_data,
				hdr->len - sizeof(*wmi))) {
			wil_err(wil, "Unhandled event 0x%04x\n",
					wmi->id);
		}
	} else {
		wil_err(wil, "Unknown event type\n");
		print_hex_dump(KERN_INFO, "evt?? ", DUMP_PREFIX_OFFSET, 16, 1,
				hdr, sizeof(*hdr) + hdr->len, true);
	}
}

static struct list_head *next_wmi_ev(struct wil6210_priv *wil)
{
	unsigned long flags;
	struct list_head *ret = NULL;
	spin_lock_irqsave(&wil->wmi_ev_lock, flags);
	if (!list_empty(&wil->pending_wmi_ev)) {
		ret = wil->pending_wmi_ev.next;
		list_del(ret);
	}
	spin_unlock_irqrestore(&wil->wmi_ev_lock, flags);
	return ret;
}

void wmi_event_worker(struct work_struct *work)
{
	struct wil6210_priv *wil = container_of(work,
			struct wil6210_priv, wmi_event_worker);
	struct pending_wmi_event *evt;
	struct list_head *lh;
	wil_info(wil, "%s()\n", __func__);
	while ((lh = next_wmi_ev(wil)) != NULL) {
		evt = list_entry(lh, struct pending_wmi_event, list);
		wmi_event_handle(wil, &evt->event.mbox_hdr);
		kfree(evt);
	}
	wil_info(wil, "%s() done\n", __func__);
}

void wmi_connect_worker(struct work_struct *work)
{
	int rc;
	struct wil6210_priv *wil = container_of(work,
			struct wil6210_priv, wmi_connect_worker);
	if (wil->pending_connect_cid < 0) {
		wil_err(wil, "No connection pending\n");
		return;
	}
	wil_info(wil, "Configure for connection CID %d\n",
			wil->pending_connect_cid);
	rc = vring_init_tx(wil, 0, WIL6210_TX_RING_SIZE,
			wil->pending_connect_cid, 0);
	wil->pending_connect_cid = -1;
	if (rc == 0)
		wil_link_on(wil);
}
