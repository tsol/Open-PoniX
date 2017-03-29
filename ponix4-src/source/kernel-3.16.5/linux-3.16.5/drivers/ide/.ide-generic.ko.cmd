cmd_drivers/ide/ide-generic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/ide/ide-generic.ko drivers/ide/ide-generic.o drivers/ide/ide-generic.mod.o
