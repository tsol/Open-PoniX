cmd_/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o := gcc -Wp,-MD,/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/.rtl8192c_mp.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -m32 -msoft-float -mregparm=3 -freg-struct-return -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -DCC_HAVE_ASM_GOTO -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-unused-function -Wno-unused -Wno-uninitialized -I/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/include -DCONFIG_POWER_SAVING -DCONFIG_LITTLE_ENDIAN  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(rtl8192c_mp)"  -D"KBUILD_MODNAME=KBUILD_STR(8192cu)" -c -o /source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o /source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.c

source_/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o := /source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.c

deps_/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o := \
    $(wildcard include/config/mp/included.h) \
    $(wildcard include/config/rtl8192c.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/rtl8192d.h) \
    $(wildcard include/config/mp/include.h) \

/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o: $(deps_/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o)

$(deps_/source/drivers/realtek-wifi/rtl8188C_8192C_usb_linux_v3.4.4_4749.20120730/hal/rtl8192c/rtl8192c_mp.o):
