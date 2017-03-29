cmd_crypto/sha256_generic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/sha256_generic.ko crypto/sha256_generic.o crypto/sha256_generic.mod.o
