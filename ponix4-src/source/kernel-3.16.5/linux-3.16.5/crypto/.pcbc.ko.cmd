cmd_crypto/pcbc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/pcbc.ko crypto/pcbc.o crypto/pcbc.mod.o
