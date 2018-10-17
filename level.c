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

#include <sys/stat.h>

#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <curses.h>

#include "ui.h"
#include "level.h"
#include "creature.h"
#include "rng.h"

static void level_add_stair(struct level *, bool);

bool
tile_is_empty(struct tile const *t) {
	if ((T_EMPTY == t->type || T_UPSTAIR == t->type
	    || T_DOWNSTAIR == t->type) && NULL == t->creature)
		return(true);
	return(false);
}

bool
tile_is_wall(struct tile const *t) {
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
	for (int y = 0; y < MAXROWS; y++) {
		for (int x = 0; x < MAXCOLS; x++) {
			l->tile[y][x].type = T_EMPTY;
			l->tile[y][x].creature = NULL;
		}
	}
}

void
level_load(struct level *l, const char *path)
{
	struct stat	 stat;
	char		 line[81];
	const char	*err;
	FILE		*s;

	l->type = L_STATIC;
	if (NULL == (s = fopen(path, "r"))) {
		err = strerror(errno);
		goto clean;
	}
	if (-1 == fstat(fileno(s), &stat)) {
		err = strerror(errno);
		goto closeclean;
	}
	if (stat.st_size != 1782) {
		err = "file should be 1782 bytes long";
		goto closeclean;
	}
	for (int y = 0; y < MAXROWS; y++) {
		if (0 == fread(line, sizeof(char), sizeof(line), s))
			break;
		for (int x = 0; x < MAXCOLS; x++)
			if (line[x] != ' ')
				l->tile[y][x].type = T_WALL;
	}
	fclose(s);
	return;
closeclean:
	fclose(s);
clean:
	ui_cleanup();
	fprintf(stderr, "%s: %s\n", path, err);
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
	w->levelsz = 3;
	w->levels = calloc(w->levelsz, sizeof(struct level *));
	for (int32_t i = 0; i < w->levelsz; i++) {
		w->levels[i] = calloc(1, sizeof(struct level));
		level_init(w->levels[i]);
		cave_gen(w->levels[i]);
		if (0 == i)
			level_add_stairs(w->levels[i], true, false);
		else if (w->levelsz - 1 == i)
			level_add_stairs(w->levels[i], false, true);
		else
			level_add_stairs(w->levels[i], true, true);
		world_add(w, w->levels[i]);
	}
}

void
world_add(struct world *w, struct level *l)
{
	for (int32_t i = 0; i < w->levelsz; i++) {
		if (w->levels[i] == NULL) {
			w->levels[i] = l;
			break;
		}
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
		free(w->levels[i]);
		w->levels[i] = NULL;
	}
	free(w->levels);
	w->levels = NULL;
	w->levelsz = 0;
	w->current = -1;
}

