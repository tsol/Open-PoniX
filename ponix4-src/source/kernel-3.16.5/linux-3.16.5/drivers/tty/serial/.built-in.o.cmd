cmd_drivers/tty/serial/built-in.o :=  ld -m elf_i386   -r -o drivers/tty/serial/built-in.o drivers/tty/serial/serial_core.o drivers/tty/serial/8250/built-in.o 
