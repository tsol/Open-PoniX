cmd_drivers/crypto/geode-aes.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/crypto/geode-aes.ko drivers/crypto/geode-aes.o drivers/crypto/geode-aes.mod.o
