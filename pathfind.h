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

#ifndef PATHFIND_H__
#define PATHFIND_H__

#include <stdbool.h>

struct coordinate;

struct coordqueue {
	size_t			 queuez;
	struct coordinate	*queue;
	int			*counter;
};

int coordqueue_size(struct coordqueue *);
void coordqueue_print(struct coordqueue *);
int coordqueue_exists(struct coordqueue *, int, int, int);
int coordqueue_get_counter_at_coord(struct coordqueue *, int, int);
int coordqueue_get_elem_with_coord(struct coordqueue *, int, int, int);
int coordqueue_grow(struct coordqueue *);
int coordqueue_add(struct coordqueue *, int, int, int);
int coordqueue_init(struct coordqueue *);
void coordqueue_free(struct coordqueue *);
bool are_coordinate_reachable(struct level *, struct coordinate *, struct coordinate *);

#endif
