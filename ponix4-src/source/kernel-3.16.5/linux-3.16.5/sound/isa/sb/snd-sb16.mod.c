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
"depends=snd-sb-common,snd-opl3-lib,snd-mpu401-uart,snd-sb16-dsp,snd,snd-sb16-csp";

MODULE_ALIAS("pnp:dCTL0031*");
MODULE_ALIAS("acpi*:CTL0031:*");
MODULE_ALIAS("pnp:dCTL0001*");
MODULE_ALIAS("acpi*:CTL0001:*");
MODULE_ALIAS("pnp:dCTL0041*");
MODULE_ALIAS("acpi*:CTL0041:*");
MODULE_ALIAS("pnp:dCTL0043*");
MODULE_ALIAS("acpi*:CTL0043:*");
MODULE_ALIAS("pnp:dPNPb003*");
MODULE_ALIAS("acpi*:PNPB003:*");
