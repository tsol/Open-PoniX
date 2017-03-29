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

MODULE_ALIAS("pci:v00001425d0000A000sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004002sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004003sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004004sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004005sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004006sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004007sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004008sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000400Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004401sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004402sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004403sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004404sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004405sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004406sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004407sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004408sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00004409sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000440Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000440Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000440Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005001sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005002sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005003sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005004sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005005sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005006sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005007sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005008sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005009sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Csv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000500Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005010sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005011sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005012sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005013sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005014sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005015sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005080sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005081sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005082sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005083sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005084sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005085sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005401sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005402sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005403sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005404sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005405sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005406sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005407sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005408sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005409sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Asv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Bsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Csv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Dsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Esv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d0000540Fsv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005410sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005411sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005412sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005413sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005414sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005415sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005480sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005481sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005482sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005483sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005484sv*sd*bc*sc*i*");
MODULE_ALIAS("pci:v00001425d00005485sv*sd*bc*sc*i*");

MODULE_INFO(srcversion, "9121BDB29AE7EA3C43AA7F5");
