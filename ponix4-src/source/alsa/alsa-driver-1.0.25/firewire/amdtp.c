#define __NO_VERSION__
#include "adriver.h"
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 39)
#define fw_iso_context_queue_flush(x)
#endif
#include "../alsa-kernel/firewire/amdtp.c"
