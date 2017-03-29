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

#include <linux/interrupt.h>

#include "wil6210.h"
#include "wil6210_rgf.h"

/**
 * Theory of operation:
 *
 * There is ISR pseudo-cause register,
 * dma_rgf->DMA_RGF.PSEUDO_CAUSE.PSEUDO_CAUSE
 * Its bits represents OR'ed bits from 3 real ISR registers:
 * TX, RX, and MISC.
 *
 * Registers may be configured to either "write 1 to clear" or
 * "clear on read" mode
 *
 * When handling interrupt, one have to mask/unmask interrupts for the
 * real ISR registers, or hardware may malfunction.
 *
 */

#define WIL6210_IRQ_DISABLE	(0xFFFFFFFFUL)
#define WIL6210_IMC_RX		BIT_DMA_EP_RX_ICR_RX_DONE
#define WIL6210_IMC_TX		(BIT_DMA_EP_TX_ICR_TX_DONE | \
				BIT_DMA_EP_TX_ICR_TX_DONE_N(0))
#define WIL6210_IMC_MISC	(ISR_MISC_FW_READY | ISR_MISC_MBOX_EVT)

static inline u32 ioread32_and_clear(void __iomem *addr)
{
	u32 x = ioread32(addr);
#if !defined(WIL6210_ISR_COR)
	iowrite32(x, addr);
#endif
	return x;
}

static void wil6210_mask_irq(struct wil6210_priv *wil)
{
#if 0
	wil_info(wil, "%s()\n", __func__);
#endif
	clear_bit(wil_status_irqen, &wil->status);
	iowrite32(WIL6210_IRQ_DISABLE, wil->csr +
			HOSTADDR(RGF_DMA_EP_RX_ICR) +
			offsetof(struct RGF_ICR, IMS));
	iowrite32(WIL6210_IRQ_DISABLE, wil->csr +
			HOSTADDR(RGF_DMA_EP_TX_ICR) +
			offsetof(struct RGF_ICR, IMS));
	iowrite32(WIL6210_IRQ_DISABLE, wil->csr +
			HOSTADDR(RGF_DMA_EP_MISC_ICR) +
			offsetof(struct RGF_ICR, IMS));
}

static void wil6210_unmask_irq(struct wil6210_priv *wil)
{
	iowrite32(WIL6210_IMC_RX, wil->csr +
			HOSTADDR(RGF_DMA_EP_RX_ICR) +
			offsetof(struct RGF_ICR, IMC));
	iowrite32(WIL6210_IMC_TX, wil->csr +
			HOSTADDR(RGF_DMA_EP_TX_ICR) +
			offsetof(struct RGF_ICR, IMC));
	iowrite32(WIL6210_IMC_MISC, wil->csr +
			HOSTADDR(RGF_DMA_EP_MISC_ICR) +
			offsetof(struct RGF_ICR, IMC));
	set_bit(wil_status_irqen, &wil->status);
}

void wil6210_disable_irq(struct wil6210_priv *wil)
{
	wil6210_mask_irq(wil);
}

void wil6210_enable_irq(struct wil6210_priv *wil)
{
#if 0
	wil_info(wil, "%s()\n", __func__);
#endif
#if defined(WIL6210_ISR_COR)
	/* configure to Clear-On-Read */
	iowrite32(0xFFFFFFFFUL, wil->csr +
			HOSTADDR(RGF_DMA_EP_RX_ICR) +
			offsetof(struct RGF_ICR, ICC));
	iowrite32(0xFFFFFFFFUL, wil->csr +
			HOSTADDR(RGF_DMA_EP_TX_ICR) +
			offsetof(struct RGF_ICR, ICC));
	iowrite32(0xFFFFFFFFUL, wil->csr +
			HOSTADDR(RGF_DMA_EP_MISC_ICR) +
			offsetof(struct RGF_ICR, ICC));
#else
	iowrite32(0, wil->csr +
			HOSTADDR(RGF_DMA_EP_RX_ICR) +
			offsetof(struct RGF_ICR, ICC));
	iowrite32(0, wil->csr +
			HOSTADDR(RGF_DMA_EP_TX_ICR) +
			offsetof(struct RGF_ICR, ICC));
	iowrite32(0, wil->csr +
			HOSTADDR(RGF_DMA_EP_MISC_ICR) +
			offsetof(struct RGF_ICR, ICC));
#endif

	wil6210_unmask_irq(wil);
}

static irqreturn_t wil6210_irq_rx(int irq, void *cookie)
{
	struct wil6210_priv *wil = cookie;
	u32 isr = wil->isr_rx;
#if 0
	wil_info(wil, "ISR RX 0x%08x\n", isr);
#endif
	if (isr & BIT_DMA_EP_RX_ICR_RX_DONE) {
#if 0
		wil_info(wil, "RX done\n");
#endif
		isr &= ~BIT_DMA_EP_RX_ICR_RX_DONE;
		rx_handle(wil);
	}
	if (isr)
		wil_err(wil, "un-handled RX ISR bits 0x%08x\n", isr);
	return IRQ_HANDLED;
}

static irqreturn_t wil6210_irq_tx(int irq, void *cookie)
{
	struct wil6210_priv *wil = cookie;
	u32 isr = wil->isr_tx;
#if 0
	wil_info(wil, "ISR TX 0x%08x\n", isr);
#endif
	if (isr & BIT_DMA_EP_TX_ICR_TX_DONE) {
		int i;
#if 0
		wil_info(wil, "TX done\n");
#endif
		isr &= ~BIT_DMA_EP_TX_ICR_TX_DONE;
		for (i = 0; i < 24; i++) {
			u32 mask = BIT_DMA_EP_TX_ICR_TX_DONE_N(i);
			if (isr & mask) {
				isr &= ~mask;
#if 0
				wil_info(wil, "TX done(%i)\n", i);
#endif
				tx_complete(wil, i);
			}
		}
	}
	if (isr)
		wil_err(wil, "un-handled TX ISR bits 0x%08x\n", isr);
	return IRQ_HANDLED;
}

static irqreturn_t wil6210_irq_misc(int irq, void *cookie)
{
	struct wil6210_priv *wil = cookie;
	u32 isr = wil->isr_misc;
#if 0
	wil_info(wil, "ISR MISC 0x%08x\n", isr);
#endif
	if (isr & ISR_MISC_FW_READY) {
		wil_info(wil, "IRQ: FW ready\n");
		/**
		 * Actual FW ready indicated by the
		 * WMI_FW_READY_EVENTID
		 */
		isr &= ~ISR_MISC_FW_READY;
	}
	if (isr & ISR_MISC_MBOX_EVT) {
#if 0
		wil_info(wil, "MBOX event\n");
#endif
		wmi_recv_cmd(wil);
		isr &= ~ISR_MISC_MBOX_EVT;
	}
	if (isr)
		wil_err(wil, "un-handled MISC ISR bits 0x%08x\n", isr);
	return IRQ_HANDLED;
}

/**
 * thread IRQ handler
 */
static irqreturn_t wil6210_thread_irq(int irq, void *cookie)
{
	struct wil6210_priv *wil = cookie;
#if 0
	wil_info(wil, "Thread IRQ\n");
#endif
	/* Discover real IRQ cause */
	if (wil->isr_misc) {
		wil6210_irq_misc(irq, cookie);
		wil->isr_misc = 0;
	}
	wil6210_unmask_irq(wil);
	return IRQ_HANDLED;
}

static irqreturn_t wil6210_hardirq(int irq, void *cookie)
{
	irqreturn_t rc = IRQ_HANDLED;
	struct wil6210_priv *wil = cookie;
	u32 pseudo_cause = ioread32(wil->csr +
			HOSTADDR(RGF_DMA_PSEUDO_CAUSE));
	/**
	 * pseudo_cause is Clear-On-Read, no need to ACK
	 */
	if ((pseudo_cause == 0) || ((pseudo_cause & 0xff) == 0xff))
		return IRQ_NONE;
	/* FIXME: IRQ mask debug */
	if (!test_bit(wil_status_irqen, &wil->status)) {
		u32 icm_rx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_RX_ICR) +
				offsetof(struct RGF_ICR, ICM));
		u32 icr_rx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_RX_ICR) +
				offsetof(struct RGF_ICR, ICR));
		u32 imv_rx = ioread32(wil->csr +
				HOSTADDR(RGF_DMA_EP_RX_ICR) +
				offsetof(struct RGF_ICR, IMV));
		u32 icm_tx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_TX_ICR) +
				offsetof(struct RGF_ICR, ICM));
		u32 icr_tx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_TX_ICR) +
				offsetof(struct RGF_ICR, ICR));
		u32 imv_tx = ioread32(wil->csr +
				HOSTADDR(RGF_DMA_EP_TX_ICR) +
				offsetof(struct RGF_ICR, IMV));
		u32 icm_misc = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_MISC_ICR) +
				offsetof(struct RGF_ICR, ICM));
		u32 icr_misc = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_MISC_ICR) +
				offsetof(struct RGF_ICR, ICR));
		u32 imv_misc = ioread32(wil->csr +
				HOSTADDR(RGF_DMA_EP_MISC_ICR) +
				offsetof(struct RGF_ICR, IMV));
		wil_err(wil, "IRQ when it should be masked: pseudo 0x%08x\n"
				"Rx   icm:icr:imv 0x%08x 0x%08x 0x%08x\n"
				"Tx   icm:icr:imv 0x%08x 0x%08x 0x%08x\n"
				"Misc icm:icr:imv 0x%08x 0x%08x 0x%08x\n",
				pseudo_cause,
				icm_rx, icr_rx, imv_rx,
				icm_tx, icr_tx, imv_tx,
				icm_misc, icr_misc, imv_misc);
		return IRQ_NONE;
	}
	wil6210_mask_irq(wil);
	/* Discover real IRQ cause */
	/* All ISR regs configured Clear-On-Read, no need to ACK */
	if (pseudo_cause & BIT_DMA_PSEUDO_CAUSE_RX) {
		wil->isr_rx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_RX_ICR) +
				offsetof(struct RGF_ICR, ICR));
	}
	if (pseudo_cause & BIT_DMA_PSEUDO_CAUSE_TX) {
		wil->isr_tx = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_TX_ICR) +
				offsetof(struct RGF_ICR, ICR));
	}
	if (pseudo_cause & BIT_DMA_PSEUDO_CAUSE_MISC) {
		wil->isr_misc = ioread32_and_clear(wil->csr +
				HOSTADDR(RGF_DMA_EP_MISC_ICR) +
				offsetof(struct RGF_ICR, ICR));
		rc = IRQ_WAKE_THREAD;
	}
	/* process what to be done right in hard IRQ */
	if (wil->isr_rx) {
		if (wil6210_irq_rx(irq, cookie) == IRQ_WAKE_THREAD)
			rc = IRQ_WAKE_THREAD;
		else
			wil->isr_rx = 0;
	}
	if (wil->isr_tx) {
		if (wil6210_irq_tx(irq, cookie) == IRQ_WAKE_THREAD)
			rc = IRQ_WAKE_THREAD;
		else
			wil->isr_tx = 0;
	}
	/* if thread is requested, it will unmask IRQ */
	if (rc != IRQ_WAKE_THREAD)
		wil6210_unmask_irq(wil);
#if 0
	wil_info(wil, "Hard IRQ 0x%08x\n", pseudo_cause);
#endif
	return rc;
}

int wil6210_init_irq(struct wil6210_priv *wil, int irq)
{
	int rc;
	wil_info(wil, "%s()\n", __func__);
	/* TODO: handle multiple MSI */
	rc = request_threaded_irq(irq,
			wil6210_hardirq, wil6210_thread_irq,
			wil->n_msi ? 0 : IRQF_SHARED,
			WIL_NAME, wil);
	if (rc)
		return rc;
	wil6210_enable_irq(wil);
	return 0;
}

void wil6210_fini_irq(struct wil6210_priv *wil, int irq)
{
	wil_info(wil, "%s()\n", __func__);
	wil6210_disable_irq(wil);
	free_irq(irq, wil);
}
