cmd_drivers/net/usb/zaurus.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/zaurus.ko drivers/net/usb/zaurus.o drivers/net/usb/zaurus.mod.o
