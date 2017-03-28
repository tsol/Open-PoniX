CC = gcc
LIBS = `gdk-pixbuf-config --libs`  `gtk-config --libs` -lXpm -lXmu
INCS = `gdk-pixbuf-config --cflags` `gtk-config --cflags`

SRC = panel.c misc.c plugin.c separator.c image.c launchbar.c \
	dclock.c wincmd.c test.c taskbar.c gtkhtbar.c gtkvtbar.c \
	pager.c tray.c
OBJ = $(SRC:%.c=%.o)
DEP = $(SRC:%.c=%.dep)

CFLAGS += -Wall
ifneq (,$(DEVEL))
CFLAGS += -g
endif
ifeq (,$(DEVEL))
CFLAGS += -O2
endif

BIN = fbpanel
all: $(BIN)


ifneq ($(MAKECMDGOALS),clean)
-include $(DEP)
endif



$(BIN): $(OBJ) 
	$(CC) $(LDFLAGS) $(LIBS) $(OBJ) -o $@
ifeq (,$(DEVEL))
	strip $@
endif


%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $<

%.dep: %.c
	$(CC) $(CFLAGS) $(INCS) -MM $< -o $@



clean:
	$(RM) $(BIN) $(OBJ) $(DEP) *~
