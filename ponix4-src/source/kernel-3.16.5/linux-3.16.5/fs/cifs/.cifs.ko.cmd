cmd_fs/cifs/cifs.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o fs/cifs/cifs.ko fs/cifs/cifs.o fs/cifs/cifs.mod.o
