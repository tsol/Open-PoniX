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
"depends=snd-wss-lib,snd-pcm,snd,snd-opl3-lib,snd-mpu401-uart";

MODULE_ALIAS("pnp:dCSC0100*");
MODULE_ALIAS("acpi*:CSC0100:*");
MODULE_ALIAS("pnp:dCSC0000*");
MODULE_ALIAS("acpi*:CSC0000:*");
MODULE_ALIAS("pnp:dGIM0100*");
MODULE_ALIAS("acpi*:GIM0100:*");
MODULE_ALIAS("pnp:dCSC0000*");
MODULE_ALIAS("acpi*:CSC0000:*");
MODULE_ALIAS("pnp:dCSC0010*");
MODULE_ALIAS("acpi*:CSC0010:*");
MODULE_ALIAS("pnp:dPNPb006*");
MODULE_ALIAS("acpi*:PNPB006:*");
MODULE_ALIAS("pnp:dCSC0003*");
MODULE_ALIAS("acpi*:CSC0003:*");
MODULE_ALIAS("pnp:dCSC0002*");
MODULE_ALIAS("acpi*:CSC0002:*");
MODULE_ALIAS("pnp:dCSCb006*");
MODULE_ALIAS("acpi*:CSCB006:*");
MODULE_ALIAS("pnp:dCSC0100*");
MODULE_ALIAS("acpi*:CSC0100:*");
MODULE_ALIAS("pnp:dCSC0110*");
MODULE_ALIAS("acpi*:CSC0110:*");
MODULE_ALIAS("pnp:dCSC010f*");
MODULE_ALIAS("acpi*:CSC010F:*");
MODULE_ALIAS("pnp:dCSC0103*");
MODULE_ALIAS("acpi*:CSC0103:*");
MODULE_ALIAS("pnp:dCSCa800*");
MODULE_ALIAS("acpi*:CSCA800:*");
MODULE_ALIAS("pnp:dCSCa810*");
MODULE_ALIAS("acpi*:CSCA810:*");
MODULE_ALIAS("pnp:dCSCa803*");
MODULE_ALIAS("acpi*:CSCA803:*");
