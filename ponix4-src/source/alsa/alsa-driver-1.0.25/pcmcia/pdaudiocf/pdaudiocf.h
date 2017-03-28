#include <pcmcia/cs.h>
#ifdef SND_HAVE_DUMMY_CS_TYPES_H
#include "../../alsa-kernel/pcmcia/pdaudiocf/pdaudiocf.h"
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35)
#include "pdaudiocf-2.6.35.h"
#elif LINUX_VERSION_CODE > KERNEL_VERSION(2,6,16)
#include "pdaudiocf-2.6.34.h"
#else
#include "pdaudiocf-old.h"
#endif
