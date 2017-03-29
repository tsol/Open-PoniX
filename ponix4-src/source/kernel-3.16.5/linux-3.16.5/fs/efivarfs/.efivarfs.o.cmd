cmd_fs/efivarfs/efivarfs.o := ld -m elf_i386   -r -o fs/efivarfs/efivarfs.o fs/efivarfs/inode.o fs/efivarfs/file.o fs/efivarfs/super.o 
