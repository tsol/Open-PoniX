#include "alsa-autoconf.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5)
#define SND_NEED_USB_SET_INTERFACE
#endif
// #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define SND_NEED_USB_WRAPPER
// #endif
#include "adriver.h"
#include "usbcompat.h"

/* wrapper for hweight16() */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 4, 21) || \
	(LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0) && \
	 LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 10))
static unsigned int _hweight16(unsigned short x)
{
	unsigned short bits;
	for (bits = 0; x; ++bits)
		x &= x - 1;
	return bits;
}
#define hweight16	_hweight16
#endif

// vim: ft=c
