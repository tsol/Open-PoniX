cmd_drivers/usb/serial/visor.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/visor.ko drivers/usb/serial/visor.o drivers/usb/serial/visor.mod.o
