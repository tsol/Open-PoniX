cmd_drivers/net/usb/sierra_net.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/sierra_net.ko drivers/net/usb/sierra_net.o drivers/net/usb/sierra_net.mod.o
