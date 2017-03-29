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
"depends=snd,snd-pcm,snd-opl3-lib,snd-mpu401-uart,snd-timer";

MODULE_ALIAS("pnp:dADS7150*");
MODULE_ALIAS("acpi*:ADS7150:*");
MODULE_ALIAS("pnp:dADS7151*");
MODULE_ALIAS("acpi*:ADS7151:*");
MODULE_ALIAS("pnp:dADS7180*");
MODULE_ALIAS("acpi*:ADS7180:*");
MODULE_ALIAS("pnp:dADS7181*");
MODULE_ALIAS("acpi*:ADS7181:*");
MODULE_ALIAS("pnp:dAZT1018*");
MODULE_ALIAS("acpi*:AZT1018:*");
MODULE_ALIAS("pnp:dAZT2002*");
MODULE_ALIAS("acpi*:AZT2002:*");
MODULE_ALIAS("pnp:dTER1100*");
MODULE_ALIAS("acpi*:TER1100:*");
MODULE_ALIAS("pnp:dTER1101*");
MODULE_ALIAS("acpi*:TER1101:*");
