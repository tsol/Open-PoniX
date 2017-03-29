cmd_drivers/uwb/hwa-rc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/uwb/hwa-rc.ko drivers/uwb/hwa-rc.o drivers/uwb/hwa-rc.mod.o
