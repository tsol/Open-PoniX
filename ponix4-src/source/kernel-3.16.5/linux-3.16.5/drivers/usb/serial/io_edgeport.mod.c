#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

MODULE_INFO(intree, "Y");

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=usbserial";

MODULE_ALIAS("usb:v1608p0001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0003d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0004d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p1403d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0005d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0006d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0007d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p000Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p000Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p000Ed*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p000Fd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0010d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p001Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0019d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0014d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1608p0018d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0404p0202d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0404p0203d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0404p0310d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0404p0311d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0404p0312d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05D9pA758d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05D9pA794d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05D9pA225d*dc*dsc*dp*ic*isc*ip*in*");
