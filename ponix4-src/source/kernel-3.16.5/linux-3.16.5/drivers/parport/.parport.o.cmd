cmd_drivers/parport/parport.o := ld -m elf_i386   -r -o drivers/parport/parport.o drivers/parport/share.o drivers/parport/ieee1284.o drivers/parport/ieee1284_ops.o drivers/parport/procfs.o 
