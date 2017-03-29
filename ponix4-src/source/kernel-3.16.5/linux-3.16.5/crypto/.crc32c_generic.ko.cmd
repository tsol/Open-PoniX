cmd_crypto/crc32c_generic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/crc32c_generic.ko crypto/crc32c_generic.o crypto/crc32c_generic.mod.o
