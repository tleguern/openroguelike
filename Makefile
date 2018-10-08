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

game.c: creature.h level.h ui.h
ui.c: ui.h
cave.c: level.h
level.c: creature.h level.h
creature.c: creature.h level.h ui.h

clean:
	rm -f -- ${PROG} ${OBJS}
