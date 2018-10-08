#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <curses.h>

#include "level.h"
#include "player.h"
#include "ui.h"

struct player p;
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
	player_init(&p, &l);
	do {
		ui_draw(&l);
		c = wgetch(stdscr);
		switch (c) {
		case 'h':
			player_move_left(&p, &l);
			break;
		case 'j':
			player_move_down(&p, &l);
			break;
		case 'k':
			player_move_up(&p, &l);
			break;
		case 'l':
			player_move_right(&p, &l);
			break;
		case 'y':
			player_move_upleft(&p, &l);
			break;
		case 'u':
			player_move_upright(&p, &l);
			break;
		case 'b':
			player_move_downleft(&p, &l);
			break;
		case 'n':
			player_move_downright(&p, &l);
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

