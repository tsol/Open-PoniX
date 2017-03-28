obj-m := elok_s.o 
elok_s-objs := main.o

KDIR := /lib/modules/3.16.5-PoniX-4/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules 

clean:
	rm -f *.o *.cmd *.mod.c .*.o* .*.ko* *~ *.ko Module.markers modules.order Module.symvers 
	rm -rf .tmp_versions/
