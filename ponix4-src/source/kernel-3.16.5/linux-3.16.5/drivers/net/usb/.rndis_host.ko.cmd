cmd_drivers/net/usb/rndis_host.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/usb/rndis_host.ko drivers/net/usb/rndis_host.o drivers/net/usb/rndis_host.mod.o
