cmd_drivers/mmc/built-in.o :=  ld -m elf_i386   -r -o drivers/mmc/built-in.o drivers/mmc/core/built-in.o drivers/mmc/card/built-in.o drivers/mmc/host/built-in.o 
