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

#include <curses.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "level.h"
#include "pathfind.h"
#include "ui.h"

static void usage(void);

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

	/*
	 * Starting from (start.y, start.x) explore every adjacent cell and
	 * count the number of steps required to reach it.
	 * Do not insert a new position if the tested cell is out of
	 * the screen, is a wall or other unreachable type, or if a shorter path
	 * a already been found.
	 */
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
	/* Find the target destination in the coordqueue */
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
		ui_draw2(&l, &cq);
		coordqueue_free(&cq);
		ui_cleanup();
		return(-1);
	}
	/* Starting from the target destination find the shortest path back */
	do {
		int y, x, counter, smallest, elem;

		elem = found;
		y = cq.queue[elem].y;
		x = cq.queue[elem].x;
		counter = 0;
		smallest = INT16_MAX;
		for (int yaxes = -1; yaxes <= 1; yaxes++) {
			for (int xaxes = -1; xaxes <= 1; xaxes++) {
				if (0 == yaxes && 0 == xaxes) {
					continue;
				}
				counter = coordqueue_get_counter_at_coord(&cq,
				    y + yaxes, x + xaxes);
				if (-1 == counter || counter > smallest) {
					continue;
				}
				smallest = counter;
				found = coordqueue_get_elem_with_coord(&cq,
				    y + yaxes, x + xaxes, counter);
			}
		}
		cq.counter[found] = -1;
		ui_draw2(&l, &cq);
		ui_pause(0, 300);
		/* The first element in the coordqueue is the starting cell */
		if (0 == found) {
			break;
		}
	} while(true);
	ui_draw2(&l, &cq);
	coordqueue_free(&cq);
	(void)ui_get_input();
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s file\n", getprogname());
	exit(1);
}

