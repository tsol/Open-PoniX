cmd_drivers/uwb/whci.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/uwb/whci.ko drivers/uwb/whci.o drivers/uwb/whci.mod.o
