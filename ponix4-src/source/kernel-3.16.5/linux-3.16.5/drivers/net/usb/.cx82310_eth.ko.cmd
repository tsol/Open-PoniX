cmd_drivers/net/usb/cx82310_eth.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/cx82310_eth.ko drivers/net/usb/cx82310_eth.o drivers/net/usb/cx82310_eth.mod.o
