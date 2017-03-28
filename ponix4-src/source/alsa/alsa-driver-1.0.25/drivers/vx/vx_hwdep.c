#include <linux/version.h>
#define __NO_VERSION__
/* to be in alsa-driver-specific code */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#define CONFIG_USE_VXLOADER
#endif
#include "adriver.h"
#include "../../alsa-kernel/drivers/vx/vx_hwdep.c"
