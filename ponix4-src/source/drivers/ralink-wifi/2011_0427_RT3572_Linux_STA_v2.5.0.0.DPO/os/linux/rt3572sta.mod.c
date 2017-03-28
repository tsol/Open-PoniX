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
"depends=";

MODULE_ALIAS("usb:v148Fp2770d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp2870d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p2870d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07B8p2770d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0039d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p003Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap7512d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0162d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0163d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0789p0164d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v177Fp0302d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1731d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1732d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0B05p1742d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0017d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p002Dd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C06d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C28d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v2019pED06d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07D1p3C11d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C07d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp8053d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C23d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C27d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp002Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp003Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v07AAp003Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1044p800Bd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v15A9p0006d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApB522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083ApA618d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap8522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap7522d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CDEp0022d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0586p3416d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0CDEp0025d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9701d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9702d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0471p200Fd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C25d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v13D3p3247d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v083Ap6618d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v15C5p0008d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0E66p0003d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v129Bp1828d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v157Ep300Ed*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp805Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp815Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1482p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v14B2p3C09d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04E8p2018d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0740d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0280d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0282d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7718d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v7392p7717d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0070d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1737p0071d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0411p00E8d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v050Dp815Cd*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v100Dp9031d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DB0p6899d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v148Fp3572d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1740p9801d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0041d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0DF6p0042d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v04BBp0944d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0740d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v1690p0744d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v5A57p0284d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v167Bp4001d*dc*dsc*dp*ic*isc*ip*");
MODULE_ALIAS("usb:v0930p0A07d*dc*dsc*dp*ic*isc*ip*");

MODULE_INFO(srcversion, "D7F6D97B756AC2B0CFC20A9");
