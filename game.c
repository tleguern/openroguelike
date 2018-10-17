#include "config.h"

#include <curses.h>
#include <err.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "level.h"
#include "ui.h"
#include "creature.h"
#include "rng.h"

static void usage(void);

int debug = false;

struct creature p;
struct creature g;
struct world w;

int
main(int argc, char *argv[])
{
	int		 ch;
	uint32_t	 seed;
	const char	*errstr;
	struct level	*lp;

	while ((ch = getopt(argc, argv, "ds:")) != -1) {
		switch (ch) {
		case 'd':
			debug = true;
			break;
		case 's':
			seed = strtonum(optarg, 0, UINT32_MAX, &errstr);
			if (errstr != NULL) {
				errx(1, "invalid seed value");
			}
			rng_set_seed(seed);
			break;
		default:
			usage();
		}
	}
	argc -= optind;
	argv += optind;

	rng_init();
	ui_init();
	if (debug == true)
		ui_message("Seed: %u", rng_get_seed());
	else
		ui_message("Welcome to the cave of the Goblin King");
	/* Check for 23 because of ripoffline */
	if ((LINES < 23) || (COLS < 80)) {
		ui_cleanup();
		fprintf(stderr, "must be displayed on 80x24 screen.");
		fprintf(stderr, "LINES: %i, COLS: %i\n", LINES, COLS);
		return(1);
	}

	world_init(&w);
	lp = world_first(&w);
	creature_init(&p, lp, R_HUMAN);
	creature_init(&g, lp, R_GOBLIN);
	do {
		int key = -1;
		int noaction = 0;

		ui_draw(lp);
		key = ui_keybinding_get(wgetch(stdscr));
		if (key == K__MAX) {
			continue;
		}
		switch (key) {
		case K_LEFT:
			noaction = creature_move_left(&p, lp);
			break;
		case K_DOWN:
			noaction = creature_move_down(&p, lp);
			break;
		case K_UP:
			noaction = creature_move_up(&p, lp);
			break;
		case K_RIGHT:
			noaction = creature_move_right(&p, lp);
			break;
		case K_UPLEFT:
			noaction = creature_move_upleft(&p, lp);
			break;
		case K_UPRIGHT:
			noaction = creature_move_upright(&p, lp);
			break;
		case K_DOWNLEFT:
			noaction = creature_move_downleft(&p, lp);
			break;
		case K_DOWNRIGHT:
			noaction = creature_move_downright(&p, lp);
			break;
		case K_UPSTAIR:
			noaction = creature_climb_upstair(&p, lp, world_next(&w));
			lp = world_current(&w);
			break;
		case K_DOWNSTAIR:
			noaction = creature_climb_downstair(&p, lp, world_prev(&w));
			lp = world_current(&w);
			break;
		case K_REST:
			noaction = creature_rest(&p);
			break;
		case K_OPTIONMENU:
			ui_menu_options();
			noaction = -1;
			break;
		case K_HELPMENU:
			ui_menu_help();
			noaction = -1;
			break;
		default:
			noaction = -1;
			break;
		}
		if (noaction == -1)
			continue;
		/* Monsters' turn */
		creature_do_something(&g, world_first(&w));
	} while (1);
	world_free(&w);
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-d] [-s seed]\n", getprogname());
	exit(1);
}

