#include "adriver.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 18)
#include <linux/mutex.h>
#ifndef mutex_lock_nested
#define mutex_lock_nested(a, b) mutex_lock(a)
#endif
#endif
#include "../alsa-kernel/soc/soc-pcm.c"
