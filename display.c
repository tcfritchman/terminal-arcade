#include "display.h"
#include <ncurses.h>
#include <stdio.h>
#include <errno.h>

#define TEXT_COLOR_PAIR 8 // After solid colors 0-7

u_int16_t screen_width;
u_int16_t screen_height;
u_int16_t no_pixels;
WINDOW *win;

void set_screen_dimensions(u_int16_t w, u_int16_t h) {
	screen_height = h;
	screen_width = w;
    no_pixels = h * w;
}

void display_init() {
	// ncurses initialization
    initscr();              /* Start curses mode */
    //raw();                  /* Line buffering disabled */
	cbreak();
    keypad(stdscr, TRUE);   /* We get F1, F2 etc.. */
    noecho();               /* Don't echo() while we do getch */
	curs_set(0);            /* Make the cursor invisible */
	start_color();          /* Turn on colors */
	timeout(0);				/* Sets non-blocking input mode */

	// Initialize color pairs for solid blocks
	init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
	init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);
	init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLUE);
	init_pair(COLOR_RED, COLOR_RED, COLOR_RED);
	init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_GREEN);
	init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_CYAN);
	init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_MAGENTA);
	init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_YELLOW);

	// Color pair for text
	init_pair(TEXT_COLOR_PAIR, COLOR_WHITE, COLOR_BLACK);

	refresh();

	// We use 2X screen width to initialize the screen dimensions
	// since each "pixel" is two block characters wide
	win = newwin(screen_height, screen_width * 2, 0, 0);
}

void display_destroy() {
	endwin(); /* End curses mode */
}

void display_update(frame_data *f) {
	/* Draw  pixels onto the window */
	for (int i = 0; i < no_pixels / sizeof(u_int8_t); i++) {
		int col = i % screen_width;
		int row = i / screen_width;
		wattron(win, COLOR_PAIR(f->pixels[i]));
		wmove(win, row, col * 2);
		waddch(win, ACS_CKBOARD);
		wmove(win, row, col * 2 + 1);
		waddch(win, ACS_CKBOARD);
		wattroff(win, COLOR_PAIR(f->pixels[i]));
	}

	/* Display the updates */
	wrefresh(win);
}

key_control get_key_input() {
	// TODO: Read into buffer
	int ch = getch();
	switch (ch) {
		case KEY_LEFT:
			return left_arrow;
		case KEY_RIGHT:
			return right_arrow;
		case KEY_UP:
			return up_arrow;
		case KEY_DOWN:
			return down_arrow;
		default:
			return none;
	}
}