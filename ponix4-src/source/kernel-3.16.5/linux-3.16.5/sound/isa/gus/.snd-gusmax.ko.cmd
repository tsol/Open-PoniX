cmd_sound/isa/gus/snd-gusmax.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/gus/snd-gusmax.ko sound/isa/gus/snd-gusmax.o sound/isa/gus/snd-gusmax.mod.o
