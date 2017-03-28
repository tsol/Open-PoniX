# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common

# backslashify slashes to avoid problems with sed
BPREFIX := $(subst /,\/,$(PREFIX))

CFG := $(wildcard *.cfg)
RES := $(CFG:%.cfg=%)

all: $(RES)

%: %.cfg
	sed 's/PREFIX/$(BPREFIX)/g' < $< > $@

clean:
	$(RM) $(RES) *~

install: 
	install -d  $(PREFIX)/share/fbpanel
	install -m 644 $(RES) $(PREFIX)/share/fbpanel
	install -d  $(PREFIX)/share/fbpanel/images
	install -m 644 images/*.png $(PREFIX)/share/fbpanel/images
	install -m 644 images/*.xpm $(PREFIX)/share/fbpanel/images
	
uninstall:
	rm -rf $(PREFIX)/share/fbpanel
