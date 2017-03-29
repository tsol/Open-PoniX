cmd_drivers/uwb/umc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/uwb/umc.ko drivers/uwb/umc.o drivers/uwb/umc.mod.o
