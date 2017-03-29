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
"depends=rsi_91x";

MODULE_ALIAS("sdio:c*v0303d0100*");
MODULE_ALIAS("sdio:c*v041Bd0301*");
MODULE_ALIAS("sdio:c*v041Bd0201*");
MODULE_ALIAS("sdio:c*v041Bd9330*");

MODULE_INFO(srcversion, "C289A39FF855769A7B819EA");
