#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <curses.h>

#include "level.h"
#include "ui.h"
#include "creature.h"

struct creature p;
struct creature g;
struct level l;

int
main(void)
{
	int c;

	ui_init();
	ui_message("Welcome to the cave of the Goblin King");
	/* Check for 23 because of ripoffline */
	if ((LINES < 23) || (COLS < 80)) {
		ui_cleanup();
		fprintf(stderr, "must be displayed on 80x24 screen.");
		fprintf(stderr, "LINES: %i, COLS: %i\n", LINES, COLS);
		return(1);
	}

	c = -1;
	level_init(&l);
	cave_gen(&l);
	creature_init(&p, &l, R_HUMAN);
	creature_init(&g, &l, R_GOBLIN);
	do {
		ui_draw(&l);
		c = wgetch(stdscr);
		switch (c) {
		case 'h':
			creature_move_left(&p, &l);
			break;
		case 'j':
			creature_move_down(&p, &l);
			break;
		case 'k':
			creature_move_up(&p, &l);
			break;
		case 'l':
			creature_move_right(&p, &l);
			break;
		case 'y':
			creature_move_upleft(&p, &l);
			break;
		case 'u':
			creature_move_upright(&p, &l);
			break;
		case 'b':
			creature_move_downleft(&p, &l);
			break;
		case 'n':
			creature_move_downright(&p, &l);
			break;
		case 'O':
			ui_menu_options();
			break;
		default:
			break;
		}
	} while (1);
	ui_cleanup();
	return(0);
}

