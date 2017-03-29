cmd_sound/isa/snd-sscape.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-sscape.ko sound/isa/snd-sscape.o sound/isa/snd-sscape.mod.o
