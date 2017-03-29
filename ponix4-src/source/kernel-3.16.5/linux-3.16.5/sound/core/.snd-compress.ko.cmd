cmd_sound/core/snd-compress.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/snd-compress.ko sound/core/snd-compress.o sound/core/snd-compress.mod.o
