#include "adriver.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 27)
/* HACK HACK - debugfs_remove_recursive() isn't defined */
#undef CONFIG_DEBUG_FS
#endif
#include "../alsa-kernel/soc/soc-dapm.c"
