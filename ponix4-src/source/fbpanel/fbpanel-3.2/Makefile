# Part 0
# load common stuff
TOPDIR = .
include $(TOPDIR)/Makefile.common

# Part 1
# recursive make
.PHONY: subdirs
all clean distclean install uninstall: subdirs

SUBDIRS = config man systray
.PHONY: $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)



SRC = panel.c misc.c plugin.c separator.c image.c launchbar.c \
	dclock.c wincmd.c test.c taskbar.c pager.c  \
	gtkhtbar.c gtkvtbar.c 
OBJ = $(SRC:%.c=%.o)

SYSTRAYOBJ = systray/systray.o
SYSTRAYOBJ: systray
#DEP = $(SRC:%.c=%.dep)



TARGET = fbpanel
$(TARGET): $(OBJ) $(SYSTRAYOBJ)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJ) $(SYSTRAYOBJ) -o $@
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


TMPVER=/tmp/.fbpanel.version
CWD=$(shell pwd)
VER=$(shell grep -e "\#define[[:space:]]\+VERSION[[:space:]]\+" panel.c | \
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
		tar --exclude CVS -hzcvf fbpanel-$(VER).tgz fbpanel-$(VER);\
		rm -f fbpanel-$(VER);\
	fi;

