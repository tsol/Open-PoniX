cmd_sound/pci/oxygen/snd-oxygen.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/oxygen/snd-oxygen.ko sound/pci/oxygen/snd-oxygen.o sound/pci/oxygen/snd-oxygen.mod.o
