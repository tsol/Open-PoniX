cmd_crypto/ccm.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/ccm.ko crypto/ccm.o crypto/ccm.mod.o
