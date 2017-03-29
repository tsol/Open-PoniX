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
"depends=mdio,ptp";

MODULE_ALIAS("pci:v00001924d00000703sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001924d00000710sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001924d00000803sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001924d00000813sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001924d00000903sv*sd*bc*sc*i*");
