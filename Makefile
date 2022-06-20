include config.mk

SRC = tanuki.c
OBJ = ${SRC:.c=}

all: options tanuki

options:
	@echo tanuki build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${CFLAGS}"
	@echo "CC       = ${CC}"

tanuki:
	${CC} ${CFLAGS} ${LDFLAGS} tanuki.c -o tanuki
