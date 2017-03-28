#ifndef __BACKPORT_VGA_SWITCHEROO_H
#define __BACKPORT_VGA_SWITCHEROO_H
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34)
#include_next <linux/vga_switcheroo.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,5,0)
/*
 * This backports:
 *
 *   From 26ec685ff9d9c16525d8ec4c97e52fcdb187b302 Mon Sep 17 00:00:00 2001
 *   From: Takashi Iwai <tiwai@suse.de>
 *   Date: Fri, 11 May 2012 07:51:17 +0200
 *   Subject: [PATCH] vga_switcheroo: Introduce struct vga_switcheroo_client_ops
 *
 */

struct vga_switcheroo_client_ops {
    void (*set_gpu_state)(struct pci_dev *dev, enum vga_switcheroo_state);
    void (*reprobe)(struct pci_dev *dev);
    bool (*can_switch)(struct pci_dev *dev);
};

/* Wrap around the old code and redefine vga_switcheroo_register_client()
 * for older kernels < 3.5.0.
 */
static inline int compat_vga_switcheroo_register_client(struct pci_dev *dev,
		const struct vga_switcheroo_client_ops *ops) {

	return vga_switcheroo_register_client(dev,
					      ops->set_gpu_state,
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,38))
					      ops->reprobe,
#endif
					      ops->can_switch);
}

#define vga_switcheroo_register_client(_dev, _ops) \
	compat_vga_switcheroo_register_client(_dev, _ops)

#endif /* < 3.5 */

#endif /* >= 2.6.34 */
#endif /* __BACKPORT_VGA_SWITCHEROO_H */
