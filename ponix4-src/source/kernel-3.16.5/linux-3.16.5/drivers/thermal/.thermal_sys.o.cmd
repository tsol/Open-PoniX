cmd_drivers/thermal/thermal_sys.o := ld -m elf_i386   -r -o drivers/thermal/thermal_sys.o drivers/thermal/thermal_core.o drivers/thermal/thermal_hwmon.o drivers/thermal/step_wise.o 
