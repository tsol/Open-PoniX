cmd_drivers/ide/ide-cd_mod.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/ide/ide-cd_mod.ko drivers/ide/ide-cd_mod.o drivers/ide/ide-cd_mod.mod.o
