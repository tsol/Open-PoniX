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
"depends=p54common,mac80211";

MODULE_ALIAS("usb:v0411p0050d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v045Ep00C2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0506p0A11d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0675p0530d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06B9p0120d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0707pEE06d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v07AAp001Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083Ap4501d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083Ap4502d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083Ap5501d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0846p4200d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0846p4210d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0846p4220d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v09AAp1000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0BF8p1007d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0CDEp0006d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0DB0p6826d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v107Bp55F2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v124Ap4023d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1435p0210d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v15A9p0002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1630p0005d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v182Dp096Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1915p2234d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1915p2235d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3701d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3703d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3762d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v5041p2234d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v5041p2235d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0471p1230d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v050Dp7050d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0572p2000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0572p2002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06A9p000Ed*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v06B9p0121d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0707pEE13d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v07AAp0020d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0803p4310d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083Ap4521d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083Ap4531d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083ApC501d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v083ApF503d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0846p4240d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0915p2000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0915p2002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0BAFp0118d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0BF8p1009d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0CDEp0008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0CDEp0015d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0D8Ep3762d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v124Ap4025d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1260pEE22d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v13B1p000Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v13B1p000Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1413p5400d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1435p0427d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1668p1050d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1740p1000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3704d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v2001p3705d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp5513d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8102d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8104d*dc*dsc*dp*ic*isc*ip*in*");
