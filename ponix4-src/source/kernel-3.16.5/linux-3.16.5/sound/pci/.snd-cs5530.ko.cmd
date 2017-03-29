cmd_sound/pci/snd-cs5530.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-cs5530.ko sound/pci/snd-cs5530.o sound/pci/snd-cs5530.mod.o
