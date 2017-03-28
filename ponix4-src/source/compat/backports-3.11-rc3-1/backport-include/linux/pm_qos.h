#ifndef _COMPAT_LINUX_PM_QOS_H
#define _COMPAT_LINUX_PM_QOS_H 1

#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,25))
/*
 * Kernels >= 2.6.25 have pm-qos and its initialized as part of
 * the bootup process
 */
static inline int backport_pm_qos_power_init(void)
{
	return 0;
}

static inline int backport_pm_qos_power_deinit(void)
{
	return 0;
}
#else
/*
 * Backport work for QoS dependencies (kernel/pm_qos_params.c)
 * pm-qos stuff written by mark gross mgross@linux.intel.com.
 *
 * ipw2100 now makes use of:
 *
 * pm_qos_add_requirement(),
 * pm_qos_update_requirement() and
 * pm_qos_remove_requirement() from it
 *
 * mac80211 uses the network latency to determine if to enable or not
 * dynamic PS. mac80211 also and registers a notifier for when
 * the latency changes. Since older kernels do no thave pm-qos stuff
 * we just implement it completley here and register it upon cfg80211
 * init. I haven't tested ipw2100 on 2.6.24 though.
 *
 * This pm-qos implementation is copied verbatim from the kernel
 * written by mark gross mgross@linux.intel.com. You don't have
 * to do anythinig to use pm-qos except use the same exported
 * routines as used in newer kernels. The backport_pm_qos_power_init()
 * defned below is used by the compat module to initialize pm-qos.
 */
int backport_pm_qos_power_init(void);
int backport_pm_qos_power_deinit(void);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0))
#include_next <linux/pm_qos.h>
#else
#include <linux/pm_qos_params.h>
#endif /* (LINUX_VERSION_CODE >= KERNEL_VERSION(3,2,0)) */

#ifndef PM_QOS_DEFAULT_VALUE
#define PM_QOS_DEFAULT_VALUE -1
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36)
struct pm_qos_request_list {
	u32 qos;
	void *request;
};

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))

#define pm_qos_add_request(_req, _class, _value) do {			\
	(_req)->request = #_req;					\
	(_req)->qos = _class;						\
	pm_qos_add_requirement((_class), (_req)->request, (_value));	\
    } while(0)

#define pm_qos_update_request(_req, _value)				\
	pm_qos_update_requirement((_req)->qos, (_req)->request, (_value))

#define pm_qos_remove_request(_req)					\
	pm_qos_remove_requirement((_req)->qos, (_req)->request)

#else

#define pm_qos_add_request(_req, _class, _value) do {			\
	(_req)->request = pm_qos_add_request((_class), (_value));	\
    } while (0)

#define pm_qos_update_request(_req, _value)				\
	pm_qos_update_request((_req)->request, (_value))

#define pm_qos_remove_request(_req)					\
	pm_qos_remove_request((_req)->request)

#endif /* < 2.6.35 */
#endif /* < 2.6.36 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35)
#define pm_qos_request(_qos) pm_qos_requirement(_qos)
#endif

#endif	/* _COMPAT_LINUX_PM_QOS_H */
