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

#ifndef _ALX_HWCOMMON_H_
#define _ALX_HWCOMMON_H_

#include <linux/bitops.h>
#include "alx_sw.h"


#define BIT_ALL	    0xffffffffUL

#define ASHFT31(_x)  ((_x) << 31)
#define ASHFT30(_x)  ((_x) << 30)
#define ASHFT29(_x)  ((_x) << 29)
#define ASHFT28(_x)  ((_x) << 28)
#define ASHFT27(_x)  ((_x) << 27)
#define ASHFT26(_x)  ((_x) << 26)
#define ASHFT25(_x)  ((_x) << 25)
#define ASHFT24(_x)  ((_x) << 24)
#define ASHFT23(_x)  ((_x) << 23)
#define ASHFT22(_x)  ((_x) << 22)
#define ASHFT21(_x)  ((_x) << 21)
#define ASHFT20(_x)  ((_x) << 20)
#define ASHFT19(_x)  ((_x) << 19)
#define ASHFT18(_x)  ((_x) << 18)
#define ASHFT17(_x)  ((_x) << 17)
#define ASHFT16(_x)  ((_x) << 16)
#define ASHFT15(_x)  ((_x) << 15)
#define ASHFT14(_x)  ((_x) << 14)
#define ASHFT13(_x)  ((_x) << 13)
#define ASHFT12(_x)  ((_x) << 12)
#define ASHFT11(_x)  ((_x) << 11)
#define ASHFT10(_x)  ((_x) << 10)
#define ASHFT9(_x)   ((_x) << 9)
#define ASHFT8(_x)   ((_x) << 8)
#define ASHFT7(_x)   ((_x) << 7)
#define ASHFT6(_x)   ((_x) << 6)
#define ASHFT5(_x)   ((_x) << 5)
#define ASHFT4(_x)   ((_x) << 4)
#define ASHFT3(_x)   ((_x) << 3)
#define ASHFT2(_x)   ((_x) << 2)
#define ASHFT1(_x)   ((_x) << 1)
#define ASHFT0(_x)   ((_x) << 0)


#define FIELD_GETX(_x, _name)   (((_x) & (_name##_MASK)) >> (_name##_SHIFT))
#define FIELD_SETS(_x, _name, _v)   (\
(_x) =                               \
((_x) & ~(_name##_MASK))            |\
(((u16)(_v) << (_name##_SHIFT)) & (_name##_MASK)))
#define FIELD_SETL(_x, _name, _v)   (\
(_x) =                               \
((_x) & ~(_name##_MASK))            |\
(((u32)(_v) << (_name##_SHIFT)) & (_name##_MASK)))
#define FIELDL(_name, _v) (((u32)(_v) << (_name##_SHIFT)) & (_name##_MASK))
#define FIELDS(_name, _v) (((u16)(_v) << (_name##_SHIFT)) & (_name##_MASK))



#define LX_SWAP_DW(_x) (\
	(((_x) << 24) & 0xFF000000UL) |\
	(((_x) <<  8) & 0x00FF0000UL) |\
	(((_x) >>  8) & 0x0000FF00UL) |\
	(((_x) >> 24) & 0x000000FFUL))

#define LX_SWAP_W(_x) (\
	(((_x) >> 8) & 0x00FFU) |\
	(((_x) << 8) & 0xFF00U))


#define LX_ERR_SUCCESS          0x0000
#define LX_ERR_ALOAD            0x0001
#define LX_ERR_RSTMAC           0x0002
#define LX_ERR_PARM             0x0003
#define LX_ERR_MIIBUSY          0x0004

/* link capability */
#define LX_LC_10H               0x01
#define LX_LC_10F               0x02
#define LX_LC_100H              0x04
#define LX_LC_100F              0x08
#define LX_LC_1000F             0x10
#define LX_LC_ALL               \
	(LX_LC_10H|LX_LC_10F|LX_LC_100H|LX_LC_100F|LX_LC_1000F)

/* options for MAC contrl */
#define LX_MACSPEED_1000        BIT(0)  /* 1:1000M, 0:10/100M */
#define LX_MACDUPLEX_FULL       BIT(1)  /* 1:full, 0:half */
#define LX_FLT_BROADCAST        BIT(2)  /* 1:enable rx-broadcast */
#define LX_FLT_MULTI_ALL        BIT(3)
#define LX_FLT_DIRECT           BIT(4)
#define LX_FLT_PROMISC          BIT(5)
#define LX_FC_TXEN              BIT(6)
#define LX_FC_RXEN              BIT(7)
#define LX_VLAN_STRIP           BIT(8)
#define LX_LOOPBACK             BIT(9)
#define LX_ADD_FCS              BIT(10)
#define LX_SINGLE_PAUSE         BIT(11)


/* interop between drivers */
#define LX_DRV_TYPE_MASK                ASHFT27(0x1FUL)
#define LX_DRV_TYPE_SHIFT               27
#define LX_DRV_TYPE_UNKNOWN             0
#define LX_DRV_TYPE_BIOS                1
#define LX_DRV_TYPE_BTROM               2
#define LX_DRV_TYPE_PKT                 3
#define LX_DRV_TYPE_NDS2                4
#define LX_DRV_TYPE_UEFI                5
#define LX_DRV_TYPE_NDS5                6
#define LX_DRV_TYPE_NDS62               7
#define LX_DRV_TYPE_NDS63               8
#define LX_DRV_TYPE_LNX                 9
#define LX_DRV_TYPE_ODI16               10
#define LX_DRV_TYPE_ODI32               11
#define LX_DRV_TYPE_FRBSD               12
#define LX_DRV_TYPE_NTBSD               13
#define LX_DRV_TYPE_WCE                 14
#define LX_DRV_PHY_AUTO                 BIT(26)  /* 1:auto, 0:force */
#define LX_DRV_PHY_1000                 BIT(25)
#define LX_DRV_PHY_100                  BIT(24)
#define LX_DRV_PHY_10                   BIT(23)
#define LX_DRV_PHY_DUPLEX               BIT(22)  /* 1:full, 0:half */
#define LX_DRV_PHY_FC                   BIT(21)  /* 1:en flow control */
#define LX_DRV_PHY_MASK                 ASHFT21(0x1FUL)
#define LX_DRV_PHY_SHIFT                21
#define LX_DRV_PHY_UNKNOWN              0
#define LX_DRV_DISABLE                  BIT(18)
#define LX_DRV_WOLS5_EN                 BIT(17)
#define LX_DRV_WOLS5_BIOS_EN            BIT(16)
#define LX_DRV_AZ_EN                    BIT(12)
#define LX_DRV_WOLPATTERN_EN            BIT(11)
#define LX_DRV_WOLLINKUP_EN             BIT(10)
#define LX_DRV_WOLMAGIC_EN              BIT(9)
#define LX_DRV_WOLCAP_BIOS_EN           BIT(8)
#define LX_DRV_ASPM_SPD1000LMT_MASK     ASHFT4(3UL)
#define LX_DRV_ASPM_SPD1000LMT_SHIFT    4
#define LX_DRV_ASPM_SPD1000LMT_100M     0
#define LX_DRV_ASPM_SPD1000LMT_NO       1
#define LX_DRV_ASPM_SPD1000LMT_1M       2
#define LX_DRV_ASPM_SPD1000LMT_10M      3
#define LX_DRV_ASPM_SPD100LMT_MASK      ASHFT2(3UL)
#define LX_DRV_ASPM_SPD100LMT_SHIFT     2
#define LX_DRV_ASPM_SPD100LMT_1M        0
#define LX_DRV_ASPM_SPD100LMT_10M       1
#define LX_DRV_ASPM_SPD100LMT_100M      2
#define LX_DRV_ASPM_SPD100LMT_NO        3
#define LX_DRV_ASPM_SPD10LMT_MASK       ASHFT0(3UL)
#define LX_DRV_ASPM_SPD10LMT_SHIFT      0
#define LX_DRV_ASPM_SPD10LMT_1M         0
#define LX_DRV_ASPM_SPD10LMT_10M        1
#define LX_DRV_ASPM_SPD10LMT_100M       2
#define LX_DRV_ASPM_SPD10LMT_NO         3

/* flag of phy inited */
#define LX_PHY_INITED           0x003F

/* check if the mac address is valid */
#define macaddr_valid(_addr) (\
	((*(u8 *)(_addr))&1) == 0 && \
	!(*(u32 *)(_addr) == 0 && *((u16 *)(_addr)+2) == 0))

#define test_set_or_clear(_val, _ctrl, _flag, _bit)	\
do {							\
	if ((_ctrl) & (_flag))				\
		(_val) |= (_bit);			\
	else						\
		(_val) &= ~(_bit);			\
} while (0)


#endif/*_ALX_HWCOMMON_H_*/

