/*
 * Public domain - 2018 Tristan Le Guern <tleguern@bouledef.eu>
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "rng.h"

static uint32_t rng_storage[4096];
static uint32_t rng_counter = 4095;
static uint32_t rng_seed = 0;

/*
 * This is interesting in cases such as save file or tests.
 */
void
rng_set_seed(uint32_t seed)
{
	rng_seed = seed;
}

uint32_t
rng_get_seed(void)
{
	return(rng_seed);
}

/*
 * Initialize the array from the seed using an Linear Congruential Generator.
 *
 * Numbers from https://en.wikipedia.org/wiki/Linear_congruential_generator
 * Section Parameters_in_common_use
 * Row Numerical Recipes
 */
void
rng_init(void)
{
	uint32_t i, seed;

	if (rng_seed == 0)
		rng_seed = arc4random();
	seed = rng_seed;
	for (i = 0; i < (sizeof(rng_storage) / sizeof(uint32_t)); i++) {
		rng_storage[i] = seed;
		seed = seed * 1664525 + 1013904223;
	}
}

/*
 * Simple RNG from Marsaglia RNGs 2003 post.
 */
uint32_t
rng_rand(void)
{
	uint64_t t;
	uint32_t x;

	rng_counter = (rng_counter + 1) & 4095;
	t = 18782LL * rng_storage[rng_counter] + rng_seed;
	rng_seed = (t >> 32);
	x = (uint32_t)(t + rng_seed);
	if (x < rng_seed) {
		x += 1;
		rng_seed += 1;
	}
	rng_storage[rng_counter] = 0xfffffffe - x;
	return(rng_storage[rng_counter]);
}

/*
 * Taken from arc4random_uniform
 * Copyright (c) 2008, Damien Miller <djm@openbsd.org>
 * ISC license
 */
uint32_t
rng_rand_uniform(uint32_t bound)
{
	uint32_t r, min;

	if (bound < 2)
		return(0);
	min = -bound % bound;
	for (;;) {
		r = rng_rand();
		if (r >= min)
			break;
	}
	return (r % bound);
}

