#include "alsa-autoconf.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,5)
#define SND_NEED_USB_SET_INTERFACE
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define SND_NEED_USB_WRAPPER
#endif
#endif
#include "adriver.h"
#include "../usbcompat.h"
