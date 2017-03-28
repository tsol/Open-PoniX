#include "adriver.h"
/* XXX */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2, 6, 39)
#define irq_set_irq_wake(irq,on)	0
#endif
#include "../alsa-kernel/soc/soc-jack.c"
