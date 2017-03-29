cmd_fs/nfs/nfs.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o fs/nfs/nfs.ko fs/nfs/nfs.o fs/nfs/nfs.mod.o
