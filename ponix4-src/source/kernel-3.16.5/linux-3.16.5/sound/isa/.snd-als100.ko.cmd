cmd_sound/isa/snd-als100.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-als100.ko sound/isa/snd-als100.o sound/isa/snd-als100.mod.o
