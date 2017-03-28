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
"depends=cfg80211";

MODULE_ALIAS("usb:v0BDAp8171d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8173d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8712d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8713d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDApC512d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p8188d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1786d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1791d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp945Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp0047d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2001p3306d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3306d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7611d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9603d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0016d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v06F8pE034d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v06F8pE032d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0167d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pAB28d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pED16d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0057d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0045d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0059d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p004Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0063d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v177Fp0154d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp5077d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0752d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v20F4p646Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApC512d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8172d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0EB0p9061d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8172d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3323d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3311d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3342d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3333d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3334d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3335d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3336d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3309d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp815Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3302d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3303d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7612d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9605d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v06F8pE031d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0015d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3306d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pED18d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019p4901d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0058d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0049d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p004Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0064d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3300d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3301d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3302d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF2d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF5d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04F2pAFF6d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3339d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3340d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3341d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3310d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3325d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8174d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0BDAp8174d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp845Ad*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp0051d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7622d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0409p02B6d*dc*dsc*dp*ic*isc*ip*");
