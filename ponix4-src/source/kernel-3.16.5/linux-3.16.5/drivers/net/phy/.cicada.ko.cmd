cmd_drivers/net/phy/cicada.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/cicada.ko drivers/net/phy/cicada.o drivers/net/phy/cicada.mod.o
