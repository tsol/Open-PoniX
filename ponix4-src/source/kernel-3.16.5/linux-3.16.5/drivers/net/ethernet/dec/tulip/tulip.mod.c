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

MODULE_ALIAS("pci:v00001011d00000009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001011d00000019sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000011ADd00000002sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010D9d00000512sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010D9d00000531sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000125Bd00001400sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000011ADd0000C115sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00000981sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00000985sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00001985sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001317d00009511sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000013D1d0000AB02sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000013D1d0000AB03sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000013D1d0000AB08sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000104Ad00000981sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000104Ad00002774sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001259d0000A120sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000011F6d00009881sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00008086d00000039sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001113d00001216sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001113d00001217sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001113d00009511sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001186d00001541sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001186d00001561sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001186d00001591sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014F1d00001803sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001626d00008410sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001737d0000AB09sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001737d0000AB08sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000017B3d0000AB08sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010B7d00009300sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014EAd0000AB08sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001414d00000001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001414d00000002sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "75539DB825AE9DA507B1BB2");
