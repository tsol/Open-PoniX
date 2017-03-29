cmd_drivers/net/usb/net1080.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/net1080.ko drivers/net/usb/net1080.o drivers/net/usb/net1080.mod.o
