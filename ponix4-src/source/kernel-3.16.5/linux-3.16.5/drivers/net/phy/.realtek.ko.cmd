cmd_drivers/net/phy/realtek.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/realtek.ko drivers/net/phy/realtek.o drivers/net/phy/realtek.mod.o
