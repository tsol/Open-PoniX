cmd_sound/isa/snd-azt2320.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-azt2320.ko sound/isa/snd-azt2320.o sound/isa/snd-azt2320.mod.o
