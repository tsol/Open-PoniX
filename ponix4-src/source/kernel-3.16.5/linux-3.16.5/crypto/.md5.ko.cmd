cmd_crypto/md5.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/md5.ko crypto/md5.o crypto/md5.mod.o
