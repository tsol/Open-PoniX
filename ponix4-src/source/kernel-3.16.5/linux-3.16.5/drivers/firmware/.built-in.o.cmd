cmd_drivers/firmware/built-in.o :=  ld -m elf_i386   -r -o drivers/firmware/built-in.o drivers/firmware/dmi_scan.o drivers/firmware/dmi-id.o drivers/firmware/memmap.o drivers/firmware/efi/built-in.o 
