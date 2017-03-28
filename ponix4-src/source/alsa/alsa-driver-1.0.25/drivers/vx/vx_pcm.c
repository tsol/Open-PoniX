#include <linux/version.h>
#define __NO_VERSION__

#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 2, 18)
#define vmalloc_32(x) vmalloc_nocheck(x)
#endif

#include "adriver.h"
#include "../../alsa-kernel/drivers/vx/vx_pcm.c"
