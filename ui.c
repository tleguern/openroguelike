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
#include "options.h"

WINDOW *messagewin;

static void ui_reset_colors(void);
static void ui_reset_tileset(void);
static int  ui_set_message_window(WINDOW *, int);

static chtype tileset[T__MAX];

void
ui_cleanup(void)
{
	curs_set(1);
	endwin();
}

static void
ui_tile_print(struct tile *t, int x, int y) {
	mvaddch(y, x, tileset[t->type]);
	if (NULL != t->creature) {
		int glyphe;

		switch (t->creature->race) {
		case R_GOBLIN:
			glyphe = tileset[T_GOBLIN];
			break;
		case R_HUMAN:
			glyphe = tileset[T_HUMAN];
			break;
		case R__MAX:
		default:
			/* Visual error meaning I forgot to assign a glyphe */
			glyphe = 'X' | COLOR_PAIR(3);
			break;
		}
		mvaddch(y, x, glyphe);
	}
}

static void
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
		int choice, key;

		choice = -1;
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
		key = keybinding_resolve(wgetch(menuwin));
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
			choice = hl;
			break;
		case K_ESCAPE:
			exit = 1;
			break;
		default:
			break;
		}
		if (exit != -1) {
			break;
		}
		if (choice != -1) {
			optionsmap[choice].value = !optionsmap[choice].value;
			switch (choice) {
			case O_COLORS:
				ui_reset_colors();
				break;
			case O_DECGRAPHICS:
				ui_reset_tileset();
				break;
			case O__MAX:
			default:
				break;
			}
			redrawwin(menuwin); /* XXX: Hum */
		}
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

		for (int i = NONCONFIGURABLEKEYS; i < K__MAX; i++) {
			mvwprintw(helpwin, i - nonconfigurablekeys + 1, 1, "%c %s", \
			    keybindingsmap[i].key, keybindingsmap[i].name);
		}
		mvwprintw(helpwin, K__MAX + 1, 1, "Escape to quit this menu");
		wrefresh(helpwin);
		key = keybinding_resolve(wgetch(helpwin));
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
	vw_printw(messagewin, message, ap);
	va_end(ap);
	wclrtoeol(messagewin);
	wrefresh(messagewin);
}

void
ui_clearmessage(void)
{
	wclear(messagewin);
	wrefresh(messagewin);
}

/* TODO: Change to var args */
void
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
	if (optionsmap[O_DECGRAPHICS].value == true) {
		tileset[T_EMPTY] = ' ';
		tileset[T_WALL] = '#';
		tileset[T_UPSTAIR] = '>';
		tileset[T_DOWNSTAIR] = '<';
		tileset[T_HLINE] = ACS_HLINE;
		tileset[T_VLINE] = ACS_VLINE;
		tileset[T_BTEE] = ACS_BTEE;
		tileset[T_TTEE] = ACS_TTEE;
		tileset[T_LTEE] = ACS_LTEE;
		tileset[T_RTEE] = ACS_RTEE;
		tileset[T_CROSS] = ACS_PLUS;
		tileset[T_LLCORNER] = ACS_LLCORNER;
		tileset[T_LRCORNER] = ACS_LRCORNER;
		tileset[T_ULCORNER] = ACS_ULCORNER;
		tileset[T_URCORNER] = ACS_URCORNER;
	} else {
		tileset[T_EMPTY] = ' ';
		tileset[T_WALL] = '#';
		tileset[T_UPSTAIR] = '>';
		tileset[T_DOWNSTAIR] = '<';
		tileset[T_HLINE] = '#';
		tileset[T_VLINE] = '#';
		tileset[T_BTEE] = '#';
		tileset[T_TTEE] = '#';
		tileset[T_LTEE] = '#';
		tileset[T_RTEE] = '#';
		tileset[T_CROSS] = '#';
		tileset[T_LLCORNER] = '#';
		tileset[T_LRCORNER] = '#';
		tileset[T_ULCORNER] = '#';
		tileset[T_URCORNER] = '#';
	}
	tileset[T_GOBLIN] = 'g' | COLOR_PAIR(5);
	tileset[T_HUMAN] = '@' | COLOR_PAIR(2);
	/* use box_set */
}

void
ui_look(struct level *l, int y, int x)
{
	WINDOW		*lookwin;
	const char	*message;
	size_t		 messagez;

	switch (l->tile[y][x].type) {
	case T_WALL: message = "a solid wall made of hard rocks"; break;
	case T_EMPTY: message = "dirt"; break;
	case T_UPSTAIR: message = "a flight of stairs going up"; break;
	case T_DOWNSTAIR: message = "a flight of stairs going down"; break;
	default: message = "not sure, a monstrosity perhaps"; break;
	}
	messagez = strlen(message);
	lookwin = newwin(3, messagez + 2, LINES / 2, COLS / 2 - messagez / 2);
	wbkgd(lookwin, ' ' | COLOR_PAIR(3));
	box(lookwin, 0, 0);
	mvwaddstr(lookwin, 1, 1, message);
	wrefresh(lookwin);
	wgetch(lookwin);
	delwin(lookwin);
}

void
ui_look_elsewhere(struct level *l, int current_y, int current_x)
{
	int y, x, exit;

	exit = -1;
	y = current_y;
	x = current_x;
	wmove(stdscr, y, x);
	curs_set(2);
	do {
		int key;

		key = keybinding_resolve(wgetch(stdscr));
		if (key == K__MAX) {
			continue;
		}
		switch (key) {
		case K_LEFT:
			x -= 1;
			break;
		case K_DOWN:
			y += 1;
			break;
		case K_UP:
			y -= 1;
			break;
		case K_RIGHT:
			x += 1;
			break;
		case K_UPLEFT:
			y -= 1;
			x -= 1;
			break;
		case K_UPRIGHT:
			y -= 1;
			x += 1;
			break;
		case K_DOWNLEFT:
			y += 1;
			x -= 1;
			break;
		case K_DOWNRIGHT:
			y += 1;
			x += 1;
			break;
		case K_ENTER:
			exit = 1;
			break;
		default:
			break;
		}
		if (exit != -1) {
			break;
		}
		if (0 > y) {
			y = 0;
		}
		if (0 > x) {
			x = 0;
		}
		if (MAXROWS <= y) {
			y = MAXROWS - 1;
		}
		if (MAXCOLS <= x) {
			x = MAXCOLS - 1;
		}
		wmove(stdscr, y, x);
		wrefresh(stdscr);
	} while (1);
	curs_set(0);
	ui_look(l, y, x);
}

int
ui_get_input(void)
{
	return(wgetch(stdscr));
}

int
ui_get_lines(void)
{
	return(LINES);
}

int
ui_get_cols(void)
{
	return(COLS);
}

