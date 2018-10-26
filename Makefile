include Makefile.configure

PROG= roguelike
SRCS= game.c ui.c creature.c level.c cave.c rng.c options.c compats.c
OBJS= ${SRCS:.c=.o}
DEPS= ${SRCS:.c=.d}

LDADD+= -lcurses
CFLAGS+= -Wall -Wextra -Wno-unused-function

.SUFFIXES: .c .o
.PHONY: clean

.c.o:
	${CC} -MMD -MF ${<:.c=.d} ${CFLAGS} -c $<

${PROG}: ${OBJS}
	${CC} ${LDFLAGS} -o $@ ${OBJS} ${LDADD}

clean:
	rm -f -- ${PROG} ${OBJS} ${DEPS}

-include *.d
