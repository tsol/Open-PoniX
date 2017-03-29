cmd_crypto/ctr.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/ctr.ko crypto/ctr.o crypto/ctr.mod.o
