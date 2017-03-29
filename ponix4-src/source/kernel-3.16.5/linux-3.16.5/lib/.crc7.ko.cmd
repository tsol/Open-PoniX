cmd_lib/crc7.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o lib/crc7.ko lib/crc7.o lib/crc7.mod.o
