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

#ifndef WIL6210_RGF_H
#define WIL6210_RGF_H

/**
 * Hardware registers map
 *
 */

/**
 * Mapping
 * RGF File      | Host addr    |  FW addr
 *               |              |
 * user_rgf      | 0x000000     | 0x880000
 *  dma_rgf      | 0x001000     | 0x881000
 * pcie_rgf      | 0x002000     | 0x882000
 *               |              |
 */

/* Where various structures placed in host address space */
#define WIL6210_FW_HOST_OFF      (0x880000UL)

#define HOSTADDR(fwaddr)        (fwaddr - WIL6210_FW_HOST_OFF)

/**
 * Interrupt control registers block
 *
 * each interrupt controlled by the same bit in all registers
 */
struct RGF_ICR {
	u32 ICC; /* Cause Control, RW: 0 - W1C, 1 - COR */
	u32 ICR; /* Cause, W1C/COR depending on ICC */
	u32 ICM; /* Cause masked (ICR & ~IMV), W1C/COR depending on ICC */
	u32 ICS; /* Cause Set, WO */
	u32 IMV; /* Mask, RW+S/C */
	u32 IMS; /* Mask Set, write 1 to set */
	u32 IMC; /* Mask Clear, write 1 to clear */
} __packed;

/* registers - FW addresses */
#define RGF_USER_USER_SCRATCH_PAD       (0x8802bc)
#define RGF_USER_USER_ICR               (0x880b4c) /* struct RGF_ICR */
	#define BIT_USER_USER_ICR_SW_INT_2  BIT(18)
#define RGF_USER_CLKS_CTL_SW_RST_MASK_0 (0x880b14)
#define RGF_USER_MAC_CPU_0              (0x8801fc)
#define RGF_USER_USER_CPU_0             (0x8801e0)
#define RGF_USER_CLKS_CTL_SW_RST_VEC_0  (0x880b04)
#define RGF_USER_CLKS_CTL_SW_RST_VEC_1  (0x880b08)
#define RGF_USER_CLKS_CTL_SW_RST_VEC_2  (0x880b0c)
#define RGF_USER_CLKS_CTL_SW_RST_VEC_3  (0x880b10)

#define RGF_DMA_PSEUDO_CAUSE            (0x881c68)
#define RGF_DMA_PSEUDO_CAUSE_MASK_SW    (0x881c6c)
#define RGF_DMA_PSEUDO_CAUSE_MASK_FW    (0x881c70)
	#define BIT_DMA_PSEUDO_CAUSE_RX     BIT(0)
	#define BIT_DMA_PSEUDO_CAUSE_TX     BIT(1)
	#define BIT_DMA_PSEUDO_CAUSE_MISC   BIT(2)

#define RGF_DMA_EP_TX_ICR               (0x881bb4) /* struct RGF_ICR */
	#define BIT_DMA_EP_TX_ICR_TX_DONE   BIT(0)
	#define BIT_DMA_EP_TX_ICR_TX_DONE_N(n)   BIT(n+1) /* n = [0..23] */
#define RGF_DMA_EP_RX_ICR               (0x881bd0) /* struct RGF_ICR */
	#define BIT_DMA_EP_RX_ICR_RX_DONE   BIT(0)
#define RGF_DMA_EP_MISC_ICR             (0x881bec) /* struct RGF_ICR */
	#define BIT_DMA_EP_MISC_ICR_FW_INT0      BIT(28)
	#define BIT_DMA_EP_MISC_ICR_FW_INT1      BIT(29)

/* popular locations */
#define HOST_MBOX   HOSTADDR(RGF_USER_USER_SCRATCH_PAD)
#define HOST_SW_INT (HOSTADDR(RGF_USER_USER_ICR) + \
	offsetof(struct RGF_ICR, ICS))
#define SW_INT_MBOX BIT_USER_USER_ICR_SW_INT_2

/* ISR register bits */
#define ISR_MISC_FW_READY BIT_DMA_EP_MISC_ICR_FW_INT0
#define ISR_MISC_MBOX_EVT BIT_DMA_EP_MISC_ICR_FW_INT1

#endif /* WIL6210_RGF_H */
