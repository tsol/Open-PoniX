cmd_drivers/net/usb/kaweth.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/kaweth.ko drivers/net/usb/kaweth.o drivers/net/usb/kaweth.mod.o
