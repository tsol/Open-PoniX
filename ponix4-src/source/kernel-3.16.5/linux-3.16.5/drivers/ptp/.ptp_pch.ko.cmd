cmd_drivers/ptp/ptp_pch.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/ptp/ptp_pch.ko drivers/ptp/ptp_pch.o drivers/ptp/ptp_pch.mod.o
