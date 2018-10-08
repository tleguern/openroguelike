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

#include "level.h"
#include "ui.h"

WINDOW *messagewin;

static void ui_alert(const char *);
static void ui_reset_colors(void);
static int  ui_set_message_window(WINDOW *, int);

enum options {
	O_COLORS,
	O__MAX,
};

struct optionsmap {
	const char	*name;
	bool		 value;
	void		(*func)(void);
};

struct optionsmap optionsmap[] = {
	{"colors", false, ui_reset_colors},
};

void
ui_cleanup(void)
{
	curs_set(1);
	endwin();
}

void
ui_draw(struct level *l)
{
	werase(stdscr);
	/* draw main screen */
	level_draw(l);
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
	if (optionsmap[O_COLORS].value == true)
		ui_reset_colors();
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
		switch (wgetch(menuwin)) {
		case 'k':
			if (hl == 0)
				break;
			hl -= 1;
			break;
		case 'j':
			if (hl == O__MAX - 1)
				break;
			hl += 1;
			break;
		case '\r':
			optionsmap[hl].value = !optionsmap[hl].value;
			optionsmap[hl].func();
			break;
		case 27: /* Escape */
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
	start_color();
	/* Don't bother calling can_change_color() */
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_RED, COLOR_WHITE);
	init_pair(4, COLOR_BLACK, COLOR_CYAN);
	init_pair(5, COLOR_GREEN, COLOR_BLACK);
}

