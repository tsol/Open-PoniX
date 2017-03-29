cmd_arch/x86/kernel/iosf_mbi.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o arch/x86/kernel/iosf_mbi.ko arch/x86/kernel/iosf_mbi.o arch/x86/kernel/iosf_mbi.mod.o
