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
"depends=bluetooth";

MODULE_ALIAS("usb:v0CF3p3000d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CF3p3002d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3304d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0930p0215d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v03F0p311Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CF3p3004d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0489pE02Cd*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "A05EB38FCA8402A89CD2DAA");
