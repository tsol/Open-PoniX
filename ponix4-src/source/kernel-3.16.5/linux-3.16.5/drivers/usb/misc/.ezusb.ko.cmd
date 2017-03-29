cmd_drivers/usb/misc/ezusb.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/misc/ezusb.ko drivers/usb/misc/ezusb.o drivers/usb/misc/ezusb.mod.o
