PROG= roguelike
SRCS= game.c ui.c creature.c level.c cave.c
OBJS= ${SRCS:.c=.o}

LDADD+= -lcurses
CFLAGS+= -Wall -Wextra -Wno-unused-function

.SUFFIXES: .c .o
.PHONY: clean

.c.o:
	${CC} ${CFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS} ${LDADD}

game.c: ui.h level.h
ui.c: ui.h
cave.c: level.h
level.c: level.h
player.c: ui.h level.h player.h

clean:
	rm -f -- ${PROG} ${OBJS}
