cmd_drivers/usb/serial/option.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/option.ko drivers/usb/serial/option.o drivers/usb/serial/option.mod.o
