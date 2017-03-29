cmd_crypto/md4.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/md4.ko crypto/md4.o crypto/md4.mod.o
