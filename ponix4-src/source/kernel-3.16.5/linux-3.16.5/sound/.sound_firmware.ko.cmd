cmd_sound/sound_firmware.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/sound_firmware.ko sound/sound_firmware.o sound/sound_firmware.mod.o
