cmd_drivers/net/ppp/ppp_generic.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/ppp/ppp_generic.ko drivers/net/ppp/ppp_generic.o drivers/net/ppp/ppp_generic.mod.o
