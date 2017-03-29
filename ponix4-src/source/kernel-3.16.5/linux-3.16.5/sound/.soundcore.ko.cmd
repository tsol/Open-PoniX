cmd_sound/soundcore.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/soundcore.ko sound/soundcore.o sound/soundcore.mod.o
