cmd_drivers/ptp/ptp.o := ld -m elf_i386   -r -o drivers/ptp/ptp.o drivers/ptp/ptp_clock.o drivers/ptp/ptp_chardev.o drivers/ptp/ptp_sysfs.o 
