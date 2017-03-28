cmd_firmware/radeon/R600_pfp.bin.gen.o := gcc -Wp,-MD,firmware/radeon/.R600_pfp.bin.gen.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I/source/kernel-3.2.12/linux-3.2.12/arch/x86/include -Iarch/x86/include/generated -Iinclude  -include /source/kernel-3.2.12/linux-3.2.12/include/linux/kconfig.h -D__KERNEL__ -D__ASSEMBLY__ -m32 -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1          -c -o firmware/radeon/R600_pfp.bin.gen.o firmware/radeon/R600_pfp.bin.gen.S

source_firmware/radeon/R600_pfp.bin.gen.o := firmware/radeon/R600_pfp.bin.gen.S

deps_firmware/radeon/R600_pfp.bin.gen.o := \

firmware/radeon/R600_pfp.bin.gen.o: $(deps_firmware/radeon/R600_pfp.bin.gen.o)

$(deps_firmware/radeon/R600_pfp.bin.gen.o):
