#ifndef __LINUX_PM_QOS_PARAMS_H
#define __LINUX_PM_QOS_PARAMS_H

#define PM_QOS_RESERVED 0
#define PM_QOS_CPU_DMA_LATENCY 1
#define PM_QOS_NETWORK_LATENCY 2
#define PM_QOS_NETWORK_THROUGHPUT 3

#define PM_QOS_NUM_CLASSES 4
#define PM_QOS_DEFAULT_VALUE -1

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
#include <linux/latency.h>

static inline int pm_qos_add_requirement(int qos, char *name, s32 value)
{
	set_acceptable_latency(name, value);
	return 0;
}

static inline void pm_qos_remove_requirement(int qos, char *name)
{
	remove_acceptable_latency(name);
}

#else

static inline int pm_qos_add_requirement(int qos, char *name, s32 value)
{
	return 0;
}

static inline void pm_qos_remove_requirement(int qos, char *name)
{
}

#endif /* >= 2.6.19 */

#endif /* __LINUX_PM_QOS_PARAMS_H */
