cmd_sound/isa/snd-adlib.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/isa/snd-adlib.ko sound/isa/snd-adlib.o sound/isa/snd-adlib.mod.o
