cmd_drivers/usb/host/hwa-hc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/host/hwa-hc.ko drivers/usb/host/hwa-hc.o drivers/usb/host/hwa-hc.mod.o
