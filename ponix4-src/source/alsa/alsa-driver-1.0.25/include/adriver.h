#ifndef __SOUND_LOCAL_DRIVER_H
#define __SOUND_LOCAL_DRIVER_H

/*
 *  Main header file for the ALSA driver
 *  Copyright (c) 1994-2000 by Jaroslav Kysela <perex@perex.cz>
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 */

#include "config-top.h"

/* number of supported soundcards */
#ifdef CONFIG_SND_DYNAMIC_MINORS
#define SNDRV_CARDS 32
#else
#define SNDRV_CARDS 8		/* don't change - minor numbers */
#endif

#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33)
#include <generated/autoconf.h>
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 18)
#include <linux/autoconf.h>
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 2, 3)
#error "This driver requires Linux 2.2.3 or higher."
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 3, 1)
#  if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0)
#  error "This code requires Linux 2.4.0-test1 and higher."
#  endif
#define LINUX_2_4__donotuse
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 0)
#define LINUX_2_2
#endif

#if defined(ALSA_BUILD) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#if defined(CONFIG_MODVERSIONS) && !defined(__GENKSYMS__) && !defined(__DEPEND__)
#include "sndversions.h"
#endif
#endif /* ALSA_BUILD && KERNEL < 2.6.0 */

#ifndef RHEL_RELEASE_VERSION
#define RHEL_RELEASE_VERSION(a, b) (((a) << 8) | (b))
#endif

#include <linux/module.h>

#ifdef CONFIG_HAVE_OLD_REQUEST_MODULE
#include <linux/kmod.h>
#undef request_module
void snd_compat_request_module(const char *name, ...);
#define request_module(name, args...) snd_compat_request_module(name, ##args)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12)
#include <linux/compiler.h>
#ifndef __iomem
#define __iomem
#endif
#ifndef __user
#define __user
#endif
#ifndef __kernel
#define __kernel
#endif
#ifndef __nocast
#define __nocast
#endif
#ifndef __force
#define __force
#endif
#ifndef __safe
#define __safe
#endif
#include <linux/types.h>
#ifndef __bitwise
#define __bitwise
typedef __u16 __le16;
typedef __u16 __be16;
typedef __u32 __le32;
typedef __u32 __be32;
#endif
#ifndef __deprecated
# define __deprecated           /* unimplemented */
#endif
#endif /* < 2.6.9 */

/* other missing types */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
#if !defined(RHEL_RELEASE_CODE) || RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(5, 4)
typedef unsigned int fmode_t;
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
/* for compat layer */
#include <linux/pci.h>
#ifdef LINUX_2_2
#include <linux/pci_ids.h>
#endif

#ifdef LINUX_2_2
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 2, 18)
#include <linux/init.h>
#endif
#ifndef LINUX_2_4__donotuse
#include "compat_22.h"
#endif
#endif /* LINUX_2_2 */

#ifdef LINUX_2_4__donotuse
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pm.h>
#include <linux/list.h>
#include <asm/processor.h>
#include <asm/page.h>
#ifndef likely
#define likely(x)	x
#define unlikely(x)	x
#endif
#ifndef cpu_relax
#define cpu_relax()
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 3)
#define pci_set_dma_mask(pci, mask) (pci->dma_mask = mask, 0)
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 7)
#define PCI_OLD_SUSPEND
#endif
#ifndef virt_to_page
#define virt_to_page(x) (&mem_map[MAP_NR(x)])
#endif
#define snd_request_region request_region
#ifndef rwlock_init
#define rwlock_init(x) do { *(x) = RW_LOCK_UNLOCKED; } while(0)
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 9)
#define prefetch(x)	x
#endif
#ifndef list_for_each_safe
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)
#endif
#ifndef list_for_each_entry
#define list_for_each_entry(pos, head, member)                          \
	for (pos = list_entry((head)->next, typeof(*pos), member);	\
	     prefetch(pos->member.next), &pos->member != (head);	\
	     pos = list_entry(pos->member.next, typeof(*pos), member))
#endif
#ifndef list_for_each_entry_safe
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))
#endif
#ifndef list_for_each_prev
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; prefetch(pos->prev), pos != (head); \
        	pos = pos->prev)
#endif
#ifndef might_sleep
#define might_sleep() do { } while (0)
#endif
#endif /* LINUX_2_4__donotuse */

#ifndef __devexit_p
#define __devexit_p(x) x
#endif
#ifndef __init_or_module
#define __init_or_module
#define __initdata_or_module
#endif

#ifndef va_copy
#define va_copy __va_copy
#endif

#include <linux/kdev_t.h>
#ifndef major
#define major(x) MAJOR(x)
#endif
#ifndef minor
#define minor(x) MINOR(x)
#endif
#ifndef imajor
#define imajor(x) major((x)->i_rdev)
#endif
#ifndef iminor
#define iminor(x) minor((x)->i_rdev)
#endif
#ifndef mk_kdev
#define mk_kdev(maj, min) MKDEV(maj, min)
#endif
#ifndef DECLARE_BITMAP
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[((bits)+BITS_PER_LONG-1)/BITS_PER_LONG]
#endif

#include <linux/sched.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 3) && !defined(need_resched)
#define need_resched() (current->need_resched)
#endif
#ifndef CONFIG_HAVE_PDE
#include <linux/fs.h>
#include <linux/proc_fs.h>
static inline struct proc_dir_entry *PDE(const struct inode *inode)
{
	return (struct proc_dir_entry *) inode->u.generic_ip;
}
#endif
#ifndef cond_resched
#define cond_resched() \
	do { \
		if (need_resched()) { \
			set_current_state(TASK_RUNNING); \
			schedule(); \
		} \
	} while (0)
#endif
#include <asm/io.h>
#if !defined(isa_virt_to_bus)
#if defined(virt_to_bus) || defined(__alpha__)
#define isa_virt_to_bus virt_to_bus
#endif
#endif

/* isapnp support for 2.2 kernels */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0)
#undef CONFIG_ISAPNP
#ifdef CONFIG_SND_ISAPNP
#define CONFIG_ISAPNP
#endif
#endif

/* support of pnp compatible layer for 2.2/2.4 kernels */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#undef CONFIG_PNP
#ifdef CONFIG_SND_PNP
#define CONFIG_PNP
#endif
#endif

#if !defined(CONFIG_ISA) && defined(CONFIG_SND_ISA)
#define CONFIG_ISA
#endif

#ifndef MODULE_LICENSE
#define MODULE_LICENSE(license)
#endif

#endif /* < 2.6.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 13)
#ifdef CONFIG_SND_ISA
#ifndef CONFIG_ISA_DMA_API
#define CONFIG_ISA_DMA_API
#endif
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 9)
#include <linux/interrupt.h>
#ifndef in_atomic
#define in_atomic()	in_interrupt()
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14) && \
	!defined(CONFIG_HAVE_GFP_T)
typedef unsigned __nocast gfp_t;
#endif

#ifndef CONFIG_HAVE_GFP_DMA32
#define GFP_DMA32 0		/* driver must check for 32-bit address */
#endif

#include <linux/wait.h>
#ifndef wait_event_timeout
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define __wait_event_timeout(wq, condition, ret)			\
do {									\
	DEFINE_WAIT(__wait);						\
									\
	for (;;) {							\
		prepare_to_wait(&wq, &__wait, TASK_UNINTERRUPTIBLE);	\
		if (condition)						\
			break;						\
		ret = schedule_timeout(ret);				\
		if (!ret)						\
			break;						\
	}								\
	finish_wait(&wq, &__wait);					\
} while (0)
#else
#define __wait_event_timeout(wq, condition, ret)			\
do {									\
	wait_queue_t __wait;						\
	init_waitqueue_entry(&__wait, current);				\
									\
	add_wait_queue(&wq, &__wait);					\
	for (;;) {							\
		set_current_state(TASK_UNINTERRUPTIBLE);		\
		if (condition)						\
			break;						\
		ret = schedule_timeout(ret);				\
		if (!ret)						\
			break;						\
	}								\
	set_current_state(TASK_RUNNING);				\
	remove_wait_queue(&wq, &__wait);				\
} while (0)
#endif /* 2.6.0 */
#define wait_event_timeout(wq, condition, timeout)			\
({									\
	long __ret = timeout;						\
	if (!(condition))						\
		__wait_event_timeout(wq, condition, __ret);		\
	__ret;								\
})
#endif
#ifndef wait_event_interruptible_timeout
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define __wait_event_interruptible_timeout(wq, condition, ret)		\
do {									\
	DEFINE_WAIT(__wait);						\
									\
	for (;;) {							\
		prepare_to_wait(&wq, &__wait, TASK_INTERRUPTIBLE);	\
		if (condition)						\
			break;						\
		if (!signal_pending(current)) {				\
			ret = schedule_timeout(ret);			\
			if (!ret)					\
				break;					\
			continue;					\
		}							\
		ret = -ERESTARTSYS;					\
		break;							\
	}								\
	finish_wait(&wq, &__wait);					\
} while (0)
#else
#define __wait_event_interruptible_timeout(wq, condition, ret)		\
do {									\
	wait_queue_t __wait;						\
	init_waitqueue_entry(&__wait, current);				\
									\
	add_wait_queue(&wq, &__wait);					\
	for (;;) {							\
		set_current_state(TASK_INTERRUPTIBLE);			\
		if (condition)						\
			break;						\
		if (!signal_pending(current)) {				\
			ret = schedule_timeout(ret);			\
			if (!ret)					\
				break;					\
			continue;					\
		}							\
		ret = -ERESTARTSYS;					\
		break;							\
	}								\
	set_current_state(TASK_RUNNING);				\
	remove_wait_queue(&wq, &__wait);				\
} while (0)
#endif /* 2.6.0 */
#define wait_event_interruptible_timeout(wq, condition, timeout)	\
({									\
	long __ret = timeout;						\
	if (!(condition))						\
		__wait_event_interruptible_timeout(wq, condition, __ret); \
	__ret;								\
})
#endif

#ifndef CONFIG_HAVE_STRLCPY
size_t snd_compat_strlcpy(char *dest, const char *src, size_t size);
#define strlcpy(dest, src, size) snd_compat_strlcpy(dest, src, size)
size_t snd_compat_strlcat(char *dest, const char *src, size_t size);
#define strlcat(dest, src, size) snd_compat_strlcat(dest, src, size)
#endif

#ifndef CONFIG_HAVE_SNPRINTF
int snd_compat_snprintf(char * buf, size_t size, const char * fmt, ...);
#define snprintf(buf,size,fmt,args...) snd_compat_snprintf(buf,size,fmt,##args)
#endif
#ifndef CONFIG_HAVE_VSNPRINTF
#include <stdarg.h>
int snd_compat_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
#define vsnprintf(buf,size,fmt,args) snd_compat_vsnprintf(buf,size,fmt,args)
#endif

#ifndef CONFIG_HAVE_SCNPRINTF
#define scnprintf(buf,size,fmt,args...) snprintf(buf,size,fmt,##args)
#define vscnprintf(buf,size,fmt,args) vsnprintf(buf,size,fmt,args)
#endif

#ifndef CONFIG_HAVE_SSCANF
#include <stdarg.h>
int snd_compat_sscanf(const char * buf, const char * fmt, ...);
int snd_compat_vsscanf(const char * buf, const char * fmt, va_list args);
#define sscanf(buf,fmt,args...) snd_compat_sscanf(buf,fmt,##args)
#define vsscanf(buf,fmt,args) snd_compat_vsscanf(buf,fmt,args)
#endif

#if defined(__alpha__) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 3, 14)
#include <asm/io.h>
#undef writeb
#define writeb(v, a) do { __writeb((v),(a)); mb(); } while(0)
#undef writew
#define writew(v, a) do { __writew((v),(a)); mb(); } while(0)
#undef writel
#define writel(v, a) do { __writel((v),(a)); mb(); } while(0)
#undef writeq
#define writeq(v, a) do { __writeq((v),(a)); mb(); } while(0)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 28)
#include <linux/interrupt.h>
static inline void synchronize_irq_wrapper(unsigned int irq) { synchronize_irq(); }
#undef synchronize_irq
#define synchronize_irq(irq)	synchronize_irq_wrapper(irq)
#endif /* LINUX_VERSION_CODE < 2.5.28 */
#ifndef IRQ_NONE
typedef void irqreturn_t;
#define IRQ_NONE
#define IRQ_HANDLED
#define IRQ_RETVAL(x)
#endif
#endif /* < 2.6.0 */

#ifndef min
/*
 * copied from the include/linux/kernel.h file
 * for compatibility with earlier kernels.
 */
#define min(x,y) ({ \
	const typeof(x) _x = (x); \
	const typeof(y) _y = (y); \
	(void) (&_x == &_y); \
	_x < _y ? _x : _y; })
#define max(x,y) ({ \
	const typeof(x) _x = (x);	\
	const typeof(y) _y = (y);	\
	(void) (&_x == &_y);		\
	_x > _y ? _x : _y; })
#endif

#ifndef min_t
#define min_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x < __y ? __x: __y; })
#define max_t(type,x,y) \
	({ type __x = (x); type __y = (y); __x > __y ? __x: __y; })
#endif

#ifndef container_of
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#endif

#ifndef ALIGN
#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))
#endif

#ifndef roundup
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif

#ifndef BUG_ON
#define BUG_ON(x) /* nothing */
#endif

#ifndef BUILD_BUG_ON
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#include <linux/devfs_fs_kernel.h>
#ifdef CONFIG_DEVFS_FS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 29)
#include <linux/fs.h>
#undef register_chrdev
#define register_chrdev devfs_register_chrdev
#undef unregister_chrdev
#define unregister_chrdev devfs_unregister_chrdev
#undef devfs_remove
void snd_compat_devfs_remove(const char *fmt, ...);
#define devfs_remove snd_compat_devfs_remove
#endif /* < 2.5.29 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 67)
#undef devfs_mk_dir
int snd_compat_devfs_mk_dir(const char *dir, ...);
#define devfs_mk_dir snd_compat_devfs_mk_dir
#undef devfs_mk_cdev
int snd_compat_devfs_mk_cdev(dev_t dev, umode_t mode, const char *fmt, ...);
#define devfs_mk_cdev snd_compat_devfs_mk_cdev
#endif /* < 2.5.67 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 3, 0)
static inline void devfs_find_and_unregister (devfs_handle_t dir, const char *name,
					      unsigned int major, unsigned int minor,
                                              char type, int traverse_symlinks)
{
	devfs_handle_t master;
	master = devfs_find_handle(dir, name, strlen(name), major, minor, type, traverse_symlinks);
	devfs_unregister(master);
}
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
static inline void devfs_find_and_unregister (devfs_handle_t dir, const char *name,
					      unsigned int major, unsigned int minor,
                                              char type, int traverse_symlinks)
{
	devfs_handle_t master;
	master = devfs_find_handle(dir, name, major, minor, type, traverse_symlinks);
	devfs_unregister(master);
}
#endif
#else /* !CONFIG_DEVFS_FS */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 29)
static inline void devfs_remove(const char *fmt, ...) { }
#endif
#undef devfs_mk_dir
#define devfs_mk_dir(dir, args...) do { (void)(dir); } while (0)
#undef devfs_mk_cdev
#define devfs_mk_cdev(dev, mode, fmt, args...) do { (void)(dev); } while (0)
#endif /* CONFIG_DEVFS_FS */
#endif /* < 2.6.0 */

#if defined(SND_NEED_USB_WRAPPER) && (defined(CONFIG_USB) || defined(CONFIG_USB_MODULE))
#include "usb_wrapper.h"
#endif

/* workqueue-alike; 2.5.45 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#include <linux/workqueue.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 45) && !defined(__WORK_INITIALIZER)
#define SND_WORKQUEUE_COMPAT
struct workqueue_struct;
struct delayed_work;
struct work_struct {
	unsigned long pending;
	struct list_head entry;
	void (*func)(void *);
	void *data;
	void *wq_data;
	struct timer_list timer;
};
#define INIT_WORK(_work, _func, _data)			\
	do {						\
		(_work)->func = _func;			\
		(_work)->data = _data;			\
		init_timer(&(_work)->timer);		\
	} while (0)
#define __WORK_INITIALIZER(n, f, d) {			\
		.func = (f),				\
		.data = (d),				\
	}
#define DECLARE_WORK(n, f, d)				\
	struct work_struct n = __WORK_INITIALIZER(n, f, d)
int snd_compat_schedule_work(struct work_struct *work);
#define schedule_work(w) snd_compat_schedule_work(w)
struct workqueue_struct *snd_compat_create_workqueue(const char *name);
#define create_workqueue(name) snd_compat_create_workqueue((name))
void snd_compat_flush_workqueue(struct workqueue_struct *wq);
#define flush_workqueue(wq) snd_compat_flush_workqueue((wq));
void snd_compat_destroy_workqueue(struct workqueue_struct *wq);
#define destroy_workqueue(wq) snd_compat_destroy_workqueue((wq));
int snd_compat_queue_work(struct workqueue_struct *wq, struct work_struct *work);
#define queue_work(wq, work) snd_compat_queue_work((wq), (work))
int snd_compat_queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *work, unsigned long delay);
#define queue_delayed_work(wq, work, delay) snd_compat_queue_delayed_work((wq), (work), (delay))
#define schedule_delayed_work(work, delay) snd_compat_queue_delayed_work(NULL, (work), (delay))
int snd_compat_cancel_delayed_work(struct delayed_work *work);
#define cancel_delayed_work(work) snd_compat_cancel_delayed_work(work)
#define work_pending(work) test_bit(0, &(work)->pending)
#define delayed_work_pending(w) work_pending(&(w)->work)
#define flush_scheduled_work()
#endif /* < 2.5.45 */
#endif /* < 2.6.0 */

#ifdef CONFIG_CREATE_WORKQUEUE_FLAGS
#include <linux/workqueue.h>
#undef create_workqueue
struct workqueue_struct *snd_compat_create_workqueue2(const char *name);
#define create_workqueue(name) snd_compat_create_workqueue2(name)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 6)
#include <linux/workqueue.h>
#ifndef create_singlethread_workqueue
#define create_singlethread_workqueue(name) create_workqueue(name)
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
#include <linux/workqueue.h>
/* redefine INIT_WORK() */
static inline void snd_INIT_WORK(struct work_struct *w, void (*f)(struct work_struct *))
{
	INIT_WORK(w, (void(*)(void*))(f), w);
}
#undef INIT_WORK
#define INIT_WORK(w,f) snd_INIT_WORK(w,f)
#ifndef SND_WORKQUEUE_COMPAT
#define delayed_work snd_delayed_work
#endif
/* delayed_work wrapper */
struct delayed_work {
	struct work_struct work;
};
#undef INIT_DELAYED_WORK
#define INIT_DELAYED_WORK(_work, _func)	INIT_WORK(&(_work)->work, _func)
#ifndef SND_WORKQUEUE_COMPAT
/* redefine *_delayed_work() */
#define queue_delayed_work(wq,_work,delay) \
	queue_delayed_work(wq, &(_work)->work, delay)
#define schedule_delayed_work(_work,delay) \
	schedule_delayed_work(&(_work)->work, delay)
#define cancel_delayed_work(_work) \
	cancel_delayed_work(&(_work)->work)
#define work_pending(work) \
	test_bit(0, &(work)->pending)
#define delayed_work_pending(w) \
	work_pending(&(w)->work)
#endif /* !SND_WORKQUEUE_COMPAT */
#endif /* < 2.6.20 */

/* 2.5 new modules */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#define try_module_get(x) try_inc_mod_count(x)
static inline void module_put(struct module *module)
{
	if (module)
		__MOD_DEC_USE_COUNT(module);
}
#endif /* 2.5.0 */

/* gameport - 2.4 has different defines */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#ifdef CONFIG_INPUT_GAMEPORT
#define CONFIG_GAMEPORT
#endif
#ifdef CONFIG_INPUT_GAMEPORT_MODULE
#define CONFIG_GAMEPORT_MODULE
#endif
#endif /* 2.5.0 */

/* vmalloc_to_page wrapper */
#ifndef CONFIG_HAVE_VMALLOC_TO_PAGE
struct page *snd_compat_vmalloc_to_page(void *addr);
#define vmalloc_to_page(addr) snd_compat_vmalloc_to_page(addr)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 69)
#include <linux/vmalloc.h>
static inline void *snd_compat_vmap(struct page **pages, unsigned int count, unsigned long flags, pgprot_t prot)
{
	return vmap(pages, count);
}
#undef vmap
#define vmap snd_compat_vmap
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 0) /* correct version? */
#define EXPORT_NO_SYMBOLS
#endif

/* MODULE_ALIAS & co. */
#ifndef MODULE_ALIAS
#define MODULE_ALIAS(x)
#define MODULE_ALIAS_CHARDEV_MAJOR(x)
#endif

#ifndef MODULE_FIRMWARE
#define MODULE_FIRMWARE(x)
#endif

#ifndef CONFIG_HAVE_PCI_CONSISTENT_DMA_MASK
#define pci_set_consistent_dma_mask(p,x) 0 /* success */
#endif

/* sysfs */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 2)
struct class;
struct class_device;
struct class_device_attribute { }; /* dummy */
#define __ATTR(cls,perm,read,write) { } /* dummy */
static inline struct class_device* class_device_create(struct class *class, struct class_device *parent, int devnum, ...) { return NULL; }
static inline void class_device_destroy(struct class *class, int devnum) { return; }
static inline void *class_get_devdata(struct class_device *dev) { return NULL; }
#else /* >= 2.6.2 */
#ifndef CONFIG_SND_NESTED_CLASS_DEVICE
#include <linux/device.h>
#define class_device_create(cls,prt,devnum,args...) class_device_create(cls,devnum,##args)
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
#include <linux/sysfs.h>
#ifndef __ATTR
#define __ATTR(_name,_mode,_show,_store) { \
	.attr = {.name = __stringify(_name), .mode = _mode, .owner = THIS_MODULE },	\
	.show	= _show,					\
	.store	= _store,					\
}
#endif /* __ATTR */
#endif /* < 2.6.8 */
#endif

/* msleep */
#ifndef CONFIG_HAVE_MSLEEP
void snd_compat_msleep(unsigned int msecs);
#define msleep snd_compat_msleep
#endif

#ifndef CONFIG_HAVE_MSLEEP_INTERRUPTIBLE
#include <linux/delay.h>
unsigned long snd_compat_msleep_interruptible(unsigned int msecs);
#define msleep_interruptible snd_compat_msleep_interruptible
#ifndef ssleep
#define ssleep(x) msleep((unsigned int)(x) * 1000)
#endif
#endif

/* msecs_to_jiffies */
#ifndef CONFIG_HAVE_MSECS_TO_JIFFIES
#include <linux/jiffies.h>
#if defined(DESKTOP_HZ) && LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define HAVE_VARIABLE_HZ	/* 2.4 SUSE kernel, HZ is a variable */
#endif
static inline unsigned int jiffies_to_msecs(const unsigned long j)
{
#ifndef HAVE_VARIABLE_HZ
	#if HZ <= 1000 && !(1000 % HZ)
		return (1000 / HZ) * j;
	#elif HZ > 1000 && !(HZ % 1000)
		return (j + (HZ / 1000) - 1)/(HZ / 1000);
	#else
		return (j * 1000) / HZ;
	#endif
#else
	if (HZ <= 1000 && !(1000 % HZ))
		return (1000 / HZ) * j;
	else if (HZ > 1000 && !(HZ % 1000))
		return (j + (HZ / 1000) - 1)/(HZ / 1000);
	else
		return (j * 1000) / HZ;
#endif
}
static inline unsigned long msecs_to_jiffies(const unsigned int m)
{
	if (m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
#ifndef HAVE_VARIABLE_HZ
	#if HZ <= 1000 && !(1000 % HZ)
		return (m + (1000 / HZ) - 1) / (1000 / HZ);
	#elif HZ > 1000 && !(HZ % 1000)
		return m * (HZ / 1000);
	#else
		return (m * HZ + 999) / 1000;
	#endif
#else
	if (HZ <= 1000 && !(1000 % HZ))
		return (m + (1000 / HZ) - 1) / (1000 / HZ);
	else if (HZ > 1000 && !(HZ % 1000))
		return m * (HZ / 1000);
	else
		return (m * HZ + 999) / 1000;
#endif
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#define snd_dma_pci_data(pci)	((struct device *)(pci))
#define snd_dma_isa_data()	NULL
#define snd_dma_sbus_data(sbus)	((struct device *)(sbus))
#define snd_dma_continuous_data(x)	((struct device *)(unsigned long)(x))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 6)
#include <linux/mm.h>
#include <asm/io.h>
#include <asm/scatterlist.h>
#endif
#include <linux/dma-mapping.h>

#ifndef DMA_32BIT_MASK
#define DMA_32BIT_MASK 0xffffffff
#endif
#ifndef DMA_31BIT_MASK
#define DMA_31BIT_MASK	0x000000007fffffffULL
#endif
#ifndef DMA_30BIT_MASK
#define DMA_30BIT_MASK	0x000000003fffffffULL
#endif
#ifndef DMA_28BIT_MASK
#define DMA_28BIT_MASK	0x000000000fffffffULL
#endif
#ifndef DMA_24BIT_MASK
#define DMA_24BIT_MASK	0x0000000000ffffffULL
#endif
#ifndef DMA_BIT_MASK
#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))
#endif
#ifndef __GFP_ZERO
#define __GFP_ZERO	0x4000
#define CONFIG_HAVE_OWN_GFP_ZERO 1
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0)
#include <linux/moduleparam.h>
#ifndef param_get_bint
#define bint bool
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#undef module_param
#define SNDRV_MODULE_TYPE_int	"i"
#define SNDRV_MODULE_TYPE_bool	"i"
#define SNDRV_MODULE_TYPE_uint	"i"
#define SNDRV_MODULE_TYPE_charp	"s"
#define SNDRV_MODULE_TYPE_long	"l"
#define module_param_array(name, type, nump, perm) \
	MODULE_PARM(name, "1-" __MODULE_STRING(SNDRV_CARDS) SNDRV_MODULE_TYPE_##type)
#define module_param(name, type, perm) \
	MODULE_PARM(name, SNDRV_MODULE_TYPE_##type)
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 10)
#include <linux/moduleparam.h>
#undef module_param_array
/* we assumme nump is always NULL so we can use a dummy variable */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 5)
#define module_param_array(name, type, nump, perm) \
	static int boot_devs_##name; \
	static struct kparam_array __param_arr_##name			\
	= { ARRAY_SIZE(name), &boot_devs_##name, param_set_##type, param_get_##type,	\
	    sizeof(name[0]), name };					\
	module_param_call(name, param_array_set, param_array_get, 	\
			  &__param_arr_##name, perm)
#else
#define module_param_array(name, type, nump, perm) \
	static int boot_devs_##name; \
	module_param_array_named(name, name, type, boot_devs_##name, perm)
#endif /* < 2.6.5 */
#endif /* < 2.6.10 */
#endif /* < 3.2.0 */

/* dump_stack hack */
#ifndef CONFIG_HAVE_DUMP_STACK
#undef dump_stack
#define dump_stack()
#endif

#ifdef CONFIG_PCI
#ifndef CONFIG_HAVE_PCI_DEV_PRESENT
#include <linux/pci.h>
#ifndef PCI_DEVICE
#define PCI_DEVICE(vend,dev) \
	.vendor = (vend), .device = (dev), \
	.subvendor = PCI_ANY_ID, .subdevice = PCI_ANY_ID
#endif
int snd_pci_dev_present(const struct pci_device_id *ids);
#define pci_dev_present(x) snd_pci_dev_present(x)
#endif
#endif

/*
 * memory allocator wrappers
 */
#if defined(CONFIG_SND_DEBUG_MEMORY) && !defined(SKIP_HIDDEN_MALLOCS)

#include <linux/slab.h>
void *snd_hidden_kmalloc(size_t size, gfp_t flags);
void *snd_hidden_kzalloc(size_t size, gfp_t flags);
void *snd_hidden_kcalloc(size_t n, size_t size, gfp_t flags);
char *snd_hidden_kstrdup(const char *s, gfp_t flags);
char *snd_hidden_kstrndup(const char *s, size_t len, gfp_t flags);
void snd_hidden_kfree(const void *obj);
size_t snd_hidden_ksize(const void *obj);

static inline void *snd_wrapper_kmalloc(size_t size, gfp_t flags)
{
	return kmalloc(size, flags);
}
static inline void snd_wrapper_kfree(const void *obj)
{
	kfree(obj);
}

#define kmalloc(size, flags) snd_hidden_kmalloc(size, flags)
#define kzalloc(size, flags) snd_hidden_kzalloc(size, flags)
#define kcalloc(n, size, flags) snd_hidden_kcalloc(n, size, flags)
#define kstrdup(s, flags)  snd_hidden_kstrdup(s, flags)
#define kstrndup(s, len, flags)  snd_hidden_kstrndup(s, len, flags)
#define ksize(obj) snd_hidden_ksize(obj)
#define kfree(obj) snd_hidden_kfree(obj)

#define kmalloc_nocheck(size, flags) snd_wrapper_kmalloc(size, flags)
#define kfree_nocheck(obj) snd_wrapper_kfree(obj)

#else /* ! CONFIG_SND_DEBUG_MEMORY */

#define kmalloc_nocheck(size, flags) kmalloc(size, flags)
#define kfree_nocheck(obj) kfree(obj)

#ifndef CONFIG_HAVE_KCALLOC
void *snd_compat_kcalloc(size_t n, size_t size, gfp_t gfp_flags);
#define kcalloc(n,s,f) snd_compat_kcalloc(n,s,f)
#endif

#ifndef CONFIG_HAVE_KSTRDUP
char *snd_compat_kstrdup(const char *s, gfp_t gfp_flags);
#define kstrdup(s,f) snd_compat_kstrdup(s,f)
#endif

#ifndef CONFIG_HAVE_KSTRNDUP
char *snd_compat_kstrndup(const char *s, size_t len, gfp_t gfp_flags);
#define kstrndup(s,l,f) snd_compat_kstrndup(s,l,f)
#endif

#ifndef CONFIG_HAVE_KZALLOC
void *snd_compat_kzalloc(size_t n, gfp_t gfp_flags);
#define kzalloc(s,f) snd_compat_kzalloc(s,f)
#endif

#endif /* CONFIG_SND_DEBUG_MEMORY */

/* DEFINE_SPIN/RWLOCK (up to 2.6.11-rc2) */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 11)
#include <linux/spinlock.h>
#ifndef DEFINE_SPINLOCK
#define DEFINE_SPINLOCK(x) spinlock_t x = SPIN_LOCK_UNLOCKED
#define DEFINE_RWLOCK(x) rwlock_t x = RW_LOCK_UNLOCKED
#endif
#endif

/* pm_message_t type */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 11)
#include <linux/pm.h>
#ifndef PMSG_FREEZE
typedef u32 __bitwise pm_message_t;
#define PMSG_FREEZE	3
#define PMSG_SUSPEND	3
#define PMSG_ON		0
#endif
#endif

/* PMSG_IS_AUTO macro */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0)
#include <linux/pm.h>
#ifndef PMSG_IS_AUTO
#define PMSG_IS_AUTO(msg)	(((msg).event & PM_EVENT_AUTO) != 0)
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 11)
#ifdef CONFIG_PCI
#include <linux/pci.h>
#ifndef PCI_D0
#define PCI_D0     0
#define PCI_D1     1
#define PCI_D2     2
#define PCI_D3hot  3
#define PCI_D3cold 4
#define pci_choose_state(pci,state)	((state) ? PCI_D3hot : PCI_D0)
#endif
#endif
#endif

/* __GFP_XXX */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 6)
/* #incldue <linux/gfp.h> */
#ifndef __GFP_COMP
#define __GFP_COMP	0
#endif
#ifndef __GFP_NOWARN
#define __GFP_NOWARN	0
#endif
#ifndef __GFP_NORETRY
#define __GFP_NORETRY	0
#endif
#endif

/* vprintk */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 9)
#include <linux/kernel.h>
static inline void snd_compat_vprintk(const char *fmt, va_list args)
{
	char tmpbuf[512];
	vsnprintf(tmpbuf, sizeof(tmpbuf), fmt, args);
	printk(tmpbuf);
}
#define vprintk snd_compat_vprintk
#endif

/* printk_ratelimit() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#include <linux/kernel.h>
#define printk_ratelimit()	1
#endif

#if defined(CONFIG_GAMEPORT) || defined(CONFIG_GAMEPORT_MODULE)
#define wait_ms gameport_wait_ms
#include <linux/gameport.h>
#undef wait_ms
#ifndef to_gameport_driver
#include <linux/slab.h>
/* old gameport interface */
struct snd_gameport {
	struct gameport gp;
	void *port_data;
};
static inline struct gameport *gameport_allocate_port(void)
{
	struct snd_gameport *gp;
	gp = kzalloc(sizeof(*gp), GFP_KERNEL);
	if (gp)
		return &gp->gp;
	return NULL;
}
#define gameport_free_port(gp)	kfree(gp)
static inline void snd_gameport_unregister_port(struct gameport *gp)
{
	gameport_unregister_port(gp);
	kfree(gp);
}
#undef gameport_unregister_port
#define gameport_unregister_port(gp)	snd_gameport_unregister_port(gp)
#define gameport_set_port_data(gp,r) (((struct snd_gameport *)(gp))->port_data = (r))
#define gameport_get_port_data(gp) ((struct snd_gameport *)(gp))->port_data
#define gameport_set_dev_parent(gp,xdev)
#define gameport_set_name(gp,x)
#define gameport_set_phys(gp,x,y)
#endif /* to_gameport_driver */
#endif /* GAMEPORT || GAMEPORT_MODULE */

/* use pci_module_init on 2.4 kernels */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 4, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#ifdef CONFIG_PCI
#include <linux/pci.h>

/* power management compatibility layer */
#if defined(CONFIG_PM) && defined(PCI_OLD_SUSPEND)
struct snd_compat_pci_driver {
	struct pci_driver real_driver;
	char *name;
	const struct pci_device_id *id_table;
	int (*probe)(struct pci_dev *dev, const struct pci_device_id *id);
	void (*remove)(struct pci_dev *dev);
	int (*suspend)(struct pci_dev *dev, u32 state);
	int (*resume)(struct pci_dev *dev);
};

static inline void snd_pci_old_suspend(sturct pci_dev *pci)
{
	struct snd_compat_pci_driver *driver = (struct snd_compat_pci_driver *)pci->driver;
	if (driver->suspend)
		driver->suspend(pci, PMSG_SUSPEND);
}
static inline void snd_pci_old_resume(struct pci_dev *pci)
{
	struct snd_compat_pci_driver *driver = (struct snd_compat_pci_driver *)pci->driver;
	if (driver->resume)
		driver->resume(pci);
}

static inline int snd_pci_compat_register_driver(struct snd_compat_pci_driver *driver)
{
	driver->real_driver.name = driver->name;
	driver->real_driver.id_table = driver->id_table;
	driver->real_driver.probe = driver->probe;
	driver->real_driver.remove = driver->remove;
	if (driver->suspend || driver->resume) {
		driver->real_driver.suspend = snd_pci_old_suspend;
		driver->real_driver.resume = snd_pci_old_resume;
	} else {
		driver->real_driver.suspend = driver->suspend;
		driver->real_driver.resume = driver->resume;
	}
	return pci_module_init(&driver->real_driver);
}

static inline void snd_pci_compat_unregister_driver(struct snd_compat_pci_driver *driver)
{
	pci_unregister_driver(&driver->real_driver);
}

#define pci_driver snd_compat_pci_driver
#undef pci_register_driver
#define pci_register_driver snd_pci_compat_register_driver
#undef pci_unregister_driver
#define pci_unregister_driver snd_pci_compat_unregister_driver

#else

static inline int snd_pci_compat_register_driver(struct pci_driver *driver)
{
	return pci_module_init(driver);
}

#undef pci_register_driver
#define pci_register_driver snd_pci_compat_register_driver

#endif /* CONFIG_PM && PCI_OLD_SUSPEND */
#endif /* CONFIG_PCI */
#endif /* 2.4 */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 4, 0)
#ifdef CONFIG_PCI
#ifndef CONFIG_HAVE_NEW_PCI_SAVE_STATE
int snd_pci_compat_save_state(struct pci_dev *pci);
int snd_pci_compat_restore_state(struct pci_dev *pci);
static inline int snd_pci_orig_save_state(struct pci_dev *pci, u32 *buffer)
{
	return pci_save_state(pci, buffer);
}
static inline int snd_pci_orig_restore_state(struct pci_dev *pci, u32 *buffer)
{
	return pci_restore_state(pci, buffer);
}

#undef pci_save_state
#define pci_save_state		snd_pci_compat_save_state
#undef pci_restore_state
#define pci_restore_state	snd_pci_compat_restore_state
#endif /* !CONFIG_HAVE_NEW_PCI_SAVE_STATE */
#endif /* CONFIG_PCI */
#endif /* >= 2.4.0 */

/* pci_get_device() and pci_dev_put() wrappers */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#ifdef CONFIG_PCI
#define pci_get_device	pci_find_device
#define pci_dev_put(x)
#endif
#endif

/* wrapper for getnstimeofday()
 * it's needed for recent 2.6 kernels, too, due to lack of EXPORT_SYMBOL
 */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 14) && !defined(CONFIG_TIME_INTERPOLATION)
#define getnstimeofday(x) do { \
	struct timeval __x; \
	do_gettimeofday(&__x); \
	(x)->tv_sec = __x.tv_sec;	\
	(x)->tv_nsec = __x.tv_usec * 1000; \
} while (0)
#endif

/* schedule_timeout_[un]interruptible() wrappers */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
#include <linux/sched.h>
#define schedule_timeout_interruptible(x) ({set_current_state(TASK_INTERRUPTIBLE); schedule_timeout(x);})
#define schedule_timeout_uninterruptible(x) ({set_current_state(TASK_UNINTERRUPTIBLE); schedule_timeout(x);})
#endif

#if defined(CONFIG_PNP) && defined(CONFIG_PM)
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#ifndef CONFIG_HAVE_PNP_SUSPEND
#include <linux/pnp.h>
#define HAVE_PNP_PM_HACK
struct snd_pnp_driver {
	struct pnp_driver real_driver;
	char *name;
	const struct pnp_device_id *id_table;
	unsigned int flags;
	int  (*probe)  (struct pnp_dev *dev, const struct pnp_device_id *dev_id);
	void (*remove) (struct pnp_dev *dev);
	int (*suspend) (struct pnp_dev *dev, pm_message_t state);
	int (*resume) (struct pnp_dev *dev);
};	

int snd_pnp_register_driver(struct snd_pnp_driver *driver);
static inline void snd_pnp_unregister_driver(struct snd_pnp_driver *driver)
{
	pnp_unregister_driver(&driver->real_driver);
}

#define pnp_driver	snd_pnp_driver
#undef pnp_register_driver
#define pnp_register_driver	snd_pnp_register_driver
#undef pnp_unregister_driver
#define pnp_unregister_driver	snd_pnp_unregister_driver

struct snd_pnp_card_driver {
	struct pnp_card_driver real_driver;
	char * name;
	const struct pnp_card_device_id *id_table;
	unsigned int flags;
	int  (*probe)  (struct pnp_card_link *card, const struct pnp_card_device_id *card_id);
	void (*remove) (struct pnp_card_link *card);
	int (*suspend) (struct pnp_card_link *dev, pm_message_t state);
	int (*resume) (struct pnp_card_link *dev);
};

int snd_pnp_register_card_driver(struct snd_pnp_card_driver *driver);
static inline void snd_pnp_unregister_card_driver(struct snd_pnp_card_driver *driver)
{
	pnp_unregister_card_driver(&driver->real_driver);
}

#define pnp_card_driver		snd_pnp_card_driver
#undef pnp_register_card_driver
#define pnp_register_card_driver	snd_pnp_register_card_driver
#undef pnp_unregister_card_driver
#define pnp_unregister_card_driver	snd_pnp_unregister_card_driver

#endif /* ! CONFIG_HAVE_PNP_SUSPEND */
#endif /* 2.6 */
#endif /* CONFIG_PNP && CONFIG_PM */

/*
 * Another wrappers for pnp_register_*driver()
 * They shouldn't return positive values in the new API
 */
#ifdef CONFIG_PNP
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) && \
	LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 17)
#ifndef HAVE_PNP_PM_HACK
#include <linux/pnp.h>
static inline int snd_pnp_register_driver(struct pnp_driver *driver)
{
	int err = pnp_register_driver(driver);
	return err < 0 ? err : 0;
}
#define pnp_register_driver	snd_pnp_register_driver

static inline int snd_pnp_register_card_driver(struct pnp_card_driver *drv)
{
	int err = pnp_register_card_driver(drv);
	return err < 0 ? err : 0;
}
#define pnp_register_card_driver	snd_pnp_register_card_driver

#endif /* HAVE_PNP_PM_HACK */
#endif /* < 2.6.17 */
#endif /* PNP */

/*
 * old defines
 */
#define OPL3_HW_OPL3_PC98	0x0305	/* PC9800 */

/*
 * IRQF_* flags
 */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 17)
#include <linux/interrupt.h>
#ifndef IRQF_SHARED
#include <linux/signal.h>
#define IRQF_SHARED			SA_SHIRQ
#define IRQF_DISABLED			SA_INTERRUPT
#define IRQF_SAMPLE_RANDOM		SA_SAMPLE_RANDOM
#define IRQF_PERCPU			SA_PERCPU
#ifdef SA_PROBEIRQ
#define IRQF_PROBE_SHARED		SA_PROBEIRQ
#else
#define IRQF_PROBE_SHARED		0 /* dummy */
#endif
#endif /* IRQ_SHARED */
#endif /* <= 2.6.17 */

/*
 * lockdep macros
 */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 17)
#define lockdep_set_class(lock, key)		do { (void)(key); } while (0)
#define down_read_nested(sem, subclass)		down_read(sem)
#define down_write_nested(sem, subclass)	down_write(sem)
#define down_read_non_owner(sem)		down_read(sem)
#define up_read_non_owner(sem)			up_read(sem)
#define spin_lock_nested(lock, x)		spin_lock(lock)
#define spin_lock_irqsave_nested(lock, f, x)	spin_lock_irqsave(lock, f)
#endif

/*
 * PPC-specfic
 */
#ifdef CONFIG_PPC
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,18)
#include <linux/interrupt.h>
#ifndef NO_IRQ
#define NO_IRQ	(-1)
#endif
#define irq_of_parse_and_map(node, x) \
	(((node) && (node)->n_intrs > (x)) ? (node)->intrs[x].line : NO_IRQ)
#endif /* < 2.6.18 */
#endif /* PPC */

/* MSI */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
static inline int snd_pci_enable_msi(struct pci_dev *dev) { return -1; }
#undef pci_enable_msi
#define pci_enable_msi(dev) snd_pci_enable_msi(dev)
#undef pci_disable_msi
#define pci_disable_msi(dev)
#endif

/* SEEK_XXX */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#include <linux/fs.h>
#define SEEK_SET	0
#define SEEK_CUR	1
#define SEEK_END	2
#endif

/* kmemdup() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19) || defined(CONFIG_SND_DEBUG_MEMORY)
#include <linux/string.h>
#include <linux/slab.h>
static inline void *snd_kmemdup(const void *src, size_t len, gfp_t gfp)
{
	void *dst = kmalloc(len, gfp);
	if (!dst)
		return NULL;
	memcpy(dst, src, len);
	return dst;
}
#define kmemdup	snd_kmemdup
#endif

/* wrapper for new irq handler type */
#ifndef CONFIG_SND_NEW_IRQ_HANDLER
#include <linux/interrupt.h>
typedef irqreturn_t (*snd_irq_handler_t)(int, void *);
#undef irq_handler_t
#define irq_handler_t snd_irq_handler_t
int snd_request_irq(unsigned int, irq_handler_t handler,
		    unsigned long, const char *, void *);
void snd_free_irq(unsigned int, void *);
#undef request_irq
#define request_irq	snd_request_irq
#undef free_irq
#define free_irq	snd_free_irq
extern struct pt_regs *snd_irq_regs;
#define get_irq_regs()	snd_irq_regs
#endif /* !CONFIG_SND_NEW_IRQ_HANDLER */

/* pci_intx() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
#ifdef CONFIG_PCI
#undef pci_intx
#define pci_intx(pci,x)
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
#define CONFIG_SYSFS_DEPRECATED	1
#endif

#ifndef CONFIG_HAVE_IS_POWER_OF_2
static inline __attribute__((const))
int is_power_of_2(unsigned long n)
{
	return n != 0 && ((n & (n - 1)) == 0);
}
#endif

#ifdef CONFIG_PCI
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 23)
#define snd_pci_revision(pci)	((pci)->revision)
#else
#include <linux/pci.h>
static inline unsigned char snd_pci_revision(struct pci_dev *pci)
{
	unsigned char rev;
	pci_read_config_byte(pci, PCI_REVISION_ID, &rev);
	return rev;
}
#endif
#endif /* PCI */

/* BIT* macros */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
#include <linux/bitops.h>
/*
#ifndef BIT
#define BIT(nr)			(1UL << (nr))
#endif
*/
#ifndef BIT_MASK
#define BIT_MASK(nr)		(1UL << ((nr) % BITS_PER_LONG))
#endif
#ifndef BIT_WORD
#define BIT_WORD(nr)		((nr) / BITS_PER_LONG)
#endif
#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE		8
#endif
#endif

#ifndef CONFIG_HAVE_FFS
#if defined(__i386__)
static inline unsigned long __ffs(unsigned long word)
{
	__asm__("bsfl %1,%0"
		:"=r" (word)
		:"rm" (word));
	return word;
}
#else
static inline unsigned long __ffs(unsigned long word)
{
	__asm__("need_asm_for_your_arch_in_adriver.h");
	return word;
}
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
#ifndef uninitialized_var
#define uninitialized_var(x) x = x
#endif
#endif /* <2.6.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
typedef unsigned long uintptr_t;
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 16)
#include <linux/time.h>
#define do_posix_clock_monotonic_gettime getnstimeofday
#endif

/* undefine SNDRV_CARDS again - it'll be re-defined in sound/core.h */
#undef SNDRV_CARDS

/* put_unaligned_*() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26)
#include <asm/unaligned.h>
static inline void put_unaligned_le16(u16 val, void *p)
{
	val = cpu_to_le16(val);
	put_unaligned(val, (u16 *)p);
}

static inline void put_unaligned_le32(u32 val, void *p)
{
	val = cpu_to_le32(val);
	put_unaligned(val, (u32 *)p);
}

static inline void put_unaligned_le64(u64 val, void *p)
{
	val = cpu_to_le64(val);
	put_unaligned(val, (u64 *)p);
}

static inline void put_unaligned_be16(u16 val, void *p)
{
	val = cpu_to_be16(val);
	put_unaligned(val, (u16 *)p);
}

static inline void put_unaligned_be32(u32 val, void *p)
{
	val = cpu_to_be32(val);
	put_unaligned(val, (u32 *)p);
}

static inline void put_unaligned_be64(u64 val, void *p)
{
	val = cpu_to_be64(val);
	put_unaligned(val, (u64 *)p);
}

static inline u16 get_unaligned_le16(void *p)
{
	u16 val = get_unaligned((u16 *)p);
	return cpu_to_le16(val);
}

static inline u32 get_unaligned_le32(void *p)
{
	u32 val = get_unaligned((u32 *)p);
	return cpu_to_le32(val);
}

static inline u64 get_unaligned_le64(void *p)
{
	u64 val = get_unaligned((u64 *)p);
	return cpu_to_le64(val);
}
static inline u16 get_unaligned_be16(void *p)
{
	u16 val = get_unaligned((u16 *)p);
	return cpu_to_be16(val);
}

static inline u32 get_unaligned_be32(void *p)
{
	u32 val = get_unaligned((u32 *)p);
	return cpu_to_be32(val);
}

static inline u64 get_unaligned_be64(void *p)
{
	u64 val = get_unaligned((u64 *)p);
	return cpu_to_be64(val);
}
#endif

/* list_first_entry */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
#include <linux/list.h>
#ifndef list_first_entry
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)
#endif
#endif /* < 2.6.22 */

#ifndef upper_32_bits
#define upper_32_bits(n) ((u32)(((n) >> 16) >> 16))
#endif
#ifndef lower_32_bits
#define lower_32_bits(n) ((u32)(n))
#endif

#ifndef CONFIG_HAVE_PAGE_TO_PFN
#define page_to_pfn(page)       (page_to_phys(page) >> PAGE_SHIFT)
#endif

/* strto*() wrappers */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
/* lazy wrapper - always returns 0 */
#define XXX_DEFINE_STRTO(name, type) \
static inline int __strict_strto##name(const char *cp, unsigned int base, \
				       type *valp) \
{ \
	*valp = simple_strto##name(cp, NULL, base); \
	return 0; \
}
XXX_DEFINE_STRTO(l, long);
XXX_DEFINE_STRTO(ul, unsigned long);
XXX_DEFINE_STRTO(ll, long long);
XXX_DEFINE_STRTO(ull, unsigned long long);
#undef XXX_DEFINE_STRTO
#define strict_strtol	__strict_strtol
#define strict_strtoll	__strict_strtoll
#define strict_strtoul	__strict_strtoul
#define strict_strtoull	__strict_strtoull
#endif /* < 2.6.25 */

/* pr_xxx() macros */
#ifndef pr_emerg
#define pr_emerg(fmt, arg...) \
	printk(KERN_EMERG fmt, ##arg)
#endif
#ifndef pr_alert
#define pr_alert(fmt, arg...) \
	printk(KERN_ALERT fmt, ##arg)
#endif
#ifndef pr_crit
#define pr_crit(fmt, arg...) \
	printk(KERN_CRIT fmt, ##arg)
#endif
#ifndef pr_err
#define pr_err(fmt, arg...) \
	printk(KERN_ERR fmt, ##arg)
#endif
#ifndef pr_warning
#define pr_warning(fmt, arg...) \
	printk(KERN_WARNING fmt, ##arg)
#endif
#ifndef pr_warn
#define pr_warn(fmt, arg...) \
	printk(KERN_WARNING fmt, ##arg)
#endif
#ifndef pr_notice
#define pr_notice(fmt, arg...) \
	printk(KERN_NOTICE fmt, ##arg)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 26) && \
    LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
static inline const char *dev_name(struct device *dev)
{
	return dev->bus_id;
}

/* FIXME: return value is invalid */
#define dev_set_name(dev, fmt, args...) \
	snprintf((dev)->bus_id, sizeof((dev)->bus_id), fmt, ##args) 
#endif /* < 2.6.26 */

#ifndef WARN
#define WARN(condition, arg...) ({ \
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on)) {					\
		printk("WARNING: at %s:%d %s()\n",			\
			__FILE__, __LINE__, __func__);			\
		printk(arg);						\
		dump_stack();						\
	}								\
	unlikely(__ret_warn_on);					\
})
#endif

/* force to redefine WARN_ON() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
#undef WARN_ON
#undef WARN_ON_ONCE
#undef WARN_ONCE
#endif

#ifndef WARN_ON
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on)) {					\
		printk("WARNING: at %s:%d %s()\n",			\
			__FILE__, __LINE__, __func__);			\
		dump_stack();						\
	}								\
	unlikely(__ret_warn_on);					\
})
#endif

#ifndef WARN_ON_ONCE
#define WARN_ON_ONCE(condition) ({                              \
        static int __warned;                                    \
        int __ret_warn_once = !!(condition);                    \
                                                                \
        if (unlikely(__ret_warn_once))                          \
                if (WARN_ON(!__warned))                         \
                        __warned = 1;                           \
        unlikely(__ret_warn_once);                              \
})
#endif

#ifndef WARN_ONCE
#define WARN_ONCE(condition, format...)	({			\
	static int __warned;					\
	int __ret_warn_once = !!(condition);			\
								\
	if (unlikely(__ret_warn_once))				\
		if (WARN(!__warned, format)) 			\
			__warned = 1;				\
	unlikely(__ret_warn_once);				\
})
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#define dev_printk(level, dev, format, arg...)	\
	printk(level format, ##arg)
#define dev_emerg(dev, format, arg...)		\
	dev_printk(KERN_EMERG , dev , format , ## arg)
#define dev_alert(dev, format, arg...)		\
	dev_printk(KERN_ALERT , dev , format , ## arg)
#define dev_crit(dev, format, arg...)		\
	dev_printk(KERN_CRIT , dev , format , ## arg)
#define dev_err(dev, format, arg...)		\
	dev_printk(KERN_ERR , dev , format , ## arg)
#define dev_warn(dev, format, arg...)		\
	dev_printk(KERN_WARNING , dev , format , ## arg)
#define dev_notice(dev, format, arg...)		\
	dev_printk(KERN_NOTICE , dev , format , ## arg)
#define dev_info(dev, format, arg...)		\
	dev_printk(KERN_INFO , dev , format , ## arg)
#ifdef DEBUG
#define dev_dbg(dev, format, arg...)		\
	dev_printk(KERN_DEBUG , dev , format , ## arg)
#else
#define dev_dbg(dev, format, arg...)		\
	({ if (0) dev_printk(KERN_DEBUG, dev, format, ##arg); 0; })
#endif
#endif /* < 2.6.0 */

/*
 * wrapper for older SPARC codes with SBUS/EBUS specific bus
 */
#if defined(CONFIG_SBUS) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 28)
struct sbus_dev;
#define snd_dma_sbus_data(sbus)        ((struct device *)(sbus))
#define SNDRV_DMA_TYPE_SBUS            4       /* SBUS continuous */
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
#define DUMP_PREFIX_NONE	0
#define DUMP_PREFIX_OFFSET	1
void snd_compat_print_hex_dump_bytes(const char *prefix_str, int prefix_type,
				     const void *buf, size_t len);
#define print_hex_dump_bytes(a,b,c,d) snd_compat_print_hex_dump_bytes(a,b,c,d)
#endif

/*
 * pci_ioremap_bar() wrapper
 */
#ifdef CONFIG_PCI
#ifndef CONFIG_HAVE_PCI_IOREMAP_BAR
#include <linux/pci.h>
static inline void *pci_ioremap_bar(struct pci_dev *pdev, int bar)
{
	return ioremap_nocache(pci_resource_start(pdev, bar),
			       pci_resource_len(pdev, bar));
}
#endif
#endif

/* pci_name() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 23)
#ifdef CONFIG_PCI
#undef pci_name
#define pci_name(pci)	((pci)->slot_name)
#endif
#endif

/*
 * definition of type 'bool'
 */
#ifndef CONFIG_SND_HAS_BUILTIN_BOOL
typedef int _Bool;
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
#ifndef bool	/* just to be sure */
#if !defined(RHEL_RELEASE_CODE) || RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(5, 4)
typedef _Bool bool;
#endif
#define true	1
#define false	0
#endif
#endif

/* memdup_user() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 30) || \
	defined(CONFIG_SND_DEBUG_MEMORY)
#include <linux/err.h>
#include <asm/uaccess.h>
static inline void *snd_memdup_user(const void __user *src, size_t len)
{
	void *p = kmalloc(len, GFP_KERNEL);
	if (!p)
		return ERR_PTR(-ENOMEM);
	if (copy_from_user(p, src, len)) {
		kfree(p);
		return ERR_PTR(-EFAULT);
	}
	return p;
}
#define memdup_user snd_memdup_user
#endif

/* PCI_VEDEVICE() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 20)
#include <linux/pci.h>
#ifndef PCI_VDEVICE
#define PCI_VDEVICE(vendor, device)		\
	PCI_VENDOR_ID_##vendor, (device),	\
	PCI_ANY_ID, PCI_ANY_ID, 0, 0
#endif
#endif /* < 2.6.20 */

/* put_pid() function was not exported before 2.6.19 */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
#define CONFIG_SND_HAS_REFCOUNTED_STRUCT_PID
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19)
#define CONFIG_SND_HAS_TASK_PID
#endif
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24)
#define CONFIG_SND_HAS_PID_VNR
#endif
#ifndef CONFIG_SND_HAS_REFCOUNTED_STRUCT_PID
/* use nr as pointer */
struct pid;
#define get_pid(p) (p)
#define put_pid(p)
#define task_pid(t) ((struct pid *)((t)->pid))
#define pid_vnr(p) ((pid_t)(p))
#else
#ifndef CONFIG_SND_HAS_TASK_PID
static inline struct pid *task_pid(struct task_struct *task)
{
	return task->pids[PIDTYPE_PID].pid;
}
#endif
#ifndef CONFIG_SND_HAS_PID_VNR
static inline pid_t pid_vnr(struct pid *pid)
{
	return pid ? pid->nr : 0;
}
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 29)
#define pci_clear_master(x)
#endif

/* some new macros */
#ifndef FIELD_SIZEOF
#define FIELD_SIZEOF(t, f) (sizeof(((t*)0)->f))
#endif
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#endif
#ifndef roundup
#define roundup(x, y) ((((x) + ((y) - 1)) / (y)) * (y))
#endif
#ifndef DIV_ROUND_CLOSEST
#define DIV_ROUND_CLOSEST(x, divisor)(			\
{							\
	typeof(divisor) __divisor = divisor;		\
	(((x) + ((__divisor) / 2)) / (__divisor));	\
}							\
)
#endif

/* skip_spaces() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 33)
char *compat_skip_spaces(const char *);
#define skip_spaces	compat_skip_spaces
#endif

/* subsys_initcall() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 0)
#include <linux/init.h>
#ifndef subsys_initcall
#define subsys_initcall(x) module_init(x)
#endif
#endif

/* DEFINE_PCI_DEVICE_TABLE() */
#ifdef CONFIG_PCI
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 25)
#ifndef DEFINE_PCI_DEVICE_TABLE
/* originally it's __devinitconst but we use __devinitdata to be compatible
 * with older kernels
 */
#define DEFINE_PCI_DEVICE_TABLE(_table) \
	const struct pci_device_id _table[] __devinitdata
#endif
#endif /* < 2.6.25 */
#endif /* CONFIG_PCI */

/* blocking_notifier */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 17)
#include <linux/notifier.h>
#ifndef BLOCKING_INIT_NOTIFIER_HEAD
struct blocking_notifier_head {
	struct rw_semaphore rwsem;
	struct notifier_block *head;
};

#define BLOCKING_INIT_NOTIFIER_HEAD(name) do {	\
		init_rwsem(&(name)->rwsem);	\
		(name)->head = NULL;		\
	} while (0)

static inline int
blocking_notifier_call_chain(struct blocking_notifier_head *nh,
			     unsigned long val, void *v)
{
	int ret;
	down_read(&nh->rwsem);
	ret = notifier_call_chain(&nh->head, val, v);
	up_read(&nh->rwsem);
	return ret;
}

static inline int
blocking_notifier_chain_register(struct blocking_notifier_head *nh,
				 struct notifier_block *nb)
{
	int ret;
	down_write(&nh->rwsem);
	ret = notifier_chain_register(&nh->head, nb);
	up_write(&nh->rwsem);
	return ret;
}

static inline int
blocking_notifier_chain_unregister(struct blocking_notifier_head *nh,
				   struct notifier_block *nb)
{
	int ret;
	down_write(&nh->rwsem);
	ret = notifier_chain_unregister(&nh->head, nb);
	up_write(&nh->rwsem);
	return ret;
}
#endif /* BLOCKING_INIT_NOTIFIER_HEAD */
#endif /* <2.6.17 */

/* pgprot_noncached - 2.4 has different defines */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
#ifndef pgprot_noncached
#define pgprot_noncached(x) (x)
#endif
#endif

/* no_llseek() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 0)
#define no_llseek	NULL
#endif

/* noop_llseek() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#define noop_llseek	NULL
#endif

/* nonseekable_open() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
#define nonseekable_open(i,f) 0
#endif

/* hex_to_bin() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
/* RHEL 6.1 kernels has version 2.6.32, but already have hex_to_bin() */
#if !defined(RHEL_RELEASE_CODE) || RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(6,1)
static inline int hex_to_bin(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}
#endif
#endif

#ifndef CONFIG_HAVE_VZALLOC
#include <linux/vmalloc.h>
static inline void *vzalloc(unsigned long size)
{
	void *p = vmalloc(size);
	if (p)
		memset(p, 0, size);
	return p;
}
#endif

/* flush_delayed_work_sync() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)
#include <linux/workqueue.h>
static inline bool flush_work_sync(struct work_struct *work)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
	/* XXX */
	flush_scheduled_work();
	return true;
#else
	if (!flush_work(work))
		return false;
	while (flush_work(work))
		;
	return true;
#endif
}

static inline bool flush_delayed_work_sync(struct delayed_work *dwork)
{
	bool ret;
	ret = cancel_delayed_work(dwork);
	if (ret) {
		schedule_delayed_work(dwork, 0);
		flush_scheduled_work();
	}
	return ret;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 23)
/* XXX this is a workaround; these are really different, but almost same
 * as used in the usual free/error path
 */
#define cancel_delayed_work_sync flush_delayed_work_sync
#endif

/* cancel_work_sync wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)
/* XXX */
#define cancel_work_sync(w)	flush_scheduled_work()
#endif

/* to_delayed_work() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 30)
#define to_delayed_work(w) \
	((struct delayed_work *)container_of(w, struct delayed_work, work))
#endif

#endif /* < 2.6.37 */

/* pm_wakeup_event() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 37)
#define pm_wakeup_event(dev, msec)
#endif

/* request_any_context_irq() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 35)
#define request_any_context_irq(irq, fn, flags, name, dev_id) \
	request_irq(irq, fn, flags, name, dev_id)
#endif

/* usleep_range() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 36)
/* FIXME: assuming msleep() */
#define usleep_range(x, y)	msleep(((x) + 999) / 1000)
#endif

/* hack - CONFIG_SND_HDA_INPUT_JACK can be wrongly set for older kernels */
#ifndef CONFIG_SND_JACK
#undef CONFIG_SND_HDA_INPUT_JACK
#endif

/* krealloc() wrapper */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 22)) || \
	(defined(CONFIG_SND_DEBUG_MEMORY) && !defined(SKIP_HIDDEN_MALLOCS))
#include <linux/slab.h>
static inline void *snd_compat_krealloc(const void *p, size_t new_size, gfp_t flags)
{
	void *n;
	if (!p)
		return kmalloc(new_size, flags);
	if (!new_size) {
		kfree(p);
		return NULL;
	}
	n = kmalloc(new_size, flags);
	if (!n)
		return NULL;
	memcpy(n, p, min(new_size, (size_t)ksize(p)));
	kfree(p);
	return n;
}
#define krealloc(p, s, f)	snd_compat_krealloc(p, s, f)
#endif

/* Workaround for IRQF_DISABLED removal in the upstream code */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 1, 0)
#include <linux/interrupt.h>
static inline int
request_irq_with_irqf_check(unsigned int irq, irq_handler_t handler,
			    unsigned long flags, const char *name, void *dev)
{
	if (!(flags & ~IRQF_PROBE_SHARED))
		flags |= IRQF_DISABLED;
	return request_irq(irq, handler, flags, name, dev);
}
#undef request_irq
#define request_irq(irq, fn, flags, name, dev_id) \
	request_irq_with_irqf_check(irq, fn, flags, name, dev_id)
#endif

/* if no __printf() macro is defined, just ignore it
 * (to be safer than defining gcc-specific)
 */
#ifndef __printf
#define __printf(a,b)	/*nop*/
#endif

/* module_platform_driver() wrapper */
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 2, 0)
#include <linux/platform_device.h>
#ifndef module_platform_driver
#define module_platform_driver(__platform_driver) \
static int __init __platform_driver##_init(void) \
{ \
	return platform_driver_register(&(__platform_driver)); \
} \
module_init(__platform_driver##_init); \
static void __exit __platform_driver##_exit(void) \
{ \
	platform_driver_unregister(&(__platform_driver)); \
} \
module_exit(__platform_driver##_exit);
#endif
#endif /* < 3.2 */

/* module_usb_driver() wrapper */
#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 2, 0)
#include <linux/device.h>
#ifndef module_driver
#define module_driver(__driver, __register, __unregister) \
static int __init __driver##_init(void) { return __register(&(__driver)); } \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) { __unregister(&(__driver)); } \
module_exit(__driver##_exit);
#define module_usb_driver(__usb_driver) \
    module_driver(__usb_driver, usb_register, \
			usb_deregister)
#endif
#endif /* <= 3.2 */

/* some old kernels define info(), and this breaks the build badly */
#ifdef info
#undef info
#endif

#endif /* __SOUND_LOCAL_DRIVER_H */
