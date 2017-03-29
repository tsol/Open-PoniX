cmd_sound/drivers/snd-dummy.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/drivers/snd-dummy.ko sound/drivers/snd-dummy.o sound/drivers/snd-dummy.mod.o
