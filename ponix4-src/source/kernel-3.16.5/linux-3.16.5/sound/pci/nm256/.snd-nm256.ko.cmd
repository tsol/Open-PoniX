cmd_sound/pci/nm256/snd-nm256.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/nm256/snd-nm256.ko sound/pci/nm256/snd-nm256.o sound/pci/nm256/snd-nm256.mod.o
