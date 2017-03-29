cmd_drivers/media/usb/pwc/pwc.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/media/usb/pwc/pwc.ko drivers/media/usb/pwc/pwc.o drivers/media/usb/pwc/pwc.mod.o
