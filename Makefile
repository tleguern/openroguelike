include Makefile.configure

PROG= roguelike
SRCS= game.c ui.c creature.c level.c cave.c rng.c options.c compats.c world.c pathfind.c
OBJS= ${SRCS:.c=.o}
DEPS= ${SRCS:.c=.d}

LDADD+= -lcurses
CFLAGS+= -std=c99 -Wall -Wextra -Wno-unused-function -O0 -g

.SUFFIXES: .c .o
.PHONY: clean

.c.o:
	${CC} -MMD -MF ${<:.c=.d} ${CFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS} ${LDADD}

PATHFINDDEMOOBJS= pathfind-demo.o ui.o level.o rng.o options.o compats.o pathfind.o
pathfind-demo: ${PATHFINDDEMOOBJS}
	${CC} ${LDFLAGS} -o $@ ${PATHFINDDEMOOBJS} ${LDADD}

LEVELVIEWOBJS= level-view.o ui.o level.o rng.o options.o compats.o pathfind.o cave.o
level-view: ${LEVELVIEWOBJS}
	${CC} ${LDFLAGS} -o $@ ${LEVELVIEWOBJS} ${LDADD}

clean:
	rm -f -- ${PROG} ${OBJS} ${DEPS} pathfind-demo

-include *.d
