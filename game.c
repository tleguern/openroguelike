#include "config.h"

#include <curses.h>
#include <err.h>
#include <glob.h>
#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "level.h"
#include "ui.h"
#include "creature.h"
#include "rng.h"

static void usage(void);

static const char *filename = ".roguelikerc";

int debug = false;

static void
config_file_read(const char *configfile)
{
	size_t		 linez = 0;
	ssize_t		 linelen = 0;
	char		*line = NULL;
	FILE		*s;

	if (NULL == (s = fopen(configfile, "r"))) {
		return;
	}
	while (-1 != (linelen = getline(&line, &linez, s))) {
		char *linep, *key, *value;
		int found = 0;

		line[linelen - 1] = '\0';
		linep = line;
		if ('#' == linep[0])
			continue;
		while (' ' == linep[0] || '\t' == linep[0]) {
			linep++;
		}
		if (strlen(linep) == 0)
			continue;
		key = strsep(&linep, ":");
		if (NULL == linep)
			errx(EXIT_FAILURE, "bad line -- %s", line);
		value = linep;
		while (' ' == value[0] || '\t' == value[0]) {
			value++;
		}
		for (int i = 0; i < O__MAX; i++) {
			if (strcmp(optionsmap[i].name, key) == 0) {
				if (strcmp(value, "true") == 0) {
					optionsmap[i].value = true;
				} else if (strcmp(value, "false") == 0) {
					optionsmap[i].value = false;
				} else {
					errx(EXIT_FAILURE,
					    "option %s has wrong value -- %s",
					    key, value);
				}
				found = 1;
				break;
			}
		}
		if (found == 1)
			continue;
		for (int i = NONCONFIGURABLEKEYS; i < K__MAX; i++) {
			if (strcmp(keybindingsmap[i].name, key) == 0) {
				if (strlen(value) > 1) {
					errx(EXIT_FAILURE,
					    "key %s has wrong value -- %s",
					    key, value);
				}
				keybindingsmap[i].key = value[0];
			}
		}
	}
	free(line);
	line = NULL;
}

struct creature p;
struct world w;

int
main(int argc, char *argv[])
{
	int		 ch;
	uint32_t	 seed;
	glob_t		 gl;
	char		 path[PATH_MAX];
	char		*configfile = NULL;
	const char	*errstr;
	struct level	*lp;
	struct passwd	*pw;

	while ((ch = getopt(argc, argv, "df:s:")) != -1) {
		switch (ch) {
		case 'd':
			debug = true;
			break;
		case 'f':
			configfile = optarg;
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

	if (NULL == configfile) {
		pw = getpwuid(getuid());
		snprintf(path, sizeof(path), "%s/%s", pw->pw_dir, filename);
	} else {
		glob(configfile, GLOB_NOCHECK | GLOB_TILDE, NULL, &gl);
		snprintf(path, sizeof(path), "%s", gl.gl_pathv[0]);
		globfree(&gl);
	}
	config_file_read(path);

	rng_init();
	ui_init();
	if (debug == true)
		ui_message("Seed: %u", rng_get_seed());
	else
		ui_message("Unwelcome to the cave of the Goblin King");
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
	do {
		int key = -1;
		int noaction = 0;

		ui_draw(lp);
		p.actionpoints += p.speed;
		while (p.actionpoints >= 5) {
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
			p.actionpoints -= 5;
		}
		/* Monsters' turn */
		for (int32_t i = 0; i < w.creaturesz; i++) {
			struct creature *c;

			c = w.creatures[i];
			c->actionpoints += c->speed;
			while (c->actionpoints >= 5) {
				creature_do_something(c, world_first(&w));
				c->actionpoints -= 5;
			}
		}
	} while (1);
	world_free(&w);
	ui_cleanup();
	return(0);
}

static void
usage(void)
{
	fprintf(stderr, "usage: %s [-d] [-f file] [-s seed]\n", getprogname());
	exit(1);
}

