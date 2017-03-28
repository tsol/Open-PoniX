/* Copyright (C) 2010 - 2012 Broadcom Corporation.
 * Portions Copyright (C) VMware, Inc. 2007-2011. All Rights Reserved.
 */

struct tg3;

/*
 *  On ESX the wmb() instruction is defined to only a compiler barrier.
 *  The macro wmb() needs to be overridden to properly synchronize memory.
 */
#if defined(__VMKLNX__)
#undef wmb
#define wmb()   asm volatile("sfence" ::: "memory")
#endif

static int psod_on_tx_timeout = 0;

module_param(psod_on_tx_timeout, int, 0);
MODULE_PARM_DESC(psod_on_tx_timeout, "For debugging purposes, crash the system "
                                     " when a tx timeout occurs");
#define TG3_MAX_NIC		32

#ifndef TG3_VMWARE_NETQ_DISABLE
#define TG3_VMWARE_NETQ_ENABLE

#define TG3_OPTION_UNSET	-1

static unsigned int __devinitdata tg3_netq_index;
static int __devinitdata tg3_netq_force[TG3_MAX_NIC+1] =
			 { [0 ... TG3_MAX_NIC] = TG3_OPTION_UNSET };

module_param_array_named(force_netq, tg3_netq_force, int, NULL, 0);
MODULE_PARM_DESC(force_netq,
"Force the maximum number of NetQueues available per port (NetQueue capable devices only)");

static const struct {
	const char string[ETH_GSTRING_LEN];
} tg3_vmware_ethtool_stats_keys[] = {
	{ "[0]: rx_packets (sw)" },
	{ "[0]: rx_packets (hw)" },
	{ "[0]: rx_bytes (sw)" },
	{ "[0]: rx_bytes (hw)" },
	{ "[0]: rx_errors (sw)" },
	{ "[0]: rx_errors (hw)" },
	{ "[0]: rx_crc_errors" },
	{ "[0]: rx_frame_errors" },
	{ "[0]: tx_bytes" },
	{ "[0]: tx_ucast_packets" },
	{ "[0]: tx_mcast_packets" },
	{ "[0]: tx_bcast_packets" },
};

/*
 * Pack this structure so that we don't get an extra 8 bytes
 * should this driver be built for a 128-bit CPU. :)
 */
struct tg3_netq_stats {
	u64	rx_packets_sw;
	u64	rx_packets_hw;
	u64	rx_bytes_sw;
	u64	rx_bytes_hw;
	u64	rx_errors_sw;
	u64	rx_errors_hw;
	u64	rx_crc_errors;
	u64	rx_frame_errors;
	u64	tx_bytes;
	u64	tx_ucast_packets;
	u64	tx_mcast_packets;
	u64	tx_bcast_packets;
} __attribute__((packed));

#define TG3_NETQ_NUM_STATS	(sizeof(struct tg3_netq_stats)/sizeof(u64))

struct tg3_netq_napi {
	u32			flags;
	#define TG3_NETQ_TXQ_ALLOCATED		0x0001
	#define TG3_NETQ_RXQ_ALLOCATED		0x0002
	#define TG3_NETQ_RXQ_ENABLED		0x0008
	#define TG3_NETQ_TXQ_FREE_STATE		0x0010
	#define TG3_NETQ_RXQ_FREE_STATE		0x0020

	struct tg3_netq_stats	stats;
	struct net_device_stats	net_stats;
};

struct tg3_vmware_netq {
	u16			n_tx_queues_allocated;
	u16			n_rx_queues_allocated;

	u32			index;
};

static void tg3_vmware_fetch_stats(struct tg3 *tp);
static void tg3_disable_prod_rcbs(struct tg3 *tp, u32 ring);
static void tg3_setup_prod_mboxes(struct tg3 *tp, u32 ring);
static void tg3_netq_init(struct tg3 *tp);
static void tg3_netq_free_all_qs(struct tg3 *tp);
static void tg3_netq_invalidate_state(struct tg3 *tp);
static void tg3_netq_restore(struct tg3 *tp);
static void tg3_netq_limit_dflt_queue_counts(struct tg3 *tp);
static u32  tg3_netq_tune_vector_count(struct tg3 *tp);
static int  tg3_netq_stats_size(struct tg3 *tp);
static void tg3_netq_stats_get_strings(struct tg3 *tp, u8 *buf);
static void tg3_netq_stats_get(struct tg3 *tp, u64 *tmp_stats);
static void tg3_netq_stats_clear(struct tg3 *tp);
#endif /* TG3_VMWARE_NETQ_ENABLE */

struct tg3_vmware {
	u32 rx_mode_reset_counter;

#ifdef TG3_VMWARE_NETQ_ENABLE
	struct tg3_vmware_netq	netq;
#endif
};

#if !defined(TG3_VMWARE_BMAPILNX_DISABLE)

#include "esx_ioctl.h"

static int
tg3_vmware_ioctl_cim(struct net_device *dev, struct ifreq *ifr);

#endif  /* !TG3_VMWARE_BMAPILNX_DISABLED */

static void tg3_vmware_timer(struct tg3 *tp);
static netdev_features_t tg3_vmware_tune_tso(struct tg3 *tp,
					     netdev_features_t features);
