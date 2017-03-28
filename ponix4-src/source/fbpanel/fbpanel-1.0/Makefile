
CC = gcc
LIBS = `gdk-pixbuf-config --libs`  `gtk-config --libs` -lXpm
INCS = `gdk-pixbuf-config --cflags` `gtk-config --cflags`
FLAGS = -O3 -Wall
SRC = fbpanel.c clip.c tb.c launchbar.c clock.c
OBJ = fbpanel.o clip.o tb.o launchbar.o clock.o
EXE = fbpanel

fbpanel: $(OBJ) 
	$(CC) $(FLAGS) $(LIBS) $(OBJ) -o $(EXE)
#	strip $(EXE)

.c.o:
	$(CC) $(FLAGS) $(INCS) -c $<

clean: 
	rm -f *.o $(EXE)
