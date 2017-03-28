# Part 0
# load common stuff
TOPDIR = .
PLUGIN_CPU=ON
include $(TOPDIR)/Makefile.common

# Part 1
# recursive make
.PHONY: subdirs
all clean distclean install uninstall: subdirs

SUBDIRS = config man systray plugins
.PHONY: $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)



SRC = panel.c misc.c plugin.c gtkbar.c bg.c gtkbgbox.c ev.c configurator.c
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.dep)


ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),tar)
-include $(DEP)
endif
endif
endif

ifneq  (,$(STATIC))
CFLAGS += -DSTATIC_PLUGINS
endif

TARGET = fbpanel
$(TARGET): $(OBJ)
ifneq  (,$(STATIC))
	$(CC) $(LDFLAGS) $(LIBS) $(OBJ) plugins/*.o systray/*.o -o $@
else
	$(CC) $(LDFLAGS) $(LIBS) $(OBJ) -o $@
endif
ifeq (,$(DEVEL))
	strip $@
endif

all: $(TARGET)


clean:
	$(RM) $(TARGET) $(OBJ) $(DEP) *~

distclean: 
	rm -f Makefile.config config.h

install: 
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin

uninstall:
	rm -f $(PREFIX)/bin/$(TARGET)

.PHONY: tar


CWD=$(shell pwd)
VER=$(shell grep -e "\#define[[:space:]]\+VERSION[[:space:]]\+" version.h | \
		sed -e 's/^[^\"]\+\"//' -e 's/\".*$$//' )


tar: 
	$(MAKE) distclean
	cd ..; \
	if [ -e fbpanel-$(VER) ]; then \
		echo fbpanel-$(VER) already exist; \
		echo "won't override";\
		exit 1;\
	else\
		ln -s $(CWD) fbpanel-$(VER);\
		tar --exclude CVS --exclude .cvsignore -hzcvf fbpanel-$(VER).tgz fbpanel-$(VER);\
		rm -f fbpanel-$(VER);\
	fi;

