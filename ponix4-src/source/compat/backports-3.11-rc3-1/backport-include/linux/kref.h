#ifndef __BACKPORT_KREF_H
#define __BACKPORT_KREF_H
#include_next <linux/kref.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,8,0)
#include <linux/atomic.h>

/* This was backported to some kernels (e.g. 3.2.44 and 3.4.41), mask it */
#define kref_get_unless_zero LINUX_BACKPORT(kref_get_unless_zero)

/* This backports:
 *
 * commit 4b20db3de8dab005b07c74161cb041db8c5ff3a7
 * Author: Thomas Hellstrom <thellstrom@vmware.com>
 * Date:   Tue Nov 6 11:31:49 2012 +0000
 *
 *	kref: Implement kref_get_unless_zero v3
 */
static inline int __must_check kref_get_unless_zero(struct kref *kref)
{
	return atomic_add_unless(&kref->refcount, 1, 0);
}
#endif

#endif /* __BACKPORT_KREF_H */
