cmd_drivers/net/usb/catc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/catc.ko drivers/net/usb/catc.o drivers/net/usb/catc.mod.o
