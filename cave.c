/*
 * Copyright (c) 2012,2014 Tristan Le Guern <tleguern@bouledef.eu>
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

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "level.h"

/* Cave specific options */
static int cave_step = 3; /* Number of calls to cave_reduce_noise */
static int cave_ratio = 40; /* Percentage of wall generated at initialization */

/* Cave functions */
static enum tile_type rand_pick(unsigned int);
static void cave_init(struct level *);
static void cave_reduce_noise(struct level *, struct level *);

/* Cave level generation with the cellular automata algorithm */
void
cave_gen(struct level *l) {
	int s;
	struct level tmp;

	cave_init(l);
	(void)memcpy(&tmp, l, sizeof(*l));
	for (s = 0; s < cave_step; ++s) {
		cave_reduce_noise(l, &tmp);
		(void)memcpy(l, &tmp, sizeof(tmp));
	}
}

static void
cave_reduce_noise(struct level *l, struct level *tmp) {
	int x, y;
	int ix, iy;

	for (y = 1; y < MAXROWS - 1; ++y)
		for (x = 1; x < MAXCOLS - 1; ++x) {
			unsigned int nwall = 0;
			for (iy = -1; iy <= 1; ++iy)
				for (ix = -1; ix <= 1; ++ix)
					if (l->tile[y + iy][x + ix].type \
					    == T_WALL)
						++nwall;
			if (nwall >= 5)
				tmp->tile[y][x].type = T_WALL;
			else
				tmp->tile[y][x].type = T_EMPTY;
		}
}

static void
cave_init(struct level *l) {
	int x, y;

	l->type = L_CAVE;
	/* randomly fill the map */
	for (y = 1; y < MAXROWS - 1; ++y)
		for (x = 1; x < MAXCOLS - 1; ++x)
			l->tile[y][x].type = rand_pick(cave_ratio);

	for (y = 0; y < MAXROWS; ++y) {
		l->tile[y][0].type = T_WALL;
		l->tile[y][MAXCOLS - 1].type = T_WALL;
	}

	for (x = 0; x < MAXCOLS; ++x) {
		l->tile[0][x].type = T_WALL;
		l->tile[MAXROWS - 1][x].type = T_WALL;
	}
}

static enum tile_type
rand_pick(unsigned int ratio) {
	unsigned int x = arc4random_uniform(100);
	return (x < ratio) ? T_WALL : T_EMPTY;
}

