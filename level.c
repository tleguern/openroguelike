/*
 * Copyright (c) 2013,2018 Tristan Le Guern <tleguern@bouledef.eu>
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

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>

#include "config.h"
#include "creature.h"
#include "level.h"
#include "pathfind.h"
#include "rng.h"
#include "ui.h"

enum operand {
	OP_NAME,
	OP_TYPE,
	OP_SIZE,
	OP_POSITION,
	OP_MAP,
	OP__MAX,
};

static const char *operandmaps[] = {
	"name",
	"type",
	"size",
	"position",
	"map",
};

/*
 * Fill a struct coordinate from a string with the specific format "%i %i"
 * The source string is modified due to strsep(3).
 */
static int
string_to_coordinate(char *src, struct coordinate *c)
{
	char		*x, *y;
	const char	*errstr;

	y = strsep(&src, " ");
	x = src;
	errstr = NULL;
	c->y = (int)strtonum(y, 0, MAXROWS, &errstr);
	if (NULL != errstr)
		return(-1);
	c->x = (int)strtonum(x, 0, MAXCOLS, &errstr);
	if (NULL != errstr)
		return(-1);
	return(0);
}

bool
tile_is_empty(struct tile *t) {
	if ((T_EMPTY == t->type || T_UPSTAIR == t->type
	    || T_DOWNSTAIR == t->type) && NULL == t->creature)
		return(true);
	return(false);
}

bool
tile_is_wall(struct tile *t) {
	if (T_WALL == t->type)
		return(true);
	return(false);
}

void
level_init(struct level *l) {
	l->type = L_NONE;
	l->visited = false;
	l->entrymessage = NULL;
	for (int y = 0; y < MAXROWS; y++) {
		for (int x = 0; x < MAXCOLS; x++) {
			l->tile[y][x].type = T_EMPTY;
			l->tile[y][x].creature = NULL;
		}
	}
}

void
level_load(struct level *l, const char *filename)
{
	struct coordinate	 size, position = {0, 0};
	size_t			 linez;
	ssize_t			 linelen;
	const char		*errstr;
	char			*line;
	FILE			*s;

	errstr = NULL;
	line = NULL;
	linez = 0;
	linelen = 0;
	size.x = -1;
	size.y = -1;
	if (NULL == (s = fopen(filename, "r"))) {
		errstr = strerror(errno);
		goto clean;
	}
	/*
	 * Read the key at the begining of each line, with a special
	 * treatment for map.
	 */
	while (-1 != (linelen = getline(&line, &linez, s))) {
		enum operand	 op;
		int		 y;
		char		*linep, *key, *value;

		line[linelen - 1] = '\0';
		linep = line;
		if (NULL == (key = strsep(&linep, ":"))) {
			errstr = "malformed line";
			goto closeclean;
		}
		for (op = 0; op < OP__MAX; op++) {
			if (strcmp(operandmaps[op], key) == 0) {
				break;
			}
		}
		if (OP__MAX == op) {
			errstr = "unknown operand";
			goto closeclean;
		} else if (OP_MAP != op) {
			value = linep;
			if (' ' != value[0]) {
				errstr = "malformed value";
				goto closeclean;
			}
			value++;
		}
		switch (op) {
		case OP_NAME:
			/* This name is more a label than a real attribute */
			continue;
		case OP_TYPE:
			if (strcmp("cave", value) == 0) {
				l->type = L_CAVE;
			} else if (strcmp("static", value) == 0) {
				l->type = L_STATIC;
			} else {
				errstr = "unknown type";
				goto closeclean;
			}
			break;
		case OP_SIZE:
			if (-1 == string_to_coordinate(value, &size)) {
				errstr = "invalid coordinate for \"size\"";
				goto closeclean;
			}
			break;
		case OP_POSITION:
			if (-1 == string_to_coordinate(value, &position)) {
				errstr = "invalid coordinate for \"position\"";
				goto closeclean;
			}
			break;
		case OP_MAP:
			if (-1 == size.x || -1 == size.y) {
				errstr = "\"size\" should be defined"
				    " before \"map\"";
				goto closeclean;
			}
			y = position.y;
			linez = 0;
			linelen = 0;
			line = NULL;
			while (-1 != (linelen = getline(&line, &linez, s))) {
				if (linelen - 1 != size.x) {
					errstr = "bad value for size.x";
					goto closeclean;
				}
				line[linelen - 1] = '\0';
				for (int x = 0; x < size.x; x++) {
					int lx;

					lx = x + position.x;
					if ('#' == line[x]) {
						l->tile[y][lx].type = T_WALL;
					} else if ('<' == line[x]) {
						l->tile[y][lx].type = T_UPSTAIR;
					} else if ('>' == line[x]) {
						l->tile[y][lx].type = T_DOWNSTAIR;
					} else if (' ' == line[x]) {
						l->tile[y][lx].type = T_EMPTY;
					}
				}
				y += 1;
			}
			break;
		case OP__MAX:
		default:
			errstr = "unknown operand";
			goto closeclean;
		}
	}
	fclose(s);
	free(line);
	line = NULL;
	return;
closeclean:
	fclose(s);
	free(line);
	line = NULL;
clean:
	ui_cleanup();
	fprintf(stderr, "%s: %s\n", filename, errstr);
	exit(EXIT_FAILURE);
}

void
coordinate_init(struct coordinate *c)
{
	c->y = -1;
	c->x = -1;
}

void
coordinate_copy(struct coordinate *dest, struct coordinate *src)
{
	dest->y = src->y;
	dest->x = src->x;
}

void
level_add_stairs(struct level *l, bool build_upstair, bool build_downstair)
{
	struct coordinate upstair, downstair;
	struct coordinate existing_upstair, existing_downstair;

	coordinate_init(&existing_upstair);
	coordinate_init(&existing_downstair);
	/*
	 * Look if stairs are already present on the map, which could happen
	 * with level_load().
	*/
	for (int y = 1; y < MAXROWS; y++) {
		for (int x = 1; x < MAXCOLS; x++) {
			if (l->tile[y][x].type == T_UPSTAIR) {
				existing_upstair.y = y;
				existing_upstair.x = x;
			}
			if (l->tile[y][x].type == T_DOWNSTAIR) {
				existing_downstair.y = y;
				existing_downstair.x = x;
			}
		}
	}
	coordinate_copy(&upstair, &existing_upstair);
	coordinate_copy(&downstair, &existing_downstair);
	do {
		if (-1 == existing_upstair.y)
			upstair.y = rng_rand_uniform(MAXROWS);
		if (-1 == existing_upstair.x)
			upstair.x = rng_rand_uniform(MAXCOLS);
		if (-1 == existing_downstair.y)
			downstair.y = rng_rand_uniform(MAXROWS);
		if (-1 == existing_downstair.x)
			downstair.x = rng_rand_uniform(MAXCOLS);
		/* Ensure stairs are not too close */
		if (abs((upstair.y + upstair.x) - (downstair.y + downstair.x)) < 50)
			continue;
		if (! tile_is_empty(&(l->tile[upstair.y][upstair.x])))
			continue;
		if (! tile_is_empty(&(l->tile[downstair.y][downstair.x])))
			continue;
		if (false == are_coordinate_reachable(l, &upstair, &downstair))
			continue;
		if (build_upstair)
			l->tile[upstair.y][upstair.x].type = T_UPSTAIR;
		if (build_downstair)
			l->tile[downstair.y][downstair.x].type = T_DOWNSTAIR;
		break;
	} while (1);
}

int
level_find(struct level *l, enum tile_type tile, struct coordinate *coord)
{
	for (int y = 0; y < MAXROWS; y++) {
		for (int x = 0; x < MAXCOLS; x++) {
			if (tile == l->tile[y][x].type) {
				coord->y = y;
				coord->x = x;
				return(0);
			}
		}
	}
	coord->y = -1;
	coord->x = -1;
	return(-1);
}
