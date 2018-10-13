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

#include <stdarg.h>
#include <string.h>
#include <curses.h>

#include "creature.h"
#include "level.h"
#include "ui.h"

WINDOW *messagewin;

static void ui_alert(const char *);
static void ui_reset_colors(void);
static void ui_reset_tileset(void);
static int  ui_set_message_window(WINDOW *, int);

static chtype tileset[T__MAX];

struct optionsmap optionsmap[] = {
	{"colors", false, ui_reset_colors},
};

struct keybindingsmap keybindingsmap[] = {
	{"enter",	'\r'},
	{"escape",	27}, /* Escape key*/
	{"left",	'h'},
	{"down",	'j'},
	{"up",		'k'},
	{"right",	'l'},
	{"upleft",	'y'},
	{"upright",	'u'},
	{"downleft",	'b'},
	{"downright",	'n'},
	{"rest",	'.'},
	{"upstair",	'>'},
	{"downstair",	'<'},
	{"show help menu", '?'},
	{"show options menu", 'O'},
};

void
ui_cleanup(void)
{
	curs_set(1);
	endwin();
}

void
ui_tile_print(struct tile *t, int x, int y) {
	mvaddch(y, x, tileset[t->type]);
	if (NULL != t->creature) {
		mvaddch(y, x, t->creature->glyphe);
	}
}

void
ui_level_draw(struct level *l)
{
	int x, y;

	for (y = 0; y < MAXROWS; ++y)
		for (x = 0; x < MAXCOLS; ++x)
			ui_tile_print(&(l->tile[y][x]), x, y);
}

void
ui_draw(struct level *l)
{
	werase(stdscr);
	/* draw main screen */
	ui_level_draw(l);
	wnoutrefresh(stdscr);
	/* draw message screen */
	wnoutrefresh(messagewin);
	doupdate();
}

void
ui_init(void)
{
	ripoffline(1, ui_set_message_window);
	initscr();
	curs_set(0);
	cbreak();
	noecho();
	nonl();
	keypad(stdscr, TRUE);
	intrflush(stdscr, FALSE);
	meta(stdscr, TRUE);
	if (has_colors() == FALSE)
		return;
	ui_reset_colors();
	ui_reset_tileset();
	redrawwin(stdscr);
}

void
ui_menu_options(void)
{
	int hl, exit;
	WINDOW *menuwin;

	hl = 0;
	exit = -1;
	menuwin = newwin(O__MAX + 2, 20, LINES / 2, COLS / 2 - 10);
	wbkgd(menuwin, ' ' | COLOR_PAIR(4));
	box(menuwin, 0, 0);
	ui_message("Options menu");
	mvwaddstr(menuwin, 0, 1, "[OPTIONS]");
	do {
		int key = -1;

		for (int i = 0; i < O__MAX; i++) {
			char cursor, x;

			cursor = ' ';
			x = ' ';
			if (optionsmap[i].value == true)
				x = 'X';
			if (hl == i)
				cursor = '>';
			mvwprintw(menuwin, i + 1, 1, "%c[%c] %s", \
			    cursor, x, optionsmap[i].name);
		}
		wrefresh(menuwin);
		key = ui_keybinding_get(wgetch(menuwin));
		if (key == K__MAX) {
			continue;
		}
		switch (key) {
		case K_UP:
			if (hl == 0)
				break;
			hl -= 1;
			break;
		case K_DOWN:
			if (hl == O__MAX - 1)
				break;
			hl += 1;
			break;
		case K_ENTER:
			optionsmap[hl].value = !optionsmap[hl].value;
			optionsmap[hl].func();
			redrawwin(menuwin); /* XXX: Hum */
			break;
		case K_ESCAPE:
			exit = 1;
			break;
		default:
			break;
		}
		if (exit != -1)
			break;
	} while (1);
	delwin(menuwin);
}

void
ui_menu_help(void)
{
	int exit, nonconfigurablekeys;
	WINDOW *helpwin;

	exit = -1;
	nonconfigurablekeys = 2;
	helpwin = newwin(K__MAX - nonconfigurablekeys + 2, \
	    40, LINES / 2, COLS / 2 - 20);
	wbkgd(helpwin, ' ' | COLOR_PAIR(4));
	box(helpwin, 0, 0);
	ui_message("Help menu");
	mvwaddstr(helpwin, 0, 1, "[HELP]");
	do {
		int key = -1;

		for (int i = 2; i < K__MAX; i++) {
			mvwprintw(helpwin, i - nonconfigurablekeys + 1, 1, "%c %s", \
			    keybindingsmap[i].key, keybindingsmap[i].name);
		}
		mvwprintw(helpwin, K__MAX + 1, 1, "Escape to quit this menu");
		wrefresh(helpwin);
		key = ui_keybinding_get(wgetch(helpwin));
		if (key == K__MAX) {
			continue;
		}
		switch (key) {
		case K_ESCAPE:
			exit = 1;
			break;
		default:
			break;
		}
		if (exit != -1)
			break;
	} while (1);
	delwin(helpwin);
}

void
ui_message(const char *message, ...)
{
	va_list ap;

	va_start(ap, message);
	wmove(messagewin, 0, 0);
	vwprintw(messagewin, message, ap);
	va_end(ap);
	wclrtoeol(messagewin);
	wrefresh(messagewin);
}

/* TODO: Change to var args */
static void
ui_alert(const char *message)
{
	size_t messagez;
	WINDOW *alertwin;

	messagez = strlen(message);
	alertwin = newwin(3, messagez + 2, LINES / 2, COLS / 2 - messagez / 2);
	wbkgd(alertwin, ' ' | COLOR_PAIR(3));
	box(alertwin, 0, 0);
	mvwaddstr(alertwin, 1, 1, message);
	wrefresh(alertwin);
	wgetch(alertwin);
	delwin(alertwin);
}

static int
ui_set_message_window(WINDOW *win, int ncols)
{
	(void)ncols;
	messagewin = win;
	wbkgd(messagewin, ' ' | COLOR_PAIR(4));
	return(0);
}

static void
ui_reset_colors(void)
{
	ui_message("ui_reset_colors");
	start_color();
	/* Don't bother calling can_change_color() */
	if (optionsmap[O_COLORS].value == true) {
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_BLACK, COLOR_WHITE);
		init_pair(3, COLOR_RED, COLOR_WHITE);
		init_pair(4, COLOR_BLACK, COLOR_CYAN);
		init_pair(5, COLOR_GREEN, COLOR_BLACK);
	} else {
		init_pair(1, COLOR_WHITE, COLOR_BLACK);
		init_pair(2, COLOR_WHITE, COLOR_BLACK);
		init_pair(3, COLOR_WHITE, COLOR_BLACK);
		init_pair(4, COLOR_WHITE, COLOR_BLACK);
		init_pair(5, COLOR_WHITE, COLOR_BLACK);
	}
}

static void
ui_reset_tileset(void)
{
	tileset[T_EMPTY] = ' ';
	tileset[T_WALL] = '#';
	tileset[T_UPSTAIR] = '>';
	tileset[T_DOWNSTAIR] = '<';
	/* use box_set */
}

enum keybindings
ui_keybinding_get(int keypress)
{
	int key;

	for (key = 0; key < K__MAX; key++) {
		if (keypress == keybindingsmap[key].key)
			break;
	}
	return(key);
}

