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

MODULE_ALIAS("usb:v1B75p8187d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8187d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0846p6100d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0846p6A00d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "E1E15894BD1429E2987D5B8");
