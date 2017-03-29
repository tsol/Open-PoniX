cmd_drivers/rtc/built-in.o :=  ld -m elf_i386   -r -o drivers/rtc/built-in.o drivers/rtc/rtc-lib.o drivers/rtc/hctosys.o drivers/rtc/systohc.o drivers/rtc/rtc-core.o drivers/rtc/rtc-cmos.o 
