cmd_drivers/i2c/i2c-smbus.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/i2c/i2c-smbus.ko drivers/i2c/i2c-smbus.o drivers/i2c/i2c-smbus.mod.o
