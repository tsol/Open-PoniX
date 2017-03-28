#ifndef __ALSA_USBCOMPAT_H
#define __ALSA_USBCOMPAT_H

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 24)
#include <linux/usb.h>

static inline int usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

static inline int usb_endpoint_type(const struct usb_endpoint_descriptor *epd)
{
	return epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK;
}
#endif /* < 2.6.24 */

#if !defined(RHEL_RELEASE_CODE) || RHEL_RELEASE_CODE < RHEL_RELEASE_VERSION(5, 4)
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
static inline int usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN);
}

static inline int usb_endpoint_dir_out(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT);
}

static inline int usb_endpoint_xfer_bulk(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_BULK);
}

static inline int usb_endpoint_xfer_int(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_INT);
}

static inline int usb_endpoint_xfer_isoc(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_ISOC);
}

static inline int usb_endpoint_is_bulk_in(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_in(epd));
}

static inline int usb_endpoint_is_bulk_out(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_bulk(epd) && usb_endpoint_dir_out(epd));
}

static inline int usb_endpoint_is_int_in(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_int(epd) && usb_endpoint_dir_in(epd));
}

static inline int usb_endpoint_is_int_out(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_int(epd) && usb_endpoint_dir_out(epd));
}

static inline int usb_endpoint_is_isoc_in(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_in(epd));
}

static inline int usb_endpoint_is_isoc_out(
				const struct usb_endpoint_descriptor *epd)
{
	return (usb_endpoint_xfer_isoc(epd) && usb_endpoint_dir_out(epd));
}
#endif /* < 2.6.19 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 21)
static inline int usb_endpoint_xfer_control(
				const struct usb_endpoint_descriptor *epd)
{
	return ((epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		USB_ENDPOINT_XFER_CONTROL);
}
#endif /* < 2.6.21 */
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
#define PM_EVENT_AUTO	0
static inline int usb_autopm_get_interface(struct usb_interface *intf)
{
	return 0;
}
static inline void usb_autopm_put_interface(struct usb_interface *intf)
{
}
#endif /* < 2.6.20 */

#endif /* __ALSA_USBCOMPAT_H */
