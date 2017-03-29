cmd_drivers/staging/wlags49_h25/wl_profile.o := gcc -Wp,-MD,drivers/staging/wlags49_h25/.wl_profile.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I./arch/x86/include -Iarch/x86/include/generated  -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -m32 -msoft-float -mregparm=3 -freg-struct-return -mno-mmx -mno-sse -fno-pic -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fno-delete-null-pointer-checks -Os -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -DCC_HAVE_ASM_GOTO -I/include -Idrivers/staging/wlags49_h25 -DBUS_PCMCIA -DUSE_WEXT -DSTA_ONLY -DWVLAN_49 -DHERMES25  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(wl_profile)"  -D"KBUILD_MODNAME=KBUILD_STR(wlags49_h25_cs)" -c -o drivers/staging/wlags49_h25/wl_profile.o drivers/staging/wlags49_h25/wl_profile.c

source_drivers/staging/wlags49_h25/wl_profile.o := drivers/staging/wlags49_h25/wl_profile.c

deps_drivers/staging/wlags49_h25/wl_profile.o := \
  drivers/staging/wlags49_h25/../wlags49_h2/wl_profile.c \
    $(wildcard include/config/filename.h) \

drivers/staging/wlags49_h25/wl_profile.o: $(deps_drivers/staging/wlags49_h25/wl_profile.o)

$(deps_drivers/staging/wlags49_h25/wl_profile.o):
