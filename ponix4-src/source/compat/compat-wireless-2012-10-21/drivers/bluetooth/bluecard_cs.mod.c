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
"depends=pcmcia,compat,bluetooth";

MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paBAF16FBFpb657CC15Epc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paE3987764pb2524B59Cpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa0A0736ECpb24E6DFABpc*pd*");
