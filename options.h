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

#ifndef OPTIONS_H__
#define OPTIONS_H__

enum option {
	O_COLORS,
	O__MAX,
};

struct optionsmap {
	const char	*name;
	bool		 value;
};

#define NONCONFIGURABLEKEYS 2
enum keybinding {
	K_ENTER,
	K_ESCAPE,
	K_LEFT,
	K_DOWN,
	K_UP,
	K_RIGHT,
	K_UPLEFT,
	K_UPRIGHT,
	K_DOWNLEFT,
	K_DOWNRIGHT,
	K_RUNLEFT,
	K_RUNDOWN,
	K_RUNUP,
	K_RUNRIGHT,
	K_RUNUPLEFT,
	K_RUNUPRIGHT,
	K_RUNDOWNLEFT,
	K_RUNDOWNRIGHT,
	K_REST,
	K_UPSTAIR,
	K_DOWNSTAIR,
	K_LOOKHERE,
	K_LOOKELSEWHERE,
	K_HELPMENU,
	K_OPTIONMENU,
	K__MAX,
};

struct keybindingsmap {
	const char	*name;
	char		 key;
};

extern struct optionsmap optionsmap[];
extern struct keybindingsmap keybindingsmap[];

enum keybinding keybinding_resolve(int);

#endif
