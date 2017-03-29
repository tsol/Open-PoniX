cmd_fs/ramfs/ramfs.o := ld -m elf_i386   -r -o fs/ramfs/ramfs.o fs/ramfs/inode.o fs/ramfs/file-mmu.o 
