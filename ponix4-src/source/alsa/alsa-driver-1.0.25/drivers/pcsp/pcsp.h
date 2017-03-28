#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
#include "../../alsa-kernel/drivers/pcsp/pcsp.h"
#else
#include "pcsp-2.6.34.h"
#endif
