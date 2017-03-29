cmd_drivers/net/ethernet/ec_bhf.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/ethernet/ec_bhf.ko drivers/net/ethernet/ec_bhf.o drivers/net/ethernet/ec_bhf.mod.o
