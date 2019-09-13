/*
 * Copyright (c) 2019 Tristan Le Guern <tleguern@bouledef.eu>
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

#include <curses.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "level.h"
#include "rng.h"
#include "ui.h"

static void usage(void);

int
main(int argc, char *argv[])
{
	int ch;
	int seed;
	struct level l;
	const char	*errstr;

	while ((ch = getopt(argc, argv, "")) != -1) {
		switch (ch) {
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;
	if (argc != 1) {
		warnx("seed expected");
		usage();
	}
	seed = strtonum(argv[0], 0, UINT32_MAX, &errstr);
	if (errstr != NULL) {
		errx(1, "invalid seed value");
	}
	rng_set_seed(seed);

	rng_init();
	ui_init();
	level_init(&l);
	cave_gen(&l);
	level_load(&l, "misc/entry");

	ui_draw(&l);
	(void)ui_get_input();
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s seed\n", getprogname());
	exit(1);
}

