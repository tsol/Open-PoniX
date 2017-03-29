cmd_drivers/net/phy/icplus.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/icplus.ko drivers/net/phy/icplus.o drivers/net/phy/icplus.mod.o
