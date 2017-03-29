cmd_drivers/net/usb/usbnet.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/usbnet.ko drivers/net/usb/usbnet.o drivers/net/usb/usbnet.mod.o
