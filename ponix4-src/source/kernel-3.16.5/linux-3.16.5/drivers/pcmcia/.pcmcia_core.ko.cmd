cmd_drivers/pcmcia/pcmcia_core.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/pcmcia/pcmcia_core.ko drivers/pcmcia/pcmcia_core.o drivers/pcmcia/pcmcia_core.mod.o
