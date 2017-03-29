cmd_sound/pci/snd-es1968.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-es1968.ko sound/pci/snd-es1968.o sound/pci/snd-es1968.mod.o
