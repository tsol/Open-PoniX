cmd_drivers/firmware/efi/built-in.o :=  ld -m elf_i386   -r -o drivers/firmware/efi/built-in.o drivers/firmware/efi/efi.o drivers/firmware/efi/vars.o drivers/firmware/efi/efivars.o 
