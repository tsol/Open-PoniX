#
# This Makefile was automatically generated; do not edit.
# Generated on 'swio-display-x86-rhel47-07.nvidia.com' on Wed Nov 16 19:27:30 PST 2011.
#

#
# Top level makefile for the NVIDIA kernel module; this file selects
# which "real" Makefile should actually be used to build the kernel
# module (either the "old" nvidia Makefile, or the kbuild makefile),
# and then passes the target on to that Makefile.
#

KERNEL_MODLIB := /lib/modules/3.2.12-poniX-3.0

ifdef SYSSRC
 KERNEL_SOURCES := $(SYSSRC)
 KERNEL_HEADERS := $(KERNEL_SOURCES)/include
else
 ifdef SYSINCLUDE
 KERNEL_HEADERS := $(SYSINCLUDE)
 KERNEL_SOURCES := $(KERNEL_HEADERS)/..
 else
 KERNEL_SOURCES := $(shell test -d $(KERNEL_MODLIB)/source && echo $(KERNEL_MODLIB)/source || echo $(KERNEL_MODLIB)/build)
 KERNEL_HEADERS := $(KERNEL_SOURCES)/include
 endif
endif

ifdef SYSOUT
 #
 # Support separate KBUILD output directories; SYSOUT
 # needs to reflect the correct KBUILD_OUTPUT.
 #
 KERNEL_OUTPUT := $(SYSOUT)
else
 ifeq ($(KERNEL_SOURCES), $(KERNEL_MODLIB)/source)
 KERNEL_OUTPUT := $(KERNEL_MODLIB)/build
 else
 KERNEL_OUTPUT := $(KERNEL_SOURCES)
 endif
endif

CC ?= cc
HOST_CC ?= $(CC)

ifndef ARCH
 ARCH := $(shell uname -m | sed -e 's/i.86/i386/')
endif

CONFTEST := sh ./conftest.sh "$(CC)" "$(HOST_CC)" $(ARCH) $(KERNEL_SOURCES) $(KERNEL_OUTPUT)

.PHONY: select_makefile
select_makefile:
	@ $(CONFTEST) cc_sanity_check full_output && \
	$(CONFTEST) select_makefile full_output

default: module

module nvidia.o nvidia.ko rmlite install nv-linux.o module-install package-install: select_makefile
	@ $(MAKE) --no-print-directory -f Makefile $@

# Commands associated with this target must not produce any output except
# for the printed module name -- nvidia-installer depends on this...
# So make this its own complete target to make it easier to guarantee this.
print-module-filename: 
	@ $(CONFTEST) select_makefile full_output && \
	$(MAKE) --no-print-directory -f Makefile $@

clean:
	@if [ -f Makefile ]; then \
	 $(MAKE) --no-print-directory -f Makefile $@; \
	fi
	@ rm -f Makefile
