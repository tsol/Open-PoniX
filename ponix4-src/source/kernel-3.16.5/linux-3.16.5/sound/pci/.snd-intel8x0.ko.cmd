cmd_sound/pci/snd-intel8x0.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-intel8x0.ko sound/pci/snd-intel8x0.o sound/pci/snd-intel8x0.mod.o
