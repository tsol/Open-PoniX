cmd_sound/core/snd-rawmidi.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/core/snd-rawmidi.ko sound/core/snd-rawmidi.o sound/core/snd-rawmidi.mod.o
