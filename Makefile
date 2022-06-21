include config.mk

SRC = tanuki.c
OBJ = ${SRC:.c=}

all: tanuki

options:
	@echo tanuki build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${CFLAGS}"
	@echo "CC       = ${CC}"

build:
	${CC} ${CFLAGS} ${LDFLAGS} tanuki.c -o tanuki
