#ifndef __BACKPORT_INCLUDE_CPUFREQ_H
#define __BACKPORT_INCLUDE_CPUFREQ_H
#include_next <linux/cpufreq.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,1,0)
#ifdef CONFIG_CPU_FREQ
#define cpufreq_quick_get_max LINUX_BACKPORT(cpufreq_quick_get_max)
unsigned int cpufreq_quick_get_max(unsigned int cpu);
#endif
#endif

#endif /* __BACKPORT_INCLUDE_CPUFREQ_H */
