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
"depends=snd-es1688-lib,snd-opl3-lib,snd-mpu401-uart,snd";

MODULE_ALIAS("pnp:d@@@0968*");
MODULE_ALIAS("acpi*:@@@0968:*");
MODULE_ALIAS("pnp:dESS0968*");
MODULE_ALIAS("acpi*:ESS0968:*");
