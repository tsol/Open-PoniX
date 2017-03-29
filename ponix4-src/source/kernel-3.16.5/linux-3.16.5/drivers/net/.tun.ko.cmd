cmd_drivers/net/tun.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/tun.ko drivers/net/tun.o drivers/net/tun.mod.o
