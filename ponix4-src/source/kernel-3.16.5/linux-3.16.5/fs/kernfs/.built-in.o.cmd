cmd_fs/kernfs/built-in.o :=  ld -m elf_i386   -r -o fs/kernfs/built-in.o fs/kernfs/mount.o fs/kernfs/inode.o fs/kernfs/dir.o fs/kernfs/file.o fs/kernfs/symlink.o 
