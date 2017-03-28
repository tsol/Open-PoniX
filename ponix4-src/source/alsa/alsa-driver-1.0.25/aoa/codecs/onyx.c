#include "ppc-prom-hack.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
#include "onyx_old_i2c.c"
#else
#include "../../alsa-kernel/aoa/codecs/onyx.c"
#endif
