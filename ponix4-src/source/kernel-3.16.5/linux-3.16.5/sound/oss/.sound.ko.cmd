cmd_sound/oss/sound.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/oss/sound.ko sound/oss/sound.o sound/oss/sound.mod.o
