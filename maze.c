/*
 * Copyright (c) 2025 Maciej Matiaszowski <maciej.matiaszowski@gmail.com>
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

#include "level.h"
#include "rng.h"
#include "ui.h"

#define NORTH	0
#define SOUTH	1
#define WEST	2
#define EAST 	3

#define DIRECTIONS	4

int random_odd(int);
static int valid_position(int, int);
int dig_maze(struct level *, int, int);
void shuffle_directions(void);
int dig_maze_in_direction(struct level *, int);
void generate_maze(struct level *);
static int get_tile(struct level *, int, int);
static void set_tile(struct level *, int, int);

int directions[] = { NORTH, SOUTH, WEST, EAST };
int row, col;

int random_odd(int max) {
	int number = rng_rand_uniform(max / 2 - 1);
	number *= 2;
	return 1 + number;
}

static int valid_position(int position, int max) {
	return position >= 1 && position < max;
}

static int get_tile(struct level *l, int y, int x) {
	return l->tile[y][x].type == T_WALL;
}

static void set_tile(struct level *l, int y, int x) {
	l->tile[y][x].type = T_WALL;
}

int dig_maze(struct level *l, int direction_row, int direction_col) {
	int new_row = row + direction_row * 2;
	int new_col = col + direction_col * 2;
	int can_dig = valid_position(new_row, MAXROWS)
			&& valid_position(new_col, MAXCOLS)
			&& !get_tile(l, new_row, new_col);

	if (can_dig) {
		set_tile(l, new_row, new_col);
		set_tile(l, row + direction_row, col + direction_col);
		row = new_row;
		col = new_col;
	}

	return can_dig;
}


void shuffle_directions(void) {
	int i;

	for (i = DIRECTIONS - 1; i > 0; i--) {
		int j = rng_rand_uniform(i);
		int tmp = directions[i];

		directions[i] = directions[j];
		directions[j] = tmp;
	}

}


int dig_maze_in_direction(struct level *l, int direction) {
	int can_dig;

	switch (direction) {
		case NORTH:
			can_dig = dig_maze(l, -1, 0);
			break;
		case SOUTH:
			can_dig = dig_maze(l, 1, 0);
			break;
		case WEST:
			can_dig = dig_maze(l, 0, -1);
			break;
		case EAST:
			can_dig = dig_maze(l, 0, 1);
			break;
		default:
			log_debug("Completely unexpected direction (%i)\n", direction);
			can_dig = 0;
			break;
	}

	return can_dig;
}

void generate_maze(struct level *l) {
	int i;
	int blocked;

	shuffle_directions();

	do {
		blocked = 1;

		if (rng_rand_uniform(DIRECTIONS - 1) == 0) {
			/* new direction */
			shuffle_directions();
		}
		
		for (i = 0; i < DIRECTIONS; i++) {
			
			if (dig_maze_in_direction(l, directions[i])) {
				blocked = 0;
				break;
			}

		}

	} while (!blocked);

}

void maze_gen(struct level *l) {
	int done = 0;

	do {
		row = random_odd(MAXROWS);
		col = random_odd(MAXCOLS);

		if (done == 0) {
			set_tile(l, row, col);
		}

		if (get_tile(l, row, col)) {
			generate_maze(l);
			done++;
		}

	} while (done < MAXROWS * MAXCOLS / 4);

	level_clear(l, L_MAZE);
}
