cmd_sound/isa/snd-sc6000.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-sc6000.ko sound/isa/snd-sc6000.o sound/isa/snd-sc6000.mod.o
