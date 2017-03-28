#ifndef __BACKPORT_LINUX_CPUMASK_H
#define __BACKPORT_LINUX_CPUMASK_H
#include_next <linux/cpumask.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28)
typedef struct cpumask { DECLARE_BITMAP(bits, NR_CPUS); } compat_cpumask_t;
#endif

#endif /* __BACKPORT_LINUX_CPUMASK_H */
