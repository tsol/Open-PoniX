cmd_sound/isa/snd-es18xx.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-es18xx.ko sound/isa/snd-es18xx.o sound/isa/snd-es18xx.mod.o
