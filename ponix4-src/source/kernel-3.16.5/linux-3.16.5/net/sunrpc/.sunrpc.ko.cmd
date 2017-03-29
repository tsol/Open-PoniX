cmd_net/sunrpc/sunrpc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o net/sunrpc/sunrpc.ko net/sunrpc/sunrpc.o net/sunrpc/sunrpc.mod.o
