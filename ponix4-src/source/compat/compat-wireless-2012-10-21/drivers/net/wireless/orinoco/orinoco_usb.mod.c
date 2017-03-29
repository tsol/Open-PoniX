#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=orinoco,compat";

MODULE_ALIAS("usb:v049Fp001Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v049Fp0082d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v049Fp0076d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p0006d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p000Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p000Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v047Ep0300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D98p0300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D9Ep0300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D4Ep1000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D4Ep1001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v05CCp3100d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E7Cp0300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p5002d0000dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p5B11d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p7011d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0681p0012d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BF8p1002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1630pFF81d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0D4Ep047Ad*dc*dsc*dp*ic*isc*ip*");
