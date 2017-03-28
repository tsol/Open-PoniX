/* workarounds for USB API */
#include <linux/usb.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 20)
inline static int usb_make_path(struct usb_device *dev, char *buf, size_t size)
{
	int actual;
	actual = snprintf(buf, size, "%03d/%03d", dev->bus->busnum, dev->devnum);
	return (actual >= (int)size) ? -1 : actual;
}
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)
inline static struct urb *usb_alloc_urb_wrapper(int iso_packets, int flags)
{
	return usb_alloc_urb(iso_packets);
}
inline static int usb_submit_urb_wrapper(struct urb *urb, int flags)
{
	return usb_submit_urb(urb);
}
#undef usb_alloc_urb
#undef usb_submit_urb
#define usb_alloc_urb(n,flags) usb_alloc_urb_wrapper(n,flags)
#define usb_submit_urb(p,flags) usb_submit_urb_wrapper(p,flags)
#define OLD_USB
#endif /* LINUX_VERSION_CODE < 2.5.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 24)
int snd_hack_usb_set_interface(struct usb_device *dev, int interface, int alternate);
#undef usb_set_interface
#define usb_set_interface(dev,iface,alt) snd_hack_usb_set_interface(dev,iface,alt)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 45)
#define URB_ISO_ASAP		USB_ISO_ASAP
#define URB_ASYNC_UNLINK	USB_ASYNC_UNLINK
#define usb_fill_int_urb	FILL_INT_URB
#define usb_fill_bulk_urb	FILL_BULK_URB
#define usb_fill_control_urb	FILL_CONTROL_URB
#define usb_host_config		usb_config_descriptor
#define usb_host_interface	usb_interface_descriptor
#define usb_host_endpoint	usb_endpoint_descriptor
#define get_iface_desc(iface)	(iface)
#define get_endpoint(alt,ep)	(&(alt)->endpoint[ep])
#define get_ep_desc(ep)		(ep)
#define get_cfg_desc(cfg)	(cfg)
#define USB_DT_CS_DEVICE	0x21
#define USB_DT_CS_CONFIG	0x22
#define USB_DT_CS_STRING	0x23
#define USB_DT_CS_INTERFACE	0x24
#define USB_DT_CS_ENDPOINT	0x25
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 45)
#define usb_pipe_needs_resubmit(pipe) (!usb_pipeint(pipe))
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 3, 0)
enum {
	USB_SPEED_UNKNOWN = 0,
	USB_SPEED_LOW, USB_SPEED_FULL,
	USB_SPEED_HIGH
};
#define snd_usb_get_speed(dev) USB_SPEED_FULL
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 8)
#define usb_kill_urb(urb) usb_unlink_urb(urb)
#endif

#ifndef HAVE_USB_BUFFERS /* defined in usb.h */
#define usb_buffer_alloc(dev, size, flags, dma) kmalloc(size, flags)
#define usb_buffer_free(dev, size, addr, dma) kfree(addr)
#undef URB_NO_TRANSFER_DMA_MAP
#define URB_NO_TRANSFER_DMA_MAP 0
#endif

#ifndef CONFIG_SND_HAVE_USB_ALLOC_COHERENT
#define usb_alloc_coherent	usb_buffer_alloc
#define usb_free_coherent	usb_buffer_free
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 3, 0) \
    && LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 20)
struct usb_ctrlrequest {
	__u8 bRequestType;
	__u8 bRequest;
	__u16 wValue;
	__u16 wIndex;
	__u16 wLength;
} __attribute__ ((packed));
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#define usb_interrupt_msg	usb_bulk_msg
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 5, 24) \
    && LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 5) \
    && defined(SND_NEED_USB_SET_INTERFACE)

#include <linux/usb.h>

inline static int real_usb_set_interface(struct usb_device *dev, int interface, int alternate)
{
	return usb_set_interface(dev, interface, alternate);
}

int snd_hack_usb_set_interface(struct usb_device *dev, int interface, int alternate);
#undef usb_set_interface
#define usb_set_interface(dev,iface,alt) snd_hack_usb_set_interface(dev,iface,alt)

#endif /* >= 2.5.24 && < 2.6.5 && SND_NEED_USB_SET_INTERFACE */

/*
 * USB 3.0
 */
#ifndef USB_SPEED_SUPER
#define USB_SPEED_SUPER		0 /* dummy */
#endif
