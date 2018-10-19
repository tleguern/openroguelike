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

#ifndef UI_H__
#define UI_H__

struct level;

enum options {
	O_COLORS,
	O_DECGRAPHICS,
	O__MAX,
};

struct optionsmap {
	const char	*name;
	bool		 value;
	void		(*func)(void);
};

#define NONCONFIGURABLEKEYS 2
enum keybindings {
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
	K_REST,
	K_UPSTAIR,
	K_DOWNSTAIR,
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

void ui_cleanup(void);
void ui_draw(struct level *);
void ui_init(void);
void ui_menu_options(void);
void ui_menu_help(void);
void ui_message(const char *, ...);
enum keybindings ui_keybinding_get(int);

#endif

