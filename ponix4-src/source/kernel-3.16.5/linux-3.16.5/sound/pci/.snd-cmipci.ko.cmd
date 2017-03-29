cmd_sound/pci/snd-cmipci.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-cmipci.ko sound/pci/snd-cmipci.o sound/pci/snd-cmipci.mod.o
