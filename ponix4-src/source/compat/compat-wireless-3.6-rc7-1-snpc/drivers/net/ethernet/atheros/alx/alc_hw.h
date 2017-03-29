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

#ifndef L1C_HW_H_
#define L1C_HW_H_

/*********************************************************************
 * some reqs for l1x_sw.h
 *
 * 1. some basic type must be defined if there are not defined by
 *    your compiler:
 *    u8, u16, u32, bool
 *
 * 2. PETHCONTEXT difinition should be in l1x_sw.h and it must contain
 *    pci_devid & pci_venid & pci_revid
 *
 *********************************************************************/

#include "alx_hwcom.h"

/******************************************************************************/

#define L1C_DEV_ID                      0x1063
#define L2C_DEV_ID                      0x1062
#define L2CB_DEV_ID                     0x2060
#define L2CB2_DEV_ID                    0x2062
#define L1D_DEV_ID                      0x1073
#define L1D2_DEV_ID                     0x1083

#define L2CB_V10                        0xC0
#define L2CB_V11                        0xC1
#define L2CB_V20                        0xC0
#define L2CB_V21                        0xC1

#define L1C_PM_CSR                      0x0044  /* 16bit */
#define L1C_PM_CSR_PME_STAT             BIT(15)
#define L1C_PM_CSR_DSCAL_MASK           ASHFT13(3U)
#define L1C_PM_CSR_DSCAL_SHIFT          13
#define L1C_PM_CSR_DSEL_MASK            ASHFT9(0xFU)
#define L1C_PM_CSR_DSEL_SHIFT           9
#define L1C_PM_CSR_PME_EN               BIT(8)
#define L1C_PM_CSR_PWST_MASK            ASHFT0(3U)
#define L1C_PM_CSR_PWST_SHIFT           0

#define L1C_PM_DATA                     0x0047  /* 8bit */

#define L1C_DEV_CAP                     0x005C
#define L1C_DEV_CAP_SPLSL_MASK          ASHFT26(3UL)
#define L1C_DEV_CAP_SPLSL_SHIFT         26
#define L1C_DEV_CAP_SPLV_MASK           ASHFT18(0xFFUL)
#define L1C_DEV_CAP_SPLV_SHIFT          18
#define L1C_DEV_CAP_RBER                BIT(15)
#define L1C_DEV_CAP_PIPRS               BIT(14)
#define L1C_DEV_CAP_AIPRS               BIT(13)
#define L1C_DEV_CAP_ABPRS               BIT(12)
#define L1C_DEV_CAP_L1ACLAT_MASK        ASHFT9(7UL)
#define L1C_DEV_CAP_L1ACLAT_SHIFT       9
#define L1C_DEV_CAP_L0SACLAT_MASK       ASHFT6(7UL)
#define L1C_DEV_CAP_L0SACLAT_SHIFT      6
#define L1C_DEV_CAP_EXTAG               BIT(5)
#define L1C_DEV_CAP_PHANTOM             BIT(4)
#define L1C_DEV_CAP_MPL_MASK            ASHFT0(7UL)
#define L1C_DEV_CAP_MPL_SHIFT           0
#define L1C_DEV_CAP_MPL_128             1
#define L1C_DEV_CAP_MPL_256             2
#define L1C_DEV_CAP_MPL_512             3
#define L1C_DEV_CAP_MPL_1024            4
#define L1C_DEV_CAP_MPL_2048            5
#define L1C_DEV_CAP_MPL_4096            6

#define L1C_DEV_CTRL                    0x0060    /* 16bit */
#define L1C_DEV_CTRL_MAXRRS_MASK        ASHFT12(7U)
#define L1C_DEV_CTRL_MAXRRS_SHIFT       12
#define L1C_DEV_CTRL_MAXRRS_MIN         2
#define L1C_DEV_CTRL_NOSNP_EN           BIT(11)
#define L1C_DEV_CTRL_AUXPWR_EN          BIT(10)
#define L1C_DEV_CTRL_PHANTOM_EN         BIT(9)
#define L1C_DEV_CTRL_EXTAG_EN           BIT(8)
#define L1C_DEV_CTRL_MPL_MASK           ASHFT5(7U)
#define L1C_DEV_CTRL_MPL_SHIFT          5
#define L1C_DEV_CTRL_RELORD_EN          BIT(4)
#define L1C_DEV_CTRL_URR_EN             BIT(3)
#define L1C_DEV_CTRL_FERR_EN            BIT(2)
#define L1C_DEV_CTRL_NFERR_EN           BIT(1)
#define L1C_DEV_CTRL_CERR_EN            BIT(0)

#define L1C_DEV_STAT                    0x0062    /* 16bit */
#define L1C_DEV_STAT_XS_PEND            BIT(5)
#define L1C_DEV_STAT_AUXPWR             BIT(4)
#define L1C_DEV_STAT_UR                 BIT(3)
#define L1C_DEV_STAT_FERR               BIT(2)
#define L1C_DEV_STAT_NFERR              BIT(1)
#define L1C_DEV_STAT_CERR               BIT(0)

#define L1C_LNK_CAP                     0x0064
#define L1C_LNK_CAP_PRTNUM_MASK         ASHFT24(0xFFUL)
#define L1C_LNK_CAP_PRTNUM_SHIFT        24
#define L1C_LNK_CAP_CLK_PM              BIT(18)
#define L1C_LNK_CAP_L1EXTLAT_MASK       ASHFT15(7UL)
#define L1C_LNK_CAP_L1EXTLAT_SHIFT      15
#define L1C_LNK_CAP_L0SEXTLAT_MASK      ASHFT12(7UL)
#define L1C_LNK_CAP_L0SEXTLAT_SHIFT     12
#define L1C_LNK_CAP_ASPM_SUP_MASK       ASHFT10(3UL)
#define L1C_LNK_CAP_ASPM_SUP_SHIFT      10
#define L1C_LNK_CAP_ASPM_SUP_L0S        1
#define L1C_LNK_CAP_ASPM_SUP_L0SL1      3
#define L1C_LNK_CAP_MAX_LWH_MASK        ASHFT4(0x3FUL)
#define L1C_LNK_CAP_MAX_LWH_SHIFT       4
#define L1C_LNK_CAP_MAX_LSPD_MASH       ASHFT0(0xFUL)
#define L1C_LNK_CAP_MAX_LSPD_SHIFT      0

#define L1C_LNK_CTRL                    0x0068  /* 16bit */
#define L1C_LNK_CTRL_CLK_PM_EN          BIT(8)
#define L1C_LNK_CTRL_EXTSYNC            BIT(7)
#define L1C_LNK_CTRL_CMNCLK_CFG         BIT(6)
#define L1C_LNK_CTRL_RCB_128B           BIT(3)  /* 0:64b,1:128b */
#define L1C_LNK_CTRL_ASPM_MASK          ASHFT0(3U)
#define L1C_LNK_CTRL_ASPM_SHIFT         0
#define L1C_LNK_CTRL_ASPM_DIS           0
#define L1C_LNK_CTRL_ASPM_ENL0S         1
#define L1C_LNK_CTRL_ASPM_ENL1          2
#define L1C_LNK_CTRL_ASPM_ENL0SL1       3

#define L1C_LNK_STAT                    0x006A  /* 16bit */
#define L1C_LNK_STAT_SCLKCFG            BIT(12)
#define L1C_LNK_STAT_LNKTRAIN           BIT(11)
#define L1C_LNK_STAT_TRNERR             BIT(10)
#define L1C_LNK_STAT_LNKSPD_MASK        ASHFT0(0xFU)
#define L1C_LNK_STAT_LNKSPD_SHIFT       0
#define L1C_LNK_STAT_NEGLW_MASK         ASHFT4(0x3FU)
#define L1C_LNK_STAT_NEGLW_SHIFT        4

#define L1C_UE_SVRT                     0x010C
#define L1C_UE_SVRT_UR                  BIT(20)
#define L1C_UE_SVRT_ECRCERR             BIT(19)
#define L1C_UE_SVRT_MTLP                BIT(18)
#define L1C_UE_SVRT_RCVOVFL             BIT(17)
#define L1C_UE_SVRT_UNEXPCPL            BIT(16)
#define L1C_UE_SVRT_CPLABRT             BIT(15)
#define L1C_UE_SVRT_CPLTO               BIT(14)
#define L1C_UE_SVRT_FCPROTERR           BIT(13)
#define L1C_UE_SVRT_PTLP                BIT(12)
#define L1C_UE_SVRT_DLPROTERR           BIT(4)
#define L1C_UE_SVRT_TRNERR              BIT(0)

#define L1C_SLD                         0x0218  /* efuse load */
#define L1C_SLD_FREQ_MASK               ASHFT24(3UL)
#define L1C_SLD_FREQ_SHIFT              24
#define L1C_SLD_FREQ_100K               0
#define L1C_SLD_FREQ_200K               1
#define L1C_SLD_FREQ_300K               2
#define L1C_SLD_FREQ_400K               3
#define L1C_SLD_EXIST                   BIT(23)
#define L1C_SLD_SLVADDR_MASK            ASHFT16(0x7FUL)
#define L1C_SLD_SLVADDR_SHIFT           16
#define L1C_SLD_IDLE                    BIT(13)
#define L1C_SLD_STAT                    BIT(12)  /* 0:finish,1:in progress */
#define L1C_SLD_START                   BIT(11)
#define L1C_SLD_STARTADDR_MASK          ASHFT0(0xFFUL)
#define L1C_SLD_STARTADDR_SHIFT         0
#define L1C_SLD_MAX_TO                  100

#define L1C_PPHY_MISC1                  0x1000
#define L1C_PPHY_MISC1_RCVDET           BIT(2)
#define L1C_PPHY_MISC1_NFTS_MASK        ASHFT16(0xFFUL)
#define L1C_PPHY_MISC1_NFTS_SHIFT       16
#define L1C_PPHY_MISC1_NFTS_HIPERF      0xA0    /* ???? */

#define L1C_PPHY_MISC2                  0x1004
#define L1C_PPHY_MISC2_L0S_TH_MASK      ASHFT18(0x3UL)
#define L1C_PPHY_MISC2_L0S_TH_SHIFT     18
#define L1C_PPHY_MISC2_L0S_TH_L2CB1     3
#define L1C_PPHY_MISC2_CDR_BW_MASK      ASHFT16(0x3UL)
#define L1C_PPHY_MISC2_CDR_BW_SHIFT     16
#define L1C_PPHY_MISC2_CDR_BW_L2CB1     3

#define L1C_PDLL_TRNS1                  0x1104
#define L1C_PDLL_TRNS1_D3PLLOFF_EN      BIT(11)
#define L1C_PDLL_TRNS1_REGCLK_SEL_NORM  BIT(10)
#define L1C_PDLL_TRNS1_REPLY_TO_MASK    ASHFT0(0x3FFUL)
#define L1C_PDLL_TRNS1_REPLY_TO_SHIFT   0

#define L1C_TWSI_DBG                    0x1108
#define L1C_TWSI_DBG_DEV_EXIST          BIT(29)

#define L1C_DMA_DBG                     0x1114
#define L1C_DMA_DBG_VENDOR_MSG          BIT(0)

#define L1C_TLEXTN_STATS                0x1204  /* diff with l1f */
#define L1C_TLEXTN_STATS_DEVNO_MASK     ASHFT16(0x1FUL)
#define L1C_TLEXTN_STATS_DEVNO_SHIFT    16
#define L1C_TLEXTN_STATS_BUSNO_MASK     ASHFT8(0xFFUL)
#define L1C_TLEXTN_STATS_BUSNO_SHIFT    8

#define L1C_EFUSE_CTRL                  0x12C0
#define L1C_EFUSE_CTRL_FLAG             BIT(31)  /* 0:read,1:write */
#define L1C_EUFSE_CTRL_ACK              BIT(30)
#define L1C_EFUSE_CTRL_ADDR_MASK        ASHFT16(0x3FFUL)
#define L1C_EFUSE_CTRL_ADDR_SHIFT       16

#define L1C_EFUSE_DATA                  0x12C4

#define EFUSE_OP_MAX_AC_TIMER           100     /* 1ms */

#define L1C_EFUSE_CTRL2                 0x12F0
#define L1C_EFUSE_CTRL2_CLK_EN          BIT(1)

#define L1C_PMCTRL                      0x12F8
#define L1C_PMCTRL_HOTRST_WTEN          BIT(31)
#define L1C_PMCTRL_ASPM_FCEN            BIT(30)  /* L0s/L1 dis by MAC based on
						 * thrghput(setting in 15A0) */
#define L1C_PMCTRL_SADLY_EN             BIT(29)
#define L1C_PMCTRL_L0S_BUFSRX_EN        BIT(28)
#define L1C_PMCTRL_LCKDET_TIMER_MASK    ASHFT24(0xFUL)
#define L1C_PMCTRL_LCKDET_TIMER_SHIFT   24
#define L1C_PMCTRL_LCKDET_TIMER_DEF     0xC
#define L1C_PMCTRL_L1REQ_TO_MASK        ASHFT20(0xFUL)
#define L1C_PMCTRL_L1REQ_TO_SHIFT       20      /* pm_request_l1 time > @
						 * ->L0s not L1 */
#define L1C_PMCTRL_L1REG_TO_DEF         0xC
#define L1D_PMCTRL_TXL1_AFTER_L0S       BIT(19)  /* l1dv2.0+ */
#define L1D_PMCTRL_L1_TIMER_MASK        ASHFT16(7UL)
#define L1D_PMCTRL_L1_TIMER_SHIFT       16
#define L1D_PMCTRL_L1_TIMER_DIS         0
#define L1D_PMCTRL_L1_TIMER_2US         1
#define L1D_PMCTRL_L1_TIMER_4US         2
#define L1D_PMCTRL_L1_TIMER_8US         3
#define L1D_PMCTRL_L1_TIMER_16US        4
#define L1D_PMCTRL_L1_TIMER_24US        5
#define L1D_PMCTRL_L1_TIMER_32US        6
#define L1D_PMCTRL_L1_TIMER_63US        7
#define L1C_PMCTRL_L1_TIMER_MASK        ASHFT16(0xFUL)
#define L1C_PMCTRL_L1_TIMER_SHIFT       16
#define L1C_PMCTRL_L1_TIMER_L2CB1       7
#define L1C_PMCTRL_L1_TIMER_DEF         0xF
#define L1C_PMCTRL_RCVR_WT_1US          BIT(15)  /* 1:1us, 0:2ms */
#define L1C_PMCTRL_PWM_VER_11           BIT(14)  /* 0:1.0a,1:1.1 */
#define L1C_PMCTRL_L1_CLKSW_EN          BIT(13)  /* en pcie clk sw in L1 */
#define L1C_PMCTRL_L0S_EN               BIT(12)
#define L1D_PMCTRL_RXL1_AFTER_L0S       BIT(11)  /* l1dv2.0+ */
#define L1D_PMCTRL_L0S_TIMER_MASK       ASHFT8(7UL)
#define L1D_PMCTRL_L0S_TIMER_SHIFT      8
#define L1C_PMCTRL_L0S_TIMER_MASK       ASHFT8(0xFUL)
#define L1C_PMCTRL_L0S_TIMER_SHIFT      8
#define L1C_PMCTRL_L1_BUFSRX_EN         BIT(7)
#define L1C_PMCTRL_L1_SRDSRX_PWD        BIT(6)   /* power down serdes rx */
#define L1C_PMCTRL_L1_SRDSPLL_EN        BIT(5)
#define L1C_PMCTRL_L1_SRDS_EN           BIT(4)
#define L1C_PMCTRL_L1_EN                BIT(3)
#define L1C_PMCTRL_CLKREQ_EN            BIT(2)
#define L1C_PMCTRL_RBER_EN              BIT(1)
#define L1C_PMCTRL_SPRSDWER_EN          BIT(0)

#define L1C_LTSSM_CTRL                  0x12FC
#define L1C_LTSSM_WRO_EN                BIT(12)
#define L1C_LTSSM_TXTLP_BYPASS          BIT(7)

#define L1C_MASTER                      0x1400
#define L1C_MASTER_OTP_FLG              BIT(31)
#define L1C_MASTER_DEV_NUM_MASK         ASHFT24(0x7FUL)
#define L1C_MASTER_DEV_NUM_SHIFT        24
#define L1C_MASTER_REV_NUM_MASK         ASHFT16(0xFFUL)
#define L1C_MASTER_REV_NUM_SHIFT        16
#define L1C_MASTER_RDCLR_INT            BIT(14)
#define L1C_MASTER_CLKSW_L2EV1          BIT(13)      /* 0:l2ev2.0,1:l2ev1.0 */
#define L1C_MASTER_PCLKSEL_SRDS         BIT(12)      /* 1:alwys sel pclk from
						     * serdes, not sw to 25M */
#define L1C_MASTER_IRQMOD2_EN           BIT(11)      /* IRQ MODURATION FOR RX */
#define L1C_MASTER_IRQMOD1_EN           BIT(10)      /* MODURATION FOR TX/RX */
#define L1C_MASTER_MANU_INT             BIT(9)       /* SOFT MANUAL INT */
#define L1C_MASTER_MANUTIMER_EN         BIT(8)
#define L1C_MASTER_SYSALVTIMER_EN       BIT(7)       /* SYS ALIVE TIMER EN */
#define L1C_MASTER_OOB_DIS              BIT(6)       /* OUT OF BOX DIS */
#define L1C_MASTER_WAKEN_25M            BIT(5)       /* WAKE WO. PCIE CLK */
#define L1C_MASTER_BERT_START           BIT(4)
#define L1C_MASTER_PCIE_TSTMOD_MASK     ASHFT2(3UL)
#define L1C_MASTER_PCIE_TSTMOD_SHIFT    2
#define L1C_MASTER_PCIE_RST             BIT(1)
#define L1C_MASTER_DMA_MAC_RST          BIT(0)       /* RST MAC & DMA */
#define L1C_DMA_MAC_RST_TO              50

#define L1C_MANU_TIMER                  0x1404

#define L1C_IRQ_MODU_TIMER              0x1408
#define L1C_IRQ_MODU_TIMER2_MASK        ASHFT16(0xFFFFUL)
#define L1C_IRQ_MODU_TIMER2_SHIFT       16          /* ONLY FOR RX */
#define L1C_IRQ_MODU_TIMER1_MASK        ASHFT0(0xFFFFUL)
#define L1C_IRQ_MODU_TIMER1_SHIFT       0

#define L1C_PHY_CTRL                    0x140C
#define L1C_PHY_CTRL_ADDR_MASK          ASHFT19(0x1FUL)
#define L1C_PHY_CTRL_ADDR_SHIFT         19
#define L1C_PHY_CTRL_BP_VLTGSW          BIT(18)
#define L1C_PHY_CTRL_100AB_EN           BIT(17)
#define L1C_PHY_CTRL_10AB_EN            BIT(16)
#define L1C_PHY_CTRL_PLL_BYPASS         BIT(15)
#define L1C_PHY_CTRL_POWER_DOWN         BIT(14)      /* affect MAC & PHY,
						     * go to low power sts */
#define L1C_PHY_CTRL_PLL_ON             BIT(13)      /* 1:PLL ALWAYS ON
						     * 0:CAN SWITCH IN LPW */
#define L1C_PHY_CTRL_RST_ANALOG         BIT(12)
#define L1C_PHY_CTRL_HIB_PULSE          BIT(11)
#define L1C_PHY_CTRL_HIB_EN             BIT(10)
#define L1C_PHY_CTRL_GIGA_DIS           BIT(9)
#define L1C_PHY_CTRL_IDDQ_DIS           BIT(8)       /* POWER ON RST */
#define L1C_PHY_CTRL_IDDQ               BIT(7)       /* WHILE REBOOT, BIT8(1)
						     * EFFECTS BIT7 */
#define L1C_PHY_CTRL_LPW_EXIT           BIT(6)
#define L1C_PHY_CTRL_GATE_25M           BIT(5)
#define L1C_PHY_CTRL_RVRS_ANEG          BIT(4)
#define L1C_PHY_CTRL_ANEG_NOW           BIT(3)
#define L1C_PHY_CTRL_LED_MODE           BIT(2)
#define L1C_PHY_CTRL_RTL_MODE           BIT(1)
#define L1C_PHY_CTRL_DSPRST_OUT         BIT(0)       /* OUT OF DSP RST STATE */
#define L1C_PHY_CTRL_DSPRST_TO          80
#define L1C_PHY_CTRL_CLS                (\
	L1C_PHY_CTRL_LED_MODE           |\
	L1C_PHY_CTRL_100AB_EN           |\
	L1C_PHY_CTRL_PLL_ON)


#define L1C_MAC_STS                     0x1410
#define L1C_MAC_STS_SFORCE_MASK         ASHFT14(0xFUL)
#define L1C_MAC_STS_SFORCE_SHIFT        14
#define L1C_MAC_STS_CALIB_DONE          BIT13
#define L1C_MAC_STS_CALIB_RES_MASK      ASHFT8(0x1FUL)
#define L1C_MAC_STS_CALIB_RES_SHIFT     8
#define L1C_MAC_STS_CALIBERR_MASK       ASHFT4(0xFUL)
#define L1C_MAC_STS_CALIBERR_SHIFT      4
#define L1C_MAC_STS_TXQ_BUSY            BIT(3)
#define L1C_MAC_STS_RXQ_BUSY            BIT(2)
#define L1C_MAC_STS_TXMAC_BUSY          BIT(1)
#define L1C_MAC_STS_RXMAC_BUSY          BIT(0)
#define L1C_MAC_STS_IDLE                (\
	L1C_MAC_STS_TXQ_BUSY            |\
	L1C_MAC_STS_RXQ_BUSY            |\
	L1C_MAC_STS_TXMAC_BUSY          |\
	L1C_MAC_STS_RXMAC_BUSY)

#define L1C_MDIO                        0x1414
#define L1C_MDIO_MODE_EXT               BIT(30)      /* 0:normal,1:ext */
#define L1C_MDIO_POST_READ              BIT(29)
#define L1C_MDIO_AUTO_POLLING           BIT(28)
#define L1C_MDIO_BUSY                   BIT(27)
#define L1C_MDIO_CLK_SEL_MASK           ASHFT24(7UL)
#define L1C_MDIO_CLK_SEL_SHIFT          24
#define L1C_MDIO_CLK_SEL_25MD4          0           /* 25M DIV 4 */
#define L1C_MDIO_CLK_SEL_25MD6          2
#define L1C_MDIO_CLK_SEL_25MD8          3
#define L1C_MDIO_CLK_SEL_25MD10         4
#define L1C_MDIO_CLK_SEL_25MD32         5
#define L1C_MDIO_CLK_SEL_25MD64         6
#define L1C_MDIO_CLK_SEL_25MD128        7
#define L1C_MDIO_START                  BIT(23)
#define L1C_MDIO_SPRES_PRMBL            BIT(22)
#define L1C_MDIO_OP_READ                BIT(21)      /* 1:read,0:write */
#define L1C_MDIO_REG_MASK               ASHFT16(0x1FUL)
#define L1C_MDIO_REG_SHIFT              16
#define L1C_MDIO_DATA_MASK              ASHFT0(0xFFFFUL)
#define L1C_MDIO_DATA_SHIFT             0
#define L1C_MDIO_MAX_AC_TO              120

#define L1C_MDIO_EXTN                   0x1448
#define L1C_MDIO_EXTN_PORTAD_MASK       ASHFT21(0x1FUL)
#define L1C_MDIO_EXTN_PORTAD_SHIFT      21
#define L1C_MDIO_EXTN_DEVAD_MASK        ASHFT16(0x1FUL)
#define L1C_MDIO_EXTN_DEVAD_SHIFT       16
#define L1C_MDIO_EXTN_REG_MASK          ASHFT0(0xFFFFUL)
#define L1C_MDIO_EXTN_REG_SHIFT         0

#define L1C_PHY_STS                     0x1418
#define L1C_PHY_STS_LPW                 BIT(31)
#define L1C_PHY_STS_LPI                 BIT(30)
#define L1C_PHY_STS_PWON_STRIP_MASK     ASHFT16(0xFFFUL)
#define L1C_PHY_STS_PWON_STRIP_SHIFT    16

#define L1C_PHY_STS_DUPLEX              BIT(3)
#define L1C_PHY_STS_LINKUP              BIT(2)
#define L1C_PHY_STS_SPEED_MASK          ASHFT0(3UL)
#define L1C_PHY_STS_SPEED_SHIFT         0
#define L1C_PHY_STS_SPEED_SHIFT         0
#define L1C_PHY_STS_SPEED_1000M         2
#define L1C_PHY_STS_SPEED_100M          1
#define L1C_PHY_STS_SPEED_10M           0

#define L1C_BIST0                       0x141C
#define L1C_BIST0_COL_MASK              ASHFT24(0x3FUL)
#define L1C_BIST0_COL_SHIFT             24
#define L1C_BIST0_ROW_MASK              ASHFT12(0xFFFUL)
#define L1C_BIST0_ROW_SHIFT             12
#define L1C_BIST0_STEP_MASK             ASHFT8(0xFUL)
#define L1C_BIST0_STEP_SHIFT            8
#define L1C_BIST0_PATTERN_MASK          ASHFT4(7UL)
#define L1C_BIST0_PATTERN_SHIFT         4
#define L1C_BIST0_CRIT                  BIT(3)
#define L1C_BIST0_FIXED                 BIT(2)
#define L1C_BIST0_FAIL                  BIT(1)
#define L1C_BIST0_START                 BIT(0)

#define L1C_BIST1                       0x1420
#define L1C_BIST1_COL_MASK              ASHFT24(0x3FUL)
#define L1C_BIST1_COL_SHIFT             24
#define L1C_BIST1_ROW_MASK              ASHFT12(0xFFFUL)
#define L1C_BIST1_ROW_SHIFT             12
#define L1C_BIST1_STEP_MASK             ASHFT8(0xFUL)
#define L1C_BIST1_STEP_SHIFT            8
#define L1C_BIST1_PATTERN_MASK          ASHFT4(7UL)
#define L1C_BIST1_PATTERN_SHIFT         4
#define L1C_BIST1_CRIT                  BIT(3)
#define L1C_BIST1_FIXED                 BIT(2)
#define L1C_BIST1_FAIL                  BIT(1)
#define L1C_BIST1_START                 BIT(0)

#define L1C_SERDES                      0x1424
#define L1C_SERDES_PHYCLK_SLWDWN        BIT(18)
#define L1C_SERDES_MACCLK_SLWDWN        BIT(17)
#define L1C_SERDES_SELFB_PLL_MASK       ASHFT14(3UL)
#define L1C_SERDES_SELFB_PLL_SHIFT      14
#define L1C_SERDES_PHYCLK_SEL_GTX       BIT(13)          /* 1:gtx_clk, 0:25M */
#define L1C_SERDES_PCIECLK_SEL_SRDS     BIT(12)          /* 1:serdes,0:25M */
#define L1C_SERDES_BUFS_RX_EN           BIT(11)
#define L1C_SERDES_PD_RX                BIT(10)
#define L1C_SERDES_PLL_EN               BIT(9)
#define L1C_SERDES_EN                   BIT(8)
#define L1C_SERDES_SELFB_PLL_SEL_CSR    BIT(6)       /* 0:state-machine,1:csr */
#define L1C_SERDES_SELFB_PLL_CSR_MASK   ASHFT4(3UL)
#define L1C_SERDES_SELFB_PLL_CSR_SHIFT  4
#define L1C_SERDES_SELFB_PLL_CSR_4      3           /* 4-12% OV-CLK */
#define L1C_SERDES_SELFB_PLL_CSR_0      2           /* 0-4% OV-CLK */
#define L1C_SERDES_SELFB_PLL_CSR_12     1           /* 12-18% OV-CLK */
#define L1C_SERDES_SELFB_PLL_CSR_18     0           /* 18-25% OV-CLK */
#define L1C_SERDES_VCO_SLOW             BIT(3)
#define L1C_SERDES_VCO_FAST             BIT(2)
#define L1C_SERDES_LOCKDCT_EN           BIT(1)
#define L1C_SERDES_LOCKDCTED            BIT(0)

#define L1C_LED_CTRL                    0x1428
#define L1C_LED_CTRL_PATMAP2_MASK       ASHFT8(3UL)
#define L1C_LED_CTRL_PATMAP2_SHIFT      8
#define L1C_LED_CTRL_PATMAP1_MASK       ASHFT6(3UL)
#define L1C_LED_CTRL_PATMAP1_SHIFT      6
#define L1C_LED_CTRL_PATMAP0_MASK       ASHFT4(3UL)
#define L1C_LED_CTRL_PATMAP0_SHIFT      4
#define L1C_LED_CTRL_D3_MODE_MASK       ASHFT2(3UL)
#define L1C_LED_CTRL_D3_MODE_SHIFT      2
#define L1C_LED_CTRL_D3_MODE_NORMAL     0
#define L1C_LED_CTRL_D3_MODE_WOL_DIS    1
#define L1C_LED_CTRL_D3_MODE_WOL_ANY    2
#define L1C_LED_CTRL_D3_MODE_WOL_EN     3
#define L1C_LED_CTRL_DUTY_CYCL_MASK     ASHFT0(3UL)
#define L1C_LED_CTRL_DUTY_CYCL_SHIFT    0
#define L1C_LED_CTRL_DUTY_CYCL_50       0           /* 50% */
#define L1C_LED_CTRL_DUTY_CYCL_125      1           /* 12.5% */
#define L1C_LED_CTRL_DUTY_CYCL_25       2           /* 25% */
#define L1C_LED_CTRL_DUTY_CYCL_75       3           /* 75% */

#define L1C_LED_PATN                    0x142C
#define L1C_LED_PATN1_MASK              ASHFT16(0xFFFFUL)
#define L1C_LED_PATN1_SHIFT             16
#define L1C_LED_PATN0_MASK              ASHFT0(0xFFFFUL)
#define L1C_LED_PATN0_SHIFT             0

#define L1C_LED_PATN2                   0x1430
#define L1C_LED_PATN2_MASK              ASHFT0(0xFFFFUL)
#define L1C_LED_PATN2_SHIFT             0

#define L1C_SYSALV                      0x1434
#define L1C_SYSALV_FLAG                 BIT(0)

#define L1C_PCIERR_INST                 0x1438
#define L1C_PCIERR_INST_TX_RATE_MASK    ASHFT4(0xFUL)
#define L1C_PCIERR_INST_TX_RATE_SHIFT   4
#define L1C_PCIERR_INST_RX_RATE_MASK    ASHFT0(0xFUL)
#define L1C_PCIERR_INST_RX_RATE_SHIFT   0

#define L1C_LPI_DECISN_TIMER            0x143C
#define L1C_LPI_DESISN_TIMER_L2CB       0x7D00

#define L1C_LPI_CTRL                    0x1440
#define L1C_LPI_CTRL_CHK_DA             BIT(31)
#define L1C_LPI_CTRL_ENH_TO_MASK        ASHFT12(0x1FFFUL)
#define L1C_LPI_CTRL_ENH_TO_SHIFT       12
#define L1C_LPI_CTRL_ENH_TH_MASK        ASHFT6(0x1FUL)
#define L1C_LPI_CTRL_ENH_TH_SHIFT       6
#define L1C_LPI_CTRL_ENH_EN             BIT(5)
#define L1C_LPI_CTRL_CHK_RX             BIT(4)
#define L1C_LPI_CTRL_CHK_STATE          BIT(3)
#define L1C_LPI_CTRL_GMII               BIT(2)
#define L1C_LPI_CTRL_TO_PHY             BIT(1)
#define L1C_LPI_CTRL_EN                 BIT(0)

#define L1C_LPI_WAIT                    0x1444
#define L1C_LPI_WAIT_TIMER_MASK         ASHFT0(0xFFFFUL)
#define L1C_LPI_WAIT_TIMER_SHIFT        0

#define L1C_MAC_CTRL                    0x1480
#define L1C_MAC_CTRL_WOLSPED_SWEN       BIT(30)  /* 0:phy,1:sw */
#define L1C_MAC_CTRL_MHASH_ALG_HI5B     BIT(29)  /* 1:legacy, 0:marvl(low5b)*/
#define L1C_MAC_CTRL_SPAUSE_EN          BIT(28)
#define L1C_MAC_CTRL_DBG_EN             BIT(27)
#define L1C_MAC_CTRL_BRD_EN             BIT(26)
#define L1C_MAC_CTRL_MULTIALL_EN        BIT(25)
#define L1C_MAC_CTRL_RX_XSUM_EN         BIT(24)
#define L1C_MAC_CTRL_THUGE              BIT(23)
#define L1C_MAC_CTRL_MBOF               BIT(22)
#define L1C_MAC_CTRL_SPEED_MASK         ASHFT20(3UL)
#define L1C_MAC_CTRL_SPEED_SHIFT        20
#define L1C_MAC_CTRL_SPEED_10_100       1
#define L1C_MAC_CTRL_SPEED_1000         2
#define L1C_MAC_CTRL_SIMR               BIT(19)
#define L1C_MAC_CTRL_SSTCT              BIT(17)
#define L1C_MAC_CTRL_TPAUSE             BIT(16)
#define L1C_MAC_CTRL_PROMISC_EN         BIT(15)
#define L1C_MAC_CTRL_VLANSTRIP          BIT(14)
#define L1C_MAC_CTRL_PRMBLEN_MASK       ASHFT10(0xFUL)
#define L1C_MAC_CTRL_PRMBLEN_SHIFT      10
#define L1C_MAC_CTRL_RHUGE_EN           BIT(9)
#define L1C_MAC_CTRL_FLCHK              BIT(8)
#define L1C_MAC_CTRL_PCRCE              BIT(7)
#define L1C_MAC_CTRL_CRCE               BIT(6)
#define L1C_MAC_CTRL_FULLD              BIT(5)
#define L1C_MAC_CTRL_LPBACK_EN          BIT(4)
#define L1C_MAC_CTRL_RXFC_EN            BIT(3)
#define L1C_MAC_CTRL_TXFC_EN            BIT(2)
#define L1C_MAC_CTRL_RX_EN              BIT(1)
#define L1C_MAC_CTRL_TX_EN              BIT(0)

#define L1C_GAP                         0x1484
#define L1C_GAP_IPGR2_MASK              ASHFT24(0x7FUL)
#define L1C_GAP_IPGR2_SHIFT             24
#define L1C_GAP_IPGR1_MASK              ASHFT16(0x7FUL)
#define L1C_GAP_IPGR1_SHIFT             16
#define L1C_GAP_MIN_IFG_MASK            ASHFT8(0xFFUL)
#define L1C_GAP_MIN_IFG_SHIFT           8
#define L1C_GAP_IPGT_MASK               ASHFT0(0x7FUL)
#define L1C_GAP_IPGT_SHIFT              0

#define L1C_STAD0                       0x1488
#define L1C_STAD1                       0x148C

#define L1C_HASH_TBL0                   0x1490
#define L1C_HASH_TBL1                   0x1494

#define L1C_HALFD                       0x1498
#define L1C_HALFD_JAM_IPG_MASK          ASHFT24(0xFUL)
#define L1C_HALFD_JAM_IPG_SHIFT         24
#define L1C_HALFD_ABEBT_MASK            ASHFT20(0xFUL)
#define L1C_HALFD_ABEBT_SHIFT           20
#define L1C_HALFD_ABEBE                 BIT(19)
#define L1C_HALFD_BPNB                  BIT(18)
#define L1C_HALFD_NOBO                  BIT(17)
#define L1C_HALFD_EDXSDFR               BIT(16)
#define L1C_HALFD_RETRY_MASK            ASHFT12(0xFUL)
#define L1C_HALFD_RETRY_SHIFT           12
#define L1C_HALFD_LCOL_MASK             ASHFT0(0x3FFUL)
#define L1C_HALFD_LCOL_SHIFT            0

#define L1C_MTU                         0x149C
#define L1C_MTU_JUMBO_TH                1514
#define L1C_MTU_STD_ALGN                1536
#define L1C_MTU_MIN                     64

#define L1C_WOL0                        0x14A0
#define L1C_WOL0_PT7_MATCH              BIT(31)
#define L1C_WOL0_PT6_MATCH              BIT(30)
#define L1C_WOL0_PT5_MATCH              BIT(29)
#define L1C_WOL0_PT4_MATCH              BIT(28)
#define L1C_WOL0_PT3_MATCH              BIT(27)
#define L1C_WOL0_PT2_MATCH              BIT(26)
#define L1C_WOL0_PT1_MATCH              BIT(25)
#define L1C_WOL0_PT0_MATCH              BIT(24)
#define L1C_WOL0_PT7_EN                 BIT(23)
#define L1C_WOL0_PT6_EN                 BIT(22)
#define L1C_WOL0_PT5_EN                 BIT(21)
#define L1C_WOL0_PT4_EN                 BIT(20)
#define L1C_WOL0_PT3_EN                 BIT(19)
#define L1C_WOL0_PT2_EN                 BIT(18)
#define L1C_WOL0_PT1_EN                 BIT(17)
#define L1C_WOL0_PT0_EN                 BIT(16)
#define L1C_WOL0_IPV4_SYNC_EVT          BIT(14)
#define L1C_WOL0_IPV6_SYNC_EVT          BIT(13)
#define L1C_WOL0_LINK_EVT               BIT(10)
#define L1C_WOL0_MAGIC_EVT              BIT(9)
#define L1C_WOL0_PATTERN_EVT            BIT(8)
#define L1D_WOL0_OOB_EN                 BIT(6)
#define L1C_WOL0_PME_LINK               BIT(5)
#define L1C_WOL0_LINK_EN                BIT(4)
#define L1C_WOL0_PME_MAGIC_EN           BIT(3)
#define L1C_WOL0_MAGIC_EN               BIT(2)
#define L1C_WOL0_PME_PATTERN_EN         BIT(1)
#define L1C_WOL0_PATTERN_EN             BIT(0)

#define L1C_WOL1                        0x14A4
#define L1C_WOL1_PT3_LEN_MASK           ASHFT24(0xFFUL)
#define L1C_WOL1_PT3_LEN_SHIFT          24
#define L1C_WOL1_PT2_LEN_MASK           ASHFT16(0xFFUL)
#define L1C_WOL1_PT2_LEN_SHIFT          16
#define L1C_WOL1_PT1_LEN_MASK           ASHFT8(0xFFUL)
#define L1C_WOL1_PT1_LEN_SHIFT          8
#define L1C_WOL1_PT0_LEN_MASK           ASHFT0(0xFFUL)
#define L1C_WOL1_PT0_LEN_SHIFT          0

#define L1C_WOL2                        0x14A8
#define L1C_WOL2_PT7_LEN_MASK           ASHFT24(0xFFUL)
#define L1C_WOL2_PT7_LEN_SHIFT          24
#define L1C_WOL2_PT6_LEN_MASK           ASHFT16(0xFFUL)
#define L1C_WOL2_PT6_LEN_SHIFT          16
#define L1C_WOL2_PT5_LEN_MASK           ASHFT8(0xFFUL)
#define L1C_WOL2_PT5_LEN_SHIFT          8
#define L1C_WOL2_PT4_LEN_MASK           ASHFT0(0xFFUL)
#define L1C_WOL2_PT4_LEN_SHIFT          0

#define L1C_SRAM0                       0x1500
#define L1C_SRAM_RFD_TAIL_ADDR_MASK     ASHFT16(0xFFFUL)
#define L1C_SRAM_RFD_TAIL_ADDR_SHIFT    16
#define L1C_SRAM_RFD_HEAD_ADDR_MASK     ASHFT0(0xFFFUL)
#define L1C_SRAM_RFD_HEAD_ADDR_SHIFT    0
#define L1C_SRAM_RFD_HT_L2CB1           0x02bf02a0L

#define L1C_SRAM1                       0x1510
#define L1C_SRAM_RFD_LEN_MASK           ASHFT0(0xFFFUL) /* 8BYTES UNIT */
#define L1C_SRAM_RFD_LEN_SHIFT          0

#define L1C_SRAM2                       0x1518
#define L1C_SRAM_TRD_TAIL_ADDR_MASK     ASHFT16(0xFFFUL)
#define L1C_SRAM_TRD_TAIL_ADDR_SHIFT    16
#define L1C_SRMA_TRD_HEAD_ADDR_MASK     ASHFT0(0xFFFUL)
#define L1C_SRAM_TRD_HEAD_ADDR_SHIFT    0
#define L1C_SRAM_TRD_HT_L2CB1           0x03df03c0L

#define L1C_SRAM3                       0x151C
#define L1C_SRAM_TRD_LEN_MASK           ASHFT0(0xFFFUL) /* 8BYTES UNIT */
#define L1C_SRAM_TRD_LEN_SHIFT          0

#define L1C_SRAM4                       0x1520
#define L1C_SRAM_RXF_TAIL_ADDR_MASK     ASHFT16(0xFFFUL)
#define L1C_SRAM_RXF_TAIL_ADDR_SHIFT    16
#define L1C_SRAM_RXF_HEAD_ADDR_MASK     ASHFT0(0xFFFUL)
#define L1C_SRAM_RXF_HEAD_ADDR_SHIFT    0
#define L1C_SRAM_RXF_HT_L2CB1           0x029f0000L

#define L1C_SRAM5                       0x1524
#define L1C_SRAM_RXF_LEN_MASK           ASHFT0(0xFFFUL) /* 8BYTES UNIT */
#define L1C_SRAM_RXF_LEN_SHIFT          0
#define L1C_SRAM_RXF_LEN_8K             (8*1024)
#define L1C_SRAM_RXF_LEN_L2CB1          0x02a0L

#define L1C_SRAM6                       0x1528
#define L1C_SRAM_TXF_TAIL_ADDR_MASK     ASHFT16(0xFFFUL)
#define L1C_SRAM_TXF_TAIL_ADDR_SHIFT    16
#define L1C_SRAM_TXF_HEAD_ADDR_MASK     ASHFT0(0xFFFUL)
#define L1C_SRAM_TXF_HEAD_ADDR_SHIFT    0
#define L1C_SRAM_TXF_HT_L2CB1           0x03bf02c0L

#define L1C_SRAM7                       0x152C
#define L1C_SRAM_TXF_LEN_MASK           ASHFT0(0xFFFUL) /* 8BYTES UNIT */
#define L1C_SRAM_TXF_LEN_SHIFT          0
#define L1C_SRAM_TXF_LEN_L2CB1          0x0100L

#define L1C_SRAM8                       0x1530
#define L1C_SRAM_PATTERN_ADDR_MASK      ASHFT16(0xFFFUL)
#define L1C_SRAM_PATTERN_ADDR_SHIFT     16
#define L1C_SRAM_TSO_ADDR_MASK          ASHFT0(0xFFFUL)
#define L1C_SRAM_TSO_ADDR_SHIFT         0

#define L1C_SRAM9                       0x1534
#define L1C_SRAM_LOAD_PTR               BIT(0)

#define L1C_RX_BASE_ADDR_HI             0x1540

#define L1C_TX_BASE_ADDR_HI             0x1544

#define L1C_RFD_ADDR_LO                 0x1550
#define L1C_RFD_RING_SZ                 0x1560
#define L1C_RFD_BUF_SZ                  0x1564
#define L1C_RFD_BUF_SZ_MASK             ASHFT0(0xFFFFUL)
#define L1C_RFD_BUF_SZ_SHIFT            0

#define L1C_RRD_ADDR_LO                 0x1568
#define L1C_RRD_RING_SZ                 0x1578
#define L1C_RRD_RING_SZ_MASK            ASHFT0(0xFFFUL)
#define L1C_RRD_RING_SZ_SHIFT           0

#define L1C_TPD_PRI1_ADDR_LO            0x157C
#define L1C_TPD_PRI0_ADDR_LO            0x1580      /* LOWEST PRORITY */

#define L1C_TPD_PRI1_PIDX               0x15F0      /* 16BIT */
#define L1C_TPD_PRI0_PIDX               0x15F2      /* 16BIT */

#define L1C_TPD_PRI1_CIDX               0x15F4      /* 16BIT */
#define L1C_TPD_PRI0_CIDX               0x15F6      /* 16BIT */

#define L1C_TPD_RING_SZ                 0x1584
#define L1C_TPD_RING_SZ_MASK            ASHFT0(0xFFFFUL)
#define L1C_TPD_RING_SZ_SHIFT           0

#define L1C_TXQ0                        0x1590
#define L1C_TXQ0_TXF_BURST_PREF_MASK    ASHFT16(0xFFFFUL)
#define L1C_TXQ0_TXF_BURST_PREF_SHIFT   16
#define L1C_TXQ0_TXF_BURST_PREF_DEF     0x200
#define L1C_TXQ0_TXF_BURST_PREF_L2CB    0x40
#define L1D_TXQ0_PEDING_CLR             BIT(8)
#define L1C_TXQ0_LSO_8023_EN            BIT(7)
#define L1C_TXQ0_MODE_ENHANCE           BIT(6)
#define L1C_TXQ0_EN                     BIT(5)
#define L1C_TXQ0_SUPT_IPOPT             BIT(4)
#define L1C_TXQ0_TPD_BURSTPREF_MASK     ASHFT0(0xFUL)
#define L1C_TXQ0_TPD_BURSTPREF_SHIFT    0
#define L1C_TXQ0_TPD_BURSTPREF_DEF      5

#define L1C_TXQ1                        0x1594
#define L1C_TXQ1_JUMBO_TSOTHR_MASK      ASHFT0(0x7FFUL) /* 8BYTES UNIT */
#define L1C_TXQ1_JUMBO_TSOTHR_SHIFT     0
#define L1C_TXQ1_JUMBO_TSO_TH           (7*1024)    /* byte */

#define L1C_TXQ2                        0x1598          /* ENTER L1 CONTROL */
#define L1C_TXQ2_BURST_EN               BIT(31)
#define L1C_TXQ2_BURST_HI_WM_MASK       ASHFT16(0xFFFUL)
#define L1C_TXQ2_BURST_HI_WM_SHIFT      16
#define L1C_TXQ2_BURST_LO_WM_MASK       ASHFT0(0xFFFUL)
#define L1C_TXQ2_BURST_LO_WM_SHIFT      0

#define L1C_RFD_PIDX                    0x15E0
#define L1C_RFD_PIDX_MASK               ASHFT0(0xFFFUL)
#define L1C_RFD_PIDX_SHIFT              0

#define L1C_RFD_CIDX                    0x15F8
#define L1C_RFD_CIDX_MASK               ASHFT0(0xFFFUL)
#define L1C_RFD_CIDX_SHIFT              0

#define L1C_RXQ0                        0x15A0
#define L1C_RXQ0_EN                     BIT(31)
#define L1C_RXQ0_CUT_THRU_EN            BIT(30)
#define L1C_RXQ0_RSS_HASH_EN            BIT(29)
#define L1C_RXQ0_NON_IP_QTBL            BIT(28)  /* 0:q0,1:table */
#define L1C_RXQ0_RSS_MODE_MASK          ASHFT26(3UL)
#define L1C_RXQ0_RSS_MODE_SHIFT         26
#define L1C_RXQ0_RSS_MODE_DIS           0
#define L1C_RXQ0_RSS_MODE_SQSI          1
#define L1C_RXQ0_RSS_MODE_MQSI          2
#define L1C_RXQ0_RSS_MODE_MQMI          3
#define L1C_RXQ0_NUM_RFD_PREF_MASK      ASHFT20(0x3FUL)
#define L1C_RXQ0_NUM_RFD_PREF_SHIFT     20
#define L1C_RXQ0_NUM_RFD_PREF_DEF       8
#define L1C_RXQ0_RSS_HSTYP_IPV6_TCP_EN  BIT(19)
#define L1C_RXQ0_RSS_HSTYP_IPV6_EN      BIT(18)
#define L1C_RXQ0_RSS_HSTYP_IPV4_TCP_EN  BIT(17)
#define L1C_RXQ0_RSS_HSTYP_IPV4_EN      BIT(16)
#define L1C_RXQ0_RSS_HSTYP_ALL          (\
	L1C_RXQ0_RSS_HSTYP_IPV6_TCP_EN  |\
	L1C_RXQ0_RSS_HSTYP_IPV4_TCP_EN  |\
	L1C_RXQ0_RSS_HSTYP_IPV6_EN      |\
	L1C_RXQ0_RSS_HSTYP_IPV4_EN)
#define L1C_RXQ0_IDT_TBL_SIZE_MASK      ASHFT8(0xFFUL)
#define L1C_RXQ0_IDT_TBL_SIZE_SHIFT     8
#define L1C_RXQ0_IDT_TBL_SIZE_DEF       0x80
#define L1C_RXQ0_IPV6_PARSE_EN          BIT(7)
#define L1C_RXQ0_ASPM_THRESH_MASK       ASHFT0(3UL)
#define L1C_RXQ0_ASPM_THRESH_SHIFT      0
#define L1C_RXQ0_ASPM_THRESH_NO         0
#define L1C_RXQ0_ASPM_THRESH_1M         1
#define L1C_RXQ0_ASPM_THRESH_10M        2
#define L1C_RXQ0_ASPM_THRESH_100M       3

#define L1C_RXQ1                        0x15A4
#define L1C_RXQ1_RFD_PREF_DOWN_MASK     ASHFT6(0x3FUL)
#define L1C_RXQ1_RFD_PREF_DOWN_SHIFT    6
#define L1C_RXQ1_RFD_PREF_UP_MASK       ASHFT0(0x3FUL)
#define L1C_RXQ1_RFD_PREF_UP_SHIFT      0

#define L1C_RXQ2                        0x15A8
/* XOFF: USED SRAM LOWER THAN IT, THEN NOTIFY THE PEER TO SEND AGAIN */
#define L1C_RXQ2_RXF_XOFF_THRESH_MASK   ASHFT16(0xFFFUL)
#define L1C_RXQ2_RXF_XOFF_THRESH_SHIFT  16
#define L1C_RXQ2_RXF_XON_THRESH_MASK    ASHFT0(0xFFFUL)
#define L1C_RXQ2_RXF_XON_THRESH_SHIFT   0

#define L1C_RXQ3                        0x15AC
#define L1C_RXQ3_RXD_TIMER_MASK         ASHFT16(0xFFFFUL)
#define L1C_RXQ3_RXD_TIMER_SHIFT        16
#define L1C_RXQ3_RXD_THRESH_MASK        ASHFT0(0xFFFUL) /* 8BYTES UNIT */
#define L1C_RXQ3_RXD_THRESH_SHIFT       0

#define L1C_DMA                         0x15C0
#define L1C_DMA_WPEND_CLR               BIT(30)
#define L1C_DMA_RPEND_CLR               BIT(29)
#define L1C_DMA_WDLY_CNT_MASK           ASHFT16(0xFUL)
#define L1C_DMA_WDLY_CNT_SHIFT          16
#define L1C_DMA_WDLY_CNT_DEF            4
#define L1C_DMA_RDLY_CNT_MASK           ASHFT11(0x1FUL)
#define L1C_DMA_RDLY_CNT_SHIFT          11
#define L1C_DMA_RDLY_CNT_DEF            15
#define L1C_DMA_RREQ_PRI_DATA           BIT(10)      /* 0:tpd, 1:data */
#define L1C_DMA_WREQ_BLEN_MASK          ASHFT7(7UL)
#define L1C_DMA_WREQ_BLEN_SHIFT         7
#define L1C_DMA_RREQ_BLEN_MASK          ASHFT4(7UL)
#define L1C_DMA_RREQ_BLEN_SHIFT         4
#define L1C_DMA_RCB_LEN128              BIT(3)   /* 0:64bytes,1:128bytes */
#define L1C_DMA_RORDER_MODE_MASK        ASHFT0(7UL)
#define L1C_DMA_RORDER_MODE_SHIFT       0
#define L1C_DMA_RORDER_MODE_OUT         4
#define L1C_DMA_RORDER_MODE_ENHANCE     2
#define L1C_DMA_RORDER_MODE_IN          1

#define L1C_SMB_TIMER                   0x15C4

#define L1C_TINT_TPD_THRSHLD            0x15C8

#define L1C_TINT_TIMER                  0x15CC

#define L1C_ISR                         0x1600
#define L1C_ISR_DIS                     BIT(31)
#define L1C_ISR_PCIE_LNKDOWN            BIT(26)
#define L1C_ISR_PCIE_CERR               BIT(25)
#define L1C_ISR_PCIE_NFERR              BIT(24)
#define L1C_ISR_PCIE_FERR               BIT(23)
#define L1C_ISR_PCIE_UR                 BIT(22)
#define L1C_ISR_MAC_TX                  BIT(21)
#define L1C_ISR_MAC_RX                  BIT(20)
#define L1C_ISR_RX_Q0                   BIT(16)
#define L1C_ISR_TX_Q0                   BIT(15)
#define L1C_ISR_TXQ_TO                  BIT(14)
#define L1C_ISR_PHY_LPW                 BIT(13)
#define L1C_ISR_PHY                     BIT(12)
#define L1C_ISR_TX_CREDIT               BIT(11)
#define L1C_ISR_DMAW                    BIT(10)
#define L1C_ISR_DMAR                    BIT(9)
#define L1C_ISR_TXF_UR                  BIT(8)
#define L1C_ISR_RFD_UR                  BIT(4)
#define L1C_ISR_RXF_OV                  BIT(3)
#define L1C_ISR_MANU                    BIT(2)
#define L1C_ISR_TIMER                   BIT(1)
#define L1C_ISR_SMB                     BIT(0)

#define L1C_IMR                         0x1604

#define L1C_INT_RETRIG                  0x1608  /* re-send deassrt/assert
						 * if sw no reflect */
#define L1C_INT_RETRIG_TO               20000   /* 40 ms */

/* WOL mask register only for L1Dv2.0 and later chips */
#define L1D_PATTERN_MASK                0x1620  /* 128bytes, sleep state */
#define L1D_PATTERN_MASK_LEN            128     /* 128bytes, 32DWORDs */


#define L1C_BTROM_CFG                   0x1800  /* pwon rst */

#define L1C_DRV                         0x1804  /* pwon rst */
/* bit definition is in lx_hwcomm.h */

#define L1C_DRV_ERR1                    0x1808  /* perst */
#define L1C_DRV_ERR1_GEN                BIT(31)  /* geneneral err */
#define L1C_DRV_ERR1_NOR                BIT(30)  /* rrd.nor */
#define L1C_DRV_ERR1_TRUNC              BIT(29)
#define L1C_DRV_ERR1_RES                BIT(28)
#define L1C_DRV_ERR1_INTFATAL           BIT(27)
#define L1C_DRV_ERR1_TXQPEND            BIT(26)
#define L1C_DRV_ERR1_DMAW               BIT(25)
#define L1C_DRV_ERR1_DMAR               BIT(24)
#define L1C_DRV_ERR1_PCIELNKDWN         BIT(23)
#define L1C_DRV_ERR1_PKTSIZE            BIT(22)
#define L1C_DRV_ERR1_FIFOFUL            BIT(21)
#define L1C_DRV_ERR1_RFDUR              BIT(20)
#define L1C_DRV_ERR1_RRDSI              BIT(19)
#define L1C_DRV_ERR1_UPDATE             BIT(18)

#define L1C_DRV_ERR2                    0x180C  /* perst */

#define L1C_CLK_GATE                    0x1814
#define L1C_CLK_GATE_RXMAC              BIT(5)
#define L1C_CLK_GATE_TXMAC              BIT(4)
#define L1C_CLK_GATE_RXQ                BIT(3)
#define L1C_CLK_GATE_TXQ                BIT(2)
#define L1C_CLK_GATE_DMAR               BIT(1)
#define L1C_CLK_GATE_DMAW               BIT(0)
#define L1C_CLK_GATE_ALL    (\
	L1C_CLK_GATE_RXMAC  |\
	L1C_CLK_GATE_TXMAC  |\
	L1C_CLK_GATE_RXQ    |\
	L1C_CLK_GATE_TXQ    |\
	L1C_CLK_GATE_DMAR   |\
	L1C_CLK_GATE_DMAW)

#define L1C_DBG_ADDR                    0x1900  /* DWORD reg */
#define L1C_DBG_DATA                    0x1904  /* DWORD reg */

/***************************** IO mapping registers ***************************/
#define L1C_IO_ADDR                     0x00    /* DWORD reg */
#define L1C_IO_DATA                     0x04    /* DWORD reg */
#define L1C_IO_MASTER                   0x08    /* DWORD same as reg0x1400 */
#define L1C_IO_MAC_CTRL                 0x0C    /* DWORD same as reg0x1480*/
#define L1C_IO_ISR                      0x10    /* DWORD same as reg0x1600 */
#define L1C_IO_IMR                      0x14    /* DWORD same as reg0x1604 */
#define L1C_IO_TPD_PRI1_PIDX            0x18    /* WORD same as reg0x15F0 */
#define L1C_IO_TPD_PRI0_PIDX            0x1A    /* WORD same as reg0x15F2 */
#define L1C_IO_TPD_PRI1_CIDX            0x1C    /* WORD same as reg0x15F4 */
#define L1C_IO_TPD_PRI0_CIDX            0x1E    /* WORD same as reg0x15F6 */
#define L1C_IO_RFD_PIDX                 0x20    /* WORD same as reg0x15E0 */
#define L1C_IO_RFD_CIDX                 0x30    /* WORD same as reg0x15F8 */
#define L1C_IO_MDIO                     0x38    /* WORD same as reg0x1414 */
#define L1C_IO_PHY_CTRL                 0x3C    /* DWORD same as reg0x140C */



/********************* PHY regs definition ***************************/

/* Autoneg Advertisement Register (0x4) */
#define L1C_ADVERTISE_SPEED_MASK            0x01E0
#define L1C_ADVERTISE_DEFAULT_CAP           0x0DE0 /* diff with L1C */

/* 1000BASE-T Control Register (0x9) */
#define L1C_GIGA_CR_1000T_HD_CAPS           0x0100
#define L1C_GIGA_CR_1000T_FD_CAPS           0x0200
#define L1C_GIGA_CR_1000T_REPEATER_DTE      0x0400
#define L1C_GIGA_CR_1000T_MS_VALUE          0x0800
#define L1C_GIGA_CR_1000T_MS_ENABLE         0x1000
#define L1C_GIGA_CR_1000T_TEST_MODE_NORMAL  0x0000
#define L1C_GIGA_CR_1000T_TEST_MODE_1       0x2000
#define L1C_GIGA_CR_1000T_TEST_MODE_2       0x4000
#define L1C_GIGA_CR_1000T_TEST_MODE_3       0x6000
#define L1C_GIGA_CR_1000T_TEST_MODE_4       0x8000
#define L1C_GIGA_CR_1000T_SPEED_MASK        0x0300
#define L1C_GIGA_CR_1000T_DEFAULT_CAP       0x0300

/* 1000BASE-T Status Register */
#define L1C_MII_GIGA_SR                     0x0A

/* PHY Specific Status Register */
#define L1C_MII_GIGA_PSSR                   0x11
#define L1C_GIGA_PSSR_FC_RXEN               0x0004
#define L1C_GIGA_PSSR_FC_TXEN               0x0008
#define L1C_GIGA_PSSR_SPD_DPLX_RESOLVED     0x0800
#define L1C_GIGA_PSSR_DPLX                  0x2000
#define L1C_GIGA_PSSR_SPEED                 0xC000
#define L1C_GIGA_PSSR_10MBS                 0x0000
#define L1C_GIGA_PSSR_100MBS                0x4000
#define L1C_GIGA_PSSR_1000MBS               0x8000

/* PHY Interrupt Enable Register */
#define L1C_MII_IER                         0x12
#define L1C_IER_LINK_UP                     0x0400
#define L1C_IER_LINK_DOWN                   0x0800

/* PHY Interrupt Status Register */
#define L1C_MII_ISR                         0x13
#define L1C_ISR_LINK_UP                     0x0400
#define L1C_ISR_LINK_DOWN                   0x0800

/* Cable-Detect-Test Control Register */
#define L1C_MII_CDTC                        0x16
#define L1C_CDTC_EN                         1       /* sc */
#define L1C_CDTC_PAIR_MASK                  ASHFT8(3U)
#define L1C_CDTC_PAIR_SHIFT                 8


/* Cable-Detect-Test Status Register */
#define L1C_MII_CDTS                        0x1C
#define L1C_CDTS_STATUS_MASK                ASHFT8(3U)
#define L1C_CDTS_STATUS_SHIFT               8
#define L1C_CDTS_STATUS_NORMAL              0
#define L1C_CDTS_STATUS_SHORT               1
#define L1C_CDTS_STATUS_OPEN                2
#define L1C_CDTS_STATUS_INVALID             3

#define L1C_MII_DBG_ADDR                    0x1D
#define L1C_MII_DBG_DATA                    0x1E

/***************************** debug port *************************************/

#define L1C_MIIDBG_ANACTRL                  0x00
#define L1C_ANACTRL_CLK125M_DELAY_EN        BIT(15)
#define L1C_ANACTRL_VCO_FAST                BIT(14)
#define L1C_ANACTRL_VCO_SLOW                BIT(13)
#define L1C_ANACTRL_AFE_MODE_EN             BIT(12)
#define L1C_ANACTRL_LCKDET_PHY              BIT(11)
#define L1C_ANACTRL_LCKDET_EN               BIT(10)
#define L1C_ANACTRL_OEN_125M                BIT(9)
#define L1C_ANACTRL_HBIAS_EN                BIT(8)
#define L1C_ANACTRL_HB_EN                   BIT(7)
#define L1C_ANACTRL_SEL_HSP                 BIT(6)
#define L1C_ANACTRL_CLASSA_EN               BIT(5)
#define L1C_ANACTRL_MANUSWON_SWR_MASK       ASHFT2(3U)
#define L1C_ANACTRL_MANUSWON_SWR_SHIFT      2
#define L1C_ANACTRL_MANUSWON_SWR_2V         0
#define L1C_ANACTRL_MANUSWON_SWR_1P9V       1
#define L1C_ANACTRL_MANUSWON_SWR_1P8V       2
#define L1C_ANACTRL_MANUSWON_SWR_1P7V       3
#define L1C_ANACTRL_MANUSWON_BW3_4M         BIT(1)
#define L1C_ANACTRL_RESTART_CAL             BIT(0)
#define L1C_ANACTRL_DEF                     0x02EF


#define L1C_MIIDBG_SYSMODCTRL               0x04
#define L1C_SYSMODCTRL_IECHOADJ_PFMH_PHY    BIT(15)
#define L1C_SYSMODCTRL_IECHOADJ_BIASGEN     BIT(14)
#define L1C_SYSMODCTRL_IECHOADJ_PFML_PHY    BIT(13)
#define L1C_SYSMODCTRL_IECHOADJ_PS_MASK     ASHFT10(3U)
#define L1C_SYSMODCTRL_IECHOADJ_PS_SHIFT    10
#define L1C_SYSMODCTRL_IECHOADJ_PS_40       3
#define L1C_SYSMODCTRL_IECHOADJ_PS_20       2
#define L1C_SYSMODCTRL_IECHOADJ_PS_0        1
#define L1C_SYSMODCTRL_IECHOADJ_10BT_100MV  BIT(6) /* 1:100mv, 0:200mv */
#define L1C_SYSMODCTRL_IECHOADJ_HLFAP_MASK  ASHFT4(3U)
#define L1C_SYSMODCTRL_IECHOADJ_HLFAP_SHIFT 4
#define L1C_SYSMODCTRL_IECHOADJ_VDFULBW     BIT(3)
#define L1C_SYSMODCTRL_IECHOADJ_VDBIASHLF   BIT(2)
#define L1C_SYSMODCTRL_IECHOADJ_VDAMPHLF    BIT(1)
#define L1C_SYSMODCTRL_IECHOADJ_VDLANSW     BIT(0)
#define L1C_SYSMODCTRL_IECHOADJ_DEF         0x88BB /* ???? */



#define L1D_MIIDBG_SYSMODCTRL               0x04    /* l1d & l2cb */
#define L1D_SYSMODCTRL_IECHOADJ_CUR_ADD     BIT(15)
#define L1D_SYSMODCTRL_IECHOADJ_CUR_MASK    ASHFT12(7U)
#define L1D_SYSMODCTRL_IECHOADJ_CUR_SHIFT   12
#define L1D_SYSMODCTRL_IECHOADJ_VOL_MASK    ASHFT8(0xFU)
#define L1D_SYSMODCTRL_IECHOADJ_VOL_SHIFT   8
#define L1D_SYSMODCTRL_IECHOADJ_VOL_17ALL   3
#define L1D_SYSMODCTRL_IECHOADJ_VOL_100M15  1
#define L1D_SYSMODCTRL_IECHOADJ_VOL_10M17   0
#define L1D_SYSMODCTRL_IECHOADJ_BIAS1_MASK  ASHFT4(0xFU)
#define L1D_SYSMODCTRL_IECHOADJ_BIAS1_SHIFT 4
#define L1D_SYSMODCTRL_IECHOADJ_BIAS2_MASK  ASHFT0(0xFU)
#define L1D_SYSMODCTRL_IECHOADJ_BIAS2_SHIFT 0
#define L1D_SYSMODCTRL_IECHOADJ_DEF         0x4FBB


#define L1C_MIIDBG_SRDSYSMOD                0x05
#define L1C_SRDSYSMOD_LCKDET_EN             BIT(13)
#define L1C_SRDSYSMOD_PLL_EN                BIT(11)
#define L1C_SRDSYSMOD_SEL_HSP               BIT(10)
#define L1C_SRDSYSMOD_HLFTXDR               BIT(9)
#define L1C_SRDSYSMOD_TXCLK_DELAY_EN        BIT(8)
#define L1C_SRDSYSMOD_TXELECIDLE            BIT(7)
#define L1C_SRDSYSMOD_DEEMP_EN              BIT(6)
#define L1C_SRDSYSMOD_MS_PAD                BIT(2)
#define L1C_SRDSYSMOD_CDR_ADC_VLTG          BIT(1)
#define L1C_SRDSYSMOD_CDR_DAC_1MA           BIT(0)
#define L1C_SRDSYSMOD_DEF                   0x2C46

#define L1C_MIIDBG_CFGLPSPD                 0x0A
#define L1C_CFGLPSPD_RSTCNT_MASK            ASHFT(3U)
#define L1C_CFGLPSPD_RSTCNT_SHIFT           14
#define L1C_CFGLPSPD_RSTCNT_CLK125SW        BIT(13)

#define L1C_MIIDBG_HIBNEG                   0x0B
#define L1C_HIBNEG_PSHIB_EN                 BIT(15)
#define L1C_HIBNEG_WAKE_BOTH                BIT(14)
#define L1C_HIBNEG_ONOFF_ANACHG_SUDEN       BIT(13)
#define L1C_HIBNEG_HIB_PULSE                BIT(12)
#define L1C_HIBNEG_GATE_25M_EN              BIT(11)
#define L1C_HIBNEG_RST_80U                  BIT(10)
#define L1C_HIBNEG_RST_TIMER_MASK           ASHFT8(3U)
#define L1C_HIBNEG_RST_TIMER_SHIFT          8
#define L1C_HIBNEG_GTX_CLK_DELAY_MASK       ASHFT5(3U)
#define L1C_HIBNEG_GTX_CLK_DELAY_SHIFT      5
#define L1C_HIBNEG_BYPSS_BRKTIMER           BIT(4)
#define L1C_HIBNEG_DEF                      0xBC40

#define L1C_MIIDBG_TST10BTCFG               0x12
#define L1C_TST10BTCFG_INTV_TIMER_MASK      ASHFT14(3U)
#define L1C_TST10BTCFG_INTV_TIMER_SHIFT     14
#define L1C_TST10BTCFG_TRIGER_TIMER_MASK    ASHFT12(3U)
#define L1C_TST10BTCFG_TRIGER_TIMER_SHIFT   12
#define L1C_TST10BTCFG_DIV_MAN_MLT3_EN      BIT(11)
#define L1C_TST10BTCFG_OFF_DAC_IDLE         BIT(10)
#define L1C_TST10BTCFG_LPBK_DEEP            BIT(2) /* 1:deep,0:shallow */
#define L1C_TST10BTCFG_DEF                  0x4C04

#define L1C_MIIDBG_AZ_ANADECT               0x15
#define L1C_AZ_ANADECT_10BTRX_TH            BIT(15)
#define L1C_AZ_ANADECT_BOTH_01CHNL          BIT(14)
#define L1C_AZ_ANADECT_INTV_MASK            ASHFT8(0x3FU)
#define L1C_AZ_ANADECT_INTV_SHIFT           8
#define L1C_AZ_ANADECT_THRESH_MASK          ASHFT4(0xFU)
#define L1C_AZ_ANADECT_THRESH_SHIFT         4
#define L1C_AZ_ANADECT_CHNL_MASK            ASHFT0(0xFU)
#define L1C_AZ_ANADECT_CHNL_SHIFT           0
#define L1C_AZ_ANADECT_DEF                  0x3220
#define L1C_AZ_ANADECT_LONG                 0xb210

#define L1D_MIIDBG_MSE16DB                  0x18
#define L1D_MSE16DB_UP                      0x05EA
#define L1D_MSE16DB_DOWN                    0x02EA


#define L1C_MIIDBG_LEGCYPS                  0x29
#define L1C_LEGCYPS_EN                      BIT(15)
#define L1C_LEGCYPS_DAC_AMP1000_MASK        ASHFT12(7U)
#define L1C_LEGCYPS_DAC_AMP1000_SHIFT       12
#define L1C_LEGCYPS_DAC_AMP100_MASK         ASHFT9(7U)
#define L1C_LEGCYPS_DAC_AMP100_SHIFT        9
#define L1C_LEGCYPS_DAC_AMP10_MASK          ASHFT6(7U)
#define L1C_LEGCYPS_DAC_AMP10_SHIFT         6
#define L1C_LEGCYPS_UNPLUG_TIMER_MASK       ASHFT3(7U)
#define L1C_LEGCYPS_UNPLUG_TIMER_SHIFT      3
#define L1C_LEGCYPS_UNPLUG_DECT_EN          BIT(2)
#define L1C_LEGCYPS_ECNC_PS_EN              BIT(0)
#define L1D_LEGCYPS_DEF                     0x129D
#define L1C_LEGCYPS_DEF                     0x36DD

#define L1C_MIIDBG_TST100BTCFG              0x36
#define L1C_TST100BTCFG_NORMAL_BW_EN        BIT(15)
#define L1C_TST100BTCFG_BADLNK_BYPASS       BIT(14)
#define L1C_TST100BTCFG_SHORTCABL_TH_MASK   ASHFT8(0x3FU)
#define L1C_TST100BTCFG_SHORTCABL_TH_SHIFT  8
#define L1C_TST100BTCFG_LITCH_EN            BIT(7)
#define L1C_TST100BTCFG_VLT_SW              BIT(6)
#define L1C_TST100BTCFG_LONGCABL_TH_MASK    ASHFT0(0x3FU)
#define L1C_TST100BTCFG_LONGCABL_TH_SHIFT   0
#define L1C_TST100BTCFG_DEF                 0xE12C

#define L1C_MIIDBG_VOLT_CTRL                0x3B
#define L1C_VOLT_CTRL_CABLE1TH_MASK         ASHFT7(0x1FFU)
#define L1C_VOLT_CTRL_CABLE1TH_SHIFT        7
#define L1C_VOLT_CTRL_AMPCTRL_MASK          ASHFT5(3U)
#define L1C_VOLT_CTRL_AMPCTRL_SHIFT         5
#define L1C_VOLT_CTRL_SW_BYPASS             BIT(4)
#define L1C_VOLT_CTRL_SWLOWEST              BIT(3)
#define L1C_VOLT_CTRL_DACAMP10_MASK         ASHFT0(7U)
#define L1C_VOLT_CTRL_DACAMP10_SHIFT        0

#define L1C_MIIDBG_CABLE1TH_DET             0x3E
#define L1C_CABLE1TH_DET_EN                 BIT(15)

/***************************** extension **************************************/

/******* dev 3 *********/
#define L1C_MIIEXT_PCS                      3

#define L1C_MIIEXT_CLDCTRL3                 0x8003
#define L1C_CLDCTRL3_BP_CABLE1TH_DET_GT     BIT(15)
#define L1C_CLDCTRL3_AZ_DISAMP              BIT(12)
#define L1C_CLDCTRL3_L2CB                   0x4D19
#define L1C_CLDCTRL3_L1D                    0xDD19

#define L1C_MIIEXT_CLDCTRL6                 0x8006
#define L1C_CLDCTRL6_CAB_LEN_MASK           ASHFT0(0x1FFU)
#define L1C_CLDCTRL6_CAB_LEN_SHIFT          0
#define L1C_CLDCTRL6_CAB_LEN_SHORT          0x50

#define L1C_MIIEXT_CLDCTRL7                 0x8007
#define L1C_CLDCTRL7_VDHLF_BIAS_TH_MASK     ASHFT9(0x7FU)
#define L1C_CLDCTRL7_VDHLF_BIAS_TH_SHIFT    9
#define L1C_CLDCTRL7_AFE_AZ_MASK            ASHFT4(0x1FU)
#define L1C_CLDCTRL7_AFE_AZ_SHIFT           4
#define L1C_CLDCTRL7_SIDE_PEAK_TH_MASK      ASHFT0(0xFU)
#define L1C_CLDCTRL7_SIDE_PEAK_TH_SHIFT     0
#define L1C_CLDCTRL7_DEF                    0x6BF6 /* ???? */
#define L1C_CLDCTRL7_FPGA_DEF               0x0005
#define L1C_CLDCTRL7_L2CB                   0x0175

#define L1C_MIIEXT_AZCTRL                   0x8008
#define L1C_AZCTRL_SHORT_TH_MASK            ASHFT8(0xFFU)
#define L1C_AZCTRL_SHORT_TH_SHIFT           8
#define L1C_AZCTRL_LONG_TH_MASK             ASHFT0(0xFFU)
#define L1C_AZCTRL_LONG_TH_SHIFT            0
#define L1C_AZCTRL_DEF                      0x1629
#define L1C_AZCTRL_FPGA_DEF                 0x101D
#define L1C_AZCTRL_L1D                      0x2034

#define L1C_MIIEXT_AZCTRL2                  0x8009
#define L1C_AZCTRL2_WAKETRNING_MASK         ASHFT8(0xFFU)
#define L1C_AZCTRL2_WAKETRNING_SHIFT        8
#define L1C_AZCTRL2_QUIET_TIMER_MASH        ASHFT6(3U)
#define L1C_AZCTRL2_QUIET_TIMER_SHIFT       6
#define L1C_AZCTRL2_PHAS_JMP2               BIT(4)
#define L1C_AZCTRL2_CLKTRCV_125MD16         BIT(3)
#define L1C_AZCTRL2_GATE1000_EN             BIT(2)
#define L1C_AZCTRL2_AVRG_FREQ               BIT(1)
#define L1C_AZCTRL2_PHAS_JMP4               BIT(0)
#define L1C_AZCTRL2_DEF                     0x32C0
#define L1C_AZCTRL2_FPGA_DEF                0x40C8
#define L1C_AZCTRL2_L2CB                    0xE003
#define L1C_AZCTRL2_L1D2                    0x18C0


#define L1C_MIIEXT_AZCTRL4                  0x800B
#define L1C_AZCTRL4_WAKE_STH_L2CB           0x0094

#define L1C_MIIEXT_AZCTRL5                  0x800C
#define L1C_AZCTRL5_WAKE_LTH_L2CB           0x00EB

#define L1C_MIIEXT_AZCTRL6                  0x800D
#define L1C_AZCTRL6_L1D2                    0x003F



/********* dev 7 **********/
#define L1C_MIIEXT_ANEG                     7

#define L1C_MIIEXT_LOCAL_EEEADV             0x3C
#define L1C_LOCAL_EEEADV_1000BT             BIT(2)
#define L1C_LOCAL_EEEADV_100BT              BIT(1)

#define L1C_MIIEXT_REMOTE_EEEADV            0x3D
#define L1C_REMOTE_EEEADV_1000BT            BIT(2)
#define L1C_REMOTE_EEEADV_100BT             BIT(1)

#define L1C_MIIEXT_EEE_ANEG                 0x8000
#define L1C_EEE_ANEG_1000M                  BIT(2)
#define L1C_EEE_ANEG_100M                   BIT(1)




/******************************************************************************/

/* functions */

/* get permanent mac address from
 * return
 *    0: success
 *    non-0:fail
 */
u16 l1c_get_perm_macaddr(struct alx_hw *hw, u8 *addr);


/* reset mac & dma
 * return
 *     0: success
 *     non-0:fail
 */
u16 l1c_reset_mac(struct alx_hw *hw);

/* reset phy
 * return
 *    0: success
 *    non-0:fail
 */
u16 l1c_reset_phy(struct alx_hw *hw, bool pws_en, bool az_en, bool ptp_en);


/* reset pcie
 * just reset pcie relative registers (pci command, clk, aspm...)
 * return
 *    0:success
 *    non-0:fail
 */
u16 l1c_reset_pcie(struct alx_hw *hw, bool l0s_en, bool l1_en);


/* disable/enable MAC/RXQ/TXQ
 * en
 *    true:enable
 *    false:disable
 * return
 *    0:success
 *    non-0-fail
 */
u16 l1c_enable_mac(struct alx_hw *hw, bool en, u16 en_ctrl);

/* enable/disable aspm support
 * that will change settings for phy/mac/pcie
 */
u16 l1c_enable_aspm(struct alx_hw *hw, bool l0s_en, bool l1_en, u8 lnk_stat);


/* initialize phy for speed / flow control
 * lnk_cap
 *    if autoNeg, is link capability to tell the peer
 *    if force mode, is forced speed/duplex
 */
u16 l1c_init_phy_spdfc(struct alx_hw *hw, bool auto_neg,
		       u8 lnk_cap, bool fc_en);

/* do post setting on phy if link up/down event occur
 */
u16 l1c_post_phy_link(struct alx_hw *hw, bool linkon, u8 wire_spd);


/* do power saving setting befor enter suspend mode
 * NOTE:
 *    1. phy link must be established before calling this function
 *    2. wol option (pattern,magic,link,etc.) is configed before call it.
 */
u16 l1c_powersaving(struct alx_hw *hw, u8 wire_spd, bool wol_en,
		    bool mac_txen, bool mac_rxen, bool pws_en);


/* read phy register */
u16 l1c_read_phy(struct alx_hw *hw, bool ext, u8 dev, bool fast, u16 reg,
		 u16 *data);

/* write phy register */
u16 l1c_write_phy(struct alx_hw *hw, bool ext, u8 dev,  bool fast, u16 reg,
		  u16 data);

/* phy debug port */
u16 l1c_read_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 *data);
u16 l1c_write_phydbg(struct alx_hw *hw, bool fast, u16 reg, u16 data);

/* check the configuration of the PHY */
u16 l1c_get_phy_config(struct alx_hw *hw);

/*
 * initialize mac basically
 *  most of hi-feature no init
 *      MAC/PHY should be reset before call this function
 */
u16 l1c_init_mac(struct alx_hw *hw, u8 *addr, u32 txmem_hi,
		 u32 *tx_mem_lo, u8 tx_qnum, u16 txring_sz,
		 u32 rxmem_hi, u32 rfdmem_lo, u32 rrdmem_lo,
		 u16 rxring_sz, u16 rxbuf_sz, u16 smb_timer,
		 u16 mtu, u16 int_mod, bool hash_legacy);



#endif/*L1C_HW_H_*/

