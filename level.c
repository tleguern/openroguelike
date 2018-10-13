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

#include <stdbool.h>
#include <stdlib.h>
#include <curses.h>

#include "level.h"
#include "creature.h"
#include "rng.h"

static void level_add_stair(struct level *, bool);

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
tile_print(struct tile *t, int x, int y) {
	switch (t->type) {
	case T_EMPTY:
		mvaddch(y, x, ' ');
		break;
	case T_WALL:
		mvaddch(y, x, ACS_BLOCK);
		break;
	case T_UPSTAIR:
		mvaddch(y, x, '>');
		break;
	case T_DOWNSTAIR:
		mvaddch(y, x, '<');
		break;
	default:
		break;
	}
	if (NULL != t->creature) {
		mvaddch(y, x, t->creature->glyphe);
	}
}

void
level_draw(struct level *l)
{
	int x, y;

	for (y = 0; y < MAXROWS; ++y)
		for (x = 0; x < MAXCOLS; ++x)
			tile_print(&(l->tile[y][x]), x, y);
}

void
level_init(struct level *l) {
	l->type = L_NONE;
	for (int y = 0; y < MAXROWS; y++) {
		for (u_int x = 0; x < MAXCOLS; x++) {
			l->tile[y][x].type = T_EMPTY;
			l->tile[y][x].creature = NULL;
		}
	}
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

