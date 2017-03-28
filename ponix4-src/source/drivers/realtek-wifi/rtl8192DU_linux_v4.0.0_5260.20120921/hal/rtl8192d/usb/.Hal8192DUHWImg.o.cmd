cmd_/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o := gcc -Wp,-MD,/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/.Hal8192DUHWImg.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-unused-function -Wno-unused -Wno-uninitialized -I/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/include -DCONFIG_POWER_SAVING -DCONFIG_WAKE_ON_WLAN -DCONFIG_80211D -DCONFIG_LITTLE_ENDIAN  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(Hal8192DUHWImg)"  -D"KBUILD_MODNAME=KBUILD_STR(8192du)" -c -o /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.c

source_/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o := /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.c

deps_/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o := \
  /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/include/Hal8192DUHWImg.h \
  /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/include/basic_types.h \
  /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/include/drv_conf.h \
    $(wildcard include/config/android.h) \
    $(wildcard include/config/platform/android.h) \
    $(wildcard include/config/validate/ssid.h) \
    $(wildcard include/config/signal/display/dbm.h) \
    $(wildcard include/config/has/earlysuspend.h) \
    $(wildcard include/config/resume/in/workqueue.h) \
    $(wildcard include/config/android/power.h) \
    $(wildcard include/config/wakelock.h) \
    $(wildcard include/config/usb/vendor/req/buffer/prealloc.h) \
    $(wildcard include/config/usb/vendor/req/mutex.h) \
    $(wildcard include/config/vendor/req/retry.h) \
  /source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/include/autoconf.h \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/rtl8192d.h) \
    $(wildcard include/config/ioctl/cfg80211.h) \
    $(wildcard include/config/cfg80211/force/compatible/2/6/37/under.h) \
    $(wildcard include/config/debug/cfg80211.h) \
    $(wildcard include/config/pwrctrl.h) \
    $(wildcard include/config/h2clbk.h) \
    $(wildcard include/config/embedded/fwimg.h) \
    $(wildcard include/config/file/fwimg.h) \
    $(wildcard include/config/wake/on/wlan.h) \
    $(wildcard include/config/wowlan.h) \
    $(wildcard include/config/r871x/test.h) \
    $(wildcard include/config/80211n/ht.h) \
    $(wildcard include/config/recv/reordering/ctrl.h) \
    $(wildcard include/config/tcp/csum/offload/rx.h) \
    $(wildcard include/config/drvext.h) \
    $(wildcard include/config/mp/included.h) \
    $(wildcard include/config/ips.h) \
    $(wildcard include/config/lps.h) \
    $(wildcard include/config/bt/coexist.h) \
    $(wildcard include/config/mp/iwpriv/support.h) \
    $(wildcard include/config/ap/mode.h) \
    $(wildcard include/config/nativeap/mlme.h) \
    $(wildcard include/config/hostapd/mlme.h) \
    $(wildcard include/config/find/best/channel.h) \
    $(wildcard include/config/no/wireless/handlers.h) \
    $(wildcard include/config/p2p.h) \
    $(wildcard include/config/wfd.h) \
    $(wildcard include/config/p2p/against/noise.h) \
    $(wildcard include/config/p2p/remove/group/info.h) \
    $(wildcard include/config/dbg/p2p.h) \
    $(wildcard include/config/tdls.h) \
    $(wildcard include/config/tdls/autosetup.h) \
    $(wildcard include/config/tdls/autocheckalive.h) \
    $(wildcard include/config/dualmac/concurrent.h) \
    $(wildcard include/config/skb/copy.h) \
    $(wildcard include/config/dfs.h) \
    $(wildcard include/config/led.h) \
    $(wildcard include/config/sw/led.h) \
    $(wildcard include/config/layer2/roaming.h) \
    $(wildcard include/config/layer2/roaming/resume.h) \
    $(wildcard include/config/set/scan/deny/timer.h) \
    $(wildcard include/config/new/signal/stat/process.h) \
    $(wildcard include/config/br/ext.h) \
    $(wildcard include/config/br/ext/brname.h) \
    $(wildcard include/config/tx/mcast2uni.h) \
    $(wildcard include/config/check/ac/lifetime.h) \
    $(wildcard include/config/disable/mcs13to15.h) \
    $(wildcard include/config/concurrent/mode.h) \
    $(wildcard include/config/tsf/reset/offload.h) \
    $(wildcard include/config/usb/interrupt/in/pipe.h) \
    $(wildcard include/config/minimal/memory/usage.h) \
    $(wildcard include/config/usb/tx/aggregation.h) \
    $(wildcard include/config/usb/rx/aggregation.h) \
    $(wildcard include/config/prealloc/recv/skb.h) \
    $(wildcard include/config/reduce/usb/tx/int.h) \
    $(wildcard include/config/easy/replacement.h) \
    $(wildcard include/config/use/usb/buffer/alloc/xx.h) \
    $(wildcard include/config/use/usb/buffer/alloc/tx.h) \
    $(wildcard include/config/use/usb/buffer/alloc/rx.h) \
    $(wildcard include/config/usb/vendor/req/buffer/dynamic/allocate.h) \
    $(wildcard include/config/usb/support/async/vdn/req.h) \
    $(wildcard include/config/only/one/out/ep/to/low.h) \
    $(wildcard include/config/out/ep/wifi/mode.h) \
    $(wildcard include/config/wistron/platform.h) \
    $(wildcard include/config/debug/rtl871x.h) \
    $(wildcard include/config/debug/rtl819x.h) \
    $(wildcard include/config/proc/debug.h) \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/types.h \
  include/asm-generic/types.h \
  include/asm-generic/int-ll64.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  /source/kernel-3.2.12/linux-3.2.12/arch/x86/include/asm/posix_types_32.h \

/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o: $(deps_/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o)

$(deps_/source/drivers/realtek-wifi/rtl8192DU_linux_v4.0.0_5260.20120921/hal/rtl8192d/usb/Hal8192DUHWImg.o):
