/*
 * Copyright (c) 2018 Tristan Le Guern <tleguern@bouledef.eu>
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

#ifndef PLAYER_H__
#define PLAYER_H__

struct player {
	int x;
	int y;
};

void player_move(struct player *, struct level *, int, int);
void player_move_left(struct player *, struct level *);
void player_move_down(struct player *, struct level *);
void player_move_up(struct player *, struct level *);
void player_move_right(struct player *, struct level *);
void player_move_upleft(struct player *, struct level *);
void player_move_downleft(struct player *, struct level *);
void player_move_upright(struct player *, struct level *);
void player_move_downright(struct player *, struct level *);
void player_init(struct player *, struct level *);

#endif
