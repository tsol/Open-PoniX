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
"depends=usb_wwan,usbserial";

MODULE_ALIAS("usb:v05C6p9211d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9212d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p1F1Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p201Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04DAp250Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v04DAp250Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8172d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8171d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA003d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA004d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA005d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA006d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA007d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B05p1776d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0B05p1774d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v19D2pFFF3d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v19D2pFFF2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1557p0A80d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9202d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9203d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9222d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9009d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9201d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9221d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9231d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1F45p0001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1BC7p900Ed*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA010d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA014d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8185d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8186d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9208d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p920Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9224d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9225d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9244d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9245d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p241Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p251Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9214d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9215d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9264d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9265d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9234d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9235d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9274d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9275d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9000d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9003d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9004d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9005d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9006d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9007d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9008d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9009d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p900Ad*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9011d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v16D8p8001d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v16D8p8002d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9204d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p9205d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v03F0p371Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p920Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v05C6p920Dd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA020d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1410pA021d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8193d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp8194d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A4d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A5d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A8d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A9d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9010d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9012d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9013d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9014d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9015d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9018d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9019d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p901Bd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v12D1p14F0d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v12D1p14F1d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0AF0p8120d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v0F3Dp68A2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v114Fp68A2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68A2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p68C0d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p901Cd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p901Fd*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9040d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9041d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9051d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9053d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9054d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9055d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9056d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9060d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v1199p9061d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp81A2d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp81A3d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp81A4d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp81A8d*dc*dsc*dp*ic*isc*ip*in*");
MODULE_ALIAS("usb:v413Cp81A9d*dc*dsc*dp*ic*isc*ip*in*");
