#ifndef __BACKPORT_LINUX_VMALLOC_H
#define __BACKPORT_LINUX_VMALLOC_H
#include_next <linux/vmalloc.h>
#include <linux/version.h>

/* avoid warnings due to b3bdda02aa547a0753b4fdbc105e86ef9046b30b */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25)
#define vfree(ptr) vfree((void *)(ptr))
#define vunmap(ptr) vunmap((void *)(ptr))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
#define vzalloc LINUX_BACKPORT(vzalloc)
extern void *vzalloc(unsigned long size);
#endif

#endif /* __BACKPORT_LINUX_VMALLOC_H */
