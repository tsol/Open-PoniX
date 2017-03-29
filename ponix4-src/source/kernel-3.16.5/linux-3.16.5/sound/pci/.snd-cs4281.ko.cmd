cmd_sound/pci/snd-cs4281.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-cs4281.ko sound/pci/snd-cs4281.o sound/pci/snd-cs4281.mod.o
