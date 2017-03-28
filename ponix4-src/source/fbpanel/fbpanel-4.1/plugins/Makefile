# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common


INCS += -I../
SRC = image.c menu.c separator.c taskbar.c wincmd.c \
	dclock.c launchbar.c pager.c space.c test.c \
	icons.c deskno.c

ifeq ($(PLUGIN_CPU),on)
SRC += cpu.c
endif

SOBJ = $(SRC:%.c=%.so)
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.dep)
CFLAGS += -shared -fPIC



ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),tar)
-include $(DEP)
endif
endif
endif



all:$(SOBJ)

%.so: %.o
	$(CC) $(CFLAGS) $(INCS) $< -o $@
ifeq (,$(DEVEL))
	strip $@
endif


clean:
	$(RM)  *.dep *.so *.o  *~

install: 
	install -d  $(PREFIX)/share/fbpanel/plugins
	install -m 644 $(SOBJ) $(PREFIX)/share/fbpanel/plugins

uninstall:
	echo rm -rf $(PREFIX)/share/fbpanel/plugins/

