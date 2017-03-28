# Part 0
# load common stuff
TOPDIR = ..
include $(TOPDIR)/Makefile.common

# backslashify slashes to avoid problems with sed
BPREFIX := $(subst /,\/,$(PREFIX))

SRC = fbpanel.1
TARGET = fbpanel.1.gz

all: $(TARGET)
$(TARGET): $(SRC)
	sed 's/PREFIX/$(BPREFIX)/g' < $(SRC) | gzip - > $@



clean:
	$(RM) $(TARGET) *~


install: all
	install -d  $(PREFIX)/share/man/man1
	install -m 644 $(TARGET) $(PREFIX)/share/man/man1

uninstall:
	rm -f $(PREFIX)/share/man/man1/$(TARGET)
