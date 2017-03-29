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
"depends=snd-opl3-lib,snd-mpu401-uart,snd,snd-wss-lib";

MODULE_ALIAS("pnp:dAZT1008*");
MODULE_ALIAS("acpi*:AZT1008:*");
MODULE_ALIAS("pnp:dAZT2001*");
MODULE_ALIAS("acpi*:AZT2001:*");
MODULE_ALIAS("pnp:dAZT0001*");
MODULE_ALIAS("acpi*:AZT0001:*");
MODULE_ALIAS("pnp:dAZT0002*");
MODULE_ALIAS("acpi*:AZT0002:*");
MODULE_ALIAS("pnp:dAZT1003*");
MODULE_ALIAS("acpi*:AZT1003:*");
MODULE_ALIAS("pnp:dAZT1004*");
MODULE_ALIAS("acpi*:AZT1004:*");
