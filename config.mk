PREFIX = /usr/local

X11INC = ${PREFIX}/include
X11LIB = ${PREFIX}/lib

XINERAMALIBS = -lXinerama

INCS = -I ${X11INC}
LIBS = -L ${X11LIB} -lX11 ${XINERAMALIBS}

CFLAGS 	= ${INCS}
LDFLAGS = ${LIBS}

CC = cc
