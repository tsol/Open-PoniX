cmd_drivers/net/ethernet/intel/i40e/i40e_debugfs.o := gcc -Wp,-MD,drivers/net/ethernet/intel/i40e/.i40e_debugfs.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I./arch/x86/include -Iarch/x86/include/generated  -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__ -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -m32 -msoft-float -mregparm=3 -freg-struct-return -mno-mmx -mno-sse -fno-pic -mpreferred-stack-boundary=2 -march=i486 -mtune=generic -Wa,-mtune=generic32 -ffreestanding -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1 -pipe -Wno-sign-compare -fno-asynchronous-unwind-tables -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -fno-delete-null-pointer-checks -Os -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-var-tracking-assignments -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Werror=implicit-int -Werror=strict-prototypes -DCC_HAVE_ASM_GOTO  -DMODULE  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(i40e_debugfs)"  -D"KBUILD_MODNAME=KBUILD_STR(i40e)" -c -o drivers/net/ethernet/intel/i40e/i40e_debugfs.o drivers/net/ethernet/intel/i40e/i40e_debugfs.c

source_drivers/net/ethernet/intel/i40e/i40e_debugfs.o := drivers/net/ethernet/intel/i40e/i40e_debugfs.c

deps_drivers/net/ethernet/intel/i40e/i40e_debugfs.o := \
    $(wildcard include/config/debug/fs.h) \
    $(wildcard include/config/i40e/dcb.h) \

drivers/net/ethernet/intel/i40e/i40e_debugfs.o: $(deps_drivers/net/ethernet/intel/i40e/i40e_debugfs.o)

$(deps_drivers/net/ethernet/intel/i40e/i40e_debugfs.o):
