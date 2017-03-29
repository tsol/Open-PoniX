cmd_drivers/usb/serial/ssu100.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/ssu100.ko drivers/usb/serial/ssu100.o drivers/usb/serial/ssu100.mod.o
