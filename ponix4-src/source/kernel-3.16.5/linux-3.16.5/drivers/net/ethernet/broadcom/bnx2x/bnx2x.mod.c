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
"depends=mdio,libcrc32c";

MODULE_ALIAS("pci:v000014E4d0000164Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000164Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001650sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001662sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001663sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000166Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000168Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A5sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A9sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000168Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016AEsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000168Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A1sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A2sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016AFsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016ABsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A4sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016ADsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000163Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000163Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000163Fsv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "C0DBFAA5E00A319F2E48B7F");
