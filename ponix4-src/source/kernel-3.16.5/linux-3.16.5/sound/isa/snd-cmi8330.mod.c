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
"depends=snd-sb-common,snd-wss-lib,snd-opl3-lib,snd-mpu401-uart,snd-pcm,snd-sb16-dsp,snd";

MODULE_ALIAS("pnp:d@X@0001*");
MODULE_ALIAS("acpi*:@X@0001:*");
MODULE_ALIAS("pnp:d@@@0001*");
MODULE_ALIAS("acpi*:@@@0001:*");
MODULE_ALIAS("pnp:d@H@0001*");
MODULE_ALIAS("acpi*:@H@0001:*");
MODULE_ALIAS("pnp:dA@@0001*");
MODULE_ALIAS("acpi*:A@@0001:*");
