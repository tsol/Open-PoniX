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

#ifndef _ALX_SW_H_
#define _ALX_SW_H_

#include <linux/netdevice.h>
#include <linux/crc32.h>

/* Vendor ID */
#define ALX_VENDOR_ID                   0x1969

/* Device IDs */
#define ALX_DEV_ID_AR8131               0x1063   /* l1c */
#define ALX_DEV_ID_AR8132               0x1062   /* l2c */
#define ALX_DEV_ID_AR8151_V1            0x1073   /* l1d_v1 */
#define ALX_DEV_ID_AR8151_V2            0x1083   /* l1d_v2 */
#define ALX_DEV_ID_AR8152_V1            0x2060   /* l2cb_v1 */
#define ALX_DEV_ID_AR8152_V2            0x2062   /* l2cb_v2 */
#define ALX_DEV_ID_AR8161               0x1091   /* l1f */
#define ALX_DEV_ID_AR8162               0x1090   /* l2f */

#define ALX_REV_ID_AR8152_V1_0          0xc0
#define ALX_REV_ID_AR8152_V1_1          0xc1
#define ALX_REV_ID_AR8152_V2_0          0xc0
#define ALX_REV_ID_AR8152_V2_1          0xc1
#define ALX_REV_ID_AR8161_V2_0          0x10  /* B0 */

/* Generic Registers */
#define ALX_DEV_STAT                    0x62  /* 16 bits */
#define ALX_DEV_STAT_CERR               0x0001
#define ALX_DEV_STAT_NFERR              0x0002
#define ALX_DEV_STAT_FERR               0x0004

#define ALX_ISR                         0x1600
#define ALX_IMR                         0x1604
#define ALX_ISR_SMB                     0x00000001
#define ALX_ISR_TIMER                   0x00000002
#define ALX_ISR_MANU                    0x00000004
#define ALX_ISR_RXF_OV                  0x00000008
#define ALX_ISR_RFD_UR                  0x00000010
#define ALX_ISR_TX_Q1                   0x00000020
#define ALX_ISR_TX_Q2                   0x00000040
#define ALX_ISR_TX_Q3                   0x00000080
#define ALX_ISR_TXF_UR                  0x00000100
#define ALX_ISR_DMAR                    0x00000200
#define ALX_ISR_DMAW                    0x00000400
#define ALX_ISR_TX_CREDIT               0x00000800
#define ALX_ISR_PHY                     0x00001000
#define ALX_ISR_PHY_LPW                 0x00002000
#define ALX_ISR_TXQ_TO                  0x00004000
#define ALX_ISR_TX_Q0                   0x00008000
#define ALX_ISR_RX_Q0                   0x00010000
#define ALX_ISR_RX_Q1                   0x00020000
#define ALX_ISR_RX_Q2                   0x00040000
#define ALX_ISR_RX_Q3                   0x00080000
#define ALX_ISR_MAC_RX                  0x00100000
#define ALX_ISR_MAC_TX                  0x00200000
#define ALX_ISR_PCIE_UR                 0x00400000
#define ALX_ISR_PCIE_FERR               0x00800000
#define ALX_ISR_PCIE_NFERR              0x01000000
#define ALX_ISR_PCIE_CERR               0x02000000
#define ALX_ISR_PCIE_LNKDOWN            0x04000000
#define ALX_ISR_RX_Q4                   0x08000000
#define ALX_ISR_RX_Q5                   0x10000000
#define ALX_ISR_RX_Q6                   0x20000000
#define ALX_ISR_RX_Q7                   0x40000000
#define ALX_ISR_DIS                     0x80000000


#define ALX_IMR_NORMAL_MASK (\
		ALX_ISR_MANU            |\
		ALX_ISR_OVER            |\
		ALX_ISR_TXQ             |\
		ALX_ISR_RXQ             |\
		ALX_ISR_PHY_LPW         |\
		ALX_ISR_PHY             |\
		ALX_ISR_ERROR)

#define ALX_ISR_ALERT_MASK (\
		ALX_ISR_DMAR            |\
		ALX_ISR_DMAW            |\
		ALX_ISR_TXQ_TO          |\
		ALX_ISR_PCIE_FERR       |\
		ALX_ISR_PCIE_LNKDOWN    |\
		ALX_ISR_RFD_UR          |\
		ALX_ISR_RXF_OV)

#define ALX_ISR_TXQ (\
		ALX_ISR_TX_Q0           |\
		ALX_ISR_TX_Q1           |\
		ALX_ISR_TX_Q2           |\
		ALX_ISR_TX_Q3)

#define ALX_ISR_RXQ (\
		ALX_ISR_RX_Q0           |\
		ALX_ISR_RX_Q1           |\
		ALX_ISR_RX_Q2           |\
		ALX_ISR_RX_Q3           |\
		ALX_ISR_RX_Q4           |\
		ALX_ISR_RX_Q5           |\
		ALX_ISR_RX_Q6           |\
		ALX_ISR_RX_Q7)

#define ALX_ISR_OVER (\
		ALX_ISR_RFD_UR          |\
		ALX_ISR_RXF_OV          |\
		ALX_ISR_TXF_UR)

#define ALX_ISR_ERROR (\
		ALX_ISR_DMAR            |\
		ALX_ISR_TXQ_TO          |\
		ALX_ISR_DMAW            |\
		ALX_ISR_PCIE_ERROR)

#define ALX_ISR_PCIE_ERROR (\
		ALX_ISR_PCIE_FERR       |\
		ALX_ISR_PCIE_LNKDOWN)

/* MISC Register */
#define ALX_MISC                        0x19C0
#define ALX_MISC_INTNLOSC_OPEN          0x00000008

#define ALX_CLK_GATE                    0x1814

/* DMA address */
#define DMA_ADDR_HI_MASK                0xffffffff00000000ULL
#define DMA_ADDR_LO_MASK                0x00000000ffffffffULL

#define ALX_DMA_ADDR_HI(_addr) \
		((u32)(((u64)(_addr) & DMA_ADDR_HI_MASK) >> 32))
#define ALX_DMA_ADDR_LO(_addr) \
		((u32)((u64)(_addr) & DMA_ADDR_LO_MASK))

/* mac address length */
#define ALX_ETH_LENGTH_OF_ADDRESS       6
#define ALX_ETH_LENGTH_OF_HEADER        ETH_HLEN

#define ALX_ETH_CRC(_addr, _len)        ether_crc((_len), (_addr));

/* Autonegotiation advertised speeds */
/* Link speed */
#define ALX_LINK_SPEED_UNKNOWN          0x0
#define ALX_LINK_SPEED_10_HALF          0x0001
#define ALX_LINK_SPEED_10_FULL          0x0002
#define ALX_LINK_SPEED_100_HALF         0x0004
#define ALX_LINK_SPEED_100_FULL         0x0008
#define ALX_LINK_SPEED_1GB_FULL         0x0020
#define ALX_LINK_SPEED_DEFAULT (\
		ALX_LINK_SPEED_10_HALF  |\
		ALX_LINK_SPEED_10_FULL  |\
		ALX_LINK_SPEED_100_HALF |\
		ALX_LINK_SPEED_100_FULL |\
		ALX_LINK_SPEED_1GB_FULL)

#define ALX_MAX_SETUP_LNK_CYCLE         100

/* Device Type definitions for new protocol MDIO commands */
#define ALX_MDIO_DEV_TYPE_NORM          0

/* Wake On Lan */
#define ALX_WOL_PHY                     0x00000001 /* PHY Status Change */
#define ALX_WOL_MAGIC                   0x00000002 /* Magic Packet */

#define ALX_MAX_EEPROM_LEN              0x200
#define ALX_MAX_HWREG_LEN               0x200

/* RSS Settings */
enum alx_rss_mode {
	alx_rss_mode_disable    = 0,
	alx_rss_sig_que         = 1,
	alx_rss_mul_que_sig_int = 2,
	alx_rss_mul_que_mul_int = 4,
};

/* Flow Control Settings */
enum alx_fc_mode {
	alx_fc_none = 0,
	alx_fc_rx_pause,
	alx_fc_tx_pause,
	alx_fc_full,
	alx_fc_default
};

/* WRR Restrict Settings */
enum alx_wrr_mode {
	alx_wrr_mode_none = 0,
	alx_wrr_mode_high,
	alx_wrr_mode_high2,
	alx_wrr_mode_all
};

enum alx_mac_type {
	alx_mac_unknown = 0,
	alx_mac_l1c,
	alx_mac_l2c,
	alx_mac_l1d_v1,
	alx_mac_l1d_v2,
	alx_mac_l2cb_v1,
	alx_mac_l2cb_v20,
	alx_mac_l2cb_v21,
	alx_mac_l1f,
	alx_mac_l2f,
};


/* Statistics counters collected by the MAC */
struct alx_hw_stats {
	/* rx */
	unsigned long rx_ok;
	unsigned long rx_bcast;
	unsigned long rx_mcast;
	unsigned long rx_pause;
	unsigned long rx_ctrl;
	unsigned long rx_fcs_err;
	unsigned long rx_len_err;
	unsigned long rx_byte_cnt;
	unsigned long rx_runt;
	unsigned long rx_frag;
	unsigned long rx_sz_64B;
	unsigned long rx_sz_127B;
	unsigned long rx_sz_255B;
	unsigned long rx_sz_511B;
	unsigned long rx_sz_1023B;
	unsigned long rx_sz_1518B;
	unsigned long rx_sz_max;
	unsigned long rx_ov_sz;
	unsigned long rx_ov_rxf;
	unsigned long rx_ov_rrd;
	unsigned long rx_align_err;
	unsigned long rx_bc_byte_cnt;
	unsigned long rx_mc_byte_cnt;
	unsigned long rx_err_addr;

	/* tx */
	unsigned long tx_ok;
	unsigned long tx_bcast;
	unsigned long tx_mcast;
	unsigned long tx_pause;
	unsigned long tx_exc_defer;
	unsigned long tx_ctrl;
	unsigned long tx_defer;
	unsigned long tx_byte_cnt;
	unsigned long tx_sz_64B;
	unsigned long tx_sz_127B;
	unsigned long tx_sz_255B;
	unsigned long tx_sz_511B;
	unsigned long tx_sz_1023B;
	unsigned long tx_sz_1518B;
	unsigned long tx_sz_max;
	unsigned long tx_single_col;
	unsigned long tx_multi_col;
	unsigned long tx_late_col;
	unsigned long tx_abort_col;
	unsigned long tx_underrun;
	unsigned long tx_trd_eop;
	unsigned long tx_len_err;
	unsigned long tx_trunc;
	unsigned long tx_bc_byte_cnt;
	unsigned long tx_mc_byte_cnt;
	unsigned long update;
};

/* HW callback function pointer table */
struct alx_hw;
struct alx_hw_callbacks {
	/* NIC */
	int (*identify_nic)(struct alx_hw *);
	/* PHY */
	int (*init_phy)(struct alx_hw *);
	int (*reset_phy)(struct alx_hw *);
	int (*read_phy_reg)(struct alx_hw *, u16, u16 *);
	int (*write_phy_reg)(struct alx_hw *, u16, u16);
	/* Link */
	int (*setup_phy_link)(struct alx_hw *, u32, bool, bool);
	int (*setup_phy_link_speed)(struct alx_hw *, u32, bool, bool);
	int (*check_phy_link)(struct alx_hw *, u32 *, bool *);

	/* MAC */
	int (*reset_mac)(struct alx_hw *);
	int (*start_mac)(struct alx_hw *);
	int (*stop_mac)(struct alx_hw *);
	int (*config_mac)(struct alx_hw *, u16, u16, u16, u16, u16);
	int (*get_mac_addr)(struct alx_hw *, u8 *);
	int (*set_mac_addr)(struct alx_hw *, u8 *);
	int (*set_mc_addr)(struct alx_hw *, u8 *);
	int (*clear_mc_addr)(struct alx_hw *);

	/* intr */
	int (*ack_phy_intr)(struct alx_hw *);
	int (*enable_legacy_intr)(struct alx_hw *);
	int (*disable_legacy_intr)(struct alx_hw *);
	int (*enable_msix_intr)(struct alx_hw *, u8);
	int (*disable_msix_intr)(struct alx_hw *, u8);

	/* Configure */
	int (*config_rx)(struct alx_hw *);
	int (*config_tx)(struct alx_hw *);
	int (*config_fc)(struct alx_hw *);
	int (*config_rss)(struct alx_hw *, bool);
	int (*config_msix)(struct alx_hw *, u16, bool, bool);
	int (*config_wol)(struct alx_hw *, u32);
	int (*config_aspm)(struct alx_hw *, bool, bool);
	int (*config_mac_ctrl)(struct alx_hw *);
	int (*config_pow_save)(struct alx_hw *, u32,
				bool, bool, bool, bool);
	int (*reset_pcie)(struct alx_hw *, bool, bool);

	/* NVRam function */
	int (*check_nvram)(struct alx_hw *, bool *);
	int (*read_nvram)(struct alx_hw *, u16, u32 *);
	int (*write_nvram)(struct alx_hw *, u16, u32);

	/* Others */
	int (*get_ethtool_regs)(struct alx_hw *, void *);
};

struct alx_hw {
	struct alx_adapter	*adpt;
	struct alx_hw_callbacks	 cbs;
	u8 __iomem     *hw_addr; /* inner register address */
	u16             pci_venid;
	u16             pci_devid;
	u16             pci_sub_devid;
	u16             pci_sub_venid;
	u8              pci_revid;

	bool            long_cable;
	bool            aps_en;
	bool            hi_txperf;
	bool            msi_lnkpatch;
	u32             dma_chnl;
	u32             hwreg_sz;
	u32             eeprom_sz;

	/* PHY parameter */
	u32             phy_id;
	u32             autoneg_advertised;
	u32             link_speed;
	bool            link_up;
	spinlock_t      mdio_lock;

	/* MAC parameter */
	enum alx_mac_type mac_type;
	u8              mac_addr[ALX_ETH_LENGTH_OF_ADDRESS];
	u8              mac_perm_addr[ALX_ETH_LENGTH_OF_ADDRESS];

	u32             mtu;
	u16             rxstat_reg;
	u16             rxstat_sz;
	u16             txstat_reg;
	u16             txstat_sz;

	u16             tx_prod_reg[4];
	u16             tx_cons_reg[4];
	u16             rx_prod_reg[2];
	u16             rx_cons_reg[2];
	u64             tpdma[4];
	u64             rfdma[2];
	u64             rrdma[2];

	/* WRR parameter */
	enum alx_wrr_mode wrr_mode;
	u32             wrr_prio0;
	u32             wrr_prio1;
	u32             wrr_prio2;
	u32             wrr_prio3;

	/* RSS parameter */
	enum alx_rss_mode rss_mode;
	u8              rss_hstype;
	u8              rss_base_cpu;
	u16             rss_idt_size;
	u32             rss_idt[32];
	u8              rss_key[40];

	/* flow control parameter */
	enum alx_fc_mode cur_fc_mode; /* FC mode in effect */
	enum alx_fc_mode req_fc_mode; /* FC mode requested by caller */
	bool            disable_fc_autoneg; /* Do not autonegotiate FC */
	bool            fc_was_autonegged;  /* the result of autonegging */
	bool            fc_single_pause;

	/* Others */
	u32             preamble;
	u32             intr_mask;
	u16             smb_timer;
	u16             imt;    /* Interrupt Moderator timer (2us) */
	u32             flags;
};

#define ALX_HW_FLAG_L0S_CAP             0x00000001
#define ALX_HW_FLAG_L0S_EN              0x00000002
#define ALX_HW_FLAG_L1_CAP              0x00000004
#define ALX_HW_FLAG_L1_EN               0x00000008
#define ALX_HW_FLAG_PWSAVE_CAP          0x00000010
#define ALX_HW_FLAG_PWSAVE_EN           0x00000020
#define ALX_HW_FLAG_AZ_CAP              0x00000040
#define ALX_HW_FLAG_AZ_EN               0x00000080
#define ALX_HW_FLAG_PTP_CAP             0x00000100
#define ALX_HW_FLAG_PTP_EN              0x00000200
#define ALX_HW_FLAG_GIGA_CAP            0x00000400

#define ALX_HW_FLAG_PROMISC_EN          0x00010000   /* for mac ctrl reg */
#define ALX_HW_FLAG_VLANSTRIP_EN        0x00020000   /* for mac ctrl reg */
#define ALX_HW_FLAG_MULTIALL_EN         0x00040000   /* for mac ctrl reg */
#define ALX_HW_FLAG_LOOPBACK_EN         0x00080000   /* for mac ctrl reg */

#define CHK_HW_FLAG(_flag)              CHK_FLAG(hw, HW, _flag)
#define SET_HW_FLAG(_flag)              SET_FLAG(hw, HW, _flag)
#define CLI_HW_FLAG(_flag)              CLI_FLAG(hw, HW, _flag)


/* RSS hstype Definitions */
#define ALX_RSS_HSTYP_IPV4_EN           0x00000001
#define ALX_RSS_HSTYP_TCP4_EN           0x00000002
#define ALX_RSS_HSTYP_IPV6_EN           0x00000004
#define ALX_RSS_HSTYP_TCP6_EN           0x00000008
#define ALX_RSS_HSTYP_ALL_EN (\
		ALX_RSS_HSTYP_IPV4_EN   |\
		ALX_RSS_HSTYP_TCP4_EN   |\
		ALX_RSS_HSTYP_IPV6_EN   |\
		ALX_RSS_HSTYP_TCP6_EN)


/* definitions for flags */

#define CHK_FLAG_ARRAY(_st, _idx, _type, _flag)	\
		((_st)->flags[_idx] & (ALX_##_type##_FLAG_##_idx##_##_flag))
#define CHK_FLAG(_st, _type, _flag)	\
		((_st)->flags & (ALX_##_type##_FLAG_##_flag))

#define SET_FLAG_ARRAY(_st, _idx, _type, _flag) \
		((_st)->flags[_idx] |= (ALX_##_type##_FLAG_##_idx##_##_flag))
#define SET_FLAG(_st, _type, _flag) \
		((_st)->flags |= (ALX_##_type##_FLAG_##_flag))

#define CLI_FLAG_ARRAY(_st, _idx, _type, _flag) \
		((_st)->flags[_idx] &= ~(ALX_##_type##_FLAG_##_idx##_##_flag))
#define CLI_FLAG(_st, _type, _flag) \
		((_st)->flags &= ~(ALX_##_type##_FLAG_##_flag))

int alx_cfg_r16(const struct alx_hw *hw, int reg, u16 *pval);
int alx_cfg_w16(const struct alx_hw *hw, int reg, u16 val);


void alx_mem_flush(const struct alx_hw *hw);
void alx_mem_r32(const struct alx_hw *hw, int reg, u32 *val);
void alx_mem_w32(const struct alx_hw *hw, int reg, u32 val);
void alx_mem_w8(const struct alx_hw *hw, int reg, u8 val);


/* special definitions for hw */
#define ALF_MAX_MSIX_NOQUE_INTRS        4
#define ALF_MIN_MSIX_NOQUE_INTRS        4
#define ALF_MAX_MSIX_QUEUE_INTRS        12
#define ALF_MIN_MSIX_QUEUE_INTRS        12
#define ALF_MAX_MSIX_INTRS \
		(ALF_MAX_MSIX_QUEUE_INTRS + ALF_MAX_MSIX_NOQUE_INTRS)
#define ALF_MIN_MSIX_INTRS \
		(ALF_MIN_MSIX_NOQUE_INTRS + ALF_MIN_MSIX_QUEUE_INTRS)


/* function */
extern int alc_init_hw_callbacks(struct alx_hw *hw);
extern int alf_init_hw_callbacks(struct alx_hw *hw);

/* Logging message functions */
void __printf(3, 4) alx_hw_printk(const char *level, const struct alx_hw *hw,
				  const char *fmt, ...);

#define alx_hw_err(_hw, _format, ...) \
	alx_hw_printk(KERN_ERR, _hw, _format, ##__VA_ARGS__)
#define alx_hw_warn(_hw, _format, ...) \
	alx_hw_printk(KERN_WARNING, _hw, _format, ##__VA_ARGS__)
#define alx_hw_info(_hw, _format, ...) \
	alx_hw_printk(KERN_INFO, _hw, _format, ##__VA_ARGS__)

#endif /* _ALX_SW_H_ */

