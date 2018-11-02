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
	T_GOBLIN,
	T_HUMAN,
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
	enum level_type	 type;
	bool		 visited;
	char		*entrymessage;
	struct tile	 tile[MAXROWS][MAXCOLS];
};

struct coordinate {
	int x;
	int y;
};

bool tile_is_empty(struct tile *);
bool tile_is_wall(struct tile *);
void tile_print(struct tile *, int, int);

void level_init(struct level *);
void level_load(struct level *, const char *);
void level_draw(struct level *);
void level_add_stairs(struct level *, bool, bool);
int level_find(struct level *, enum tile_type, struct coordinate *);

void cave_gen(struct level *);

#endif
