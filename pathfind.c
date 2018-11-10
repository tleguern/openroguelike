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

#include "config.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "level.h"
#include "pathfind.h"

int
coordqueue_size(struct coordqueue *cq)
{
	return(cq->queuez);
}

int
coordqueue_exists(struct coordqueue *cq, int y, int x, int counter)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			continue;
		}
		if (y == cq->queue[i].y && x == cq->queue[i].x) {
			if (cq->counter[i] <= counter) {
				return(0);
			} else {
				return(-1);
			}
		}
	}
	return(-1);
}

int
coordqueue_get_counter_at_coord(struct coordqueue *cq, int y, int x)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			continue;
		}
		if (y == cq->queue[i].y && x == cq->queue[i].x) {
			return(cq->counter[i]);
		}
	}
	return(-1);
}

int
coordqueue_get_elem_with_coord(struct coordqueue *cq, int y, int x, int counter)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			continue;
		}
		if (y == cq->queue[i].y && x == cq->queue[i].x
		    && counter == cq->counter[i]) {
			return(i);
		}
	}
	return(-1);
}

int
coordqueue_grow(struct coordqueue *cq)
{
	struct coordinate	*tempq;
	int			*tempcounter;
	size_t			 queuez;

	queuez = cq->queuez + 50;
	tempq = reallocarray(cq->queue, queuez, sizeof(struct coordinate));
	tempcounter = reallocarray(cq->counter, queuez, sizeof(int));
	if (NULL == tempq) {
		free(cq->queue);
		free(cq->counter);
		cq->queue = NULL;
		cq->counter = NULL;
		cq->queuez = -1;
		return(-1);
	}
	cq->queue = tempq;
	cq->counter = tempcounter;
	for (size_t i = cq->queuez; i < queuez; i++) {
		cq->queue[i].y = -1;
		cq->queue[i].x = -1;
		cq->counter[i] = -1;
	}
	cq->queuez = queuez;
	return(0);
}

int
coordqueue_add(struct coordqueue *cq, int y, int x, int counter)
{
	for (size_t i = 0; i < cq->queuez; i++) {
		if (-1 == cq->queue[i].y) {
			cq->queue[i].y = y;
			cq->queue[i].x = x;
			cq->counter[i] = counter;
			return(0);
		}
	}
	coordqueue_grow(cq);
	coordqueue_add(cq, y, x, counter);
	return(-1);
}

int
coordqueue_init(struct coordqueue *cq)
{
	cq->queuez = 50;
	cq->queue = reallocarray(NULL, cq->queuez, sizeof(struct coordinate));
	cq->counter = reallocarray(NULL, cq->queuez, sizeof(int));
	if (NULL == cq->queue) {
		return(-1);
	}
	for (size_t i = 0; i < cq->queuez; i++) {
		cq->queue[i].y = -1;
		cq->queue[i].x = -1;
		cq->counter[i] = -1;
	}
	return(0);
}

void
coordqueue_free(struct coordqueue *cq)
{
	free(cq->queue);
	free(cq->counter);
	cq->queue = NULL;
	cq->counter = NULL;
	cq->queuez = -1;
}

bool
are_coordinate_reachable(struct level *l, struct coordinate *start, struct coordinate *end)
{
	struct coordqueue cq;
	int found;

	/*
	 * Starting from (start->y, start->x) explore every adjacent cell and
	 * count the number of steps required to reach it.
	 * Do not insert a new position if the tested cell is out of
	 * the screen, is a wall or other unreachable type, or if a shorter path
	 * a already been found.
	 */
	coordqueue_init(&cq);
	coordqueue_add(&cq, start->y, start->x, 0);

	for (int elem = 0; elem < coordqueue_size(&cq); elem++) {
		int y, x, counter;

		y = cq.queue[elem].y;
		x = cq.queue[elem].x;
		if (-1 == y || -1 == x) {
			continue;
		}
		counter = cq.counter[elem];
		counter += 1;
		if (0 != y
		    && tile_is_empty(&(l->tile[y - 1][x]))
		    && -1 == coordqueue_exists(&cq, y - 1, x, counter)) {
			coordqueue_add(&cq, y - 1, x, counter);
		}
		if (0 != y && MAXCOLS != x
		    && tile_is_empty(&(l->tile[y - 1][x + 1]))
		    && -1 == coordqueue_exists(&cq, y - 1, x + 1, counter)) {
			coordqueue_add(&cq, y - 1, x + 1, counter);
		}
		if (MAXCOLS != x
		    && tile_is_empty(&(l->tile[y][x + 1]))
		    && -1 == coordqueue_exists(&cq, y, x + 1, counter)) {
			coordqueue_add(&cq, y, x + 1, counter);
		}
		if (MAXROWS != y && MAXCOLS != x
		    && tile_is_empty(&(l->tile[y + 1][x + 1]))
		    && -1 == coordqueue_exists(&cq, y + 1, x + 1, counter)) {
			coordqueue_add(&cq, y + 1, x + 1, counter);
		}
		if (MAXROWS != y
		    && tile_is_empty(&(l->tile[y + 1][x]))
		    && -1 == coordqueue_exists(&cq, y + 1, x, counter)) {
			coordqueue_add(&cq, y + 1, x, counter);
		}
		if (MAXROWS != y && 0 != x
		    && tile_is_empty(&(l->tile[y + 1][x - 1]))
		    && -1 == coordqueue_exists(&cq, y + 1, x - 1, counter)) {
			coordqueue_add(&cq, y + 1, x - 1, counter);
		}
		if (0 != x
		    && tile_is_empty(&(l->tile[y][x - 1]))
		    && -1 == coordqueue_exists(&cq, y, x - 1, counter)) {
			coordqueue_add(&cq, y, x - 1, counter);
		}
		if (0 != y && 0 != x
		    && tile_is_empty(&(l->tile[y - 1][x - 1]))
		    && -1 == coordqueue_exists(&cq, y - 1, x - 1, counter)) {
			coordqueue_add(&cq, y - 1, x - 1, counter);
		}
	}
	/* Find the target destination in the coordqueue */
	found = -1;
	for (int elem = cq.queuez; elem >= 0; elem--) {
		int y, x;

		y = cq.queue[elem].y;
		x = cq.queue[elem].x;
		if (-1 == cq.queue[elem].y) {
			continue;
		}
		if (y == end->y && x == end->x) {
			found = elem;
			break;
		}
	}
	coordqueue_free(&cq);
	if (-1 == found) {
		return(false);
	}
	return(true);
}

