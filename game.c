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

struct creature p;
struct creature g;
struct world w;

int
main(int argc, char *argv[])
{
	int		 c, ch;
	uint32_t	 seed;
	const char	*errstr;
	struct level	*lp;

	while ((ch = getopt(argc, argv, "s:")) != -1) {
		switch (ch) {
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
	ui_message("Welcome to the cave of the Goblin King");
	/* Check for 23 because of ripoffline */
	if ((LINES < 23) || (COLS < 80)) {
		ui_cleanup();
		fprintf(stderr, "must be displayed on 80x24 screen.");
		fprintf(stderr, "LINES: %i, COLS: %i\n", LINES, COLS);
		return(1);
	}

	c = -1;
	world_init(&w);
	lp = world_first(&w);
	creature_init(&p, lp, R_HUMAN);
	creature_init(&g, lp, R_GOBLIN);
	do {
		ui_draw(lp);
		c = wgetch(stdscr);
		switch (c) {
		case 'h':
			creature_move_left(&p, lp);
			break;
		case 'j':
			creature_move_down(&p, lp);
			break;
		case 'k':
			creature_move_up(&p, lp);
			break;
		case 'l':
			creature_move_right(&p, lp);
			break;
		case 'y':
			creature_move_upleft(&p, lp);
			break;
		case 'u':
			creature_move_upright(&p, lp);
			break;
		case 'b':
			creature_move_downleft(&p, lp);
			break;
		case 'n':
			creature_move_downright(&p, lp);
			break;
		case '>':
			if (lp->tile[p.y][p.x].type == T_UPSTAIR) {
				lp->tile[p.y][p.x].creature = NULL;
				lp = world_next(&w);
				creature_place_at_stair(&p, lp, false);
			}
			break;
		case '<':
			if (lp->tile[p.y][p.x].type == T_DOWNSTAIR) {
				lp->tile[p.y][p.x].creature = NULL;
				lp = world_prev(&w);
				creature_place_at_stair(&p, lp, true);
			}
			break;
		case 'O':
			ui_menu_options();
			break;
		default:
			break;
		}
	} while (1);
	world_free(&w);
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-s seed]\n", getprogname());
	exit(1);
}

