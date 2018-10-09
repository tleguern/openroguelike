PROG= roguelike
SRCS= game.c ui.c creature.c level.c cave.c rng.c
OBJS= ${SRCS:.c=.o}

LDADD+= -lcurses
CFLAGS+= -Wall -Wextra -Wno-unused-function

.SUFFIXES: .c .o
.PHONY: clean

.c.o:
	${CC} ${CFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS} ${LDADD}

game.c: creature.h level.h ui.h rng.h
ui.c: ui.h
cave.c: level.h rng.h
level.c: creature.h level.h rng.h
creature.c: creature.h level.h ui.h rng.h
rng.c: rng.h

clean:
	rm -f -- ${PROG} ${OBJS}
