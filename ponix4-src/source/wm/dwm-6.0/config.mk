# dwm version
VERSION = 6.0

# Customize below to fit your system

# paths
PREFIX = /usr
MANPREFIX = ${PREFIX}/share/man

X11INC = /usr/xorg/include
X11LIB = /usr/xorg/lib

# Xinerama
XINERAMALIBS = -L${X11LIB} -lXinerama
XINERAMAFLAGS = -DXINERAMA

# includes and libs
INCS = -I. -I/usr/include -I${X11INC}
LIBS = -L/usr/lib -lc -L${X11LIB} -lX11 ${XINERAMALIBS}

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" ${XINERAMAFLAGS}
#CFLAGS = -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
CFLAGS = -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#LDFLAGS = -g ${LIBS}
LDFLAGS = -s ${LIBS}

# Solaris
#CFLAGS = -fast ${INCS} -DVERSION=\"${VERSION}\"
#LDFLAGS = ${LIBS}

# compiler and linker
#CC = cc
