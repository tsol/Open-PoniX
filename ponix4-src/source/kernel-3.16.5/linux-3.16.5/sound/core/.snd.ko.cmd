cmd_sound/core/snd.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/snd.ko sound/core/snd.o sound/core/snd.mod.o
