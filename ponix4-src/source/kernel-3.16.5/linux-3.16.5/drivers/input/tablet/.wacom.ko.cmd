cmd_drivers/input/tablet/wacom.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/input/tablet/wacom.ko drivers/input/tablet/wacom.o drivers/input/tablet/wacom.mod.o
