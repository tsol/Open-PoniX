SRC=main.c eggtraymanager.c fixedtip.c egg-marshal.c obox.c
all: $(SRC)
	gcc -Wall -g $(SRC) -o gtktray  `pkg-config --cflags gtk+-2.0` \
		`pkg-config --libs gtk+-2.0` -L/usr/X11R6/lib	-lXmu 
	strip gtktray

clean:
	rm -rf gtktray

