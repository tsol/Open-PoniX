#ifndef LINUX_3_4_COMPAT_H
#define LINUX_3_4_COMPAT_H

#include <linux/version.h>

#if (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0))


/* This backports:
 *
 * commit 63b2001169e75cd71e917ec953fdab572e3f944a
 * Author: Thomas Gleixner <tglx@linutronix.de>
 * Date:   Thu Dec 1 00:04:00 2011 +0100

 * 	sched/wait: Add __wake_up_all_locked() API
 */
#include <linux/wait.h>
extern void compat_wake_up_locked(wait_queue_head_t *q, unsigned int mode, int nr);
#define wake_up_all_locked(x)	compat_wake_up_locked((x), TASK_NORMAL, 0)

/* This backports:
 *
 * commit a8203725dfded5c1f79dca3368a4a273e24b59bb
 * Author: Xi Wang <xi.wang@gmail.com>
 * Date:   Mon Mar 5 15:14:41 2012 -0800
 *
 * 	slab: introduce kmalloc_array()
 */

/* SIZE_MAX is backported in compat-3.5.h so include it */
#include <linux/compat-3.5.h>
static inline void *kmalloc_array(size_t n, size_t size, gfp_t flags)
{
	if (size != 0 && n > SIZE_MAX / size)
		return NULL;
	return __kmalloc(n * size, flags);
}

#include <linux/etherdevice.h>
#include <linux/skbuff.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
extern const struct i2c_algorithm i2c_bit_algo;
#endif

extern int simple_open(struct inode *inode, struct file *file);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
#define skb_add_rx_frag(skb, i, page, off, size, truesize) \
	v2_6_28_skb_add_rx_frag(skb, i, page, off, size)
#else
#define skb_add_rx_frag(skb, i, page, off, size, truesize) \
	skb_add_rx_frag(skb, i, page, off, size)
#endif

#ifdef CONFIG_X86_X32_ABI
#define COMPAT_USE_64BIT_TIME \
	(!!(task_pt_regs(current)->orig_ax & __X32_SYSCALL_BIT))
#else
#define COMPAT_USE_64BIT_TIME 0
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12))
static inline void eth_hw_addr_random(struct net_device *dev)
{
#error eth_hw_addr_random() needs to be implemented for < 2.6.12
}
#else  /* kernels >= 2.6.12 */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31))
static inline void eth_hw_addr_random(struct net_device *dev)
{
	get_random_bytes(dev->dev_addr, ETH_ALEN);
	dev->dev_addr[0] &= 0xfe;       /* clear multicast bit */
	dev->dev_addr[0] |= 0x02;       /* set local assignment bit (IEEE802) */
}
#else /* kernels >= 2.6.31 */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
/* So this is 2.6.31..2.6.35 */

/* Just have the flags present, they won't really mean anything though */
#define NET_ADDR_PERM          0       /* address is permanent (default) */
#define NET_ADDR_RANDOM                1       /* address is generated randomly */
#define NET_ADDR_STOLEN                2       /* address is stolen from other device */

static inline void eth_hw_addr_random(struct net_device *dev)
{
	random_ether_addr(dev->dev_addr);
}

#else /* 2.6.36 and on */
static inline void eth_hw_addr_random(struct net_device *dev)
{
	dev_hw_addr_random(dev, dev->dev_addr);
}
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)) */

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,31)) */
#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12)) */

/* source include/linux/pci.h */
/**
 * module_pci_driver() - Helper macro for registering a PCI driver
 * @__pci_driver: pci_driver struct
 *
 * Helper macro for PCI drivers which do not do anything special in module
 * init/exit. This eliminates a lot of boilerplate. Each module may only
 * use this macro once, and calling it replaces module_init() and module_exit()
 */
#define module_pci_driver(__pci_driver) \
	module_driver(__pci_driver, pci_register_driver, \
		       pci_unregister_driver)

#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION(3,4,0)) */

#endif /* LINUX_5_4_COMPAT_H */
