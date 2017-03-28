#ifndef __BACKPORT_LINUX_PAGEMAP_H
#define __BACKPORT_LINUX_PAGEMAP_H
#include_next <linux/pagemap.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)
#include <asm/uaccess.h>
/*
 * This backports:
 *
 * commit f56f821feb7b36223f309e0ec05986bb137ce418
 * Author: Daniel Vetter <daniel.vetter@ffwll.ch>
 * Date:   Sun Mar 25 19:47:41 2012 +0200
 *
 *     mm: extend prefault helpers to fault in more than PAGE_SIZE
 *
 * The new functions are used by drm/i915 driver.
 *
 */

#define fault_in_multipages_writeable LINUX_BACKPORT(fault_in_multipages_writeable)
static inline int fault_in_multipages_writeable(char __user *uaddr, int size)
{
        int ret = 0;
        char __user *end = uaddr + size - 1;

        if (unlikely(size == 0))
                return ret;

        /*
         * Writing zeroes into userspace here is OK, because we know that if
         * the zero gets there, we'll be overwriting it.
         */
        while (uaddr <= end) {
                ret = __put_user(0, uaddr);
                if (ret != 0)
                        return ret;
                uaddr += PAGE_SIZE;
        }

        /* Check whether the range spilled into the next page. */
        if (((unsigned long)uaddr & PAGE_MASK) ==
                        ((unsigned long)end & PAGE_MASK))
                ret = __put_user(0, end);

        return ret;
}

#define fault_in_multipages_readable LINUX_BACKPORT(fault_in_multipages_readable)
static inline int fault_in_multipages_readable(const char __user *uaddr,
                                               int size)
{
        volatile char c;
        int ret = 0;
        const char __user *end = uaddr + size - 1;

        if (unlikely(size == 0))
                return ret;

        while (uaddr <= end) {
                ret = __get_user(c, uaddr);
                if (ret != 0)
                        return ret;
                uaddr += PAGE_SIZE;
        }

        /* Check whether the range spilled into the next page. */
        if (((unsigned long)uaddr & PAGE_MASK) ==
                        ((unsigned long)end & PAGE_MASK)) {
                ret = __get_user(c, end);
                (void)c;
        }

        return ret;
}
#endif /* < 3.5 */

#endif /* __BACKPORT_LINUX_PAGEMAP_H */
