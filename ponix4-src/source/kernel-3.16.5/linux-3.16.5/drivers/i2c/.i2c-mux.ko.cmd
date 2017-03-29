cmd_drivers/i2c/i2c-mux.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/i2c/i2c-mux.ko drivers/i2c/i2c-mux.o drivers/i2c/i2c-mux.mod.o
