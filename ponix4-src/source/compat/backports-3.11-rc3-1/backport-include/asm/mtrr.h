#ifndef __BACKPORT_ASM_MTRR_H
#define __BACKPORT_ASM_MTRR_H
#include_next <asm/mtrr.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0))
/*
 * The following functions are for use by other drivers that cannot use
 * arch_phys_wc_add and arch_phys_wc_del.
 */
#ifdef CONFIG_MTRR
extern int phys_wc_to_mtrr_index(int handle);
#else
static inline int phys_wc_to_mtrr_index(int handle)
{
	return -1;
}
#endif /* CONFIG_MTRR */
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,11,0)) */

#endif /* __BACKPORT_ASM_MTRR_H */
