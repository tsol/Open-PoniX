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
"depends=mac80211,eeprom_93cx6,cfg80211";

MODULE_ALIAS("pci:v000010B7d00006000sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001200d00008201sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00008201sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00008211sv*sd*bc*sc*i*");
