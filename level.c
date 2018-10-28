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
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ui.h"
#include "level.h"
#include "creature.h"
#include "rng.h"

static void level_add_stair(struct level *, bool);

struct coordinate {
	int x;
	int y;
};

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
	if (T_WALL == t->type
	    || T_HLINE == t->type
	    || T_VLINE == t->type
	    || T_BTEE == t->type
	    || T_TTEE == t->type
	    || T_LTEE == t->type
	    || T_RTEE == t->type
	    || T_CROSS == t->type
	    || T_LLCORNER == t->type
	    || T_LRCORNER == t->type
	    || T_ULCORNER == t->type
	    || T_URCORNER == t->type)
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

static void
level_add_stairs(struct level *l, bool upstair, bool downstair)
{
	int upy, upx, doy, dox;

	do {
		upy = rng_rand_uniform(MAXROWS);
		upx = rng_rand_uniform(MAXCOLS);
		doy = rng_rand_uniform(MAXROWS);
		dox = rng_rand_uniform(MAXCOLS);
		/* Ensure stairs are not too close */
		if (abs((upy + upx) - (doy + dox)) < 50)
			continue;
		if (! tile_is_empty(&(l->tile[upy][upx])))
			continue;
		if (! tile_is_empty(&(l->tile[doy][dox])))
			continue;
		if (upstair)
			l->tile[upy][upx].type = T_UPSTAIR;
		if (downstair)
			l->tile[doy][dox].type = T_DOWNSTAIR;
		break;
	} while (1);
}

void
world_init(struct world *w)
{
	w->current = 0;
	w->levelsz = 5;
	w->creaturesz = 3;
	w->levels = calloc(w->levelsz, sizeof(struct level *));
	/* The first level is the fixed entrance */
	w->levels[0] = calloc(1, sizeof(struct level));
	level_init(w->levels[0]);
	cave_gen(w->levels[0]);
	level_load(w->levels[0], "misc/entry");
	level_add_stairs(w->levels[0], false, true);
	w->levels[0]->entrymessage = strdup("You enter the Goblin's Caves");
	/* Generate three random caves */
	for (int32_t i = 1; i < w->levelsz - 1; i++) {
		w->levels[i] = calloc(1, sizeof(struct level));
		level_init(w->levels[i]);
		cave_gen(w->levels[i]);
		level_add_stairs(w->levels[i], true, true);
	}
	/* The final level is the fixed hall room of Goblin King */
	w->levels[w->levelsz - 1] = calloc(1, sizeof(struct level));
	level_init(w->levels[w->levelsz - 1]);
	cave_gen(w->levels[w->levelsz - 1]);
	w->levels[w->levelsz - 1]->entrymessage =
	    strdup("Unwelcome to the Hall of the Goblin King");
	level_load(w->levels[w->levelsz - 1], "misc/hall");
	level_add_stairs(w->levels[w->levelsz - 1], true, false);

	w->creatures = calloc(w->creaturesz, sizeof(struct creature *));
	for (int32_t i = 0; i < w->creaturesz; i++) {
		w->creatures[i] = calloc(1, sizeof(struct creature));
		creature_init(w->creatures[i], R_GOBLIN);
		creature_place_randomly(w->creatures[i], w->levels[0]);
	}
}

struct level *
world_first(struct world *w)
{
	return w->levels[0];
}

struct level *
world_next(struct world *w)
{
	if (w->current + 1 < w->levelsz)
		w->current += 1;
	return world_current(w);
}

struct level *
world_prev(struct world *w)
{
	if (w->current - 1 >= 0)
		w->current -= 1;
	return world_current(w);
}

struct level *
world_current(struct world *w)
{
	return w->levels[w->current];
}

void
world_free(struct world *w)
{
	for (int32_t i = 0; i < w->levelsz; i++) {
		free(w->levels[i]->entrymessage);
		free(w->levels[i]);
		w->levels[i] = NULL;
	}
	free(w->levels);
	w->levels = NULL;
	w->levelsz = 0;
	w->current = -1;
}

