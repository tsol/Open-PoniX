cmd_drivers/net/usb/cdc_subset.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/cdc_subset.ko drivers/net/usb/cdc_subset.o drivers/net/usb/cdc_subset.mod.o
