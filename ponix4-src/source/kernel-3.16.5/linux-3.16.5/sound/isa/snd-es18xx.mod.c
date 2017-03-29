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
"depends=snd-pcm,snd,snd-opl3-lib,snd-mpu401-uart";

MODULE_ALIAS("pnp:dESS1868*");
MODULE_ALIAS("acpi*:ESS1868:*");
MODULE_ALIAS("pnp:dESS0000*");
MODULE_ALIAS("acpi*:ESS0000:*");
MODULE_ALIAS("pnp:dESS8601*");
MODULE_ALIAS("acpi*:ESS8601:*");
MODULE_ALIAS("pnp:dESS8600*");
MODULE_ALIAS("acpi*:ESS8600:*");
MODULE_ALIAS("pnp:dESS8611*");
MODULE_ALIAS("acpi*:ESS8611:*");
MODULE_ALIAS("pnp:dESS8610*");
MODULE_ALIAS("acpi*:ESS8610:*");
MODULE_ALIAS("pnp:dESS1869*");
MODULE_ALIAS("acpi*:ESS1869:*");
MODULE_ALIAS("pnp:dESS0006*");
MODULE_ALIAS("acpi*:ESS0006:*");
MODULE_ALIAS("pnp:dESS1878*");
MODULE_ALIAS("acpi*:ESS1878:*");
MODULE_ALIAS("pnp:dESS0004*");
MODULE_ALIAS("acpi*:ESS0004:*");
MODULE_ALIAS("pnp:dESS1879*");
MODULE_ALIAS("acpi*:ESS1879:*");
MODULE_ALIAS("pnp:dESS0009*");
MODULE_ALIAS("acpi*:ESS0009:*");
MODULE_ALIAS("pnp:dESS1869*");
MODULE_ALIAS("acpi*:ESS1869:*");
MODULE_ALIAS("pnp:dESS1879*");
MODULE_ALIAS("acpi*:ESS1879:*");
