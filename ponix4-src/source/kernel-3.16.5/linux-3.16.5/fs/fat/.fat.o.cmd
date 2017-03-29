cmd_fs/fat/fat.o := ld -m elf_i386   -r -o fs/fat/fat.o fs/fat/cache.o fs/fat/dir.o fs/fat/fatent.o fs/fat/file.o fs/fat/inode.o fs/fat/misc.o fs/fat/nfs.o 
