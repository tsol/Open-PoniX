cmd_drivers/hid/hid.o := ld -m elf_i386   -r -o drivers/hid/hid.o drivers/hid/hid-core.o drivers/hid/hid-input.o drivers/hid/hidraw.o 
