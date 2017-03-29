cmd_fs/sysfs/built-in.o :=  ld -m elf_i386   -r -o fs/sysfs/built-in.o fs/sysfs/file.o fs/sysfs/dir.o fs/sysfs/symlink.o fs/sysfs/mount.o fs/sysfs/group.o 
