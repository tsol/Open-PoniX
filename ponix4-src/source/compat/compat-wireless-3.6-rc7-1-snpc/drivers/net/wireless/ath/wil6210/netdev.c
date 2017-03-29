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

#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/slab.h>

#include "wil6210.h"
#include "cfg80211.h"

static int wil_open(struct net_device *ndev)
{
	struct wil6210_priv *wil = ndev_to_wil(ndev);
	wil_info(wil, "%s()\n", __func__);

	return wil_up(wil);
}

static int wil_stop(struct net_device *ndev)
{
	struct wil6210_priv *wil = ndev_to_wil(ndev);
	wil_info(wil, "%s()\n", __func__);

	return wil_down(wil);
}

/*
 * AC to queue mapping
 *
 * AC_VO -> queue 3
 * AC_VI -> queue 2
 * AC_BE -> queue 1
 * AC_BK -> queue 0
 */
static const u16 wil_1d_to_queue[8] = { 1, 0, 0, 1, 2, 2, 3, 3 };
#if 0
static int wil_tid_to_queue(u16 tid)
{
	if (tid >= ARRAY_SIZE(wil_1d_to_queue))
		return -EINVAL;

	return wil_1d_to_queue[tid];
}
#endif
static u16 wil_select_queue(struct net_device *ndev, struct sk_buff *skb)
{
#ifdef WIL_DEBUG_TXRX
	struct wil6210_priv *wil = ndev_to_wil(ndev);
#endif
	u16 ret;
	skb->priority = cfg80211_classify8021d(skb);

	ret = wil_1d_to_queue[skb->priority];
#ifdef WIL_DEBUG_TXRX
	wil_info(wil, "%s() %d -> %d\n", __func__,
			(int)skb->priority, (int)ret);
#endif
	return ret;
}

static const struct net_device_ops wil_netdev_ops = {
	.ndo_open		= wil_open,
	.ndo_stop		= wil_stop,
	.ndo_start_xmit		= wil_start_xmit,
	.ndo_select_queue	= wil_select_queue,
	.ndo_set_mac_address    = eth_mac_addr,
	.ndo_validate_addr      = eth_validate_addr,
};

void *wil_if_alloc(struct device *dev, void __iomem *csr)
{
	struct net_device *ndev;
	struct wireless_dev *wdev;
	struct wil6210_priv *wil;
	int ret = 0;
	wdev = wil_cfg80211_init(dev);
	if (IS_ERR(wdev)) {
		dev_err(dev, "wil_cfg80211_init failed\n");
		return wdev;
	}

	wil = wdev_to_wil(wdev);
	wil->csr = csr;
	wil->wdev = wdev;

	ret = wil_priv_init(wil);
	if (ret) {
		dev_err(dev, "wil_priv_init failed\n");
		goto out_wdev;
	}

	wdev->iftype = NL80211_IFTYPE_STATION; /* TODO */
	/* default monitor channel */
	wil->monitor_chan = wdev->wiphy->bands[IEEE80211_BAND_60GHZ]->channels;

	ndev = alloc_netdev_mqs(0, "wlan%d", ether_setup, WIL6210_TX_QUEUES, 1);
	if (!ndev) {
		dev_err(dev, "alloc_netdev_mqs failed\n");
		ret = -ENOMEM;
		goto out_priv;
	}

	ndev->netdev_ops = &wil_netdev_ops;
	ndev->ieee80211_ptr = wdev;
	SET_NETDEV_DEV(ndev, wiphy_dev(wdev->wiphy));
	wdev->netdev = ndev;

	wil_link_off(wil);
	return wil;

/*out_profile:*/
	free_netdev(ndev);

 out_priv:
	wil_priv_deinit(wil);

 out_wdev:
	wil_wdev_free(wil);
	return ERR_PTR(ret);
}

void wil_if_free(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	if (!ndev)
		return;

	free_netdev(ndev);
	wil_priv_deinit(wil);
	wil_wdev_free(wil);
}

int wil_if_add(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	int ret;

	ret = register_netdev(ndev);
	if (ret < 0) {
		dev_err(&ndev->dev, "Failed to register netdev: %d\n", ret);
		return ret;
	}
	wil_link_off(wil);

	return 0;
}

void wil_if_remove(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	unregister_netdev(ndev);
}
