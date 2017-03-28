# Part 0
# load common stuff
TOPDIR = .
PLUGIN_CPU=ON
include $(TOPDIR)/Makefile.common

# Part 1
# recursive make
RGOALS := $(filter-out tar, $(MAKECMDGOALS))
#$(warning recursive goals $(RGOALS))

.PHONY: subdirs
all clean distclean install uninstall: subdirs

SUBDIRS = config man systray plugins
.PHONY: $(SUBDIRS)
subdirs: $(SUBDIRS)
$(SUBDIRS):
	$(Q)$(MAKE) -C $@ $(RGOALS)



SRC = panel.c misc.c plugin.c gtkbar.c bg.c gtkbgbox.c ev.c configurator.c 


ifeq (1,$(BUILD))
-include $(DEP)
endif

TARGET := fbpanel

EXTRAOBJ :=
ifeq (1,$(STATIC_PLUGINS))
override CFLAGS += -DSTATIC_PLUGINS
EXTRAOBJ = plugins/plugins.a systray/tray.a
endif

all: $(TARGET)
$(TARGET): $(OBJ)
	$(call summary,LD  ,$@)
	$(Q)$(CC) $(LDFLAGS) $(OBJ) $(EXTRAOBJ) $(LIBS) -o $@
ifeq (0,$(DEVEL))
	$(Q)strip $@
endif





distclean: 
	$(RM) Makefile.config config.h

install: 
	install -d $(PREFIX)/bin
	install -m 755 $(TARGET) $(PREFIX)/bin

uninstall:
	$(RM) $(PREFIX)/bin/$(TARGET)

.PHONY: tar

ifeq ($(filter tar, $(MAKECMDGOALS)),tar)
CWD=$(shell pwd)
VER=$(shell grep -e "\#define[[:space:]]\+VERSION[[:space:]]\+" version.h | \
	sed -e 's/^[^\"]\+\"//' -e 's/\".*$$//' )
endif

tar: 
	$(MAKE) distclean
	cd ..; \
	if [ -e fbpanel-$(VER) ]; then \
		echo fbpanel-$(VER) already exist; \
		echo "won't override";\
		exit 1;\
	else\
		ln -s $(CWD) fbpanel-$(VER);\
		tar --exclude .svn --exclude CVS --exclude .cvsignore \
			-hzcvf fbpanel-$(VER).tgz fbpanel-$(VER);\
		rm -f fbpanel-$(VER);\
	fi;
	@ls -al ../fbpanel-$(VER).tgz;
