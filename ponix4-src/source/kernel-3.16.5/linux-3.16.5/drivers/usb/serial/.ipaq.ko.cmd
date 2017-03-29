cmd_drivers/usb/serial/ipaq.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/ipaq.ko drivers/usb/serial/ipaq.o drivers/usb/serial/ipaq.mod.o
