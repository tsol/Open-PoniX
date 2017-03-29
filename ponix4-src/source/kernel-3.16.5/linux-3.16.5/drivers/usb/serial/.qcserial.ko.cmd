cmd_drivers/usb/serial/qcserial.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/usb/serial/qcserial.ko drivers/usb/serial/qcserial.o drivers/usb/serial/qcserial.mod.o
