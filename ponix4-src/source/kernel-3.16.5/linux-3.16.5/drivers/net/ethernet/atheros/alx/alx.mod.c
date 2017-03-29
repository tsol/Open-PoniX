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
"depends=mdio";

MODULE_ALIAS("pci:v00001969d00001091sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001969d0000E091sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001969d00001090sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001969d000010A1sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001969d000010A0sv*sd*bc*sc*i*");
