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

MODULE_ALIAS("usb:v148Fp3572d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9801d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0041d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0042d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0944d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0740d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0744d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0284d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v167Bp4001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0764d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0930p0A07d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0761d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13B1p002Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0079d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3573d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7733d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0846p9012d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "9C18FCD8F85C6DAB6ABDE1C");
