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
#include <linux/sched.h>
#include <linux/etherdevice.h>
#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <net/cfg80211.h>

#include "wil6210.h"
#include "cfg80211.h"
#include "wmi.h"

#define CHAN60G(_channel, _flags) {				\
	.band			= IEEE80211_BAND_60GHZ,		\
	.center_freq		= 56160 + (2160 * (_channel)),	\
	.hw_value		= (_channel),			\
	.flags			= (_flags),			\
	.max_antenna_gain	= 0,				\
	.max_power		= 40,				\
}

static struct ieee80211_channel wil_60ghz_channels[] = {
	CHAN60G(1, 0),
	CHAN60G(2, 0),
	CHAN60G(3, 0),
#if 0
	CHAN60G(4, 0),
#endif
};

static struct ieee80211_supported_band wil_band_60ghz = {
	.channels = wil_60ghz_channels,
	.n_channels = ARRAY_SIZE(wil_60ghz_channels),
	.ht_cap = {
		.ht_supported = true,
		.cap = 0, /* TODO */
		.ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K, /* TODO */
		.ampdu_density = IEEE80211_HT_MPDU_DENSITY_8, /* TODO */
		.mcs = {
			.rx_mask = {0xfe, 0xff, 0xff, 0x0f}, /* 1..27 */
			.tx_params = IEEE80211_HT_MCS_TX_DEFINED, /* TODO */
		},
	},
};

static const struct ieee80211_txrx_stypes
wil_mgmt_stypes[NUM_NL80211_IFTYPES] = {
	[NL80211_IFTYPE_STATION] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_CLIENT] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
	[NL80211_IFTYPE_P2P_GO] = {
		.tx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_RESP >> 4),
		.rx = BIT(IEEE80211_STYPE_ACTION >> 4) |
		BIT(IEEE80211_STYPE_PROBE_REQ >> 4)
	},
};



int iftype_nl2wmi(enum nl80211_iftype type)
{
	static const struct {
		enum nl80211_iftype nl;
		enum WMI_NETWORK_TYPE wmi;
	} __nl2wmi[] = {
		{NL80211_IFTYPE_ADHOC,        ADHOC_NETWORK},
		{NL80211_IFTYPE_STATION,      INFRA_NETWORK},
		{NL80211_IFTYPE_AP,           AP_NETWORK},
		{NL80211_IFTYPE_P2P_CLIENT,   P2P_NETWORK},
		{NL80211_IFTYPE_P2P_GO,       P2P_NETWORK},
		{NL80211_IFTYPE_MONITOR,      ADHOC_NETWORK}, /* FIXME */
	#if 0
		{NL80211_IFTYPE_AP_VLAN,      0},
		{NL80211_IFTYPE_WDS,          0},
		{NL80211_IFTYPE_MESH_POINT,   0},
		    INFRA_NETWORK       = 0x01,
		    ADHOC_NETWORK       = 0x02,
		    ADHOC_CREATOR       = 0x04,
		    AP_NETWORK          = 0x10,
		    P2P_NETWORK         = 0x20,
		    WBE_NETWORK         = 0x40,
	#endif
	};
	int i;
	for (i = 0; i < ARRAY_SIZE(__nl2wmi); i++) {
		if (__nl2wmi[i].nl == type)
			return __nl2wmi[i].wmi;
	}
	return -EOPNOTSUPP;
}

static int wil_cfg80211_get_station(struct wiphy *wiphy,
				    struct net_device *ndev,
				    u8 *mac, struct station_info *sinfo)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	int rc;
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_NOTIFY_REQ_CMD cmd;
	} __packed wmi = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 1,
			.len = sizeof(wmi.wmi)
			+ sizeof(wmi.cmd),
		},
		.wmi = {
			.id = WMI_NOTIFY_REQ_CMDID,
			.info1 = 0,
		},
		.cmd = {
			.cid = 0,
			.interval_usec = 0,
		},
	};

#if 0
	wil_info(wil, "%s(%pM)\n", __func__, mac);
#endif
	if (memcmp(mac, wil->dst_addr[0], ETH_ALEN))
		return -ENOENT;
	rc = wmi_call(wil, &wmi, WMI_NOTIFY_REQ_DONE_EVENTID, NULL, 0, 20);
	if (rc)
		return rc;

	sinfo->filled |= STATION_INFO_TX_BITRATE;
	sinfo->txrate.flags = RATE_INFO_FLAGS_MCS | RATE_INFO_FLAGS_60G;
	sinfo->txrate.mcs = wil->stats.bf_mcs;
	sinfo->filled |= STATION_INFO_RX_BITRATE;
	sinfo->rxrate.flags = RATE_INFO_FLAGS_MCS | RATE_INFO_FLAGS_60G;
	sinfo->rxrate.mcs = wil->stats.last_mcs_rx;

	if (test_bit(wil_status_fwconnected, &wil->status)) {
		sinfo->filled |= STATION_INFO_SIGNAL;
		sinfo->signal = 12; /* TODO: provide real number */
	}

	return 0;
}

static int wil_cfg80211_change_iface(struct wiphy *wiphy,
				     struct net_device *ndev,
				     enum nl80211_iftype type, u32 *flags,
				     struct vif_params *params)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	struct wireless_dev *wdev = wil->wdev;
	wil_info(wil, "%s()\n", __func__);

	switch (type) {
	case NL80211_IFTYPE_STATION:
		wil_info(wil, "type: STATION\n");
		break;
	case NL80211_IFTYPE_AP:
		wil_info(wil, "type: AP\n");
		break;
	case NL80211_IFTYPE_P2P_CLIENT:
		wil_info(wil, "type: P2P_CLIENT\n");
		break;
	case NL80211_IFTYPE_P2P_GO:
		wil_info(wil, "type: P2P_GO\n");
		break;
	case NL80211_IFTYPE_MONITOR:
		wil_info(wil, "type: Monitor\n");
		if (flags) {
			wil_info(wil, "Monitor flags: 0x%08x\n", *flags);
			wil->monitor_flags = *flags;
		} else {
			wil->monitor_flags = 0;
		}
		break;
	default:
		return -EOPNOTSUPP;
	}
	wdev->iftype = type;
	return 0;
}

static int wil_cfg80211_scan(struct wiphy *wiphy, struct net_device *ndev,
			     struct cfg80211_scan_request *request)
{
#if 0
	int rc;
#endif
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	struct wireless_dev *wdev = wil->wdev;
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_START_SCAN_CMD scan;
		u16 channels[4];
	} __packed wmi_scan = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_START_SCAN_CMD),
		},
		.wmi = {
			.id = WMI_START_SCAN_CMDID,
			.info1 = 0,
		},
		.scan = {
		},
	};
#if 0
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_SET_BSS_FILTER_CMD filter;
	} __packed wmi_bss_filter = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_SET_BSS_FILTER_CMD),
		},
		.wmi = {
			.id = WMI_SET_BSS_FILTER_CMDID,
			.info1 = 0,
		},
		.filter = {
			.bssFilter = ALL_BSS_FILTER,
		},
	};
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_SET_SCAN_PARAMS_CMD params;
	} __packed wmi_scan_params = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_SET_BSS_FILTER_CMD),
		},
		.wmi = {
			.id = WMI_SET_SCAN_PARAMS_CMDID,
			.info1 = 0,
		},
		.params = {
			.fg_start_period = 0,        /* seconds */
			.fg_end_period = 0,          /* seconds */
			.bg_period = 0,              /* seconds */
			.maxact_chdwell_time = 0,    /* msec */
			.pas_chdwell_time = 0,       /* msec */
			.shortScanRatio = 0,         /* how many short scans */
						/* for one long */
			.scanCtrlFlags = DEFAULT_SCAN_CTRL_FLAGS,
			.minact_chdwell_time = 0,    /* msec */
			.maxact_scan_per_ssid = 0,   /* max active scans */
						/* per ssid */
			.max_dfsch_act_time = 0,  /* msecs */
		},
	};
#endif
	int i, n;
	wil_info(wil, "%s()\n", __func__);
	if (wil->scan_request) {
		wil_err(wil, "Already scanning\n");
		return -EAGAIN;
	}
	/* check we are client side */
	switch (wdev->iftype) {
	case NL80211_IFTYPE_STATION:
	case NL80211_IFTYPE_P2P_CLIENT:
		break;
	default:
		return -EOPNOTSUPP;

	}
	/**
	 * FW don't support scan after connection attempt
	 */
	if (test_bit(wil_status_dontscan, &wil->status)) {
		wil_err(wil, "Scan after connect attempt not supported\n");
		return -EBUSY;
	}

	wil->scan_request = request;
	wmi_scan.scan.forceFgScan = 0;
	wmi_scan.scan.isLegacy = 0;
	wmi_scan.scan.homeDwellTime = 0;
	wmi_scan.scan.forceScanInterval = 0;
	wmi_scan.scan.scanType = 0;
	wmi_scan.scan.numChannels = 0;
	n = min(request->n_channels, 4U);
	for (i = 0; i < n; i++) {
		int ch = ieee80211_frequency_to_channel(
				request->channels[i]->center_freq);
		if (ch == 0) {
			wil_err(wil,
				"Scan requested for unknown frequency %dMhz\n",
				request->channels[i]->center_freq);
			continue;
		}
		/* 0-based index */
		/* TODO convert CPU to LE */
		wmi_scan.scan.channelList[wmi_scan.scan.numChannels++] = ch-1;
		wil_info(wil, "Scan for ch %d  : %d MHz\n", ch,
				request->channels[i]->center_freq);
	}
	wmi_scan.hdr.len += wmi_scan.scan.numChannels *
			sizeof(wmi_scan.scan.channelList[0]);
#if 0
	rc = wmi_send_cmd(wil, &wmi_bss_filter);
	if (rc)
		return rc;
	rc = wmi_send_cmd(wil, &wmi_scan_params);
	if (rc)
		return rc;
#endif
	return wmi_send_cmd(wil, &wmi_scan);
}

static int wil_cfg80211_connect(struct wiphy *wiphy, struct net_device *ndev,
				 struct cfg80211_connect_params *sme)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	struct wireless_dev *wdev = wil->wdev;
	struct cfg80211_bss *bss;
	const u8 *ssid_eid;
	int rc = 0;
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
		struct WMI_CONNECT_CMD conn;
	} __packed wmi_conn = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi)
			+ sizeof(struct WMI_CONNECT_CMD),
		},
		.wmi = {
			.id = WMI_CONNECT_CMDID,
			.info1 = 0,
		},
		.conn = {
		},
	};
	wil_info(wil, "%s()\n", __func__);
	if (sme->channel)
		wil_info(wil, "Channel : %d MHz\n",
				sme->channel->center_freq);
	if (sme->bssid)
		wil_info(wil, "BSSID   : %pM\n", sme->bssid);
	if (sme->ssid && sme->ssid_len)
		print_hex_dump(KERN_INFO, "SSID : ", DUMP_PREFIX_NONE, 16, 1,
				sme->ssid, sme->ssid_len, true);
	bss = cfg80211_get_bss(wiphy, sme->channel, sme->bssid,
			sme->ssid, sme->ssid_len,
			WLAN_CAPABILITY_ESS, WLAN_CAPABILITY_ESS);
	if (!bss) {
		wil_err(wil, "Unable to find BSS\n");
		return -ENOENT;
	}
	ssid_eid = ieee80211_bss_get_ie(bss, WLAN_EID_SSID);
	if (!ssid_eid) {
		wil_err(wil, "No SSID\n");
		rc = -ENOENT;
		goto out;
	}
	wmi_conn.conn.networkType = iftype_nl2wmi(wdev->iftype);
	/* FIXME Firmware works now in PBSS mode(ToDS=0, FromDS=0) */
	wmi_conn.conn.networkType = iftype_nl2wmi(NL80211_IFTYPE_ADHOC);
	wmi_conn.conn.dot11AuthMode = OPEN_AUTH; /* TODO: crypto flow */
	wmi_conn.conn.authMode = NONE_AUTH; /* TODO: crypto flow */
	/* wmi_conn.conn.pairwiseCryptoType; */
	/* wmi_conn.conn.pairwiseCryptoLen; */
	wmi_conn.conn.groupCryptoType = NONE_CRYPT; /* TODO: crypto flow */
	/* wmi_conn.conn.groupCryptoLen; */
	wmi_conn.conn.ssidLength = min_t(u8, ssid_eid[1], 32);
	memcpy(wmi_conn.conn.ssid, ssid_eid+2, wmi_conn.conn.ssidLength);
	{
		int ch = ieee80211_frequency_to_channel(
				bss->channel->center_freq);
		if (ch == 0) {
			wil_err(wil, "BSS at unknown frequency %dMhz\n",
				bss->channel->center_freq);
			rc = -EOPNOTSUPP;
			goto out;
		}
		wmi_conn.conn.channel = ch - 1;
	}
	memcpy(wmi_conn.conn.bssid, bss->bssid, 6);
	wmi_conn.conn.ctrl_flags = 0; /* TODO: set real value */
	memcpy(wmi_conn.conn.destMacAddr, bss->bssid, 6);
	/**
	 * FW don't support scan after connection attempt
	 */
	set_bit(wil_status_dontscan, &wil->status);
	rc = wmi_send_cmd(wil, &wmi_conn);
	if (rc == 0) {
		/* Connect can take lots of time */
		mod_timer(&wil->connect_timer,
				jiffies + msecs_to_jiffies(2000));
	}
 out:
	cfg80211_put_bss(bss);
	return rc;
}

static int wil_cfg80211_disconnect(struct wiphy *wiphy,
		struct net_device *ndev,
		u16 reason_code)
{
	int rc;
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	struct {
		struct wil6210_mbox_hdr hdr;
		struct wil6210_mbox_hdr_wmi wmi;
	} __packed wmi_conn = {
		.hdr = {
			.seq = 1,
			.ctx = 1,
			.type = 0,
			.flags = 0,
			.len = sizeof(struct wil6210_mbox_hdr_wmi),
		},
		.wmi = {
			.id = WMI_DISCONNECT_CMDID,
			.info1 = 0,
		},
	};
	wil_info(wil, "%s()\n", __func__);
	rc = wmi_send_cmd(wil, &wmi_conn);
	return rc;
}

static int wil_cfg80211_set_txpower(struct wiphy *wiphy,
		enum nl80211_tx_power_setting type, int mbm)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	wil_info(wil, "%s()\n", __func__);
	return 0;
}

static int wil_cfg80211_get_txpower(struct wiphy *wiphy, int *dbm)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	wil_info(wil, "%s()\n", __func__);

	*dbm = 43;

	return 0;
}

static int wil_mgmt_tx(struct wiphy *wiphy, struct net_device *ndev,
		struct ieee80211_channel *chan, bool offchan,
		enum nl80211_channel_type channel_type,
		bool channel_type_valid, unsigned int wait,
		const u8 *buf, size_t len, bool no_cck,
		bool dont_wait_for_ack,
		u64 *cookie)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	wil_info(wil, "%s()\n", __func__);
	print_hex_dump(KERN_INFO, "mgmt_tx ", DUMP_PREFIX_OFFSET, 16, 1,
			buf, len, true);
	return 0;
}

static void wil_mgmt_frame_register(struct wiphy *wiphy,
		struct net_device *ndev, u16 frame_type, bool reg)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	wil_info(wil, "%s()\n", __func__);
	wil_info(wil, "frame_type = 0x%04x, reg = %d\n", frame_type, reg);
}

static int wil_set_monitor_channel(struct wiphy *wiphy,
#if defined(OLD_SET_CHANNEL_API)
		struct net_device *dev,
#endif
		struct ieee80211_channel *chan,
		enum nl80211_channel_type channel_type)
{
	struct wil6210_priv *wil = wiphy_to_wil(wiphy);
	wil_info(wil, "%s()\n", __func__);
	wil_info(wil, "freq = %d\n", chan->center_freq);
	wil->monitor_chan = chan;
	return 0;
}

static struct cfg80211_ops wil_cfg80211_ops = {
	.scan = wil_cfg80211_scan,
	.connect = wil_cfg80211_connect,
	.disconnect = wil_cfg80211_disconnect,
	.set_tx_power = wil_cfg80211_set_txpower,
	.get_tx_power = wil_cfg80211_get_txpower,
	.change_virtual_intf = wil_cfg80211_change_iface,
	.get_station = wil_cfg80211_get_station,
	.mgmt_tx = wil_mgmt_tx,
	.mgmt_frame_register = wil_mgmt_frame_register,
#if defined(OLD_SET_CHANNEL_API)
	.set_channel = wil_set_monitor_channel,
#else
	.set_monitor_channel = wil_set_monitor_channel,
#endif /* defined(NEW_SET_CHANNEL_API) */
};

static const u32 cipher_suites[] = {
	WLAN_CIPHER_SUITE_CCMP, /* keep for debug, TODO: remove */
	WLAN_CIPHER_SUITE_GCMP,
};

static void wil_wiphy_init(struct wiphy *wiphy)
{
	/* TODO: figure this out */
	wiphy->max_scan_ssids = 10;
#if 0
	wiphy->max_num_pmkids = UMAC_MAX_NUM_PMKIDS;
#endif
	wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
				 BIT(NL80211_IFTYPE_AP) |
				 BIT(NL80211_IFTYPE_P2P_CLIENT) |
				 BIT(NL80211_IFTYPE_P2P_GO) |
				 BIT(NL80211_IFTYPE_MONITOR);

	wiphy->bands[IEEE80211_BAND_60GHZ] = &wil_band_60ghz;

	/* TODO: figure this out */
	wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

	wiphy->cipher_suites = cipher_suites;
	wiphy->n_cipher_suites = ARRAY_SIZE(cipher_suites);
	wiphy->mgmt_stypes = wil_mgmt_stypes;
}

struct wireless_dev *wil_cfg80211_init(struct device *dev)
{
	int r = 0;
	struct wireless_dev *wdev;

	wdev = kzalloc(sizeof(struct wireless_dev), GFP_KERNEL);
	if (!wdev)
		return ERR_PTR(-ENOMEM);

	wdev->wiphy = wiphy_new(&wil_cfg80211_ops,
				sizeof(struct wil6210_priv));
	if (!wdev->wiphy) {
		r = -ENOMEM;
		goto out;
	}

	set_wiphy_dev(wdev->wiphy, dev);
	wil_wiphy_init(wdev->wiphy);
	r = wiphy_register(wdev->wiphy);

	if (r < 0)
		goto out_failed_reg;

	return wdev;

out_failed_reg:
	wiphy_free(wdev->wiphy);
out:
	kfree(wdev);
	return ERR_PTR(r);
}

void wil_wdev_free(struct wil6210_priv *wil)
{
	struct wireless_dev *wdev = wil_to_wdev(wil);
	struct device *dev = wil_to_dev(wil);
	dev_info(dev, "%s()\n", __func__);

	if (!wdev)
		return;

	wiphy_unregister(wdev->wiphy);
	wiphy_free(wdev->wiphy);
	kfree(wdev);
}
