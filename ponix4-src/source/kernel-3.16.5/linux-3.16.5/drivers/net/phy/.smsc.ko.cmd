cmd_drivers/net/phy/smsc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/smsc.ko drivers/net/phy/smsc.o drivers/net/phy/smsc.mod.o
