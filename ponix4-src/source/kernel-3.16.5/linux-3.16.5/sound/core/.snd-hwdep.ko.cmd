cmd_sound/core/snd-hwdep.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/snd-hwdep.ko sound/core/snd-hwdep.o sound/core/snd-hwdep.mod.o
