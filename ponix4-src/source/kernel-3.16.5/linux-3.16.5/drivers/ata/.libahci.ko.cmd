cmd_drivers/ata/libahci.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/ata/libahci.ko drivers/ata/libahci.o drivers/ata/libahci.mod.o
