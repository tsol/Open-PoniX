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

MODULE_ALIAS("pci:v000010ECd00008199sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010ECd00008185sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001799d0000700Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001799d0000701Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010ECd00008180sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001799d00006001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001799d00006020sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001186d00003300sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001186d00003301sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001432d00007106sv*sd*bc*sc*i*");
