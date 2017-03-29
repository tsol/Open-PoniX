cmd_drivers/usb/serial/omninet.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/omninet.ko drivers/usb/serial/omninet.o drivers/usb/serial/omninet.mod.o
