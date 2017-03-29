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
"depends=snd-sb-common,snd-opl3-lib,snd-mpu401-uart,snd-sb16-dsp,snd";

MODULE_ALIAS("pnp:d@@@0001*");
MODULE_ALIAS("acpi*:@@@0001:*");
MODULE_ALIAS("pnp:d@X@0001*");
MODULE_ALIAS("acpi*:@X@0001:*");
MODULE_ALIAS("pnp:d@H@0001*");
MODULE_ALIAS("acpi*:@H@0001:*");
MODULE_ALIAS("pnp:d@@@1001*");
MODULE_ALIAS("acpi*:@@@1001:*");
MODULE_ALIAS("pnp:d@X@1001*");
MODULE_ALIAS("acpi*:@X@1001:*");
MODULE_ALIAS("pnp:d@H@1001*");
MODULE_ALIAS("acpi*:@H@1001:*");
MODULE_ALIAS("pnp:d@@@2001*");
MODULE_ALIAS("acpi*:@@@2001:*");
MODULE_ALIAS("pnp:d@X@2001*");
MODULE_ALIAS("acpi*:@X@2001:*");
MODULE_ALIAS("pnp:d@H@2001*");
MODULE_ALIAS("acpi*:@H@2001:*");
MODULE_ALIAS("pnp:d@@@0020*");
MODULE_ALIAS("acpi*:@@@0020:*");
MODULE_ALIAS("pnp:d@X@0020*");
MODULE_ALIAS("acpi*:@X@0020:*");
MODULE_ALIAS("pnp:d@H@0020*");
MODULE_ALIAS("acpi*:@H@0020:*");
