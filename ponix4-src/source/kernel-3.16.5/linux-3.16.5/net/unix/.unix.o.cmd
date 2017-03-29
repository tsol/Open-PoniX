cmd_net/unix/unix.o := ld -m elf_i386   -r -o net/unix/unix.o net/unix/af_unix.o net/unix/garbage.o net/unix/sysctl_net_unix.o 
