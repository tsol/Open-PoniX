cmd_sound/oss/msnd.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/oss/msnd.ko sound/oss/msnd.o sound/oss/msnd.mod.o
