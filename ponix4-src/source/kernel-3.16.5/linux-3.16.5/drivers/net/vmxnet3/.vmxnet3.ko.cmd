cmd_drivers/net/vmxnet3/vmxnet3.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/net/vmxnet3/vmxnet3.ko drivers/net/vmxnet3/vmxnet3.o drivers/net/vmxnet3/vmxnet3.mod.o
