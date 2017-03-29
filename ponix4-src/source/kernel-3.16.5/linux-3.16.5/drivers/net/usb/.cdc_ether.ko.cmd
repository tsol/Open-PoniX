cmd_drivers/net/usb/cdc_ether.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/cdc_ether.ko drivers/net/usb/cdc_ether.o drivers/net/usb/cdc_ether.mod.o
