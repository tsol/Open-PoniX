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
"depends=pcmcia,compat,pcmcia_core,orinoco";

MODULE_ALIAS("pcmcia:m0101c0777f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m016Bc0001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m01EBc080Af*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0261c0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0268c0001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m0268c0003f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m026Fc0305f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m02AAc0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m02ACc0002f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m02ACc3021f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m14EAcB001f*fn*pfn*pa*pb*pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa41240E5Bpb56010AF3pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa5CD01705pb4271660Fpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa78FC06EEpb45A50C1Epc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa78FC06EEpbDB9AA842pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paD8A43B78pb0D341169pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa35DADC74pb01F7FEDBpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa32D445F5pbEDEFFD90pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paEF544D24pbCD8EA916pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa4507A33ApbEF54F0E3pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa56CC3F1Apb0BCF220Cpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa816CC815pb07F58077pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa7E3B326Apb49893E92pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa23EB9949pbC562E72Apc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa481E0094pb7360E410pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa481E0094pbF57CA4B3pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa24358CD4pbC562E72Apc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa2D617EA0pb88CD5767pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa4AC44287pb235A6BEDpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC6536A5Epb090C3CD9pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*paC6536A5Epb9F494E26pc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa43D74CB4pb579BD91Bpc*pd*");
MODULE_ALIAS("pcmcia:m*c*f*fn*pfn*pa3F02B4D6pb3663CB0Epc*pd*");
MODULE_ALIAS("pcmcia:m0156c0002f*fn*pfn*pa*pb*pcD27DEB1Apd*");
