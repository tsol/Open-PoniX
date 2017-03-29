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
"depends=rt2x00lib,rt2x00pci,compat,eeprom_93cx6";

MODULE_ALIAS("pci:v00001814d00000301sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001814d00000302sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001814d00000401sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "9D70DB2C5E2C37F07E9F7F8");
