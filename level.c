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
#include <curses.h>

#include "level.h"
#include "creature.h"

bool
tile_is_empty(struct tile *t) {
	if (T_EMPTY == t->type && NULL == t->creature)
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

