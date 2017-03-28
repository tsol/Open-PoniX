#include "alsa-autoconf.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define SND_NEED_USB_WRAPPER
#define __NO_VERSION__
#endif
#include "adriver.h"
#include "usbcompat.h"
