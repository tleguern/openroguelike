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

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "creature.h"
#include "level.h"
#include "ui.h"
#include "rng.h"
#include "world.h"

void
world_init(struct world *w)
{
	w->current = 0;
	w->levelsz = 5;
	w->creaturesz = 3;
	w->levels = calloc(w->levelsz, sizeof(struct level *));
	/* The first level is the fixed entrance */
	log_debug("Generate the first level\n");
	w->levels[0] = calloc(1, sizeof(struct level));
	do {
		level_init(w->levels[0]);
		cave_gen(w->levels[0]);
		level_load(w->levels[0], "misc/entry");
	} while (-1 == level_add_stairs(w->levels[0], false, true));
	w->levels[0]->entrymessage = strdup("You enter the Goblin's Caves");
	/* Generate three random caves */
	log_debug("Generate three random caves\n");
	for (int32_t i = 1; i < w->levelsz - 1; i++) {
		w->levels[i] = calloc(1, sizeof(struct level));
		level_init(w->levels[i]);
		cave_gen(w->levels[i]);
		level_add_stairs(w->levels[i], true, true);
	}
	/* The final level is the fixed hall room of Goblin King */
	log_debug("Generate the Goblin King's room\n");
	w->levels[w->levelsz - 1] = calloc(1, sizeof(struct level));
	level_init(w->levels[w->levelsz - 1]);
	cave_gen(w->levels[w->levelsz - 1]);
	w->levels[w->levelsz - 1]->entrymessage =
	    strdup("Unwelcome to the Hall of the Goblin King");
	level_load(w->levels[w->levelsz - 1], "misc/hall");
	level_add_stairs(w->levels[w->levelsz - 1], true, false);

	log_debug("--- creature (goblins) ---\n");
	w->creatures = calloc(w->creaturesz, sizeof(struct creature *));
	for (int32_t i = 0; i < w->creaturesz; i++) {
		w->creatures[i] = calloc(1, sizeof(struct creature));
		creature_init(w->creatures[i], R_GOBLIN);
		creature_place_randomly(w->creatures[i], w->levels[0]);
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
		free(w->levels[i]->entrymessage);
		free(w->levels[i]);
		w->levels[i] = NULL;
	}
	free(w->levels);
	w->levels = NULL;
	w->levelsz = 0;
	w->current = -1;
}

