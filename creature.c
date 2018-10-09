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

#include <stdlib.h>
#include <curses.h>

#include "level.h"
#include "ui.h"
#include "creature.h"
#include "rng.h"

static void human_init(struct creature *);
static void goblin_init(struct creature *);

void
creature_init(struct creature *c, struct level *l, enum race race)
{
	creature_place_randomly(c, l);
	c->race = race;
	switch (race) {
	case R_HUMAN:
		human_init(c);
		break;
	case R_GOBLIN:
		goblin_init(c);
		break;
	case R__MAX:
	default:
		return; /* XXX error in some way */
	}
}

void
creature_place_randomly(struct creature *c, struct level *l)
{
	int y, x;

	do {
		y = rng_rand_uniform(MAXROWS);
		x = rng_rand_uniform(MAXCOLS);
		if (tile_is_empty(&(l->tile[y][x]))) {
			c->x = x;
			c->y = y;
			l->tile[y][x].creature = c;
			break;
		}
	} while (1);
}

void
creature_place_at_stair(struct creature *c, struct level *l, bool up)
{
	int y, x;

	for (y = 0; y < MAXROWS; ++y)
		for (x = 0; x < MAXCOLS; ++x)
			if ((l->tile[y][x].type == T_UPSTAIR && up)
			    || (l->tile[y][x].type == T_DOWNSTAIR && !up)) {
				c->x = x;
				c->y = y;
				l->tile[y][x].creature = c;
				return;
			}
}

void
creature_move(struct creature *c, struct level *l, int row, int col)
{
	if (c->y + row < 0 || c->y + row >= LINES) {
		return;
	}
	if (c->x + col < 0 || c->x + col >= COLS) {
		return;
	}
	if (tile_is_wall(&(l->tile[c->y + row][c->x + col]))) {
		ui_message("You bumped into a wall");
		return;
	}
	if (tile_is_empty(&(l->tile[c->y + row][c->x + col])) == false) {
		ui_message("You bumped into something");
		return;
	}
	l->tile[c->y][c->x].creature = NULL;
	c->y += row;
	c->x += col;
	l->tile[c->y][c->x].creature = c;
}

void
creature_move_left(struct creature *c, struct level* l)
{
	creature_move(c, l, 0, -1);
}

void
creature_move_down(struct creature *c, struct level* l)
{
	creature_move(c, l, 1, 0);
}

void
creature_move_up(struct creature *c, struct level* l)
{
	creature_move(c, l, -1, 0);
}

void
creature_move_right(struct creature *c, struct level* l)
{
	creature_move(c, l, 0, 1);
}

void
creature_move_upleft(struct creature *c, struct level* l)
{
	creature_move(c, l, -1, -1);
}

void
creature_move_downleft(struct creature *c, struct level* l)
{
	creature_move(c, l, 1, -1);
}

void
creature_move_upright(struct creature *c, struct level* l)
{
	creature_move(c, l, -1, 1);
}

void
creature_move_downright(struct creature *c, struct level* l)
{
	creature_move(c, l, 1, 1);
}

static void
human_init(struct creature *c)
{
	c->glyphe = '@' | COLOR_PAIR(2);
}

static void
goblin_init(struct creature *c)
{
	c->glyphe = 'g' | COLOR_PAIR(5);
}

