cmd_drivers/usb/serial/pl2303.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/pl2303.ko drivers/usb/serial/pl2303.o drivers/usb/serial/pl2303.mod.o
