cmd_firmware/radeon/RV620_pfp.bin.gen.o := gcc -Wp,-MD,firmware/radeon/.RV620_pfp.bin.gen.o.d  -nostdinc -isystem /toolchain/i486-TSOL-linux-gnu/bin/../lib/gcc/i486-TSOL-linux-gnu/4.6.3/include -I./arch/x86/include -Iarch/x86/include/generated  -Iinclude -I./arch/x86/include/uapi -Iarch/x86/include/generated/uapi -I./include/uapi -Iinclude/generated/uapi -include ./include/linux/kconfig.h -D__KERNEL__  -D__ASSEMBLY__ -m32 -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1  -DCONFIG_AS_CRC32=1 -DCONFIG_AS_AVX=1           -c -o firmware/radeon/RV620_pfp.bin.gen.o firmware/radeon/RV620_pfp.bin.gen.S

source_firmware/radeon/RV620_pfp.bin.gen.o := firmware/radeon/RV620_pfp.bin.gen.S

deps_firmware/radeon/RV620_pfp.bin.gen.o := \

firmware/radeon/RV620_pfp.bin.gen.o: $(deps_firmware/radeon/RV620_pfp.bin.gen.o)

$(deps_firmware/radeon/RV620_pfp.bin.gen.o):
