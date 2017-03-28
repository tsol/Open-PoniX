#ifndef __BACKPORT_LINUX_HRTIMER_H
#define __BACKPORT_LINUX_HRTIMER_H
#include_next <linux/hrtimer.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)
#define ktime_get_monotonic_offset LINUX_BACKPORT(ktime_get_monotonic_offset)
extern ktime_t ktime_get_monotonic_offset(void);
#endif

#endif /* __BACKPORT_LINUX_HRTIMER_H */
