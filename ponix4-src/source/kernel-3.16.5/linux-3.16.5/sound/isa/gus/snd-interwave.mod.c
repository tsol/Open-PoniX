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
"depends=snd-wss-lib,snd-gus-lib,snd";

MODULE_ALIAS("pnp:dGRV0000*");
MODULE_ALIAS("acpi*:GRV0000:*");
MODULE_ALIAS("pnp:dSTB0010*");
MODULE_ALIAS("acpi*:STB0010:*");
MODULE_ALIAS("pnp:dDXP0010*");
MODULE_ALIAS("acpi*:DXP0010:*");
MODULE_ALIAS("pnp:dCDC1112*");
MODULE_ALIAS("acpi*:CDC1112:*");
MODULE_ALIAS("pnp:dADV0010*");
MODULE_ALIAS("acpi*:ADV0010:*");
