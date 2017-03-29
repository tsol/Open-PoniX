cmd_kernel/rcu/built-in.o :=  ld -m elf_i386   -r -o kernel/rcu/built-in.o kernel/rcu/update.o kernel/rcu/srcu.o kernel/rcu/tree.o 
