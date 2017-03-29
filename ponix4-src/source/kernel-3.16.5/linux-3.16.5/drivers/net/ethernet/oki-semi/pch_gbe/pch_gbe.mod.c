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
"depends=mii,ptp_pch";

MODULE_ALIAS("pci:v00008086d00008802sv00001CC8sd00000001bc02sc00i*");
MODULE_ALIAS("pci:v00008086d00008802sv*sd*bc02sc00i*");
MODULE_ALIAS("pci:v000010DBd00008013sv*sd*bc02sc00i*");
MODULE_ALIAS("pci:v000010DBd00008802sv*sd*bc02sc00i*");

MODULE_INFO(srcversion, "8EF1EAE8DEB7B3085312101");
