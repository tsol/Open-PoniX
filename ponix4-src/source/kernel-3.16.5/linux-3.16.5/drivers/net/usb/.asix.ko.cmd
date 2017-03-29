cmd_drivers/net/usb/asix.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/asix.ko drivers/net/usb/asix.o drivers/net/usb/asix.mod.o
