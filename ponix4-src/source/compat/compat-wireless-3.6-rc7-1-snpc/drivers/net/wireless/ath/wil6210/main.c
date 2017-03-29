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
#include <linux/ieee80211.h>
#include <linux/wireless.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/if_arp.h>

#include "wil6210.h"
#include "wil6210_rgf.h"

/**
 * We have to read/write to/from NIC in 32-bit chunks;
 * otherwise it is not work on 64-bit platform
 */
void memcpy_fromio_32(void *dst, const volatile void __iomem *src, size_t count)
{
	u32 *d = dst;
	const volatile u32 __iomem *s = src;
	/* size_t is unsigned, if (count%4 != 0) it will wrap */
	for (count += 4; count > 4; count -= 4)
		*d++ = readl(s++);
}

void memcpy_toio_32(volatile void __iomem *dst, const void *src, size_t count)
{
	volatile u32 __iomem *d = dst;
	const u32 *s = src;
	for (count += 4; count > 4; count -= 4)
		writel(*s++, d++);
}

/* debug */
void send_echo(struct wil6210_priv *wil);

static void _wil6210_disconnect(struct wil6210_priv *wil, void *bssid)
{
	int i;
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	wil_info(wil, "%s()\n", __func__);
	wil_link_off(wil);
	clear_bit(wil_status_fwconnected, &wil->status);
	switch (wdev->sme_state) {
	case CFG80211_SME_CONNECTED:
		cfg80211_disconnected(ndev, WLAN_STATUS_UNSPECIFIED_FAILURE,
				NULL, 0, GFP_KERNEL);
		break;
	case CFG80211_SME_CONNECTING:
		cfg80211_connect_result(ndev, bssid, NULL, 0, NULL, 0,
				WLAN_STATUS_UNSPECIFIED_FAILURE, GFP_KERNEL);
		break;
	default:
		;
	}
	for (i = 0; i < ARRAY_SIZE(wil->vring_tx); i++)
		vring_fini_tx(wil, i);
}

static void connect_timer_fn(unsigned long x)
{
	struct wil6210_priv *wil = (void *)x;
	wil_info(wil, "Connect timeout\n");
	_wil6210_disconnect(wil, NULL);
}

int wil_priv_init(struct wil6210_priv *wil)
{
	wil_info(wil, "%s()\n", __func__);

	mutex_init(&wil->mutex);
	mutex_init(&wil->wmi_mutex);
	init_completion(&wil->wmi_ready);
	wil->pending_connect_cid = -1;
	setup_timer(&wil->connect_timer, connect_timer_fn,
			(unsigned long)wil);
	INIT_WORK(&wil->wmi_connect_worker, wmi_connect_worker);
	INIT_WORK(&wil->wmi_event_worker, wmi_event_worker);
	INIT_LIST_HEAD(&wil->pending_wmi_ev);
	spin_lock_init(&wil->wmi_ev_lock);
	wil->wmi_wq = create_singlethread_workqueue(WIL_NAME"_wmi");
	if (!wil->wmi_wq)
		return -EAGAIN;
	wil->wmi_wq_conn = create_singlethread_workqueue(WIL_NAME"_connect");
	if (!wil->wmi_wq_conn) {
		destroy_workqueue(wil->wmi_wq);
		return -EAGAIN;
	}
	/* make shadow copy of registers that should not change on run time */
	memcpy_fromio_32(&wil->mbox_ctl, wil->csr + HOST_MBOX,
			sizeof(struct wil6210_mbox_ctl));
	return 0;
}

void wil6210_disconnect(struct wil6210_priv *wil, void *bssid)
{
	del_timer_sync(&wil->connect_timer);
	_wil6210_disconnect(wil, bssid);
}

void wil_priv_deinit(struct wil6210_priv *wil)
{
	wil6210_disconnect(wil, NULL);
	wmi_event_flush(wil);
	destroy_workqueue(wil->wmi_wq_conn);
	destroy_workqueue(wil->wmi_wq);
}

static void wil_target_reset(struct wil6210_priv *wil)
{
	u32 x;
	u32 off;
	wil_info(wil, "Resetting...\n");
	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_MASK_0);
	x = ioread32(wil->csr + off);
	x |= BIT(6); /* hpal_perst_from_pad_src_n_mask */
	iowrite32(x, wil->csr + off);
	x |= BIT(7); /* car_perst_rst_src_n_mask */
	iowrite32(x, wil->csr + off);

	off = HOSTADDR(RGF_USER_MAC_CPU_0);
	iowrite32(BIT(1), wil->csr + off); /* mac_cpu_man_rst */

	off = HOSTADDR(RGF_USER_USER_CPU_0);
	iowrite32(BIT(1), wil->csr + off); /* user_cpu_man_rst */

	msleep(100);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_2);
	iowrite32(0xFE000000, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_1);
	iowrite32(0x0000003F, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_3);
	iowrite32(0x00000170, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_0);
	iowrite32(0xFFE7FC00, wil->csr + off);

	msleep(100);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_3);
	iowrite32(0, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_2);
	iowrite32(0, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_1);
	iowrite32(0, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_0);
	iowrite32(0, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_3);
	iowrite32(0x00000001, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_2);
	iowrite32(0x00000080, wil->csr + off);

	off = HOSTADDR(RGF_USER_CLKS_CTL_SW_RST_VEC_0);
	iowrite32(0, wil->csr + off);

	msleep(2000);

	off = HOSTADDR(RGF_USER_USER_CPU_0);
	iowrite32(BIT(0), wil->csr + off); /* user_cpu_man_de_rst */

	msleep(2000);
	wil_info(wil, "Reset completed\n");
}

/*
 * We reset all the structures, and we reset the UMAC.
 * After calling this routine, you're expected to reload
 * the firmware.
 */
int wil_reset(struct wil6210_priv *wil)
{
	wil_info(wil, "%s()\n", __func__);
	wil6210_disconnect(wil, NULL);
	wmi_event_flush(wil);
	flush_workqueue(wil->wmi_wq);
	flush_workqueue(wil->wmi_wq_conn);
	wil6210_disable_irq(wil);
	wil->status = 0;
	/* TODO: put MAC in reset */
	wil_target_reset(wil);
	/* init after reset */
	wil->pending_connect_cid = -1;
	INIT_COMPLETION(wil->wmi_ready);
	/* make shadow copy of registers that should not change on run time */
	memcpy_fromio_32(&wil->mbox_ctl, wil->csr + HOST_MBOX,
			sizeof(struct wil6210_mbox_ctl));
	/* TODO: release MAC reset */
	wil6210_enable_irq(wil);
	/* we just started MAC, wait for FW ready */
	{
		unsigned long to = msecs_to_jiffies(1000);
		unsigned long left = wait_for_completion_timeout(
				&wil->wmi_ready, to);
		if (0 == left) {
			wil_err(wil, "Firmware not ready\n");
			return -ETIME;
		} else {
			wil_info(wil, "FW ready after %d ms\n",
					jiffies_to_msecs(to-left));
		}
	}
	return 0;
}


void wil_link_on(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	wil_info(wil, "%s()\n", __func__);
	netif_carrier_on(ndev);
	netif_tx_wake_all_queues(ndev);
}

void wil_link_off(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	wil_info(wil, "%s()\n", __func__);
	netif_tx_stop_all_queues(ndev);
	netif_carrier_off(ndev);
}

static int __wil_up(struct wil6210_priv *wil)
{
	struct net_device *ndev = wil_to_ndev(wil);
	struct wireless_dev *wdev = wil->wdev;
	int rc = wil_reset(wil);
	if (rc)
		return rc;
	/* Apply profile in the following order: */
	/* MAC address - pre-requisite for other commands */
	wil6210_set_mac_address(wil, ndev->dev_addr);
	/* Interface type. Set up beaconing if required. After MAC */
	{
		u16 wmi_nettype = iftype_nl2wmi(wdev->iftype);
		int bi;
		/* FIXME Firmware works now in PBSS mode(ToDS=0, FromDS=0) */
		wmi_nettype = iftype_nl2wmi(NL80211_IFTYPE_ADHOC);
		switch (wdev->iftype) {
		case NL80211_IFTYPE_STATION:
			wil_info(wil, "type: STATION\n");
			bi = 0;
			ndev->type = ARPHRD_ETHER;
			break;
		case NL80211_IFTYPE_AP:
			wil_info(wil, "type: AP\n");
			bi = 100;
			ndev->type = ARPHRD_ETHER;
			break;
		case NL80211_IFTYPE_P2P_CLIENT:
			wil_info(wil, "type: P2P_CLIENT\n");
			bi = 0;
			ndev->type = ARPHRD_ETHER;
			break;
		case NL80211_IFTYPE_P2P_GO:
			wil_info(wil, "type: P2P_GO\n");
			bi = 100;
			ndev->type = ARPHRD_ETHER;
			break;
		case NL80211_IFTYPE_MONITOR:
			wil_info(wil, "type: Monitor\n");
			bi = 0;
			ndev->type = ARPHRD_IEEE80211_RADIOTAP;
			/* ARPHRD_IEEE80211 or ARPHRD_IEEE80211_RADIOTAP ? */
			break;
		default:
			return -EOPNOTSUPP;
		}
		rc = wil6210_set_bcon(wil, bi, wmi_nettype);
		if (rc)
			return rc;
	}
	/* Rx VRING. After MAC and beacon */
	rx_init(wil);
	return 0;
}

int wil_up(struct wil6210_priv *wil)
{
	int ret;
	wil_info(wil, "%s()\n", __func__);

	mutex_lock(&wil->mutex);
	ret = __wil_up(wil);
	mutex_unlock(&wil->mutex);

	return ret;
}

static int __wil_down(struct wil6210_priv *wil)
{
	if (wil->scan_request) {
		cfg80211_scan_done(wil->scan_request, true);
		wil->scan_request = NULL;
	}
	wil6210_disconnect(wil, NULL);
	rx_fini(wil);
	return 0;
}

int wil_down(struct wil6210_priv *wil)
{
	int ret;
	wil_info(wil, "%s()\n", __func__);

	mutex_lock(&wil->mutex);
	ret = __wil_down(wil);
	mutex_unlock(&wil->mutex);

	return ret;
}

