cmd_drivers/usb/class/cdc-wdm.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/class/cdc-wdm.ko drivers/usb/class/cdc-wdm.o drivers/usb/class/cdc-wdm.mod.o
