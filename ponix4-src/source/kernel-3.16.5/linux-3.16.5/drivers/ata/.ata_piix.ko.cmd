cmd_drivers/ata/ata_piix.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/ata/ata_piix.ko drivers/ata/ata_piix.o drivers/ata/ata_piix.mod.o
