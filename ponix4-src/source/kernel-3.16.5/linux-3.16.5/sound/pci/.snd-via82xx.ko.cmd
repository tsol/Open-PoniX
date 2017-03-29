cmd_sound/pci/snd-via82xx.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-via82xx.ko sound/pci/snd-via82xx.o sound/pci/snd-via82xx.mod.o
