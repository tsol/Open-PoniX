#include "adriver.h"
#if LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 39)
#include "iso-resources-old.h"
#endif
#include "../alsa-kernel/firewire/speakers.c"
EXPORT_NO_SYMBOLS;
