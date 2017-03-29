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
"depends=soc_camera,soc_scale_crop,videobuf2-dma-contig,soc_mediabus,v4l2-common,videobuf2-core";

MODULE_ALIAS("platform:r8a7791-vin");
MODULE_ALIAS("platform:r8a7790-vin");
MODULE_ALIAS("platform:r8a7779-vin");
MODULE_ALIAS("platform:r8a7778-vin");
MODULE_ALIAS("platform:uPD35004-vin");
