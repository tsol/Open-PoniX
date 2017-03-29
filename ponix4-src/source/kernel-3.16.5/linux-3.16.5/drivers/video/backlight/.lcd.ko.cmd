cmd_drivers/video/backlight/lcd.ko := ld -r -m elf_i386 -T ./scripts/module-common.lds --build-id  -o drivers/video/backlight/lcd.ko drivers/video/backlight/lcd.o drivers/video/backlight/lcd.mod.o
