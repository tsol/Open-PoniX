#ifndef __BACKPORT_LINUX_IO_H
#define __BACKPORT_LINUX_IO_H
#include_next <linux/io.h>

/*
 * Some systems (x86 without PAT) have a somewhat reliable way to mark a
 * physical address range such that uncached mappings will actually
 * end up write-combining.  This facility should be used in conjunction
 * with pgprot_writecombine, ioremap-wc, or set_memory_wc, since it has
 * no effect if the per-page mechanisms are functional.
 * (On x86 without PAT, these functions manipulate MTRRs.)
 *
 * arch_phys_del_wc(0) or arch_phys_del_wc(any error code) is guaranteed
 * to have no effect.
 */
#ifndef arch_phys_wc_add
#ifdef CONFIG_MTRR
extern int __must_check arch_phys_wc_add(unsigned long base,
					 unsigned long size);
extern void arch_phys_wc_del(int handle);
#else
static inline int __must_check arch_phys_wc_add(unsigned long base,
						unsigned long size)
{
	return 0;  /* It worked (i.e. did nothing). */
}

static inline void arch_phys_wc_del(int handle)
{
}
#endif /* CONFIG_MTRR */

#define arch_phys_wc_add arch_phys_wc_add
#endif

#endif /* __BACKPORT_LINUX_IO_H */
