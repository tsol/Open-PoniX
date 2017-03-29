cmd_crypto/cbc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/cbc.ko crypto/cbc.o crypto/cbc.mod.o
