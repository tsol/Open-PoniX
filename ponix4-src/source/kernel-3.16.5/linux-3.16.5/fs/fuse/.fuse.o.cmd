cmd_fs/fuse/fuse.o := ld -m elf_i386   -r -o fs/fuse/fuse.o fs/fuse/dev.o fs/fuse/dir.o fs/fuse/file.o fs/fuse/inode.o fs/fuse/control.o 
