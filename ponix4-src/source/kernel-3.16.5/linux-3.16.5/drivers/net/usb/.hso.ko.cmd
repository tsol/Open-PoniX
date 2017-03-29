cmd_drivers/net/usb/hso.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/hso.ko drivers/net/usb/hso.o drivers/net/usb/hso.mod.o
