# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common
#$(warning INCS=$(INCS))

INCS += -I../
SRC := egg-marshal.c   eggtraymanager.c  fixedtip.c main.c  
OBJ := $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.dep)
CFLAGS += -fPIC -static

TARGET = tray.so


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),tar)
-include $(DEP)
endif
endif
endif

all:$(TARGET)
$(TARGET): $(OBJ) 
	$(CC) $(CFLAGS) $(INCS) $(OBJ) -o $@
ifeq (,$(DEVEL))
	strip $@
endif


clean:
	$(RM)  *.dep *.o *.so  $(TARGET) *~


install: 
	install -d  $(PREFIX)/share/fbpanel/plugins
	install -m 644 $(TARGET) $(PREFIX)/share/fbpanel/plugins

uninstall:
	echo rm -rf $(PREFIX)/share/fbpanel/plugins/

