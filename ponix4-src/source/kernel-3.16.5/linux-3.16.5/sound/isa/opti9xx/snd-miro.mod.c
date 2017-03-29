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
"depends=snd-mpu401-uart,snd-opl4-lib,snd,snd-wss-lib";

MODULE_ALIAS("pnp:dMIR0000*");
MODULE_ALIAS("acpi*:MIR0000:*");
MODULE_ALIAS("pnp:dMIR0002*");
MODULE_ALIAS("acpi*:MIR0002:*");
MODULE_ALIAS("pnp:dMIR0005*");
MODULE_ALIAS("acpi*:MIR0005:*");
