cmd_lib/cordic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o lib/cordic.ko lib/cordic.o lib/cordic.mod.o
