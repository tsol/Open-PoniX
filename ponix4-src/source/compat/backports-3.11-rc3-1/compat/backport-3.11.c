/*
 * Copyright (c) 2013  Hauke Mehrtens <hauke@hauke-m.de>
 *
 * Backport functionality introduced in Linux 3.11.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/export.h>

#ifdef CONFIG_MTRR

#include <asm/mtrr.h>
#include <asm/cpufeature.h>
#include <linux/io.h>
#include <linux/printk.h>

/* arch_phys_wc_add returns an MTRR register index plus this offset. */
#define MTRR_TO_PHYS_WC_OFFSET 1000

/**
 * arch_phys_wc_add - add a WC MTRR and handle errors if PAT is unavailable
 * @base: Physical base address
 * @size: Size of region
 *
 * If PAT is available, this does nothing.  If PAT is unavailable, it
 * attempts to add a WC MTRR covering size bytes starting at base and
 * logs an error if this fails.
 *
 * Drivers must store the return value to pass to mtrr_del_wc_if_needed,
 * but drivers should not try to interpret that return value.
 */
int arch_phys_wc_add(unsigned long base, unsigned long size)
{
	int ret;

#if defined(CONFIG_X86_PAT)
	if (cpu_has_pat)
		return 0;
#endif

	ret = mtrr_add(base, size, MTRR_TYPE_WRCOMB, true);
	if (ret < 0) {
		pr_warn("Failed to add WC MTRR for [%p-%p]; performance may suffer.",
			(void *)base, (void *)(base + size - 1));
		return ret;
	}
	return ret + MTRR_TO_PHYS_WC_OFFSET;
}
EXPORT_SYMBOL_GPL(arch_phys_wc_add);

/*
 * arch_phys_wc_del - undoes arch_phys_wc_add
 * @handle: Return value from arch_phys_wc_add
 *
 * This cleans up after mtrr_add_wc_if_needed.
 *
 * The API guarantees that mtrr_del_wc_if_needed(error code) and
 * mtrr_del_wc_if_needed(0) do nothing.
 */
void arch_phys_wc_del(int handle)
{
	if (handle >= 1) {
		WARN_ON(handle < MTRR_TO_PHYS_WC_OFFSET);
		mtrr_del(handle - MTRR_TO_PHYS_WC_OFFSET, 0, 0);
	}
}
EXPORT_SYMBOL_GPL(arch_phys_wc_del);

/*
 * phys_wc_to_mtrr_index - translates arch_phys_wc_add's return value
 * @handle: Return value from arch_phys_wc_add
 *
 * This will turn the return value from arch_phys_wc_add into an mtrr
 * index suitable for debugging.
 *
 * Note: There is no legitimate use for this function, except possibly
 * in printk line.  Alas there is an illegitimate use in some ancient
 * drm ioctls.
 */
int phys_wc_to_mtrr_index(int handle)
{
	if (handle < MTRR_TO_PHYS_WC_OFFSET)
		return -1;
	else
		return handle - MTRR_TO_PHYS_WC_OFFSET;
}
EXPORT_SYMBOL_GPL(phys_wc_to_mtrr_index);

#endif /* CONFIG_MTRR */
