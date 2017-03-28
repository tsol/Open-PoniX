#  Date Created:	1/7/2007
#  FileName:	Makefile
#  Description:	Makefile to compile the metro-usb.o driver.
#  Copyright:	2007 Metrologic Instruments. All rights reserved.
#  Requirements: 	Notepad.exe
 
#  Revision History:

#  Date:	Developer:		Revisions:		
#  ------------------------------------------------------------------------------
#  1/7/2007	Philip Nicastro		Initial release. (v1.0.0.0)


# If KERNELRELEASE is defined, we have been invoked from the kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)

	obj-m := metro-usb.o

# Called directly from the command line; invoke the kernel build system.
else

	KERNELDIR ?= /lib/modules/3.2.12-poniX-3.0/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

endif

clean:
	make -C /lib/modules/3.2.12-poniX-3.0/build M=$(PWD) clean
