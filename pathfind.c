/*
 * Copyright (c) 2018 Tristan Le Guern <tleguern@bouledef.eu>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "config.h"
#include <curses.h>
#include <err.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "level.h"
#include "ui.h"

struct coordqueue {
	size_t			 queuez;
	struct coordinate	*queue;
	int			*counter;
};

int coordqueue_size(struct coordqueue *);
void coordqueue_print(struct coordqueue *);
int coordqueue_exists(struct coordqueue *, int, int, int);
int coordqueue_get_counter_at_coord(struct coordqueue *, int, int);
int coordqueue_grow(struct coordqueue *);
int coordqueue_add(struct coordqueue *, int, int, int);
int coordqueue_init(struct coordqueue *);
void coordqueue_free(struct coordqueue *);

static void usage(void);

int
coordqueue_size(struct coordqueue *cq)
{
	return(cq->queuez);
}

void
coordqueue_print(struct coordqueue *cq)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		printf("%2zu: (%i, %i, %i)\n", i,
		    cq->queue[i].y,
		    cq->queue[i].x,
		    cq->counter[i]);
	}
}

int
coordqueue_exists(struct coordqueue *cq, int y, int x, int counter)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			continue;
		}
		if (y == cq->queue[i].y && x == cq->queue[i].x) {
			if (cq->counter[i] <= counter) {
				return(0);
			} else {
				return(-1);
			}
		}
	}
	return(-1);
}

int
coordqueue_get_counter_at_coord(struct coordqueue *cq, int y, int x)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			continue;
		}
		if (y == cq->queue[i].y && x == cq->queue[i].x) {
			return(cq->counter[i]);
		}
	}
	return(-1);
}

int
coordqueue_grow(struct coordqueue *cq)
{
	struct coordinate	*tempq;
	int			*tempcounter;
	size_t			 queuez;

	queuez = cq->queuez + 50;
	tempq = reallocarray(cq->queue, queuez, sizeof(struct coordinate));
	tempcounter = reallocarray(cq->counter, queuez, sizeof(int));
	if (NULL == tempq) {
		free(cq->queue);
		free(cq->counter);
		cq->queue = NULL;
		cq->counter = NULL;
		cq->queuez = -1;
		warn("reallocarray1");
		return(-1);
	}
	cq->queue = tempq;
	cq->counter = tempcounter;
	for (size_t i = cq->queuez; i < queuez; i++) {
		cq->queue[i].y = -1;
		cq->queue[i].x = -1;
		cq->counter[i] = -1;
	}
	cq->queuez = queuez;
	return(0);
}

int
coordqueue_add(struct coordqueue *cq, int y, int x, int counter)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			cq->queue[i].y = y;
			cq->queue[i].x = x;
			cq->counter[i] = counter;
			return(0);
		}
	}
	coordqueue_grow(cq);
	coordqueue_add(cq, y, x, counter);
	return(-1);
}

int
coordqueue_init(struct coordqueue *cq)
{
	cq->queuez = 50;
	cq->queue = reallocarray(NULL, cq->queuez, sizeof(struct coordinate));
	cq->counter = reallocarray(NULL, cq->queuez, sizeof(int));
	if (NULL == cq->queue) {
		warn("reallocarray");
		return(-1);
	}
	for (size_t i = 0; i < cq->queuez; i++) {
		cq->queue[i].y = -1;
		cq->queue[i].x = -1;
		cq->counter[i] = -1;
	}
	return(0);
}

void
coordqueue_free(struct coordqueue *cq)
{
	free(cq->queue);
	free(cq->counter);
	cq->queue = NULL;
	cq->counter = NULL;
	cq->queuez = -1;
}

static void
ui_draw2(struct level *l, struct coordqueue *cq)
{
	int counter;
	int tileset[T__MAX];

	tileset[T_EMPTY] = ' ';
	tileset[T_WALL] = '#';
	tileset[T_UPSTAIR] = '<';
	tileset[T_DOWNSTAIR] = '>';
	werase(stdscr);
	/* draw main screen */
	for (int y = 0; y < MAXROWS; ++y) {
		for (int x = 0; x < MAXCOLS; ++x) {
			counter = coordqueue_get_counter_at_coord(cq, y, x);
			if (-1 == counter) {
				mvaddch(y, x, tileset[l->tile[y][x].type]);
			} else {
				if (counter >= 10) {
					counter = counter % 10;
				}
				mvaddch(y, x, counter + 48);
			}
		}
	}
	wnoutrefresh(stdscr);
	doupdate();
}

int
main(int argc, char *argv[])
{
	struct level l;
	struct coordinate start, end;
	struct coordqueue cq;
	char *levelpath;
	int ch, found;

	while ((ch = getopt(argc, argv, "")) != -1) {
		switch (ch) {
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		warnx("level path expected");
		usage();
	}
	levelpath = argv[0];

	ui_init();
	level_init(&l);
	level_load(&l, levelpath);
	if (L_STATIC != l.type) {
		warnx("only entirely static levels are allowed");
		ui_cleanup();
		return(1);
	}
	level_find(&l, T_UPSTAIR, &start);
	level_find(&l, T_DOWNSTAIR, &end);

	coordqueue_init(&cq);
	coordqueue_add(&cq, start.y, start.x, 0);
	for (int elem = 0; elem < coordqueue_size(&cq); elem++) {
		int y, x, counter;

		y = cq.queue[elem].y;
		x = cq.queue[elem].x;
		if (-1 == y || -1 == x) {
			continue;
		}
		counter = cq.counter[elem];
		ui_draw2(&l, &cq);
		counter += 1;
		if (0 != y
		    && tile_is_empty(&(l.tile[y - 1][x]))
		    && -1 == coordqueue_exists(&cq, y - 1, x, counter)) {
			coordqueue_add(&cq, y - 1, x, counter);
		}
		if (0 != y && MAXCOLS != x
		    && tile_is_empty(&(l.tile[y - 1][x + 1]))
		    && -1 == coordqueue_exists(&cq, y - 1, x + 1, counter)) {
			coordqueue_add(&cq, y - 1, x + 1, counter);
		}
		if (MAXCOLS != x
		    && tile_is_empty(&(l.tile[y][x + 1]))
		    && -1 == coordqueue_exists(&cq, y, x + 1, counter)) {
			coordqueue_add(&cq, y, x + 1, counter);
		}
		if (MAXROWS != y && MAXCOLS != x
		    && tile_is_empty(&(l.tile[y + 1][x + 1]))
		    && -1 == coordqueue_exists(&cq, y + 1, x + 1, counter)) {
			coordqueue_add(&cq, y + 1, x + 1, counter);
		}
		if (MAXROWS != y
		    && tile_is_empty(&(l.tile[y + 1][x]))
		    && -1 == coordqueue_exists(&cq, y + 1, x, counter)) {
			coordqueue_add(&cq, y + 1, x, counter);
		}
		if (MAXROWS != y && 0 != x
		    && tile_is_empty(&(l.tile[y + 1][x - 1]))
		    && -1 == coordqueue_exists(&cq, y + 1, x - 1, counter)) {
			coordqueue_add(&cq, y + 1, x - 1, counter);
		}
		if (0 != x
		    && tile_is_empty(&(l.tile[y][x - 1]))
		    && -1 == coordqueue_exists(&cq, y, x - 1, counter)) {
			coordqueue_add(&cq, y, x - 1, counter);
		}
		if (0 != y && 0 != x
		    && tile_is_empty(&(l.tile[y - 1][x - 1]))
		    && -1 == coordqueue_exists(&cq, y - 1, x - 1, counter)) {
			coordqueue_add(&cq, y - 1, x - 1, counter);
		}
		ui_pause(0, 100);
	}
	found = -1;
	for (int elem = cq.queuez; elem >= 0; elem--) {
		int y, x;

		y = cq.queue[elem].y;
		x = cq.queue[elem].x;
		if (-1 == cq.queue[elem].y) {
			continue;
		}
		if (y == end.y && x == end.x) {
			found = elem;
			break;
		}
	}
	if (-1 == found) {
		ui_alert("This level is unwinnable");
	}
	ui_draw2(&l, &cq);
	coordqueue_free(&cq);
	ui_pause(3, 0);
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s file\n", getprogname());
	exit(1);
}

