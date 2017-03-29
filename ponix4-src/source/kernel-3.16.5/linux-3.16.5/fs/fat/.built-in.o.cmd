cmd_fs/fat/built-in.o :=  ld -m elf_i386   -r -o fs/fat/built-in.o fs/fat/fat.o fs/fat/vfat.o fs/fat/msdos.o 
