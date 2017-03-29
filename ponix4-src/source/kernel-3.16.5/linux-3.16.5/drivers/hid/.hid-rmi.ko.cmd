cmd_drivers/hid/hid-rmi.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/hid/hid-rmi.ko drivers/hid/hid-rmi.o drivers/hid/hid-rmi.mod.o
