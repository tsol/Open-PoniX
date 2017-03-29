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
"depends=ptp,libphy";

MODULE_ALIAS("pci:v000014E4d00001644sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001645sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001646sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001647sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001648sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000164Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001653sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001654sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000165Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000165Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A6sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A7sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A8sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016C6sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016C7sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001696sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000169Csv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000169Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000170Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000170Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001649sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000166Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001659sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000165Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001676sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001677sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000167Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000167Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001600sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001601sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016F7sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016FDsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016FEsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000167Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001672sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000167Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001673sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001674sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000169Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000169Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001693sv000017AAsd00003056bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001693sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000167Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001668sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001669sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001678sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001679sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000166Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000166Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016DDsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001712sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001713sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001698sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001684sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000165Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001681sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001680sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001688sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001689sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001699sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016A0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001692sv00001025sd00000601bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001692sv00001025sd00000612bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001692sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001690sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001694sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001691sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001655sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001665sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001656sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B1sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B5sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B0sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B4sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B2sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B6sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001657sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d0000165Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001682sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001686sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001687sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001643sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016F3sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001642sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001683sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d00001641sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B7sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000014E4d000016B3sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001148d00004400sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001148d00004500sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000173Bd000003E8sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000173Bd000003E9sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000173Bd000003EBsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000173Bd000003EAsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v0000106Bd00001645sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v000010CFd000011A2sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "A0752D487364FD33C627A98");
