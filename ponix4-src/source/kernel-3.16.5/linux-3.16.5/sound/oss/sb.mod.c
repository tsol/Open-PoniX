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
"depends=sb_lib";

MODULE_ALIAS("pnp:dCTL0031*");
MODULE_ALIAS("acpi*:CTL0031:*");
MODULE_ALIAS("pnp:dCTL0041*");
MODULE_ALIAS("acpi*:CTL0041:*");
MODULE_ALIAS("pnp:dCTL0001*");
MODULE_ALIAS("acpi*:CTL0001:*");
MODULE_ALIAS("pnp:dCTL0043*");
MODULE_ALIAS("acpi*:CTL0043:*");
MODULE_ALIAS("pnp:dCTL0042*");
MODULE_ALIAS("acpi*:CTL0042:*");
MODULE_ALIAS("pnp:dCTL0044*");
MODULE_ALIAS("acpi*:CTL0044:*");
MODULE_ALIAS("pnp:dCTL0045*");
MODULE_ALIAS("acpi*:CTL0045:*");
MODULE_ALIAS("pnp:dESS0968*");
MODULE_ALIAS("acpi*:ESS0968:*");
MODULE_ALIAS("pnp:dESS1868*");
MODULE_ALIAS("acpi*:ESS1868:*");
MODULE_ALIAS("pnp:dESS8611*");
MODULE_ALIAS("acpi*:ESS8611:*");
MODULE_ALIAS("pnp:dESS1869*");
MODULE_ALIAS("acpi*:ESS1869:*");
MODULE_ALIAS("pnp:dESS1878*");
MODULE_ALIAS("acpi*:ESS1878:*");
MODULE_ALIAS("pnp:dESS1879*");
MODULE_ALIAS("acpi*:ESS1879:*");
MODULE_ALIAS("pnp:d@X@0001*");
MODULE_ALIAS("acpi*:@X@0001:*");
MODULE_ALIAS("pnp:d@H@0001*");
MODULE_ALIAS("acpi*:@H@0001:*");
MODULE_ALIAS("pnp:d@@@0001*");
MODULE_ALIAS("acpi*:@@@0001:*");
MODULE_ALIAS("pnp:d@@@1001*");
MODULE_ALIAS("acpi*:@@@1001:*");
MODULE_ALIAS("pnp:d@X@1001*");
MODULE_ALIAS("acpi*:@X@1001:*");
MODULE_ALIAS("pnp:d@@@2001*");
MODULE_ALIAS("acpi*:@@@2001:*");
MODULE_ALIAS("pnp:d@X@2001*");
MODULE_ALIAS("acpi*:@X@2001:*");
MODULE_ALIAS("pnp:d@@@0020*");
MODULE_ALIAS("acpi*:@@@0020:*");
MODULE_ALIAS("pnp:d@X@0030*");
MODULE_ALIAS("acpi*:@X@0030:*");
MODULE_ALIAS("pnp:dPNPb003*");
MODULE_ALIAS("acpi*:PNPB003:*");
