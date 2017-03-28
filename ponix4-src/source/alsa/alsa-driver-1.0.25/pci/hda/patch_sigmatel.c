#define __NO_VERSION__
#include "adriver.h"

/* dmi wrappers */
#include <linux/dmi.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 14)
#define dmi_find_device(a, b, c)	NULL
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 19)
#define DMI_DEV_TYPE_OEM_STRING		-2
#endif

#include "../../alsa-kernel/pci/hda/patch_sigmatel.c"
