# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common

# backslashify slashes to avoid problems with sed
BPREFIX := $(subst /,\/,$(PREFIX))

CFG := $(wildcard *.cfg)
TARGET := $(CFG:%.cfg=%)

all: $(TARGET)

%: %.cfg
	$(call summary,TEXT,$@)
	$(Q)sed 's/PREFIX/$(BPREFIX)/g' < $< > $@


install: 
	install -d  $(PREFIX)/share/fbpanel
	install -m 644 $(TARGET) $(PREFIX)/share/fbpanel
	install -d  $(PREFIX)/share/fbpanel/images
	install -m 644 images/*.svg $(PREFIX)/share/fbpanel/images
	install -m 644 images/*.xpm $(PREFIX)/share/fbpanel/images
	install -m 644 images/*.png $(PREFIX)/share/fbpanel/images

uninstall:
	$(RM) -r $(PREFIX)/share/fbpanel/images/
