cmd_drivers/usb/serial/sierra.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/sierra.ko drivers/usb/serial/sierra.o drivers/usb/serial/sierra.mod.o
