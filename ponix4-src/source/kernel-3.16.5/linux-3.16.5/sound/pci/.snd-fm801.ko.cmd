cmd_sound/pci/snd-fm801.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-fm801.ko sound/pci/snd-fm801.o sound/pci/snd-fm801.mod.o
