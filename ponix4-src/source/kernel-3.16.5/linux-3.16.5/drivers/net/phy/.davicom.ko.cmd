cmd_drivers/net/phy/davicom.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/davicom.ko drivers/net/phy/davicom.o drivers/net/phy/davicom.mod.o
