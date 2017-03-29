cmd_drivers/usb/serial/ipw.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/ipw.ko drivers/usb/serial/ipw.o drivers/usb/serial/ipw.mod.o
