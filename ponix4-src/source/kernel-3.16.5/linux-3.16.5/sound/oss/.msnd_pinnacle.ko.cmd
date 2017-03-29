cmd_sound/oss/msnd_pinnacle.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/oss/msnd_pinnacle.ko sound/oss/msnd_pinnacle.o sound/oss/msnd_pinnacle.mod.o
