cmd_drivers/net/usb/kalmia.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/kalmia.ko drivers/net/usb/kalmia.o drivers/net/usb/kalmia.mod.o
