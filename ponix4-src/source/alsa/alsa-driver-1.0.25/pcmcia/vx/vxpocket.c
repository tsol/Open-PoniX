#include "alsa-autoconf.h"
#include "adriver.h"
#include <pcmcia/cs_types.h>

#if LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,15)
#include "vxpocket_old.c"
#elif LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,16)
#include "vxpocket-2.6.16.c"
#elif LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,34)
#include "vxpocket-2.6.34.c"
#elif !defined(SND_HAVE_DUMMY_CS_TYPES_H)
#include "vxpocket-2.6.35.c"
#elif LINUX_VERSION_CODE <= KERNEL_VERSION(2,6,36)
#include "vxpocket-2.6.36.c"
#else
#include "../../alsa-kernel/pcmcia/vx/vxpocket.c"
#endif

