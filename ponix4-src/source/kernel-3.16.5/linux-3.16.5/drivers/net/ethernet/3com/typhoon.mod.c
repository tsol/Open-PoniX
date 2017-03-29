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
"depends=";

MODULE_ALIAS("pci:v000010B7d00009900sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009902sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009903sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009904sv*sd00001000bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009904sv*sd00001102bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009904sv*sd00002000bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009905sv*sd00001101bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009905sv*sd00001102bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009905sv*sd00002101bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009905sv*sd00002102bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009908sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009909sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d0000990Asv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "A629E5206DCF3673F42B970");
