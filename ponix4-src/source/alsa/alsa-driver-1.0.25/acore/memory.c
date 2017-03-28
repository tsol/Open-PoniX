#include "alsa-autoconf.h"
#define __NO_VERSION__

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,12)
#include <linux/compiler.h>
#ifndef __iomem
#define __iomem
#endif
#ifndef __user
#define __user
#endif
#ifndef __kernel
#define __kernel
#endif
#ifndef __nocast
#define __nocast
#endif
#ifndef __force
#define __force
#endif
#ifndef __safe
#define __safe
#endif
#ifndef __bitwise
#define __bitwise
#endif
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
#include "adriver.h"
#endif /* KERNEL < 2.6.0 */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,21)
#include <linux/compiler.h>
#ifndef __printf
#define __printf(a,b)   /*nop*/
#endif
#endif

#include "../alsa-kernel/core/memory.c"
