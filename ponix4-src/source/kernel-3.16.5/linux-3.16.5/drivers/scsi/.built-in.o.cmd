cmd_drivers/scsi/built-in.o :=  ld -m elf_i386   -r -o drivers/scsi/built-in.o drivers/scsi/scsi_mod.o drivers/scsi/sd_mod.o drivers/scsi/sr_mod.o drivers/scsi/sg.o 
