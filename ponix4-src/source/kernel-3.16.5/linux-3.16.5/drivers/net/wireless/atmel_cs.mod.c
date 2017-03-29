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
"depends=pcmcia,atmel";

MODULE_ALIAS("pcmcia:m0101c0620f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0101c0696f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m01BFc3302f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mD601c0007f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa9EB2DA1FpbC9A0D3F9pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pb41B37E1Fpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pb3675D704pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pb4172E792pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pb917F3D72pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pb5040670Apc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paABDA4164pbE15ED87Fpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paAE49B86Apb1E957CD5pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paBC477DDEpb502FAE6Bpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa5B878724pb122F1DF6pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa5B878724pb5FBA533Apc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paFEA54C90pb1C5B0F68pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC4F8B18Bpb30F38774pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC4F8B18Bpb172D1377pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paA407ECDDpb119F6314pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa575C516CpbB1F6DBC4pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paB474D43Apb6B1FEC94pc*pd*");
