cmd_drivers/net/phy/libphy.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/libphy.ko drivers/net/phy/libphy.o drivers/net/phy/libphy.mod.o
