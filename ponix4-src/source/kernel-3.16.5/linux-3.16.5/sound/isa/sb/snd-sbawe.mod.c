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
"depends=snd-sb-common,snd-opl3-lib,snd-mpu401-uart,snd-sb16-dsp,snd,snd-sb16-csp,snd-seq-device";

MODULE_ALIAS("pnp:dCTL0031*");
MODULE_ALIAS("acpi*:CTL0031:*");
MODULE_ALIAS("pnp:dCTL0021*");
MODULE_ALIAS("acpi*:CTL0021:*");
MODULE_ALIAS("pnp:dCTL0041*");
MODULE_ALIAS("acpi*:CTL0041:*");
MODULE_ALIAS("pnp:dCTL0042*");
MODULE_ALIAS("acpi*:CTL0042:*");
MODULE_ALIAS("pnp:dCTL0022*");
MODULE_ALIAS("acpi*:CTL0022:*");
MODULE_ALIAS("pnp:dCTL0044*");
MODULE_ALIAS("acpi*:CTL0044:*");
MODULE_ALIAS("pnp:dCTL0023*");
MODULE_ALIAS("acpi*:CTL0023:*");
MODULE_ALIAS("pnp:dCTL0045*");
MODULE_ALIAS("acpi*:CTL0045:*");
MODULE_ALIAS("pnp:dCTL0070*");
MODULE_ALIAS("acpi*:CTL0070:*");
