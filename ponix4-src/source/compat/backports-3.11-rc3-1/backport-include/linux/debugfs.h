#ifndef __BACKPORT_LINUX_DEBUGFS_H
#define __BACKPORT_LINUX_DEBUGFS_H
#include_next <linux/debugfs.h>
#include <linux/version.h>


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
#define debugfs_remove_recursive LINUX_BACKPORT(debugfs_remove_recursive)

#if defined(CONFIG_DEBUG_FS)
void debugfs_remove_recursive(struct dentry *dentry);
#else
static inline void debugfs_remove_recursive(struct dentry *dentry)
{ }
#endif
#endif /* < 2.6.27 */

#endif /* __BACKPORT_LINUX_DEBUGFS_H */
