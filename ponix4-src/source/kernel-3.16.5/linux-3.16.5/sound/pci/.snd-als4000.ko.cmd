cmd_sound/pci/snd-als4000.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-als4000.ko sound/pci/snd-als4000.o sound/pci/snd-als4000.mod.o
