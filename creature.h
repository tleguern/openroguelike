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

#ifndef CREATURE_H__
#define CREATURE_H__

struct level;

enum race {
	R_HUMAN,
	R_GOBLIN,
	R__MAX,
};

struct creature {
	int x;
	int y;
	int glyphe;
	enum race race;
};

int creature_move(struct creature *, struct level *, int, int);
int creature_move_left(struct creature *, struct level *);
int creature_move_down(struct creature *, struct level *);
int creature_move_up(struct creature *, struct level *);
int creature_move_right(struct creature *, struct level *);
int creature_move_upleft(struct creature *, struct level *);
int creature_move_downleft(struct creature *, struct level *);
int creature_move_upright(struct creature *, struct level *);
int creature_move_downright(struct creature *, struct level *);
int creature_climb_upstair(struct creature *, struct level *, struct level *);
int creature_climb_downstair(struct creature *, struct level *, struct level *);
int creature_rest(struct creature *);
void creature_init(struct creature *, struct level *, enum race);
void creature_place_randomly(struct creature *, struct level *);
void creature_place_at_stair(struct creature *, struct level *, bool);
void creature_do_something(struct creature *, struct level *);

#endif

