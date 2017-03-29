cmd_crypto/michael_mic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o crypto/michael_mic.ko crypto/michael_mic.o crypto/michael_mic.mod.o
