cmd_sound/oss/msnd_classic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/oss/msnd_classic.ko sound/oss/msnd_classic.o sound/oss/msnd_classic.mod.o
