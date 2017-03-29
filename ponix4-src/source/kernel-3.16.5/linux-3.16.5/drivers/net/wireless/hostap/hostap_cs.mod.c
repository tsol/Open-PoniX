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
"depends=pcmcia,hostap,lib80211";

MODULE_ALIAS("pcmcia:m000Bc7100f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m000Bc7300f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0101c0777f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0126c8000f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0138c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m01BFc3301f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0250c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m026Fc030Bf*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0274c1612f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0274c1613f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m028Ac0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m02AAc0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m02D2c0001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m50C2c0001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m50C2c7300f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mC250c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mD601c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mD601c0005f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mD601c0010f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0126c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:mD601c0005f*fn*pfn*pa2D858104pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0156c0002f*fn*pfn*pa74C5E40Dpb*pc*pd*");
MODULE_ALIAS("pcmcia:m0156c0002f*fn*pfn*pa4B801A17pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0156c0002f*fn*pfn*pa*pb*pc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn00pfn*pa7A954BD9pb74BE00C6pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paE6EC52CEpb08649AF2pc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa96EF6FE2pb263FCBABpcA57ADB8Cpd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa71B18589pbB6F1B0ABpc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa11D901AFpb6E9BD926pc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC4F8B18Bpb474A1F2Apc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa2DECECE3pb82067C18pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa54F7C49Cpb15A75E5Bpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa74C5E40DpbDB472A18pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa0733CC81pb0C52F395pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa273FE3DBpb32A1EAEEpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paA37434E9pb9762E8F1pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa1CADD3E5pbE697636Cpc7A5BFCF1pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC7B8DF9Dpb1700D087pc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa5CD01705pb4271660Fpc9D08EE12pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa4B8870FFpb70E946D1pc4B74BAA0pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa*pb*pc355CB092pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa*pb*pc630D52B2pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa*pb*pcDD97A26Bpd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa*pb*pcC9049A39pd*");
