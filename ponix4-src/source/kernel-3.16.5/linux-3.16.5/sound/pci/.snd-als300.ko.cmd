cmd_sound/pci/snd-als300.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/snd-als300.ko sound/pci/snd-als300.o sound/pci/snd-als300.mod.o
