cmd_sound/pci/mixart/snd-mixart.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o sound/pci/mixart/snd-mixart.ko sound/pci/mixart/snd-mixart.o sound/pci/mixart/snd-mixart.mod.o
