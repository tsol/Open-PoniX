cmd_drivers/usb/serial/usb_wwan.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/usb_wwan.ko drivers/usb/serial/usb_wwan.o drivers/usb/serial/usb_wwan.mod.o
