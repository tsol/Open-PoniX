CC=i486-TSOL-linux-gnu-gcc
CFLAGS=-Wall -g -O2
CPPFLAGS= -I/usr/xorg/include
LDFLAGS=-lcurl  -L/usr/xorg/lib  -lz -lpthread -lmagic
MAGIC=magic
MAGICFILES=$(wildcard $(MAGIC)/$(MAGIC).*)
prefix=/usr
exec_prefix=${prefix}
MAGICPATH=${prefix}/etc/binwalk/$(MAGIC)
ETC=$(DESTDIR)${prefix}/etc/binwalk
BIN=$(DESTDIR)${exec_prefix}/bin

.PHONY: all magi install uninstall clean cleanall distclean

all: binwalk

binwalk: dd.o common.o md5.o mparse.o filter.o update.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -DMAGIC='"$(MAGICPATH).binwalk"' -DMAGIC_CAST='"$(MAGICPATH).bincast"' -DMAGIC_ARCH='"$(MAGICPATH).binarch"' binwalk.c -o binwalk *.o $(LDFLAGS)

dd.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c dd.c

common.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c common.c

md5.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c md5.c

mparse.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c mparse.c

filter.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c filter.c

update.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c update.c

magi:
	cat $(MAGICFILES) > $(MAGIC).binwalk

install:
	mkdir -p $(ETC)
	mkdir -p $(BIN)
	cp $(MAGIC).bin* $(ETC)
	cp binwalk $(BIN)/binwalk

uninstall:
	rm -rf $(ETC)
	rm -f $(BIN)/binwalk

clean:
	rm -f binwalk *.o

cleanall: clean
	rm -rf config.* *.cache
	rm -f Makefile

distclean: cleanall

