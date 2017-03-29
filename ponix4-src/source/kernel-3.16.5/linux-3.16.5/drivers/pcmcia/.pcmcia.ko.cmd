cmd_drivers/pcmcia/pcmcia.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/pcmcia/pcmcia.ko drivers/pcmcia/pcmcia.o drivers/pcmcia/pcmcia.mod.o
