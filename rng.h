/*
 * Public domain - 2018 Tristan Le Guern <tleguern@bouledef.eu>
 */

#include <stdint.h>

#ifndef RNG_H__
#define RNG_H__

void rng_set_seed(uint32_t);
uint32_t rng_get_seed(void);
void rng_init(void);
uint32_t rng_rand(void);
uint32_t rng_rand_uniform(uint32_t);

#endif
