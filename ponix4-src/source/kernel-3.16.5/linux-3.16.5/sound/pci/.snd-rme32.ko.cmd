cmd_sound/pci/snd-rme32.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-rme32.ko sound/pci/snd-rme32.o sound/pci/snd-rme32.mod.o
