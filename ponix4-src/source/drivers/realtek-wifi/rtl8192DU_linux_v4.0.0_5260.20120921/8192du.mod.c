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
"depends=";

MODULE_ALIAS("usb:v0BDAp8193d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8194d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8111d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp0193d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8171d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB2Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019p4903d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019p4904d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p8193d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v20F4p664Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0930p0A0Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB2Dd*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "9E8B440B9CC7CC6ADA4A0C0");
