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
"depends=usbnet,cdc_ether";

MODULE_ALIAS("usb:v1630p0042d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*ic02isc02ipFF*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*icEFisc01ip01*");
MODULE_ALIAS("usb:v*p*d*dc*dsc*dp*icE0isc01ip03*");
