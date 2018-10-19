/*
 * Copyright (c) 2013,2014 Tristan Le Guern <tleguern@bouledef.eu>
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

#ifndef LEVEL_H__
#define LEVEL_H__

#define MAXROWS 22
#define MAXCOLS 80

enum tile_type {
	T_EMPTY,
	T_WALL,
	T_UPSTAIR,
	T_DOWNSTAIR,
	T_HLINE,
	T_VLINE,
	T_BTEE,
	T_TTEE,
	T_LTEE,
	T_RTEE,
	T_CROSS,
	T_LLCORNER,
	T_LRCORNER,
	T_ULCORNER,
	T_URCORNER,
	T__MAX,
};

struct tile {
	enum tile_type	 type;
	struct creature	*creature;
};

enum level_type {
	L_NONE,
	L_CAVE,
	L_STATIC,
	L__MAX,
};

struct level {
	enum level_type	type;
	struct tile	tile[MAXROWS][MAXCOLS];
};

struct world {
	int32_t		  levelsz;
	int32_t		  creaturesz;
	int32_t	  	  current;
	struct level	**levels;
	struct creature **creatures;
};

bool tile_is_empty(struct tile *);
bool tile_is_wall(struct tile *);
void tile_print(struct tile *, int, int);

void level_init(struct level *);
void level_load(struct level *, const char *);
void level_draw(struct level *);

void cave_gen(struct level *);

void world_init(struct world *);
void world_add(struct world *, struct level *);
void world_free(struct world *);
struct level *world_first(struct world *);
struct level *world_prev(struct world *);
struct level *world_next(struct world *);
struct level *world_current(struct world *);

#endif
