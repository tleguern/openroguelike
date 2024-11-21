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

#include "options.h"

struct optionsmap optionsmap[] = {
	{"colors", false},
};

struct keybindingsmap keybindingsmap[] = {
	{"enter",		'\r'},
	{"escape",		27}, /* Escape key*/
	{"left",		'h'},
	{"down",		'j'},
	{"up",			'k'},
	{"right",		'l'},
	{"upleft",		'y'},
	{"upright",		'u'},
	{"downleft",		'b'},
	{"downright",		'n'},
	{"run left",		'H'},
	{"run down",		'J'},
	{"run up",		'K'},
	{"run right",		'L'},
	{"run upleft",		'Y'},
	{"run upright",		'U'},
	{"run downleft",	'B'},
	{"run downright",	'N'},
	{"rest",		'.'},
	{"upstair",		'<'},
	{"downstair",		'>'},
	{"look here",   	':'},
	{"look elsewhere",	';'},
	{"show help menu",	'?'},
	{"show options menu",	'O'},
	{"quit",		'Q'},
};

enum keybinding
keybinding_resolve(int keypress)
{
	int key;

	for (key = 0; key < K__MAX; key++) {
		if (keypress == keybindingsmap[key].key)
			break;
	}
	return(key);
}

