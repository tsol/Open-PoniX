cmd_drivers/net/phy/qsemi.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/phy/qsemi.ko drivers/net/phy/qsemi.o drivers/net/phy/qsemi.mod.o
