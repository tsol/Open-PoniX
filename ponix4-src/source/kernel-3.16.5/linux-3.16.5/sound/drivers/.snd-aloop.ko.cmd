cmd_sound/drivers/snd-aloop.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/drivers/snd-aloop.ko sound/drivers/snd-aloop.o sound/drivers/snd-aloop.mod.o
