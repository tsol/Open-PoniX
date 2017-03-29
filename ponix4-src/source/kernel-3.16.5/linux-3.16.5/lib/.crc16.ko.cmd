cmd_lib/crc16.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o lib/crc16.ko lib/crc16.o lib/crc16.mod.o
