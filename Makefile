include Makefile.configure

PROG= roguelike
SRCS= game.c ui.c creature.c level.c cave.c rng.c options.c compats.c world.c
OBJS= ${SRCS:.c=.o}
DEPS= ${SRCS:.c=.d}

LDADD+= -lcurses
CFLAGS+= -std=c99 -Wall -Wextra -Wno-unused-function

.SUFFIXES: .c .o
.PHONY: clean

.c.o:
	${CC} -MMD -MF ${<:.c=.d} ${CFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS} ${LDADD}

PATHFINDOBJ= pathfind.o ui.o level.o rng.o options.o compats.o
pathfind: ${PATHFINDOBJ}
	${CC} ${LDFLAGS} -o $@ ${PATHFINDOBJ} ${LDADD}

clean:
	rm -f -- ${PROG} ${OBJS} ${DEPS} pathfind pathfind.o

-include *.d
