#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "display.h"
#include "tetris.h"

u_int8_t map_pixel(u_int16_t tetris_pixel) {
    return tetris_pixel == 0 ? 4 : 7;
}

void get_tetris_pixels(frame_data *fd) {
    int pixel_index = 0;
    u_int16_t *grid_buffer = get_grid_buffer();
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            if (i < (GRID_HEIGHT - MARGIN_WIDTH) && j >= MARGIN_WIDTH && j < GRID_WIDTH - MARGIN_WIDTH) {
                u_int16_t tetris_pixel = grid_buffer[(i * GRID_WIDTH) + j];
                fd->pixels[pixel_index] = map_pixel(tetris_pixel);
                pixel_index++;
            }
        }
    }
}

int tetris() {
    srand(time(NULL));
    reset_game();

    frame_data fd;

    // Allocate frame buffer of "actual" screen size
    fd.pixels = malloc((GRID_HEIGHT - MARGIN_WIDTH) * (GRID_WIDTH - (2 * MARGIN_WIDTH)));

    while (1) {
        for (int i = 0; i < 30; i++) { // "Frames" per drop
            draw();
            get_tetris_pixels(&fd);
            display_update(&fd);
            key_control input = get_key_input();
            int fin;
            switch (input) {
                case up_arrow:
                    rotate_piece(1);
                    break;
                case left_arrow:
                    move_piece_horizontal(-1);
                    break;
                case down_arrow:
                    // TODO: Move downward correcly
                    fin = move_piece_down();
                    if (fin) {
                        add_fixed_piece();
                        clear_rows();
                        setup_next_piece();
                    }
                    break;
                case right_arrow:
                    move_piece_horizontal(1);
            }
            usleep(20000);
        }
        int is_piece_finished = move_piece_down();
        if (is_piece_finished) {
            add_fixed_piece();
            clear_rows();
            setup_next_piece();
        }
    }
}

int main(int argc, char **argv) {
    set_screen_dimensions(10, 20);
    display_init();
    tetris();

    // TODO: Catch interrupts & turn off ncurses mode
    // display_destroy();
}