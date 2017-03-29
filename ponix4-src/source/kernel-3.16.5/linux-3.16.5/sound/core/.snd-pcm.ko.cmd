cmd_sound/core/snd-pcm.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/snd-pcm.ko sound/core/snd-pcm.o sound/core/snd-pcm.mod.o
