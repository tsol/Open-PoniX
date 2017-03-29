cmd_block/partitions/built-in.o :=  ld -m elf_i386   -r -o block/partitions/built-in.o block/partitions/check.o block/partitions/msdos.o block/partitions/efi.o 
