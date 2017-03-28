#include <pcmcia/cs.h>
#if LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 35)
#include "../../alsa-kernel/pcmcia/vx/vxpocket.h"
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
#include "vxpocket-2.6.35.h"
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 16)
#include "vxpocket-2.6.34.h"
#else
#include "vxpocket-old.h"
#endif
