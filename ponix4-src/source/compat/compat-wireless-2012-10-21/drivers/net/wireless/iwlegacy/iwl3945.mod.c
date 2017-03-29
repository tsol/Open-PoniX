#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=iwlegacy,cfg80211,mac80211,compat";

MODULE_ALIAS("pci:v00008086d00004222sv*sd00001005bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00004222sv*sd00001034bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00004222sv*sd00001044bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00004227sv*sd00001014bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00004222sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00004227sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "18E5963687E6B4B462D379C");
