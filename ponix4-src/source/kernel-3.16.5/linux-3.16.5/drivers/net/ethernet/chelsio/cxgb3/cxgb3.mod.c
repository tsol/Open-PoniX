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

MODULE_ALIAS("pci:v00001425d00000020sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000021sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000022sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000023sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000024sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000025sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000026sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000030sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000031sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000032sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000035sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000036sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00000037sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "A8E4B482A54754538963ED7");
